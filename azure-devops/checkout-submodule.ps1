# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

[CmdletBinding(PositionalBinding=$false)]
Param(
  [Parameter(Mandatory)][ValidateSet('llvm-project', 'boost-math', 'benchmarks/google-benchmark')][String]$submodule
)

$ErrorActionPreference = 'Stop'
$PSNativeCommandUseErrorActionPreference = $true

$rawStatus = git submodule status --cached $submodule
$sha = $rawStatus -creplace '^[ \-+]([0-9a-f]+) .*$', '$1'
cd $submodule
git init
if ((git remote) -eq $null) {
  $urls = @{
    'llvm-project'                = 'https://github.com/llvm/llvm-project.git'
    'boost-math'                  = 'https://github.com/boostorg/math.git'
    'benchmarks/google-benchmark' = 'https://github.com/google/benchmark.git'
  }
  git remote add submodule-upstream $urls[$submodule]
}
git fetch --filter=tree:0 --depth=1 submodule-upstream $sha
if ($submodule -ceq 'llvm-project') {
  git sparse-checkout set --sparse-index libcxx/test libcxx/utils/libcxx llvm/utils/lit
}
git reset --quiet --hard FETCH_HEAD
git clean --quiet -x -d -f -f
