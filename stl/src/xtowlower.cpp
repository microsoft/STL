// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// _Towlower -- convert wchar_t to lower case

#include <__msvc_xlocinfo_types.hpp>
#include <cwchar>

#include "awint.hpp"

_EXTERN_C_UNLESS_PURE

_CRTIMP2_PURE wchar_t __CLRCALL_PURE_OR_CDECL _Towlower(wchar_t _Ch, const _Ctypevec* _Ctype) noexcept {
    // convert element to lower case
    wchar_t _Res = _Ch;
    if (_Ch != WEOF) {
        if (_Ctype->_LocaleName == nullptr && _Ch < 256) { // handle ASCII character in C locale
            if (L'A' <= _Ch && _Ch <= L'Z') {
                _Res = static_cast<wchar_t>(_Ch - L'A' + L'a');
            }
        } else if (__crtLCMapStringW(_Ctype->_LocaleName, LCMAP_LOWERCASE, &_Ch, 1, &_Res, 1) == 0) {
            _Res = _Ch;
        }
    }

    return _Res;
}

#ifdef MRTDLL
_CRTIMP2_PURE unsigned short __CLRCALL_PURE_OR_CDECL _Towlower(unsigned short _Ch, const _Ctypevec* _Ctype) noexcept {
    return _Towlower(static_cast<wchar_t>(_Ch), _Ctype);
}
#endif

_END_EXTERN_C_UNLESS_PURE
