// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "xmath.hpp"

namespace {
    short _Dnorm(_Dval* ps) noexcept { // normalize double fraction
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

    short _Dscale(double* px, long lexp) noexcept { // scale *px by 2^xexp with checking
        const auto ps = reinterpret_cast<_Dval*>(px);
        short xchar   = static_cast<short>((ps->_Sh[_D0] & _DMASK) >> _DOFF);

        if (xchar == _DMAX) {
            return (ps->_Sh[_D0] & _DFRAC) != 0 || ps->_Sh[_D1] != 0 || ps->_Sh[_D2] != 0 || ps->_Sh[_D3] != 0
                     ? _NANCODE
                     : _INFCODE;
        } else if (xchar == 0 && 0 < (xchar = _Dnorm(ps))) {
            return 0;
        }

        if (0 < lexp && _DMAX - xchar <= lexp) { // overflow, return +/-INF
            constexpr double inf = _STD numeric_limits<double>::infinity();

            *px = ps->_Sh[_D0] & _DSIGN ? -inf : inf;
            return _INFCODE;
        } else if (-xchar < lexp) { // finite result, repack
            ps->_Sh[_D0] = static_cast<unsigned short>(ps->_Sh[_D0] & ~_DMASK | (lexp + xchar) << _DOFF);
            return _FINITE;
        } else { // denormalized, scale
            unsigned short sign = static_cast<unsigned short>(ps->_Sh[_D0] & _DSIGN);

            ps->_Sh[_D0] = static_cast<unsigned short>(1 << _DOFF | ps->_Sh[_D0] & _DFRAC);
            lexp += xchar - 1;
            if (lexp < -(48 + 1 + _DOFF) || 0 <= lexp) { // certain underflow, return +/-0
                ps->_Sh[_D0] = sign;
                ps->_Sh[_D1] = 0;
                ps->_Sh[_D2] = 0;
                ps->_Sh[_D3] = 0;
                return 0;
            } else { // nonzero, align fraction
                short xexp         = static_cast<short>(lexp);
                unsigned short psx = 0;

                for (; xexp <= -16; xexp += 16) { // scale by words
                    psx          = ps->_Sh[_D3] | (psx != 0 ? 1 : 0);
                    ps->_Sh[_D3] = ps->_Sh[_D2];
                    ps->_Sh[_D2] = ps->_Sh[_D1];
                    ps->_Sh[_D1] = ps->_Sh[_D0];
                    ps->_Sh[_D0] = 0;
                }
                if (xexp != 0) { // scale by bits
                    xexp         = -xexp;
                    psx          = (ps->_Sh[_D3] << (16 - xexp)) | (psx != 0 ? 1 : 0);
                    ps->_Sh[_D3] = static_cast<unsigned short>(ps->_Sh[_D3] >> xexp | ps->_Sh[_D2] << (16 - xexp));
                    ps->_Sh[_D2] = static_cast<unsigned short>(ps->_Sh[_D2] >> xexp | ps->_Sh[_D1] << (16 - xexp));
                    ps->_Sh[_D1] = static_cast<unsigned short>(ps->_Sh[_D1] >> xexp | ps->_Sh[_D0] << (16 - xexp));
                    ps->_Sh[_D0] >>= xexp;
                }

                ps->_Sh[_D0] |= sign;
                if ((0x8000 < psx || 0x8000 == psx && (ps->_Sh[_D3] & 0x0001) != 0) && (++ps->_Sh[_D3] & 0xffff) == 0
                    && (++ps->_Sh[_D2] & 0xffff) == 0 && (++ps->_Sh[_D1] & 0xffff) == 0) {
                    ++ps->_Sh[_D0]; // round up
                } else if (ps->_Sh[_D0] == sign && ps->_Sh[_D1] == 0 && ps->_Sh[_D2] == 0 && ps->_Sh[_D3] == 0) {
                    return 0;
                }

                return _FINITE;
            }
        }
    }
} // unnamed namespace

_EXTERN_C_UNLESS_PURE

_CRTIMP2_PURE short __CLRCALL_PURE_OR_CDECL _Exp(double* px, double y, short eoff) noexcept {
    // compute y * e^(*px), (*px) finite, |y| not huge

    // coefficients
    static constexpr double p[] = {1.0, 420.30235984910635, 15132.70094680474802};
    static constexpr double q[] = {30.01511290683317, 3362.72154416553028, 30265.40189360949691};
    constexpr double c1         = 22713.0 / 32768.0;
    constexpr double c2         = 1.4286068203094172321214581765680755e-6;
    constexpr double hugexp     = static_cast<int>(_DMAX * 900L / 1000);
    constexpr double invln2     = 1.4426950408889634073599246810018921;

    if (y == 0.0) { // zero
        *px = y;
        return 0;
    } else if (*px < -hugexp) { // certain underflow
        *px = _Xfe_underflow(y);
        return 0;
    } else if (hugexp < *px) { // certain overflow
        *px = _Xfe_overflow(y);
        return _INFCODE;
    } else { // xexp won't overflow
        double g   = *px * invln2;
        short xexp = static_cast<short>(g + (g < 0.0 ? -0.5 : +0.5));

        g = xexp;
        g = (*px - g * c1) - g * c2;

        constexpr double eps = 0x1p-54;
        if (-eps < g && g < eps) {
            *px = y;
        } else { // g * g worth computing
            const double z = g * g;
            const double w = (q[0] * z + q[1]) * z + q[2];

            g *= (z + p[1]) * z + p[2];
            *px = (w + g) / (w - g) * 2.0 * y;
            --xexp;
        }

        const short result_code = _Dscale(px, static_cast<long>(xexp) + eoff);

        switch (result_code) {
        case 0:
            *px = _Xfe_underflow(y);
            break;
        case _INFCODE:
            *px = _Xfe_overflow(y);
            break;
        default:
            break;
        }

        return result_code;
    }
}

_CRTIMP2_PURE short __CLRCALL_PURE_OR_CDECL _LExp(long double* px, long double y, short eoff) noexcept {
    return _Exp(reinterpret_cast<double*>(px), static_cast<double>(y), eoff);
}

_CRTIMP2_PURE short __CLRCALL_PURE_OR_CDECL _FExp(float* px, float y, short eoff) noexcept {
    // compute y * e^(*px), (*px) finite, |y| not huge

    static constexpr float p[] = {1.0F, 60.09114349F};
    static constexpr float q[] = {12.01517514F, 120.18228722F};
    constexpr float c1         = (22713.0F / 32768.0F);
    constexpr float c2         = 1.4286068203094172321214581765680755e-6F;
    constexpr float hugexp     = static_cast<int>(_FMAX * 900L / 1000);
    constexpr float invln2     = 1.4426950408889634073599246810018921F;

    if (y == 0.0F) { // zero
        *px = y;
        return 0;
    } else if (*px < -hugexp) { // certain underflow
        *px = _Xfe_underflow(y);
        return 0;
    } else if (hugexp < *px) { // certain overflow
        *px = _Xfe_overflow(y);
        return _INFCODE;
    } else { // xexp won't overflow
        float g    = *px * invln2;
        short xexp = static_cast<short>(g + (g < 0.0F ? -0.5F : +0.5F));

        g = xexp;
        g = static_cast<float>((*px - g * c1) - g * c2);

        constexpr float eps = 0x1p-25f;
        if (-eps < g && g < eps) {
            *px = y;
        } else { // g * g worth computing
            const float z = g * g;
            const float w = q[0] * z + q[1];

            g *= z + p[1];
            *px = (w + g) / (w - g) * 2.0F * y;
            --xexp;
        }

        const short result_code = _FDscale(px, static_cast<long>(xexp) + eoff);

        switch (result_code) {
        case 0:
            *px = _Xfe_underflow(y);
            break;
        case _INFCODE:
            *px = _Xfe_overflow(y);
            break;
        default:
            break;
        }

        return result_code;
    }
}

_END_EXTERN_C_UNLESS_PURE
