// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Convert wide character to multibyte character, with locale.

#include <yvals.h>

#include <__msvc_xlocinfo_types.hpp>
#include <cerrno>
#include <climits> // for MB_LEN_MAX
#include <clocale>
#include <cstdlib>
#include <mbctype.h>

#include <Windows.h>

_EXTERN_C_UNLESS_PURE

_CRTIMP2_PURE _Cvtvec __CLRCALL_PURE_OR_CDECL _Getcvt() noexcept { // get conversion info for current locale
    _Cvtvec _Cvt = {0};

    _Cvt._Page      = ___lc_codepage_func();
    _Cvt._Mbcurmax  = ___mb_cur_max_func();
    _Cvt._Isclocale = ___lc_locale_name_func()[LC_CTYPE] == nullptr;

    if (!_Cvt._Isclocale) {
        const unsigned short* const _Ctype_table = __pctype_func();
        for (int _Idx = 0; _Idx < 256; ++_Idx) {
            if (_Ctype_table[_Idx] & _LEADBYTE) {
                _Cvt._Isleadbyte[_Idx >> 3] |= 1 << (_Idx & 7);
            }
        }
    }

    return _Cvt;
}

// int _Wcrtomb() - Convert wide character to multibyte character.
//
// Purpose:
//     Convert a wide character into the equivalent multi-byte character,
//     according to the specified LC_CTYPE category, or the current locale.
//     [ANSI].
//
// Entry:
//     char* s             = pointer to multibyte character
//     wchar_t wchar       = source wide character
//     mbstate_t* pst      = pointer to state (not used)
//     const _Cvtvec* ploc = pointer to locale info
//
// Exit:
// Returns:
//     -1 (if error) or number of bytes comprising converted mbc
//
// Exceptions:
//     None.

_CRTIMP2_PURE _Success_(return != -1) int __CLRCALL_PURE_OR_CDECL
    _Wcrtomb(_Out_ char* s, wchar_t wchar, mbstate_t* pst, const _Cvtvec* ploc) noexcept {
    _CRT_UNUSED(pst);
    if (ploc->_Isclocale) {
        if (wchar > 255) { // validate high byte
            errno = EILSEQ;
            return -1;
        }

        *s = static_cast<char>(wchar);
        return sizeof(char);
    } else {
        BOOL defused = 0;
        _Cvtvec cvtvec;

        if (ploc == nullptr) {
            cvtvec = _Getcvt();
            ploc   = &cvtvec;
        }

        const int size = WideCharToMultiByte(ploc->_Page, 0, &wchar, 1, s, ploc->_Mbcurmax, nullptr, &defused);
        if (size == 0 || defused) {
            errno = EILSEQ;
            return -1;
        }

        return size;
    }
}

#ifdef MRTDLL
_CRTIMP2_PURE int __CLRCALL_PURE_OR_CDECL _Wcrtomb(
    char* s, unsigned short wchar, mbstate_t* pst, const _Cvtvec* ploc) noexcept {
    return _Wcrtomb(s, static_cast<wchar_t>(wchar), pst, ploc);
}
#endif // defined(MRTDLL)

// TRANSITION, ABI: __Wcrtomb_lk() is preserved for binary compatibility
_CRTIMP2_PURE int __CLRCALL_PURE_OR_CDECL __Wcrtomb_lk(
    char* s, wchar_t wchar, mbstate_t* pst, const _Cvtvec* ploc) noexcept {
    return _Wcrtomb(s, wchar, pst, ploc);
}

_END_EXTERN_C_UNLESS_PURE
