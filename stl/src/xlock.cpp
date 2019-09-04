// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// global lock for locales, etc.

#include <yvals.h>

#include "xmtx.h"
#include <locale.h>
#include <stdlib.h>

_STD_BEGIN

#define MAX_LOCK 8 // must be power of two

#pragma warning(disable : 4074)
#pragma init_seg(compiler)

static _Rmtx mtx[MAX_LOCK];
static long init = -1;

#if !defined(MRTDLL)

__thiscall _Init_locks::_Init_locks() noexcept { // initialize locks
    if (InterlockedIncrement(&init) == 0) {
        for (auto& elem : mtx) {
            _Mtxinit(&elem);
        }
    }
}

__thiscall _Init_locks::~_Init_locks() noexcept { // clean up locks
    if (InterlockedDecrement(&init) < 0) {
        for (auto& elem : mtx) {
            _Mtxdst(&elem);
        }
    }
}

#endif

void __cdecl _Init_locks::_Init_locks_ctor(_Init_locks*) noexcept { // initialize locks
    if (InterlockedIncrement(&init) == 0) {
        for (auto& elem : mtx) {
            _Mtxinit(&elem);
        }
    }
}

void __cdecl _Init_locks::_Init_locks_dtor(_Init_locks*) noexcept { // clean up locks
    if (InterlockedDecrement(&init) < 0) {
        for (auto& elem : mtx) {
            _Mtxdst(&elem);
        }
    }
}

static _Init_locks initlocks;

#if !defined(MRTDLL)

__thiscall _Lockit::_Lockit() noexcept : _Locktype(0) { // lock default mutex
    if (_Locktype == _LOCK_LOCALE) {
        _lock_locales();
    } else {
        _Mtxlock(&mtx[0]);
    }
}

__thiscall _Lockit::_Lockit(int kind) noexcept : _Locktype(kind) { // lock the mutex
    if (_Locktype == _LOCK_LOCALE) {
        _lock_locales();
    } else if (_Locktype < MAX_LOCK) {
        _Mtxlock(&mtx[_Locktype]);
    }
}

__thiscall _Lockit::~_Lockit() noexcept { // unlock the mutex
    if (_Locktype == _LOCK_LOCALE) {
        _unlock_locales();
    } else if (_Locktype < MAX_LOCK) {
        _Mtxunlock(&mtx[_Locktype]);
    }
}

#endif

void __cdecl _Lockit::_Lockit_ctor(_Lockit*) noexcept { // lock default mutex
    _Mtxlock(&mtx[0]);
}

void __cdecl _Lockit::_Lockit_ctor(_Lockit* _This, int kind) noexcept { // lock the mutex
    if (kind == _LOCK_LOCALE) {
        _lock_locales();
    } else {
        _This->_Locktype = kind & (MAX_LOCK - 1);
        _Mtxlock(&mtx[_This->_Locktype]);
    }
}

void __cdecl _Lockit::_Lockit_dtor(_Lockit* _This) noexcept { // unlock the mutex
    _Mtxunlock(&mtx[_This->_Locktype]);
}

_RELIABILITY_CONTRACT
void __cdecl _Lockit::_Lockit_ctor(int kind) noexcept { // lock the mutex
    if (kind == _LOCK_LOCALE) {
        _lock_locales();
    } else {
        _Mtxlock(&mtx[kind & (MAX_LOCK - 1)]);
    }
}

_RELIABILITY_CONTRACT
void __cdecl _Lockit::_Lockit_dtor(int kind) noexcept { // unlock the mutex
    if (kind == _LOCK_LOCALE) {
        _unlock_locales();
    } else {
        _Mtxunlock(&mtx[kind & (MAX_LOCK - 1)]);
    }
}

extern "C" void _Lock_at_thread_exit_mutex() { // lock the at-thread-exit mutex
    _Mtxlock(&mtx[_LOCK_AT_THREAD_EXIT]);
}

extern "C" void _Unlock_at_thread_exit_mutex() { // unlock the at-thread-exit mutex
    _Mtxunlock(&mtx[_LOCK_AT_THREAD_EXIT]);
}

_STD_END
