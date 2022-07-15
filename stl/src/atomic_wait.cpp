// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// implement atomic wait / notify_one / notify_all

#include <atomic>
#include <cstdint>
#include <new>
#include <thread>

#include <Windows.h>

namespace {

    constexpr size_t _Wait_table_size_power = 8;
    constexpr size_t _Wait_table_size       = 1 << _Wait_table_size_power;
    constexpr size_t _Wait_table_index_mask = _Wait_table_size - 1;

    struct _Wait_context {
        const void* _Storage; // Pointer to wait on
        _Wait_context* _Next;
        _Wait_context* _Prev;
        CONDITION_VARIABLE _Condition;
    };

    struct _NODISCARD _Guarded_wait_context : _Wait_context {
        _Guarded_wait_context(const void* _Storage_, _Wait_context* const _Head) noexcept
            : _Wait_context{_Storage_, _Head, _Head->_Prev, CONDITION_VARIABLE_INIT} {
            _Prev->_Next = this;
            _Next->_Prev = this;
        }

        ~_Guarded_wait_context() {
            const auto _Next_local = _Next;
            const auto _Prev_local = _Prev;
            _Next->_Prev           = _Prev_local;
            _Prev->_Next           = _Next_local;
        }

        _Guarded_wait_context(const _Guarded_wait_context&)            = delete;
        _Guarded_wait_context& operator=(const _Guarded_wait_context&) = delete;
    };

    class _NODISCARD _SrwLock_guard {
    public:
        explicit _SrwLock_guard(SRWLOCK& _Locked_) noexcept : _Locked(&_Locked_) {
            AcquireSRWLockExclusive(_Locked);
        }

        ~_SrwLock_guard() {
            ReleaseSRWLockExclusive(_Locked);
        }

        _SrwLock_guard(const _SrwLock_guard&)            = delete;
        _SrwLock_guard& operator=(const _SrwLock_guard&) = delete;

    private:
        SRWLOCK* _Locked;
    };


#pragma warning(push)
#pragma warning(disable : 4324) // structure was padded due to alignment specifier
    struct alignas(_STD hardware_destructive_interference_size) _Wait_table_entry {
        SRWLOCK _Lock = SRWLOCK_INIT;
        // Initialize to all zeros, self-link lazily to optimize for space.
        // Since _Wait_table_entry is initialized to all zero bytes,
        // _Atomic_wait_table_entry::wait_table will also be all zero bytes.
        // It can thus can be stored in the .bss section, and not in the actual binary.
        _Wait_context _Wait_list_head = {nullptr, nullptr, nullptr, CONDITION_VARIABLE_INIT};

        constexpr _Wait_table_entry() noexcept = default;
    };
#pragma warning(pop)

    [[nodiscard]] _Wait_table_entry& _Atomic_wait_table_entry(const void* const _Storage) noexcept {
        static _Wait_table_entry wait_table[_Wait_table_size];
        auto index = reinterpret_cast<_STD uintptr_t>(_Storage);
        index ^= index >> (_Wait_table_size_power * 2);
        index ^= index >> _Wait_table_size_power;
        return wait_table[index & _Wait_table_index_mask];
    }

    void _Assume_timeout() noexcept {
#ifdef _DEBUG
        if (GetLastError() != ERROR_TIMEOUT) {
            _CSTD abort();
        }
#endif // _DEBUG
    }

#ifndef _ATOMIC_WAIT_ON_ADDRESS_STATICALLY_AVAILABLE
#if _STL_WIN32_WINNT >= _STL_WIN32_WINNT_WIN8
#define _ATOMIC_WAIT_ON_ADDRESS_STATICALLY_AVAILABLE 1
#else // ^^^ _STL_WIN32_WINNT >= _STL_WIN32_WINNT_WIN8 // _STL_WIN32_WINNT < _STL_WIN32_WINNT_WIN8 vvv
#define _ATOMIC_WAIT_ON_ADDRESS_STATICALLY_AVAILABLE 0
#endif // ^^^ _STL_WIN32_WINNT < _STL_WIN32_WINNT_WIN8 ^^^
#endif // _ATOMIC_WAIT_ON_ADDRESS_STATICALLY_AVAILABLE

#if _ATOMIC_WAIT_ON_ADDRESS_STATICALLY_AVAILABLE

#pragma comment(lib, "synchronization")

#define __crtWaitOnAddress       WaitOnAddress
#define __crtWakeByAddressSingle WakeByAddressSingle
#define __crtWakeByAddressAll    WakeByAddressAll

#else // ^^^ _ATOMIC_WAIT_ON_ADDRESS_STATICALLY_AVAILABLE / !_ATOMIC_WAIT_ON_ADDRESS_STATICALLY_AVAILABLE vvv


