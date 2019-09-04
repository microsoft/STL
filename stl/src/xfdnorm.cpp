// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// _FDnorm function -- IEEE 754 version

#include "xmath.h"

_EXTERN_C_UNLESS_PURE

short _FDnorm(_Fval* ps) { // normalize float fraction
    short xchar;
    unsigned short sign = (unsigned short) (ps->_Sh[_F0] & _FSIGN);

    xchar = 1;
    if ((ps->_Sh[_F0] &= _FFRAC) != 0 || ps->_Sh[_F1]) { // nonzero, scale
        if (ps->_Sh[_F0] == 0) {
            ps->_Sh[_F0] = ps->_Sh[_F1];
            ps->_Sh[_F1] = 0;
            xchar -= 16;
        }

        for (; ps->_Sh[_F0] < 1 << _FOFF; --xchar) { // shift left by 1
            ps->_Sh[_F0] = (unsigned short) (ps->_Sh[_F0] << 1 | ps->_Sh[_F1] >> 15);
            ps->_Sh[_F1] <<= 1;
        }
        for (; 1 << (_FOFF + 1) <= ps->_Sh[_F0]; ++xchar) { // shift right by 1
            ps->_Sh[_F1] = (unsigned short) (ps->_Sh[_F1] >> 1 | ps->_Sh[_F0] << 15);
            ps->_Sh[_F0] >>= 1;
        }
        ps->_Sh[_F0] &= _FFRAC;
    }
    ps->_Sh[_F0] |= sign;
    return xchar;
}

_END_EXTERN_C_UNLESS_PURE
