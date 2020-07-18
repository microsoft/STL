// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cmath>
#include <complex>
#include <limits>

using namespace std;

template <typename T>
void check(const T x, const T y) {
    constexpr T tolerance = 2 * numeric_limits<T>::epsilon();

    if (isfinite(x) && isfinite(y)) {
        assert(signbit(x) == signbit(y));
        assert(abs(x - y) <= tolerance * fmax(abs(x), abs(y)));
    } else if (isnan(x) || isnan(y)) {
        assert(isnan(x) == isnan(y));
    } else {
        assert(x == y);
    }
}

template <typename T>
void check_part(const complex<T>& x, const complex<T>& y) {
    check(x.real(), y.real());
    check(x.imag(), y.imag());
}

template <typename T>
void check_norm(const complex<T>& x, const complex<T>& y) {
    constexpr T tolerance = 4 * numeric_limits<T>::epsilon();

    const bool x_is_finite = isfinite(x.real()) && isfinite(x.imag());
    const bool y_is_finite = isfinite(y.real()) && isfinite(y.imag());
    assert(x_is_finite == y_is_finite);

    if (x_is_finite) {
        assert(abs(x - y) <= tolerance * fmax(abs(x), abs(y)));
    } else {
        const bool x_is_inf = isinf(x.real()) || isinf(x.imag());
        const bool y_is_inf = isinf(y.real()) || isinf(y.imag());
        assert(x_is_inf == y_is_inf);
    }
}

template <typename T>
void test() {
    using CT = complex<T>;

    constexpr T zero     = T{0};
    constexpr T tiny     = numeric_limits<T>::denorm_min();
    constexpr T log10_e  = T{0.4342944819032518};
    constexpr T half     = T{0.5};
    constexpr T one_down = T{1} - numeric_limits<T>::epsilon() / T{numeric_limits<T>::radix};
    constexpr T one      = T{1};
    constexpr T one_up   = T{1} + numeric_limits<T>::epsilon();
    constexpr T pi_2     = T{1.5707963267948966};
    constexpr T two      = T{2};
    constexpr T pi       = T{3.141592653589793};
    constexpr T huge     = (numeric_limits<T>::max)();
    constexpr T inf      = numeric_limits<T>::infinity();

    // arg
    for (const T x : {zero, tiny, half, one, two, huge, inf}) {
        check(arg(CT{-x, +zero}), +pi);
        check(arg(CT{-x, -zero}), -pi);
    }

    // log
    for (const T x : {zero, tiny, half, one, two, huge, inf}) {
        check_part(log(CT{-x, +zero}), CT{log(x), +pi});
        check_part(log(CT{-x, -zero}), CT{log(x), -pi});
    }

    // log10
    for (const T x : {zero, tiny, half, one, two, huge, inf}) {
        check_part(log10(CT{-x, +zero}), CT{log10(x), +pi * log10_e});
        check_part(log10(CT{-x, -zero}), CT{log10(x), -pi * log10_e});
    }

    // sqrt
    for (const T x : {zero, tiny, half, one, two, huge, inf}) {
        check_part(sqrt(CT{-x, +zero}), CT{zero, +sqrt(x)});
        check_part(sqrt(CT{-x, -zero}), CT{zero, -sqrt(x)});
    }

    // pow
    for (const T x : {half, one, two}) {
        for (const T yr : {zero, half, one, two}) {
            for (const T yi : {zero, half, one, two}) {
                check_norm(pow(CT{-x, +zero}, CT{+yr, +yi}), exp(CT{log(x), +pi} * CT{+yr, +yi}));
                check_norm(pow(CT{-x, -zero}, CT{+yr, +yi}), exp(CT{log(x), -pi} * CT{+yr, +yi}));

                check_norm(pow(CT{-x, +zero}, CT{+yr, -yi}), exp(CT{log(x), +pi} * CT{+yr, -yi}));
                check_norm(pow(CT{-x, -zero}, CT{+yr, -yi}), exp(CT{log(x), -pi} * CT{+yr, -yi}));

                check_norm(pow(CT{-x, +zero}, CT{-yr, +yi}), exp(CT{log(x), +pi} * CT{-yr, +yi}));
                check_norm(pow(CT{-x, -zero}, CT{-yr, +yi}), exp(CT{log(x), -pi} * CT{-yr, +yi}));

                check_norm(pow(CT{-x, +zero}, CT{-yr, -yi}), exp(CT{log(x), +pi} * CT{-yr, -yi}));
                check_norm(pow(CT{-x, -zero}, CT{-yr, -yi}), exp(CT{log(x), -pi} * CT{-yr, -yi}));
            }
        }
    }

    // acos
    for (const T x : {one, one_up, two, huge, inf}) {
        check_part(acos(CT{+x, +zero}), CT{zero, -acosh(x)});
        check_part(acos(CT{+x, -zero}), CT{zero, +acosh(x)});

        check_part(acos(CT{-x, +zero}), CT{pi, -acosh(x)});
        check_part(acos(CT{-x, -zero}), CT{pi, +acosh(x)});
    }

    // asin
    for (const T x : {one, one_up, two, huge, inf}) {
        check_part(asin(CT{+x, +zero}), CT{+pi_2, +acosh(x)});
        check_part(asin(CT{+x, -zero}), CT{+pi_2, -acosh(x)});

        check_part(asin(CT{-x, +zero}), CT{-pi_2, +acosh(x)});
        check_part(asin(CT{-x, -zero}), CT{-pi_2, -acosh(x)});
    }

    // atan
    for (const T x : {one_up, two, huge, inf}) {
        check_part(atan(CT{+zero, +x}), CT{+pi_2, atanh(one / +x)});
        check_part(atan(CT{-zero, +x}), CT{-pi_2, atanh(one / +x)});

        check_part(atan(CT{+zero, -x}), CT{+pi_2, atanh(one / -x)});
        check_part(atan(CT{-zero, -x}), CT{-pi_2, atanh(one / -x)});
    }

    // acosh
    for (const T x : {one, one_down, half, tiny, zero}) {
        check_part(acosh(CT{+x, +zero}), CT{zero, +acos(+x)});
        check_part(acosh(CT{+x, -zero}), CT{zero, -acos(+x)});

        check_part(acosh(CT{-x, +zero}), CT{zero, +acos(-x)});
        check_part(acosh(CT{-x, -zero}), CT{zero, -acos(-x)});
    }

    for (const T x : {one, one_up, two, huge, inf}) {
        check_part(acosh(CT{-x, +zero}), CT{acosh(x), +pi});
        check_part(acosh(CT{-x, -zero}), CT{acosh(x), -pi});
    }

    // asinh
    for (const T x : {one, one_up, two, huge, inf}) {
        check_part(asinh(CT{+zero, +x}), CT{+acosh(x), +pi_2});
        check_part(asinh(CT{-zero, +x}), CT{-acosh(x), +pi_2});

        check_part(asinh(CT{+zero, -x}), CT{+acosh(x), -pi_2});
        check_part(asinh(CT{-zero, -x}), CT{-acosh(x), -pi_2});
    }

    // atanh
    for (const T x : {one_up, two, huge, inf}) {
        check_part(atanh(CT{+x, +zero}), CT{atanh(one / +x), +pi_2});
        check_part(atanh(CT{+x, -zero}), CT{atanh(one / +x), -pi_2});

        check_part(atanh(CT{-x, +zero}), CT{atanh(one / -x), +pi_2});
        check_part(atanh(CT{-x, -zero}), CT{atanh(one / -x), -pi_2});
    }
}

int main() {
    test<double>();
    test<float>();
    test<long double>();
}
