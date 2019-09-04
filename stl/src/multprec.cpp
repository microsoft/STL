// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// implements multiprecision math for random number generators

#include <limits>
#include <random>

_STD_BEGIN
constexpr int shift                 = _STD numeric_limits<unsigned long long>::digits / 2;
constexpr unsigned long long mask   = ~(~0ULL << shift);
constexpr unsigned long long maxVal = mask + 1;

_NODISCARD unsigned long long __CLRCALL_PURE_OR_CDECL _MP_Get(
    _MP_arr u) noexcept { // convert multi-word value to scalar value
    return (u[1] << shift) + u[0];
}

static void add(unsigned long long* u, int ulen, unsigned long long* v,
    int vlen) noexcept { // add multi-word value to multi-word value
    int i;
    unsigned long long k = 0;
    for (i = 0; i < vlen; ++i) { // add multi-word values
        u[i] += v[i] + k;
        k = u[i] >> shift;
        u[i] &= mask;
    }
    for (; k != 0 && i < ulen; ++i) { // propagate carry
        u[i] += k;
        k = u[i] >> shift;
        u[i] &= mask;
    }
}

void __CLRCALL_PURE_OR_CDECL _MP_Add(
    _MP_arr u, unsigned long long v0) noexcept { // add scalar value to multi-word value
    unsigned long long v[2];
    v[0] = v0 & mask;
    v[1] = v0 >> shift;
    add(u, _MP_len, v, 2);
}

static void mul(
    unsigned long long* u, int ulen, unsigned long long v0) noexcept { // multiply multi-word value by single-word value
    unsigned long long k = 0;
    for (int i = 0; i < ulen; ++i) { // multiply and propagate carry
        u[i] = u[i] * v0 + k;
        k    = u[i] >> shift;
        u[i] &= mask;
    }
}

void __CLRCALL_PURE_OR_CDECL _MP_Mul(
    _MP_arr w, unsigned long long u0, unsigned long long v0) noexcept { // multiply multi-word value by multi-word value
    constexpr int m = 2;
    constexpr int n = 2;
    unsigned long long u[2];
    unsigned long long v[2];
    u[0] = u0 & mask;
    u[1] = u0 >> shift;
    v[0] = v0 & mask;
    v[1] = v0 >> shift;

    // Knuth, vol. 2, p. 268, Algorithm M
    // M1: [Initialize.]
    for (int i = 0; i < m + n + 1; ++i) {
        w[i] = 0;
    }

    for (int j = 0; j < n; ++j) { // M2: [Zero multiplier?]
        if (v[j] == 0) {
            w[j + m] = 0;
        } else { // multiply by non-zero value
            unsigned long long k = 0;
            int i;
            // M3: [Initialize i.]
            for (i = 0; i < m; ++i) { // M4: [Multiply and add.]
                w[i + j] = u[i] * v[j] + w[i + j] + k;
                k        = w[i + j] >> shift;
                w[i + j] &= mask;
                // M5: [Loop on i.]
            }
            w[i + j] = k;
        }
        // M6: [Loop on j.]
    }
}

static void div(_MP_arr u,
    unsigned long long
        v0) noexcept { // divide multi-word value by scalar value (fits in lower half of unsigned long long)
    unsigned long long k = 0;
    int ulen             = _MP_len;
    while (0 <= --ulen) { // propagate remainder and divide
        unsigned long long tmp = (k << shift) + u[ulen];
        u[ulen]                = tmp / v0;
        k                      = tmp % v0;
    }
}

_NODISCARD static int limit(const unsigned long long* u, int ulen) noexcept { // get index of last non-zero value
    while (u[ulen - 1] == 0) {
        --ulen;
    }

    return ulen;
}

void __CLRCALL_PURE_OR_CDECL _MP_Rem(
    _MP_arr u, unsigned long long v0) noexcept { // divide multi-word value by value, leaving remainder in u
    unsigned long long v[2];
    v[0]        = v0 & mask;
    v[1]        = v0 >> shift;
    const int n = limit(v, 2);
    const int m = limit(u, _MP_len) - n;

    // Knuth, vol. 2, p. 272, Algorithm D
    // D1: [Normalize.]
    unsigned long long d = maxVal / (v[n - 1] + 1);
    if (d != 1) { // scale numerator and divisor
        mul(u, _MP_len, d);
        mul(v, n, d);
    }
    // D2: [Initialize j.]
    for (int j = m; 0 <= j; --j) { // D3: [Calculate qh.]
        unsigned long long qh = ((u[j + n] << shift) + u[j + n - 1]) / v[n - 1];
        if (qh == 0) {
            continue;
        }

        unsigned long long rh = ((u[j + n] << shift) + u[j + n - 1]) % v[n - 1];
        for (;;) {
            if (qh < maxVal && qh * v[n - 2] <= (rh << shift) + u[j + n - 2]) {
                break;
            } else { // reduce tentative value and retry
                --qh;
                rh += v[n - 1];
                if (maxVal <= rh) {
                    break;
                }
            }
        }

        // D4: [Multiply and subtract.]
        unsigned long long k = 0;
        int i;
        for (i = 0; i < n; ++i) { // multiply and subtract
            u[j + i] -= qh * v[i] + k;
            k = u[j + i] >> shift;
            if (k) {
                k = maxVal - k;
            }

            u[j + i] &= mask;
        }
        for (; k != 0 && j + i < _MP_len; ++i) { // propagate borrow
            u[j + i] -= k;
            k = u[j + i] >> shift;
            if (k) {
                k = maxVal - k;
            }

            u[j + i] &= mask;
        }
        // D5: [Test remainder.]
        if (k != 0) { // D6: [Add back.]
            --qh;
            add(u + j, n + 1, v, n);
        }
        // D7: [Loop on j.]
    }
    // D8: [Unnormalize.]
    if (d != 1) {
        div(u, d);
    }
}
_STD_END
