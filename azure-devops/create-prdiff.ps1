# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

[CmdletBinding(PositionalBinding = $False)]
Param(
    [Parameter(Mandatory = $True)]
    [String]$DiffFile
)

Start-Process -FilePath 'git' -ArgumentList 'diff' `
    -NoNewWindow -Wait `
    -RedirectStandardOutput $DiffFile
if (0 -ne (Get-Item -LiteralPath $DiffFile).Length) {
    $errorMessage = @(
        'The formatting of the files in the repo was not what we expected.'
        'Please access the diff from format.diff in the build artifacts,'
        'and apply it with `git apply`.'
        'Alternatively, you can run the `format` CMake target:'
        '    cmake --build <builddir> --target format'
    )
    Write-Error ($errorMessage -join "`n") -ErrorAction Continue

    $restOfMessage = @(
        ''
        '##[group]Expected formatting - diff'
        Get-Content -LiteralPath $DiffFile -Raw
        '##[endgroup]'
        "##vso[artifact.upload artifactname=format.diff]$DiffFile"
        '##vso[task.complete result=Failed]DONE'
    )

    Write-Host ($restOfMessage -join "`n")
}
