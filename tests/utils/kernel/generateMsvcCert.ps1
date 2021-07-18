# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

param([string]$out="MsvcStlTestingCert.pfx",[string]$pass="placeholderPassword")

$ErrorActionPreference = 'Stop'

$isAdmin = ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole] "Administrator")
if (!$isAdmin) {
        Throw "The current prompt is not an elevated administrator prompt!"
}

# Clean up old certificates
Get-ChildItem cert:\localmachine\My, cert:\localmachine\root, cert:\localmachine\trustedpublisher |
Where-Object { $_.Subject -eq 'CN=MsvcStlTestingCert' } |
Remove-Item

# Make the new cert
$cert = New-SelfSignedCertificate -Type CodeSigningCert -DnsName "MsvcStlTestingCert" `
        -certstorelocation cert:\localmachine\my -NotAfter (Get-Date).AddDays(2)
$path = 'cert:\localMachine\my\' + $cert.thumbprint
$pwd = ConvertTo-SecureString -String $pass -Force -AsPlainText
Export-PfxCertificate -cert $path -FilePath $out -Password $pwd

# install the cert so that we can load our drivers
Import-PfxCertificate -FilePath $out -CertStoreLocation cert:\localmachine\root -Password $pwd
Import-PfxCertificate -FilePath $out -CertStoreLocation cert:\localmachine\trustedpublisher -Password $pwd