    struct _Wait_functions_table {
        _STD atomic<decltype(&::WaitOnAddress)> _Pfn_WaitOnAddress{nullptr};
        _STD atomic<decltype(&::WakeByAddressSingle)> _Pfn_WakeByAddressSingle{nullptr};
        _STD atomic<decltype(&::WakeByAddressAll)> _Pfn_WakeByAddressAll{nullptr};
        _STD atomic<__std_atomic_api_level> _Api_level{__std_atomic_api_level::__not_set};
    };

    _Wait_functions_table _Wait_functions;

    void _Force_wait_functions_srwlock_only() noexcept {
        auto _Local = _Wait_functions._Api_level.load(_STD memory_order_acquire);
        if (_Local <= __std_atomic_api_level::__detecting) {
            while (!_Wait_functions._Api_level.compare_exchange_weak(_Local, __std_atomic_api_level::__has_srwlock)) {
                if (_Local > __std_atomic_api_level::__detecting) {
                    return;
                }
            }
        }
    }

    [[nodiscard]] __std_atomic_api_level _Init_wait_functions(__std_atomic_api_level _Level) {
        while (!_Wait_functions._Api_level.compare_exchange_weak(_Level, __std_atomic_api_level::__detecting)) {
            if (_Level > __std_atomic_api_level::__detecting) {
                return _Level;
            }
        }

        _Level = __std_atomic_api_level::__has_srwlock;

        const HMODULE _Sync_module = GetModuleHandleW(L"api-ms-win-core-synch-l1-2-0.dll");
        if (_Sync_module != nullptr) {
            const auto _Wait_on_address =
                reinterpret_cast<decltype(&::WaitOnAddress)>(GetProcAddress(_Sync_module, "WaitOnAddress"));
            const auto _Wake_by_address_single =
                reinterpret_cast<decltype(&::WakeByAddressSingle)>(GetProcAddress(_Sync_module, "WakeByAddressSingle"));
            const auto _Wake_by_address_all =
                reinterpret_cast<decltype(&::WakeByAddressAll)>(GetProcAddress(_Sync_module, "WakeByAddressAll"));

            if (_Wait_on_address != nullptr && _Wake_by_address_single != nullptr && _Wake_by_address_all != nullptr) {
                _Wait_functions._Pfn_WaitOnAddress.store(_Wait_on_address, _STD memory_order_relaxed);
                _Wait_functions._Pfn_WakeByAddressSingle.store(_Wake_by_address_single, _STD memory_order_relaxed);
                _Wait_functions._Pfn_WakeByAddressAll.store(_Wake_by_address_all, _STD memory_order_relaxed);
                _Level = __std_atomic_api_level::__has_wait_on_address;
            }
        }

        // for __has_srwlock, relaxed would have been enough, not distinguishing for consistency
        _Wait_functions._Api_level.store(_Level, _STD memory_order_release);
        return _Level;
    }

    [[nodiscard]] __std_atomic_api_level _Acquire_wait_functions() noexcept {
        auto _Level = _Wait_functions._Api_level.load(_STD memory_order_acquire);
        if (_Level <= __std_atomic_api_level::__detecting) {
            _Level = _Init_wait_functions(_Level);
        }

        return _Level;
    }

    [[nodiscard]] BOOL __crtWaitOnAddress(
        volatile VOID* Address, PVOID CompareAddress, SIZE_T AddressSize, DWORD dwMilliseconds) {
        const auto _Wait_on_address = _Wait_functions._Pfn_WaitOnAddress.load(_STD memory_order_relaxed);
        return _Wait_on_address(Address, CompareAddress, AddressSize, dwMilliseconds);
    }

