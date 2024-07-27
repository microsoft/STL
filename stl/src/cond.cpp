// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cstdlib>
#include <internal_shared.h>
#include <new>
#include <type_traits>
#include <xthreads.h>
#include <xtimec.h>

#include "primitives.hpp"

extern "C" {

// TRANSITION, ABI: preserved for binary compatibility (and _DISABLE_CONSTEXPR_MUTEX_CONSTRUCTOR)
_CRTIMP2_PURE void __cdecl _Cnd_init_in_situ(const _Cnd_t cond) noexcept { // initialize condition variable in situ
    new (cond) _Cnd_internal_imp_t;
}

// TRANSITION, ABI: preserved for binary compatibility
_CRTIMP2_PURE void __cdecl _Cnd_destroy_in_situ(_Cnd_t) noexcept {} // destroy condition variable in situ

// TRANSITION, ABI: preserved for binary compatibility
_CRTIMP2_PURE _Thrd_result __cdecl _Cnd_init(_Cnd_t* const pcond) noexcept { // initialize
    *pcond = nullptr;

    const auto cond = static_cast<_Cnd_t>(_calloc_crt(1, sizeof(_Cnd_internal_imp_t)));
    if (cond == nullptr) {
        return _Thrd_result::_Nomem; // report alloc failed
    }

    _Cnd_init_in_situ(cond);
    *pcond = cond;
    return _Thrd_result::_Success;
}

// TRANSITION, ABI: preserved for binary compatibility
_CRTIMP2_PURE void __cdecl _Cnd_destroy(const _Cnd_t cond) noexcept { // clean up
    if (cond) { // something to do, do it
        _free_crt(cond);
    }
}

// TRANSITION, ABI: should be static; dllexported for binary compatibility
_CRTIMP2_PURE void __cdecl _Mtx_clear_owner(_Mtx_t mtx) noexcept { // set owner to nobody
    mtx->_Thread_id = -1;
    --mtx->_Count;
}

// TRANSITION, ABI: should be static; dllexported for binary compatibility
_CRTIMP2_PURE void __cdecl _Mtx_reset_owner(_Mtx_t mtx) noexcept { // set owner to current thread
    mtx->_Thread_id = static_cast<long>(GetCurrentThreadId());
    ++mtx->_Count;
}

_CRTIMP2_PURE _Thrd_result __cdecl _Cnd_wait(const _Cnd_t cond, const _Mtx_t mtx) noexcept { // wait until signaled
    _Mtx_clear_owner(mtx);
    _Primitive_wait(cond, mtx);
    _Mtx_reset_owner(mtx);
    return _Thrd_result::_Success; // TRANSITION, ABI: Always succeeds
}

// TRANSITION, ABI: preserved for compatibility; wait until signaled or timeout
_CRTIMP2_PURE _Thrd_result __cdecl _Cnd_timedwait(
    const _Cnd_t cond, const _Mtx_t mtx, const _timespec64* const target) noexcept {
    _Thrd_result res = _Thrd_result::_Success;
    if (target == nullptr) { // no target time specified, wait on mutex
        _Mtx_clear_owner(mtx);
        _Primitive_wait(cond, mtx);
        _Mtx_reset_owner(mtx);
    } else { // target time specified, wait for it
        _timespec64 now;
        _Timespec64_get_sys(&now);
        _Mtx_clear_owner(mtx);
        if (!_Primitive_wait_for(cond, mtx, _Xtime_diff_to_millis2(target, &now))) { // report timeout
            _Timespec64_get_sys(&now);
            if (_Xtime_diff_to_millis2(target, &now) == 0) {
                res = _Thrd_result::_Timedout;
            }
        }
        _Mtx_reset_owner(mtx);
    }
    return res;
}

_CRTIMP2_PURE _Thrd_result __cdecl _Cnd_signal(const _Cnd_t cond) noexcept { // release one waiting thread
    _Primitive_notify_one(cond);
    return _Thrd_result::_Success; // TRANSITION, ABI: Always succeeds
}

_CRTIMP2_PURE _Thrd_result __cdecl _Cnd_broadcast(const _Cnd_t cond) noexcept { // release all waiting threads
    _Primitive_notify_all(cond);
    return _Thrd_result::_Success; // TRANSITION, ABI: Always succeeds
}

} // extern "C"

/*
 * This file is derived from software bearing the following
 * restrictions:
 *
 * (c) Copyright William E. Kempf 2001
 *
 * Permission to use, copy, modify, distribute and sell this
 * software and its documentation for any purpose is hereby
 * granted without fee, provided that the above copyright
 * notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting
 * documentation. William E. Kempf makes no representations
 * about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 */
