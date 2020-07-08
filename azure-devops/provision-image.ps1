# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

<#
.SYNOPSIS
Sets up a machine to be an image for a scale set.

.DESCRIPTION
provision-image.ps1 runs on an existing, freshly provisioned virtual machine,
and sets up that virtual machine as a build machine. After this is done,
(outside of this script), we take that machine and make it an image to be copied
for setting up new VMs in the scale set.

This script must either be run as admin, or one must pass AdminUserPassword;
if the script is run with AdminUserPassword, it runs itself again as an
administrator.

.PARAMETER AdminUserPassword
The administrator user's password; if this is $null, or not passed, then the
script assumes it's running on an administrator account.
#>
param(
  [string]$AdminUserPassword = $null
)

$ErrorActionPreference = 'Stop'

<#
.SYNOPSIS
Gets a random file path in the temp directory.

.DESCRIPTION
Get-TempFilePath takes an extension, and returns a path with a random
filename component in the temporary directory with that extension.

.PARAMETER Extension
The extension to use for the path.
#>
Function Get-TempFilePath {
  Param(
    [String]$Extension
  )

  if ([String]::IsNullOrWhiteSpace($Extension)) {
    throw 'Missing Extension'
  }

  $tempPath = [System.IO.Path]::GetTempPath()
  $tempName = [System.IO.Path]::GetRandomFileName() + '.' + $Extension
  return Join-Path $tempPath $tempName
}

if (-not [string]::IsNullOrEmpty($AdminUserPassword)) {
  Write-Host "AdminUser password supplied; switching to AdminUser"
  $PsExecPath = Get-TempFilePath -Extension 'exe'
  Write-Host "Downloading psexec to $PsExecPath"
  & curl.exe -L -o $PsExecPath -s -S https://live.sysinternals.com/PsExec64.exe
  $PsExecArgs = @(
    '-u',
    'AdminUser',
    '-p',
    $AdminUserPassword,
    '-accepteula',
    '-h',
    'C:\Windows\System32\WindowsPowerShell\v1.0\powershell.exe',
    '-ExecutionPolicy',
    'Unrestricted',
    '-File',
    $PSCommandPath
  )

  Write-Host "Executing $PsExecPath " + @PsExecArgs

  $proc = Start-Process -FilePath $PsExecPath -ArgumentList $PsExecArgs -Wait -PassThru
  Write-Host 'Cleaning up...'
  Remove-Item $PsExecPath
  exit $proc.ExitCode
}

$Workloads = @(
  'Microsoft.VisualStudio.Component.VC.CLI.Support',
  'Microsoft.VisualStudio.Component.VC.CoreIde',
  'Microsoft.VisualStudio.Component.VC.Tools.x86.x64',
  'Microsoft.VisualStudio.Component.VC.Tools.ARM64',
  'Microsoft.VisualStudio.Component.VC.Tools.ARM',
  'Microsoft.VisualStudio.Component.Windows10SDK.18362'
)

$ReleaseInPath = 'Preview'
$Sku = 'Enterprise'
$VisualStudioBootstrapperUrl = 'https://aka.ms/vs/16/pre/vs_enterprise.exe'
$CMakeUrl = 'https://github.com/Kitware/CMake/releases/download/v3.16.5/cmake-3.16.5-win64-x64.msi'
$LlvmUrl = 'https://github.com/llvm/llvm-project/releases/download/llvmorg-10.0.0/LLVM-10.0.0-win64.exe'
$NinjaUrl = 'https://github.com/ninja-build/ninja/releases/download/v1.10.0/ninja-win.zip'
$PythonUrl = 'https://www.python.org/ftp/python/3.8.2/python-3.8.2-amd64.exe'

$CudaUrl = 'https://developer.download.nvidia.com/compute/cuda/10.1/Prod/local_installers/cuda_10.1.243_426.00_win10.exe'
$CudaFeatures = 'nvcc_10.1 cuobjdump_10.1 nvprune_10.1 cupti_10.1 gpu_library_advisor_10.1 memcheck_10.1 ' + `
  'nvdisasm_10.1 nvprof_10.1 visual_profiler_10.1 visual_studio_integration_10.1 cublas_10.1 cublas_dev_10.1 ' + `
  'cudart_10.1 cufft_10.1 cufft_dev_10.1 curand_10.1 curand_dev_10.1 cusolver_10.1 cusolver_dev_10.1 cusparse_10.1 ' + `
  'cusparse_dev_10.1 nvgraph_10.1 nvgraph_dev_10.1 npp_10.1 npp_dev_10.1 nvrtc_10.1 nvrtc_dev_10.1 nvml_dev_10.1 ' + `
  'occupancy_calculator_10.1 fortran_examples_10.1'

