// xbit_ops.h internal header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _XBIT_OPS_H
#define _XBIT_OPS_H
#include <yvals.h>
#if _STL_COMPILER_PREPROCESSOR

#include <intrin0.h>

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

_STD_BEGIN

_NODISCARD inline unsigned long _Floor_of_log_2(size_t _Value) noexcept { // returns floor(log_2(_Value))
    _Value |= size_t{1}; // avoid undefined answer from _BitScanReverse for 0
    unsigned long _Result;

#ifdef _M_CEE_PURE
#ifdef _WIN64
    _Result = 63;
#else // ^^^ 64-bit ^^^ / vvv 32-bit vvv
    _Result = 31;
#endif // 64 vs. 32-bit

    while ((size_t{1} << _Result) > _Value) {
        --_Result;
    }

#else // ^^^ _M_CEE_PURE ^^^ // vvv !_M_CEE_PURE vvv
#ifdef _WIN64
    _BitScanReverse64(&_Result, _Value);
#else // ^^^ 64-bit ^^^ / vvv 32-bit vvv
    _BitScanReverse(&_Result, _Value);
#endif // 64 vs. 32-bit
#endif // _M_CEE_PURE

    return _Result;
}

_NODISCARD inline unsigned long _Ceiling_of_log_2(const size_t _Value) noexcept { // returns ceil(log_2(_Value))
                                                                                  // pre: _Value > 1
    return 1 + _Floor_of_log_2(_Value - 1);
}

_STD_END

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif // _STL_COMPILER_PREPROCESSOR
#endif // _XBIT_OPS_H
