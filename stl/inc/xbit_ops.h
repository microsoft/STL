// xbit_ops.h internal header (core)

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _XBIT_OPS_H
#define _XBIT_OPS_H
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR

#include <cstdint>

#include _STL_INTRIN_HEADER

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
#else // ^^^ 64-bit / 32-bit vvv
    _Result = 31;
#endif // 64 vs. 32-bit

    while ((size_t{1} << _Result) > _Value) {
        --_Result;
    }

#else // ^^^ _M_CEE_PURE / !_M_CEE_PURE vvv
#ifdef _WIN64
    _BitScanReverse64(&_Result, _Value); // lgtm [cpp/conditionallyuninitializedvariable]
#else // ^^^ 64-bit / 32-bit vvv
    _BitScanReverse(&_Result, _Value); // lgtm [cpp/conditionallyuninitializedvariable]
#endif // 64 vs. 32-bit
#endif // _M_CEE_PURE

    return _Result;
}

_NODISCARD inline unsigned long _Ceiling_of_log_2(const size_t _Value) noexcept { // returns ceil(log_2(_Value))
                                                                                  // pre: _Value > 1
    return 1 + _Floor_of_log_2(_Value - 1);
}

_NODISCARD inline uint32_t _Bit_scan_reverse(const uint32_t _Value) noexcept {
    unsigned long _Index; // Intentionally uninitialized for better codegen

    if (_BitScanReverse(&_Index, _Value)) {
        return _Index + 1;
    }

    return 0;
}

_NODISCARD inline uint32_t _Bit_scan_reverse(const uint64_t _Value) noexcept {
    unsigned long _Index; // Intentionally uninitialized for better codegen

#ifdef _WIN64
    if (_BitScanReverse64(&_Index, _Value)) {
        return _Index + 1;
    }
#else // ^^^ 64-bit / 32-bit vvv
    uint32_t _Ui32 = static_cast<uint32_t>(_Value >> 32);

    if (_BitScanReverse(&_Index, _Ui32)) {
        return _Index + 1 + 32;
    }

    _Ui32 = static_cast<uint32_t>(_Value);

    if (_BitScanReverse(&_Index, _Ui32)) {
        return _Index + 1;
    }
#endif // ^^^ 32-bit ^^^

    return 0;
}

_STD_END

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif // _STL_COMPILER_PREPROCESSOR
#endif // _XBIT_OPS_H
