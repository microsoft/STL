// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <yvals.h>

_STD_BEGIN
// TRANSITION, ABI: _Debug_message() overloads are preserved for binary compatibility

#ifdef _DEBUG
_CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Debug_message(
    const wchar_t* message, const wchar_t* file, unsigned int line) { // report error and die
    if (::_CrtDbgReportW(_CRT_ASSERT, file, line, nullptr, L"%ls", message) == 1) {
        ::_CrtDbgBreak();
    }
}

_CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Debug_message(
    const unsigned short* message, const unsigned short* file, unsigned int line) { // report error and die
    _Debug_message((wchar_t*) message, (wchar_t*) file, line);
}
#endif
_STD_END
