# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# This script assumes you have installed Azure tools into PowerShell by following the instructions
# at https://docs.microsoft.com/en-us/powershell/azure/install-az-ps?view=azps-3.6.1
# or are running from Azure Cloud Shell.
#

$Location = 'westus2'
$Prefix = 'StlBuild-' + (Get-Date -Format 'yyyy-MM-dd')
$VMSize = 'Standard_D16s_v3'
$ProtoVMName = 'PROTOTYPE'
$LiveVMPrefix = 'BUILD'
$WindowsServerSku = '2019-Datacenter'

$ProgressActivity = 'Creating Scale Set'
$TotalProgress = 8
$CurrentProgress = 1

function Find-ResourceGroupNameCollision {
  Param([string]$Test, $Resources)

  foreach ($resource in $Resources) {
    if ($resource.ResourceGroupName -eq $Test) {
      return $true
    }
  }

  return $false
}

function Find-ResourceGroupName {
  Param([string] $Prefix)

  $resources = Get-AzResourceGroup
  $result = $Prefix
  $suffix = 0
  while (Find-ResourceGroupNameCollision -Test $result -Resources $resources) {
    $suffix++
    $result = "$Prefix-$suffix"
  }

  return $result
}

function New-Password {
  Param ([int] $Length = 32)

  $Chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
  $result = ''
  for ($idx = 0; $idx -lt $Length; $idx++) {
    $result += $Chars[(Get-Random -Minimum 0 -Maximum $Chars.Length)]
  }

  return $result
}

function Start-WaitForShutdown {
  Param([string]$ResourceGroupName, [string]$Name)

  Write-Output "Waiting for $Name to stop..."
  while ($true) {
    $Vm = Get-AzVM -ResourceGroupName $ResourceGroupName -Name $Name -Status
    $highestStatus = $Vm.Statuses.Count
    for ($idx = 0; $idx -lt $highestStatus; $idx++) {
      if ($Vm.Statuses[$idx].Code -eq 'PowerState/stopped') {
        return
      }
    }

    Write-Output "... not stopped yet, sleeping for 10 seconds"
    Start-Sleep -Seconds 10
  }
}

function Write-Reminders {
  Param([string]$AdminPW)

  Write-Output "Location: $Location"
  Write-Output "Resource group name: $ResourceGroupName"
  Write-Output "User name: AdminUser"
  Write-Output "Using generated password: $AdminPW"
}

####################################################################################################
Write-Progress `
  -Activity $ProgressActivity `
  -Status 'Creating resource group' `
  -PercentComplete (100 / $TotalProgress * $CurrentProgress++)

$ResourceGroupName = Find-ResourceGroupName $Prefix
$AdminPW = New-Password
Write-Reminders $AdminPW
New-AzResourceGroup -Name $ResourceGroupName -Location $Location
$AdminPWSecure = ConvertTo-SecureString $AdminPW -AsPlainText -Force
$Credential = New-Object System.Management.Automation.PSCredential ("AdminUser", $AdminPWSecure)

####################################################################################################
Write-Progress `
  -Activity 'Creating prototype VM' `
  -PercentComplete (100 / $TotalProgress * $CurrentProgress++)

$allowHttp = New-AzNetworkSecurityRuleConfig `
  -Name AllowHTTP `
  -Description 'Allow HTTP(S)' `
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

####################################################################################################
Write-Progress `
  -Activity $ProgressActivity `
  -Status 'Running provisioning script in VM' `
  -PercentComplete (100 / $TotalProgress * $CurrentProgress++)

$VM = Get-AzVM -ResourceGroupName $ResourceGroupName -Name $ProtoVMName
$PrototypeOSDiskName = $VM.StorageProfile.OsDisk.Name
Invoke-AzVMRunCommand `
  -ResourceGroupName $ResourceGroupName `
  -VMName $ProtoVMName `
  -CommandId 'RunPowerShellScript' `
  -ScriptPath "$PSScriptRoot\provision-image.ps1" `
  -Parameter @{AdminUserPassword = $AdminPW }

####################################################################################################
Write-Progress `
  -Activity $ProgressActivity `
  -Status 'Waiting for VM to shut down' `
  -PercentComplete (100 / $TotalProgress * $CurrentProgress++)

Start-WaitForShutdown -ResourceGroupName $ResourceGroupName -Name $ProtoVMName

####################################################################################################
Write-Progress `
  -Activity $ProgressActivity `
  -Status 'Converting VM to Image' `
  -PercentComplete (100 / $TotalProgress * $CurrentProgress++)

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

####################################################################################################
Write-Progress `
  -Activity $ProgressActivity `
  -Status 'Deleting unused VM and disk' `
  -PercentComplete (100 / $TotalProgress * $CurrentProgress++)

Remove-AzVM -Id $VM.ID -Force
Remove-AzDisk -ResourceGroupName $ResourceGroupName -DiskName $PrototypeOSDiskName -Force

####################################################################################################
Write-Progress `
  -Activity $ProgressActivity `
  -Status 'Creating scale set' `
  -PercentComplete (100 / $TotalProgress * $CurrentProgress++)

$VmssIpConfigName = $ResourceGroupName + 'VmssIpConfig'
$VmssIpConfig = New-AzVmssIpConfig -SubnetId $Nic.IpConfigurations[0].Subnet.Id -Primary -Name $VmssIpConfigName
$VmssName = $ResourceGroupName + 'Vmss'
$Vmss = New-AzVmssConfig `
  -Location $Location `
  -SkuCapacity 2 `
  -SkuName $VMSize `
  -SkuTier 'Standard' `
  -Overprovision $false `
  -UpgradePolicyMode Manual `
  -EvictionPolicy Delete `
  -ScaleInPolicy OldestVM `
  -Priority Spot `
  -MaxPrice -1

$Vmss = Add-AzVmssNetworkInterfaceConfiguration `
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

####################################################################################################
Write-Progress -Activity $ProgressActivity -Completed
Write-Reminders $AdminPW
Write-Output 'Finished!'