$ErrorActionPreference = 'Stop'
$ProgressPreference = 'SilentlyContinue'

<#
.SYNOPSIS
Writes a message to the screen depending on ExitCode.

.DESCRIPTION
Since msiexec can return either 0 or 3010 successfully, in both cases
we write that installation succeeded, and which exit code it exited with.
If msiexec returns anything else, we write an error.

.PARAMETER ExitCode
The exit code that msiexec returned.
#>
Function PrintMsiExitCodeMessage {
  Param(
    $ExitCode
  )

  # 3010 is probably ERROR_SUCCESS_REBOOT_REQUIRED
  if ($ExitCode -eq 0 -or $ExitCode -eq 3010) {
    Write-Host "Installation successful! Exited with $ExitCode."
  }
  else {
    Write-Error "Installation failed! Exited with $ExitCode."
  }
}

<#
.SYNOPSIS
Install Visual Studio.

.DESCRIPTION
InstallVisualStudio takes the $Workloads array, and installs it with the
installer that's pointed at by $BootstrapperUrl.

.PARAMETER Workloads
The set of VS workloads to install.

.PARAMETER BootstrapperUrl
The URL of the Visual Studio installer, i.e. one of vs_*.exe.

.PARAMETER InstallPath
The path to install Visual Studio at.

.PARAMETER Nickname
The nickname to give the installation.
#>
Function InstallVisualStudio {
  Param(
    [String[]]$Workloads,
    [String]$BootstrapperUrl,
    [String]$InstallPath = $null,
    [String]$Nickname = $null
  )

  try {
    Write-Host 'Downloading Visual Studio...'
    [string]$bootstrapperExe = Get-TempFilePath -Extension 'exe'
    curl.exe -L -o $bootstrapperExe -s -S $BootstrapperUrl
    Write-Host "Installing Visual Studio..."
    $args = @('/c', $bootstrapperExe, '--quiet', '--norestart', '--wait', '--nocache')
    foreach ($workload in $Workloads) {
      $args += '--add'
      $args += $workload
    }

    if (-not ([String]::IsNullOrWhiteSpace($InstallPath))) {
      $args += '--installpath'
      $args += $InstallPath
    }

    if (-not ([String]::IsNullOrWhiteSpace($Nickname))) {
      $args += '--nickname'
      $args += $Nickname
    }

    $proc = Start-Process -FilePath cmd.exe -ArgumentList $args -Wait -PassThru
    PrintMsiExitCodeMessage $proc.ExitCode
  }
  catch {
    Write-Error "Failed to install Visual Studio! $($_.Exception.Message)"
  }
}

<#
.SYNOPSIS
Install an .msi file.

.DESCRIPTION
InstallMSI takes a URL where an .msi lives, and installs that .msi to the system.

.PARAMETER Name
The name of the thing to install.

.PARAMETER Url
The URL at which the .msi lives.
#>
Function InstallMSI {
  Param(
    [String]$Name,
    [String]$Url
  )

  try {
    Write-Host "Downloading $Name..."
    [string]$msiPath = Get-TempFilePath -Extension 'msi'
    curl.exe -L -o $msiPath -s -S $Url
    Write-Host "Installing $Name..."
    $args = @('/i', $msiPath, '/norestart', '/quiet', '/qn')
    $proc = Start-Process -FilePath 'msiexec.exe' -ArgumentList $args -Wait -PassThru
    PrintMsiExitCodeMessage $proc.ExitCode
  }
  catch {
    Write-Error "Failed to install $Name! $($_.Exception.Message)"
  }
}

<#
.SYNOPSIS
Unpacks a zip file to $Dir.

.DESCRIPTION
InstallZip takes a URL of a zip file, and unpacks the zip file to the directory
$Dir.

.PARAMETER Name
The name of the tool being installed.

.PARAMETER Url
The URL of the zip file to unpack.

.PARAMETER Dir
The directory to unpack the zip file to.
#>
Function InstallZip {
  Param(
    [String]$Name,
    [String]$Url,
    [String]$Dir
  )

  try {
    Write-Host "Downloading $Name..."
    [string]$zipPath = Get-TempFilePath -Extension 'zip'
    curl.exe -L -o $zipPath -s -S $Url
    Write-Host "Installing $Name..."
    Expand-Archive -Path $zipPath -DestinationPath $Dir -Force
  }
  catch {
    Write-Error "Failed to install $Name! $($_.Exception.Message)"
  }
}

