// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <__msvc_tzdb.hpp>
#include <atomic>
#include <functional>
#include <icu.h>
#include <internal_shared.h>
#include <memory>
#include <string_view>
#include <xfilesystem_abi.h>

#include <Windows.h>

#pragma comment(lib, "Advapi32")

namespace {
    enum class _Icu_api_level : unsigned long {
        _Not_set,
        _Detecting,
        _Has_failed,
        _Has_icu_addresses,
    };

    struct _Icu_functions_table {
        _STD atomic<decltype(&::ucal_close)> _Pfn_ucal_close{nullptr};
        _STD atomic<decltype(&::ucal_get)> _Pfn_ucal_get{nullptr};
        _STD atomic<decltype(&::ucal_getCanonicalTimeZoneID)> _Pfn_ucal_getCanonicalTimeZoneID{nullptr};
        _STD atomic<decltype(&::ucal_getDefaultTimeZone)> _Pfn_ucal_getDefaultTimeZone{nullptr};
        _STD atomic<decltype(&::ucal_getTimeZoneDisplayName)> _Pfn_ucal_getTimeZoneDisplayName{nullptr};
        _STD atomic<decltype(&::ucal_getTimeZoneTransitionDate)> _Pfn_ucal_getTimeZoneTransitionDate{nullptr};
        _STD atomic<decltype(&::ucal_getTZDataVersion)> _Pfn_ucal_getTZDataVersion{nullptr};
        _STD atomic<decltype(&::ucal_inDaylightTime)> _Pfn_ucal_inDaylightTime{nullptr};
        _STD atomic<decltype(&::ucal_open)> _Pfn_ucal_open{nullptr};
        _STD atomic<decltype(&::ucal_openTimeZoneIDEnumeration)> _Pfn_ucal_openTimeZoneIDEnumeration{nullptr};
        _STD atomic<decltype(&::ucal_setMillis)> _Pfn_ucal_setMillis{nullptr};
        _STD atomic<decltype(&::uenum_close)> _Pfn_uenum_close{nullptr};
        _STD atomic<decltype(&::uenum_count)> _Pfn_uenum_count{nullptr};
        _STD atomic<decltype(&::uenum_unext)> _Pfn_uenum_unext{nullptr};
        _STD atomic<_Icu_api_level> _Api_level{_Icu_api_level::_Not_set};
    };

    _Icu_functions_table _Icu_functions;

    template <class _Ty>
    void _Load_address(
        const HMODULE _Module, _STD atomic<_Ty>& _Stored_Pfn, LPCSTR _Fn_name, DWORD& _Last_error) noexcept {
        const auto _Pfn = reinterpret_cast<_Ty>(GetProcAddress(_Module, _Fn_name));
        if (_Pfn != nullptr) {
            _Stored_Pfn.store(_Pfn, _STD memory_order_relaxed);
        } else {
            _Last_error = GetLastError();
        }
    }

