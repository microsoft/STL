// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <eh.h>
#include <exception>
_STD_BEGIN

_CRTIMP2_PURE bool __CLRCALL_PURE_OR_CDECL uncaught_exception() noexcept { // report if handling a throw
    return __uncaught_exception();
}

_STD_END
