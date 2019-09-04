// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// _Dint function -- IEEE 754 version

#include "xmath.h"

_EXTERN_C_UNLESS_PURE

short _Dint(double* px, short xexp) { // test and drop (scaled) fraction bits
    _Dval* ps = (_Dval*) (char*) px;
    unsigned short frac;
    short xchar = (ps->_Sh[_D0] & _DMASK) >> _DOFF;

    if (xchar == _DMAX) {
        return (ps->_Sh[_D0] & _DFRAC) == 0 && ps->_Sh[_D1] == 0 && ps->_Sh[_D2] == 0 && ps->_Sh[_D3] == 0 ? _INFCODE
                                                                                                           : _NANCODE;
    } else if ((ps->_Sh[_D0] & ~_DSIGN) == 0 && ps->_Sh[_D1] == 0 && ps->_Sh[_D2] == 0 && ps->_Sh[_D3] == 0) {
        return 0;
    }

    xchar = (_DBIAS + 48 + _DOFF + 1) - xchar - xexp;
    if (xchar <= 0) {
        return 0; // no frac bits to drop
    } else if ((48 + _DOFF + 1) <= xchar) { // all frac bits
        ps->_Sh[_D0] &= _DSIGN;
        ps->_Sh[_D1] = 0;
        ps->_Sh[_D2] = 0;
        ps->_Sh[_D3] = 0;
        return _FINITE; // report on frac, not result
    } else { // strip out frac bits
        static const unsigned short mask[] = {//
            0x0000, 0x0001, 0x0003, 0x0007, //
            0x000f, 0x001f, 0x003f, 0x007f, //
            0x00ff, 0x01ff, 0x03ff, 0x07ff, //
            0x0fff, 0x1fff, 0x3fff, 0x7fff};
        static const size_t sub[]          = {_D3, _D2, _D1, _D0};

        frac = mask[xchar & 0xf];
        xchar >>= 4;
        frac &= ps->_Sh[sub[xchar]];
        ps->_Sh[sub[xchar]] ^= frac;
        switch (xchar) { // cascade through!
        case 3:
            frac |= ps->_Sh[_D1];
            ps->_Sh[_D1] = 0;
        case 2:
            frac |= ps->_Sh[_D2];
            ps->_Sh[_D2] = 0;
        case 1:
            frac |= ps->_Sh[_D3];
            ps->_Sh[_D3] = 0;
        }
        return frac != 0 ? _FINITE : 0;
    }
}

_END_EXTERN_C_UNLESS_PURE
