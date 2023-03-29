// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// _FDscale function -- IEEE 754 version

#include "xmath.hpp"

_EXTERN_C_UNLESS_PURE

short _FDscale(float* px, long lexp) { // scale *px by 2^xexp with checking
    const auto ps = reinterpret_cast<_Fval*>(px);
    short xchar   = static_cast<short>((ps->_Sh[_F0] & _FMASK) >> _FOFF);

    if (xchar == _FMAX) {
        return (ps->_Sh[_F0] & _FFRAC) != 0 || ps->_Sh[_F1] != 0 ? _NANCODE : _INFCODE;
    } else if (xchar == 0 && 0 < (xchar = _FDnorm(ps))) {
        return 0;
    }

    if (0 < lexp && _FMAX - xchar <= lexp) { // overflow, return +/-INF
        *px = ps->_Sh[_F0] & _FSIGN ? -_FInf._Float : _FInf._Float;
        return _INFCODE;
    } else if (-xchar < lexp) { // finite result, repack
        ps->_Sh[_F0] = static_cast<unsigned short>(ps->_Sh[_F0] & ~_FMASK | (lexp + xchar) << _FOFF);
        return _FINITE;
    } else { // denormalized, scale
        unsigned short sign = static_cast<unsigned short>(ps->_Sh[_F0] & _FSIGN);

        ps->_Sh[_F0] = static_cast<unsigned short>(1 << _FOFF | ps->_Sh[_F0] & _FFRAC);
        lexp += xchar - 1;
        if (lexp < -(16 + 1 + _FOFF) || 0 <= lexp) { // underflow, return +/-0
            ps->_Sh[_F0] = sign;
            ps->_Sh[_F1] = 0;
            return 0;
        } else { // nonzero, align fraction
            short xexp         = static_cast<short>(lexp);
            unsigned short psx = 0;

            if (xexp <= -16) { // scale by words
                psx          = ps->_Sh[_F1];
                ps->_Sh[_F1] = ps->_Sh[_F0];
                ps->_Sh[_F0] = 0;
                xexp += 16;
            }

            if (xexp != 0) { // scale by bits
                xexp         = -xexp;
                psx          = (ps->_Sh[_F1] << (16 - xexp)) | (psx != 0 ? 1 : 0);
                ps->_Sh[_F1] = static_cast<unsigned short>(ps->_Sh[_F1] >> xexp | ps->_Sh[_F0] << (16 - xexp));
                ps->_Sh[_F0] >>= xexp;
            }

            ps->_Sh[_F0] |= sign;
            if ((0x8000 < psx || 0x8000 == psx && (ps->_Sh[_F1] & 0x0001) != 0) && (++ps->_Sh[_F1] & 0xffff) == 0) {
                ++ps->_Sh[_F0]; // round up
            } else if (ps->_Sh[_F0] == sign && ps->_Sh[_F1] == 0) {
                return 0;
            }

            return _FINITE;
        }
    }
}

_END_EXTERN_C_UNLESS_PURE
