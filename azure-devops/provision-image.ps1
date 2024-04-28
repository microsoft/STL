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
  [CmdletBinding(PositionalBinding=$false)]
  Param(
    [Parameter(Mandatory)][String]$Extension
  )

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
  [CmdletBinding(PositionalBinding=$false)]
  Param(
    [Parameter(Mandatory)][String]$Url
  )

  $ZipPath = Get-TempFilePath -Extension 'zip'
  curl.exe -L -o $ZipPath -s -S $Url
  $TempSubdirPath = Get-TempFilePath -Extension 'dir'
  Expand-Archive -Path $ZipPath -DestinationPath $TempSubdirPath -Force
  Remove-Item -Path $ZipPath

  return $TempSubdirPath
}

if ($PSVersionTable.PSVersion -lt [Version]::new('7.4.2')) {
  Write-Host "Old PowerShell version: $($PSVersionTable.PSVersion)"

  # https://github.com/PowerShell/PowerShell/releases/latest
  $PowerShellZipUrl = 'https://github.com/PowerShell/PowerShell/releases/download/v7.4.2/PowerShell-7.4.2-win-x64.zip'
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

$VisualStudioWorkloads = @(
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

$VisualStudioUrl = 'https://aka.ms/vs/17/pre/vs_enterprise.exe'
$VisualStudioArgs = @('--quiet', '--norestart', '--wait', '--nocache')
foreach ($workload in $VisualStudioWorkloads) {
  $VisualStudioArgs += '--add'
  $VisualStudioArgs += $workload
}

$PythonUrl = 'https://www.python.org/ftp/python/3.12.3/python-3.12.3-amd64.exe'
$PythonArgs = @('/quiet', 'InstallAllUsers=1', 'PrependPath=1', 'CompileAll=1', 'Include_doc=0')

$CudaUrl = 'https://developer.download.nvidia.com/compute/cuda/12.4.0/local_installers/cuda_12.4.0_551.61_windows.exe'
$CudaArgs = @('-s')

<#
.SYNOPSIS
Download and install a component.

.DESCRIPTION
DownloadAndInstall downloads an executable from the given URL, and runs it with the given command-line arguments.

.PARAMETER Name
The name of the component, to be displayed in logging messages.

.PARAMETER Url
The URL of the installer.

.PARAMETER Args
The command-line arguments to pass to the installer.
#>
Function DownloadAndInstall {
  [CmdletBinding(PositionalBinding=$false)]
  Param(
    [Parameter(Mandatory)][String]$Name,
    [Parameter(Mandatory)][String]$Url,
    [Parameter(Mandatory)][String[]]$Args
  )

  try {
    Write-Host "Downloading $Name..."
    [string]$installerPath = Get-TempFilePath -Extension 'exe'
    curl.exe -L -o $installerPath -s -S $Url

    Write-Host "Installing $Name..."
    $proc = Start-Process -FilePath $installerPath -ArgumentList $Args -Wait -PassThru
    $exitCode = $proc.ExitCode

    if ($exitCode -eq 0) {
      Write-Host 'Installation successful!'
    } elseif ($exitCode -eq 3010) {
      Write-Host 'Installation successful! Exited with 3010 (ERROR_SUCCESS_REBOOT_REQUIRED).'
    } else {
      Write-Error "Installation failed! Exited with $exitCode."
    }

    Remove-Item -Path $installerPath
  } catch {
    Write-Error "Installation failed! Exception: $($_.Exception.Message)"
  }
}

# Print the Windows version, so we can verify whether Patch Tuesday has been picked up.
cmd /c ver

DownloadAndInstall -Name 'Python'        -Url $PythonUrl       -Args $PythonArgs
DownloadAndInstall -Name 'Visual Studio' -Url $VisualStudioUrl -Args $VisualStudioArgs
DownloadAndInstall -Name 'CUDA'          -Url $CudaUrl         -Args $CudaArgs

Write-Host 'Setting environment variables...'

# The STL's PR/CI builds are totally unrepresentative of customer usage.
[Environment]::SetEnvironmentVariable('VSCMD_SKIP_SENDTELEMETRY', '1', 'Machine')

# Tell create-1es-hosted-pool.ps1 that we succeeded.
Write-Host 'PROVISION_IMAGE_SUCCEEDED'

exit
