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
    $msg = @(
        'The formatting of the files in the repo was not what we expected.'
        'Please access the diff from format.diff in the build artifacts,'
        'and apply it with `git apply`.'
        'Alternatively, you can run the `format` CMake target:'
        '    cmake --build <builddir> --target format'
        ''
    )
    Write-Host ($msg -join '`n')
    Write-Host (Get-Content -LiteralPath $DiffFile)
    throw
}
