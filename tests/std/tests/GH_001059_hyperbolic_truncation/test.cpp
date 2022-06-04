// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// GH-1059: "Incorrect const float values in xvalues.cpp files"
// The _Xbig constants, used to determine when the exp(-x) terms in
// cosh and sinh can be ignored, have off-by-one and integer
// truncation errors.

#include <array>
#include <cassert>
#include <cmath>
#include <complex>
#include <iostream>
#include <limits>

using namespace std;

template <class T>
void Test(T x) {
    const complex<T> z{x};
    const T exp_over_2 = T{0.5} * real(exp(z));

    const T cosh_expected = exp_over_2 + T{0.25} / exp_over_2;
    const T cosh_calc     = real(cosh(z));
    if (cosh_expected != cosh_calc) {
        cout.precision(numeric_limits<T>::digits10 + 2);
        cout << "x = " << x << '\n'
             << "cosh (expected)   = " << cosh_expected << '\n'
             << "cosh (calculated) = " << cosh_calc << endl;
        assert(cosh_expected == cosh_calc);
    }

    const T sinh_expected = exp_over_2 - T{0.25} / exp_over_2;
    const T sinh_calc     = real(sinh(z));
    if (sinh_expected != sinh_calc) {
        cout.precision(numeric_limits<T>::digits10 + 2);
        cout << "x = " << x << '\n'
             << "sinh (expected)   = " << sinh_expected << '\n'
             << "sinh (calculated) = " << sinh_calc << endl;
        assert(sinh_expected == sinh_calc);
    }
}

template <class T>
constexpr array<T, 3> GenerateValues() {
    // {old crossover, difference ~ 1 ulp, difference ~ ulp/2}
    constexpr int DIG = numeric_limits<T>::digits;
    return {DIG * 347L / 1000L, DIG * static_cast<T>(0.347), (DIG + 1) * static_cast<T>(0.347)};
}

int main() {
    constexpr auto fValues{GenerateValues<float>()};
    for (const auto& x : fValues) {
        Test<float>(x);
    }

    constexpr auto dValues{GenerateValues<double>()};
    for (const auto& x : dValues) {
        Test<double>(x);
    }

    return 0;
}
