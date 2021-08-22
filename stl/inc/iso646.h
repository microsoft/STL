// iso646.h standard header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _ISO646
#define _ISO646

#ifndef _STL_COMPILER_PREPROCESSOR
// All STL headers avoid exposing their contents when included by various
// non-C++-compiler tools to avoid breaking builds when we use newer language
// features in the headers than such tools understand.
#if defined(RC_INVOKED) || defined(Q_MOC_RUN) || defined(__midl)
#define _STL_COMPILER_PREPROCESSOR 0
#else
#define _STL_COMPILER_PREPROCESSOR 1
#endif
#endif // _STL_COMPILER_PREPROCESSOR

#if _STL_COMPILER_PREPROCESSOR

#if !defined(__cplusplus) || defined(_MSC_EXTENSIONS)
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
#endif // !defined(__cplusplus) || defined(_MSC_EXTENSIONS)
#endif // _STL_COMPILER_PREPROCESSOR
#endif // _ISO646
