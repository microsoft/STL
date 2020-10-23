:: Copyright (c) Microsoft Corporation.
:: SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
call "%PROGRAMFILES(X86)%\Microsoft Visual Studio\2019\Preview\Common7\Tools\VsDevCmd.bat" ^
-host_arch=amd64 -arch=amd64 -no_logo
"%1" "clang-format.exe -style=file -i" ^
stl/inc ^
stl/src ^
tests ^
tools
@echo If your build fails here, you need to format the following files with:
@clang-format.exe --version
@git status --porcelain stl tests tools 1>&2
@echo clang-format will produce the following diff:
@git diff stl tests tools 1>&2
