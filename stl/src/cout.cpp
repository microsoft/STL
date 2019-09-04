// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// initialize standard output stream

#include <fstream>
#include <iostream>

#pragma warning(disable : 4074)
#pragma init_seg(compiler)
static std::_Init_locks initlocks;

_STD_BEGIN
// OBJECT DECLARATIONS

__PURE_APPDOMAIN_GLOBAL static filebuf fout(_cpp_stdout);

#if defined(_M_CEE_PURE)
__PURE_APPDOMAIN_GLOBAL extern ostream cout(&fout);

#else // defined(_M_CEE_PURE)
__PURE_APPDOMAIN_GLOBAL extern _CRTDATA2_IMPORT ostream cout(&fout);
#endif // defined(_M_CEE_PURE)

// INITIALIZATION CODE
struct _Init_cout { // ensures that cout is initialized
    __CLR_OR_THIS_CALL _Init_cout() { // initialize cout
        _Ptr_cout = &cout;
        if (_Ptr_cin != 0) {
            _Ptr_cin->tie(_Ptr_cout);
        }

        if (_Ptr_cerr != 0) {
            _Ptr_cerr->tie(_Ptr_cout);
        }

        if (_Ptr_clog != 0) {
            _Ptr_clog->tie(_Ptr_cout);
        }
    }
};

__PURE_APPDOMAIN_GLOBAL static _Init_cout init_cout;

_STD_END
