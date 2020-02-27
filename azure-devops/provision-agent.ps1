# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

# Sets up VM for use as a build machine

Write-Output 'Starting...'

if (Test-Path 'C:\agent') {
    Write-Output 'Agent already installed, terminating.'
    exit 0
}

Write-Output 'Agent doesn''t look installed yet'

$AzureDevOpsURL = 'https://dev.azure.com/vclibs/'
$AzureDevOpsPool = 'STL'
[string]$PersonalAccessToken = $args[0]

$WorkLoads =  '--add Microsoft.VisualStudio.Component.VC.CLI.Support ' + `
              '--add Microsoft.VisualStudio.Component.VC.Tools.x86.x64 ' + `
              '--add Microsoft.VisualStudio.Component.VC.Tools.ARM64 ' + `
              '--add Microsoft.VisualStudio.Component.VC.Tools.ARM ' + `
              '--add Microsoft.VisualStudio.Component.Windows10SDK.18362 '

$ReleaseInPath = 'Preview'
$Sku = 'Enterprise'
$VSBootstrapperURL = 'https://aka.ms/vs/16/pre/vs_buildtools.exe'
$CMakeURL = 'https://github.com/Kitware/CMake/releases/download/v3.16.4/cmake-3.16.4-win64-x64.msi'
$LlvmURL = 'https://releases.llvm.org/9.0.0/LLVM-9.0.0-win64.exe'
$NinjaURL = 'https://github.com/ninja-build/ninja/releases/download/v1.9.0/ninja-win.zip'
$PythonURL = 'https://www.python.org/ftp/python/3.8.1/python-3.8.1-amd64.exe'
$VstsAgentURL = 'https://vstsagentpackage.azureedge.net/agent/2.165.0/vsts-agent-win-x64-2.165.0.zip'

$ErrorActionPreference = 'Stop'
$ProgressPreference = 'SilentlyContinue'

Function InstallVS
{
  Param(
    [String]$WorkLoads,
    [String]$Sku,
    [String]$VSBootstrapperURL)
  try
  {
    Write-Output 'Downloading VS bootstrapper ...'
    [string]$bootstrapperExe = Join-Path ([System.IO.Path]::GetTempPath()) ([System.IO.Path]::GetRandomFileName() + '.exe')
    curl.exe -L -o $bootstrapperExe $VSBootstrapperURL

    $Arguments = ('/c', $bootstrapperExe, $WorkLoads, '--quiet', '--norestart', '--wait', '--nocache')

    Write-Output "Starting Install: $Arguments"
    $proc = Start-Process -FilePath cmd.exe -ArgumentList $Arguments -Wait -PassThru
    $exitCode = $proc.ExitCode

    if ($exitCode -eq 0 -or $exitCode -eq 3010)
    {
        Write-Output "Installation successful! $exitCode"
    }
    else
    {
        Write-Output "Nonzero exit code returned by the installation process : $exitCode."
        exit $exitCode
    }
  }
  catch
  {
    Write-Output 'Failed to install Visual Studio!'
    Write-Output $_.Exception.Message
    exit 1
  }
}

Function InstallMSI
{
    Param(
        [String]$Name,
        [String]$Uri
    )

    try
    {
        Write-Output "Downloading $Name..."
        [string]$tempRoot = [System.IO.Path]::GetTempPath();
        [string]$randomRoot = Join-Path $tempRoot ([System.IO.Path]::GetRandomFileName())
        [string]$msiPath = $randomRoot + '.msi'
        [string]$msiExecPath = 'msiexec.exe'
        $args = @('/i', $msiPath, '/norestart', '/quiet', '/qn')

        curl.exe -L -o $msiPath $Uri
        Write-Output "Installing $Name..."
        $proc = Start-Process -FilePath $msiExecPath -ArgumentList $args -Wait -PassThru
        $exitCode = $proc.ExitCode
        if ($exitCode -eq 0 -or $exitCode -eq 3010)
        {
            Write-Output 'Installation successful!'
        }
        else
        {
            Write-Output "Nonzero exit code returned by the installation process : $exitCode."
            exit $exitCode
        }
    }
    catch
    {
        Write-Output "Failed to install $Name!"
        Write-Output $_.Exception.Message
        exit -1
    }
}

