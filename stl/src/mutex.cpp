// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cstdio>
#include <cstdlib>
#include <internal_shared.h>
#include <mutex>
#include <new>
#include <type_traits>
#include <xthreads.h>
#include <xtimec.h>

extern "C" {

// TRANSITION, ABI: exported only for ABI compat
[[noreturn]] _CRTIMP2_PURE void __cdecl _Thrd_abort(const char* msg) noexcept { // abort on precondition failure
    fputs(msg, stderr);
    fputc('\n', stderr);
    abort();
}

#ifdef _DEBUG
#define _THREAD_QUOTX(x)          #x
#define _THREAD_QUOT(x)           _THREAD_QUOTX(x)
#define _THREAD_ASSERT(expr, msg) ((expr) ? (void) 0 : _Thrd_abort(__FILE__ "(" _THREAD_QUOT(__LINE__) "): " msg))
#else // ^^^ defined(_DEBUG) / !defined(_DEBUG) vvv
#define _THREAD_ASSERT(expr, msg) ((void) 0)
#endif // ^^^ !defined(_DEBUG) ^^^

// TRANSITION, ABI: preserved for binary compatibility
enum class __stl_sync_api_modes_enum { normal, win7, vista, concrt };
_CRTIMP2 void __cdecl __set_stl_sync_api_mode(__stl_sync_api_modes_enum) noexcept {}

[[nodiscard]] static PSRWLOCK get_srw_lock(_Mtx_t mtx) noexcept {
    return reinterpret_cast<PSRWLOCK>(&mtx->_Critical_section._M_srw_lock);
}

// TRANSITION, ABI: preserved for binary compatibility (and _DISABLE_CONSTEXPR_MUTEX_CONSTRUCTOR)
_CRTIMP2_PURE void __cdecl _Mtx_init_in_situ(_Mtx_t mtx, int type) noexcept { // initialize mutex in situ
    new (&mtx->_Critical_section) _Stl_critical_section;
    mtx->_Thread_id = -1;
    mtx->_Type      = type;
    mtx->_Count     = 0;
}

// TRANSITION, ABI: preserved for binary compatibility
_CRTIMP2_PURE void __cdecl _Mtx_destroy_in_situ(_Mtx_t mtx) noexcept { // destroy mutex in situ
    _THREAD_ASSERT(mtx->_Count == 0, "mutex destroyed while busy");
    (void) mtx;
}

// TRANSITION, ABI: preserved for binary compatibility
_CRTIMP2_PURE _Thrd_result __cdecl _Mtx_init(_Mtx_t* mtx, int type) noexcept { // initialize mutex
    *mtx = nullptr;

    _Mtx_t mutex = static_cast<_Mtx_t>(_calloc_crt(1, sizeof(_Mtx_internal_imp_t)));

    if (mutex == nullptr) {
        return _Thrd_result::_Nomem; // report alloc failed
    }

    _Mtx_init_in_situ(mutex, type);

    *mtx = mutex;
    return _Thrd_result::_Success;
}

// TRANSITION, ABI: preserved for binary compatibility
_CRTIMP2_PURE void __cdecl _Mtx_destroy(_Mtx_t mtx) noexcept { // destroy mutex
    if (mtx) { // something to do, do it
        _Mtx_destroy_in_situ(mtx);
        _free_crt(mtx);
    }
}

static _Thrd_result mtx_do_lock(_Mtx_t mtx, const _timespec64* target) noexcept { // lock mutex
    // TRANSITION, ABI: the use of `const _timespec64*` is preserved for `_Mtx_timedlock`
    const auto current_thread_id = static_cast<long>(GetCurrentThreadId());
    if ((mtx->_Type & ~_Mtx_recursive) == _Mtx_plain) { // set the lock
        // TRANSITION, ABI: this branch is preserved for `_Thrd_create`

        if (mtx->_Thread_id != current_thread_id) { // not current thread, do lock
            AcquireSRWLockExclusive(get_srw_lock(mtx));
            mtx->_Thread_id = current_thread_id;
        }
        ++mtx->_Count;

        return _Thrd_result::_Success;
    } else { // handle timed or recursive mutex
        int res = WAIT_TIMEOUT;
        if (target == nullptr) { // no target --> plain wait (i.e. infinite timeout)
            if (mtx->_Thread_id != current_thread_id) {
                AcquireSRWLockExclusive(get_srw_lock(mtx));
            }

            res = WAIT_OBJECT_0;

        } else if (target->tv_sec < 0 || target->tv_sec == 0 && target->tv_nsec <= 0) {
            // target time <= 0 --> plain trylock or timed wait for time that has passed; try to lock with 0 timeout
            if (mtx->_Thread_id != current_thread_id) { // not this thread, lock it
                if (TryAcquireSRWLockExclusive(get_srw_lock(mtx)) != 0) {
                    res = WAIT_OBJECT_0;
                }
            } else {
                res = WAIT_OBJECT_0;
            }

        } else { // check timeout
            // TRANSITION, ABI: this branch is preserved for `_Mtx_timedlock`
            _timespec64 now;
            _Timespec64_get_sys(&now);
            while (now.tv_sec < target->tv_sec || now.tv_sec == target->tv_sec && now.tv_nsec < target->tv_nsec) {
                // time has not expired
                if (mtx->_Thread_id == current_thread_id
                    || TryAcquireSRWLockExclusive(get_srw_lock(mtx)) != 0) { // stop waiting
                    res = WAIT_OBJECT_0;
                    break;
                }

                _Timespec64_get_sys(&now);
            }
        }

        if (res == WAIT_OBJECT_0) {
            if (1 < ++mtx->_Count) { // check count
                if ((mtx->_Type & _Mtx_recursive) != _Mtx_recursive) { // not recursive, fixup count
                    --mtx->_Count;
                    res = WAIT_TIMEOUT;
                }
            } else {
                mtx->_Thread_id = current_thread_id;
            }
        }

        if (res == WAIT_OBJECT_0) {
            return _Thrd_result::_Success;
        }

        // res is WAIT_TIMEOUT here

        if (target == nullptr || (target->tv_sec == 0 && target->tv_nsec == 0)) {
            return _Thrd_result::_Busy;
        }

        return _Thrd_result::_Timedout;
    }
}

_CRTIMP2_PURE _Thrd_result __cdecl _Mtx_unlock(_Mtx_t mtx) noexcept { // unlock mutex
    _THREAD_ASSERT(mtx->_Count > 0, "unlock of unowned mutex");
    _THREAD_ASSERT(
        mtx->_Thread_id == static_cast<long>(GetCurrentThreadId()), "unlock of mutex not owned by the current thread");

    if (--mtx->_Count == 0) { // leave critical section
        mtx->_Thread_id = -1;

        auto srw_lock = get_srw_lock(mtx);
        _Analysis_assume_lock_held_(*srw_lock);
        ReleaseSRWLockExclusive(srw_lock);
    }
    return _Thrd_result::_Success; // TRANSITION, ABI: Always succeeds
}

_CRTIMP2_PURE _Thrd_result __cdecl _Mtx_lock(_Mtx_t mtx) noexcept { // lock mutex
    return mtx_do_lock(mtx, nullptr);
}

_CRTIMP2_PURE _Thrd_result __cdecl _Mtx_trylock(_Mtx_t mtx) noexcept { // attempt to lock try_mutex
    _timespec64 xt;
    _THREAD_ASSERT((mtx->_Type & (_Mtx_try | _Mtx_timed)) != 0, "trylock not supported by mutex");
    xt.tv_sec  = 0;
    xt.tv_nsec = 0;
    return mtx_do_lock(mtx, &xt);
}

// TRANSITION, ABI: preserved for binary compatibility
_CRTIMP2_PURE _Thrd_result __cdecl _Mtx_timedlock(_Mtx_t mtx, const _timespec64* xt) noexcept {
    // attempt to lock timed mutex
    _Thrd_result res;

    _THREAD_ASSERT((mtx->_Type & _Mtx_timed) != 0, "timedlock not supported by mutex");
    res = mtx_do_lock(mtx, xt);
    return res == _Thrd_result::_Busy ? _Thrd_result::_Timedout : res;
}

_CRTIMP2_PURE int __cdecl _Mtx_current_owns(_Mtx_t mtx) noexcept { // test if current thread owns mutex
    return mtx->_Count != 0 && mtx->_Thread_id == static_cast<long>(GetCurrentThreadId());
}

// TRANSITION, ABI: preserved for binary compatibility
_CRTIMP2_PURE void* __cdecl _Mtx_getconcrtcs(_Mtx_t mtx) noexcept { // get internal cs impl
    return &mtx->_Critical_section;
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
