// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <eh.h>
#include <exception>
_STD_BEGIN

_CXX17_DEPRECATE_UNCAUGHT_EXCEPTION _NODISCARD _CRTIMP2_PURE bool __CLRCALL_PURE_OR_CDECL
    uncaught_exception() noexcept {
    return __uncaught_exception();
}

_STD_END