    VOID __crtWakeByAddressSingle(PVOID Address) {
        const auto _Wake_by_address_single = _Wait_functions._Pfn_WakeByAddressSingle.load(_STD memory_order_relaxed);
        _Wake_by_address_single(Address);
    }

    VOID __crtWakeByAddressAll(PVOID Address) {
        const auto _Wake_by_address_all = _Wait_functions._Pfn_WakeByAddressAll.load(_STD memory_order_relaxed);
        _Wake_by_address_all(Address);
    }

    bool __stdcall _Atomic_wait_are_equal_direct_fallback(
        const void* _Storage, void* _Comparand, size_t _Size, void*) noexcept {
        switch (_Size) {
        case 1:
            return __iso_volatile_load8(static_cast<const char*>(_Storage)) == *static_cast<const char*>(_Comparand);
        case 2:
            return __iso_volatile_load16(static_cast<const short*>(_Storage)) == *static_cast<const short*>(_Comparand);
        case 4:
            return __iso_volatile_load32(static_cast<const int*>(_Storage)) == *static_cast<const int*>(_Comparand);
        case 8:
            return __iso_volatile_load64(static_cast<const long long*>(_Storage))
                == *static_cast<const long long*>(_Comparand);
        default:
            _CSTD abort();
        }
    }
#endif // _ATOMIC_WAIT_ON_ADDRESS_STATICALLY_AVAILABLE

    _NODISCARD unsigned char __std_atomic_compare_exchange_128_fallback(_Inout_bytecount_(16) long long* _Destination,
        _In_ long long _ExchangeHigh, _In_ long long _ExchangeLow,
        _Inout_bytecount_(16) long long* _ComparandResult) noexcept {
        static SRWLOCK _Mtx = SRWLOCK_INIT;
        _SrwLock_guard _Guard{_Mtx};
        if (_Destination[0] == _ComparandResult[0] && _Destination[1] == _ComparandResult[1]) {
            _ComparandResult[0] = _Destination[0];
            _ComparandResult[1] = _Destination[1];
            _Destination[0]     = _ExchangeLow;
            _Destination[1]     = _ExchangeHigh;
            return static_cast<unsigned char>(true);
        } else {
            _ComparandResult[0] = _Destination[0];
            _ComparandResult[1] = _Destination[1];
            return static_cast<unsigned char>(false);
        }
    }
} // unnamed namespace


_EXTERN_C
int __stdcall __std_atomic_wait_direct(const void* const _Storage, void* const _Comparand, const size_t _Size,
    const unsigned long _Remaining_timeout) noexcept {
#if _ATOMIC_WAIT_ON_ADDRESS_STATICALLY_AVAILABLE == 0
    if (_Acquire_wait_functions() < __std_atomic_api_level::__has_wait_on_address) {
        return __std_atomic_wait_indirect(
            _Storage, _Comparand, _Size, nullptr, &_Atomic_wait_are_equal_direct_fallback, _Remaining_timeout);
    }
#endif // _ATOMIC_WAIT_ON_ADDRESS_STATICALLY_AVAILABLE == 0

    const auto _Result = __crtWaitOnAddress(
        const_cast<volatile void*>(_Storage), const_cast<void*>(_Comparand), _Size, _Remaining_timeout);

    if (!_Result) {
        _Assume_timeout();
    }
    return _Result;
}

void __stdcall __std_atomic_notify_one_direct(const void* const _Storage) noexcept {
#if _ATOMIC_WAIT_ON_ADDRESS_STATICALLY_AVAILABLE == 0
    if (_Acquire_wait_functions() < __std_atomic_api_level::__has_wait_on_address) {
        __std_atomic_notify_one_indirect(_Storage);
        return;
    }
#endif // _ATOMIC_WAIT_ON_ADDRESS_STATICALLY_AVAILABLE = 0

    __crtWakeByAddressSingle(const_cast<void*>(_Storage));
}

void __stdcall __std_atomic_notify_all_direct(const void* const _Storage) noexcept {
#if _ATOMIC_WAIT_ON_ADDRESS_STATICALLY_AVAILABLE == 0
    if (_Acquire_wait_functions() < __std_atomic_api_level::__has_wait_on_address) {
        __std_atomic_notify_all_indirect(_Storage);
        return;
    }
#endif // _ATOMIC_WAIT_ON_ADDRESS_STATICALLY_AVAILABLE == 0

    __crtWakeByAddressAll(const_cast<void*>(_Storage));
}

