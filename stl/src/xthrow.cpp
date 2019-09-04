// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// exception handling support functions

#include <new>
#include <stdexcept>

_STD_BEGIN

    [[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL
    _Xbad_alloc() { // report a bad_alloc error
    _THROW(bad_alloc{});
}

[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xinvalid_argument(
    _In_z_ const char* _Message) { // report an invalid_argument error
    _THROW(invalid_argument(_Message));
}

[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xlength_error(
    _In_z_ const char* _Message) { // report a length_error
    _THROW(length_error(_Message));
}

[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xout_of_range(
    _In_z_ const char* _Message) { // report an out_of_range error
    _THROW(out_of_range(_Message));
}

[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xoverflow_error(
    _In_z_ const char* _Message) { // report an overflow error
    _THROW(overflow_error(_Message));
}

[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xruntime_error(
    _In_z_ const char* _Message) { // report a runtime_error
    _THROW(runtime_error(_Message));
}
_STD_END

#include <functional>

_STD_BEGIN

    [[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL
    _Xbad_function_call() { // report a bad_function_call error
    _THROW(bad_function_call{});
}
_STD_END

#if _HAS_EXCEPTIONS
#include <regex>

_STD_BEGIN

    [[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL
    _Xregex_error(regex_constants::error_type _Code) { // report a regex_error
    _THROW(regex_error(_Code));
}
_STD_END
#endif // _HAS_EXCEPTIONS
