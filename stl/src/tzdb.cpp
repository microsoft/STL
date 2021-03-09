// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <atomic>
#include <memory>
#include <xfilesystem_abi.h>
#include <xtzdb.h>

#define NOMINMAX
#include <icu.h>
#include <internal_shared.h>

#include <Windows.h>

#pragma comment(lib, "Advapi32")

namespace {
    enum class _Icu_api_level : unsigned long {
        __not_set,
        __detecting,
        __has_failed,
        __has_icu_addresses,
    };

    struct _Icu_functions_table {
        _STD atomic<decltype(&::ucal_getDefaultTimeZone)> _Pfn_ucal_getDefaultTimeZone{nullptr};
        _STD atomic<decltype(&::ucal_openTimeZoneIDEnumeration)> _Pfn_ucal_openTimeZoneIDEnumeration{nullptr};
        _STD atomic<decltype(&::uenum_close)> _Pfn_uenum_close{nullptr};
        _STD atomic<decltype(&::uenum_count)> _Pfn_uenum_count{nullptr};
        _STD atomic<decltype(&::uenum_unext)> _Pfn_uenum_unext{nullptr};
        _STD atomic<_Icu_api_level> _Api_level{_Icu_api_level::__not_set};
    };

    _Icu_functions_table _Icu_functions;

    template <typename T>
    void _Load_address(const HMODULE _Module, _STD atomic<T>& _Stored_Pfn, LPCSTR _Fn_name, DWORD& _Last_error) {
        const auto _Pfn = reinterpret_cast<T>(GetProcAddress(_Module, _Fn_name));
        if (_Pfn != nullptr) {
            _Stored_Pfn.store(_Pfn, _STD memory_order_relaxed);
        } else {
            _Last_error = GetLastError();
        }
    }

    // FIXME: Inspired from what I found in atomic.cpp. Is this overkill, I wasn't sure what to do
    //        with race conditions and static state. I didn't want to LoadLibraryExW twice.
    _NODISCARD _Icu_api_level _Init_icu_functions(_Icu_api_level _Level) noexcept {
        while (!_Icu_functions._Api_level.compare_exchange_weak(_Level, _Icu_api_level::__detecting)) {
            if (_Level > _Icu_api_level::__detecting) {
                return _Level;
            }
        }

        _Level = _Icu_api_level::__has_failed;

        const HMODULE _Icu_module = LoadLibraryExW(L"icu.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (_Icu_module != nullptr) {
            // collect at least one error if any GetProcAddress call fails
            DWORD _Last_error{0};
            _Load_address(
                _Icu_module, _Icu_functions._Pfn_ucal_getDefaultTimeZone, "ucal_getDefaultTimeZone", _Last_error);
            _Load_address(_Icu_module, _Icu_functions._Pfn_ucal_openTimeZoneIDEnumeration,
                "ucal_openTimeZoneIDEnumeration", _Last_error);
            _Load_address(_Icu_module, _Icu_functions._Pfn_uenum_close, "uenum_close", _Last_error);
            _Load_address(_Icu_module, _Icu_functions._Pfn_uenum_count, "uenum_count", _Last_error);
            _Load_address(_Icu_module, _Icu_functions._Pfn_uenum_unext, "uenum_unext", _Last_error);
            if (_Last_error == ERROR_SUCCESS) {
                _Level = _Icu_api_level::__has_icu_addresses;
            } else {
                // reset last-error in-case a later GetProcAddress resets it
                SetLastError(_Last_error);
            }
        }

        _Icu_functions._Api_level.store(_Level, _STD memory_order_release);
        return _Level;
    }

    _NODISCARD _Icu_api_level _Acquire_icu_functions() noexcept {
        auto _Level = _Icu_functions._Api_level.load(_STD memory_order_acquire);
        if (_Level <= _Icu_api_level::__detecting) {
            _Level = _Init_icu_functions(_Level);
        }

        return _Level;
    }

