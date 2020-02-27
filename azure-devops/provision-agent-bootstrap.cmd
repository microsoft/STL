:: Copyright (c) Microsoft Corporation.
:: SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

:: Kicks off provision-agent.ps1 after switching to "AdminUser" account
:: %1 AdminUser password
:: %2 Azure Pipelines PAT

curl.exe -L -o "%TEMP%\psexec.exe" https://live.sysinternals.com/PsExec64.exe
"%TEMP%\psexec.exe" -u AdminUser -p %1 -accepteula -h C:\Windows\System32\WindowsPowerShell\v1.0\powershell.exe -ExecutionPolicy Unrestricted -File "%CD%\provision-agent.ps1" %2
