// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// initialize standard wide output stream

#include <fstream>
#include <iostream>

#pragma warning(disable : 4074)
#pragma init_seg(compiler)
static std::_Init_locks initlocks;

_STD_BEGIN
// OBJECT DECLARATIONS

__PURE_APPDOMAIN_GLOBAL static wfilebuf wfout(_cpp_stdout);
#if defined(_M_CEE_PURE)
__PURE_APPDOMAIN_GLOBAL extern wostream wcout(&wfout);
#else
__PURE_APPDOMAIN_GLOBAL extern _CRTDATA2_IMPORT wostream wcout(&wfout);
#endif

// INITIALIZATION CODE
struct _Init_wcout { // ensures that wcout is initialized
    __CLR_OR_THIS_CALL _Init_wcout() { // initialize wcout
        _Ptr_wcout = &wcout;
        if (_Ptr_wcin != 0) {
            _Ptr_wcin->tie(_Ptr_wcout);
        }

        if (_Ptr_wcerr != 0) {
            _Ptr_wcerr->tie(_Ptr_wcout);
        }

        if (_Ptr_wclog != 0) {
            _Ptr_wclog->tie(_Ptr_wcout);
        }
    }
};
__PURE_APPDOMAIN_GLOBAL static _Init_wcout init_wcout;

_STD_END
