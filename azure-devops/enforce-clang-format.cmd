:: Copyright (c) Microsoft Corporation.
:: SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
@echo off
clang-format -style=file -i stl/inc/* stl/inc/cvt/* stl/inc/experimental/* stl/src/* 2>&1
echo If your build fails here, you need to format the following files with clang-format 8.0.1.
git status --porcelain 1>&2
