// iso646.h standard header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _ISO646
#define _ISO646
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR

#if !defined(__cplusplus) || defined(_MSC_EXTENSIONS)
// clang-format off
#define and &&
#define and_eq &=
#define bitand &
#define bitor |
#define compl ~
#define not !
#define not_eq !=
#define or ||
#define or_eq |=
#define xor ^
#define xor_eq ^=
// clang-format on
#endif // !defined(__cplusplus) || defined(_MSC_EXTENSIONS)
#endif // _STL_COMPILER_PREPROCESSOR
#endif // _ISO646
