# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

# Change "ADMIN_PASSWORD" to the "AdminUser" password on the virtual machines, and "PERSONAL_ACCESS_TOKEN" to
# a PAT with permission to register Azure DevOps agents, then  aste the contents of this file into an Azure
# Cloud Shell Powershell prompt.

# When demoing changes you may need to change the "STL/master" part of the below URIs to point to your desired
# branch.

$customConfig = @{
  "fileUris" = @("https://raw.githubusercontent.com/microsoft/STL/master/azure-devops/provision-agent.ps1",
  "https://raw.githubusercontent.com/microsoft/STL/master/azure-devops/provision-agent-bootstrap.cmd"
  )
}
$protectedConfig = @{
    "commandToExecute" = "C:\Windows\System32\cmd.exe /c provision-agent-bootstrap.cmd ADMIN_PASSWORD PERSONAL_ACCESS_TOKEN"
}
$vmss = Get-AzVmss -ResourceGroupName "CppStlGithubBuildMachines" -VMScaleSetName "MSVCSTL-BUILD"
$vmss = Add-AzVmssExtension -VirtualMachineScaleSet $vmss  -Name "DeployAgent" -Publisher "Microsoft.Compute" `
  -Type "CustomScriptExtension" -TypeHandlerVersion 1.10 -Setting $customConfig -ProtectedSetting $protectedConfig
Update-AzVmss -ResourceGroupName "CppStlGithubBuildMachines" -Name "MSVCSTL-BUILD" -VirtualMachineScaleSet $vmss
