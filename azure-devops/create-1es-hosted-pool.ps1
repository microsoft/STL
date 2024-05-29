# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

<#
.SYNOPSIS
Creates a 1ES Hosted Pool, set up for the STL's CI.

.DESCRIPTION
See https://github.com/microsoft/STL/wiki/Checklist-for-Toolset-Updates for more information.
#>

$ErrorActionPreference = 'Stop'

$CurrentDate = Get-Date

$Location = 'eastus'
$VMSize = 'Standard_D32ads_v5'
$ProtoVMName = 'PROTOTYPE'
$ImagePublisher = 'MicrosoftWindowsServer'
$ImageOffer = 'WindowsServer'
$ImageSku = '2022-datacenter-g2'

$ProgressActivity = 'Preparing STL CI pool'
$TotalProgress = 26
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
  -Scope 'Process' | Out-Null

####################################################################################################
Display-ProgressBar -Status 'Setting the subscription context'

Set-AzContext `
  -SubscriptionName 'CPP_STL_GitHub' | Out-Null

####################################################################################################
Display-ProgressBar -Status 'Creating resource group'

$ResourceGroupName = 'StlBuild-' + $CurrentDate.ToString('yyyy-MM-ddTHHmm')

New-AzResourceGroup `
  -Name $ResourceGroupName `
  -Location $Location | Out-Null

####################################################################################################
Display-ProgressBar -Status 'Creating credentials'

$AdminPWSecure = New-Password
$Credential = New-Object System.Management.Automation.PSCredential ('AdminUser', $AdminPWSecure)

####################################################################################################
Display-ProgressBar -Status 'Creating virtual network'

$NetworkSecurityGroupName = $ResourceGroupName + '-NetworkSecurity'
$NetworkSecurityGroup = New-AzNetworkSecurityGroup `
  -Name $NetworkSecurityGroupName `
  -ResourceGroupName $ResourceGroupName `
  -Location $Location

$SubnetName = $ResourceGroupName + '-Subnet'
$Subnet = New-AzVirtualNetworkSubnetConfig `
  -Name $SubnetName `
  -AddressPrefix '10.0.0.0/16' `
  -NetworkSecurityGroup $NetworkSecurityGroup

$VirtualNetworkName = $ResourceGroupName + '-Network'
$VirtualNetwork = New-AzVirtualNetwork `
  -Name $VirtualNetworkName `
  -ResourceGroupName $ResourceGroupName `
  -Location $Location `
  -AddressPrefix '10.0.0.0/16' `
  -Subnet $Subnet

####################################################################################################
Display-ProgressBar -Status 'Creating network interface'

$NicName = $ResourceGroupName + '-NIC'
$Nic = New-AzNetworkInterface `
  -Name $NicName `
  -ResourceGroupName $ResourceGroupName `
  -Location $Location `
  -Subnet $VirtualNetwork.Subnets[0]

####################################################################################################
Display-ProgressBar -Status 'Creating prototype VM'

# Previously: -Priority 'Spot'
$VM = New-AzVMConfig `
  -VMName $ProtoVMName `
  -VMSize $VMSize `
  -Priority 'Regular'

$VM = Set-AzVMOperatingSystem `
  -VM $VM `
  -Windows `
  -ComputerName $ProtoVMName `
  -Credential $Credential `
  -ProvisionVMAgent

$VM = Add-AzVMNetworkInterface `
  -VM $VM `
  -Id $Nic.Id

$VM = Set-AzVMSourceImage `
  -VM $VM `
  -PublisherName $ImagePublisher `
  -Offer $ImageOffer `
  -Skus $ImageSku `
  -Version 'latest'

$VM = Set-AzVMBootDiagnostic `
  -VM $VM `
  -Disable

New-AzVm `
  -ResourceGroupName $ResourceGroupName `
  -Location $Location `
  -VM $VM | Out-Null

$VM = Get-AzVM `
  -ResourceGroupName $ResourceGroupName `
  -Name $ProtoVMName

$PrototypeOSDiskName = $VM.StorageProfile.OsDisk.Name

####################################################################################################
Display-ProgressBar -Status 'Running provision-image.ps1 in VM'

$ProvisionImageResult = Invoke-AzVMRunCommand `
  -ResourceId $VM.ID `
  -CommandId 'RunPowerShellScript' `
  -ScriptPath "$PSScriptRoot\provision-image.ps1"

Write-Host $ProvisionImageResult.value.Message

if ($ProvisionImageResult.value.Message -cnotmatch 'PROVISION_IMAGE_SUCCEEDED') {
  Write-Host 'provision-image.ps1 failed, stopping VM...'

  Stop-AzVM `
    -Id $VM.ID `
    -Force | Out-Null

  Write-Error "VM stopped. Remember to delete unusable resource group: $ResourceGroupName"
}

####################################################################################################
Display-ProgressBar -Status 'Restarting VM'

Restart-AzVM `
  -Id $VM.ID | Out-Null

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
  -ScriptString 'C:\Windows\system32\sysprep\sysprep.exe /oobe /generalize /mode:vm /shutdown' | Out-Null

