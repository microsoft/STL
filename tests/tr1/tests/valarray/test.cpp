// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <valarray>
#define TEST_NAME "<valarray>"

#include <math.h>
#include <valarray>

#define FLOAT_TYPE IS_DOUBLE
#include "tdefs.h"

typedef char Mytype;
typedef STD valarray<Mytype> Mycont;

static const double e      = 2.7182818284590452353602874713526625L;
static const double ln2    = 0.69314718055994530941723212145817657L;
static const double piby4  = 0.78539816339744830961566084581987572L;
static const double rthalf = 0.70710678118654752440084436210484904L;

void test_math() { // test valarray math functions
    typedef STD valarray<double> Myfloat;
    Myfloat v0(1), v1(1);

    v0[0] = -1.0;
    CHECK(STD abs(v0)[0] == 1.0);

    v0[0] = -1.0;
    CHECK(approx(STD acos(v0)[0], 4.0 * piby4));

    v0[0] = -1.0;
    CHECK(approx(STD asin(v0)[0], -2.0 * piby4));

    v0[0] = -1.0;
    CHECK(approx(STD atan(v0)[0], -piby4));

    v0[0] = -piby4;
    CHECK(approx(STD cos(v0)[0], rthalf));

    v0[0] = -piby4;
    CHECK(approx(STD sin(v0)[0], -rthalf));

    v0[0] = -piby4;
    CHECK(approx(STD tan(v0)[0], -1.0));

    v0[0] = -1.0;
    CHECK(approx(STD cosh(v0)[0], (e + 1.0 / e) / 2.0));

    v0[0] = ln2;
    CHECK(approx(STD exp(v0)[0], 2.0));

    v0[0] = e * e * e;
    CHECK(approx(STD log(v0)[0], 3.0));

    v0[0] = 1e5;
    CHECK(approx(STD log10(v0)[0], 5.0));

    v0[0] = -1.0;
    CHECK(approx(STD sinh(v0)[0], -(e - 1.0 / e) / 2.0));

    v0[0] = 2.0;
    CHECK(approx(STD sqrt(v0)[0], 1.0 / rthalf));

    v0[0] = -1.0;
    CHECK(approx(STD tanh(v0)[0], -(e * e - 1.0) / (e * e + 1.0)));

    v0[0] = -1.0;
    v1[0] = 0.0;
    CHECK(approx(STD atan2(v0, v1)[0], -2.0 * piby4));

    v0[0] = 2.0;
    v1[0] = -0.5;
    CHECK(approx(STD pow(v0, v1)[0], rthalf));
}

Mytype inc_val(Mytype val) { // increment operand
    return (Mytype) (val + 1);
}

