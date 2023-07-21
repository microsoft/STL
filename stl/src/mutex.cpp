// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// mutex functions

#include <cstdio>
#include <cstdlib>
#include <internal_shared.h>
#include <mutex>
#include <type_traits>
#include <xthreads.h>
#include <xtimec.h>

#include "primitives.hpp"

extern "C" [[noreturn]] _CRTIMP2_PURE void _Thrd_abort(const char* msg) { // abort on precondition failure
    fputs(msg, stderr);
    fputc('\n', stderr);
    abort();
}

#if defined(_THREAD_CHECK) || defined(_DEBUG)
#define _THREAD_CHECKX 1
#else // defined(_THREAD_CHECK) || defined(_DEBUG)
#define _THREAD_CHECKX 0
#endif // defined(_THREAD_CHECK) || defined(_DEBUG)

#if _THREAD_CHECKX
#define _THREAD_QUOTX(x)          #x
#define _THREAD_QUOT(x)           _THREAD_QUOTX(x)
#define _THREAD_ASSERT(expr, msg) ((expr) ? (void) 0 : _Thrd_abort(__FILE__ "(" _THREAD_QUOT(__LINE__) "): " msg))
#else // _THREAD_CHECKX
#define _THREAD_ASSERT(expr, msg) ((void) 0)
#endif // _THREAD_CHECKX

// TRANSITION, ABI: preserved for binary compatibility
enum class __stl_sync_api_modes_enum { normal, win7, vista, concrt };
extern "C" _CRTIMP2 void __cdecl __set_stl_sync_api_mode(__stl_sync_api_modes_enum) {}

[[nodiscard]] static PSRWLOCK get_srw_lock(_Mtx_t mtx) {
    return reinterpret_cast<PSRWLOCK>(&mtx->_Critical_section._M_srw_lock);
}

// TRANSITION, only used when constexpr mutex constructor is not enabled
void _Mtx_init_in_situ(_Mtx_t mtx, int type) { // initialize mutex in situ
    Concurrency::details::create_stl_critical_section(&mtx->_Critical_section);
    mtx->_Thread_id = -1;
    mtx->_Type      = type;
    mtx->_Count     = 0;
}

void _Mtx_destroy_in_situ(_Mtx_t mtx) { // destroy mutex in situ
    _THREAD_ASSERT(mtx->_Count == 0, "mutex destroyed while busy");
    (void) mtx;
}

_Thrd_result _Mtx_init(_Mtx_t* mtx, int type) { // initialize mutex
    *mtx = nullptr;

    _Mtx_t mutex = static_cast<_Mtx_t>(_calloc_crt(1, sizeof(_Mtx_internal_imp_t)));

    if (mutex == nullptr) {
        return _Thrd_result::_Thrd_nomem; // report alloc failed
    }

    _Mtx_init_in_situ(mutex, type);

    *mtx = mutex;
    return _Thrd_result::_Thrd_success;
}

void _Mtx_destroy(_Mtx_t mtx) { // destroy mutex
    if (mtx) { // something to do, do it
        _Mtx_destroy_in_situ(mtx);
        _free_crt(mtx);
    }
}

