// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// initialize standard wide error stream

#include <fstream>
#include <iostream>

#pragma warning(disable : 4074)
#pragma init_seg(compiler)
static std::_Init_locks initlocks;

_STD_BEGIN
// OBJECT DECLARATIONS

__PURE_APPDOMAIN_GLOBAL static wfilebuf wferr(_cpp_stderr);
#if defined(_M_CEE_PURE)
__PURE_APPDOMAIN_GLOBAL extern wostream wcerr(&wferr);
#else
__PURE_APPDOMAIN_GLOBAL extern _CRTDATA2_IMPORT wostream wcerr(&wferr);
#endif

// INITIALIZATION CODE
struct _Init_wcerr { // ensures that wcerr is initialized
    __CLR_OR_THIS_CALL _Init_wcerr() { // initialize wcerr
        _Ptr_wcerr = &wcerr;
        wcerr.tie(_Ptr_wcout);
        wcerr.setf(ios_base::unitbuf);
    }
};
__PURE_APPDOMAIN_GLOBAL static _Init_wcerr init_wcerr;

_STD_END
