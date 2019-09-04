// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// initialize standard input stream

#include <fstream>
#include <iostream>

#pragma warning(disable : 4074)
#pragma init_seg(compiler)
static std::_Init_locks initlocks;

_STD_BEGIN
// OBJECT DECLARATIONS

__PURE_APPDOMAIN_GLOBAL static filebuf fin(_cpp_stdin);

#if defined(_M_CEE_PURE)
__PURE_APPDOMAIN_GLOBAL extern istream cin(&fin);

#else // defined(_M_CEE_PURE)
__PURE_APPDOMAIN_GLOBAL extern _CRTDATA2_IMPORT istream cin(&fin);
#endif // defined(_M_CEE_PURE)

// INITIALIZATION CODE
struct _Init_cin { // ensures that cin is initialized
    __CLR_OR_THIS_CALL _Init_cin() { // initialize cin
        _Ptr_cin = &cin;
        cin.tie(_Ptr_cout);
    }
};

__PURE_APPDOMAIN_GLOBAL static _Init_cin init_cin;

_STD_END