    _NODISCARD int32_t __icu_ucal_getDefaultTimeZone(UChar* result, int32_t resultCapacity, UErrorCode* ec) noexcept {
        const auto _Fun = _Icu_functions._Pfn_ucal_getDefaultTimeZone.load(_STD memory_order_relaxed);
        return _Fun(result, resultCapacity, ec);
    }

    _NODISCARD UEnumeration* __icu_ucal_openTimeZoneIDEnumeration(
        USystemTimeZoneType zoneType, const char* region, const int32_t* rawOffset, UErrorCode* ec) {
        const auto _Fun = _Icu_functions._Pfn_ucal_openTimeZoneIDEnumeration.load(_STD memory_order_relaxed);
        return _Fun(zoneType, region, rawOffset, ec);
    }

    _NODISCARD void __icu_uenum_close(UEnumeration* en) {
        const auto _Fun = _Icu_functions._Pfn_uenum_close.load(_STD memory_order_relaxed);
        return _Fun(en);
    }

    _NODISCARD int32_t __icu_uenum_count(UEnumeration* en, UErrorCode* ec) {
        const auto _Fun = _Icu_functions._Pfn_uenum_count.load(_STD memory_order_relaxed);
        return _Fun(en, ec);
    }

    _NODISCARD const UChar* __icu_uenum_unext(UEnumeration* en, int32_t* resultLength, UErrorCode* status) {
        const auto _Fun = _Icu_functions._Pfn_uenum_unext.load(_STD memory_order_relaxed);
        return _Fun(en, resultLength, status);
    }

    struct _Tz_link {
        const char* _Target;
        const char* _Name;
    };

    // FIXME: Likely not the final implementation just here to open a design discussion on
    //        how to handle time_zone_link. See test.cpp for further details on the issue.
    static const _Tz_link _Known_links[] = {
        // clang-format off
        // Target                   // Name
        {"Pacific/Auckland",        "Antarctica/McMurdo"},
        {"Africa/Maputo",           "Africa/Lusaka"}
        // clang-format on
    };

    _NODISCARD const char* _Allocate_wide_to_narrow(
        const char16_t* _Input, int _Input_len, __std_tzdb_error& _Err) noexcept {
        const auto _Code_page      = __std_fs_code_page();
        const auto _Input_as_wchar = reinterpret_cast<const wchar_t*>(_Input);
        // FIXME: Is is ok to pull in xfilesystem_abi.h and use these here?
        const auto _Count_result = __std_fs_convert_wide_to_narrow(_Code_page, _Input_as_wchar, _Input_len, nullptr, 0);
        if (_Count_result._Err != __std_win_error::_Success) {
            _Err = __std_tzdb_error::_Win_error;
            return nullptr;
        }

        auto* _Data = new (_STD nothrow) char[_Count_result._Len + 1];
        if (_Data == nullptr) {
            return nullptr;
        }

        _Data[_Count_result._Len] = '\0';

        const auto _Result =
            __std_fs_convert_wide_to_narrow(_Code_page, _Input_as_wchar, _Input_len, _Data, _Count_result._Len);
        if (_Result._Err != __std_win_error::_Success) {
            _Err = __std_tzdb_error::_Win_error;
            delete[] _Data;
            return nullptr;
        }

        return _Data;
    }

} // namespace

_EXTERN_C

