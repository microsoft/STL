// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// support for <execution>

#include <internal_shared.h>
#include <thread>
#include <xatomic_wait.h>

extern "C" {

[[nodiscard]] unsigned int __stdcall __std_parallel_algorithms_hw_threads() noexcept {
    static int _Cached_hw_concurrency = -1;
    int _Hw_concurrency               = __iso_volatile_load32(&_Cached_hw_concurrency);
    if (_Hw_concurrency == -1) {
        _Hw_concurrency = static_cast<int>(_STD thread::hardware_concurrency());
        __iso_volatile_store32(&_Cached_hw_concurrency, _Hw_concurrency);
    }

    return static_cast<unsigned int>(_Hw_concurrency);
}

[[nodiscard]] PTP_WORK __stdcall __std_create_threadpool_work(
    PTP_WORK_CALLBACK _Callback, void* _Context, PTP_CALLBACK_ENVIRON _Callback_environ) noexcept {
    return CreateThreadpoolWork(_Callback, _Context, _Callback_environ);
}

void __stdcall __std_submit_threadpool_work(PTP_WORK _Work) noexcept {
    SubmitThreadpoolWork(_Work);
}

void __stdcall __std_bulk_submit_threadpool_work(PTP_WORK _Work, const size_t _Submissions) noexcept {
    for (size_t _Idx = 0; _Idx < _Submissions; ++_Idx) {
        SubmitThreadpoolWork(_Work);
    }
}

void __stdcall __std_close_threadpool_work(PTP_WORK _Work) noexcept {
    CloseThreadpoolWork(_Work);
}

void __stdcall __std_wait_for_threadpool_work_callbacks(PTP_WORK _Work, BOOL _Cancel) noexcept {
    WaitForThreadpoolWorkCallbacks(_Work, _Cancel);
}

void __stdcall __std_execution_wait_on_uchar(const volatile unsigned char* _Address, unsigned char _Compare) noexcept {
    __std_atomic_wait_direct(const_cast<const unsigned char*>(_Address), &_Compare, 1, _Atomic_wait_no_timeout);
}

void __stdcall __std_execution_wake_by_address_all(const volatile void* _Address) noexcept {
    __std_atomic_notify_all_direct(const_cast<const void*>(_Address));
}

} // extern "C"
