// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// ios_base basic members

#include <new>
#include <xiosbase>
_STD_BEGIN

#define NSTDSTR 8 // cin, wcin, cout, wcout, cerr, wcerr, clog, wclog

// TRANSITION, ABI: _BADOFF is preserved for binary compatibility
#if defined(_M_CEE_PURE)
extern const streamoff _BADOFF;
const streamoff _BADOFF = -1;
#else
extern _CRTDATA2_IMPORT _PGLOBAL const streamoff _BADOFF;
_PGLOBAL _CRTDATA2_IMPORT const streamoff _BADOFF = -1; // initialize constant for bad file offset
#endif

__PURE_APPDOMAIN_GLOBAL int ios_base::_Index = 0; // initialize source of unique indexes
__PURE_APPDOMAIN_GLOBAL bool ios_base::_Sync = true; // initialize synchronization flag


__PURE_APPDOMAIN_GLOBAL static ios_base* stdstr[NSTDSTR + 2] = {0}; // [1, NSTDSTR] hold pointers to standard streams
__PURE_APPDOMAIN_GLOBAL static char stdopens[NSTDSTR + 2] = {0}; // [1, NSTDSTR] hold open counts for standard streams

// void __CLR_OR_THIS_CALL ios_base::clear(iostate state, bool reraise) { // set state, possibly reraise exception
//     _Mystate = (iostate)(state & _Statmask);
//     if ((_Mystate & _Except) == 0)
//         ;
//     else if (reraise)
//         _RERAISE;
//     else if (_Mystate & _Except & badbit)
//         _THROW(failure("ios_base::badbit set"));
//     else if (_Mystate & _Except & failbit)
//         _THROW(failure("ios_base::failbit set"));
//     else
//         _THROW(failure("ios_base::eofbit set"));
// }

void __CLRCALL_PURE_OR_CDECL ios_base::_Ios_base_dtor(ios_base* _This) { // destroy the object
    if (0 < _This->_Stdstr && 0 < --stdopens[_This->_Stdstr]) {
        return;
    }

    _This->_Tidy();
    delete _This->_Ploc;
}

// ios_base::_Iosarray& __CLR_OR_THIS_CALL ios_base::_Findarr(int idx) { // locate or make a variable array element
//     static _Iosarray stub(0, 0);
//     _Iosarray *p, *q;
//
//     if (idx < 0) { // handle bad index
//         setstate(badbit);
//         return stub;
//     }
//
//     for (p = _Arr, q = 0; p != 0; p = p->_Next)
//         if (p->_Index == idx)
//             return *p; // found element, return it
//         else if (q == 0 && p->_Lo == 0 && p->_Vp == 0)
//             q = p; // found recycling candidate
//
//     if (q != 0) { // recycle existing element
//         q->_Index = idx;
//         return *q;
//     }
//
//     _Arr = new _Iosarray(idx, _Arr); // make a new element
//     return *_Arr;
// }

void __CLRCALL_PURE_OR_CDECL ios_base::_Addstd(ios_base* _This) { // add standard stream to destructor list
    _BEGIN_LOCK(_LOCK_STREAM)
    for (_This->_Stdstr = 0; ++_This->_Stdstr < NSTDSTR;) {
        if (stdstr[_This->_Stdstr] == 0 || stdstr[_This->_Stdstr] == _This) {
            break; // found a candidate
        }
    }

    stdstr[_This->_Stdstr] = _This;
    ++stdopens[_This->_Stdstr];
    _END_LOCK()
}

_STD_END
