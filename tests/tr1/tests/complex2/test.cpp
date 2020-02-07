// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <complex>, part 2
#define TEST_NAME    "<complex>, part 2"
#define CPP_COMPLEX  1
#define COMPLEX_TYPE IS_FLOAT
#define FLOAT_TYPE   IS_FLOAT

#include "tdefs.h"
#include <complex>
#include <sstream>
#include <string>

void test_main() { // test basic workings of complex<float> definitions
    Flt_complex fc0, fc1(1), fc2(2, 2);

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
    Dbl_complex dc(2, 3);
    fc0 = Flt_complex(dc);
    CHECK(STD real(fc0) == 2 && STD imag(fc0) == 3);
    Ldbl_complex lc(-4, -5);
    fc0 = Flt_complex(lc);
    CHECK(STD real(fc0) == -4 && STD imag(fc0) == -5);
    fc0 = Dbl_complex(3, 4);
    fc0 = Ldbl_complex(3, 4);
    fc0 += fc0;
    fc0 = 2.0F + fc2 + 3.0F;
    CHECK(STD real(fc0) == 7 && STD imag(fc0) == 2);
    fc0 = 2.0F - fc2 - 3.0F;
    CHECK(STD real(fc0) == -3 && STD imag(fc0) == -2);
    fc0 = 2.0F * fc2 * 3.0F;
    CHECK(STD real(fc0) == 12 && STD imag(fc0) == 12);
    fc0 = 8.0F / fc2 / 2.0F;
    CHECK(STD real(fc0) == 1 && STD imag(fc0) == -1);
    fc0 = +fc1 + -fc2;
    CHECK(STD real(fc0) == -1 && STD imag(fc0) == -2);
    CHECK(fc2 == fc2 && fc1 == 1.0F && 1.0F == fc1);
    CHECK(fc1 != fc2 && fc1 != 0.0F && 3.0F != fc1);

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
    static const float e      = (float) 2.7182818284590452353602874713526625L;
    static const float ln2    = (float) 0.69314718055994530941723212145817657L;
    static const float piby4  = (float) 0.78539816339744830961566084581987572L;
    static const float rthalf = (float) 0.70710678118654752440084436210484904L;
    const float cosh1x        = (float) (rthalf * (e + 1 / e) / 2);
    const float sinh1x        = (float) (rthalf * (e - 1 / e) / 2);

    CHECK(approx(STD abs(Flt_complex(5, -12)), 13));
    CHECK(STD arg(fc1) == 0 && approx(STD arg(fc2), piby4));
    CHECK(STD conj(fc2) == Flt_complex(2, -2));

    CHECK(STD norm(Flt_complex(3, -4)) == 25 && STD norm(fc2) == 8);
    CHECK(STD proj(fc2) == fc2);
    CHECK((STD proj(Flt_complex(-INFINITY, 3))) == (Flt_complex(INFINITY, 0.0)));
    CHECK(STD real(STD proj(3.0F)) == 3.0F);

    fc0 = STD cos(Flt_complex(piby4, -1));
    CHECK(approx(STD real(fc0), cosh1x) && approx(STD imag(fc0), sinh1x));
    fc0 = STD cosh(Flt_complex(-1, piby4));
    CHECK(approx(STD real(fc0), cosh1x) && approx(STD imag(fc0), -sinh1x));
    fc0 = STD exp(fc1);
    CHECK(approx(STD real(fc0), e) && STD imag(fc0) == 0);
    fc0 = STD exp(Flt_complex(1, -piby4));
    CHECK(approx(STD real(fc0), e * rthalf) && approx(STD imag(fc0), -e * rthalf));
    fc0 = STD log(Flt_complex(1, -1));
    CHECK(approx(STD real(fc0), ln2 / 2) && approx(STD imag(fc0), -piby4));
    CHECK(STD norm(Flt_complex(3, -4)) == 25 && STD norm(fc2) == 8);
    fc0 = STD polar(1.0F, -piby4);
    CHECK(approx(STD real(fc0), rthalf) && approx(STD imag(fc0), -rthalf));
    fc0 = STD pow(fc2, fc2);
    fc0 = STD pow(fc2, 3.0F);
    CHECK(approx(STD real(fc0), -16) && approx(STD imag(fc0), 16));
    fc0 = STD pow(2.0F, fc2);
    fc0 = STD sin(Flt_complex(piby4, -1));
    CHECK(approx(STD real(fc0), cosh1x) && approx(STD imag(fc0), -sinh1x));
    fc0 = STD sinh(Flt_complex(-1, piby4));
    CHECK(approx(STD real(fc0), -sinh1x) && approx(STD imag(fc0), cosh1x));
    fc0 = STD sqrt(Flt_complex(0, -1));
    CHECK(approx(STD real(fc0), rthalf) && approx(STD imag(fc0), -rthalf));

    fc0 = STD acos(Flt_complex(cosh1x, sinh1x));
    CHECK(approx(STD real(fc0), piby4));
    fc0 = STD acosh(Flt_complex(cosh1x, -sinh1x));
    CHECK(approx(STD real(fc0), 1));

    fc0 = STD asin(Flt_complex(-cosh1x, sinh1x));
    CHECK(approx(STD real(fc0), -piby4));
    fc0 = STD asinh(Flt_complex(sinh1x, -cosh1x));
    CHECK(approx(STD real(fc0), 1));

    fc0 = STD atan(Flt_complex(-1, 0));
    CHECK(approx(STD real(fc0), -piby4));
    fc0 = STD atanh(Flt_complex(0, -1));
    CHECK(approx(STD real(fc0), 0));
}
