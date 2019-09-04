// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// initialize standard wide input stream

#include <fstream>
#include <iostream>

#pragma warning(disable : 4074)
#pragma init_seg(compiler)
static std::_Init_locks initlocks;

_STD_BEGIN
// OBJECT DECLARATIONS

__PURE_APPDOMAIN_GLOBAL static wfilebuf wfin(_cpp_stdin);
#if defined(_M_CEE_PURE)
__PURE_APPDOMAIN_GLOBAL extern wistream wcin(&wfin);
#else
__PURE_APPDOMAIN_GLOBAL extern _CRTDATA2_IMPORT wistream wcin(&wfin);
#endif

// INITIALIZATION CODE
struct _Init_wcin { // ensures that wcin is initialized
    __CLR_OR_THIS_CALL _Init_wcin() { // initialize wcin
        _Ptr_wcin = &wcin;
        wcin.tie(_Ptr_wcout);
    }
};
__PURE_APPDOMAIN_GLOBAL static _Init_wcin init_wcin;

_STD_END
