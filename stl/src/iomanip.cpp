// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// instantiations of iomanip

#include <iomanip>
_STD_BEGIN
// FUNCTION resetiosflags
static void __cdecl rsfun(ios_base& iostr, ios_base::fmtflags mask) { // reset specified format flags
    iostr.setf(ios_base::_Fmtzero, mask);
}

// FUNCTION setiosflags
static void __cdecl sifun(ios_base& iostr, ios_base::fmtflags mask) { // set specified format flags
    iostr.setf(ios_base::_Fmtmask, mask);
}

// FUNCTION setbase
static void __cdecl sbfun(ios_base& iostr, int base) { // set base
    iostr.setf(base == 8 ? ios_base::oct : base == 10 ? ios_base::dec : base == 16 ? ios_base::hex : ios_base::_Fmtzero,
        ios_base::basefield);
}

// FUNCTION setprecision
static void __cdecl spfun(ios_base& iostr, streamsize prec) { // set precision
    iostr.precision(prec);
}

// FUNCTION setw
static void __cdecl swfun(ios_base& iostr, streamsize wide) { // set width
    iostr.width(wide);
}

_MRTIMP2 _Smanip<ios_base::fmtflags> __cdecl resetiosflags(
    ios_base::fmtflags mask) { // manipulator to reset format flags
    return _Smanip<ios_base::fmtflags>(&rsfun, mask);
}

_MRTIMP2 _Smanip<ios_base::fmtflags> __cdecl setiosflags(ios_base::fmtflags mask) { // manipulator to set format flags
    return _Smanip<ios_base::fmtflags>(&sifun, mask);
}

_MRTIMP2 _Smanip<int> __cdecl setbase(int base) { // manipulator to set base
    return _Smanip<int>(&sbfun, base);
}

_MRTIMP2 _Smanip<streamsize> __cdecl setprecision(streamsize prec) { // manipulator to set precision
    return _Smanip<streamsize>(&spfun, prec);
}

_MRTIMP2 _Smanip<streamsize> __cdecl setw(streamsize wide) { // manipulator to set width
    return _Smanip<streamsize>(&swfun, wide);
}
_STD_END
