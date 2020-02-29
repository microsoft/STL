# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

# Sets up VM for use as a build machine

Write-Output 'Starting...'

if (Test-Path 'C:\agent') {
    Write-Output 'Agent already installed, terminating.'
    exit 0
}

Write-Output 'Agent does not appear to be installed.'

$AzureDevOpsUrl = 'https://dev.azure.com/vclibs/'
$AzureDevOpsPool = 'STL'
[string]$PersonalAccessToken = $args[0]

$WorkLoads =  '--add Microsoft.VisualStudio.Component.VC.CLI.Support ' + `
              '--add Microsoft.VisualStudio.Component.VC.Tools.x86.x64 ' + `
              '--add Microsoft.VisualStudio.Component.VC.Tools.ARM64 ' + `
              '--add Microsoft.VisualStudio.Component.VC.Tools.ARM ' + `
              '--add Microsoft.VisualStudio.Component.Windows10SDK.18362 '

$ReleaseInPath = 'Preview'
$Sku = 'Enterprise'
$VisualStudioBootstrapperUrl = 'https://aka.ms/vs/16/pre/vs_buildtools.exe'
$CMakeUrl = 'https://github.com/Kitware/CMake/releases/download/v3.16.4/cmake-3.16.4-win64-x64.msi'
$LlvmUrl = 'https://releases.llvm.org/9.0.0/LLVM-9.0.0-win64.exe'
$NinjaUrl = 'https://github.com/ninja-build/ninja/releases/download/v1.9.0/ninja-win.zip'
$PythonUrl = 'https://www.python.org/ftp/python/3.8.1/python-3.8.1-amd64.exe'
$VstsAgentUrl = 'https://vstsagentpackage.azureedge.net/agent/2.165.0/vsts-agent-win-x64-2.165.0.zip'

$ErrorActionPreference = 'Stop'
$ProgressPreference = 'SilentlyContinue'

Function PrintMsiExitCodeMessage
{
    Param(
        $ExitCode
    )

    if ($ExitCode -eq 0 -or $ExitCode -eq 3010)
    {
        Write-Output "Installation successful! Exited with $ExitCode."
    }
    else
    {
        Write-Output "Installation failed! Exited with $ExitCode."
        exit $ExitCode
    }
}

Function InstallVisualStudio
{
    Param(
        [String]$WorkLoads,
        [String]$Sku,
        [String]$BootstrapperUrl
    )

    try
    {
        Write-Output 'Downloading Visual Studio...'
        [string]$bootstrapperExe = Join-Path ([System.IO.Path]::GetTempPath()) `
            ([System.IO.Path]::GetRandomFileName() + '.exe')
        curl.exe -L -o $bootstrapperExe $BootstrapperUrl

        Write-Output "Installing Visual Studio..."
        $args = ('/c', $bootstrapperExe, $WorkLoads, '--quiet', '--norestart', '--wait', '--nocache')
        $proc = Start-Process -FilePath cmd.exe -ArgumentList $args -Wait -PassThru
        PrintMsiExitCodeMessage $proc.ExitCode
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
        [String]$Url
    )

    try
    {
        Write-Output "Downloading $Name..."
        [string]$randomRoot = Join-Path ([System.IO.Path]::GetTempPath()) ([System.IO.Path]::GetRandomFileName())
        [string]$msiPath = $randomRoot + '.msi'
        curl.exe -L -o $msiPath $Url

        Write-Output "Installing $Name..."
        $args = @('/i', $msiPath, '/norestart', '/quiet', '/qn')
        $proc = Start-Process -FilePath 'msiexec.exe' -ArgumentList $args -Wait -PassThru
        PrintMsiExitCodeMessage $proc.ExitCode
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
        [String]$Url,
        [String]$Dir
    )

    try
    {
        Write-Output "Downloading $Name..."
        [string]$randomRoot = Join-Path ([System.IO.Path]::GetTempPath()) ([System.IO.Path]::GetRandomFileName())
        [string]$zipPath = $randomRoot + '.zip'
        curl.exe -L -o $zipPath $Url

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
        [String]$Url
    )

    try
    {
        Write-Output 'Downloading LLVM...'
        [string]$randomRoot = Join-Path ([System.IO.Path]::GetTempPath()) ([System.IO.Path]::GetRandomFileName())
        [string]$installerPath = $randomRoot + '.exe'
        curl.exe -L -o $installerPath $Url

        Write-Output 'Installing LLVM...'
        $proc = Start-Process -FilePath $installerPath -ArgumentList @('/S') -NoNewWindow -Wait -PassThru
        PrintMsiExitCodeMessage $proc.ExitCode
    }
    catch
    {
        Write-Output "Failed to install LLVM!"
        Write-Output $_.Exception.Message
        exit -1
    }
}

Function InstallPython
{
    Param(
        [String]$Url
    )

    Write-Output 'Downloading Python...'
    [string]$randomRoot = Join-Path ([System.IO.Path]::GetTempPath()) ([System.IO.Path]::GetRandomFileName())
    [string]$installerPath = $randomRoot + '.exe'
    curl.exe -L -o $installerPath $Url

    Write-Output 'Installing Python...'
    $proc = Start-Process -FilePath $installerPath -ArgumentList `
        @('/passive', 'InstallAllUsers=1', 'PrependPath=1', 'CompileAll=1') -Wait -PassThru
    $exitCode = $proc.ExitCode
    if ($exitCode -eq 0)
    {
        Write-Output 'Installation successful!'
    }
    else
    {
        Write-Output "Installatio failed! Exited with $exitCode."
        exit $exitCode
    }
}

if ([string]::IsNullOrEmpty($PersonalAccessToken) `
    -or ($PersonalAccessToken -eq 'CHANGE THIS') `
    -or ($PersonalAccessToken -eq 'PERSONAL_ACCESS_TOKEN')) {
    Write-Output 'You forgot to fill in your personal access token.'
    exit 1
}

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
InstallZip 'Azure DevOps Agent' $VstsAgentUrl 'C:\agent'
Add-MpPreference -ExclusionPath C:\agent
& 'C:\agent\config.cmd' --unattended --url $AzureDevOpsUrl --auth pat --token $PersonalAccessToken `
    --pool $AzureDevOpsPool --runAsService --work D:\
shutdown /r /t 10
