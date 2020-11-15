// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _XTZDB_H
#define _XTZDB_H
#include <yvals.h>
#if _STL_COMPILER_PREPROCESSOR
#include <cstddef>
#include <cstdint>

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

struct _RegistryLeapSecondInfo {
    uint16_t _Year;
    uint16_t _Month;
    uint16_t _Day;
    uint16_t _Hour;
    uint16_t _Negative;
    uint16_t _Reserved;
};

_EXTERN_C

_RegistryLeapSecondInfo* __stdcall __std_tzdb_get_reg_leap_seconds(
    size_t _Prev_reg_ls_size, size_t* _Current_reg_ls_size);

_END_EXTERN_C

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif // _STL_COMPILER_PREPROCESSOR
#endif // _XTZDB_H
