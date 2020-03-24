# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# This script assumes you have installed Azure tools into Powershell by following the instructions
# at https://docs.microsoft.com/en-us/powershell/azure/install-az-ps?view=azps-3.6.1
# or are running from Azure Cloud Shell
#

$Location = 'westus2'
$Prefix = 'CppStlGithubBuild'
$VMSize = 'Standard_D16s_v3'
$ProtoVMName = 'PROTOTYPE'
$LiveVMPrefix = 'BUILD'
$WindowsServerSku = '2019-Datacenter'

function Find-ResourceGroupName {
  Param(
    [string] $prefix
  )

  $suffix = 0
  $resources = Get-AzResourceGroup
  do {
    $collision = $false
    $suffix++
    $result = "$prefix$suffix"
    foreach ($resource in $resources) {
      if ($resource.ResourceGroupName -eq $result) {
        $collision = $true
        break
      }
    }
  } while ($collision)
  return $result
}

function New-Password {
  Param (
    [int] $length = 32
  )

  $Chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"

  $result = ''
  for ($idx = 0; $idx -lt $length; $idx++) {
    $result += $Chars[(Get-Random -Minimum 0 -Maximum ($Chars.Length - 1))]
  }

  return $result
}

$ResourceGroupName = Find-ResourceGroupName $Prefix
$AdminPW = New-Password
Write-Output "Location: $Location"
Write-Output "Resource group name: $ResourceGroupName"
Write-Output "User name: AdminUser"
Write-Output "Using Generated Password: $AdminPW"

New-AzResourceGroup -Name $ResourceGroupName -Location $Location

$AdminPWSecure = ConvertTo-SecureString $AdminPW -AsPlainText -Force
$Credential = New-Object System.Management.Automation.PSCredential ("AdminUser", $AdminPWSecure)

$allowHttp = New-AzNetworkSecurityRuleConfig `
  -Name AllowHTTP `
  -Description 'Allow HTTP(s)' `
  -Access Allow `
  -Protocol Tcp `
  -Direction Outbound `
  -Priority 1008 `
  -SourceAddressPrefix * `
  -SourcePortRange * `
  -DestinationAddressPrefix * `
  -DestinationPortRange @(80, 443)

$allowDns = New-AzNetworkSecurityRuleConfig `
  -Name AllowDNS `
  -Description 'Allow DNS' `
  -Access Allow `
  -Protocol * `
  -Direction Outbound `
  -Priority 1009 `
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
  -Priority 1010 `
  -SourceAddressPrefix * `
  -SourcePortRange * `
  -DestinationAddressPrefix * `
  -DestinationPortRange *

$NetworkSecurityGroupName = $ResourceGroupName + 'NetworkSecurity'
$NetworkSecurityGroup = New-AzNetworkSecurityGroup `
  -Name $NetworkSecurityGroupName `
  -ResourceGroupName $ResourceGroupName `
  -Location $Location `
  -SecurityRules @($allowHttp, $allowDns, $denyEverythingElse)

$SubnetName = $ResourceGroupName + 'Subnet'
$Subnet = New-AzVirtualNetworkSubnetConfig `
  -Name $SubnetName `
  -AddressPrefix "10.0.0.0/16" `
  -NetworkSecurityGroup $NetworkSecurityGroup

$VirtualNetworkName = $ResourceGroupName + 'Network'
$VirtualNetwork = New-AzVirtualNetwork `
  -Name $VirtualNetworkName `
  -ResourceGroupName $ResourceGroupName `
  -Location $Location `
  -AddressPrefix "10.0.0.0/16" `
  -Subnet $Subnet

$NicName = $ResourceGroupName + 'NIC'
$Nic = New-AzNetworkInterface `
  -Name $NicName `
  -ResourceGroupName $ResourceGroupName `
  -Location $Location `
  -Subnet $VirtualNetwork.Subnets[0]

$VM = New-AzVMConfig -Name $ProtoVMName -VMSize $VMSize
$VM = Set-AzVMOperatingSystem `
  -VM $VM `
  -Windows `
  -ComputerName $ProtoVMName `
  -Credential $Credential `
  -ProvisionVMAgent `
  -EnableAutoUpdate

$VM = Add-AzVMNetworkInterface -VM $VM -Id $Nic.Id
$VM = Set-AzVMSourceImage `
  -VM $VM `
  -PublisherName 'MicrosoftWindowsServer' `
  -Offer 'WindowsServer' `
  -Skus $WindowsServerSku `
  -Version latest

