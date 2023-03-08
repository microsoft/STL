// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// thread support functions

#include <cstdlib>
#include <system_error>
#include <xthreads.h>

_STD_BEGIN

static constexpr const char* msgs[] = {
    // error messages
    "device or resource busy",
    "invalid argument",
    "no such process",
    "not enough memory",
    "operation not permitted",
    "resource deadlock would occur",
    "resource unavailable try again",
};

static constexpr errc codes[] = {
    // system_error codes
    errc::device_or_resource_busy,
    errc::invalid_argument,
    errc::no_such_process,
    errc::not_enough_memory,
    errc::operation_not_permitted,
    errc::resource_deadlock_would_occur,
    errc::resource_unavailable_try_again,
};

[[noreturn]] _CRTIMP2_PURE void __cdecl _Throw_Cpp_error(int code) { // throw error object
    _THROW(system_error(static_cast<int>(codes[code]), _STD generic_category(), msgs[code]));
}

// TRANSITION, ABI: preserved for binary compatibility
[[noreturn]] _CRTIMP2_PURE void __cdecl _Throw_C_error(int code) { // throw error object for C error
    switch (code) { // select the exception
    case _Thrd_nomem:
    case _Thrd_timedout:
        _Throw_Cpp_error(_RESOURCE_UNAVAILABLE_TRY_AGAIN);

    case _Thrd_busy:
        _Throw_Cpp_error(_DEVICE_OR_RESOURCE_BUSY);

    case _Thrd_error:
        _Throw_Cpp_error(_INVALID_ARGUMENT);

    default:
        _CSTD abort();
    }
}
_STD_END