_NODISCARD __std_tzdb_time_zones_info* __stdcall __std_tzdb_get_time_zones() noexcept {
    // On exit---
    //    *_Info == nullptr         --> bad_alloc()
    //    _Info->_Err == _Win_error --> failed, call GetLastError()
    //    _Info->_Err == _Icu_error --> runtime_error interacting with ICU
    _STD unique_ptr<__std_tzdb_time_zones_info, decltype(&__std_tzdb_delete_time_zones)> _Info{
        new (_STD nothrow) __std_tzdb_time_zones_info{}, &__std_tzdb_delete_time_zones};
    if (_Info == nullptr) {
        return nullptr;
    }

    if (_Acquire_icu_functions() < _Icu_api_level::__has_icu_addresses) {
        _Info->_Err = __std_tzdb_error::_Win_error;
        return _Info.release();
    }

    UErrorCode _Err{};
    _STD unique_ptr<UEnumeration, decltype(&__icu_uenum_close)> _Enum{
        __icu_ucal_openTimeZoneIDEnumeration(USystemTimeZoneType::UCAL_ZONE_TYPE_CANONICAL, nullptr, nullptr, &_Err),
        &__icu_uenum_close};
    if (U_FAILURE(_Err)) {
        _Info->_Err = __std_tzdb_error::_Icu_error;
        return _Info.release();
    }

    // uenum_count may be expensive but is required to pre allocated arrays.
    int32_t _Num_time_zones = __icu_uenum_count(_Enum.get(), &_Err);
    if (U_FAILURE(_Err)) {
        _Info->_Err = __std_tzdb_error::_Icu_error;
        return _Info.release();
    }

    _Info->_Num_time_zones = static_cast<size_t>(_Num_time_zones);
    _Info->_Names          = new (_STD nothrow) const char*[_Info->_Num_time_zones];
    if (_Info->_Names == nullptr) {
        return nullptr;
    }

    // init to ensure __std_tzdb_delete_init_info cleanup is valid
    _STD fill_n(_Info->_Names, (ptrdiff_t) _Info->_Num_time_zones, nullptr);

    _Info->_Links = new (_STD nothrow) const char*[_Info->_Num_time_zones];
    if (_Info->_Links == nullptr) {
        return nullptr;
    }

    // init to ensure __std_tzdb_delete_init_info cleanup is valid
    _STD fill_n(_Info->_Links, _Info->_Num_time_zones, nullptr);

    for (size_t _Name_idx = 0; _Name_idx < _Info->_Num_time_zones; ++_Name_idx) {
        int32_t _Elem_len{};
        const auto* _Elem = __icu_uenum_unext(_Enum.get(), &_Elem_len, &_Err);
        if (U_FAILURE(_Err) || _Elem == nullptr) {
            _Info->_Err = __std_tzdb_error::_Icu_error;
            return _Info.release();
        }

        _Info->_Names[_Name_idx] = _Allocate_wide_to_narrow(_Elem, _Elem_len, _Info->_Err);
        if (_Info->_Names[_Name_idx] == nullptr) {
            return _Info->_Err != __std_tzdb_error::_Success ? _Info.release() : nullptr;
        }

        // ensure time_zone is not a known time_zone_link
        for (const auto& _Link : _Known_links) {
            if (strcmp(_Info->_Names[_Name_idx], _Link._Name) == 0) {
                _Info->_Links[_Name_idx] = _Link._Target; // no need to allocate a string
            }
        }
    }

    return _Info.release();
}

void __stdcall __std_tzdb_delete_time_zones(__std_tzdb_time_zones_info* _Info) noexcept {
    if (_Info != nullptr) {
        if (_Info->_Names != nullptr) {
            for (size_t _Idx = 0; _Idx < _Info->_Num_time_zones; _Idx++) {
                if (_Info->_Names[_Idx] != nullptr) {
                    delete[] _Info->_Names[_Idx];
                }
            }

            delete[] _Info->_Names;
            _Info->_Names = nullptr;
        }

        if (_Info->_Links != nullptr) {
            delete[] _Info->_Links;
            _Info->_Links = nullptr;
        }
    }
}

