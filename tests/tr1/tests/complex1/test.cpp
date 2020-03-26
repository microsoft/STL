// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <complex>, part 1
#define TEST_NAME    "<complex>, part 1"
#define CPP_COMPLEX  1
#define COMPLEX_TYPE IS_DOUBLE
#define FLOAT_TYPE   IS_DOUBLE

#include "tdefs.h"
#include <complex>
#include <sstream>
#include <string>

void test_main() { // test basic workings of complex<double> definitions
    Dbl_complex fc0, fc1(1), fc2(2, 2);

    CHECK(STD real(fc0) == 0 && STD imag(fc0) == 0);
    CHECK(STD real(fc1) == 1 && STD imag(fc1) == 0);
    CHECK(STD real(fc2) == 2 && STD imag(fc2) == 2);
    fc0 += fc2;
    CHECK(STD real(fc0) == 2 && STD imag(fc0) == 2);
    fc0 -= fc1;
    CHECK(STD real(fc0) == 1 && STD imag(fc0) == 2);
    fc0 *= fc2;
    CHECK(STD real(fc0) == -2 && STD imag(fc0) == 6);
    fc0 /= fc2;
    CHECK(STD real(fc0) == 1 && STD imag(fc0) == 2);

    // test arithmetic
    Ldbl_complex lc(-4, -5);
    fc0 = Dbl_complex(lc);
    CHECK(STD real(fc0) == -4 && STD imag(fc0) == -5);
    fc0 = Flt_complex(3, 4);
    fc0 = Ldbl_complex(3, 4);
    fc0 += fc0;
    fc0 = 2.0 + fc2 + 3.0;
    CHECK(STD real(fc0) == 7 && STD imag(fc0) == 2);
    fc0 = 2.0 - fc2 - 3.0;
    CHECK(STD real(fc0) == -3 && STD imag(fc0) == -2);
    fc0 = 2.0 * fc2 * 3.0;
    CHECK(STD real(fc0) == 12 && STD imag(fc0) == 12);
    fc0 = 8.0 / fc2 / 2.0;
    CHECK(STD real(fc0) == 1 && STD imag(fc0) == -1);
    fc0 = +fc1 + -fc2;
    CHECK(STD real(fc0) == -1 && STD imag(fc0) == -2);
    CHECK(fc2 == fc2 && fc1 == 1.0 && 1.0 == fc1);
    CHECK(fc1 != fc2 && fc1 != 0.0 && 3.0 != fc1);

    // test I/O
    STD istringstream istr("(3, -1) (002,  2e1)");
    STD ostringstream ostr;
    istr >> fc0;
    CHECK(STD real(fc0) == 3 && STD imag(fc0) == -1);
    ostr << fc0;
    istr >> fc0;
    CHECK(STD real(fc0) == 2 && STD imag(fc0) == 20);
    ostr << fc0;
    CHECK(ostr.str() == "(3,-1)(2,20)");

    // test math functions
    static const double e      = (double) 2.7182818284590452353602874713526625L;
    static const double ln2    = (double) 0.69314718055994530941723212145817657L;
    static const double piby4  = (double) 0.78539816339744830961566084581987572L;
    static const double rthalf = (double) 0.70710678118654752440084436210484904L;
    const double cosh1x        = rthalf * (e + 1 / e) / 2;
    const double sinh1x        = rthalf * (e - 1 / e) / 2;

    CHECK(approx(STD abs(Dbl_complex(5, -12)), 13));
    CHECK(STD arg(fc1) == 0 && approx(STD arg(fc2), piby4));
    CHECK(STD conj(fc2) == Dbl_complex(2, -2));

    CHECK(STD norm(Dbl_complex(3, -4)) == 25 && STD norm(fc2) == 8);
    CHECK(STD proj(fc2) == fc2);
    CHECK((STD proj(Dbl_complex(-INFINITY, 3))) == (Dbl_complex(INFINITY, 0.0)));
    CHECK(STD real(STD proj(3)) == 3);

    CHECK_DOUBLE(STD real(3), 3.0); // test promotions
    CHECK_DOUBLE(STD imag(3), 0.0);
    CHECK_DOUBLE(STD real(STD conj(3)), 3.0);
    CHECK_DOUBLE(STD abs(4), 4.0);
    CHECK_DOUBLE(STD norm(4), 16.0);
    CHECK_DOUBLE(STD arg(5), 0.0);
    CHECK_DOUBLE(STD real(STD proj(3)), 3.0);

    CHECK_DOUBLE(STD real(STD pow(cmplx(3.0, 0.0), 1)), 3.0);
    CHECK_DOUBLE(STD real(STD pow(2.0, cmplx(2.0, 0.0))), 4.0);

    fc0 = STD cos(Dbl_complex(piby4, -1));
    CHECK(approx(STD real(fc0), cosh1x) && approx(STD imag(fc0), sinh1x));
    fc0 = STD cosh(Dbl_complex(-1, piby4));
    CHECK(approx(STD real(fc0), cosh1x) && approx(STD imag(fc0), -sinh1x));
    fc0 = STD exp(fc1);
    CHECK(approx(STD real(fc0), e) && STD imag(fc0) == 0);
    fc0 = STD exp(Dbl_complex(1, -piby4));
    CHECK(approx(STD real(fc0), e * rthalf) && approx(STD imag(fc0), -e * rthalf));
    fc0 = STD log(Dbl_complex(1, -1));
    CHECK(approx(STD real(fc0), ln2 / 2) && approx(STD imag(fc0), -piby4));
    fc0 = STD polar(1.0, -piby4);
    CHECK(approx(STD real(fc0), rthalf) && approx(STD imag(fc0), -rthalf));
    fc0 = STD pow(fc2, fc2);
    fc0 = STD pow(fc2, 3.0);
    CHECK(approx(STD real(fc0), -16) && approx(STD imag(fc0), 16));
    fc0 = STD pow(2.0, fc2);
    fc0 = STD sin(Dbl_complex(piby4, -1));
    CHECK(approx(STD real(fc0), cosh1x) && approx(STD imag(fc0), -sinh1x));
    fc0 = STD sinh(Dbl_complex(-1, piby4));
    CHECK(approx(STD real(fc0), -sinh1x) && approx(STD imag(fc0), cosh1x));
    fc0 = STD sqrt(Dbl_complex(0, -1));
    CHECK(approx(STD real(fc0), rthalf) && approx(STD imag(fc0), -rthalf));

    fc0 = STD acos(Dbl_complex(cosh1x, sinh1x));
    CHECK(approx(STD real(fc0), piby4));
    fc0 = STD acosh(Dbl_complex(cosh1x, -sinh1x));
    CHECK(approx(STD real(fc0), 1));

    fc0 = STD asin(Dbl_complex(-cosh1x, sinh1x));
    CHECK(approx(STD real(fc0), -piby4));
    fc0 = STD asinh(Dbl_complex(sinh1x, -cosh1x));
    CHECK(approx(STD real(fc0), 1));

    fc0 = STD atan(Dbl_complex(-1, 0));
    CHECK(approx(STD real(fc0), -piby4));
    fc0 = STD atanh(Dbl_complex(0, -1));
    CHECK(approx(STD real(fc0), 0));
}
