// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// abort on precondition failure in random number generators

#include <yvals.h>

#include <cstdlib>

_STD_BEGIN
// TRANSITION, ABI: preserved for binary compatibility
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Rng_abort(const char*) {
    // abort on precondition failure
    _CSTD abort();
}
_STD_END
