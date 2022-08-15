# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

[CmdletBinding(PositionalBinding = $False)]
Param(
    [Parameter()]
    [String]$DiffFile
)

if ([string]::IsNullOrEmpty($DiffFile)) {
    $DiffFile = [System.IO.Path]::GetTempFileName()
}

Start-Process -FilePath 'git' -ArgumentList 'diff', '--ignore-submodules' `
    -NoNewWindow -Wait `
    -RedirectStandardOutput $DiffFile
if (0 -ne (Get-Item -LiteralPath $DiffFile).Length) {
    $message = @(
        '##vso[task.logissue type=error]The formatting of the files in the repo was not what we expected.'
        'Please access the diff from format.diff in the build artifacts'
        'and apply it with `git apply`. To download the diff:'
        '1. Click the failed Validation job,'
        '2. Click "1 artifact produced",'
        '3. Hover over format.diff,'
        '4. Click the three dots that appear on the right,'
        '5. Click "Download artifacts".'
        'Alternatively, you can run the `format` CMake target:'
        '    cmake --build <builddir> --target format'
        ''
        '##[group]Expected formatting - click to expand diff'
        Get-Content -LiteralPath $DiffFile -Raw
        '##[endgroup]'
        "##vso[artifact.upload artifactname=format.diff]$DiffFile"
        '##vso[task.complete result=Failed]DONE'
    )
    Write-Host ($message -join "`n")
}
