$ErrorActionPreference = "Stop"

Function InstallVS
{
    # Microsoft hosted agents do not have the required MSVC 14.23 for building MSVC STL. 
    # This step installs MSVC 14.23, only on Microsoft hosted agents.

  Param(
    [String]$WorkLoads,
    [String]$Sku,
    [String]$VSBootstrapperURL)
  $exitCode = -1
  try
  {
    Write-Host "Downloading bootstrapper ..."
    [string]$bootstrapperExe = Join-Path ${env:Temp} ([System.IO.Path]::GetRandomFileName() + ".exe")
    Invoke-WebRequest -Uri $VSBootstrapperURL -OutFile $bootstrapperExe

    $Arguments = ('/c', $bootstrapperExe, $WorkLoads, '--quiet', '--norestart', '--wait', '--nocache' )

    Write-Host "Starting Install: $Arguments"
    $process = Start-Process -FilePath cmd.exe -ArgumentList $Arguments -Wait -PassThru
    $exitCode = $process.ExitCode

    if ($exitCode -eq 0 -or $exitCode -eq 3010)
    {
      Write-Host -Object 'Installation successful!'
    }
    else
    {
      Write-Host -Object "Non zero exit code returned by the installation process : $exitCode."
    }
  }
  catch
  {
    Write-Host -Object "Failed to install Visual Studio!"
    Write-Host -Object $_.Exception.Message
    exit $exitCode
  }

  exit $exitCode
}

# Invalidate the standard installation of VS on the hosted agent.
Move-Item "C:/Program Files (x86)/Microsoft Visual Studio/2019/Enterprise/" "C:/Program Files (x86)/Microsoft Visual Studio/2019/nouse/" -Verbose

$WorkLoads =  '--add Microsoft.VisualStudio.Component.VC.CLI.Support ' + `
              '--add Microsoft.VisualStudio.Component.VC.Runtimes.ARM.Spectre ' + `
              '--add Microsoft.VisualStudio.Component.VC.Runtimes.ARM64.Spectre ' + `
              '--add Microsoft.VisualStudio.Component.VC.Runtimes.x86.x64.Spectre ' + `
              '--add Microsoft.VisualStudio.Component.VC.Tools.x86.x64 ' + `
              '--add Microsoft.VisualStudio.Component.VC.Tools.ARM64 ' + `
              '--add Microsoft.VisualStudio.Component.VC.Tools.ARM ' + `
              '--add Microsoft.VisualStudio.Component.Windows10SDK.18362 '
     
$ReleaseInPath = 'Preview'
$Sku = 'Enterprise'
$VSBootstrapperURL = 'https://aka.ms/vs/16/pre/vs_buildtools.exe'

$ErrorActionPreference = 'Stop'

# Install VS
$exitCode = InstallVS -WorkLoads $WorkLoads -Sku $Sku -VSBootstrapperURL $VSBootstrapperURL

exit $exitCode