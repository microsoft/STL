// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// common extended precision functionality

#include "xmath.h"
#include <string.h>
#if !defined(MRTDLL)
_EXTERN_C
#endif // defined(MRTDLL)

#ifndef ldexpf
#define ldexpf(x, y) ldexp((double) (x), (y))
#endif // ldexp

#ifndef sqrtf
#define sqrtf(x) sqrt((double) (x))
#endif // sqrtf

#define BIG_EXP (2 * FMAXEXP) // very large, as exponents go
#define BITS_WORD (FBITS / 2) // all words same for now
#define NBUF 4 // size of delay line for mulh

#define COPY_UP(j, n)                                       \
    {                                                       \
        int m = j;                                          \
        while (++m < n && (p[m - 1] = p[m]) != FLIT(0.0)) { \
        }                                                   \
        p[n - 1] = FLIT(0.0);                               \
    } // STET

#if 0
#include <stdio.h>

static void printit(const char* s, FTYPE* p, int n) { // print xp array
    int i;
    printf(s);
    for (i = 0; i < n && (p[i] != FLIT(0.0) || i == 0); ++i) {
        printf(" %La", (long double) p[i]);
    }
    printf("\n");
}
#endif // 0

FTYPE FNAME(Xp_getw)(const FTYPE* p, int n) { // get total value
    if (n == 0) {
        return FLIT(0.0);
    } else if (n == 1 || p[0] == FLIT(0.0) || p[1] == FLIT(0.0)) {
        return p[0];
    } else if (n == 2 || p[2] == FLIT(0.0)) {
        return p[0] + p[1];
    } else { // extra bits, ensure proper rounding
        FTYPE p01 = p[0] + p[1];
        FTYPE p2  = p[2];

        if (4 <= n) {
            p2 += p[3]; // pick up sticky bits
        }

        if (p01 - p[0] == p[1]) {
            return p01 + p2; // carry is within p[2], add it in
        } else {
            return p[0] + (p[1] + p2); // fold in p[2] then add it in
        }
    }
}

FTYPE* FNAME(Xp_setw)(FTYPE* p, int n, FTYPE x) { // load a full-precision value
    FTYPE x0 = x;
    short errx, xexp;

    if (n > 0) {
        if (n == 1 || (errx = FNAME(Dunscale)(&xexp, &x0)) == 0) {
            p[0] = x0; // zero or no extra room, store original value
        } else if (0 < errx) { // store Inf or NaN with backstop for safety
            p[0] = x0;
            p[1] = FLIT(0.0);
        } else { // finite, unpack it
            FNAME(Dint)(&x0, BITS_WORD);
            FNAME(Dscale)(&x0, xexp);

            p[0] = x0; // ms bits
            p[1] = x - x0; // ls bits
#pragma warning(suppress : 4127)
            if ((FBITS & 1) != 0 && 2 < n && p[1] != FLIT(0.0)) { // may need a third word
                x = p[1];
                FNAME(Dunscale)(&xexp, &p[1]);
                FNAME(Dint)(&p[1], BITS_WORD);
                FNAME(Dscale)(&p[1], xexp);
                p[2] = x - p[1];
                if (3 < n && p[2] != FLIT(0.0)) {
                    p[3] = FLIT(0.0);
                }
            } else if (2 < n) {
                p[2] = FLIT(0.0);
            }
        }
    }

    return p;
}

