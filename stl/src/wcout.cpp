// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// initialize standard wide output stream

#include <fstream>
#include <iostream>

#include "init_locks.hpp"

#pragma warning(disable : 4074)
#pragma init_seg(compiler)
static std::_Init_locks initlocks;

_STD_BEGIN

__PURE_APPDOMAIN_GLOBAL static wfilebuf wfout(_cpp_stdout);
#if defined(_M_CEE_PURE)
__PURE_APPDOMAIN_GLOBAL extern wostream wcout(&wfout);
#else
__PURE_APPDOMAIN_GLOBAL extern _CRTDATA2_IMPORT wostream wcout(&wfout);
#endif

struct _Init_wcout { // ensures that wcout is initialized
    __CLR_OR_THIS_CALL _Init_wcout() { // initialize wcout
        _Ptr_wcout = &wcout;
        if (_Ptr_wcin != nullptr) {
            _Ptr_wcin->tie(_Ptr_wcout);
        }

        if (_Ptr_wcerr != nullptr) {
            _Ptr_wcerr->tie(_Ptr_wcout);
        }
    }
};
__PURE_APPDOMAIN_GLOBAL static _Init_wcout init_wcout;

_STD_END