    [[nodiscard]] _Icu_api_level _Init_icu_functions(_Icu_api_level _Level) noexcept {
        while (!_Icu_functions._Api_level.compare_exchange_weak(
            _Level, _Icu_api_level::_Detecting, _STD memory_order_acq_rel)) {
            if (_Level > _Icu_api_level::_Detecting) {
                return _Level;
            }
        }

        _Level = _Icu_api_level::_Has_failed;

        const HMODULE _Icu_module = LoadLibraryExW(L"icu.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (_Icu_module != nullptr) {
            // collect at least one error if any GetProcAddress call fails
            DWORD _Last_error{ERROR_SUCCESS};
            _Load_address(_Icu_module, _Icu_functions._Pfn_ucal_close, "ucal_close", _Last_error);
            _Load_address(_Icu_module, _Icu_functions._Pfn_ucal_get, "ucal_get", _Last_error);
            _Load_address(_Icu_module, _Icu_functions._Pfn_ucal_getCanonicalTimeZoneID, "ucal_getCanonicalTimeZoneID",
                _Last_error);
            _Load_address(
                _Icu_module, _Icu_functions._Pfn_ucal_getDefaultTimeZone, "ucal_getDefaultTimeZone", _Last_error);
            _Load_address(_Icu_module, _Icu_functions._Pfn_ucal_getTimeZoneDisplayName, "ucal_getTimeZoneDisplayName",
                _Last_error);
            _Load_address(_Icu_module, _Icu_functions._Pfn_ucal_getTimeZoneTransitionDate,
                "ucal_getTimeZoneTransitionDate", _Last_error);
            _Load_address(_Icu_module, _Icu_functions._Pfn_ucal_getTZDataVersion, "ucal_getTZDataVersion", _Last_error);
            _Load_address(_Icu_module, _Icu_functions._Pfn_ucal_inDaylightTime, "ucal_inDaylightTime", _Last_error);
            _Load_address(_Icu_module, _Icu_functions._Pfn_ucal_open, "ucal_open", _Last_error);
            _Load_address(_Icu_module, _Icu_functions._Pfn_ucal_openTimeZoneIDEnumeration,
                "ucal_openTimeZoneIDEnumeration", _Last_error);
            _Load_address(_Icu_module, _Icu_functions._Pfn_ucal_setMillis, "ucal_setMillis", _Last_error);
            _Load_address(_Icu_module, _Icu_functions._Pfn_uenum_close, "uenum_close", _Last_error);
            _Load_address(_Icu_module, _Icu_functions._Pfn_uenum_count, "uenum_count", _Last_error);
            _Load_address(_Icu_module, _Icu_functions._Pfn_uenum_unext, "uenum_unext", _Last_error);
            if (_Last_error == ERROR_SUCCESS) {
                _Level = _Icu_api_level::_Has_icu_addresses;
            } else {
                // reset last error code for thread in case a later GetProcAddress resets it
                SetLastError(_Last_error);
            }
        }

        _Icu_functions._Api_level.store(_Level, _STD memory_order_release);
        return _Level;
    }

    [[nodiscard]] _Icu_api_level _Acquire_icu_functions() noexcept {
        auto _Level = _Icu_functions._Api_level.load(_STD memory_order_acquire);
        if (_Level <= _Icu_api_level::_Detecting) {
            _Level = _Init_icu_functions(_Level);
        }

        return _Level;
    }

    void __icu_ucal_close(UCalendar* cal) noexcept {
        const auto _Fun = _Icu_functions._Pfn_ucal_close.load(_STD memory_order_relaxed);
        _Fun(cal);
    }

    [[nodiscard]] int32_t __icu_ucal_get(const UCalendar* cal, UCalendarDateFields field, UErrorCode* status) noexcept {
        const auto _Fun = _Icu_functions._Pfn_ucal_get.load(_STD memory_order_relaxed);
        return _Fun(cal, field, status);
    }

    [[nodiscard]] int32_t __icu_ucal_getCanonicalTimeZoneID(const UChar* id, int32_t len, UChar* result,
        int32_t resultCapacity, UBool* isSystemID, UErrorCode* status) noexcept {
        const auto _Fun = _Icu_functions._Pfn_ucal_getCanonicalTimeZoneID.load(_STD memory_order_relaxed);
        return _Fun(id, len, result, resultCapacity, isSystemID, status);
    }

    [[nodiscard]] int32_t __icu_ucal_getDefaultTimeZone(
        UChar* result, int32_t resultCapacity, UErrorCode* ec) noexcept {
        const auto _Fun = _Icu_functions._Pfn_ucal_getDefaultTimeZone.load(_STD memory_order_relaxed);
        return _Fun(result, resultCapacity, ec);
    }

    [[nodiscard]] int32_t __icu_ucal_getTimeZoneDisplayName(const UCalendar* cal, UCalendarDisplayNameType type,
        const char* locale, UChar* result, int32_t resultLength, UErrorCode* status) noexcept {
        const auto _Fun = _Icu_functions._Pfn_ucal_getTimeZoneDisplayName.load(_STD memory_order_relaxed);
        return _Fun(cal, type, locale, result, resultLength, status);
    }

    [[nodiscard]] UBool __icu_ucal_getTimeZoneTransitionDate(
        const UCalendar* cal, UTimeZoneTransitionType type, UDate* transition, UErrorCode* status) noexcept {
        const auto _Fun = _Icu_functions._Pfn_ucal_getTimeZoneTransitionDate.load(_STD memory_order_relaxed);
        return _Fun(cal, type, transition, status);
    }

    [[nodiscard]] const char* __icu_ucal_getTZDataVersion(UErrorCode* status) noexcept {
        const auto _Fun = _Icu_functions._Pfn_ucal_getTZDataVersion.load(_STD memory_order_relaxed);
        return _Fun(status);
    }

