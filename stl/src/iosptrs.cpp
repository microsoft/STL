// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// iostream object pointers

#include <iostream>

#include <Windows.h>

#include "init_locks.hpp"

_STD_BEGIN

#if defined(_M_CEE) && !defined(_M_CEE_MIXED)
#error This file cannot be built /clr:pure, etc. because of the use of _PGLOBAL.
#endif

#pragma warning(disable : 4074)
#pragma init_seg(compiler)
_PGLOBAL static std::_Init_locks initlocks;

__PURE_APPDOMAIN_GLOBAL extern _CRTDATA2_IMPORT istream* _Ptr_cin  = nullptr;
__PURE_APPDOMAIN_GLOBAL extern _CRTDATA2_IMPORT ostream* _Ptr_cout = nullptr;
__PURE_APPDOMAIN_GLOBAL extern _CRTDATA2_IMPORT ostream* _Ptr_cerr = nullptr;
__PURE_APPDOMAIN_GLOBAL extern _CRTDATA2_IMPORT ostream* _Ptr_clog = nullptr;

__PURE_APPDOMAIN_GLOBAL extern _CRTDATA2_IMPORT wistream* _Ptr_wcin  = nullptr;
__PURE_APPDOMAIN_GLOBAL extern _CRTDATA2_IMPORT wostream* _Ptr_wcout = nullptr;
__PURE_APPDOMAIN_GLOBAL extern _CRTDATA2_IMPORT wostream* _Ptr_wcerr = nullptr;
__PURE_APPDOMAIN_GLOBAL extern _CRTDATA2_IMPORT wostream* _Ptr_wclog = nullptr;
_STD_END

constexpr int _Nats = 10; // fclose, xgetloc, locks, facet free, etc.

__PURE_APPDOMAIN_GLOBAL static void(__cdecl* atfuns_cdecl[_Nats])() = {};
__PURE_APPDOMAIN_GLOBAL static size_t atcount_cdecl                 = _Nats;
_MRTIMP2 void __cdecl _Atexit(void(__cdecl* pf)()) { // add to wrapup list
    if (atcount_cdecl == 0) {
        abort(); // stack full, give up
    } else {
        atfuns_cdecl[--atcount_cdecl] = reinterpret_cast<void(__cdecl*)()>(EncodePointer(reinterpret_cast<void*>(pf)));
    }
}

struct _Init_atexit { // controller for atexit processing
    __CLR_OR_THIS_CALL ~_Init_atexit() noexcept { // process wrapup functions
        while (atcount_cdecl < _Nats) {
            const auto pf = reinterpret_cast<void(__cdecl*)()>(
                DecodePointer(reinterpret_cast<void*>(atfuns_cdecl[atcount_cdecl++])));
            if (pf) {
                pf();
            }
        }
    }
};

__PURE_APPDOMAIN_GLOBAL static _Init_atexit init_atexit;
