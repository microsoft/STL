// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// mutex functions

#include "primitives.h"
#include <internal_shared.h>
#include <stdio.h>
#include <stdlib.h>
#include <type_traits>
#include <xthreads.h>
#include <xtimec.h>

extern "C" _CRTIMP2_PURE void _Thrd_abort(const char* msg) { // abort on precondition failure
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
#define _THREAD_QUOTX(x) #x
#define _THREAD_QUOT(x) _THREAD_QUOTX(x)
#define _THREAD_ASSERT(expr, msg) ((expr) ? (void) 0 : _Thrd_abort(__FILE__ "(" _THREAD_QUOT(__LINE__) "): " msg))
#else // _THREAD_CHECKX
#define _THREAD_ASSERT(expr, msg) ((void) 0)
#endif // _THREAD_CHECKX

__stl_sync_api_modes_enum __stl_sync_api_impl_mode = __stl_sync_api_modes_enum::normal;

extern "C" _CRTIMP2 void __cdecl __set_stl_sync_api_mode(__stl_sync_api_modes_enum mode) {
    __stl_sync_api_impl_mode = mode;
}

struct _Mtx_internal_imp_t { // ConcRT mutex
    int type;
    std::aligned_storage_t<Concurrency::details::stl_critical_section_max_size,
        Concurrency::details::stl_critical_section_max_alignment>
        cs;
    long thread_id;
    int count;
    Concurrency::details::stl_critical_section_interface* _get_cs() { // get pointer to implementation
        return reinterpret_cast<Concurrency::details::stl_critical_section_interface*>(&cs);
    }
};

static_assert(sizeof(_Mtx_internal_imp_t) <= _Mtx_internal_imp_size, "incorrect _Mtx_internal_imp_size");
static_assert(std::alignment_of<_Mtx_internal_imp_t>::value <= _Mtx_internal_imp_alignment,
    "incorrect _Mtx_internal_imp_alignment");

void _Mtx_init_in_situ(_Mtx_t mtx, int type) { // initialize mutex in situ
    Concurrency::details::create_stl_critical_section(mtx->_get_cs());
    mtx->thread_id = -1;
    mtx->type      = type;
    mtx->count     = 0;
}

void _Mtx_destroy_in_situ(_Mtx_t mtx) { // destroy mutex in situ
    _THREAD_ASSERT(mtx->count == 0, "mutex destroyed while busy");
    mtx->_get_cs()->destroy();
}

int _Mtx_init(_Mtx_t* mtx, int type) { // initialize mutex
    _Mtx_t mutex;
    *mtx = 0;

    if ((mutex = static_cast<_Mtx_t>(_calloc_crt(1, sizeof(struct _Mtx_internal_imp_t)))) == 0) {
        return _Thrd_nomem; // report alloc failed
    }

    _Mtx_init_in_situ(mutex, type);

    *mtx = mutex;
    return _Thrd_success;
}

void _Mtx_destroy(_Mtx_t mtx) { // destroy mutex
    if (mtx) { // something to do, do it
        _Mtx_destroy_in_situ(mtx);
        _free_crt(mtx);
    }
}

static int mtx_do_lock(_Mtx_t mtx, const xtime* target) { // lock mutex
    if ((mtx->type & ~_Mtx_recursive) == _Mtx_plain) { // set the lock
        if (mtx->thread_id != static_cast<long>(GetCurrentThreadId())) { // not current thread, do lock
            mtx->_get_cs()->lock();
            mtx->thread_id = static_cast<long>(GetCurrentThreadId());
        }
        ++mtx->count;

        return _Thrd_success;
    } else { // handle timed or recursive mutex
        int res = WAIT_TIMEOUT;
        if (target == 0) { // no target --> plain wait (i.e. infinite timeout)
            if (mtx->thread_id != static_cast<long>(GetCurrentThreadId())) {
                mtx->_get_cs()->lock();
            }

            res = WAIT_OBJECT_0;

        } else if (target->sec < 0 || target->sec == 0 && target->nsec <= 0) {
            // target time <= 0 --> plain trylock or timed wait for time that has passed; try to lock with 0 timeout
            if (mtx->thread_id != static_cast<long>(GetCurrentThreadId())) { // not this thread, lock it
                if (mtx->_get_cs()->try_lock()) {
                    res = WAIT_OBJECT_0;
                } else {
                    res = WAIT_TIMEOUT;
                }
            } else {
                res = WAIT_OBJECT_0;
            }

        } else { // check timeout
            xtime now;
            xtime_get(&now, TIME_UTC);
            while (now.sec < target->sec || now.sec == target->sec && now.nsec < target->nsec) { // time has not expired
                if (mtx->thread_id == static_cast<long>(GetCurrentThreadId())
                    || mtx->_get_cs()->try_lock_for(_Xtime_diff_to_millis2(target, &now))) { // stop waiting
                    res = WAIT_OBJECT_0;
                    break;
                } else {
                    res = WAIT_TIMEOUT;
                }

                xtime_get(&now, TIME_UTC);
            }
        }
        if (res == WAIT_OBJECT_0 || res == WAIT_ABANDONED) {
            if (1 < ++mtx->count) { // check count
                if ((mtx->type & _Mtx_recursive) != _Mtx_recursive) { // not recursive, fixup count
                    --mtx->count;
                    res = WAIT_TIMEOUT;
                }
            } else {
                mtx->thread_id = static_cast<long>(GetCurrentThreadId());
            }
        }

        switch (res) {
        case WAIT_OBJECT_0:
        case WAIT_ABANDONED:
            return _Thrd_success;

        case WAIT_TIMEOUT:
            if (target == 0 || (target->sec == 0 && target->nsec == 0)) {
                return _Thrd_busy;
            } else {
                return _Thrd_timedout;
            }

        default:
            return _Thrd_error;
        }
    }
}

int _Mtx_unlock(_Mtx_t mtx) { // unlock mutex
    _THREAD_ASSERT(
        1 <= mtx->count && mtx->thread_id == static_cast<long>(GetCurrentThreadId()), "unlock of unowned mutex");

    if (--mtx->count == 0) { // leave critical section
        mtx->thread_id = -1;
        mtx->_get_cs()->unlock();
    }
    return _Thrd_success;
}

int _Mtx_lock(_Mtx_t mtx) { // lock mutex
    return mtx_do_lock(mtx, 0);
}

int _Mtx_trylock(_Mtx_t mtx) { // attempt to lock try_mutex
    xtime xt;
    _THREAD_ASSERT((mtx->type & (_Mtx_try | _Mtx_timed)) != 0, "trylock not supported by mutex");
    xt.sec  = 0;
    xt.nsec = 0;
    return mtx_do_lock(mtx, &xt);
}

int _Mtx_timedlock(_Mtx_t mtx, const xtime* xt) { // attempt to lock timed mutex
    int res;

    _THREAD_ASSERT((mtx->type & _Mtx_timed) != 0, "timedlock not supported by mutex");
    res = mtx_do_lock(mtx, xt);
    return res == _Thrd_busy ? _Thrd_timedout : res;
}

int _Mtx_current_owns(_Mtx_t mtx) { // test if current thread owns mutex
    return mtx->count != 0 && mtx->thread_id == static_cast<long>(GetCurrentThreadId());
}

void* _Mtx_getconcrtcs(_Mtx_t mtx) { // get internal cs impl
    return mtx->_get_cs();
}

void _Mtx_clear_owner(_Mtx_t mtx) { // set owner to nobody
    mtx->thread_id = -1;
    --mtx->count;
}

void _Mtx_reset_owner(_Mtx_t mtx) { // set owner to current thread
    mtx->thread_id = static_cast<long>(GetCurrentThreadId());
    ++mtx->count;
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
