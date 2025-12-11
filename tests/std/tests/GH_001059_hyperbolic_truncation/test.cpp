// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// GH-1059: "Incorrect const float values in xvalues.cpp files"
// Internal constants (used to determine when the exp(-x) terms in
// cosh and sinh could be ignored) had off-by-one and integer
// truncation errors.

#include <array>
#include <cassert>
#include <complex>
#include <cstdio>
using namespace std;

template <class T>
struct TestCase {
    T val;
    T cosh_expected;
    T sinh_expected;
};

template <class T>
void test(const TestCase<T>& tc) {
    const complex<T> z{tc.val};

    const T cosh_calc = real(cosh(z));
    if (cosh_calc != tc.cosh_expected) {
        printf("tc.val: %a (%.1000g); cosh_calc: %a (%.1000g); tc.cosh_expected: %a (%.1000g)\n", tc.val, tc.val,
            cosh_calc, cosh_calc, tc.cosh_expected, tc.cosh_expected);
        assert(cosh_calc == tc.cosh_expected);
    }

    const T sinh_calc = real(sinh(z));
    if (sinh_calc != tc.sinh_expected) {
        printf("tc.val: %a (%.1000g); sinh_calc: %a (%.1000g); tc.sinh_expected: %a (%.1000g)\n", tc.val, tc.val,
            sinh_calc, sinh_calc, tc.sinh_expected, tc.sinh_expected);
        assert(sinh_calc == tc.sinh_expected);
    }
}

int main() {
    // Precise values from Wolfram Alpha, rounded to float and double.
    constexpr array<TestCase<float>, 3> fTests{{
        {0x1p+3f, 0x1.749eaap+10f, 0x1.749ea6p+10f},
        {0x1.0a7efap+3f, 0x1.02a222p+11f, 0x1.02a22p+11f},
        {0x1.15999ap+3f, 0x1.6deb38p+11f, 0x1.6deb36p+11f},
    }};

    constexpr array<TestCase<double>, 3> dTests{{
        {0x1.2p+4, 0x1.f4f22091940bfp+24, 0x1.f4f22091940bbp+24},
        {
            0x1.264189374bc6ap+4, 0x1.7250551723516p+25,
#ifdef _M_IX86 // TRANSITION, accuracy issue
            0x1.7250551723515p+25 // Wolfram Alpha
#else // ^^^ no workaround / workaround vvv
            0x1.7250551723514p+25 // observed behavior
#endif // ^^^ workaround ^^^
        },
        {0x1.2bced916872bp+4, 0x1.05f68c44177a1p+26, 0x1.05f68c44177ap+26},
    }};

    for (const auto& tc : fTests) {
        test(tc);
    }

    for (const auto& tc : dTests) {
        test(tc);
    }
}