static _Thrd_result mtx_do_lock(_Mtx_t mtx, const _timespec64* target) { // lock mutex
    if ((mtx->_Type & ~_Mtx_recursive) == _Mtx_plain) { // set the lock
        if (mtx->_Thread_id != static_cast<long>(GetCurrentThreadId())) { // not current thread, do lock
            AcquireSRWLockExclusive(get_srw_lock(mtx));
            mtx->_Thread_id = static_cast<long>(GetCurrentThreadId());
        }
        ++mtx->_Count;

        return _Thrd_result::_Thrd_success;
    } else { // handle timed or recursive mutex
        int res = WAIT_TIMEOUT;
        if (target == nullptr) { // no target --> plain wait (i.e. infinite timeout)
            if (mtx->_Thread_id != static_cast<long>(GetCurrentThreadId())) {
                AcquireSRWLockExclusive(get_srw_lock(mtx));
            }

            res = WAIT_OBJECT_0;

        } else if (target->tv_sec < 0 || target->tv_sec == 0 && target->tv_nsec <= 0) {
            // target time <= 0 --> plain trylock or timed wait for time that has passed; try to lock with 0 timeout
            if (mtx->_Thread_id != static_cast<long>(GetCurrentThreadId())) { // not this thread, lock it
                if (TryAcquireSRWLockExclusive(get_srw_lock(mtx)) != 0) {
                    res = WAIT_OBJECT_0;
                } else {
                    res = WAIT_TIMEOUT;
                }
            } else {
                res = WAIT_OBJECT_0;
            }

        } else { // check timeout
            _timespec64 now;
            _Timespec64_get_sys(&now);
            while (now.tv_sec < target->tv_sec || now.tv_sec == target->tv_sec && now.tv_nsec < target->tv_nsec) {
                // time has not expired
                if (mtx->_Thread_id == static_cast<long>(GetCurrentThreadId())
                    || TryAcquireSRWLockExclusive(get_srw_lock(mtx)) != 0) { // stop waiting
                    res = WAIT_OBJECT_0;
                    break;
                } else {
                    res = WAIT_TIMEOUT;
                }

                _Timespec64_get_sys(&now);
            }
        }

        if (res == WAIT_OBJECT_0 || res == WAIT_ABANDONED) {
            if (1 < ++mtx->_Count) { // check count
                if ((mtx->_Type & _Mtx_recursive) != _Mtx_recursive) { // not recursive, fixup count
                    --mtx->_Count;
                    res = WAIT_TIMEOUT;
                }
            } else {
                mtx->_Thread_id = static_cast<long>(GetCurrentThreadId());
            }
        }

        switch (res) {
        case WAIT_OBJECT_0:
        case WAIT_ABANDONED:
            return _Thrd_result::_Thrd_success;

        case WAIT_TIMEOUT:
            if (target == nullptr || (target->tv_sec == 0 && target->tv_nsec == 0)) {
                return _Thrd_result::_Thrd_busy;
            } else {
                return _Thrd_result::_Thrd_timedout;
            }

        default:
            return _Thrd_result::_Thrd_error;
        }
    }
}

_Thrd_result _Mtx_unlock(_Mtx_t mtx) { // unlock mutex
    _THREAD_ASSERT(
        1 <= mtx->_Count && mtx->_Thread_id == static_cast<long>(GetCurrentThreadId()), "unlock of unowned mutex");

    if (--mtx->_Count == 0) { // leave critical section
        mtx->_Thread_id = -1;

        auto srw_lock = get_srw_lock(mtx);
        _Analysis_assume_lock_held_(*srw_lock);
        ReleaseSRWLockExclusive(srw_lock);
    }
    return _Thrd_result::_Thrd_success; // TRANSITION, ABI: Always succeeds
}

_Thrd_result _Mtx_lock(_Mtx_t mtx) { // lock mutex
    return mtx_do_lock(mtx, nullptr);
}

_Thrd_result _Mtx_trylock(_Mtx_t mtx) { // attempt to lock try_mutex
    _timespec64 xt;
    _THREAD_ASSERT((mtx->_Type & (_Mtx_try | _Mtx_timed)) != 0, "trylock not supported by mutex");
    xt.tv_sec  = 0;
    xt.tv_nsec = 0;
    return mtx_do_lock(mtx, &xt);
}

_Thrd_result _Mtx_timedlock(_Mtx_t mtx, const _timespec64* xt) { // attempt to lock timed mutex
    _Thrd_result res;

    _THREAD_ASSERT((mtx->_Type & _Mtx_timed) != 0, "timedlock not supported by mutex");
    res = mtx_do_lock(mtx, xt);
    return res == _Thrd_result::_Thrd_busy ? _Thrd_result::_Thrd_timedout : res;
}

int _Mtx_current_owns(_Mtx_t mtx) { // test if current thread owns mutex
    return mtx->_Count != 0 && mtx->_Thread_id == static_cast<long>(GetCurrentThreadId());
}

void* _Mtx_getconcrtcs(_Mtx_t mtx) { // get internal cs impl
    return &mtx->_Critical_section;
}

void _Mtx_clear_owner(_Mtx_t mtx) { // set owner to nobody
    mtx->_Thread_id = -1;
    --mtx->_Count;
}

void _Mtx_reset_owner(_Mtx_t mtx) { // set owner to current thread
    mtx->_Thread_id = static_cast<long>(GetCurrentThreadId());
    ++mtx->_Count;
}

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
