// xtzdb.h internal header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _XTZDB_H
#define _XTZDB_H
#include <yvals.h>
#if _STL_COMPILER_PREPROCESSOR
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <xutility>

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

struct __std_tzdb_registry_leap_info {
    uint16_t _Year;
    uint16_t _Month;
    uint16_t _Day;
    uint16_t _Hour;
    uint16_t _Negative;
    uint16_t _Reserved;
};

enum class __std_tzdb_error {
    _Success   = 0,
    _Win_error = 1,
    _Icu_error = 2,
};

struct __std_tzdb_time_zones_info {
    __std_tzdb_error _Err;
    size_t _Num_time_zones;
    // ordered list of nullterminated time_zone/time_zone_link names
    const char** _Names;
    // contains corrosponding entry for every name, if:
    //    (_Link[i] == nullptr) then _Names[i] is a time_zone
    //    (_Link[i] != nullptr) then _Names[i] is a time_zone_link to time_zone with name _Link[i]
    const char** _Links;
};

struct __std_tzdb_current_zone_info {
    __std_tzdb_error _Err;
    const char* _Tz_name;
};

_EXTERN_C

_NODISCARD __std_tzdb_time_zones_info* __stdcall __std_tzdb_get_time_zones() noexcept;
void __stdcall __std_tzdb_delete_time_zones(__std_tzdb_time_zones_info* _Info) noexcept;

_NODISCARD __std_tzdb_current_zone_info* __stdcall __std_tzdb_get_current_zone() noexcept;
void __stdcall __std_tzdb_delete_current_zone(__std_tzdb_current_zone_info* _Info) noexcept;

__std_tzdb_registry_leap_info* __stdcall __std_tzdb_get_reg_leap_seconds(
    size_t _Prev_reg_ls_size, size_t* _Current_reg_ls_size) noexcept;

void __stdcall __std_tzdb_delete_reg_leap_seconds(__std_tzdb_registry_leap_info* _Rlsi) noexcept;

_NODISCARD void* __stdcall __std_calloc_crt(size_t _Count, size_t _Size) noexcept;
void __stdcall __std_free_crt(void* _Ptr) noexcept;

_END_EXTERN_C

_STD_BEGIN

template <class _Ty>
class _Crt_allocator {
public:
    using value_type                             = _Ty;
    using propagate_on_container_move_assignment = true_type;
    using is_always_equal                        = true_type;

    constexpr _Crt_allocator() noexcept = default;

    constexpr _Crt_allocator(const _Crt_allocator&) noexcept = default;
    template <class _Other>
    constexpr _Crt_allocator(const _Crt_allocator<_Other>&) noexcept {}

    _NODISCARD __declspec(allocator) _Ty* allocate(_CRT_GUARDOVERFLOW const size_t _Count) {
        const auto _Ptr = __std_calloc_crt(_Count, sizeof(_Ty));
        if (!_Ptr) {
            _Xbad_alloc();
        }
        return static_cast<_Ty*>(_Ptr);
    }

    void deallocate(_Ty* const _Ptr, size_t) noexcept {
        __std_free_crt(_Ptr);
    }
};

_STD_END

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif // _STL_COMPILER_PREPROCESSOR
#endif // _XTZDB_H