FTYPE* FNAME(Xp_addh)(FTYPE* p, int n, FTYPE x0) { // add a half-precision value
    FTYPE xscaled = x0;
    short errx, xexp;

    if (n != 0) {
        if (0 < (errx = FNAME(Dunscale)(&xexp, &xscaled))) {
            if (errx == _NANCODE || (errx = FNAME(Dtest)(&p[0])) <= 0) {
                p[0] = x0; // x0 NaN, or x0 Inf and y finite, just store x0
            } else if (errx != _NANCODE && FISNEG(x0) != FISNEG(p[0])) { // Inf - Inf is invalid
                _Feraise(_FE_INVALID);
                p[0] = FCONST(Nan);
                if (1 < n) {
                    p[1] = FLIT(0.0);
                }
            }
        } else if (errx < 0) { // x0 is finite nonzero, add it
            long prevexp = BIG_EXP;
            int k        = 0;

            while (k < n) { // look for term comparable to xexp to add x0
                FTYPE yscaled = p[k];
                int mybits    = BITS_WORD;
                short yexp;
                long diff;

                if (0 < (errx = FNAME(Dunscale)(&yexp, &yscaled))) {
                    break; // y is Inf or NaN, just leave it alone
                }

                if (errx == 0) { // 0 + x == x
                    p[k] = x0;
                    if (k + 1 < n) {
                        p[k + 1] = FLIT(0.0); // add new trailing zero
                    }

                    break;
                }

                if ((diff = (long) yexp - xexp) <= -mybits
                    && x0 != FLIT(0.0)) { // insert nonzero x0 and loop to renormalize
                    int j = k;

                    do {
                        ++j;
                    } while (j < n && p[j] != FLIT(0.0));

                    if (j < n - 1) {
                        ++j; // extra room, copy trailing zero down too
                    } else if (j == n) {
                        --j; // no room, don't copy smallest word
                    }

                    for (; k < j; --j) {
                        p[j] = p[j - 1]; // copy down words
                    }

                    p[k] = x0;
                    x0   = FLIT(0.0);
                } else if (mybits <= diff && x0 != FLIT(0.0)) { // loop to add finite x0 to smaller words
                    prevexp = yexp;
                    ++k;
                } else { // partition sum and renormalize
                    if ((p[k] += x0) == FLIT(0.0)) { // term sum is zero, copy up words
                        COPY_UP(k, n)
                        if (p[k] == FLIT(0.0)) {
                            break;
                        }
                    }

                    x0 = p[k];
                    FNAME(Dunscale)(&xexp, &x0);
                    if (prevexp - mybits < xexp) { // propagate bits up
                        FNAME(Dint)(&x0, (short) (xexp - (prevexp - mybits)));
                        FNAME(Dscale)(&x0, xexp);
                        if ((p[k] -= x0) == FLIT(0.0)) { // all bits carry, copy up words
                            COPY_UP(k, n)
                        }

                        if (--k == 0) {
                            prevexp = BIG_EXP;
                        } else { // recompute prevexp
                            xscaled = p[k - 1];
                            FNAME(Dunscale)(&yexp, &xscaled);
                            prevexp = yexp;
                        }
                    } else if (k + 1 == n) {
                        break; // don't truncate bits in last word
                    } else { // propagate any excess bits down
                        x0 = p[k];
                        FNAME(Dunscale)(&yexp, &p[k]);
                        FNAME(Dint)(&p[k], BITS_WORD);
                        FNAME(Dscale)(&p[k], yexp);
                        x0 -= p[k];
                        prevexp = yexp;

                        xscaled = x0 != FLIT(0.0) ? x0 : p[k];
                        FNAME(Dunscale)(&xexp, &xscaled);
                        ++k;
                    }
                }
            }
        }
    }

    return p;
}

FTYPE* FNAME(Xp_mulh)(FTYPE* p, int n, FTYPE x0) { // multiply by a half-precision value
    short errx;
    int j, k;
    FTYPE buf[NBUF];

    if (0 < n) { // check for special values
        buf[0] = p[0] * x0;
        if (0 <= (errx = FNAME(Dtest)(&buf[0]))) { // quit early on 0, Inf, or NaN
            if (errx == _NANCODE) {
                _Feraise(_FE_INVALID);
            }

            p[0] = buf[0];
            if (0 < errx && 1 < n) {
                p[1] = FLIT(0.0);
            }

            return p;
        }

        p[0] = FLIT(0.0);
    }

    for (j = 1, k = 0; k < n; ++k, --j) { // sum partial products
        for (; j < NBUF; ++j) {
            if (k + j < n && p[k + j] != FLIT(0.0)) { // copy up a partial product
                buf[j]   = p[k + j] * x0;
                p[k + j] = FLIT(0.0);
            } else { // terminate sequence
                buf[j] = FLIT(0.0);
                j      = 2 * NBUF;
                break;
            }
        }

        if (buf[0] == FLIT(0.0)) {
            break; // input done
        }

        // add in partial product by halves
        int i    = 0;
        FTYPE y0 = buf[0];
        short xexp;

        FNAME(Dunscale)(&xexp, &y0);
        FNAME(Dint)(&y0, BITS_WORD); // clear low half bits
        FNAME(Dscale)(&y0, xexp);
        FNAME(Xp_addh)(p, n, y0); // add in ms part
        FNAME(Xp_addh)(p, n, buf[0] - y0); // add in ls part

        while (++i < j) {
            if ((buf[i - 1] = buf[i]) == FLIT(0.0)) {
                break; // copy down delay line
            }
        }
    }

    return p;
}

FTYPE* FNAME(Xp_setn)(FTYPE* p, int n, long x) { // load a long integer

#if 27 <= FBITS
    FNAME(Xp_setw)(p, n, (FTYPE) x);

#else // 27 <= FBITS
    FNAME(Xp_setw)(p, n, (FTYPE)(x / 10000));
    FNAME(Xp_mulh)(p, n, (FTYPE) 10000);
    FNAME(Xp_addh)(p, n, (FTYPE)(x % 10000));
#endif // 27 <= FBITS

    return p;
}

FTYPE* FNAME(Xp_movx)(FTYPE* p, int n, const FTYPE* q) { // copy an extended precision value
    memcpy(p, q, n * sizeof(FTYPE));
    return p;
}

FTYPE* FNAME(Xp_addx)(FTYPE* p, int n, const FTYPE* q, int m) { // add an extended precision value
    int k;

    for (k = 0; k < m && q[k] != FLIT(0.0); ++k) {
        FNAME(Xp_addh)(p, n, q[k]);
    }

    return p;
}

