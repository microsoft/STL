// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Implements a win32 API wrapper for <format>

// This must be as small as possible, because its contents are
// injected into the msvcprt.lib and msvcprtd.lib import libraries.
// Do not include or define anything else here.
// In particular, basic_string must not be included here.

#include <__msvc_xlocinfo_types.hpp>
#include <xfilesystem_abi.h>

#include <Windows.h>

static_assert(__std_code_page::_Acp == __std_code_page{CP_ACP});

extern "C" [[nodiscard]] __std_win_error __stdcall __std_get_cvt(
    const __std_code_page _Codepage, _Cvtvec* const _Pcvt) noexcept {
    // get conversion info for an arbitrary codepage
    *_Pcvt = {};

    CPINFOEXW _Info{};
    const DWORD _Flags = 0; // reserved, must be zero
    if (!GetCPInfoExW(static_cast<UINT>(_Codepage), _Flags, &_Info)) {
        // NB: the only documented failure mode for GetCPInfoExW is ERROR_INVALID_PARAMETER,
        // so in practice it should never fail for CP_ACP.
        return __std_win_error{GetLastError()};
    }

    _Pcvt->_Page     = _Info.CodePage;
    _Pcvt->_Mbcurmax = _Info.MaxCharSize;

    for (int _Idx = 0; _Idx < MAX_LEADBYTES; _Idx += 2) {
        if (_Info.LeadByte[_Idx] == 0 && _Info.LeadByte[_Idx + 1] == 0) {
            break;
        }

        for (unsigned char _First = _Info.LeadByte[_Idx], _Last = _Info.LeadByte[_Idx + 1]; _First != _Last; ++_First) {
            _Pcvt->_Isleadbyte[_First >> 3] |= 1u << (_First & 0b111u);
        }
    }

    return __std_win_error::_Success;
}