void __stdcall __std_atomic_notify_one_indirect(const void* const _Storage) noexcept {
    auto& _Entry = _Atomic_wait_table_entry(_Storage);
    _SrwLock_guard _Guard(_Entry._Lock);
    _Wait_context* _Context = _Entry._Wait_list_head._Next;

    if (_Context == nullptr) {
        return;
    }

    for (; _Context != &_Entry._Wait_list_head; _Context = _Context->_Next) {
        if (_Context->_Storage == _Storage) {
            // Can't move wake outside SRWLOCKed section: SRWLOCK also protects the _Context itself
            WakeAllConditionVariable(&_Context->_Condition);
            break;
        }
    }
}

void __stdcall __std_atomic_notify_all_indirect(const void* const _Storage) noexcept {
    auto& _Entry = _Atomic_wait_table_entry(_Storage);
    _SrwLock_guard _Guard(_Entry._Lock);
    _Wait_context* _Context = _Entry._Wait_list_head._Next;

    if (_Context == nullptr) {
        return;
    }

    for (; _Context != &_Entry._Wait_list_head; _Context = _Context->_Next) {
        if (_Context->_Storage == _Storage) {
            // Can't move wake outside SRWLOCKed section: SRWLOCK also protects the _Context itself
            WakeAllConditionVariable(&_Context->_Condition);
        }
    }
}

int __stdcall __std_atomic_wait_indirect(const void* _Storage, void* _Comparand, size_t _Size, void* _Param,
    _Atomic_wait_indirect_equal_callback_t _Are_equal, unsigned long _Remaining_timeout) noexcept {
    auto& _Entry = _Atomic_wait_table_entry(_Storage);

    _SrwLock_guard _Guard(_Entry._Lock);

    if (_Entry._Wait_list_head._Next == nullptr) {
        _Entry._Wait_list_head._Next = &_Entry._Wait_list_head;
        _Entry._Wait_list_head._Prev = &_Entry._Wait_list_head;
    }

    _Guarded_wait_context _Context{_Storage, &_Entry._Wait_list_head};
    for (;;) {
        if (!_Are_equal(_Storage, _Comparand, _Size, _Param)) { // note: under lock to prevent lost wakes
            return TRUE;
        }

        if (!SleepConditionVariableSRW(&_Context._Condition, &_Entry._Lock, _Remaining_timeout, 0)) {
            _Assume_timeout();
            return FALSE;
        }

        if (_Remaining_timeout != _Atomic_wait_no_timeout) {
            // spurious wake to recheck the clock
            return TRUE;
        }
    }
}

unsigned long long __stdcall __std_atomic_wait_get_deadline(const unsigned long long _Timeout) noexcept {
    if (_Timeout == _Atomic_wait_no_deadline) {
        return _Atomic_wait_no_deadline;
    } else {
        return GetTickCount64() + _Timeout;
    }
}

unsigned long __stdcall __std_atomic_wait_get_remaining_timeout(unsigned long long _Deadline) noexcept {
    static_assert(_Atomic_wait_no_timeout == INFINITE,
        "_Atomic_wait_no_timeout is passed directly to underlying API, so should match it");

    if (_Deadline == _Atomic_wait_no_deadline) {
        return INFINITE;
    }

    const unsigned long long _Current_time = GetTickCount64();
    if (_Current_time >= _Deadline) {
        return 0;
    }

    unsigned long long _Remaining     = _Deadline - _Current_time;
    constexpr unsigned long _Ten_days = 864'000'000;
    if (_Remaining > _Ten_days) {
        return _Ten_days;
    }
    return static_cast<unsigned long>(_Remaining);
}