$VM = Set-AzVMBootDiagnostic -VM $VM -Disable
New-AzVm `
  -ResourceGroupName $ResourceGroupName `
  -Location $Location `
  -VM $VM

$VM = Get-AzVM -ResourceGroupName $ResourceGroupName -Name $ProtoVMName
$PrototypeOSDiskName = $VM.StorageProfile.OsDisk.Name

Invoke-AzVMRunCommand `
  -ResourceGroupName $ResourceGroupName `
  -VMName $ProtoVMName `
  -CommandId 'RunPowerShellScript' `
  -ScriptPath 'provision-image-bootstrap.ps1' `
  -Parameter @{AdminUserPassword = $AdminPW }

Stop-AzVM `
  -ResourceGroupName $ResourceGroupName `
  -Name $ProtoVMName `
  -Force

Set-AzVM `
  -ResourceGroupName $ResourceGroupName `
  -Name $ProtoVMName `
  -Generalized

$ImageConfig = New-AzImageConfig -Location $Location -SourceVirtualMachineId $VM.ID
$Image = New-AzImage -Image $ImageConfig -ImageName $ProtoVMName -ResourceGroupName $ResourceGroupName
$Image

# Clean up stuff we no longer need now that we have an image
Remove-AzVM -Id $VM.ID -Force
Remove-AzDisk -ResourceGroupName $ResourceGroupName -DiskName $PrototypeOSDiskName -Force

$VmssIpConfigName = $ResourceGroupName + 'VmssIpConfig'
$VmssIpConfig = New-AzVmssIpConfig -SubnetId $Nic.IpConfigurations[0].Subnet.Id -Primary -Name $VmssIpConfigName

$VmssName = $ResourceGroupName + 'Vmss'
$Vmss = New-AzVmssConfig `
  -Location $Location `
  -SkuCapacity 1 `
  -SkuName $VMSize `
  -SkuTier 'Standard' `
  -UpgradePolicyMode Manual `
  -EvictionPolicy Delete `
  -Priority Spot `
  -MaxPrice -1

$Vmss = Add-AzVmssNetworkInterfaceConfiguration
-VirtualMachineScaleSet $Vmss `
  -Primary $true `
  -IpConfiguration $VmssIpConfig `
  -NetworkSecurityGroupId $NetworkSecurityGroup.Id `
  -Name $NicName

$Vmss = Set-AzVmssOsProfile `
  -VirtualMachineScaleSet $Vmss `
  -ComputerNamePrefix $LiveVMPrefix `
  -AdminUsername 'AdminUser' `
  -AdminPassword $AdminPW `
  -WindowsConfigurationProvisionVMAgent $true `
  -WindowsConfigurationEnableAutomaticUpdate $true

$Vmss = Set-AzVmssStorageProfile `
  -VirtualMachineScaleSet $Vmss `
  -OsDiskCreateOption 'FromImage' `
  -OsDiskCaching ReadWrite `
  -ImageReferenceId $Image.Id `
  -ManagedDisk Premium_LRS

New-AzVmss `
  -ResourceGroupName $ResourceGroupName `
  -Name $VmssName `
  -VirtualMachineScaleSet $Vmss

$VmssResult = Get-AzVmss -ResourceGroupName $ResourceGroupName -Name $VmssName
$VmssResult
