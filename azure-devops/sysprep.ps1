# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

$ErrorActionPreference = 'Stop'
Write-Host 'Running sysprep'
& C:\Windows\system32\sysprep\sysprep.exe /oobe /generalize /mode:vm /shutdown
