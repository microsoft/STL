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
#>

$ErrorActionPreference = 'Stop'
$ProgressPreference = 'SilentlyContinue'

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

<#
.SYNOPSIS
Downloads and extracts a ZIP file to a newly created temporary subdirectory.

.DESCRIPTION
DownloadAndExtractZip returns a path containing the extracted contents.

.PARAMETER Url
The URL of the ZIP file to download.
#>
Function DownloadAndExtractZip {
  Param(
    [String]$Url
  )

  if ([String]::IsNullOrWhiteSpace($Url)) {
    throw 'Missing Url'
  }

  $ZipPath = Get-TempFilePath -Extension 'zip'
  curl.exe -L -o $ZipPath -s -S $Url
  $TempSubdirPath = Get-TempFilePath -Extension 'dir'
  Expand-Archive -Path $ZipPath -DestinationPath $TempSubdirPath -Force
  Remove-Item -Path $ZipPath

  return $TempSubdirPath
}

if ($PSVersionTable.PSVersion -lt [Version]::new('7.4.1')) {
  Write-Host "Old PowerShell version: $($PSVersionTable.PSVersion)"

  # https://github.com/PowerShell/PowerShell/releases/latest
  $PowerShellZipUrl = 'https://github.com/PowerShell/PowerShell/releases/download/v7.4.1/PowerShell-7.4.1-win-x64.zip'
  Write-Host "Downloading: $PowerShellZipUrl"
  $ExtractedPowerShellPath = DownloadAndExtractZip -Url $PowerShellZipUrl
  $PwshPath = Join-Path $ExtractedPowerShellPath 'pwsh.exe'

  $PwshArgs = @(
    '-ExecutionPolicy',
    'Unrestricted',
    '-File',
    $PSCommandPath
  )
  Write-Host "Executing: $PwshPath $PwshArgs"
  & $PwshPath $PwshArgs

  Write-Host 'Cleaning up...'
  Remove-Item -Recurse -Path $ExtractedPowerShellPath
  exit
}

$Workloads = @(
  'Microsoft.VisualStudio.Component.VC.ASAN',
  'Microsoft.VisualStudio.Component.VC.CLI.Support',
  'Microsoft.VisualStudio.Component.VC.CMake.Project',
  'Microsoft.VisualStudio.Component.VC.CoreIde',
  'Microsoft.VisualStudio.Component.VC.Llvm.Clang',
  'Microsoft.VisualStudio.Component.VC.Tools.ARM',
  'Microsoft.VisualStudio.Component.VC.Tools.ARM64',
  'Microsoft.VisualStudio.Component.VC.Tools.ARM64EC',
  'Microsoft.VisualStudio.Component.VC.Tools.x86.x64',
  'Microsoft.VisualStudio.Component.Windows11SDK.22621'
)

$VisualStudioBootstrapperUrl = 'https://aka.ms/vs/17/pre/vs_enterprise.exe'
$PythonUrl = 'https://www.python.org/ftp/python/3.12.2/python-3.12.2-amd64.exe'

