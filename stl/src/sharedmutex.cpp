// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <__msvc_threads_core.hpp>
#include <synchapi.h>

#include "primitives.hpp"

// these declarations must be in sync with those in xthreads.h

extern "C" {
static_assert(sizeof(_Smtx_t) == sizeof(SRWLOCK), "_Smtx_t must be the same size as SRWLOCK.");
static_assert(alignof(_Smtx_t) == alignof(SRWLOCK), "_Smtx_t must be the same alignment as SRWLOCK.");

void __cdecl _Smtx_lock_exclusive(_Smtx_t* smtx) noexcept { // lock shared mutex exclusively
    AcquireSRWLockExclusive(reinterpret_cast<PSRWLOCK>(smtx));
}

void __cdecl _Smtx_lock_shared(_Smtx_t* smtx) noexcept { // lock shared mutex non-exclusively
    AcquireSRWLockShared(reinterpret_cast<PSRWLOCK>(smtx));
}

int __cdecl _Smtx_try_lock_exclusive(_Smtx_t* smtx) noexcept { // try to lock shared mutex exclusively
    return TryAcquireSRWLockExclusive(reinterpret_cast<PSRWLOCK>(smtx));
}

int __cdecl _Smtx_try_lock_shared(_Smtx_t* smtx) noexcept { // try to lock shared mutex non-exclusively
    return TryAcquireSRWLockShared(reinterpret_cast<PSRWLOCK>(smtx));
}

void __cdecl _Smtx_unlock_exclusive(_Smtx_t* smtx) noexcept { // unlock exclusive shared mutex
    _Analysis_assume_lock_held_(*reinterpret_cast<PSRWLOCK>(smtx));
    ReleaseSRWLockExclusive(reinterpret_cast<PSRWLOCK>(smtx));
}

void __cdecl _Smtx_unlock_shared(_Smtx_t* smtx) noexcept { // unlock non-exclusive shared mutex
    ReleaseSRWLockShared(reinterpret_cast<PSRWLOCK>(smtx));
}

void __stdcall _Thrd_sleep_for(const unsigned long ms) noexcept { // suspend current thread for `ms` milliseconds
    Sleep(ms);
}

namespace {
    _Thrd_result __stdcall _Cnd_timedwait_for_impl(
        const _Cnd_t cond, const _Mtx_t mtx, const unsigned int target_ms, const bool checked) noexcept {
        _Thrd_result res            = _Thrd_result::_Success;
        unsigned long long start_ms = 0;

        if (checked) {
            start_ms = GetTickCount64();
        }

        // TRANSITION: replace with _Mtx_clear_owner(mtx);
        mtx->_Thread_id = -1;
        --mtx->_Count;

        if (!_Primitive_wait_for(cond, mtx, target_ms)) { // report timeout
            if (!checked || GetTickCount64() - start_ms >= target_ms) {
                res = _Thrd_result::_Timedout;
            }
        }
        // TRANSITION: replace with _Mtx_reset_owner(mtx);
        mtx->_Thread_id = static_cast<long>(GetCurrentThreadId());
        ++mtx->_Count;

        return res;
    }
} // namespace

// TRANSITION, ABI: preserved for compatibility
_Thrd_result __stdcall _Cnd_timedwait_for(const _Cnd_t cond, const _Mtx_t mtx, const unsigned int target_ms) noexcept {
    return _Cnd_timedwait_for_impl(cond, mtx, target_ms, true);
}

_Thrd_result __stdcall _Cnd_timedwait_for_unchecked(
    const _Cnd_t cond, const _Mtx_t mtx, const unsigned int target_ms) noexcept {
    return _Cnd_timedwait_for_impl(cond, mtx, target_ms, false);
}


} // extern "C"
