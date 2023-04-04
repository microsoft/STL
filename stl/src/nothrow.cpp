// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// define nothrow object

#ifdef CRTDLL2
#undef CRTDLL2
#endif

#ifdef MRTDLL
#undef MRTDLL
#endif

#include <yvals_core.h>

#include <vcruntime_new.h>

_STD_BEGIN
const nothrow_t nothrow = nothrow_t();
_STD_END
