// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// _Dnorm function -- IEEE 754 version

#include "xmath.hpp"

_EXTERN_C_UNLESS_PURE

short _Dnorm(_Dval* ps) { // normalize double fraction
    short xchar         = 1;
    unsigned short sign = static_cast<unsigned short>(ps->_Sh[_D0] & _DSIGN);

    if ((ps->_Sh[_D0] &= _DFRAC) != 0 || ps->_Sh[_D1] || ps->_Sh[_D2] || ps->_Sh[_D3]) { // nonzero, scale
        for (; ps->_Sh[_D0] == 0; xchar -= 16) { // shift left by 16
            ps->_Sh[_D0] = ps->_Sh[_D1];
            ps->_Sh[_D1] = ps->_Sh[_D2];
            ps->_Sh[_D2] = ps->_Sh[_D3];
            ps->_Sh[_D3] = 0;
        }
        for (; ps->_Sh[_D0] < 1 << _DOFF; --xchar) { // shift left by 1
            ps->_Sh[_D0] = static_cast<unsigned short>(ps->_Sh[_D0] << 1 | ps->_Sh[_D1] >> 15);
            ps->_Sh[_D1] = static_cast<unsigned short>(ps->_Sh[_D1] << 1 | ps->_Sh[_D2] >> 15);
            ps->_Sh[_D2] = static_cast<unsigned short>(ps->_Sh[_D2] << 1 | ps->_Sh[_D3] >> 15);
            ps->_Sh[_D3] <<= 1;
        }
        for (; 1 << (_DOFF + 1) <= ps->_Sh[_D0]; ++xchar) { // shift right by 1
            ps->_Sh[_D3] = static_cast<unsigned short>(ps->_Sh[_D3] >> 1 | ps->_Sh[_D2] << 15);
            ps->_Sh[_D2] = static_cast<unsigned short>(ps->_Sh[_D2] >> 1 | ps->_Sh[_D1] << 15);
            ps->_Sh[_D1] = static_cast<unsigned short>(ps->_Sh[_D1] >> 1 | ps->_Sh[_D0] << 15);
            ps->_Sh[_D0] >>= 1;
        }
        ps->_Sh[_D0] &= _DFRAC;
    }
    ps->_Sh[_D0] |= sign;
    return xchar;
}

_END_EXTERN_C_UNLESS_PURE
