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

.PARAMETER DiskType
The disk type can be either NVMe or SCSI.
#>
[CmdletBinding(PositionalBinding=$false)]
Param(
  [Parameter(Mandatory)][ValidateSet('x64', 'arm64')][String]$Arch,
  [Parameter(Mandatory)][ValidateSet('NVMe', 'SCSI')][String]$DiskType
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
  'Microsoft.VisualStudio.Component.VC.Tools.x86.x64', # TRANSITION, DevCom-11142709
  'Microsoft.VisualStudio.Component.Windows11SDK.28000'
)

# https://learn.microsoft.com/visualstudio/install/visual-studio-on-arm-devices
# "There's a single installer for both Visual Studio x64 and Visual Studio Arm64 architectures.
# The Visual Studio Installer detects whether the system architecture is Arm64.
# If it is, the installer downloads and installs the Arm64 version of Visual Studio."
$VisualStudioUrl = 'https://aka.ms/vs/insiders/vs_community.exe'
$VisualStudioArgs = @('--quiet', '--norestart', '--wait', '--nocache')
foreach ($workload in $VisualStudioWorkloads) {
  $VisualStudioArgs += '--add'
  $VisualStudioArgs += $workload
}

# https://github.com/PowerShell/PowerShell/releases/latest
if ($Provisioning_x64) {
  $PowerShellUrl = 'https://github.com/PowerShell/PowerShell/releases/download/v7.6.6/PowerShell-7.6.6-win-x64.msi'
} else {
  $PowerShellUrl = 'https://github.com/PowerShell/PowerShell/releases/download/v7.6.6/PowerShell-7.6.6-win-arm64.msi'
}
$PowerShellArgs = @('/quiet', '/norestart')

# https://www.python.org
if ($Provisioning_x64) {
  $PythonUrl = 'https://www.python.org/ftp/python/3.14.7/python-3.14.7-embed-amd64.zip'
} else {
  $PythonUrl = 'https://www.python.org/ftp/python/3.14.7/python-3.14.7-embed-arm64.zip'
}
$PythonPath = [uri]::new($PythonUrl).Segments[-1] -ireplace '(python-\d+\.\d+\.\d+)-embed-\w+\.zip', 'C:\$1'

# https://developer.nvidia.com/cuda-toolkit
if ($Provisioning_x64) {
  $CudaUrl = 'https://developer.download.nvidia.com/compute/cuda/13.4.2/local_installers/cuda_13.4.2_windows_x86_64.exe'
} else {
  $CudaUrl = 'CUDA supports ARM64, but is not yet installed here'
}
$CudaArgs = @('-s', '-n')

Function DownloadFile {
  [CmdletBinding(PositionalBinding=$false)]
  Param(
    [Parameter(Mandatory)][String]$Url
  )

  try {
    $tempDir = 'C:\downloadTemp'
    mkdir $tempDir -Force | Out-Null
    $fileName = [uri]::new($Url).Segments[-1]
    $downloadPath = Join-Path $tempDir $fileName
    curl.exe --fail --silent --show-error --location --output $downloadPath $Url
    if ($LASTEXITCODE -ne 0) {
      Write-Error "curl.exe failed with non-zero exit code $LASTEXITCODE."
    }
    return $downloadPath
  } catch {
    Write-Error "Download failed! Exception: $($_.Exception.Message)"
  }
}

Function SleepyDeleteFile {
  [CmdletBinding(PositionalBinding=$false)]
  Param(
    [Parameter(Mandatory)][String]$Path
  )

  try {
    # Briefly sleep before removing the file, attempting to avoid "Access to the path '$Path' is denied."
    Start-Sleep -Seconds 5
    Remove-Item -Path $Path
  } catch {
    Write-Error "Remove-Item failed! Exception: $($_.Exception.Message)"
  }
}

Function DownloadAndInstall {
  [CmdletBinding(PositionalBinding=$false)]
  Param(
    [Parameter(Mandatory)][String]$Name,
    [Parameter(Mandatory)][String]$Url,
    [Parameter(Mandatory)][String[]]$Args
  )

  Write-Host "Downloading $Name..."
  $installerPath = DownloadFile -Url $Url

  try {
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

  SleepyDeleteFile -Path $installerPath
}

Function DownloadAndExtract {
  [CmdletBinding(PositionalBinding=$false)]
  Param(
    [Parameter(Mandatory)][String]$Name,
    [Parameter(Mandatory)][String]$Url,
    [Parameter(Mandatory)][String]$Dest
  )

  Write-Host "Downloading $Name..."
  $zipPath = DownloadFile -Url $Url

  try {
    Write-Host "Extracting $Name..."
    Expand-Archive -Path $zipPath -DestinationPath $Dest -Force
    Write-Host 'Extraction successful!'
  } catch {
    Write-Error "Extraction failed! Exception: $($_.Exception.Message)"
  }

  SleepyDeleteFile -Path $zipPath
}

# Native NVMe support is opt-in for Windows Server 2025.
# TRANSITION, this will be enabled by default for the next version of Windows Server.
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
DownloadAndExtract   -Name 'Python'        -Url $PythonUrl       -Dest $PythonPath
DownloadAndInstall   -Name 'Visual Studio' -Url $VisualStudioUrl -Args $VisualStudioArgs
if ($Provisioning_x64) {
  DownloadAndInstall -Name 'CUDA'          -Url $CudaUrl         -Args $CudaArgs
}

Write-Host 'Setting environment variables...'

# Manually add Python to the PATH.
# Don't use $Env:PATH here - that's the local path for this running script, captured before we installed anything.
# The machine path was just updated by the installers above.
$machinePath = [Environment]::GetEnvironmentVariable('Path', 'Machine')
[Environment]::SetEnvironmentVariable('Path', "$PythonPath;$machinePath", 'Machine')

# The STL's PR/CI builds are totally unrepresentative of customer usage.
[Environment]::SetEnvironmentVariable('VSCMD_SKIP_SENDTELEMETRY', '1', 'Machine')

Write-Host 'Enabling long paths...'

# https://learn.microsoft.com/windows/win32/fileio/maximum-file-path-limitation
New-ItemProperty -Path 'HKLM:\SYSTEM\CurrentControlSet\Control\FileSystem' -Name 'LongPathsEnabled' `
  -Value 1 -PropertyType DWORD -Force | Out-Null

if ($DiskType -ieq 'NVMe') {
  Write-Host 'Enabling native NVMe...'
  EnableNativeNVMe
}

# Tell create-1es-hosted-pool.ps1 that we succeeded.
Write-Host 'PROVISION_IMAGE_SUCCEEDED'

exit
