// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// __std_execute_once_begin & __std_execute_once_end functions

#include "xcall_once.h"
#include <synchapi.h>

// This must be as small as possible, because its contents are
// injected into the msvcprt.lib and msvcprtd.lib import libraries.
// Do not include or define anything else here.
// In particular, basic_string must not be included here.

// these declarations must be in sync with those in xcall_once.h

_STD_BEGIN
int __CLRCALL_PURE_OR_CDECL __std_execute_once_begin(
    once_flag& _Once_flag, int& _Pending) noexcept { // wrap Win32 InitOnceBeginInitialize()
    static_assert(sizeof(_Once_flag._Opaque) == sizeof(INIT_ONCE), "invalid size");

    return ::InitOnceBeginInitialize(reinterpret_cast<PINIT_ONCE>(&_Once_flag._Opaque), 0, &_Pending, nullptr);
}

int __CLRCALL_PURE_OR_CDECL __std_execute_once_complete(
    once_flag& _Once_flag, unsigned long _Flags) noexcept { // wrap Win32 InitOnceComplete()
    static_assert(sizeof(_Once_flag._Opaque) == sizeof(INIT_ONCE), "invalid size");

    return ::InitOnceComplete(reinterpret_cast<PINIT_ONCE>(&_Once_flag._Opaque), _Flags, nullptr);
}
_STD_END