$CudaUrl = 'https://developer.download.nvidia.com/compute/cuda/12.4.0/local_installers/cuda_12.4.0_551.61_windows.exe'

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
#>
Function InstallVisualStudio {
  Param(
    [String[]]$Workloads,
    [String]$BootstrapperUrl
  )

  try {
    Write-Host 'Downloading Visual Studio...'
    [string]$bootstrapperExe = Get-TempFilePath -Extension 'exe'
    curl.exe -L -o $bootstrapperExe -s -S $BootstrapperUrl
    Write-Host 'Installing Visual Studio...'
    $args = @('/c', $bootstrapperExe, '--quiet', '--norestart', '--wait', '--nocache')
    foreach ($workload in $Workloads) {
      $args += '--add'
      $args += $workload
    }

    $proc = Start-Process -FilePath cmd.exe -ArgumentList $args -Wait -PassThru
    PrintMsiExitCodeMessage $proc.ExitCode
    Remove-Item -Path $bootstrapperExe
  }
  catch {
    Write-Error "Failed to install Visual Studio! $($_.Exception.Message)"
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

  try {
    Write-Host 'Downloading Python...'
    [string]$installerPath = Get-TempFilePath -Extension 'exe'
    curl.exe -L -o $installerPath -s -S $Url
    Write-Host 'Installing Python...'
    $args = @('/quiet', 'InstallAllUsers=1', 'PrependPath=1', 'CompileAll=1', 'Include_doc=0')
    $proc = Start-Process -FilePath $installerPath -ArgumentList $args -Wait -PassThru
    $exitCode = $proc.ExitCode
    if ($exitCode -eq 0) {
      Write-Host 'Installation successful!'
    }
    else {
      Write-Error "Installation failed! Exited with $exitCode."
    }
    Remove-Item -Path $installerPath
  }
  catch {
    Write-Error "Failed to install Python! $($_.Exception.Message)"
  }
}

<#
.SYNOPSIS
Installs NVIDIA's CUDA Toolkit.

.DESCRIPTION
InstallCuda installs the CUDA Toolkit.

.PARAMETER Url
The URL of the CUDA installer.
#>
Function InstallCuda {
  Param(
    [String]$Url
  )

  try {
    Write-Host 'Downloading CUDA...'
    [string]$installerPath = Get-TempFilePath -Extension 'exe'
    curl.exe -L -o $installerPath -s -S $Url
    Write-Host 'Installing CUDA...'
    $proc = Start-Process -FilePath $installerPath -ArgumentList @('-s') -Wait -PassThru
    $exitCode = $proc.ExitCode
    if ($exitCode -eq 0) {
      Write-Host 'Installation successful!'
    }
    else {
      Write-Error "Installation failed! Exited with $exitCode."
    }
    Remove-Item -Path $installerPath
  }
  catch {
    Write-Error "Failed to install CUDA! $($_.Exception.Message)"
  }
}

<#
.SYNOPSIS
Install or upgrade a pip package.

.DESCRIPTION
Installs or upgrades a pip package specified in $Package.

.PARAMETER Package
The name of the package to be installed or upgraded.
#>
Function PipInstall {
  Param(
    [String]$Package
  )

  try {
    Write-Host "Installing or upgrading $Package..."
    python.exe -m pip install --progress-bar off --upgrade $Package
    Write-Host "Done installing or upgrading $Package."
  }
  catch {
    Write-Error "Failed to install or upgrade $Package."
  }
}

# Print the Windows version, so we can verify whether Patch Tuesday has been picked up.
cmd /c ver

InstallPython $PythonUrl
InstallVisualStudio -Workloads $Workloads -BootstrapperUrl $VisualStudioBootstrapperUrl
InstallCuda -Url $CudaUrl

Write-Host 'Updating PATH...'

# Step 1: Read the system path, which was just updated by installing Python.
$currentSystemPath = [Environment]::GetEnvironmentVariable('Path', 'Machine')

# Step 2: Update the local path (for this running script), so PipInstall can run python.exe.
# Additional directories can be added here (e.g. if we extracted a zip file
# or installed something that didn't update the system path).
$Env:PATH="$($currentSystemPath)"

# Step 3: Update the system path, permanently recording any additional directories that were added in the previous step.
[Environment]::SetEnvironmentVariable('Path', "$Env:PATH", 'Machine')

Write-Host 'Finished updating PATH!'

Write-Host 'Running PipInstall...'

PipInstall pip
PipInstall psutil

Write-Host 'Finished running PipInstall!'

Write-Host 'Setting other environment variables...'

# The STL's PR/CI builds are totally unrepresentative of customer usage.
[Environment]::SetEnvironmentVariable('VSCMD_SKIP_SENDTELEMETRY', '1', 'Machine')

Write-Host 'Finished setting other environment variables!'

Write-Host 'Done!'

exit
