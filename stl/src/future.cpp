// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <exception>
#include <future>

_STD_BEGIN

// TRANSITION, ABI: preserved for binary compatibility
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Throw_future_error(const error_code& _Code) {
    _THROW(future_error(_Code));
}

// TRANSITION, ABI: preserved for binary compatibility
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Rethrow_future_exception(exception_ptr _Ptr) {
    _STD rethrow_exception(_Ptr);
}

_STD_END