<#
.SYNOPSIS
Installs LLVM.

.DESCRIPTION
InstallLLVM installs LLVM from the supplied URL.

.PARAMETER Url
The URL of the LLVM installer.
#>
Function InstallLLVM {
  Param(
    [String]$Url
  )

  try {
    Write-Host 'Downloading LLVM...'
    [string]$installerPath = Get-TempFilePath -Extension 'exe'
    curl.exe -L -o $installerPath -s -S $Url
    Write-Host 'Installing LLVM...'
    $proc = Start-Process -FilePath $installerPath -ArgumentList @('/S') -NoNewWindow -Wait -PassThru
    PrintMsiExitCodeMessage $proc.ExitCode
  }
  catch {
    Write-Error "Failed to install LLVM! $($_.Exception.Message)"
  }
}

<#
.SYNOPSIS
Installs Python.

.DESCRIPTION
InstallPython installs Python from the supplied URL.

.PARAMETER Url
The URL of the Python installer.
#>
Function InstallPython {
  Param(
    [String]$Url
  )

  Write-Host 'Downloading Python...'
  [string]$installerPath = Get-TempFilePath -Extension 'exe'
  curl.exe -L -o $installerPath -s -S $Url
  Write-Host 'Installing Python...'
  $proc = Start-Process -FilePath $installerPath -ArgumentList `
  @('/passive', 'InstallAllUsers=1', 'PrependPath=1', 'CompileAll=1') -Wait -PassThru
  $exitCode = $proc.ExitCode
  if ($exitCode -eq 0) {
    Write-Host 'Installation successful!'
  }
  else {
    Write-Error "Installation failed! Exited with $exitCode."
  }
}

<#
.SYNOPSIS
Installs NVIDIA's CUDA Toolkit.

.DESCRIPTION
InstallCuda installs the CUDA Toolkit with the features specified as a
space-separated list of strings in $Features.

.PARAMETER Url
The URL of the CUDA installer.

.PARAMETER Features
A space-separated list of features to install.
#>
Function InstallCuda {
  Param(
    [String]$Url,
    [String]$Features
  )

  try {
    Write-Host 'Downloading CUDA...'
    [string]$installerPath = Get-TempFilePath -Extension 'exe'
    curl.exe -L -o $installerPath -s -S $Url
    Write-Host 'Installing CUDA...'
    $proc = Start-Process -FilePath $installerPath -ArgumentList @('-s ' + $Features) -Wait -PassThru
    $exitCode = $proc.ExitCode
    if ($exitCode -eq 0) {
      Write-Host 'Installation successful!'
    }
    else {
      Write-Error "Installation failed! Exited with $exitCode."
    }
  }
  catch {
    Write-Error "Failed to install CUDA! $($_.Exception.Message)"
  }
}


Write-Host "AdminUser password not supplied; assuming already running as AdminUser"

Write-Host 'Configuring AntiVirus exclusions...'
Add-MpPreference -ExclusionPath C:\agent
Add-MpPreference -ExclusionPath D:\
Add-MpPreference -ExclusionProcess ninja.exe
Add-MpPreference -ExclusionProcess clang-cl.exe
Add-MpPreference -ExclusionProcess cl.exe
Add-MpPreference -ExclusionProcess link.exe
Add-MpPreference -ExclusionProcess python.exe

InstallMSI 'CMake' $CMakeUrl
InstallZip 'Ninja' $NinjaUrl 'C:\Program Files\CMake\bin'
InstallLLVM $LlvmUrl
InstallPython $PythonUrl
InstallVisualStudio -Workloads $Workloads -BootstrapperUrl $VisualStudioBootstrapperUrl
InstallCuda -Url $CudaUrl -Features $CudaFeatures

Write-Host 'Updating PATH...'
$environmentKey = Get-ItemProperty -Path 'HKLM:\SYSTEM\CurrentControlSet\Control\Session Manager\Environment' -Name Path
$Env:PATH="$($environmentKey.Path);C:\Program Files\CMake\bin;C:\Program Files\LLVM\bin"
Set-ItemProperty -Path 'HKLM:\SYSTEM\CurrentControlSet\Control\Session Manager\Environment' `
  -Name Path `
  -Value "$Env:PATH"

Write-Host 'Updating Python modules...'
python -m pip install --upgrade pip
pip install psutil
