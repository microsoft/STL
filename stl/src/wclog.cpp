// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// initialize standard wide log stream

#include <fstream>
#include <iostream>

#pragma warning(disable : 4074)
#pragma init_seg(compiler)
static std::_Init_locks initlocks;

_STD_BEGIN
// OBJECT DECLARATIONS

__PURE_APPDOMAIN_GLOBAL static wfilebuf wflog(_cpp_stderr);
#if defined(_M_CEE_PURE)
__PURE_APPDOMAIN_GLOBAL extern wostream wclog(&wflog);
#else
__PURE_APPDOMAIN_GLOBAL extern _CRTDATA2_IMPORT wostream wclog(&wflog);
#endif

// INITIALIZATION CODE
struct _Init_wclog { // ensures that wclog is initialized
    __CLR_OR_THIS_CALL _Init_wclog() { // initialize wclog
        _Ptr_wclog = &wclog;
        wclog.tie(_Ptr_wcout);
    }
};
__PURE_APPDOMAIN_GLOBAL static _Init_wclog init_wclog;

_STD_END
