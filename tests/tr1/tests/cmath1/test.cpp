// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <cmath>, part 1
#define TEST_NAME  "<cmath>, part 1"
#define FLOAT_TYPE IS_DOUBLE

#include "tdefs.h"
#include <cmath>

static const double e      = (double) 2.7182818284590452353602874713526625L;
static const double ln2    = (double) 0.69314718055994530941723212145817657L;
static const double pi3by4 = (double) 2.35619449019234492884698253745962716L;
static const double piby4  = (double) 0.78539816339744830961566084581987572L;
static const double rthalf = (double) 0.70710678118654752440084436210484904L;

#define STDx STD

void test_cpp() { // test C++ header
    double x;
    int xexp;

    x = HUGE_VAL;
    CHECK(x == HUGE_VAL);

    CHECK(STDx ceil(5.1) == 6.0);
    CHECK(STDx fabs(-5.0) == 5.0);
    CHECK(STDx floor(5.1) == 5.0);
    CHECK(STDx fmod(-7.0, 3.0) == -1.0);
    CHECK(approx(STDx frexp(96.0, &xexp), 0.75) && xexp == 7);
    CHECK(STDx ldexp(-3.0, 4) == -48.0);
    CHECK(approx(STDx modf(-11.7, &x), -11.7 + 11.0) && x == -11.0);

    CHECK(approx(STDx acos(-rthalf), pi3by4));
    CHECK(approx(STDx asin(-rthalf), -piby4));
    CHECK(approx(STDx atan(-1.0), -piby4));
    CHECK(approx(STDx atan2(-1.0, 0.0), -2.0 * piby4));
    CHECK(approx(STDx cos(-piby4), rthalf));
    CHECK(approx(STDx sin(-pi3by4), -rthalf));
    CHECK(approx(STDx tan(-piby4), -1.0));

    CHECK(approx(STDx cosh(-1.0), (e + 1.0 / e) / 2.0));
    CHECK(approx(STDx exp(ln2), 2.0));
    CHECK(approx(STDx log(e * e * e), 3.0));
    CHECK(approx(STDx log10(1e5), 5.0));
    CHECK(approx(STDx pow(2.0, -0.5), rthalf));
    CHECK(approx(STDx sinh(-1.0), -(e - 1.0 / e) / 2.0));
    CHECK(approx(STDx sqrt(2.0), 1.0 / rthalf));
    CHECK(approx(STDx tanh(-1.0), -(e * e - 1.0) / (e * e + 1.0)));

    double (*pabs)(double) = &STD abs; // C++ overloads
    CHECK((*pabs)(-rthalf) == rthalf);
}

void test_main() { // test basic workings of cmath definitions
    test_cpp();
}
