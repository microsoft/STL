# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# Sets up a machine in preparation to become a build machine image, optionally switching to
# AdminUser first.
param(
  [string]$AdminUserPassword = $null
)

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
  Write-Output "AdminUser password supplied; switching to AdminUser"
  $PsExecPath = Get-TempFilePath -Extension 'exe'
  Write-Output "Downloading psexec to $PsExecPath"
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

  Write-Output "Executing $PsExecPath " + @PsExecArgs

  $proc = Start-Process -FilePath $PsExecPath -ArgumentList $PsExecArgs -Wait -PassThru
  Write-Output 'Cleaning up...'
  Remove-Item $PsExecPath
  exit $proc.ExitCode
}

$Workloads = @(
  'Microsoft.VisualStudio.Component.VC.CLI.Support',
  'Microsoft.VisualStudio.Component.VC.Tools.x86.x64 ',
  'Microsoft.VisualStudio.Component.VC.Tools.ARM64 ',
  'Microsoft.VisualStudio.Component.VC.Tools.ARM ',
  'Microsoft.VisualStudio.Component.Windows10SDK.18362 '
)

$ReleaseInPath = 'Preview'
$Sku = 'Enterprise'
$VisualStudioBootstrapperUrl = 'https://aka.ms/vs/16/pre/vs_buildtools.exe'
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

Function PrintMsiExitCodeMessage {
  Param(
    $ExitCode
  )

  if ($ExitCode -eq 0 -or $ExitCode -eq 3010) {
    Write-Output "Installation successful! Exited with $ExitCode."
  }
  else {
    Write-Output "Installation failed! Exited with $ExitCode."
    exit $ExitCode
  }
}

Function InstallVisualStudio {
  Param(
    [String[]]$Workloads,
    [String]$BootstrapperUrl,
    [String]$InstallPath = $null,
    [String]$Nickname = $null
  )

  try {
    Write-Output 'Downloading Visual Studio...'
    [string]$bootstrapperExe = Get-TempFilePath -Extension 'exe'
    curl.exe -L -o $bootstrapperExe -s -S $BootstrapperUrl
    Write-Output "Installing Visual Studio..."
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
    Write-Output 'Failed to install Visual Studio!'
    Write-Output $_.Exception.Message
    exit 1
  }
}

Function InstallMSI {
  Param(
    [String]$Name,
    [String]$Url
  )

  try {
    Write-Output "Downloading $Name..."
    [string]$msiPath = Get-TempFilePath -Extension 'msi'
    curl.exe -L -o $msiPath -s -S $Url
    Write-Output "Installing $Name..."
    $args = @('/i', $msiPath, '/norestart', '/quiet', '/qn')
    $proc = Start-Process -FilePath 'msiexec.exe' -ArgumentList $args -Wait -PassThru
    PrintMsiExitCodeMessage $proc.ExitCode
  }
  catch {
    Write-Output "Failed to install $Name!"
    Write-Output $_.Exception.Message
    exit -1
  }
}

Function InstallZip {
  Param(
    [String]$Name,
    [String]$Url,
    [String]$Dir
  )

  try {
    Write-Output "Downloading $Name..."
    [string]$zipPath = Get-TempFilePath -Extension 'zip'
    curl.exe -L -o $zipPath -s -S $Url
    Write-Output "Installing $Name..."
    Expand-Archive -Path $zipPath -DestinationPath $Dir -Force
  }
  catch {
    Write-Output "Failed to install $Name!"
    Write-Output $_.Exception.Message
    exit -1
  }
}

Function InstallLLVM {
  Param(
    [String]$Url
  )

  try {
    Write-Output 'Downloading LLVM...'
    [string]$installerPath = Get-TempFilePath -Extension 'exe'
    curl.exe -L -o $installerPath -s -S $Url
    Write-Output 'Installing LLVM...'
    $proc = Start-Process -FilePath $installerPath -ArgumentList @('/S') -NoNewWindow -Wait -PassThru
    PrintMsiExitCodeMessage $proc.ExitCode
  }
  catch {
    Write-Output "Failed to install LLVM!"
    Write-Output $_.Exception.Message
    exit -1
  }
}

Function InstallPython {
  Param(
    [String]$Url
  )

  Write-Output 'Downloading Python...'
  [string]$installerPath = Get-TempFilePath -Extension 'exe'
  curl.exe -L -o $installerPath -s -S $Url
  Write-Output 'Installing Python...'
  $proc = Start-Process -FilePath $installerPath -ArgumentList `
  @('/passive', 'InstallAllUsers=1', 'PrependPath=1', 'CompileAll=1') -Wait -PassThru
  $exitCode = $proc.ExitCode
  if ($exitCode -eq 0) {
    Write-Output 'Installation successful!'
  }
  else {
    Write-Output "Installation failed! Exited with $exitCode."
    exit $exitCode
  }
}

Function InstallCuda {
  Param(
    [String]$Url,
    [String]$Features
  )

  try {
    Write-Output 'Downloading CUDA...'
    [string]$installerPath = Get-TempFilePath -Extension 'exe'
    curl.exe -L -o $installerPath -s -S $Url
    Write-Output 'Installing CUDA...'
    $proc = Start-Process -FilePath $installerPath -ArgumentList @('-s ' + $Features) -Wait -PassThru
    $exitCode = $proc.ExitCode
    if ($exitCode -eq 0) {
      Write-Output 'Installation successful!'
    }
    else {
      Write-Output "Installation failed! Exited with $exitCode."
      exit $exitCode
    }
  }
  catch {
    Write-Output "Failed to install CUDA!"
    Write-Output $_.Exception.Message
    exit -1
  }
}


Write-Output "AdminUser password not supplied; assuming already running as AdminUser"

Write-Host 'Configuring AntiVirus exclusions...'
Add-MPPreference -ExclusionPath C:\agent
Add-MPPreference -ExclusionPath D:\
Add-MPPreference -ExclusionProcess ninja.exe
Add-MPPreference -ExclusionProcess clang-cl.exe
Add-MPPreference -ExclusionProcess cl.exe
Add-MPPreference -ExclusionProcess link.exe
Add-MPPreference -ExclusionProcess python.exe

InstallMSI 'CMake' $CMakeUrl
InstallZip 'Ninja' $NinjaUrl 'C:\Program Files\CMake\bin'
InstallLLVM $LlvmUrl
InstallPython $PythonUrl
InstallVisualStudio -Workloads $Workloads -BootstrapperUrl $VisualStudioBootstrapperUrl
InstallCuda -Url $CudaUrl -Features $CudaFeatures
Write-Output 'Updating PATH...'
$environmentKey = Get-ItemProperty -Path 'HKLM:\SYSTEM\CurrentControlSet\Control\Session Manager\Environment' -Name Path
Set-ItemProperty -Path 'HKLM:\SYSTEM\CurrentControlSet\Control\Session Manager\Environment' `
  -Name Path `
  -Value "$($environmentKey.Path);C:\Program Files\CMake\bin;C:\Program Files\LLVM\bin"