####################################################################################################
Display-ProgressBar -Status 'Waiting for VM to shut down'

while ('PowerState/stopped' -notin (Get-AzVM -ResourceId $VM.ID -Status).Statuses.Code) {
  Start-Sleep -Seconds 10
}

####################################################################################################
Display-ProgressBar -Status 'Stopping VM'

Stop-AzVM `
  -Id $VM.ID `
  -Force | Out-Null

####################################################################################################
Display-ProgressBar -Status 'Generalizing VM'

Set-AzVM `
  -Id $VM.ID `
  -Generalized | Out-Null

####################################################################################################
Display-ProgressBar -Status 'Creating gallery'

$GalleryName = 'StlBuild_' + $CurrentDate.ToString('yyyy_MM_ddTHHmm') + '_Gallery'
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
  -Scope $Gallery.Id | Out-Null

####################################################################################################
Display-ProgressBar -Status 'Creating image definition'

$ImageDefinitionName = $ResourceGroupName + '-ImageDefinition'
New-AzGalleryImageDefinition `
  -Location $Location `
  -ResourceGroupName $ResourceGroupName `
  -GalleryName $GalleryName `
  -Name $ImageDefinitionName `
  -OsState 'Generalized' `
  -OsType 'Windows' `
  -Publisher $ImagePublisher `
  -Offer $ImageOffer `
  -Sku $ImageSku `
  -Feature @(@{ Name = 'SecurityType'; Value = 'TrustedLaunch'; }) `
  -HyperVGeneration 'V2' | Out-Null

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
  -ProviderNamespace 'Microsoft.CloudTest' | Out-Null

####################################################################################################
Display-ProgressBar -Status 'Creating 1ES image'

$ImageName = $ResourceGroupName + '-Image'
New-AzResource `
  -Location $Location `
  -ResourceGroupName $ResourceGroupName `
  -ResourceType 'Microsoft.CloudTest/Images' `
  -ResourceName $ImageName `
  -Properties @{ 'imageType' = 'SharedImageGallery'; 'resourceId' = $ImageVersion.Id; } `
  -Force | Out-Null

####################################################################################################
Display-ProgressBar -Status 'Creating 1ES Hosted Pool'

$PoolName = $ResourceGroupName + '-Pool'

$PoolProperties = @{
  'organization' = 'https://dev.azure.com/vclibs'
  'projects' = @('STL')
  'sku' = @{ 'name' = $VMSize; 'tier' = 'StandardSSD'; 'enableSpot' = $false; }
  'images' = @(@{ 'imageName' = $ImageName; 'poolBufferPercentage' = '100'; })
  'maxPoolSize' = 64
  'agentProfile' = @{ 'type' = 'Stateless'; }
}

New-AzResource `
  -Location $Location `
  -ResourceGroupName $ResourceGroupName `
  -ResourceType 'Microsoft.CloudTest/hostedpools' `
  -ResourceName $PoolName `
  -Properties $PoolProperties `
  -Force | Out-Null

####################################################################################################
Display-ProgressBar -Status 'Deleting unused VM'

Remove-AzVM `
  -Id $VM.ID `
  -Force | Out-Null

####################################################################################################
Display-ProgressBar -Status 'Deleting unused disk'

Remove-AzDisk `
  -ResourceGroupName $ResourceGroupName `
  -DiskName $PrototypeOSDiskName `
  -Force | Out-Null

####################################################################################################
Display-ProgressBar -Status 'Deleting unused network interface'

Remove-AzNetworkInterface `
-ResourceGroupName $ResourceGroupName `
-Name $NicName `
-Force | Out-Null

####################################################################################################
Display-ProgressBar -Status 'Deleting unused virtual network'

Remove-AzVirtualNetwork `
-ResourceGroupName $ResourceGroupName `
-Name $VirtualNetworkName `
-Force | Out-Null

####################################################################################################
Display-ProgressBar -Status 'Deleting unused network security group'

Remove-AzNetworkSecurityGroup `
-ResourceGroupName $ResourceGroupName `
-Name $NetworkSecurityGroupName `
-Force | Out-Null

####################################################################################################
Write-Progress -Activity $ProgressActivity -Completed

Write-Host "Elapsed time: $(((Get-Date) - $CurrentDate).ToString('hh\:mm\:ss'))"

if ((Get-AzResource -ResourceGroupName $ResourceGroupName -Name $PoolName) -ne $null) {
  Write-Host "Created pool: $PoolName"
} else {
  Write-Error "Failed to create pool: $PoolName"
}
