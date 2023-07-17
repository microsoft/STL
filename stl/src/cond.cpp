// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// condition variable functions

#include <cstdlib>
#include <internal_shared.h>
#include <type_traits>
#include <xthreads.h>
#include <xtimec.h>

#include "primitives.hpp"

struct _Cnd_internal_imp_t { // condition variable implementation for ConcRT
    typename std::_Aligned_storage<Concurrency::details::stl_condition_variable_max_size,
        Concurrency::details::stl_condition_variable_max_alignment>::type cv;

    [[nodiscard]] Concurrency::details::stl_condition_variable_win7* _get_cv() noexcept {
        // get pointer to implementation
        return reinterpret_cast<Concurrency::details::stl_condition_variable_win7*>(&cv);
    }
};

static_assert(sizeof(_Cnd_internal_imp_t) == _Cnd_internal_imp_size, "incorrect _Cnd_internal_imp_size");
static_assert(alignof(_Cnd_internal_imp_t) == _Cnd_internal_imp_alignment, "incorrect _Cnd_internal_imp_alignment");

void _Cnd_init_in_situ(const _Cnd_t cond) { // initialize condition variable in situ
    Concurrency::details::create_stl_condition_variable(cond->_get_cv());
}

void _Cnd_destroy_in_situ(_Cnd_t) {} // destroy condition variable in situ

int _Cnd_init(_Cnd_t* const pcond) { // initialize
    *pcond = nullptr;

    const auto cond = static_cast<_Cnd_t>(_calloc_crt(1, sizeof(_Cnd_internal_imp_t)));
    if (cond == nullptr) {
        return _Thrd_nomem; // report alloc failed
    }

    _Cnd_init_in_situ(cond);
    *pcond = cond;
    return _Thrd_success;
}

void _Cnd_destroy(const _Cnd_t cond) { // clean up
    if (cond) { // something to do, do it
        _Cnd_destroy_in_situ(cond);
        _free_crt(cond);
    }
}

int _Cnd_wait(const _Cnd_t cond, const _Mtx_t mtx) { // wait until signaled
    const auto cs = static_cast<Concurrency::details::stl_critical_section_win7*>(_Mtx_getconcrtcs(mtx));
    _Mtx_clear_owner(mtx);
    cond->_get_cv()->wait(cs);
    _Mtx_reset_owner(mtx);
    return _Thrd_success; // TRANSITION, ABI: Always returns _Thrd_success
}

// wait until signaled or timeout
int _Cnd_timedwait(const _Cnd_t cond, const _Mtx_t mtx, const _timespec64* const target) {
    int res       = _Thrd_success;
    const auto cs = static_cast<Concurrency::details::stl_critical_section_win7*>(_Mtx_getconcrtcs(mtx));
    if (target == nullptr) { // no target time specified, wait on mutex
        _Mtx_clear_owner(mtx);
        cond->_get_cv()->wait(cs);
        _Mtx_reset_owner(mtx);
    } else { // target time specified, wait for it
        _timespec64 now;
        _Timespec64_get_sys(&now);
        _Mtx_clear_owner(mtx);
        if (!cond->_get_cv()->wait_for(cs, _Xtime_diff_to_millis2(target, &now))) { // report timeout
            _Timespec64_get_sys(&now);
            if (_Xtime_diff_to_millis2(target, &now) == 0) {
                res = _Thrd_timedout;
            }
        }
        _Mtx_reset_owner(mtx);
    }
    return res;
}

int _Cnd_signal(const _Cnd_t cond) { // release one waiting thread
    cond->_get_cv()->notify_one();
    return _Thrd_success; // TRANSITION, ABI: Always returns _Thrd_success
}

int _Cnd_broadcast(const _Cnd_t cond) { // release all waiting threads
    cond->_get_cv()->notify_all();
    return _Thrd_success; // TRANSITION, ABI: Always returns _Thrd_success
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