void test_main() { // test basic workings of valarray definitions
    STD slice_array<Mytype>* psa    = (STD slice_array<Mytype>*) nullptr;
    STD gslice_array<Mytype>* pga   = (STD gslice_array<Mytype>*) nullptr;
    STD mask_array<Mytype>* pma     = (STD mask_array<Mytype>*) nullptr;
    STD indirect_array<Mytype>* pia = (STD indirect_array<Mytype>*) nullptr;
    STD slice* psl                  = (STD slice*) nullptr;
    STD gslice* pgsl                = (STD gslice*) nullptr;

    psa  = psa; // to quiet diagnostics
    pga  = pga;
    pma  = pma;
    pia  = pia;
    psl  = psl;
    pgsl = pgsl;

    Mycont v0, v1(4), v2('a', 5), v3("abcde", 6);
    const Mycont vx("abcdefghijklmnopqrstuvwxyz", 27);
    const CSTD size_t lv[] = {2, 3};
    const CSTD size_t dv[] = {7, 2};
    const bool vb[]        = {false, false, true, true, false, true};
    const CSTD size_t vi[] = {7, 5, 2, 3, 8};

    // test constructors
    CHECK_INT(v0.size(), 0);
    CHECK_INT(v1.size(), 4);
    CHECK_INT(v1[3], '\0');
    CHECK_INT(v2.size(), 5);
    CHECK_INT(v2[3], 'a');
    CHECK_INT(v3.size(), 6);
    CHECK_INT(v3[3], 'd');

    Mycont y0(vx[STD slice(2, 5, 3)]);
    CHECK_INT(y0.size(), 5);
    CHECK_MEM(&y0[0], "cfilo", 5);

    const STD valarray<CSTD size_t> len(lv, 2), str(dv, 2);
    Mycont y1(vx[STD gslice(3, len, str)]);
    CHECK_INT(y1.size(), 6);
    CHECK_MEM(&y1[0], "dfhkmo", 6);

    const STD valarray<bool> vmask(vb, 6);
    Mycont y2(vx[vmask]);
    CHECK_INT(y2.size(), 3);
    CHECK_MEM(&y2[0], "cdf", 3);

    const STD valarray<CSTD size_t> vind(vi, 5);
    Mycont y3(vx[vind]);
    CHECK_INT(y3.size(), 5);
    CHECK_MEM(&y3[0], "hfcdi", 3);

    // test assignments
    v0 = v1;
    CHECK_INT(v0.size(), 4);
    CHECK_INT(v0[3], '\0');

    v0                     = vx;
    v0[STD slice(2, 5, 3)] = Mycont("ABCDE", 5);
    CHECK_MEM(&v0[0], "abAdeBghCjkDmnEpqrstuvwxyz", 27);

    v0                          = vx;
    v0[STD gslice(3, len, str)] = Mycont("ABCDEF", 6);
    CHECK_MEM(&v0[0], "abcAeBgCijDlEnFpqrstuvwxyz", 27);

    v0        = vx;
    v0[vmask] = Mycont("ABCDE", 5);
    CHECK_MEM(&v0[0], "abABeCghijklmnopqrstuvwxyz", 27);

    v0       = vx;
    v0[vind] = Mycont("ABCDE", 5);
    CHECK_MEM(&v0[0], "abCDeBgAEjklmnopqrstuvwxyz", 27);

    {
        Mycont v6('x', 20);
        Mycont v7(STD move(v6));
        CHECK_INT(v6.size(), 0);
        CHECK_INT(v7.size(), 20);

        Mycont v8;
        v8 = STD move(v7);
        CHECK_INT(v7.size(), 0);
        CHECK_INT(v8.size(), 20);

        STD valarray<Movable_int> v9;
        v9.resize(10);
        CHECK_INT(v9.size(), 10);
        CHECK_INT(v9[9].val, 0);
    }

    {
        STD initializer_list<char> init{'a', 'b', 'c'};
        Mycont v11(init);
        CHECK_INT(v11.size(), 3);
        CHECK_INT(v11[2], 'c');

        v11.resize(0);
        v11 = init;
        CHECK_INT(v11.size(), 3);
        CHECK_INT(v11[2], 'c');
    }

    CHECK_INT(*STD begin(v3), 'a');
    CHECK_INT(STD begin(v3)[1], 'b');
    CHECK_INT(STD end(v3)[-2], 'e');
    CHECK_INT(STD end(v3)[-1], '\0');

    // test subscripting
    v0[2] = 'x';
    CHECK_INT(v0[2], 'x');
    CHECK_INT(vx[2], 'c');
    CHECK_INT((vx[STD slice(2, 5, 3)])[2], 'i');
    CHECK_INT((vx[STD gslice(3, len, str)])[2], 'h');
    CHECK_INT((vx[vmask])[2], 'f');
    CHECK_INT((vx[vind])[2], 'c');

    // test unary ops
    v0 = Mycont("\2\1", 3);
    CHECK_INT((+v0).size(), 3);
    CHECK_INT(v0[1], 1);
    CHECK_INT((-v0).size(), 3);
    CHECK_INT((-v0)[1], -1);
    CHECK_INT((~v0).size(), 3);
    CHECK_INT((~v0)[1], ~1);
    CHECK_INT((+v0).size(), 3);
    CHECK(!(!v0)[1]);

    // test assigning ops
    Mytype val = 2;
    v1         = Mycont("\2\2\2", 3);

    v0 = Mycont("\2\3\4", 3);
    CHECK_INT((v0 *= 3).size(), 3);
    CHECK_INT(v0[1], 9);
    CHECK_INT((v0 *= v1).size(), 3);
    CHECK_INT(v0[1], 18);
    CHECK_INT((v1 * val)[0], (v1 * v1)[0]);
    CHECK_INT((val * v1)[2], (v1 * v1)[2]);

    v0 = Mycont("\2\3\4", 3);
    CHECK_INT((v0 /= 3).size(), 3);
    CHECK_INT(v0[1], 1);
    CHECK_INT((v0 /= v1).size(), 3);
    CHECK_INT(v0[1], 0);
    CHECK_INT((v1 / val)[0], (v1 / v1)[0]);
    CHECK_INT((val / v1)[2], (v1 / v1)[2]);

    v0 = Mycont("\2\3\4", 3);
    CHECK_INT((v0 %= 3).size(), 3);
    CHECK_INT(v0[1], 0);
    CHECK_INT((v0 %= v1).size(), 3);
    CHECK_INT(v0[1], 0);
    CHECK_INT((v1 % val)[0], (v1 % v1)[0]);
    CHECK_INT((val % v1)[2], (v1 % v1)[2]);

    v0 = Mycont("\2\3\4", 3);
    CHECK_INT((v0 += 3).size(), 3);
    CHECK_INT(v0[1], 6);
    CHECK_INT((v0 += v1).size(), 3);
    CHECK_INT(v0[1], 8);
    CHECK_INT((v1 + val)[0], (v1 + v1)[0]);
    CHECK_INT((val + v1)[2], (v1 + v1)[2]);

    v0 = Mycont("\2\3\4", 3);
    CHECK_INT((v0 -= 3).size(), 3);
    CHECK_INT(v0[1], 0);
    CHECK_INT((v0 -= v1).size(), 3);
    CHECK_INT(v0[1], (char) -2);
    CHECK_INT((v1 - val)[0], (v1 - v1)[0]);
    CHECK_INT((val - v1)[2], (v1 - v1)[2]);

    v0 = Mycont("\2\3\4", 3);
    CHECK_INT((v0 &= 3).size(), 3);
    CHECK_INT(v0[1], 3);
    CHECK_INT((v0 &= v1).size(), 3);
    CHECK_INT(v0[1], 2);
    CHECK_INT((v1 & val)[0], (v1 & v1)[0]);
    CHECK_INT((val & v1)[2], (v1 & v1)[2]);

    v0 = Mycont("\2\3\4", 3);
    CHECK_INT((v0 |= 3).size(), 3);
    CHECK_INT(v0[1], 3);
    CHECK_INT((v0 |= v1).size(), 3);
    CHECK_INT(v0[1], 3);
    CHECK_INT((v1 | val)[0], (v1 | v1)[0]);
    CHECK_INT((val | v1)[2], (v1 | v1)[2]);

    v0 = Mycont("\2\3\4", 3);
    CHECK_INT((v0 ^= 3).size(), 3);
    CHECK_INT(v0[1], 0);
    CHECK_INT((v0 ^= v1).size(), 3);
    CHECK_INT(v0[1], 2);
    CHECK_INT((v1 ^ val)[0], (v1 ^ v1)[0]);
    CHECK_INT((val ^ v1)[2], (v1 ^ v1)[2]);

    v0 = Mycont("\2\3\4", 3);
    CHECK_INT((v0 <<= 3).size(), 3);
    CHECK_INT(v0[1], 24);
    CHECK_INT((v0 <<= v1).size(), 3);
    CHECK_INT(v0[1], 96);
    CHECK_INT((v1 << val)[0], (v1 << v1)[0]);
    CHECK_INT((val << v1)[2], (v1 << v1)[2]);

    v0 = Mycont("\2\3\4", 3);
    CHECK_INT((v0 >>= 3).size(), 3);
    CHECK_INT(v0[1], 0);
    CHECK_INT((v0 >>= v1).size(), 3);
    CHECK_INT(v0[1], 0);
    CHECK_INT((v1 >> val)[0], (v1 >> v1)[0]);
    CHECK_INT((val >> v1)[2], (v1 >> v1)[2]);

    // test remaining binary operators
    CHECK_INT((v1 && val)[0], (v1 && v1)[0]);
    CHECK_INT((val && v1)[2], (v1 && v1)[2]);

    CHECK_INT((v1 || val)[0], (v1 || v1)[0]);
    CHECK_INT((val || v1)[2], (v1 || v1)[2]);

    CHECK_INT((v1 == val)[0], (v1 == v1)[0]);
    CHECK_INT((val == v1)[2], (v1 == v1)[2]);

    CHECK_INT((v1 != val)[0], (v1 != v1)[0]);
    CHECK_INT((val != v1)[2], (v1 != v1)[2]);

    CHECK_INT((v1 < val)[0], (v1 < v1)[0]);
    CHECK_INT((val < v1)[2], (v1 < v1)[2]);

    CHECK_INT((v1 >= val)[0], (v1 >= v1)[0]);
    CHECK_INT((val >= v1)[2], (v1 >= v1)[2]);

    CHECK_INT((v1 > val)[0], (v1 > v1)[0]);
    CHECK_INT((val > v1)[2], (v1 > v1)[2]);

    CHECK_INT((v1 <= val)[0], (v1 <= v1)[0]);
    CHECK_INT((val <= v1)[2], (v1 <= v1)[2]);

    // test remaining member functions
    v1 = Mycont("\2\3\4", 3);
    CHECK_INT((v0 = v1.shift(1)).size(), 3);
    CHECK_INT(v0[1], 4);
    CHECK_INT((v0 = v1.shift(-1)).size(), 3);
    CHECK_INT(v0[1], 2);

    v1 = Mycont("\2\3\4", 3);
    CHECK_INT((v0 = v1.cshift(2)).size(), 3);
    CHECK_INT(v0[1], 2);
    CHECK_INT((v0 = v1.cshift(-2)).size(), 3);
    CHECK_INT(v0[1], 4);

    v1 = Mycont("\2\3\4", 3);
    CHECK_INT((v0 = v1.apply(&inc_val)).size(), 3);
    CHECK_INT(v0[1], 4);

    v0 = Mycont("\2\3\4", 3);
    v0.resize(1);
    CHECK_INT(v0.size(), 1);
    v0.resize(10, '\7');
    CHECK_INT(v0.size(), 10);
    CHECK_INT(v0[1], '\7');

    v0 = Mycont("\2\3\4", 3);
    CHECK_INT(v0.sum(), 9);
    CHECK_INT(v0.max(), 4);
    CHECK_INT(v0.min(), 2);

    // test range-based for
    v0 = Mycont();
    for (char c : v0) {
        (void) c;
        CHECK(false);
    }

    v0 = Mycont("\1\1\1", 3);
    for (char c : v0) {
        CHECK_INT(c, 1);
    }

    v0  = Mycont("\1\2\3", 3);
    val = 0;
    for (char c : v0) {
        val += c;
    }
    CHECK_INT(val, 6);

    test_math();
}
