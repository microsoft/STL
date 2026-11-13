// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <yvals.h>

_STD_BEGIN
#ifdef _DEBUG
// TRANSITION, ABI: preserved for binary compatibility
_CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Debug_message(
    const wchar_t* message, const wchar_t* file, unsigned int line) { // report error and die
    if (::_CrtDbgReportW(_CRT_ASSERT, file, line, nullptr, L"%ls", message) == 1) {
        ::_CrtDbgBreak();
    }
}

// TRANSITION, ABI: preserved for binary compatibility
_CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Debug_message(
    const unsigned short* message, const unsigned short* file, unsigned int line) { // report error and die
    _Debug_message(reinterpret_cast<const wchar_t*>(message), reinterpret_cast<const wchar_t*>(file), line);
}
#endif // ^^^ defined(_DEBUG) ^^^
_STD_END
