// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <eh.h>
#include <exception>
_STD_BEGIN

_NODISCARD _CRTIMP2_PURE int __CLRCALL_PURE_OR_CDECL uncaught_exceptions() noexcept {
    return __uncaught_exceptions();
}

_STD_END
