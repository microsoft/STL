# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# Downloads provision-image.ps1, switches to the "AdminUser" account, then runs provision-image.ps1
param(
  [string]$AdminUserPassword
)

if ([string]::IsNullOrEmpty($AdminUserPassword)) {
  Write-Output "Missing AdminUser password."
  exit 1
}

$temp = $env:TEMP

curl.exe -L -o "$temp\psexec.exe" https://live.sysinternals.com/PsExec64.exe
curl.exe -L -o "$temp\provision-image.ps1" https://raw.githubusercontent.com/microsoft/STL/master/azure-devops/provision-image.ps1
& "$temp\psexec.exe" -u AdminUser -p $AdminUserPassword -accepteula -h C:\Windows\System32\WindowsPowerShell\v1.0\powershell.exe -ExecutionPolicy Unrestricted -File "$temp\provision-image.ps1"
