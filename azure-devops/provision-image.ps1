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

.PARAMETER Arch
The architecture can be either x64 or arm64.
#>
[CmdletBinding(PositionalBinding=$false)]
Param(
  [Parameter(Mandatory)][ValidateSet('x64', 'arm64')][String]$Arch
)

$ErrorActionPreference = 'Stop'
$ProgressPreference = 'SilentlyContinue'

if ($Env:COMPUTERNAME -cne 'PROTOTYPE') {
  Write-Error 'You should not run provision-image.ps1 on your local machine.'
}

if ($Arch -ieq 'x64') {
  Write-Host 'Provisioning x64.'
  $Provisioning_x64 = $true
} else {
  Write-Host 'Provisioning ARM64.'
  $Provisioning_x64 = $false
}

$VisualStudioWorkloads = @(
  'Microsoft.VisualStudio.Component.VC.ASAN',
  'Microsoft.VisualStudio.Component.VC.CMake.Project',
  'Microsoft.VisualStudio.Component.VC.CoreIde',
  'Microsoft.VisualStudio.Component.VC.Llvm.Clang',
  'Microsoft.VisualStudio.Component.VC.Preview.ARM64',
  'Microsoft.VisualStudio.Component.VC.Preview.CLI.Support',
  'Microsoft.VisualStudio.Component.VC.Preview.Tools.x86.x64',
  'Microsoft.VisualStudio.Component.Windows11SDK.26100'
)

# https://learn.microsoft.com/visualstudio/install/visual-studio-on-arm-devices
# "There's a single installer for both Visual Studio x64 and Visual Studio Arm64 architectures.
# The Visual Studio Installer detects whether the system architecture is Arm64.
# If it is, the installer downloads and installs the Arm64 version of Visual Studio."
$VisualStudioUrl = 'https://aka.ms/vs/18/insiders/vs_Community.exe'
$VisualStudioArgs = @('--quiet', '--norestart', '--wait', '--nocache')
foreach ($workload in $VisualStudioWorkloads) {
  $VisualStudioArgs += '--add'
  $VisualStudioArgs += $workload
}

# https://github.com/PowerShell/PowerShell/releases/latest
if ($Provisioning_x64) {
  $PowerShellUrl = 'https://github.com/PowerShell/PowerShell/releases/download/v7.5.4/PowerShell-7.5.4-win-x64.msi'
} else {
  $PowerShellUrl = 'https://github.com/PowerShell/PowerShell/releases/download/v7.5.4/PowerShell-7.5.4-win-arm64.msi'
}
$PowerShellArgs = @('/quiet', '/norestart')

# https://www.python.org
if ($Provisioning_x64) {
  $PythonUrl = 'https://www.python.org/ftp/python/3.14.3/python-3.14.3-amd64.exe'
} else {
  $PythonUrl = 'https://www.python.org/ftp/python/3.14.3/python-3.14.3-arm64.exe'
}
$PythonArgs = @('/quiet', 'InstallAllUsers=1', 'PrependPath=1', 'CompileAll=1', 'Include_doc=0')

# https://developer.nvidia.com/cuda-toolkit
if ($Provisioning_x64) {
  $CudaUrl = 'https://developer.download.nvidia.com/compute/cuda/12.4.0/local_installers/cuda_12.4.0_551.61_windows.exe'
} else {
  $CudaUrl = 'CUDA is not installed for ARM64'
}
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
  } catch {
    Write-Error "Installation failed! Exception: $($_.Exception.Message)"
  }

  try {
    # Briefly sleep before removing the installer, attempting to avoid "Access to the path '$installerPath' is denied."
    Start-Sleep -Seconds 5
    Remove-Item -Path $installerPath
  } catch {
    Write-Error "Remove-Item failed! Exception: $($_.Exception.Message)"
  }
}

<#
.SYNOPSIS
Enables native NVMe support.

.DESCRIPTION
Native NVMe support is opt-in for Windows Server 2025.
TRANSITION, this will be enabled by default for the next version of Windows Server.
#>
Function EnableNativeNVMe {
  $registryKey = 'HKLM:\SYSTEM\CurrentControlSet\Policies\Microsoft\FeatureManagement\Overrides'
  $valueName = '1176759950'
  $valueData = 1

  if (!(Test-Path $registryKey)) {
    New-Item -Path $registryKey -Force | Out-Null
  }

  New-ItemProperty -Path $registryKey -Name $valueName -Value $valueData -PropertyType DWORD -Force | Out-Null
}

Write-Host "Old PowerShell version: $($PSVersionTable.PSVersion)"

# Print the Windows version, so we can verify whether Patch Tuesday has been picked up.
# Skip a blank line to improve the output.
(cmd /c ver)[1]

DownloadAndInstall   -Name 'PowerShell'    -Url $PowerShellUrl   -Args $PowerShellArgs
DownloadAndInstall   -Name 'Python'        -Url $PythonUrl       -Args $PythonArgs
DownloadAndInstall   -Name 'Visual Studio' -Url $VisualStudioUrl -Args $VisualStudioArgs
if ($Provisioning_x64) {
  DownloadAndInstall -Name 'CUDA'          -Url $CudaUrl         -Args $CudaArgs
}

Write-Host 'Setting environment variables...'

# The STL's PR/CI builds are totally unrepresentative of customer usage.
[Environment]::SetEnvironmentVariable('VSCMD_SKIP_SENDTELEMETRY', '1', 'Machine')

Write-Host 'Enabling long paths...'

# https://learn.microsoft.com/windows/win32/fileio/maximum-file-path-limitation
New-ItemProperty -Path 'HKLM:\SYSTEM\CurrentControlSet\Control\FileSystem' -Name 'LongPathsEnabled' `
  -Value 1 -PropertyType DWORD -Force | Out-Null

if ($Provisioning_x64) {
  Write-Host 'Enabling native NVMe...'
  EnableNativeNVMe
}

# TRANSITION, patch Launch-VsDevShell.ps1 to pass `-vcvars_ver=preview` before a proper parameter is available.
Write-Host 'Patching Launch-VsDevShell.ps1...'
$launchVsDevShell = 'C:\Program Files\Microsoft Visual Studio\18\Insiders\Common7\Tools\Launch-VsDevShell.ps1'
$paramRegex = 'VsInstanceId = \$instanceId'
$paramSubst = '$&; DevCmdArguments = "-vcvars_ver=preview";'
(Get-Content -Raw $launchVsDevShell) -creplace $paramRegex, $paramSubst | Set-Content -NoNewLine $launchVsDevShell

# Tell create-1es-hosted-pool.ps1 that we succeeded.
Write-Host 'PROVISION_IMAGE_SUCCEEDED'

exit
