# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# Sets up a machine in preparation to become a build machine image, optionally switching to
# AdminUser first.
param(
  [string]$AdminUserPassword = $null
)

$TempPath = [System.IO.Path]::GetTempPath()

if (-not [string]::IsNullOrEmpty($AdminUserPassword)) {
  Write-Output "AdminUser password supplied; switching to AdminUser"
  $PsExecPath = $TempPath + "\psexec.exe"
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
  Write-Output "Executing $PsExecPath @PsExecArgs"
  & $PsExecPath @PsExecArgs
  exit $?
}

$WorkLoads = '--add Microsoft.VisualStudio.Component.VC.CLI.Support ' + `
  '--add Microsoft.VisualStudio.Component.VC.Tools.x86.x64 ' + `
  '--add Microsoft.VisualStudio.Component.VC.Tools.ARM64 ' + `
  '--add Microsoft.VisualStudio.Component.VC.Tools.ARM ' + `
  '--add Microsoft.VisualStudio.Component.Windows10SDK.18362 '

$ReleaseInPath = 'Preview'
$Sku = 'Enterprise'
$VisualStudioBootstrapperUrl = 'https://aka.ms/vs/16/pre/vs_buildtools.exe'
$CMakeUrl = 'https://github.com/Kitware/CMake/releases/download/v3.16.5/cmake-3.16.5-win64-x64.msi'
$LlvmUrl = 'https://github.com/llvm/llvm-project/releases/download/llvmorg-10.0.0/LLVM-10.0.0-win64.exe'
$NinjaUrl = 'https://github.com/ninja-build/ninja/releases/download/v1.10.0/ninja-win.zip'
$PythonUrl = 'https://www.python.org/ftp/python/3.8.2/python-3.8.2-amd64.exe'

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

Function Get-TempFilePath {
  Param(
    [String]$Extension
  )
  if ([String]::IsNullOrWhiteSpace($Extension)) {
    throw 'Missing Extension'
  }

  return Join-Path $TempPath ([System.IO.Path]::GetRandomFileName() + '.' + $Extension)
}

Function InstallVisualStudio {
  Param(
    [String]$WorkLoads,
    [String]$Sku,
    [String]$BootstrapperUrl
  )

  try {
    Write-Output 'Downloading Visual Studio...'
    [string]$bootstrapperExe = Get-TempFileName -Extension 'exe'
    curl.exe -L -o $bootstrapperExe $BootstrapperUrl
    Write-Output "Installing Visual Studio..."
    $args = ('/c', $bootstrapperExe, $WorkLoads, '--quiet', '--norestart', '--wait', '--nocache')
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
    [string]$msiPath = Get-TempFileName -Extension 'msi'
    curl.exe -L -o $msiPath $Url
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
    [string]$zipPath = Get-TempFileName -Extension 'zip'
    curl.exe -L -o $zipPath $Url
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
    [string]$installerPath = Get-TempFileName -Extension 'exe'
    curl.exe -L -o $installerPath $Url
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
  [string]$installerPath = Get-TempFileName -Extension 'exe'
  curl.exe -L -o $installerPath $Url
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


Write-Output "AdminUser password not supplied; assuming already running as AdminUser"
InstallMSI 'CMake' $CMakeUrl
InstallZip 'Ninja' $NinjaUrl 'C:\Program Files\CMake\bin'
InstallLLVM $LlvmUrl
InstallPython $PythonUrl
InstallVisualStudio -WorkLoads $WorkLoads -Sku $Sku -BootstrapperUrl $VisualStudioBootstrapperUrl
Write-Output 'Updating PATH...'
$environmentKey = Get-ItemProperty -Path 'HKLM:\SYSTEM\CurrentControlSet\Control\Session Manager\Environment' -Name Path
Set-ItemProperty -Path 'HKLM:\SYSTEM\CurrentControlSet\Control\Session Manager\Environment' `
  -Name Path `
  -Value "$($environmentKey.Path);C:\Program Files\CMake\bin;C:\Program Files\LLVM\bin"

Add-MPPreference -ExclusionPath C:\agent
Add-MPPreference -ExclusionPath D:\
Add-MPPreference -ExclusionProcess ninja.exe
Add-MPPreference -ExclusionProcess clang-cl.exe
Add-MPPreference -ExclusionProcess cl.exe
Add-MPPreference -ExclusionProcess link.exe
Add-MPPreference -ExclusionProcess python.exe

C:\Windows\system32\sysprep\sysprep.exe /oobe /generalize /shutdown
