// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// The /clr:pure exception_ptr implementation.
#ifdef CRTDLL
#undef CRTDLL
#endif

#ifdef MRTDLL
#undef MRTDLL
#endif

#ifndef _DLL
#define _DLL
#endif

#include "excptptr.cpp"