_NODISCARD __std_tzdb_current_zone_info* __stdcall __std_tzdb_get_current_zone() noexcept {
    // On exit---
    //    *_Info == nullptr         --> bad_alloc()
    //    _Info->_Err == _Win_error --> failed, call GetLastError()
    //    _Info->_Err == _Icu_error --> runtime_error interacting with ICU
    _STD unique_ptr<__std_tzdb_current_zone_info, decltype(&__std_tzdb_delete_current_zone)> _Info{
        new (_STD nothrow) __std_tzdb_current_zone_info{}, &__std_tzdb_delete_current_zone};
    if (_Info == nullptr) {
        return nullptr;
    }

    if (_Acquire_icu_functions() < _Icu_api_level::__has_icu_addresses) {
        _Info->_Err = __std_tzdb_error::_Win_error;
        return _Info.release();
    }

    UErrorCode _Err{};
    char16_t _Id_buf[256];
    const auto _Id_buf_len = __icu_ucal_getDefaultTimeZone(_Id_buf, sizeof(_Id_buf), &_Err);
    if (U_FAILURE(_Err) || _Id_buf_len == 0) {
        _Info->_Err = __std_tzdb_error::_Icu_error;
        return _Info.release();
    }

    _Info->_Tz_name = _Allocate_wide_to_narrow(_Id_buf, static_cast<size_t>(_Id_buf_len), _Info->_Err);
    if (_Info->_Tz_name == nullptr) {
        return _Info->_Err != __std_tzdb_error::_Success ? _Info.release() : nullptr;
    }

    return _Info.release();
}

void __stdcall __std_tzdb_delete_current_zone(__std_tzdb_current_zone_info* _Info) noexcept {
    if (_Info) {
        delete[] _Info->_Tz_name;
        _Info->_Tz_name = nullptr;
    }
}

__std_tzdb_registry_leap_info* __stdcall __std_tzdb_get_reg_leap_seconds(
    const size_t prev_reg_ls_size, size_t* const current_reg_ls_size) noexcept {
    // On exit---
    //    *current_reg_ls_size <= prev_reg_ls_size, reg_ls_data == nullptr --> no new data
    //    *current_reg_ls_size >  prev_reg_ls_size, reg_ls_data != nullptr --> new data, successfully read
    //    *current_reg_ls_size == 0,                reg_ls_data != nullptr --> new data, failed reading
    //    *current_reg_ls_size >  prev_reg_ls_size, reg_ls_data == nullptr --> new data, failed allocation

    constexpr auto reg_key_name    = LR"(SYSTEM\CurrentControlSet\Control\LeapSecondInformation)";
    constexpr auto reg_subkey_name = L"LeapSeconds";
    *current_reg_ls_size           = 0;
    HKEY leap_sec_key              = nullptr;

    LSTATUS status = RegOpenKeyExW(HKEY_LOCAL_MACHINE, reg_key_name, 0, KEY_READ, &leap_sec_key);
    if (status != ERROR_SUCCESS) {
        // May not exist on older systems. Treat this as equivalent to the key existing but with no data.
        return nullptr;
    }

    DWORD byte_size = 0;
    status          = RegQueryValueExW(leap_sec_key, reg_subkey_name, nullptr, nullptr, nullptr, &byte_size);
    static_assert(sizeof(__std_tzdb_registry_leap_info) == 12);
    const auto ls_size   = byte_size / 12;
    *current_reg_ls_size = ls_size;

    __std_tzdb_registry_leap_info* reg_ls_data = nullptr;
    if ((status == ERROR_SUCCESS || status == ERROR_MORE_DATA) && ls_size > prev_reg_ls_size) {
        try {
            reg_ls_data = new __std_tzdb_registry_leap_info[ls_size];
            status      = RegQueryValueExW(
                leap_sec_key, reg_subkey_name, nullptr, nullptr, reinterpret_cast<LPBYTE>(reg_ls_data), &byte_size);
            if (status != ERROR_SUCCESS) {
                *current_reg_ls_size = 0;
            }
        } catch (...) {
        }
    }

    RegCloseKey(leap_sec_key);
    if (status != ERROR_SUCCESS) {
        SetLastError(status);
    }

    return reg_ls_data;
}

void __stdcall __std_tzdb_delete_reg_leap_seconds(__std_tzdb_registry_leap_info* _Rlsi) noexcept {
    delete[] _Rlsi;
}

_NODISCARD void* __stdcall __std_calloc_crt(const size_t count, const size_t size) noexcept {
    return _calloc_crt(count, size);
}

void __stdcall __std_free_crt(void* p) noexcept {
    _free_crt(p);
}

_END_EXTERN_C
