// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// iostream object pointers

#include <iostream>

#include <Windows.h>
_STD_BEGIN

#if defined(_M_CEE) && !defined(_M_CEE_MIXED)
#error This file cannot be built /clr:pure, etc. because of the use of _PGLOBAL.
#endif

#pragma warning(disable : 4074)
#pragma init_seg(compiler)
_PGLOBAL static std::_Init_locks initlocks;

// OBJECT DECLARATIONS
__PURE_APPDOMAIN_GLOBAL extern _CRTDATA2_IMPORT istream* _Ptr_cin  = 0;
__PURE_APPDOMAIN_GLOBAL extern _CRTDATA2_IMPORT ostream* _Ptr_cout = 0;
__PURE_APPDOMAIN_GLOBAL extern _CRTDATA2_IMPORT ostream* _Ptr_cerr = 0;
__PURE_APPDOMAIN_GLOBAL extern _CRTDATA2_IMPORT ostream* _Ptr_clog = 0;

// WIDE OBJECTS
__PURE_APPDOMAIN_GLOBAL extern _CRTDATA2_IMPORT wistream* _Ptr_wcin  = 0;
__PURE_APPDOMAIN_GLOBAL extern _CRTDATA2_IMPORT wostream* _Ptr_wcout = 0;
__PURE_APPDOMAIN_GLOBAL extern _CRTDATA2_IMPORT wostream* _Ptr_wcerr = 0;
__PURE_APPDOMAIN_GLOBAL extern _CRTDATA2_IMPORT wostream* _Ptr_wclog = 0;
_STD_END

// FINALIZATION CODE
#define NATS 10 // fclose, xgetloc, locks, facet free, etc.

// static data
__PURE_APPDOMAIN_GLOBAL static void(__cdecl* atfuns_cdecl[NATS])() = {0};
__PURE_APPDOMAIN_GLOBAL static size_t atcount_cdecl                = {NATS};
_MRTIMP2 void __cdecl _Atexit(void(__cdecl* pf)()) { // add to wrapup list
    if (atcount_cdecl == 0) {
        abort(); // stack full, give up
    } else {
        atfuns_cdecl[--atcount_cdecl] = (void(__cdecl*)()) EncodePointer(pf);
    }
}

struct _Init_atexit { // controller for atexit processing
    __CLR_OR_THIS_CALL ~_Init_atexit() noexcept { // process wrapup functions
        while (atcount_cdecl < NATS) {
            void(__cdecl * pf)() = (void(__cdecl*)()) DecodePointer(atfuns_cdecl[atcount_cdecl++]);
            if (pf) {
                (*pf)();
            }
        }
    }
};

__PURE_APPDOMAIN_GLOBAL static _Init_atexit init_atexit;