    [[nodiscard]] UBool __icu_ucal_inDaylightTime(const UCalendar* cal, UErrorCode* status) noexcept {
        const auto _Fun = _Icu_functions._Pfn_ucal_inDaylightTime.load(_STD memory_order_relaxed);
        return _Fun(cal, status);
    }

    [[nodiscard]] UCalendar* __icu_ucal_open(
        const UChar* zoneID, int32_t len, const char* locale, UCalendarType type, UErrorCode* status) noexcept {
        const auto _Fun = _Icu_functions._Pfn_ucal_open.load(_STD memory_order_relaxed);
        return _Fun(zoneID, len, locale, type, status);
    }

    [[nodiscard]] UEnumeration* __icu_ucal_openTimeZoneIDEnumeration(
        USystemTimeZoneType zoneType, const char* region, const int32_t* rawOffset, UErrorCode* ec) noexcept {
        const auto _Fun = _Icu_functions._Pfn_ucal_openTimeZoneIDEnumeration.load(_STD memory_order_relaxed);
        return _Fun(zoneType, region, rawOffset, ec);
    }

    void __icu_ucal_setMillis(UCalendar* cal, UDate dateTime, UErrorCode* status) noexcept {
        const auto _Fun = _Icu_functions._Pfn_ucal_setMillis.load(_STD memory_order_relaxed);
        _Fun(cal, dateTime, status);
    }

    void __icu_uenum_close(UEnumeration* en) noexcept {
        const auto _Fun = _Icu_functions._Pfn_uenum_close.load(_STD memory_order_relaxed);
        _Fun(en);
    }

    [[nodiscard]] int32_t __icu_uenum_count(UEnumeration* en, UErrorCode* ec) noexcept {
        const auto _Fun = _Icu_functions._Pfn_uenum_count.load(_STD memory_order_relaxed);
        return _Fun(en, ec);
    }

    [[nodiscard]] const UChar* __icu_uenum_unext(UEnumeration* en, int32_t* resultLength, UErrorCode* status) noexcept {
        const auto _Fun = _Icu_functions._Pfn_uenum_unext.load(_STD memory_order_relaxed);
        return _Fun(en, resultLength, status);
    }

    struct _UEnumeration_deleter {
        void operator()(UEnumeration* _En) const noexcept {
            __icu_uenum_close(_En);
        }
    };

    struct _UCalendar_deleter {
        void operator()(UCalendar* _Cal) const noexcept {
            __icu_ucal_close(_Cal);
        }
    };

    [[nodiscard]] const char* _Allocate_wide_to_narrow(
        const char16_t* const _Input, const int _Input_len, __std_tzdb_error& _Err) noexcept {
        const auto _Code_page      = __std_fs_code_page();
        const auto _Input_as_wchar = reinterpret_cast<const wchar_t*>(_Input);
        const auto _Count_result = __std_fs_convert_wide_to_narrow(_Code_page, _Input_as_wchar, _Input_len, nullptr, 0);
        if (_Count_result._Err != __std_win_error::_Success) {
            SetLastError(static_cast<DWORD>(_Count_result._Err));
            _Err = __std_tzdb_error::_Win_error;
            return nullptr;
        }

        _STD unique_ptr<char[]> _Data{new (_STD nothrow) char[_Count_result._Len + 1]};
        if (_Data == nullptr) {
            return nullptr;
        }

        _Data[_Count_result._Len] = '\0';

        const auto _Result =
            __std_fs_convert_wide_to_narrow(_Code_page, _Input_as_wchar, _Input_len, _Data.get(), _Count_result._Len);
        if (_Result._Err != __std_win_error::_Success) {
            SetLastError(static_cast<DWORD>(_Result._Err));
            _Err = __std_tzdb_error::_Win_error;
            return nullptr;
        }

        return _Data.release();
    }

