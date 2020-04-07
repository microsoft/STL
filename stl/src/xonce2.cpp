// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// __std_execute_once_begin and __std_execute_once_complete functions

#include <atomic>
#include <libloaderapi.h>
#include <synchapi.h>
#include <xcall_once.h>

// This must be as small as possible, because its contents are
// injected into the msvcprt.lib and msvcprtd.lib import libraries.
// Do not include or define anything else here.
// In particular, basic_string must not be included here.

static_assert(sizeof(std::once_flag::_Opaque) == sizeof(INIT_ONCE), "invalid size");

// these declarations must be in sync with those in xcall_once.h

#if _STL_WIN32_WINNT >= _WIN32_WINNT_VISTA

_STD_BEGIN
int __CLRCALL_PURE_OR_CDECL __std_execute_once_begin(
    once_flag& _Once_flag, int& _Pending, bool& _Fallback) noexcept { // wrap Win32 InitOnceBeginInitialize()
    (void) _Fallback;
    return ::InitOnceBeginInitialize(reinterpret_cast<PINIT_ONCE>(&_Once_flag._Opaque), 0, &_Pending, nullptr);
}

int __CLRCALL_PURE_OR_CDECL __std_execute_once_complete(
    once_flag& _Once_flag, const unsigned long _Completion_flags) noexcept { // wrap Win32 InitOnceComplete()
    return ::InitOnceComplete(reinterpret_cast<PINIT_ONCE>(&_Once_flag._Opaque), _Completion_flags, nullptr);
}
_STD_END

#else // ^^^ _STL_WIN32_WINNT >= _WIN32_WINNT_VISTA / _STL_WIN32_WINNT < _WIN32_WINNT_VISTA vvv

namespace {
    struct _Init_once_vista_functions_t {
        std::atomic<decltype(&::InitOnceBeginInitialize)> _Pfn_InitOnceBeginInitialize{};
        std::atomic<decltype(&::InitOnceComplete)> _Pfn_InitOnceComplete{};
        std::atomic<bool> _Initialized{};
    };

    const _Init_once_vista_functions_t& _Get_init_once_vista_functions() noexcept {
        static _Init_once_vista_functions_t functions;
        if (!functions._Initialized.load(std::memory_order_acquire)) {
            HMODULE kernel_module              = ::GetModuleHandleW(L"Kernel32.dll");
            FARPROC init_once_begin_initialize = ::GetProcAddress(kernel_module, "InitOnceBeginInitialize");
            FARPROC init_once_complete         = ::GetProcAddress(kernel_module, "InitOnceComplete");
            if (init_once_begin_initialize != nullptr && init_once_complete != nullptr) {
                functions._Pfn_InitOnceBeginInitialize.store(
                    reinterpret_cast<decltype(&::InitOnceBeginInitialize)>(init_once_begin_initialize),
                    std::memory_order_relaxed);
                functions._Pfn_InitOnceComplete.store(
                    reinterpret_cast<decltype(&::InitOnceComplete)>(init_once_complete), std::memory_order_relaxed);
            }
            functions._Initialized.store(true, std::memory_order_release);
        }
        return functions;
    }
} // unnamed namespace

_STD_BEGIN
int __CLRCALL_PURE_OR_CDECL _Execute_once_begin(
    once_flag& _Once_flag, int& _Pending, bool& _Fallback) noexcept { // wrap Win32 InitOnceBeginInitialize()
    const auto init_once_begin_initialize =
        _Get_init_once_vista_functions()._Pfn_InitOnceBeginInitialize.load(std::memory_order_relaxed);
    if (init_once_begin_initialize == nullptr) {
        _Fallback = true;
        return false;
    }
    return init_once_begin_initialize(reinterpret_cast<PINIT_ONCE>(&_Once_flag._Opaque), 0, &_Pending, nullptr);
}

int __CLRCALL_PURE_OR_CDECL _Execute_once_complete(
    once_flag& _Once_flag, const bool _Succeeded) noexcept { // wrap Win32 InitOnceComplete()
    const auto init_once_complete =
        _Get_init_once_vista_functions()._Pfn_InitOnceComplete.load(std::memory_order_relaxed);
    return init_once_complete(
        reinterpret_cast<PINIT_ONCE>(&_Once_flag._Opaque), _Succeeded ? 0 : INIT_ONCE_INIT_FAILED, nullptr);
}
_STD_END

#endif //  _STL_WIN32_WINNT >= _WIN32_WINNT_VISTA
