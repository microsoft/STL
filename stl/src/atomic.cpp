// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// implement shared_ptr spin lock

#include <yvals.h>

#include <atomic>
#include <intrin.h>
#include <libloaderapi.h>
#include <synchapi.h>

#pragma warning(disable : 4793) // '%s' : function is compiled as native code

namespace {
    // MUTEX FOR shared_ptr ATOMIC OPERATIONS
    SRWLOCK _Shared_ptr_lock = SRWLOCK_INIT;

#if _STL_WIN32_WINNT < _STL_WIN32_WINNT_VISTA
    // SPIN LOCK FOR shared_ptr ATOMIC OPERATIONS
    long _Shared_ptr_flag;


    struct _Srw_functions_table {
        _STD atomic<decltype(&::AcquireSRWLockExclusive)> _Pfn_AcquireSRWLockExclusive{nullptr};
        _STD atomic<decltype(&::ReleaseSRWLockExclusive)> _Pfn_ReleaseSRWLockExclusive{nullptr};
    };

    _Srw_functions_table _Table;

    enum class _Shared_ptr_api_level {
        __has_nothing ,
        __has_srwlock,
        __not_set,
    };

    _Shared_ptr_api_level _Acuire_srw_functions() {
        static _STD atomic<_Shared_ptr_api_level> _Api_level{_Shared_ptr_api_level::__not_set};

        _Shared_ptr_api_level _Current = _Api_level.load(_STD memory_order_acquire);
        if (_Current == _Shared_ptr_api_level::__not_set) {
            _Current          = _Shared_ptr_api_level::__has_nothing;
            HMODULE _Kernel32 = GetModuleHandleW(L"Kernel32.dll");
            if (_Kernel32 != nullptr) {
                auto _Pfn_AcquireSRWLockExclusive = reinterpret_cast<decltype(&::AcquireSRWLockExclusive)>(
                    GetProcAddress(_Kernel32, "AcquireSRWLockExclusive"));
                auto _Pfn_ReleaseSRWLockExclusive = reinterpret_cast<decltype(&::ReleaseSRWLockExclusive)>(
                    GetProcAddress(_Kernel32, "ReleaseSRWLockExclusive"));
                if (_Pfn_AcquireSRWLockExclusive != nullptr && _Pfn_ReleaseSRWLockExclusive != nullptr) {
                    _Table._Pfn_AcquireSRWLockExclusive.store(_Pfn_AcquireSRWLockExclusive, _STD memory_order_relaxed);
                    _Table._Pfn_ReleaseSRWLockExclusive.store(_Pfn_ReleaseSRWLockExclusive, _STD memory_order_relaxed);
                    _Current = _Shared_ptr_api_level::__has_srwlock;
                }
            }
            _Api_level.store(_Current, _STD memory_order_release);
        }
        return _Current;
    }
#endif // ^^^ _STL_WIN32_WINNT < _STL_WIN32_WINNT_VISTA ^^^
} // unnamed namespace

_EXTERN_C

_CRTIMP2_PURE void __cdecl _Lock_shared_ptr_spin_lock() { // spin until _Shared_ptr_flag successfully set
#if _STL_WIN32_WINNT >= _STL_WIN32_WINNT_VISTA
    AcquireSRWLockExclusive(_Shared_ptr_lock);
#else // ^^^ _STL_WIN32_WINNT >= _STL_WIN32_WINNT_VISTA / _STL_WIN32_WINNT < _STL_WIN32_WINNT_VISTA vvv
    if (_Acuire_srw_functions() == _Shared_ptr_api_level::__has_nothing) {
        while (_interlockedbittestandset(&_Shared_ptr_flag, 0) != 0) { // set bit 0
            while (__iso_volatile_load32(reinterpret_cast<int*>(&_Shared_ptr_flag)) != 0) {
                YieldProcessor();
            }
        }
    } else {
        _Table._Pfn_AcquireSRWLockExclusive.load(_STD memory_order_relaxed)(&_Shared_ptr_lock);
    }
#endif // ^^^ _STL_WIN32_WINNT < _STL_WIN32_WINNT_VISTA ^^^
}

_CRTIMP2_PURE void __cdecl _Unlock_shared_ptr_spin_lock() { // release previously obtained lock
#if _STL_WIN32_WINNT >= _STL_WIN32_WINNT_VISTA
    ReleaseSRWLockExclusive(&_Shared_ptr_lock);
#else // ^^^ _STL_WIN32_WINNT >= _STL_WIN32_WINNT_VISTA / _STL_WIN32_WINNT < _STL_WIN32_WINNT_VISTA vvv
    if (_Acuire_srw_functions() == _Shared_ptr_api_level::__has_nothing) {
        _interlockedbittestandreset(&_Shared_ptr_flag, 0); // reset bit 0
    } else {
        _Table._Pfn_ReleaseSRWLockExclusive.load(_STD memory_order_relaxed)(&_Shared_ptr_lock);
    }
#endif // ^^^ _STL_WIN32_WINNT < _STL_WIN32_WINNT_VISTA ^^^
}

_END_EXTERN_C
