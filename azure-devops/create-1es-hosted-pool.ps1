# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

<#
.SYNOPSIS
Creates a 1ES Hosted Pool, set up for the STL's CI.

.DESCRIPTION
See https://github.com/microsoft/STL/wiki/Checklist-for-Toolset-Updates for more information.

.PARAMETER Arch
The architecture can be either x64 or arm64.
#>
[CmdletBinding(PositionalBinding=$false)]
Param(
  [Parameter(Mandatory)][ValidateSet('x64', 'arm64')][String]$Arch
)

$ErrorActionPreference = 'Stop'

$CurrentDate = Get-Date
$Timestamp = $CurrentDate.ToString('yyyy-MM-ddTHHmm')

$Location = 'eastus2'

if ($Arch -ieq 'x64') {
  $VMSize = 'Standard_F32as_v6'
  $PoolSize = 64
  $ImagePublisher = 'MicrosoftWindowsServer'
  $ImageOffer = 'WindowsServer'
  $ImageSku = '2025-datacenter-azure-edition'
} else {
  $VMSize = 'Standard_D32ps_v6'
  $PoolSize = 32
  $ImageId = '/SharedGalleries/WindowsServer.1P/Images/2025-datacenter-azure-edition-arm64/Versions/latest'
}

$ProtoVMName = 'PROTOTYPE'

$LogFile = "1es-hosted-pool-$Timestamp-$Arch.log"
$ProgressActivity = 'Preparing STL CI pool'
$TotalProgress = 38
$CurrentProgress = 1

<#
.SYNOPSIS
Displays an updated progress bar.

.DESCRIPTION
Display-ProgressBar increments $CurrentProgress and displays $Status in the progress bar.