    [[nodiscard]] _STD unique_ptr<const char16_t[]> _Allocate_narrow_to_wide(
        const char* const _Input, const int _Input_len, __std_tzdb_error& _Err) noexcept {
        const auto _Code_page = __std_fs_code_page();
        const auto _Count     = __std_fs_convert_narrow_to_wide(_Code_page, _Input, _Input_len, nullptr, 0);
        if (_Count._Err != __std_win_error::_Success) {
            _Err = __std_tzdb_error::_Win_error;
            return nullptr;
        }

        _STD unique_ptr<char16_t[]> _Data{new (_STD nothrow) char16_t[_Count._Len + 1]};
        if (_Data == nullptr) {
            return nullptr;
        }

        _Data[_Count._Len]          = u'\0';
        const auto _Output_as_wchar = reinterpret_cast<wchar_t*>(_Data.get());

        const auto _Result =
            __std_fs_convert_narrow_to_wide(_Code_page, _Input, _Input_len, _Output_as_wchar, _Count._Len);
        if (_Result._Err != __std_win_error::_Success) {
            _Err = __std_tzdb_error::_Win_error;
            return nullptr;
        }

        return _Data;
    }

    template <class _Function>
    [[nodiscard]] _STD unique_ptr<const char16_t[]> _Get_icu_string_impl(const _Function _Icu_fn,
        const int32_t _Initial_buf_len, int32_t& _Result_len, __std_tzdb_error& _Err) noexcept {
        _STD unique_ptr<char16_t[]> _Str_buf{new (_STD nothrow) char16_t[_Initial_buf_len]};
        if (_Str_buf == nullptr) {
            return nullptr;
        }

        UErrorCode _UErr{U_ZERO_ERROR};
        _Result_len = _Icu_fn(_Str_buf.get(), _Initial_buf_len, &_UErr);
        if (_UErr == U_BUFFER_OVERFLOW_ERROR && _Result_len > 0) {
            _Str_buf.reset(new (_STD nothrow) char16_t[_Result_len + 1]);
            if (_Str_buf == nullptr) {
                return nullptr;
            }

            _UErr       = U_ZERO_ERROR; // reset error.
            _Result_len = _Icu_fn(_Str_buf.get(), _Result_len, &_UErr);
            if (U_FAILURE(_UErr)) {
                _Err = __std_tzdb_error::_Icu_error;
                return nullptr;
            }
        } else if (U_FAILURE(_UErr) || _Result_len <= 0) {
            _Err = __std_tzdb_error::_Icu_error;
            return nullptr;
        }

        return _Str_buf;
    }

    [[nodiscard]] _STD unique_ptr<const char16_t[]> _Get_canonical_id(
        const char16_t* _Id, const int32_t _Len, int32_t& _Result_len, __std_tzdb_error& _Err) noexcept {
        const auto _Icu_fn = [_Id, _Len](UChar* _Result, int32_t _Result_capacity, UErrorCode* _UErr) {
            UBool _Is_system{};
            return __icu_ucal_getCanonicalTimeZoneID(_Id, _Len, _Result, _Result_capacity, &_Is_system, _UErr);
        };
        return _Get_icu_string_impl(_Icu_fn, 32, _Result_len, _Err);
    }

    [[nodiscard]] _STD unique_ptr<const char16_t[]> _Get_default_timezone(
        int32_t& _Result_len, __std_tzdb_error& _Err) noexcept {
        const auto _Icu_fn = [](UChar* _Result, int32_t _Result_capacity, UErrorCode* _UErr) {
            return __icu_ucal_getDefaultTimeZone(_Result, _Result_capacity, _UErr);
        };
        return _Get_icu_string_impl(_Icu_fn, 32, _Result_len, _Err);
    }

    [[nodiscard]] _STD unique_ptr<const char16_t[]> _Get_timezone_short_id(
        UCalendar* const _Cal, const bool _Is_daylight, int32_t& _Result_len, __std_tzdb_error& _Err) noexcept {
        const auto _Display_type =
            _Is_daylight ? UCalendarDisplayNameType::UCAL_SHORT_DST : UCalendarDisplayNameType::UCAL_SHORT_STANDARD;
        const auto _Icu_fn = [_Cal, _Display_type](UChar* _Result, int32_t _Result_capacity, UErrorCode* _UErr) {
            return __icu_ucal_getTimeZoneDisplayName(_Cal, _Display_type, nullptr, _Result, _Result_capacity, _UErr);
        };
        return _Get_icu_string_impl(_Icu_fn, 12, _Result_len, _Err);
    }