Function InstallZip
{
    Param(
        [String]$Name,
        [String]$Uri,
        [String]$Dir
    )

    try
    {
        Write-Output "Downloading $Name..."
        [string]$randomRoot = Join-Path ([System.IO.Path]::GetTempPath()) ([System.IO.Path]::GetRandomFileName())
        [string]$zipPath = $randomRoot + '.zip'
        curl.exe -L -o $zipPath $Uri
        Write-Output "Installing $Name..."
        Expand-Archive -Path $zipPath -DestinationPath $Dir -Force
    }
    catch
    {
        Write-Output "Failed to install $Name!"
        Write-Output $_.Exception.Message
        exit -1
    }
}

Function InstallLLVM
{
    Param(
        [String]$Uri
    )

    Write-Output 'Downloading LLVM...'
    [string]$randomRoot = Join-Path ([System.IO.Path]::GetTempPath()) ([System.IO.Path]::GetRandomFileName())
    [string]$installerPath = $randomRoot + '.exe'

    curl.exe -L -o $installerPath $Uri
    Write-Output 'Installing LLVM...'
    $proc = Start-Process -FilePath $installerPath -ArgumentList @('/S') -NoNewWindow -Wait -PassThru
    $exitCode = $proc.ExitCode

    if ($exitCode -eq 0 -or $exitCode -eq 3010)
    {
        Write-Output 'Installation successful!'
    }
    else
    {
        Write-Output "Nonzero exit code returned by the installation process : $exitCode."
        exit $exitCode
    }
}

Function InstallPython
{
    Param(
        [String]$Uri
    )

    Write-Output 'Downloading Python...'
    [string]$randomRoot = Join-Path ([System.IO.Path]::GetTempPath()) ([System.IO.Path]::GetRandomFileName())
    [string]$installerPath = $randomRoot + '.exe'

    curl.exe -L -o $installerPath $Uri
    Write-Output 'Installing Python...'
    $proc = Start-Process -FilePath $installerPath -ArgumentList @('/passive', 'InstallAllUsers=1', 'PrependPath=1', 'CompileAll=1') -Wait -PassThru
    $exitCode = $proc.ExitCode

    if ($exitCode -eq 0)
    {
        Write-Output 'Installation successful!'
    }
    else
    {
        Write-Output "Nonzero exit code returned by the installation process : $exitCode."
        exit $exitCode
    }
}

if ([string]::IsNullOrEmpty($PersonalAccessToken) -or ($PersonalAccessToken -eq 'CHANGE THIS')) {
    Write-Output 'You forgot to fill in your personal access token.'
    exit 1
}

InstallMSI 'CMake' $CMakeURL
InstallZip 'Ninja' $NinjaURL 'C:\Program Files\CMake\bin'
InstallLLVM $LlvmURL
InstallPython $PythonURL
InstallVS -WorkLoads $WorkLoads -Sku $Sku -VSBootstrapperURL $VSBootstrapperURL
Write-Output 'Updating PATH ...'
$environmentKey = Get-ItemProperty -Path 'HKLM:\SYSTEM\CurrentControlSet\Control\Session Manager\Environment' -Name Path
Set-ItemProperty -Path 'HKLM:\SYSTEM\CurrentControlSet\Control\Session Manager\Environment' -Name Path -Value "$($environmentKey.Path);C:\Program Files\CMake\bin;C:\Program Files\LLVM\bin"
InstallZip 'Azure DevOps Agent' $VstsAgentURL 'C:\agent'
Add-MpPreference -ExclusionPath C:\agent
& 'C:\agent\config.cmd' --unattended --url $AzureDevOpsURL --auth pat --token $PersonalAccessToken --pool $AzureDevOpsPool --runAsService
shutdown /r /t 10
