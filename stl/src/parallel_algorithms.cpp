// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// support for <execution>

#include <corecrt_terminate.h>
#include <internal_shared.h>
#include <intrin0.h>
#include <xatomic_wait.h>

// This must be as small as possible, because its contents are
// injected into the msvcprt.lib and msvcprtd.lib import libraries.
// Do not include or define anything else here.
// In particular, basic_string must not be included here.

#if _STL_WIN32_WINNT < _WIN32_WINNT_WIN8
namespace {
    struct _Parallel_init_info {
        unsigned int _Hw_threads;
    };

    _Parallel_init_info _Parallel_info;

    unsigned char _Atomic_load_uchar(const volatile unsigned char* _Ptr) noexcept {
        // atomic load of unsigned char, copied from <atomic> except ARM and ARM64 bits
        unsigned char _Value;
#if defined(_M_IX86) || defined(_M_X64) || defined(_M_ARM) || defined(_M_ARM64)
        _Value = __iso_volatile_load8(reinterpret_cast<const volatile char*>(_Ptr));
        _ReadWriteBarrier();
#else
#error Unsupported architecture
#endif
        return _Value;
    }

    unsigned int _Atomic_load_uint(const volatile unsigned int* _Ptr) noexcept {
        // atomic load of unsigned int, copied from <atomic> except ARM and ARM64 bits
        unsigned int _Value;
#if defined(_M_IX86) || defined(_M_X64) || defined(_M_ARM) || defined(_M_ARM64)
        _Value = __iso_volatile_load32(reinterpret_cast<const volatile int*>(_Ptr));
        _ReadWriteBarrier();
#else 
#error Unsupported architecture
#endif 
        return _Value;
    }

    void _Atomic_store_uint(volatile unsigned int* _Tgt, unsigned int _Value) {
        // atomic store of unsigned int, copied from <atomic>
#if defined(_M_IX86) || defined(_M_X64) || defined(_M_ARM) || defined(_M_ARM64)
        _InterlockedExchange(reinterpret_cast<volatile long*>(_Tgt), static_cast<long>(_Value));
#else
#error Unsupported architecture
#endif
    }

} // unnamed namespace
#endif // _STL_WIN32_WINNT < _WIN32_WINNT_WIN8

static DWORD _Get_number_of_processors() noexcept {
    SYSTEM_INFO _Info;
    GetNativeSystemInfo(&_Info);
    return _Info.dwNumberOfProcessors;
}

extern "C" {

_NODISCARD unsigned int __stdcall __std_parallel_algorithms_hw_threads() noexcept {
      // _Atomic_load_uint enforces memory ordering in _Initialize_parallel_init_info:
    unsigned int _Result = _Atomic_load_uint(&_Parallel_info._Hw_threads);
    if (_Result == 0) {
        _Result = _Get_number_of_processors();
        _Atomic_store_uint(&_Parallel_info._Hw_threads, _Result);
    }

    return _Result;
}

// Relaxed reads of _Parallel_info below because __std_parallel_algorithms_hw_threads must be called
// before any of these on each thread.

_NODISCARD PTP_WORK __stdcall __std_create_threadpool_work(
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
    _Atomic_wait_context_t _Wait_context;

#if _STL_WIN32_WINNT < _WIN32_WINNT_WIN8
    while (_Atomic_wait_spin(_Wait_context._Wait_phase_and_spin_count, true)) {
        if (_Atomic_load_uchar(_Address) != _Compare) {
            return;
        }
    }

    for (;;) {
        __std_atomic_wait_direct(const_cast<const unsigned char*>(_Address), &_Compare, 1, _Wait_context);
        if (_Atomic_load_uchar(_Address) != _Compare) {
            break;
        }
    }

    if (_Wait_context._Wait_phase_and_spin_count & _Atomic_unwait_needed) {
        __std_atomic_unwait_direct(const_cast<const unsigned char*>(_Address), _Wait_context);
    }

#else // ^^^ pre-Win8 / Win8+ vvv
    __std_atomic_wait_direct(const_cast<const unsigned char*>(_Address), &_Compare, 1, _Wait_context);
#endif // _STL_WIN32_WINNT >= _WIN32_WINNT_WIN8
}

void __stdcall __std_execution_wake_by_address_all(const volatile void* _Address) noexcept {
    __std_atomic_notify_all_direct(const_cast<const void*>(_Address));
}

} // extern "C"
