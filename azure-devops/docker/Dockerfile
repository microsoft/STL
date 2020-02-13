# escape=`
FROM mcr.microsoft.com/windows/servercore:1803-KB4516058-amd64
SHELL ["cmd", "/S", "/C"]
ADD https://aka.ms/vs/16/pre/vs_buildtools.exe C:\TEMP\vs_buildtools.exe
RUN C:\TEMP\vs_buildtools.exe --quiet --wait --norestart --nocache `
    --installPath C:\BuildTools `
    --add Microsoft.VisualStudio.Component.VC.CLI.Support `
    --add Microsoft.VisualStudio.Component.VC.Tools.x86.x64 `
    --add Microsoft.VisualStudio.Component.VC.Tools.ARM64 `
    --add Microsoft.VisualStudio.Component.VC.Tools.ARM `
    --add Microsoft.VisualStudio.Component.Windows10SDK.18362 `
    || IF "%ERRORLEVEL%"=="3010" EXIT 0
# Install chocolatey, and set $env:PATh for choco and cmake.
RUN @"%SystemRoot%\System32\WindowsPowerShell\v1.0\powershell.exe" `
    -NoProfile -InputFormat None -ExecutionPolicy Bypass -Command `
    "iex ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))" `
    && SETX PATH "%PATH%;%ALLUSERSPROFILE%\chocolatey\bin;%PROGRAMFILES%\cmake\bin"
RUN choco install git cmake llvm -y -v
CMD ["powershell.exe", "-NoLogo", "-ExecutionPolicy", "Bypass"]