.PARAMETER Status
A message describing the current operation being performed.
#>
function Display-ProgressBar {
  [CmdletBinding(PositionalBinding=$false)]
  Param([Parameter(Mandatory)][string]$Status)

  Write-Progress `
    -Activity $ProgressActivity `
    -Status $Status `
    -PercentComplete (100 * $script:CurrentProgress++ / $TotalProgress)
}

<#
.SYNOPSIS
Generates a random password.

.DESCRIPTION
New-Password generates a password, randomly, of length $Length, containing
only alphanumeric characters, underscore, and dash.

.PARAMETER Length
The length of the returned password.
#>
function New-Password {
  [CmdletBinding(PositionalBinding=$false)]
  Param([int]$Length = 32)

  # This 64-character alphabet generates 6 bits of entropy per character.
  $alphabet = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_-'.ToCharArray()

  [SecureString] $result = [SecureString]::new()
  for ($idx = 0; $idx -lt $Length; $idx++) {
    $result.AppendChar((Get-SecureRandom -InputObject $alphabet))
  }

  return $result
}

####################################################################################################
Display-ProgressBar -Status 'Silencing breaking change warnings'

# https://aka.ms/azps-changewarnings
$Env:SuppressAzurePowerShellBreakingChangeWarnings = 'true'

Update-AzConfig `
  -DisplayBreakingChangeWarning $false `
  -Scope 'Process' >> $LogFile

####################################################################################################
Display-ProgressBar -Status 'Getting the subscription context'

if ((Get-AzContext).Subscription.Name -cne 'CPP_STL_GitHub') {
  Write-Error 'Please sign in with `Connect-AzAccount -Subscription ''CPP_STL_GitHub''` before running this script.'
}

####################################################################################################
Display-ProgressBar -Status 'Creating resource group'

$ResourceGroupName = "Stl-$Timestamp-$Arch"

New-AzResourceGroup `
  -Name $ResourceGroupName `
  -Location $Location >> $LogFile

####################################################################################################
Display-ProgressBar -Status 'Creating credentials'

$AdminPWSecure = New-Password
$Credential = New-Object System.Management.Automation.PSCredential ('AdminUser', $AdminPWSecure)

####################################################################################################
Display-ProgressBar -Status 'Creating public IP address'

$PublicIpAddressName = "$ResourceGroupName-PublicIpAddress"
$PublicIpAddress = New-AzPublicIpAddress `
  -Name $PublicIpAddressName `
  -ResourceGroupName $ResourceGroupName `
  -Location $Location `
  -Sku 'Standard' `
  -AllocationMethod 'Static'

####################################################################################################
Display-ProgressBar -Status 'Creating NAT gateway'

$NatGatewayName = "$ResourceGroupName-NatGateway"
$NatGateway = New-AzNatGateway `
  -Name $NatGatewayName `
  -ResourceGroupName $ResourceGroupName `
  -Location $Location `
  -IdleTimeoutInMinutes 10 `
  -Sku 'Standard' `
  -PublicIpAddress $PublicIpAddress

####################################################################################################
Display-ProgressBar -Status 'Creating network security group'

$NetworkSecurityGroupName = "$ResourceGroupName-NetworkSecurity"
$NetworkSecurityGroup = New-AzNetworkSecurityGroup `
  -Name $NetworkSecurityGroupName `
  -ResourceGroupName $ResourceGroupName `
  -Location $Location

####################################################################################################
Display-ProgressBar -Status 'Creating virtual network subnet config'

# TRANSITION, 2026-03-31: "After March 31, 2026, new virtual networks will default to using private subnets,
# meaning that an explicit outbound method must be enabled in order to reach public endpoints on the Internet
# and within Microsoft."
# https://learn.microsoft.com/en-us/azure/virtual-network/ip-services/default-outbound-access
# We're using `-DefaultOutboundAccess $false` to opt-in early.
$SubnetName = "$ResourceGroupName-Subnet"
$Subnet = New-AzVirtualNetworkSubnetConfig `
  -Name $SubnetName `
  -AddressPrefix '10.0.0.0/16' `
  -DefaultOutboundAccess $false `
  -NatGateway $NatGateway `
  -NetworkSecurityGroup $NetworkSecurityGroup

####################################################################################################
Display-ProgressBar -Status 'Creating virtual network'

$VirtualNetworkName = "$ResourceGroupName-Network"
$VirtualNetwork = New-AzVirtualNetwork `
  -Name $VirtualNetworkName `
  -ResourceGroupName $ResourceGroupName `
  -Location $Location `
  -AddressPrefix '10.0.0.0/16' `
  -Subnet $Subnet

####################################################################################################
Display-ProgressBar -Status 'Creating network interface'

$NicName = "$ResourceGroupName-NIC"
$Nic = New-AzNetworkInterface `
  -Name $NicName `
  -ResourceGroupName $ResourceGroupName `
  -Location $Location `
  -Subnet $VirtualNetwork.Subnets[0]

####################################################################################################
Display-ProgressBar -Status 'Creating prototype VM config'

if ($Arch -ieq 'x64') {
  $VM = New-AzVMConfig `
    -VMName $ProtoVMName `
    -VMSize $VMSize `
    -DiskControllerType 'NVMe' `
    -Priority 'Regular'
} else {
  $VM = New-AzVMConfig `
    -VMName $ProtoVMName `
    -VMSize $VMSize `
    -DiskControllerType 'SCSI' `
    -Priority 'Regular' `
    -SecurityType 'TrustedLaunch' `
    -SharedGalleryImageId $ImageId
}

####################################################################################################
Display-ProgressBar -Status 'Setting prototype VM OS'

$VM = Set-AzVMOperatingSystem `
  -VM $VM `
  -Windows `
  -ComputerName $ProtoVMName `
  -Credential $Credential `
  -ProvisionVMAgent

####################################################################################################
Display-ProgressBar -Status 'Adding prototype VM network interface'

$VM = Add-AzVMNetworkInterface `
  -VM $VM `
  -Id $Nic.Id

####################################################################################################
Display-ProgressBar -Status 'Setting prototype VM source image'

if ($Arch -ieq 'x64') {
  $VM = Set-AzVMSourceImage `
    -VM $VM `
    -PublisherName $ImagePublisher `
    -Offer $ImageOffer `
    -Skus $ImageSku `
    -Version 'latest'
} else {
  # We passed -SharedGalleryImageId to New-AzVMConfig above.
}

####################################################################################################
Display-ProgressBar -Status 'Setting prototype VM boot diagnostic'

$VM = Set-AzVMBootDiagnostic `
  -VM $VM `
  -Disable

####################################################################################################
Display-ProgressBar -Status 'Creating prototype VM'

New-AzVm `
  -ResourceGroupName $ResourceGroupName `
  -Location $Location `
  -VM $VM >> $LogFile

####################################################################################################
Display-ProgressBar -Status 'Getting prototype VM OS disk name'

$VM = Get-AzVM `
  -ResourceGroupName $ResourceGroupName `
  -Name $ProtoVMName

$PrototypeOSDiskName = $VM.StorageProfile.OsDisk.Name

####################################################################################################
Display-ProgressBar -Status 'Running provision-image.ps1 in VM'

$ProvisionImageResult = Invoke-AzVMRunCommand `
  -ResourceId $VM.ID `
  -CommandId 'RunPowerShellScript' `
  -ScriptPath "$PSScriptRoot\provision-image.ps1" `
  -Parameter @{ 'Arch' = $Arch; }

Write-Host $ProvisionImageResult.value.Message

if ($ProvisionImageResult.value.Message -cnotmatch 'PROVISION_IMAGE_SUCCEEDED') {
  Write-Host 'provision-image.ps1 failed, stopping VM...'

  Stop-AzVM `
    -Id $VM.ID `
    -Force >> $LogFile

  Write-Error "VM stopped. Remember to delete unusable resource group: $ResourceGroupName"
}

####################################################################################################
Display-ProgressBar -Status 'Restarting VM'

Restart-AzVM `
  -Id $VM.ID >> $LogFile

####################################################################################################
Display-ProgressBar -Status 'Sleeping after restart'

# The VM appears to be busy immediately after restarting.
# This workaround waits for a minute before attempting to run sysprep.
Start-Sleep -Seconds 60

####################################################################################################
Display-ProgressBar -Status 'Running sysprep in VM'

Invoke-AzVMRunCommand `
  -ResourceId $VM.ID `
  -CommandId 'RunPowerShellScript' `
  -ScriptString 'C:\Windows\system32\sysprep\sysprep.exe /oobe /generalize /mode:vm /shutdown' >> $LogFile

####################################################################################################
Display-ProgressBar -Status 'Waiting for VM to shut down'

while ('PowerState/stopped' -notin (Get-AzVM -ResourceId $VM.ID -Status).Statuses.Code) {
  Start-Sleep -Seconds 10
}

####################################################################################################
Display-ProgressBar -Status 'Stopping VM'

Stop-AzVM `
  -Id $VM.ID `
  -Force >> $LogFile

####################################################################################################
Display-ProgressBar -Status 'Generalizing VM'

Set-AzVM `
  -Id $VM.ID `
  -Generalized >> $LogFile

####################################################################################################
Display-ProgressBar -Status 'Creating gallery'

$GalleryName = "$ResourceGroupName-Gallery" -replace '-', '_'
$Gallery = New-AzGallery `
  -Location $Location `
  -ResourceGroupName $ResourceGroupName `
  -Name $GalleryName

####################################################################################################
Display-ProgressBar -Status 'Granting access to 1ES Resource Management'

$ServicePrincipalObjectId = (Get-AzADServicePrincipal -DisplayName '1ES Resource Management' -First 1).Id

New-AzRoleAssignment `
  -ObjectId $ServicePrincipalObjectId `
  -RoleDefinitionName 'Reader' `
  -Scope $Gallery.Id >> $LogFile

####################################################################################################
Display-ProgressBar -Status 'Creating image definition'

$ImageDefinitionName = "$ResourceGroupName-ImageDefinition"
$FeatureTrustedLaunch = @{ Name = 'SecurityType'; Value = 'TrustedLaunch'; }
if ($Arch -ieq 'x64') {
  $FeatureNVMe = @{ Name = 'DiskControllerTypes'; Value = 'SCSI, NVMe'; }
} else {
  $FeatureNVMe = @{ Name = 'DiskControllerTypes'; Value = 'SCSI'; }
}
$ImageDefinitionFeatures = @($FeatureTrustedLaunch, $FeatureNVMe)
New-AzGalleryImageDefinition `
  -Location $Location `
  -ResourceGroupName $ResourceGroupName `
  -GalleryName $GalleryName `
  -Name $ImageDefinitionName `
  -OsState 'Generalized' `
  -OsType 'Windows' `
  -Publisher 'StlPublisher' `
  -Offer 'StlOffer' `
  -Sku 'StlSku' `
  -Architecture $Arch `
  -Feature $ImageDefinitionFeatures `
  -HyperVGeneration 'V2' >> $LogFile

####################################################################################################
Display-ProgressBar -Status 'Creating image version'

$ImageVersionName = $CurrentDate.ToString('yyyyMMdd.HHmm.0')
$ImageVersion = New-AzGalleryImageVersion `
  -Location $Location `
  -ResourceGroupName $ResourceGroupName `
  -GalleryName $GalleryName `
  -GalleryImageDefinitionName $ImageDefinitionName `
  -Name $ImageVersionName `
  -SourceImageVMId $VM.ID

####################################################################################################
Display-ProgressBar -Status 'Registering CloudTest resource provider'

Register-AzResourceProvider `
  -ProviderNamespace 'Microsoft.CloudTest' >> $LogFile

####################################################################################################
Display-ProgressBar -Status 'Creating 1ES image'

$ImageName = "$ResourceGroupName-Image"
New-AzResource `
  -Location $Location `
  -ResourceGroupName $ResourceGroupName `
  -ResourceType 'Microsoft.CloudTest/Images' `
  -ResourceName $ImageName `
  -Properties @{ 'imageType' = 'SharedImageGallery'; 'resourceId' = $ImageVersion.Id; } `
  -Force >> $LogFile

####################################################################################################
Display-ProgressBar -Status 'Creating 1ES Hosted Pool'

$PoolName = "$ResourceGroupName-Pool"

$PoolProperties = @{
  'organization' = 'https://dev.azure.com/vclibs'
  'projects' = @('STL')
  'sku' = @{ 'name' = $VMSize; 'tier' = 'StandardSSD'; 'enableSpot' = $false; }
  'images' = @(@{ 'imageName' = $ImageName; 'poolBufferPercentage' = '100'; })
  'maxPoolSize' = $PoolSize
  'agentProfile' = @{ 'type' = 'Stateless'; }
}

New-AzResource `
  -Location $Location `
  -ResourceGroupName $ResourceGroupName `
  -ResourceType 'Microsoft.CloudTest/hostedpools' `
  -ResourceName $PoolName `
  -Properties $PoolProperties `
  -Force >> $LogFile

####################################################################################################
Display-ProgressBar -Status 'Deleting unused VM'

Remove-AzVM `
  -Id $VM.ID `
  -Force >> $LogFile

####################################################################################################
Display-ProgressBar -Status 'Deleting unused disk'

Remove-AzDisk `
  -ResourceGroupName $ResourceGroupName `
  -DiskName $PrototypeOSDiskName `
  -Force >> $LogFile

####################################################################################################
Display-ProgressBar -Status 'Deleting unused network interface'

Remove-AzNetworkInterface `
  -ResourceGroupName $ResourceGroupName `
  -Name $NicName `
  -Force >> $LogFile

####################################################################################################
Display-ProgressBar -Status 'Deleting unused virtual network'

Remove-AzVirtualNetwork `
  -ResourceGroupName $ResourceGroupName `
  -Name $VirtualNetworkName `
  -Force >> $LogFile

####################################################################################################
Display-ProgressBar -Status 'Deleting unused network security group'

Remove-AzNetworkSecurityGroup `
  -ResourceGroupName $ResourceGroupName `
  -Name $NetworkSecurityGroupName `
  -Force >> $LogFile

####################################################################################################
Display-ProgressBar -Status 'Deleting unused NAT gateway'

Remove-AzNatGateway `
  -ResourceGroupName $ResourceGroupName `
  -Name $NatGatewayName `
  -Force >> $LogFile

####################################################################################################
Display-ProgressBar -Status 'Deleting unused public IP address'

Remove-AzPublicIpAddress `
  -ResourceGroupName $ResourceGroupName `
  -Name $PublicIpAddressName `
  -Force >> $LogFile

####################################################################################################
Write-Progress -Activity $ProgressActivity -Completed

Write-Host "Elapsed time: $(((Get-Date) - $CurrentDate).ToString('hh\:mm\:ss'))"

if ((Get-AzResource -ResourceGroupName $ResourceGroupName -Name $PoolName) -ne $null) {
  Write-Host "Created pool: $PoolName"
} else {
  Write-Error "Failed to create pool: $PoolName"
}