    [[nodiscard]] _STD unique_ptr<UCalendar, _UCalendar_deleter> _Get_cal(
        const char* _Tz, const size_t _Tz_len, __std_tzdb_error& _Err) noexcept {
        const auto _Tz_name = _Allocate_narrow_to_wide(_Tz, static_cast<int>(_Tz_len), _Err);
        if (_Tz_name == nullptr) {
            return nullptr;
        }

        UErrorCode _UErr{U_ZERO_ERROR};
        _STD unique_ptr<UCalendar, _UCalendar_deleter> _Cal{
            __icu_ucal_open(_Tz_name.get(), -1, nullptr, UCalendarType::UCAL_DEFAULT, &_UErr)};
        if (U_FAILURE(_UErr)) {
            _Err = __std_tzdb_error::_Icu_error;
        }

        return _Cal;
    }

    template <class _Ty, class _Dx>
    [[nodiscard]] _Ty* _Report_error(_STD unique_ptr<_Ty, _Dx>& _Info, const __std_tzdb_error _Err) noexcept {
        _Info->_Err = _Err;
        return _Info.release();
    }

    template <class _Ty, class _Dx>
    [[nodiscard]] _Ty* _Propagate_error(_STD unique_ptr<_Ty, _Dx>& _Info) noexcept {
        // a bad_alloc returns nullptr and does not set __std_tzdb_error
        return _Info->_Err == __std_tzdb_error::_Success ? nullptr : _Info.release();
    }
} // unnamed namespace

_EXTERN_C

[[nodiscard]] __std_tzdb_time_zones_info* __stdcall __std_tzdb_get_time_zones() noexcept {
    // On exit---
    //    _Info == nullptr          --> bad_alloc
    //    _Info->_Err == _Win_error --> failed, call GetLastError()
    //    _Info->_Err == _Icu_error --> runtime_error interacting with ICU
#pragma warning(push)
#pragma warning(disable : 28182) // TRANSITION, VSO-1558277: Dereferencing NULL pointer.
    _STD unique_ptr<__std_tzdb_time_zones_info, _STD _Tzdb_deleter<__std_tzdb_time_zones_info>> _Info{
        new (_STD nothrow) __std_tzdb_time_zones_info{}};
#pragma warning(pop)
    if (_Info == nullptr) {
        return nullptr;
    }

    if (_Acquire_icu_functions() < _Icu_api_level::_Has_icu_addresses) {
        return _Report_error(_Info, __std_tzdb_error::_Win_error);
    }

    UErrorCode _UErr{U_ZERO_ERROR};
    _Info->_Version = __icu_ucal_getTZDataVersion(&_UErr);
    if (U_FAILURE(_UErr)) {
        return _Report_error(_Info, __std_tzdb_error::_Icu_error);
    }

    _STD unique_ptr<UEnumeration, _UEnumeration_deleter> _Enum{
        __icu_ucal_openTimeZoneIDEnumeration(USystemTimeZoneType::UCAL_ZONE_TYPE_ANY, nullptr, nullptr, &_UErr)};
    if (U_FAILURE(_UErr)) {
        return _Report_error(_Info, __std_tzdb_error::_Icu_error);
    }

    // uenum_count may be expensive but is required to pre-allocate arrays.
    const int32_t _Num_time_zones = __icu_uenum_count(_Enum.get(), &_UErr);
    if (U_FAILURE(_UErr)) {
        return _Report_error(_Info, __std_tzdb_error::_Icu_error);
    }

    _Info->_Num_time_zones = static_cast<size_t>(_Num_time_zones);
    // value-init to ensure __std_tzdb_delete_time_zones() cleanup is valid
    if (const auto _Names = new (_STD nothrow) const char* [_Info->_Num_time_zones] {}; _Names) {
        _Info->_Names = _Names;
    } else {
        return nullptr;
    }

    // value-init to ensure __std_tzdb_delete_time_zones() cleanup is valid
    if (const auto _Links = new (_STD nothrow) const char* [_Info->_Num_time_zones] {}; _Links) {
        _Info->_Links = _Links;
    } else {
        return nullptr;
    }

    for (size_t _Name_idx = 0; _Name_idx < _Info->_Num_time_zones; ++_Name_idx) {
        int32_t _Elem_len{};
        const auto _Elem = __icu_uenum_unext(_Enum.get(), &_Elem_len, &_UErr);
        if (U_FAILURE(_UErr) || _Elem == nullptr) {
            return _Report_error(_Info, __std_tzdb_error::_Icu_error);
        }

        _Info->_Names[_Name_idx] = _Allocate_wide_to_narrow(_Elem, _Elem_len, _Info->_Err);
        if (_Info->_Names[_Name_idx] == nullptr) {
            return _Propagate_error(_Info);
        }

        int32_t _Link_len{};
        const auto _Link = _Get_canonical_id(_Elem, _Elem_len, _Link_len, _Info->_Err);
        if (_Link == nullptr) {
            return _Propagate_error(_Info);
        }

        if (_STD u16string_view{_Elem, static_cast<size_t>(_Elem_len)}
            != _STD u16string_view{_Link.get(), static_cast<size_t>(_Link_len)}) {
            _Info->_Links[_Name_idx] = _Allocate_wide_to_narrow(_Link.get(), _Link_len, _Info->_Err);
            if (_Info->_Links[_Name_idx] == nullptr) {
                return _Propagate_error(_Info);
            }
        }
    }

    return _Info.release();
}

