// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// initialize standard error stream

#include <fstream>
#include <iostream>

#include "init_locks.hpp"

#pragma warning(disable : 4074)
#pragma init_seg(compiler)
static std::_Init_locks initlocks;

_STD_BEGIN

__PURE_APPDOMAIN_GLOBAL static filebuf ferr(stderr);

#if defined(_M_CEE_PURE)
__PURE_APPDOMAIN_GLOBAL extern ostream cerr(&ferr);
#else // ^^^ defined(_M_CEE_PURE) / !defined(_M_CEE_PURE) vvv
__PURE_APPDOMAIN_GLOBAL extern _CRTDATA2_IMPORT ostream cerr(&ferr);
#endif // ^^^ !defined(_M_CEE_PURE) ^^^

struct _Init_cerr { // ensures that cerr is initialized
    __CLR_OR_THIS_CALL _Init_cerr() { // initialize cerr
        _Ptr_cerr = &cerr;
        cerr.tie(_Ptr_cout);
        cerr.setf(ios_base::unitbuf);
    }
};

__PURE_APPDOMAIN_GLOBAL static _Init_cerr init_cerr;

_STD_END
