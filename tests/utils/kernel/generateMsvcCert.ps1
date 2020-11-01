param([string]$out="MsvcStlTestingCert.pfx",[string]$pass="foo")

$ErrorActionPreference = 'Stop'
# Clean up old certificates
Get-ChildItem cert:\localmachine\My |
Where-Object { $_.Subject -eq 'CN=MsvcStlTestingCert' } |
Remove-Item

Get-ChildItem cert:\localmachine\root |
Where-Object { $_.Subject -eq 'CN=MsvcStlTestingCert' } |
Remove-Item

Get-ChildItem cert:\localmachine\trustedpublisher |
Where-Object { $_.Subject -eq 'CN=MsvcStlTestingCert' } |
Remove-Item

#Make the new cert
$cert = New-SelfSignedCertificate -Type CodeSigningCert -DnsName "MsvcStlTestingCert" -certstorelocation cert:\localmachine\my -NotAfter (Get-Date).AddDays(2)
$path = 'cert:\localMachine\my\' + $cert.thumbprint
$pwd = ConvertTo-SecureString -String $pass -Force -AsPlainText
Export-PfxCertificate -cert $path -FilePath $out -Password $pwd

# install the cert so that we can load our drivers
Import-PfxCertificate -FilePath $out -CertStoreLocation cert:\localmachine\root -Password $pwd
Import-PfxCertificate -FilePath $out -CertStoreLocation cert:\localmachine\trustedpublisher -Password $pwd


