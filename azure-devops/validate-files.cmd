:: Copyright (c) Microsoft Corporation.
:: SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
call "%ProgramFiles%\Microsoft Visual Studio\2022\Preview\Common7\Tools\VsDevCmd.bat" ^
    -host_arch=amd64 -arch=amd64 -no_logo
cmake -G Ninja -DCMAKE_CXX_COMPILER=cl -DCMAKE_BUILD_TYPE=Release ^
    -S "%1\tools\validate" -B "%TEMP%\validate-build"
cmake --build "%TEMP%\validate-build"
"%TEMP%\validate-build\validate.exe"
@echo If your build fails here, you need to fix the listed issues.
