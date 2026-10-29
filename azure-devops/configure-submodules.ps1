# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

$ErrorActionPreference = 'Stop'
$PSNativeCommandUseErrorActionPreference = $true

$gitConfig = 'git config set promisor.quiet true'
$gitFetch = 'git fetch --quiet --no-recurse-submodules --filter=tree:0 --depth=1 origin "$1"'
$gitSparseCheckout = 'git sparse-checkout set --sparse-index libcxx/test libcxx/utils/libcxx llvm/utils/lit'
$gitCheckout = 'git checkout --quiet --force'

$llvmUpdate = "!$gitConfig && $gitFetch && $gitSparseCheckout && $gitCheckout"
$normalUpdate = "!$gitConfig && $gitFetch && $gitCheckout"

git config set submodule.llvm-project.update $llvmUpdate
git config set submodule.boost-math.update $normalUpdate
git config set submodule.benchmarks/google-benchmark.update $normalUpdate