FTYPE* FNAME(Xp_subx)(FTYPE* p, int n, const FTYPE* q, int m) { // subtract an extended precision value
    int k;

    for (k = 0; k < m && q[k] != FLIT(0.0); ++k) {
        FNAME(Xp_addh)(p, n, -q[k]);
    }

    return p;
}

FTYPE* FNAME(Xp_ldexpx)(FTYPE* p, int n, int m) { // scale an extended precision value
    int k;
    for (k = 0; k < n; ++k) {
        p[k] = static_cast<FTYPE>(FFUN(ldexp)(p[k], m));
        if (p[k] == FLIT(0.0)) {
            break;
        }
    }

    return p;
}

FTYPE* FNAME(Xp_mulx)(FTYPE* p, int n, const FTYPE* q, int m, FTYPE* ptemp2) {
    // multiply by an extended precision value (needs 2 * n temp)
    if (n != 0 && m != 0) {
        if (q[0] == FLIT(0.0) || q[1] == FLIT(0.0)) {
            FNAME(Xp_mulh)(p, n, q[0]);
        } else { // sum partial products
            FTYPE* px  = ptemp2;
            FTYPE* pac = ptemp2 + n;
            int j;

            FNAME(Xp_movx)(px, n, p);
            FNAME(Xp_mulh)(p, n, q[0]); // form first partial product in place
            for (j = 1; j < m && q[j] != FLIT(0.0); ++j) { // add in a partial product
                FNAME(Xp_movx)(pac, n, px);
                FNAME(Xp_mulh)(pac, n, q[j]);
                FNAME(Xp_addx)(p, n, pac, n);
            }
        }
    }
    return p;
}

FTYPE* FNAME(Xp_invx)(FTYPE* p, int n, FTYPE* ptemp4) { // invert an extended precision value (needs 4 * n temp)
    short errx;

    if (n != 0) {
        if (0 <= (errx = FNAME(Dtest)(&p[0]))) { // not finite, return special value
            if (errx == _INFCODE) {
                p[0] = FLIT(0.0); // 1/Inf == 0
            } else if (errx == 0) {
                p[0] = FCONST(Inf); // 1/0 == Inf
            }
            // else 1/NaN == NaN
        } else { // p[0] is finite nonzero, invert it
            FTYPE* pac    = ptemp4;
            FTYPE* py     = ptemp4 + n;
            FTYPE* ptemp2 = py + n;
            FTYPE x0      = p[0];
            int k;

            FNAME(Xp_movx)(py, n, p);
            FNAME(Xp_mulh)(py, n, -FLIT(1.0)); // py = -x

            if (1 < n) {
                x0 += p[1];
            }

            FNAME(Xp_setw)(p, n, FLIT(1.0) / x0); // p = y
            for (k = 1; k < n; k <<= 1) { // iterate to double previous precision of 1/x
                FNAME(Xp_movx)(pac, n, p);
                FNAME(Xp_mulx)(pac, n, py, n, ptemp2);
                FNAME(Xp_addh)(pac, n, FLIT(1.0)); // 1 - x * y
                FNAME(Xp_mulx)(pac, n, p, n, ptemp2); // y * (1 - x * y)
                FNAME(Xp_addx)(p, n, pac, n); // y += y * (1 - x * y)
            }
        }
    }

    return p;
}

FTYPE* FNAME(Xp_sqrtx)(FTYPE* p, int n, FTYPE* ptemp4) {
    // find square root of an extended precision value (needs 4 * n temp)
    if (n != 0) {
        if (0 <= FNAME(Dtest)(&p[0]) || p[0] < FLIT(0.0)) { // not finite nonnegative, return special value
            if (p[0] < FLIT(0.0)) { // sqrt(negative), report domain error
                _Feraise(_FE_INVALID);
                p[0] = FCONST(Nan);
            }
        } else { // worth iterating, compute x * sqrt(1/x)
            FTYPE* pac    = ptemp4;
            FTYPE* py     = ptemp4 + n;
            FTYPE* ptemp2 = py + n;
            FTYPE x0      = p[0];
            int k;

            if (1 < n) {
                x0 += p[1];
            }

            FNAME(Xp_setw)(py, n, static_cast<FTYPE>(FLIT(1.0) / FFUN(sqrt)(x0))); // py = y
            for (k = 2; k < n; k <<= 1) { // iterate to double previous precision of sqrt(1/x)
                FNAME(Xp_movx)(pac, n, py);
                FNAME(Xp_mulh)(pac, n, -FLIT(0.5));
                FNAME(Xp_mulx)(pac, n, p, n, ptemp2);
                FNAME(Xp_mulx)(pac, n, py, n, ptemp2);
                FNAME(Xp_addh)(pac, n, FLIT(1.5)); // 3/2 - x * y * y / 2
                FNAME(Xp_mulx)(py, n, pac, n, ptemp2); // y *= 3/2 - x * y * y / 2
            }

            FNAME(Xp_mulx)(p, n, py, n, ptemp2); // x * sqrt(1/x)
        }
    }

    return p;
}
#if !defined(MRTDLL)
_END_EXTERN_C
#endif // !defined(MRTDLL)
