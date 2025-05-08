// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <bit>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <process.h>
#include <xthreads.h>

#include <Windows.h>

namespace {
    using _Thrd_start_t = int (*)(void*);

    struct _Thrd_binder { // bind function pointer and data to pass to thread entry point
        _Thrd_start_t func;
        void* data;
        _Cnd_t* cond;
        _Mtx_t* mtx;
        int* started;
    };

    using _Thrd_callback_t = unsigned int(__stdcall*)(void*);

    unsigned int __stdcall _Thrd_runner(void* d) { // call thread function
        _Thrd_binder b = *static_cast<_Thrd_binder*>(d);
        _Mtx_lock(*b.mtx);
        *b.started = 1;
        _Cnd_signal(*b.cond);
        _Mtx_unlock(*b.mtx);
        const unsigned int res = b.func(b.data);
        _Cnd_do_broadcast_at_thread_exit();
        return res;
    }
} // unnamed namespace

extern "C" {

// TRANSITION, ABI: _Thrd_exit() is preserved for binary compatibility
[[noreturn]] _CRTIMP2_PURE void __cdecl _Thrd_exit(int res) noexcept { // terminate execution of calling thread
    _endthreadex(res);
}

// TRANSITION, ABI: _Thrd_start() is preserved for binary compatibility
_CRTIMP2_PURE _Thrd_result __cdecl _Thrd_start(_Thrd_t* thr, _Thrd_callback_t func, void* b) noexcept {
    // start a thread
    thr->_Hnd = reinterpret_cast<HANDLE>(_beginthreadex(nullptr, 0, func, b, 0, &thr->_Id));
    return thr->_Hnd == nullptr ? _Thrd_result::_Error : _Thrd_result::_Success;
}

_CRTIMP2_PURE _Thrd_result __cdecl _Thrd_join(_Thrd_t thr, int* code) noexcept { // returns when thread terminates
    if (WaitForSingleObjectEx(thr._Hnd, INFINITE, FALSE) == WAIT_FAILED) {
        return _Thrd_result::_Error;
    }

    if (code) { // TRANSITION, ABI: code is preserved for binary compatibility
        unsigned long res;
        if (!GetExitCodeThread(thr._Hnd, &res)) {
            return _Thrd_result::_Error;
        }
        *code = static_cast<int>(res);
    }

    return CloseHandle(thr._Hnd) ? _Thrd_result::_Success : _Thrd_result::_Error;
}

_CRTIMP2_PURE _Thrd_result __cdecl _Thrd_detach(_Thrd_t thr) noexcept {
    // tell OS to release thread's resources when it terminates
    return CloseHandle(thr._Hnd) ? _Thrd_result::_Success : _Thrd_result::_Error;
}

// TRANSITION, ABI: _Thrd_sleep() is preserved for binary compatibility
_CRTIMP2_PURE void __cdecl _Thrd_sleep(const _timespec64* xt) noexcept { // suspend thread until time xt
    _timespec64 now;
    _Timespec64_get_sys(&now);
    do { // sleep and check time
        Sleep(_Xtime_diff_to_millis2(xt, &now));
        _Timespec64_get_sys(&now);
    } while (now.tv_sec < xt->tv_sec || now.tv_sec == xt->tv_sec && now.tv_nsec < xt->tv_nsec);
}

_CRTIMP2_PURE void __cdecl _Thrd_yield() noexcept { // surrender remainder of timeslice
    SwitchToThread();
}

// TRANSITION, ABI: _Thrd_equal() is preserved for binary compatibility
_CRTIMP2_PURE int __cdecl _Thrd_equal(_Thrd_t thr0, _Thrd_t thr1) noexcept {
    // return 1 if thr0 and thr1 identify same thread
    return thr0._Id == thr1._Id;
}

// TRANSITION, ABI: _Thrd_current() is preserved for binary compatibility
_CRTIMP2_PURE _Thrd_t __cdecl _Thrd_current() noexcept { // return _Thrd_t identifying current thread
    _Thrd_t result;
    result._Hnd = nullptr;
    result._Id  = GetCurrentThreadId();
    return result;
}

_CRTIMP2_PURE _Thrd_id_t __cdecl _Thrd_id() noexcept { // return unique id for current thread
    return GetCurrentThreadId();
}

_CRTIMP2_PURE unsigned int __cdecl _Thrd_hardware_concurrency() noexcept { // return number of processors
    // Most devices have only one processor group and thus have the same buffer_size.
#ifdef _WIN64
    constexpr int stack_buffer_size = 48; // 16 bytes per group
#else // ^^^ 64-bit / 32-bit vvv
    constexpr int stack_buffer_size = 44; // 12 bytes per group
#endif // ^^^ 32-bit ^^^

    alignas(SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX) unsigned char stack_buffer[stack_buffer_size];
    unsigned char* buffer_ptr = stack_buffer;
    DWORD buffer_size         = stack_buffer_size;
    _STD unique_ptr<unsigned char[]> new_buffer;

    // https://learn.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-getlogicalprocessorinformationex
    // The buffer "receives a sequence of variable-sized SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX structures".
    for (;;) {
        if (GetLogicalProcessorInformationEx(RelationProcessorPackage,
                reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(buffer_ptr), &buffer_size)) {
            unsigned int logical_processors = 0;

            while (buffer_size > 0) {
                // Each structure in the buffer describes a processor package (aka socket)...
                const auto structure_ptr  = reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(buffer_ptr);
                const auto structure_size = structure_ptr->Size;

                // ... which contains one or more processor groups.
                for (WORD i = 0; i != structure_ptr->Processor.GroupCount; ++i) {
                    logical_processors += _STD popcount(structure_ptr->Processor.GroupMask[i].Mask);
                }

                // Step forward to the next structure in the buffer.
                buffer_ptr += structure_size;
                buffer_size -= structure_size;
            }

            return logical_processors;
        }

        if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
            return 0; // API failure
        }

        new_buffer.reset(::new (_STD nothrow) unsigned char[buffer_size]);

        if (!new_buffer) {
            return 0; // allocation failure
        }

        buffer_ptr = new_buffer.get();
    }
}

// TRANSITION, ABI: _Thrd_create() is preserved for binary compatibility
_CRTIMP2_PURE _Thrd_result __cdecl _Thrd_create(_Thrd_t* thr, _Thrd_start_t func, void* d) noexcept { // create thread
    _Thrd_result res;
    _Thrd_binder b;
    int started = 0;
    _Cnd_internal_imp_t cond_var{};
    _Cnd_t cond = &cond_var;
    _Mtx_internal_imp_t mtx_var{};
    _Mtx_t mtx = &mtx_var;
    _Mtx_init_in_situ(mtx, _Mtx_plain);
    b.func    = func;
    b.data    = d;
    b.cond    = &cond;
    b.mtx     = &mtx;
    b.started = &started;
    _Mtx_lock(mtx);
    if ((res = _Thrd_start(thr, _Thrd_runner, &b)) == _Thrd_result::_Success) { // wait for handshake
        while (!started) {
            _Cnd_wait(cond, mtx);
        }
    }
    _Mtx_unlock(mtx);
    return res;
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