void __stdcall __std_tzdb_delete_time_zones(__std_tzdb_time_zones_info* const _Info) noexcept {
    if (_Info != nullptr) {
        if (_Info->_Names != nullptr) {
            for (size_t _Idx = 0; _Idx < _Info->_Num_time_zones; ++_Idx) {
                delete[] _Info->_Names[_Idx];
            }

            delete[] _Info->_Names;
        }

        if (_Info->_Links != nullptr) {
            for (size_t _Idx = 0; _Idx < _Info->_Num_time_zones; ++_Idx) {
                delete[] _Info->_Links[_Idx];
            }

            delete[] _Info->_Links;
        }

        delete _Info;
    }
}

[[nodiscard]] __std_tzdb_current_zone_info* __stdcall __std_tzdb_get_current_zone() noexcept {
    // On exit---
    //    _Info == nullptr          --> bad_alloc
    //    _Info->_Err == _Win_error --> failed, call GetLastError()
    //    _Info->_Err == _Icu_error --> runtime_error interacting with ICU
#pragma warning(push)
#pragma warning(disable : 28182) // TRANSITION, VSO-1558277: Dereferencing NULL pointer.
    _STD unique_ptr<__std_tzdb_current_zone_info, _STD _Tzdb_deleter<__std_tzdb_current_zone_info>> _Info{
        new (_STD nothrow) __std_tzdb_current_zone_info{}};
#pragma warning(pop)
    if (_Info == nullptr) {
        return nullptr;
    }

    if (_Acquire_icu_functions() < _Icu_api_level::_Has_icu_addresses) {
        return _Report_error(_Info, __std_tzdb_error::_Win_error);
    }

    int32_t _Id_len{};
    const auto _Id_name = _Get_default_timezone(_Id_len, _Info->_Err);
    if (_Id_name == nullptr) {
        return _Propagate_error(_Info);
    }

    _Info->_Tz_name = _Allocate_wide_to_narrow(_Id_name.get(), _Id_len, _Info->_Err);
    if (_Info->_Tz_name == nullptr) {
        return _Propagate_error(_Info);
    }

    return _Info.release();
}

void __stdcall __std_tzdb_delete_current_zone(__std_tzdb_current_zone_info* const _Info) noexcept {
    if (_Info) {
        delete[] _Info->_Tz_name;

        delete _Info;
    }
}

