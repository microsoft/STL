// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <synchapi.h>

// these declarations must be in sync with those in xthreads.h

using _Smtx_t = void*;

extern "C" {

static_assert(sizeof(_Smtx_t) == sizeof(SRWLOCK), "_Smtx_t must be the same size as SRWLOCK.");
static_assert(alignof(_Smtx_t) == alignof(SRWLOCK), "_Smtx_t must be the same alignment as SRWLOCK.");

void __cdecl _Smtx_lock_exclusive(_Smtx_t* smtx) { // lock shared mutex exclusively
    AcquireSRWLockExclusive(reinterpret_cast<PSRWLOCK>(smtx));
}

void __cdecl _Smtx_lock_shared(_Smtx_t* smtx) { // lock shared mutex non-exclusively
    AcquireSRWLockShared(reinterpret_cast<PSRWLOCK>(smtx));
}

int __cdecl _Smtx_try_lock_exclusive(_Smtx_t* smtx) { // try to lock shared mutex exclusively
    return TryAcquireSRWLockExclusive(reinterpret_cast<PSRWLOCK>(smtx));
}

int __cdecl _Smtx_try_lock_shared(_Smtx_t* smtx) { // try to lock shared mutex non-exclusively
    return TryAcquireSRWLockShared(reinterpret_cast<PSRWLOCK>(smtx));
}

void __cdecl _Smtx_unlock_exclusive(_Smtx_t* smtx) { // unlock exclusive shared mutex
    _Analysis_assume_lock_held_(*reinterpret_cast<PSRWLOCK>(smtx));
    ReleaseSRWLockExclusive(reinterpret_cast<PSRWLOCK>(smtx));
}

void __cdecl _Smtx_unlock_shared(_Smtx_t* smtx) { // unlock non-exclusive shared mutex
    ReleaseSRWLockShared(reinterpret_cast<PSRWLOCK>(smtx));
}
}
