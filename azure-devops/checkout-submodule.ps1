# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

[CmdletBinding(PositionalBinding=$false)]
Param(
  [Parameter(Mandatory)][ValidateSet('llvm-project', 'boost-math', 'benchmarks/google-benchmark')][String]$Submodule
)

$ErrorActionPreference = 'Stop'
$PSNativeCommandUseErrorActionPreference = $true

git submodule --quiet update --init --force --no-fetch --filter=tree:0 --depth=1 -- $Submodule
git -C $Submodule clean --quiet -x -d -f -f
