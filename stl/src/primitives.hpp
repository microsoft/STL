// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <__msvc_threads_core.hpp>
#include <cstdlib>

#include <Windows.h>

inline bool _Primitive_wait_for(const _Cnd_t cond, const _Mtx_t mtx, const unsigned int timeout) noexcept {
    const auto pcv  = reinterpret_cast<PCONDITION_VARIABLE>(&cond->_Stl_cv._Win_cv);
    const auto psrw = reinterpret_cast<PSRWLOCK>(&mtx->_Critical_section._M_srw_lock);
    return SleepConditionVariableSRW(pcv, psrw, timeout, 0) != 0;
}

inline void _Primitive_wait(const _Cnd_t cond, const _Mtx_t mtx) noexcept {
    if (!_Primitive_wait_for(cond, mtx, INFINITE)) {
        _CSTD abort();
    }
}

inline void _Primitive_notify_one(const _Cnd_t cond) noexcept {
    const auto pcv = reinterpret_cast<PCONDITION_VARIABLE>(&cond->_Stl_cv._Win_cv);
    WakeConditionVariable(pcv);
}

inline void _Primitive_notify_all(const _Cnd_t cond) noexcept {
    const auto pcv = reinterpret_cast<PCONDITION_VARIABLE>(&cond->_Stl_cv._Win_cv);
    WakeAllConditionVariable(pcv);
}
