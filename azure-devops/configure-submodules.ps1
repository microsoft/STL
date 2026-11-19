# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

[CmdletBinding(PositionalBinding=$false)]
Param(
  [switch]$Quiet
)

$ErrorActionPreference = 'Stop'
$PSNativeCommandUseErrorActionPreference = $true

$quietOption = $Quiet ? '--quiet' : ''

$gitFetch = 'git fetch {0} --no-recurse-submodules --filter=tree:0 --depth=1 origin "$1"' -f $quietOption
$gitSparseLlvm = 'git sparse-checkout set --sparse-index libcxx/test libcxx/utils/libcxx llvm/utils/lit'
$gitSparseBoost = 'git sparse-checkout set --sparse-index include/boost'
$gitSwitch = 'git -c promisor.quiet={0} switch {1} --detach --force' -f $Quiet, $quietOption

$llvmUpdate = "!$gitFetch && $gitSparseLlvm && $gitSwitch"
$boostUpdate = "!$gitFetch && $gitSparseBoost && $gitSwitch"
$normalUpdate = "!$gitFetch && $gitSwitch"

git config set submodule.llvm-project.update $llvmUpdate
git config set submodule.boost-math.update $boostUpdate
git config set submodule.benchmarks/google-benchmark.update $normalUpdate
