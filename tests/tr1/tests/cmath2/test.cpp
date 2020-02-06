// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <cmath>, part 2
#define TEST_NAME  "<cmath>, part 2"
#define FLOAT_TYPE IS_FLOAT

#include "tdefs.h"
#include <cmath>

static const float e      = (float) 2.7182818284590452353602874713526625L;
static const float ln2    = (float) 0.69314718055994530941723212145817657L;
static const float pi3by4 = (float) 2.35619449019234492884698253745962716L;
static const float piby4  = (float) 0.78539816339744830961566084581987572L;
static const float rthalf = (float) 0.70710678118654752440084436210484904L;

#define STDx STD

void test_cpp() { // test C++ header
    float x;
    int xexp;

    CHECK(STDx ceilf(5.1F) == 6.0F);
    CHECK(STDx fabsf(-5.0F) == 5.0F);
    CHECK(STDx floorf(5.1F) == 5.0F);
    CHECK(STDx fmodf(-7.0F, 3.0F) == -1.0F);
    CHECK(approx(STDx frexpf(96.0F, &xexp), 0.75F) && xexp == 7);
    CHECK(STDx ldexpf(-3.0F, 4) == -48.0F);
    CHECK(approx(STDx modff(-11.7F, &x), -11.7F + 11.0F) && x == -11.0F);

    CHECK(approx(STDx acosf(-rthalf), pi3by4));
    CHECK(approx(STDx asinf(-rthalf), -piby4));
    CHECK(approx(STDx atanf(-1.0F), -piby4));
    CHECK(approx(STDx atan2f(-1.0F, 0.0F), -2.0F * piby4));
    CHECK(approx(STDx cosf(-piby4), rthalf));
    CHECK(approx(STDx sinf(-pi3by4), -rthalf));
    CHECK(approx(STDx tanf(-piby4), -1.0F));

    CHECK(approx(STDx coshf(-1.0F), (e + 1.0F / e) / 2.0F));
    CHECK(approx(STDx expf(ln2), 2.0F));
    CHECK(approx(STDx logf(e * e * e), 3.0F));
    CHECK(approx(STDx log10f(1e5F), 5.0F));
    CHECK(approx(STDx powf(2.0F, -0.5F), rthalf));
    CHECK(approx(STDx sinhf(-1.0F), -(e - 1.0F / e) / 2.0F));
    CHECK(approx(STDx sqrtf(2.0F), 1.0F / rthalf));
    CHECK(approx(STDx tanhf(-1.0F), -(e * e - 1.0F) / (e * e + 1.0F)));

    float (*pabs)(float) = &STDx abs; // C++ overloads
    CHECK((*pabs)(-rthalf) == rthalf);

    float (*pceil)(float) = &STDx ceil;
    CHECK((*pceil)(5.1F) == 6.0F);
    float (*pfabs)(float) = &STDx fabs;
    CHECK((*pfabs)(-5.0F) == 5.0F);
    float (*pfloor)(float) = &STDx floor;
    CHECK((*pfloor)(5.1F) == 5.0F);
    float (*pfmod)(float, float) = &STDx fmod;
    CHECK((*pfmod)(-7.0F, 3.0F) == -1.0F);
    float (*pfrexp)(float, int*) = &STDx frexp;
    CHECK(approx((*pfrexp)(96.0F, &xexp), 0.75F) && xexp == 7);
    float (*pldexp)(float, int) = &STDx ldexp;
    CHECK((*pldexp)(-3.0F, 4) == -48.0F);
    float (*pmodf)(float, float*) = &STDx modf;
    CHECK(approx((*pmodf)(-11.7F, &x), -11.7F + 11.0F) && x == -11.0F);

    float (*pacos)(float) = &STDx acos;
    CHECK(approx((*pacos)(-rthalf), pi3by4));
    float (*pasin)(float) = &STDx asin;
    CHECK(approx((*pasin)(-rthalf), -piby4));
    float (*patan)(float) = &STDx atan;
    CHECK(approx((*patan)(-1.0F), -piby4));
    float (*patan2)(float, float) = &STDx atan2;
    CHECK(approx((*patan2)(-1.0F, 0.0F), -2.0F * piby4));
    float (*pcos)(float) = &STDx cos;
    CHECK(approx((*pcos)(-piby4), rthalf));
    float (*psin)(float) = &STDx sin;
    CHECK(approx((*psin)(-pi3by4), -rthalf));
    float (*ptan)(float) = &STDx tan;
    CHECK(approx((*ptan)(-piby4), -1.0F));

    float (*pcosh)(float) = &STDx cosh;
    CHECK(approx((*pcosh)(-1.0F), (e + 1.0F / e) / 2.0F));
    float (*pexp)(float) = &STDx exp;
    CHECK(approx((*pexp)(ln2), 2.0F));
    float (*plog)(float) = &STDx log;
    CHECK(approx((*plog)(e * e * e), 3.0F));
    float (*plog10)(float) = &STDx log10;
    CHECK(approx((*plog10)(1e5), 5.0F));
    float (*ppow)(float, float) = &STDx pow;
    CHECK(approx((*ppow)(2.0F, -0.5F), rthalf));
    float (*psinh)(float) = &STDx sinh;
    CHECK(approx((*psinh)(-1.0F), -(e - 1.0F / e) / 2.0F));
    float (*psqrt)(float) = &STDx sqrt;
    CHECK(approx((*psqrt)(2.0F), 1.0F / rthalf));
    float (*ptanh)(float) = &STDx tanh;
    CHECK(approx((*ptanh)(-1.0F), -(e * e - 1.0F) / (e * e + 1.0F)));
}

void test_main() { // test basic workings of cmath definitions
    test_cpp();
}
