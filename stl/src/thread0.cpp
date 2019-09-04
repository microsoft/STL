// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// thread support functions

#include <yvals.h>

#include <mutex>
#include <system_error>
#include <thread>
#include <xthreads.h>

#if _HAS_EXCEPTIONS
#include <exception>
#include <string>

#else // _HAS_EXCEPTIONS
#include <cstdio>
#endif // _HAS_EXCEPTIONS

static const char* const msgs[] = { // error messages
    "device or resource busy", "invalid argument", "no such process", "not enough memory", "operation not permitted",
    "resource deadlock would occur", "resource unavailable try again"};

static const int codes[] = { // system_error codes
    (int) _STD errc::device_or_resource_busy, (int) _STD errc::invalid_argument, (int) _STD errc::no_such_process,
    (int) _STD errc::not_enough_memory, (int) _STD errc::operation_not_permitted,
    (int) _STD errc::resource_deadlock_would_occur, (int) _STD errc::resource_unavailable_try_again};

_STD_BEGIN

#if _HAS_EXCEPTIONS
_CRTIMP2_PURE void __cdecl _Throw_Cpp_error(int code) { // throw error object
    throw _STD system_error(codes[code], _STD generic_category(), msgs[code]);
}

#else // _HAS_EXCEPTIONS
_CRTIMP2_PURE void __cdecl _Throw_Cpp_error(int code) { // report system error
    _CSTD fputs(msgs[code], stderr);
    _CSTD abort();
}
#endif // _HAS_EXCEPTIONS

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
