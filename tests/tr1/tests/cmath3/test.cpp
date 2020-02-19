// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <cmath>, part 3
#define TEST_NAME  "<cmath>, part 3"
#define FLOAT_TYPE IS_LDOUBLE

#include "tdefs.h"
#include <cmath>

static const long double e      = 2.7182818284590452353602874713526625L;
static const long double ln2    = 0.69314718055994530941723212145817657L;
static const long double pi3by4 = 2.35619449019234492884698253745962716L;
static const long double piby4  = 0.78539816339744830961566084581987572L;
static const long double rthalf = 0.70710678118654752440084436210484904L;

#define STDx STD

void test_cpp() { // test C++ header
    long double x;
    int xexp;

    CHECK(STDx ceill(5.1L) == 6.0L);
    CHECK(STDx fabsl(-5.0L) == 5.0L);
    CHECK(STDx floorl(5.1L) == 5.0L);
    CHECK(STDx fmodl(-7.0L, 3.0L) == -1.0L);
    CHECK(approx(STDx frexpl(96.0L, &xexp), 0.75L) && xexp == 7);
    CHECK(STDx ldexpl(-3.0L, 4) == -48.0L);
    CHECK(approx(STDx modfl(-11.7L, &x), -11.7L + 11.0L) && x == -11.0L);

    CHECK(approx(STDx acosl(-rthalf), pi3by4));
    CHECK(approx(STDx asinl(-rthalf), -piby4));
    CHECK(approx(STDx atanl(-1.0L), -piby4));
    CHECK(approx(STDx atan2l(-1.0L, 0.0L), -2.0L * piby4));
    CHECK(approx(STDx cosl(-piby4), rthalf));
    CHECK(approx(STDx sinl(-pi3by4), -rthalf));
    CHECK(approx(STDx tanl(-piby4), -1.0L));

    CHECK(approx(STDx coshl(-1.0L), (e + 1.0L / e) / 2.0L));
    CHECK(approx(STDx expl(ln2), 2.0L));
    CHECK(approx(STDx logl(e * e * e), 3.0L));
    CHECK(approx(STDx log10l(1e5L), 5.0L));
    CHECK(approx(STDx powl(2.0L, -0.5L), rthalf));
    CHECK(approx(STDx sinhl(-1.0L), -(e - 1.0L / e) / 2.0L));
    CHECK(approx(STDx sqrtl(2.0L), 1.0L / rthalf));
    CHECK(approx(STDx tanhl(-1.0L), -(e * e - 1.0L) / (e * e + 1.0L)));

    long double (*pabs)(long double) = &STDx abs; // C++ overloads
    CHECK((*pabs)(-rthalf) == rthalf);

    long double (*pceil)(long double) = &STDx ceil;
    CHECK((*pceil)(5.1L) == 6.0L);
    long double (*pfabs)(long double) = &STDx fabs;
    CHECK((*pfabs)(-5.0L) == 5.0L);
    long double (*pfloor)(long double) = &STDx floor;
    CHECK((*pfloor)(5.1L) == 5.0L);
    long double (*pfmod)(long double, long double) = &STDx fmod;
    CHECK((*pfmod)(-7.0L, 3.0L) == -1.0L);
    long double (*pfrexp)(long double, int*) = &STDx frexp;
    CHECK(approx((*pfrexp)(96.0L, &xexp), 0.75L) && xexp == 7);
    long double (*pldexp)(long double, int) = &STDx ldexp;
    CHECK((*pldexp)(-3.0L, 4) == -48.0L);
    long double (*pmodf)(long double, long double*) = &STDx modf;
    CHECK(approx((*pmodf)(-11.7L, &x), -11.7L + 11.0L) && x == -11.0L);

    long double (*pacos)(long double) = &STDx acos;
    CHECK(approx((*pacos)(-rthalf), pi3by4));
    long double (*pasin)(long double) = &STDx asin;
    CHECK(approx((*pasin)(-rthalf), -piby4));
    long double (*patan)(long double) = &STDx atan;
    CHECK(approx((*patan)(-1.0L), -piby4));
    long double (*patan2)(long double, long double) = &STDx atan2;
    CHECK(approx((*patan2)(-1.0L, 0.0L), -2.0L * piby4));
    long double (*pcos)(long double) = &STDx cos;
    CHECK(approx((*pcos)(-piby4), rthalf));
    long double (*psin)(long double) = &STDx sin;
    CHECK(approx((*psin)(-pi3by4), -rthalf));
    long double (*ptan)(long double) = &STDx tan;
    CHECK(approx((*ptan)(-piby4), -1.0L));

    long double (*pcosh)(long double) = &STDx cosh;
    CHECK(approx((*pcosh)(-1.0L), (e + 1.0L / e) / 2.0L));
    long double (*pexp)(long double) = &STDx exp;
    CHECK(approx((*pexp)(ln2), 2.0L));
    long double (*plog)(long double) = &STDx log;
    CHECK(approx((*plog)(e * e * e), 3.0L));
    long double (*plog10)(long double) = &STDx log10;
    CHECK(approx((*plog10)(1e5), 5.0L));
    long double (*ppow)(long double, long double) = &STDx pow;
    CHECK(approx((*ppow)(2.0L, -0.5L), rthalf));
    long double (*psinh)(long double) = &STDx sinh;
    CHECK(approx((*psinh)(-1.0L), -(e - 1.0L / e) / 2.0L));
    long double (*psqrt)(long double) = &STDx sqrt;
    CHECK(approx((*psqrt)(2.0L), 1.0L / rthalf));
    long double (*ptanh)(long double) = &STDx tanh;
    CHECK(approx((*ptanh)(-1.0L), -(e * e - 1.0L) / (e * e + 1.0L)));
}

void test_main() { // test basic workings of cmath definitions
    test_cpp();
}