[[nodiscard]] __std_tzdb_sys_info* __stdcall __std_tzdb_get_sys_info(
    const char* _Tz, const size_t _Tz_len, __std_tzdb_epoch_milli _Sys) noexcept {
    // On exit---
    //    _Info == nullptr          --> bad_alloc
    //    _Info->_Err == _Win_error --> failed, call GetLastError()
    //    _Info->_Err == _Icu_error --> runtime_error interacting with ICU
#pragma warning(push)
#pragma warning(disable : 28182) // TRANSITION, VSO-1558277: Dereferencing NULL pointer.
    _STD unique_ptr<__std_tzdb_sys_info, _STD _Tzdb_deleter<__std_tzdb_sys_info>> _Info{
        new (_STD nothrow) __std_tzdb_sys_info{}};
#pragma warning(pop)
    if (_Info == nullptr) {
        return nullptr;
    }

    if (_Acquire_icu_functions() < _Icu_api_level::_Has_icu_addresses) {
        return _Report_error(_Info, __std_tzdb_error::_Win_error);
    }

    // Get the option stored after the time zone name. If there's no option, _Tz[_Tz_len] is the null terminator in the
    // std::string, and will be treated the same as __std_tzdb_sys_info_type::_Full.
    const auto _Type = static_cast<__std_tzdb_sys_info_type>(_Tz[_Tz_len]);

    // TRANSITION, vNext
    // Profiling shows that _Get_cal is a hot path. Its result should be cached (preferably in the time_zone object).
    const auto _Cal = _Get_cal(_Tz, _Tz_len, _Info->_Err);
    if (_Cal == nullptr) {
        return _Propagate_error(_Info);
    }

    UErrorCode _UErr{};
    __icu_ucal_setMillis(_Cal.get(), _Sys, &_UErr);
    if (U_FAILURE(_UErr)) {
        return _Report_error(_Info, __std_tzdb_error::_Icu_error);
    }

    const auto _Is_daylight = __icu_ucal_inDaylightTime(_Cal.get(), &_UErr);
    if (U_FAILURE(_UErr)) {
        return _Report_error(_Info, __std_tzdb_error::_Icu_error);
    }

    _Info->_Save = _Is_daylight ? __icu_ucal_get(_Cal.get(), UCalendarDateFields::UCAL_DST_OFFSET, &_UErr) : 0;
    if (U_FAILURE(_UErr)) {
        return _Report_error(_Info, __std_tzdb_error::_Icu_error);
    }

    _Info->_Offset = __icu_ucal_get(_Cal.get(), UCalendarDateFields::UCAL_ZONE_OFFSET, &_UErr) + _Info->_Save;
    if (U_FAILURE(_UErr)) {
        return _Report_error(_Info, __std_tzdb_error::_Icu_error);
    }

    if (_Type == __std_tzdb_sys_info_type::_Offset_only) {
        return _Info.release();
    }

    UDate _Transition{};
    _Info->_Begin = __icu_ucal_getTimeZoneTransitionDate(_Cal.get(),
                        UTimeZoneTransitionType::UCAL_TZ_TRANSITION_PREVIOUS_INCLUSIVE, &_Transition, &_UErr)
                      ? _Transition
                      : U_DATE_MIN;
    if (U_FAILURE(_UErr)) {
        return _Report_error(_Info, __std_tzdb_error::_Icu_error);
    }

    _Info->_End = __icu_ucal_getTimeZoneTransitionDate(
                      _Cal.get(), UTimeZoneTransitionType::UCAL_TZ_TRANSITION_NEXT, &_Transition, &_UErr)
                    ? _Transition
                    : U_DATE_MAX;
    if (U_FAILURE(_UErr)) {
        return _Report_error(_Info, __std_tzdb_error::_Icu_error);
    }

    if (_Type == __std_tzdb_sys_info_type::_Offset_and_range) {
        return _Info.release();
    }

    int32_t _Abbrev_len{};
    const auto _Abbrev = _Get_timezone_short_id(_Cal.get(), _Is_daylight, _Abbrev_len, _Info->_Err);
    if (_Abbrev == nullptr) {
        return _Propagate_error(_Info);
    }

    _Info->_Abbrev = _Allocate_wide_to_narrow(_Abbrev.get(), _Abbrev_len, _Info->_Err);
    if (_Info->_Abbrev == nullptr) {
        return _Propagate_error(_Info);
    }

    return _Info.release();
}

void __stdcall __std_tzdb_delete_sys_info(__std_tzdb_sys_info* const _Info) noexcept {
    if (_Info) {
        delete[] _Info->_Abbrev;

        delete _Info;
    }
}

[[nodiscard]] __std_tzdb_leap_info* __stdcall __std_tzdb_get_leap_seconds(
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
    static_assert(sizeof(__std_tzdb_leap_info) == 12);
    const auto ls_size   = byte_size / 12;
    *current_reg_ls_size = ls_size;

    __std_tzdb_leap_info* reg_ls_data = nullptr;
    if ((status == ERROR_SUCCESS || status == ERROR_MORE_DATA) && ls_size > prev_reg_ls_size) {
        try {
            reg_ls_data = new __std_tzdb_leap_info[ls_size];

            status = RegQueryValueExW(
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

void __stdcall __std_tzdb_delete_leap_seconds(__std_tzdb_leap_info* _Info) noexcept {
    delete[] _Info;
}

[[nodiscard]] void* __stdcall __std_calloc_crt(const size_t count, const size_t size) noexcept {
    return _calloc_crt(count, size);
}

void __stdcall __std_free_crt(void* p) noexcept {
    _free_crt(p);
}

_END_EXTERN_C
