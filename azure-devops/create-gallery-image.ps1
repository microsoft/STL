# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

<#
.SYNOPSIS
Creates an Azure Compute Gallery image, set up for the STL's CI.

.DESCRIPTION
See https://github.com/microsoft/STL/wiki/Checklist-for-Toolset-Updates for more information.
#>

$ErrorActionPreference = 'Stop'

# https://aka.ms/azps-changewarnings
$Env:SuppressAzurePowerShellBreakingChangeWarnings = 'true'

$CurrentDate = Get-Date

$Location = 'eastus'
$VMSize = 'Standard_D32ads_v5'
$ProtoVMName = 'PROTOTYPE'
$ImagePublisher = 'MicrosoftWindowsServer'
$ImageOffer = 'WindowsServer'
$ImageSku = '2022-datacenter-g2'

$ProgressActivity = 'Creating Gallery Image'
$TotalProgress = 16
$CurrentProgress = 1

<#
.SYNOPSIS
Displays an updated progress bar.

.DESCRIPTION
Display-Progress-Bar increments $CurrentProgress and displays $Status in the progress bar.

.PARAMETER Status
A message describing the current operation being performed.
#>
function Display-Progress-Bar {
  [CmdletBinding()]
  Param([string]$Status)

  Write-Progress `
    -Activity $ProgressActivity `
    -Status $Status `
    -PercentComplete (100 / $TotalProgress * $script:CurrentProgress++)
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
  Param ([int] $Length = 32)

  # This 64-character alphabet generates 6 bits of entropy per character.
  # The power-of-2 alphabet size allows us to select a character by masking a random Byte with bitwise-AND.
  $alphabet = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_-'
  $mask = 63
  if ($alphabet.Length -ne 64) {
    throw 'Bad alphabet length'
  }

  [Byte[]]$randomData = [Byte[]]::new($Length)
  $rng = $null
  try {
    $rng = [System.Security.Cryptography.RandomNumberGenerator]::Create()
    $rng.GetBytes($randomData)
  }
  finally {
    if ($null -ne $rng) {
      $rng.Dispose()
    }
  }

  $result = ''
  for ($idx = 0; $idx -lt $Length; $idx++) {
    $result += $alphabet[$randomData[$idx] -band $mask]
  }

  return $result
}

<#
.SYNOPSIS
Waits for the shutdown of the specified resource.

.DESCRIPTION
Wait-Shutdown takes a VM, and checks if there's a 'PowerState/stopped'
code; if there is, it returns. If there isn't, it waits 10 seconds and
tries again.

.PARAMETER ResourceGroupName
The name of the resource group to look up the VM in.

.PARAMETER Name
The name of the virtual machine to wait on.
#>
function Wait-Shutdown {
  [CmdletBinding()]
  Param([string]$ResourceGroupName, [string]$Name)

  Write-Host "Waiting for $Name to stop..."
  while ($true) {
    $Vm = Get-AzVM -ResourceGroupName $ResourceGroupName -Name $Name -Status
    $highestStatus = $Vm.Statuses.Count
    for ($idx = 0; $idx -lt $highestStatus; $idx++) {
      if ($Vm.Statuses[$idx].Code -eq 'PowerState/stopped') {
        return
      }
    }

    Write-Host '... not stopped yet, sleeping for 10 seconds'
    Start-Sleep -Seconds 10
  }
}


####################################################################################################
Display-Progress-Bar -Status 'Setting the subscription context'

Set-AzContext `
  -SubscriptionName CPP_STL_GitHub | Out-Null

####################################################################################################
Display-Progress-Bar -Status 'Creating resource group'

$ResourceGroupName = 'StlBuild-' + $CurrentDate.ToString('yyyy-MM-dd-THHmm')
$AdminPW = New-Password
# TRANSITION, this opt-in tag should be unnecessary after 2022-09-30.
$SimplySecureV2OptInTag = @{'NRMSV2OptIn'=$CurrentDate.ToString('yyyyMMdd')}

New-AzResourceGroup `
  -Name $ResourceGroupName `
  -Location $Location `
  -Tag $SimplySecureV2OptInTag | Out-Null

$AdminPWSecure = ConvertTo-SecureString $AdminPW -AsPlainText -Force
$Credential = New-Object System.Management.Automation.PSCredential ('AdminUser', $AdminPWSecure)

####################################################################################################
Display-Progress-Bar -Status 'Creating virtual network'

$allowHttp = New-AzNetworkSecurityRuleConfig `
  -Name AllowHTTP `
  -Description 'Allow HTTP(S)' `
  -Access Allow `
  -Protocol Tcp `
  -Direction Outbound `
  -Priority 1000 `
  -SourceAddressPrefix * `
  -SourcePortRange * `
  -DestinationAddressPrefix * `
  -DestinationPortRange @(80, 443)

$allowQuic = New-AzNetworkSecurityRuleConfig `
  -Name AllowQUIC `
  -Description 'Allow QUIC' `
  -Access Allow `
  -Protocol Udp `
  -Direction Outbound `
  -Priority 1010 `
  -SourceAddressPrefix * `
  -SourcePortRange * `
  -DestinationAddressPrefix * `
  -DestinationPortRange 443

$allowDns = New-AzNetworkSecurityRuleConfig `
  -Name AllowDNS `
  -Description 'Allow DNS' `
  -Access Allow `
  -Protocol * `
  -Direction Outbound `
  -Priority 1020 `
  -SourceAddressPrefix * `
  -SourcePortRange * `
  -DestinationAddressPrefix * `
  -DestinationPortRange 53

$denyEverythingElse = New-AzNetworkSecurityRuleConfig `
  -Name DenyElse `
  -Description 'Deny everything else' `
  -Access Deny `
  -Protocol * `
  -Direction Outbound `
  -Priority 2000 `
  -SourceAddressPrefix * `
  -SourcePortRange * `
  -DestinationAddressPrefix * `
  -DestinationPortRange *

$NetworkSecurityGroupName = $ResourceGroupName + '-NetworkSecurity'
$NetworkSecurityGroup = New-AzNetworkSecurityGroup `
  -Name $NetworkSecurityGroupName `
  -ResourceGroupName $ResourceGroupName `
  -Location $Location `
  -SecurityRules @($allowHttp, $allowQuic, $allowDns, $denyEverythingElse)

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
Display-Progress-Bar -Status 'Creating prototype VM'

$NicName = $ResourceGroupName + '-NIC'
$Nic = New-AzNetworkInterface `
  -Name $NicName `
  -ResourceGroupName $ResourceGroupName `
  -Location $Location `
  -Subnet $VirtualNetwork.Subnets[0]

$VM = New-AzVMConfig `
  -Name $ProtoVMName `
  -VMSize $VMSize `
  -Priority 'Spot' `
  -MaxPrice -1

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
  -Version latest

$VM = Set-AzVMBootDiagnostic `
  -VM $VM `
  -Disable

New-AzVm `
  -ResourceGroupName $ResourceGroupName `
  -Location $Location `
  -VM $VM | Out-Null

####################################################################################################
Display-Progress-Bar -Status 'Running provision-image.ps1 in VM'

$ProvisionImageResult = Invoke-AzVMRunCommand `
  -ResourceGroupName $ResourceGroupName `
  -VMName $ProtoVMName `
  -CommandId 'RunPowerShellScript' `
  -ScriptPath "$PSScriptRoot\provision-image.ps1" `
  -Parameter @{AdminUserPassword = $AdminPW }

Write-Host "provision-image.ps1 output: $($ProvisionImageResult.value.Message)"

####################################################################################################
Display-Progress-Bar -Status 'Restarting VM'

Restart-AzVM -ResourceGroupName $ResourceGroupName -Name $ProtoVMName | Out-Null

####################################################################################################
Display-Progress-Bar -Status 'Sleeping after restart'

# The VM appears to be busy immediately after restarting.
# This workaround waits for a minute before attempting to run sysprep.ps1.
Start-Sleep -Seconds 60

####################################################################################################
Display-Progress-Bar -Status 'Running sysprep.ps1 in VM'

Invoke-AzVMRunCommand `
  -ResourceGroupName $ResourceGroupName `
  -VMName $ProtoVMName `
  -CommandId 'RunPowerShellScript' `
  -ScriptPath "$PSScriptRoot\sysprep.ps1" | Out-Null

####################################################################################################
Display-Progress-Bar -Status 'Waiting for VM to shut down'

Wait-Shutdown -ResourceGroupName $ResourceGroupName -Name $ProtoVMName

####################################################################################################
Display-Progress-Bar -Status 'Stopping VM'

Stop-AzVM `
  -ResourceGroupName $ResourceGroupName `
  -Name $ProtoVMName `
  -Force | Out-Null

####################################################################################################
Display-Progress-Bar -Status 'Generalizing VM'

Set-AzVM `
  -ResourceGroupName $ResourceGroupName `
  -Name $ProtoVMName `
  -Generalized | Out-Null

$VM = Get-AzVM `
  -ResourceGroupName $ResourceGroupName `
  -Name $ProtoVMName

$PrototypeOSDiskName = $VM.StorageProfile.OsDisk.Name

####################################################################################################
Display-Progress-Bar -Status 'Creating gallery'

$GalleryName = 'StlBuild_' + $CurrentDate.ToString('yyyy_MM_dd_THHmm') + '_Gallery'
New-AzGallery `
  -Location $Location `
  -ResourceGroupName $ResourceGroupName `
  -Name $GalleryName | Out-Null

####################################################################################################
Display-Progress-Bar -Status 'Creating image definition'

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
  -HyperVGeneration 'V2' | Out-Null

####################################################################################################
Display-Progress-Bar -Status 'Creating image version'

$ImageVersionName = $CurrentDate.ToString('yyyyMMdd.HHmm.0')
New-AzGalleryImageVersion `
  -Location $Location `
  -ResourceGroupName $ResourceGroupName `
  -GalleryName $GalleryName `
  -GalleryImageDefinitionName $ImageDefinitionName `
  -Name $ImageVersionName `
  -SourceImageId $VM.ID | Out-Null

####################################################################################################
Display-Progress-Bar -Status 'Deleting unused VM'

Remove-AzVM `
  -Id $VM.ID `
  -Force | Out-Null

####################################################################################################
Display-Progress-Bar -Status 'Deleting unused disk'

Remove-AzDisk `
  -ResourceGroupName $ResourceGroupName `
  -DiskName $PrototypeOSDiskName `
  -Force | Out-Null

####################################################################################################
Write-Progress -Activity $ProgressActivity -Completed

Write-Host "        Location: $Location"
Write-Host "  Resource group: $ResourceGroupName"
Write-Host "         Gallery: $GalleryName"
Write-Host "Image definition: $ImageDefinitionName"
Write-Host "   Image version: $ImageVersionName"
Write-Host 'Finished!'
