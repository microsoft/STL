:: Copyright (c) Microsoft Corporation.
:: SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
@echo off
"%1" "\"clang-format.exe -style=file -i\"" stl/inc stl/inc/cvt stl/inc/experimental stl/src tools/inc tools/jobify/jobify.cpp tools/parallelize/parallelize.cpp
echo If your build fails here, you need to format the following files with clang-format 8.0.1.
git status --porcelain stl 1>&2
