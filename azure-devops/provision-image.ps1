# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

<#
.SYNOPSIS
Sets up a virtual machine to be an image for a hosted pool.

.DESCRIPTION
create-1es-hosted-pool.ps1 (running on an STL maintainer's machine) creates a "prototype" virtual machine in Azure,
then runs provision-image.ps1 on that VM. This gives us full control over what we install for building and testing
the STL. After provision-image.ps1 is done, create-1es-hosted-pool.ps1 makes an image of the prototype VM,
creates a 1ES Hosted Pool that will spin up copies of the image as worker VMs, and finally deletes the prototype VM.
#>

$ErrorActionPreference = 'Stop'
$ProgressPreference = 'SilentlyContinue'

if ($Env:COMPUTERNAME -cne 'PROTOTYPE') {
  Write-Error 'You should not run provision-image.ps1 on your local machine.'
}

$VisualStudioWorkloads = @(
  'Microsoft.VisualStudio.Component.VC.ASAN',
  'Microsoft.VisualStudio.Component.VC.CLI.Support',
  'Microsoft.VisualStudio.Component.VC.CMake.Project',
  'Microsoft.VisualStudio.Component.VC.CoreIde',
  'Microsoft.VisualStudio.Component.VC.Llvm.Clang',
  'Microsoft.VisualStudio.Component.VC.Tools.ARM64',
  'Microsoft.VisualStudio.Component.VC.Tools.ARM64EC',
  'Microsoft.VisualStudio.Component.VC.Tools.x86.x64',
  'Microsoft.VisualStudio.Component.Windows11SDK.26100'
)

$VisualStudioUrl = 'https://aka.ms/vs/18/insiders/vs_Community.exe'
$VisualStudioArgs = @('--quiet', '--norestart', '--wait', '--nocache')
foreach ($workload in $VisualStudioWorkloads) {
  $VisualStudioArgs += '--add'
  $VisualStudioArgs += $workload
}

# https://github.com/PowerShell/PowerShell/releases/latest
$PowerShellUrl = 'https://github.com/PowerShell/PowerShell/releases/download/v7.5.2/PowerShell-7.5.2-win-x64.msi'
$PowerShellArgs = @('/quiet', '/norestart')

$PythonUrl = 'https://www.python.org/ftp/python/3.13.7/python-3.13.7-amd64.exe'
$PythonArgs = @('/quiet', 'InstallAllUsers=1', 'PrependPath=1', 'CompileAll=1', 'Include_doc=0')

$CudaUrl = 'https://developer.download.nvidia.com/compute/cuda/12.4.0/local_installers/cuda_12.4.0_551.61_windows.exe'
$CudaArgs = @('-s', '-n')

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
    $tempPath = 'C:\installerTemp'
    mkdir $tempPath -Force | Out-Null
    $fileName = [uri]::new($Url).Segments[-1]
    $installerPath = Join-Path $tempPath $fileName
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

Write-Host "Old PowerShell version: $($PSVersionTable.PSVersion)"

# Print the Windows version, so we can verify whether Patch Tuesday has been picked up.
# Skip a blank line to improve the output.
(cmd /c ver)[1]

DownloadAndInstall -Name 'PowerShell'    -Url $PowerShellUrl   -Args $PowerShellArgs
DownloadAndInstall -Name 'Python'        -Url $PythonUrl       -Args $PythonArgs
DownloadAndInstall -Name 'Visual Studio' -Url $VisualStudioUrl -Args $VisualStudioArgs
DownloadAndInstall -Name 'CUDA'          -Url $CudaUrl         -Args $CudaArgs

Write-Host 'Setting environment variables...'

# The STL's PR/CI builds are totally unrepresentative of customer usage.
[Environment]::SetEnvironmentVariable('VSCMD_SKIP_SENDTELEMETRY', '1', 'Machine')

# Tell create-1es-hosted-pool.ps1 that we succeeded.
Write-Host 'PROVISION_IMAGE_SUCCEEDED'

exit