__std_atomic_api_level __stdcall __std_atomic_set_api_level(__std_atomic_api_level _Requested_api_level) noexcept {
#if _ATOMIC_WAIT_ON_ADDRESS_STATICALLY_AVAILABLE
    (void) _Requested_api_level;
    return __std_atomic_api_level::__has_wait_on_address;
#else // ^^^ _ATOMIC_WAIT_ON_ADDRESS_STATICALLY_AVAILABLE // !_ATOMIC_WAIT_ON_ADDRESS_STATICALLY_AVAILABLE vvv
    switch (_Requested_api_level) {
    case __std_atomic_api_level::__not_set:
    case __std_atomic_api_level::__detecting:
        _CSTD abort();
    case __std_atomic_api_level::__has_srwlock:
        _Force_wait_functions_srwlock_only();
        break;
    case __std_atomic_api_level::__has_wait_on_address:
    default: // future compat: new header using an old DLL will get the highest requested level supported
        break;
    }

    return _Acquire_wait_functions();
#endif // !_ATOMIC_WAIT_ON_ADDRESS_STATICALLY_AVAILABLE
}

#pragma warning(push)
#pragma warning(disable : 4324) // structure was padded due to alignment specifier
_Smtx_t* __stdcall __std_atomic_get_mutex(const void* const _Key) noexcept {
    constexpr size_t _Table_size_power = 8;
    constexpr size_t _Table_size       = 1 << _Table_size_power;
    constexpr size_t _Table_index_mask = _Table_size - 1;

    struct alignas(std::hardware_destructive_interference_size) _Table_entry {
        _Smtx_t _Mutex;
    };

    static _Table_entry _Table[_Table_size]{};

    auto _Index = reinterpret_cast<std::uintptr_t>(_Key);
    _Index ^= _Index >> (_Table_size_power * 2);
    _Index ^= _Index >> _Table_size_power;
    return &_Table[_Index & _Table_index_mask]._Mutex;
}
#pragma warning(pop)

_NODISCARD unsigned char __stdcall __std_atomic_compare_exchange_128(_Inout_bytecount_(16) long long* _Destination,
    _In_ long long _ExchangeHigh, _In_ long long _ExchangeLow,
    _Inout_bytecount_(16) long long* _ComparandResult) noexcept {
#if !defined(_WIN64)
    return __std_atomic_compare_exchange_128_fallback(_Destination, _ExchangeHigh, _ExchangeLow, _ComparandResult);
#elif _STD_ATOMIC_ALWAYS_USE_CMPXCHG16B == 1
    return _InterlockedCompareExchange128(_Destination, _ExchangeHigh, _ExchangeLow, _ComparandResult);
#else // ^^^ _STD_ATOMIC_ALWAYS_USE_CMPXCHG16B == 1 // _STD_ATOMIC_ALWAYS_USE_CMPXCHG16B == 0 vvv
    if (__std_atomic_has_cmpxchg16b()) {
        return _InterlockedCompareExchange128(_Destination, _ExchangeHigh, _ExchangeLow, _ComparandResult);
    }

    return __std_atomic_compare_exchange_128_fallback(_Destination, _ExchangeHigh, _ExchangeLow, _ComparandResult);
#endif // ^^^ _STD_ATOMIC_ALWAYS_USE_CMPXCHG16B == 0
}

_NODISCARD char __stdcall __std_atomic_has_cmpxchg16b() noexcept {
#if !defined(_WIN64)
    return false;
#elif _STD_ATOMIC_ALWAYS_USE_CMPXCHG16B == 1
    return true;
#else // ^^^ _STD_ATOMIC_ALWAYS_USE_CMPXCHG16B == 1 // _STD_ATOMIC_ALWAYS_USE_CMPXCHG16B == 0 vvv
    constexpr char _Cmpxchg_Absent  = 0;
    constexpr char _Cmpxchg_Present = 1;
    constexpr char _Cmpxchg_Unknown = 2;

    static std::atomic<char> _Cached_value{_Cmpxchg_Unknown};

    char _Value = _Cached_value.load(std::memory_order_relaxed);
    if (_Value == _Cmpxchg_Unknown) {
        _Value = IsProcessorFeaturePresent(PF_COMPARE_EXCHANGE128) ? _Cmpxchg_Present : _Cmpxchg_Absent;
        _Cached_value.store(_Value, std::memory_order_relaxed);
    }

    return _Value;
#endif // ^^^ _STD_ATOMIC_ALWAYS_USE_CMPXCHG16B == 0
}
_END_EXTERN_C
