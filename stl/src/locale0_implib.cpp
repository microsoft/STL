// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#if defined(_DLL)
#ifdef CRTDLL2
#undef CRTDLL2
#endif

#ifdef MRTDLL
#undef MRTDLL
#endif

#define STDCPP_IMPLIB 1

// When building for msvcmrt.lib, inject a dependency to the msvcp DLL.
#if defined _M_CEE && !defined _M_CEE_PURE
#ifdef _CRTBLD
#define _CRTBLD_WAS_DEFINED
#undef _CRTBLD
#endif
#include <use_ansi.h>
#ifdef _CRTBLD_WAS_DEFINED
#define _CRTBLD
#undef _CRTBLD_WAS_DEFINED
#endif
#endif

#include "locale0.cpp"

#endif // defined(_DLL)
