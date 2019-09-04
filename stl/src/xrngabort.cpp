// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// abort on precondition failure in random number generators

#include <yvals.h>

#include <stdio.h>
#include <stdlib.h>

// clang-format off
_STD_BEGIN
// TRANSITION, ABI: _Rng_abort() is preserved for binary compatibility
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Rng_abort(_In_z_ const char* _Msg) {
    // abort on precondition failure
    _CSTD fputs(_Msg, stderr);
    _CSTD fputc('\n', stderr);
    _CSTD abort();
}
_STD_END
// clang-format on
