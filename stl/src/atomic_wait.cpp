// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// implement atomic wait / notify_one / notify_all

#include <atomic>
#include <cstdint>
#include <cstdlib>
#include <new>
#include <thread>

#include <Windows.h>

#pragma comment(lib, "synchronization")

namespace {
    constexpr unsigned long long _Atomic_wait_no_deadline = 0xFFFF'FFFF'FFFF'FFFF;

    constexpr size_t _Wait_table_size_power = 8;
    constexpr size_t _Wait_table_size       = 1 << _Wait_table_size_power;
    constexpr size_t _Wait_table_index_mask = _Wait_table_size - 1;

    struct _Wait_context {
        const void* _Storage; // Pointer to wait on
        _Wait_context* _Next;
        _Wait_context* _Prev;
        CONDITION_VARIABLE _Condition;
    };

    struct [[nodiscard]] _Guarded_wait_context : _Wait_context {
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

    class [[nodiscard]] _SrwLock_guard {
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
#endif // defined(_DEBUG)
    }
} // unnamed namespace

extern "C" {
int __stdcall __std_atomic_wait_direct(const void* const _Storage, void* const _Comparand, const size_t _Size,
    const unsigned long _Remaining_timeout) noexcept {
    const auto _Result =
        WaitOnAddress(const_cast<volatile void*>(_Storage), const_cast<void*>(_Comparand), _Size, _Remaining_timeout);

    if (!_Result) {
        _Assume_timeout();
    }
    return _Result;
}

void __stdcall __std_atomic_notify_one_direct(const void* const _Storage) noexcept {
    WakeByAddressSingle(const_cast<void*>(_Storage));
}

void __stdcall __std_atomic_notify_all_direct(const void* const _Storage) noexcept {
    WakeByAddressAll(const_cast<void*>(_Storage));
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

        if (_Remaining_timeout != __std_atomic_wait_no_timeout) {
            // spurious wake to recheck the clock
            return TRUE;
        }
    }
}

// TRANSITION, ABI: preserved for binary compatibility
unsigned long long __stdcall __std_atomic_wait_get_deadline(const unsigned long long _Timeout) noexcept {
    if (_Timeout == _Atomic_wait_no_deadline) {
        return _Atomic_wait_no_deadline;
    } else {
        return GetTickCount64() + _Timeout;
    }
}

// TRANSITION, ABI: preserved for binary compatibility
unsigned long __stdcall __std_atomic_wait_get_remaining_timeout(unsigned long long _Deadline) noexcept {
    static_assert(__std_atomic_wait_no_timeout == INFINITE,
        "__std_atomic_wait_no_timeout is passed directly to underlying API, so should match it");

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

// TRANSITION, ABI: preserved for binary compatibility
enum class __std_atomic_api_level : unsigned long { __not_set, __detecting, __has_srwlock, __has_wait_on_address };
__std_atomic_api_level __stdcall __std_atomic_set_api_level(__std_atomic_api_level) noexcept {
    return __std_atomic_api_level::__has_wait_on_address;
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

// TRANSITION, ABI: preserved for binary compatibility
[[nodiscard]] unsigned char __stdcall __std_atomic_compare_exchange_128(_Inout_bytecount_(16) long long* _Destination,
    _In_ long long _ExchangeHigh, _In_ long long _ExchangeLow,
    _Inout_bytecount_(16) long long* _ComparandResult) noexcept {
#ifdef _WIN64
    return _InterlockedCompareExchange128(_Destination, _ExchangeHigh, _ExchangeLow, _ComparandResult);
#else // ^^^ 64-bit / 32-bit vvv
    (void) _Destination;
    (void) _ExchangeHigh;
    (void) _ExchangeLow;
    (void) _ComparandResult;
    _CSTD abort();
#endif // ^^^ 32-bit ^^^
}

// TRANSITION, ABI: preserved for binary compatibility
[[nodiscard]] char __stdcall __std_atomic_has_cmpxchg16b() noexcept {
#ifdef _WIN64
    return true;
#else
    _CSTD abort();
#endif
}
} // extern "C"
