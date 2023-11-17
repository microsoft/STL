// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// set_new_handler

#include <new.h>
#include <new>

namespace {
    _STD new_handler _New_handler;

    int __cdecl _New_handler_interface(size_t) { // interface to existing Microsoft _callnewh mechanism
        _New_handler();
        return 1;
    }
} // namespace

_STD_BEGIN

_CRTIMP2 new_handler __cdecl set_new_handler(_In_opt_ new_handler pnew) noexcept { // remove current handler
    _BEGIN_LOCK(_LOCK_MALLOC) // lock thread to ensure atomicity
    new_handler pold = _New_handler;
    _New_handler     = pnew;
    _set_new_handler(pnew ? _New_handler_interface : nullptr);
    return pold;
    _END_LOCK()
}

_CRTIMP2 new_handler __cdecl get_new_handler() noexcept { // get current new handler
    _BEGIN_LOCK(_LOCK_MALLOC) // lock thread to ensure atomicity
    return _New_handler;
    _END_LOCK()
}

_STD_END
