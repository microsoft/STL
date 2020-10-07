// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// _Getwctype -- return character classification flags for wide character

#include <yvals.h>

#include <cwchar>
#include <xlocinfo.h>

#include "awint.hpp"

_EXTERN_C_UNLESS_PURE

_CRTIMP2_PURE short __CLRCALL_PURE_OR_CDECL _Getwctype(wchar_t _Ch,
    const _Ctypevec* _Ctype) { // return character classification flags for _Ch
    _CRT_UNUSED(_Ctype);
    short _Mask;
    return static_cast<short>(GetStringTypeW(CT_CTYPE1, &_Ch, 1, reinterpret_cast<LPWORD>(&_Mask)) == 0 ? 0 : _Mask);
}

_CRTIMP2_PURE const wchar_t* __CLRCALL_PURE_OR_CDECL _Getwctypes(const wchar_t* _First, const wchar_t* _Last,
    short* _Dest, const _Ctypevec* _Ctype) { // get mask sequence for elements in [_First, _Last)
    _CRT_UNUSED(_Ctype);
    GetStringTypeW(CT_CTYPE1, _First, static_cast<int>(_Last - _First), reinterpret_cast<LPWORD>(_Dest));
    return _Last;
}

_END_EXTERN_C_UNLESS_PURE
