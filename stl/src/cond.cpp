// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// condition variable functions

#include "primitives.h"
#include <internal_shared.h>
#include <limits.h>
#include <stdlib.h>
#include <type_traits>
#include <xthreads.h>
#include <xtimec.h>

struct _Cnd_internal_imp_t { // condition variable implementation for ConcRT
    std::aligned_storage_t<Concurrency::details::stl_condition_variable_max_size,
        Concurrency::details::stl_condition_variable_max_alignment>
        cv;
    Concurrency::details::stl_condition_variable_interface* _get_cv() { // get pointer to implementation
        return reinterpret_cast<Concurrency::details::stl_condition_variable_interface*>(&cv);
    }
};

static_assert(sizeof(_Cnd_internal_imp_t) <= _Cnd_internal_imp_size, "incorrect _Cnd_internal_imp_size");
static_assert(std::alignment_of<_Cnd_internal_imp_t>::value <= _Cnd_internal_imp_alignment,
    "incorrect _Cnd_internal_imp_alignment");

void _Cnd_init_in_situ(_Cnd_t cond) { // initialize condition variable in situ
    Concurrency::details::create_stl_condition_variable(cond->_get_cv());
}

void _Cnd_destroy_in_situ(_Cnd_t cond) { // destroy condition variable in situ
    cond->_get_cv()->destroy();
}

int _Cnd_init(_Cnd_t* pcond) { // initialize
    _Cnd_t cond;
    *pcond = 0;

    if ((cond = static_cast<_Cnd_t>(_calloc_crt(1, sizeof(struct _Cnd_internal_imp_t)))) == 0) {
        return _Thrd_nomem; // report alloc failed
    } else { // report success
        _Cnd_init_in_situ(cond);
        *pcond = cond;
        return _Thrd_success;
    }
}

void _Cnd_destroy(_Cnd_t cond) { // clean up
    if (cond) { // something to do, do it
        _Cnd_destroy_in_situ(cond);
        _free_crt(cond);
    }
}

static int do_wait(_Cnd_t cond, _Mtx_t mtx, const xtime* target) { // wait for signal or timeout
    int res = _Thrd_success;
    auto cs = static_cast<Concurrency::details::stl_critical_section_interface*>(_Mtx_getconcrtcs(mtx));
    if (target == 0) { // no target time specified, wait on mutex
        _Mtx_clear_owner(mtx);
        cond->_get_cv()->wait(cs);
        _Mtx_reset_owner(mtx);
    } else { // target time specified, wait for it
        xtime now;
        xtime_get(&now, TIME_UTC);
        _Mtx_clear_owner(mtx);
        if (!cond->_get_cv()->wait_for(cs, _Xtime_diff_to_millis2(target, &now))) { // report timeout
            xtime_get(&now, TIME_UTC);
            if (_Xtime_diff_to_millis2(target, &now) == 0) {
                res = _Thrd_timedout;
            }
        }
        _Mtx_reset_owner(mtx);
    }
    return res;
}

static int do_signal(_Cnd_t cond, int all) { // release threads
    if (all) {
        cond->_get_cv()->notify_all();
    } else {
        cond->_get_cv()->notify_one();
    }

    return _Thrd_success;
}

int _Cnd_wait(_Cnd_t cond, _Mtx_t mtx) { // wait until signaled
    return do_wait(cond, mtx, 0);
}

int _Cnd_timedwait(_Cnd_t cond, _Mtx_t mtx, const xtime* xt) { // wait until signaled or timeout
    return do_wait(cond, mtx, xt);
}

int _Cnd_signal(_Cnd_t cond) { // release one waiting thread
    return do_signal(cond, 0);
}

int _Cnd_broadcast(_Cnd_t cond) { // release all waiting threads
    return do_signal(cond, 1);
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
