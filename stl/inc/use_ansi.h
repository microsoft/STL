// use_ansi.h internal header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#ifndef _USE_ANSI_CPP
#define _USE_ANSI_CPP

#ifdef _CRTBLD
#define _CRT_NOPRAGMA_LIBS
#else
#undef _CRT_NOPRAGMA_LIBS
#endif

#ifndef _CRT_NOPRAGMA_LIBS

#ifndef _M_CEE_PURE

#undef _DEBUG_AFFIX
#undef _IDL_AFFIX
#undef _IDL_DEFAULT
#undef _LIB_STEM

#ifdef _DEBUG
#define _DEBUG_AFFIX "d"
#define _IDL_DEFAULT 2
#else
#define _DEBUG_AFFIX ""
#define _IDL_DEFAULT 0
#endif

#if defined(_DLL) && !defined(_STATIC_CPPLIB)
#define _LIB_STEM "msvcprt"
#else
#define _LIB_STEM "libcpmt"

#if _ITERATOR_DEBUG_LEVEL != _IDL_DEFAULT
#define _IDL_AFFIX _STRINGIZE(_ITERATOR_DEBUG_LEVEL)
#endif
#endif

#ifdef _IDL_AFFIX
#else
#define _IDL_AFFIX ""
#endif

#pragma comment(lib, _LIB_STEM _DEBUG_AFFIX _IDL_AFFIX)

#undef _DEBUG_AFFIX
#undef _IDL_AFFIX
#undef _IDL_DEFAULT
#undef _LIB_STEM

#endif // _M_CEE_PURE

#endif // _CRT_NOPRAGMA_LIBS

#endif // _USE_ANSI_CPP
