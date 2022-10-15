// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <random> header, part 1
#define TEST_NAME "<random>, part 1"

#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING

#include "tdefs.h"
#include <math.h>
#include <memory>
#include <random>
#include <sstream>

typedef int Int32;
typedef unsigned int Uint32;

template <class T>
class generator {
public:
    void reset(const T* ptr0, unsigned int n0) { // establish new sequence
        ptr = ptr0;
        n   = n0;
        i   = 0;
    }

    T operator()() { // return next element
        return ptr[i++];
    }

    unsigned int index() const { // return index into sequence
        return i;
    }

private:
    const T* ptr;
    unsigned int n;
    unsigned int i;
};

template <class Eng>
struct test_globals { // tests engine global functions
    static void test() { // test globals
        Eng rng0, rng1, rng2;

        (void) rng2();
        CHECK(rng0 == rng1);
        CHECK(rng0 != rng2);

        (void) rng1();
        CHECK(rng1 == rng2);

        STD stringstream str;
        str << rng1;
        CHECK(rng1 == rng2);

        str >> rng0;
        CHECK(rng0 == rng1);
        CHECK(rng0() == rng1());
    }
};

static void tlinear() { // test linear_congruential
    typedef STD linear_congruential<Uint32, 16807, 0, 2147483647> rng_t;

    CHECK_INT(rng_t::multiplier, 16807);
    CHECK_INT(rng_t::increment, 0);
    CHECK_INT(rng_t::modulus, 2147483647);
    bool st = STD is_same<rng_t::result_type, Uint32>::value;
    CHECK(st);

    rng_t rng;
    CHECK_INT(rng.min(), 1);
    CHECK_INT(rng.max(), 2147483646);
    CHECK_INT(rng(), 16807);

    rng_t rng1(2);
    CHECK_INT(rng1.min(), 1);
    CHECK_INT(rng1.max(), 2147483646);
    CHECK_INT(rng1(), 33614);

    generator<Uint32> g;
    Uint32 init[] = {1, 2};
    g.reset(init, 2);

    rng_t rng2(g);
    CHECK_INT(rng2.min(), 1);
    CHECK_INT(rng2.max(), 2147483646);
    CHECK_INT(rng2(), 16807);
    CHECK_INT(g.index(), 1);

    rng_t rng3(g);
    CHECK_INT(rng3.min(), 1);
    CHECK_INT(rng3.max(), 2147483646);
    CHECK_INT(rng3(), 33614);
    CHECK_INT(g.index(), 2);

    rng.seed(1);
    CHECK_INT(rng.min(), 1);
    CHECK_INT(rng.max(), 2147483646);
    CHECK_INT(rng(), 16807);

    rng.seed(2);
    CHECK_INT(rng.min(), 1);
    CHECK_INT(rng.max(), 2147483646);
    CHECK_INT(rng(), 33614);

    g.reset(init, 2);
    rng3.seed(g);
    CHECK_INT(rng3.min(), 1);
    CHECK_INT(rng3.max(), 2147483646);
    CHECK_INT(rng3(), 16807);
    CHECK_INT(g.index(), 1);

    rng3.seed(g);
    CHECK_INT(rng3.min(), 1);
    CHECK_INT(rng3.max(), 2147483646);
    CHECK_INT(rng3(), 33614);
    CHECK_INT(g.index(), 2);

    test_globals<rng_t>::test();

    // check large values
    typedef unsigned long long int_type;
    const int_type max_val = (int_type) -1;

    typedef STD linear_congruential<int_type, max_val - 1, 0, max_val> rng4_t;
    rng4_t rng4(1);
    CHECK(rng4() == max_val - 1);
    CHECK(rng4() == 1);
}

static void tminstd_rand0() {
    typedef STD minstd_rand0 rng_t;
    CHECK_INT(rng_t::multiplier, 16807);
    CHECK_INT(rng_t::increment, 0);
    CHECK_INT(rng_t::modulus, 2147483647);
    rng_t rng;
    Int32 res = 0;
    for (int i = 0; i < 10000; ++i) {
        res = rng();
    }
    CHECK_INT(res, 1043618065);
}

static void tminstd_rand() {
    typedef STD minstd_rand rng_t;
    CHECK_INT(rng_t::multiplier, 48271);
    CHECK_INT(rng_t::increment, 0);
    CHECK_INT(rng_t::modulus, 2147483647);
    rng_t rng;
    Int32 res = 0;
    for (int i = 0; i < 10000; ++i) {
        res = rng();
    }
    CHECK_INT(res, 399268537);
}

static void tmersenne() {
    typedef STD mersenne_twister<Uint32, 32, 624, 397, 31, 0x9908b0df, 11, 7, 0x9d2c5680, 15, 0xefc60000, 18> rng_t;
    CHECK_INT(rng_t::word_size, 32);
    CHECK_INT(rng_t::state_size, 624);
    CHECK_INT(rng_t::shift_size, 397);
    CHECK_INT(rng_t::mask_bits, 31);
    CHECK_INT((int) rng_t::parameter_a, (int) 0x9908b0df);
    CHECK_INT(rng_t::output_u, 11);
    CHECK_INT(rng_t::output_s, 7);
    CHECK_INT((int) rng_t::output_b, (int) 0x9d2c5680);
    CHECK_INT(rng_t::output_t, 15);
    CHECK_INT((int) rng_t::output_c, (int) 0xefc60000);
    CHECK_INT(rng_t::output_l, 18);
    bool st = STD is_same<rng_t::result_type, Uint32>::value;
    CHECK(st);

    rng_t rng;
    CHECK_INT(rng.min(), 0);
    CHECK_INT(rng.max(), 0xffffffff);
    CHECK_INT(rng(), (int) 3499211612u);

    const auto ptr_rng1 = STD make_unique<rng_t>(1);
    CHECK_INT(ptr_rng1->min(), 0);
    CHECK_INT(ptr_rng1->max(), 0xffffffff);
    CHECK_INT((*ptr_rng1)(), 1791095845);

    STD linear_congruential<Uint32, 69069, 0, 0> init_gen(4357);

    const auto ptr_rng2 = STD make_unique<rng_t>(init_gen);
    CHECK_INT(ptr_rng2->min(), 0);
    CHECK_INT(ptr_rng2->max(), 0xffffffff);
    CHECK_INT((*ptr_rng2)(), (int) 4290933890u);

    rng_t rng3(init_gen);
    CHECK_INT(rng3.min(), 0);
    CHECK_INT(rng3.max(), 0xffffffff);
    CHECK_INT(rng3(), (int) 2649890907u);

    rng.seed(1);
    CHECK_INT(rng.min(), 0);
    CHECK_INT(rng.max(), 0xffffffff);
    CHECK_INT(rng(), 1791095845);

    rng.seed(2);
    CHECK_INT(rng.min(), 0);
    CHECK_INT(rng.max(), 0xffffffff);
    CHECK_INT(rng(), 1872583848);

    init_gen.seed(4357);
    rng3.seed(init_gen);
    CHECK_INT(rng3.min(), 0);
    CHECK_INT(rng3.max(), 0xffffffff);
    CHECK_INT(rng3(), (int) 4290933890u);

    rng3.seed(init_gen);
    CHECK_INT(rng3.min(), 0);
    CHECK_INT(rng3.max(), 0xffffffff);
    CHECK_INT(rng3(), (int) 2649890907u);

    test_globals<rng_t>::test();
}

static void tmt19937() {
    typedef STD mt19937 rng_t;
    CHECK_INT(rng_t::word_size, 32);
    CHECK_INT(rng_t::state_size, 624);
    CHECK_INT(rng_t::shift_size, 397);
    CHECK_INT(rng_t::mask_bits, 31);
    CHECK_INT((int) rng_t::parameter_a, (int) 0x9908b0df);
    CHECK_INT(rng_t::output_u, 11);
    CHECK_INT(rng_t::output_s, 7);
    CHECK_INT((int) rng_t::output_b, (int) 0x9d2c5680);
    CHECK_INT(rng_t::output_t, 15);
    CHECK_INT((int) rng_t::output_c, (int) 0xefc60000);
    CHECK_INT(rng_t::output_l, 18);
    rng_t rng;
    Int32 res = 0;
    for (int i = 0; i < 10000; ++i) {
        res = rng();
    }
    CHECK_INT(res, (int) 4123659995UL);

    rng_t rng0, rng1;
    for (int i = 0; i < 300; ++i) {
        (void) rng0();
    }

    STD stringstream str;
    str << rng0;
    str >> rng1;
    CHECK(rng0 == rng1);
    CHECK(rng1 == rng0);
    for (int i = 0; i < 600; ++i) {
        (void) rng0();
        (void) rng1();
    }

    CHECK(rng0 == rng1);
    CHECK(rng1 == rng0);
    for (int i = 0; i < 50; ++i) {
        (void) rng0();
        (void) rng1();
    }

    CHECK(rng0 == rng1);
    CHECK(rng1 == rng0);
    for (int i = 0; i < 300; ++i) {
        (void) rng0();
        (void) rng1();
    }

    CHECK(rng0 == rng1);
    CHECK(rng1 == rng0);
}

static void tsubtract() {
    typedef STD subtract_with_carry<Uint32, 1 << 24, 10, 24> rng_t;
    CHECK_INT(rng_t::modulus, 1 << 24);
    CHECK_INT(rng_t::short_lag, 10);
    CHECK_INT(rng_t::long_lag, 24);
    bool st = STD is_same<rng_t::result_type, Uint32>::value;
    CHECK(st);

    rng_t rng;
    CHECK_INT(rng.min(), 0);
    CHECK_INT(rng.max(), (1 << 24) - 1);
    CHECK_INT(rng(), 15039276);

    rng_t rng1(1);
    CHECK_INT(rng1.min(), 0);
    CHECK_INT(rng1.max(), (1 << 24) - 1);
    CHECK_INT(rng1(), 8871692);

    STD linear_congruential<Uint32, 40014, 0, 2147483563> init_gen(19780503);

    rng_t rng2(init_gen);
    CHECK_INT(rng2.min(), 0);
    CHECK_INT(rng2.max(), (1 << 24) - 1);
    CHECK_INT(rng2(), 15039276);

    rng_t rng3(init_gen);
    CHECK_INT(rng3.min(), 0);
    CHECK_INT(rng3.max(), (1 << 24) - 1);
    CHECK_INT(rng3(), 6319224);

    rng.seed(1);
    CHECK_INT(rng.min(), 0);
    CHECK_INT(rng.max(), (1 << 24) - 1);
    CHECK_INT(rng(), 8871692);

    rng.seed(2);
    CHECK_INT(rng.min(), 0);
    CHECK_INT(rng.max(), (1 << 24) - 1);
    CHECK_INT(rng(), 966168);

    init_gen.seed(19780503);
    rng3.seed(init_gen);
    CHECK_INT(rng3.min(), 0);
    CHECK_INT(rng3.max(), (1 << 24) - 1);
    CHECK_INT(rng3(), 15039276);

    rng3.seed(init_gen);
    CHECK_INT(rng3.min(), 0);
    CHECK_INT(rng3.max(), (1 << 24) - 1);
    CHECK_INT(rng3(), 6319224);

    test_globals<rng_t>::test();
}

#if _HAS_TR1_NAMESPACE
static void tranlux3() {
    typedef STD ranlux3 rng_t;
    CHECK_INT(rng_t::block_size, 223);
    CHECK_INT(rng_t::used_block, 24);
    CHECK_INT(rng_t::base_type::modulus, (1 << 24));
    CHECK_INT(rng_t::base_type::long_lag, 24);
    CHECK_INT(rng_t::base_type::short_lag, 10);
    rng_t rng;
    Int32 res = 0;
    for (int i = 0; i < 10000; ++i) {
        res = rng();
    }
    CHECK_INT(res, 5957620);
}

static void tranlux4() {
    typedef STD ranlux4 rng_t;
    CHECK_INT(rng_t::block_size, 389);
    CHECK_INT(rng_t::used_block, 24);
    CHECK_INT(rng_t::base_type::modulus, (1 << 24));
    CHECK_INT(rng_t::base_type::long_lag, 24);
    CHECK_INT(rng_t::base_type::short_lag, 10);
    rng_t rng;
    Int32 res = 0;
    for (int i = 0; i < 10000; ++i) {
        res = rng();
    }
    CHECK_INT(res, 8587295);
}

static void tsubtract_01() {
    float factor = ldexpf(1.0f, -24);
    typedef STD subtract_with_carry_01<float, 24, 10, 24> rng_t;
    CHECK_INT(rng_t::word_size, 24);
    CHECK_INT(rng_t::long_lag, 24);
    CHECK_INT(rng_t::short_lag, 10);
    bool st = STD is_same<rng_t::result_type, float>::value;
    CHECK(st);

    rng_t rng;
    CHECK_DOUBLE(rng.min(), 0.0);
    CHECK_DOUBLE(rng.max(), 1.0);
    CHECK_DOUBLE(rng(), 15039276 * factor);

    rng_t rng1(1);
    CHECK_DOUBLE(rng1.min(), 0.0);
    CHECK_DOUBLE(rng1.max(), 1.0);
    CHECK_DOUBLE(rng1(), 8871692 * factor);

    STD linear_congruential<unsigned int, 40014, 0, 2147483563> init_gen(19780503);

    rng_t rng2(init_gen);
    CHECK_DOUBLE(rng2.min(), 0.0);
    CHECK_DOUBLE(rng2.max(), 1.0);
    CHECK_DOUBLE(rng2(), 15039276 * factor);

    rng_t rng3(init_gen);
    CHECK_DOUBLE(rng3.min(), 0);
    CHECK_DOUBLE(rng3.max(), 1.0);
    CHECK_DOUBLE(rng3(), 6319224 * factor);

    rng.seed(1);
    CHECK_DOUBLE(rng.min(), 0.0);
    CHECK_DOUBLE(rng.max(), 1.0);
    CHECK_DOUBLE(rng(), 8871692 * factor);

    rng.seed(2);
    CHECK_DOUBLE(rng.min(), 0.0);
    CHECK_DOUBLE(rng.max(), 1.0);
    CHECK_DOUBLE(rng(), 966168 * factor);

    init_gen.seed(19780503);
    rng3.seed(init_gen);
    CHECK_DOUBLE(rng3.min(), 0);
    CHECK_DOUBLE(rng3.max(), 1.0);
    CHECK_DOUBLE(rng3(), 15039276 * factor);

    rng3.seed(init_gen);
    CHECK_DOUBLE(rng3.min(), 0.0);
    CHECK_DOUBLE(rng3.max(), 1.0);
    CHECK_DOUBLE(rng3(), 6319224 * factor);

    test_globals<rng_t>::test();
}

static void tranlux_base_01() {
    typedef STD ranlux_base_01 rng_t;
    bool st = STD is_same<rng_t::result_type, float>::value;
    CHECK(st);
    CHECK_INT(rng_t::word_size, 24);
    CHECK_INT(rng_t::short_lag, 10);
    CHECK_INT(rng_t::long_lag, 24);
}

static void tranlux64_base_01() {
    typedef STD ranlux64_base_01 rng_t;
    bool st = STD is_same<rng_t::result_type, double>::value;
    CHECK(st);
    CHECK_INT(rng_t::word_size, 48);
    CHECK_INT(rng_t::short_lag, 5);
    CHECK_INT(rng_t::long_lag, 12);
}

static void tranlux3_01() {
    typedef STD ranlux3_01 rng_t;
    CHECK_INT(rng_t::block_size, 223);
    CHECK_INT(rng_t::used_block, 24);
    CHECK_INT(rng_t::base_type::word_size, 24);
    CHECK_INT(rng_t::base_type::short_lag, 10);
    CHECK_INT(rng_t::base_type::long_lag, 24);
    rng_t rng;
    float res = 0;
    for (int i = 0; i < 10000; ++i) {
        res = rng();
    }
    CHECK_DOUBLE(res, 5957620 / CSTD pow(2.0f, 24));
}

static void tranlux4_01() {
    typedef STD ranlux4_01 rng_t;
    CHECK_INT(rng_t::block_size, 389);
    CHECK_INT(rng_t::used_block, 24);
    CHECK_INT(rng_t::base_type::word_size, 24);
    CHECK_INT(rng_t::base_type::short_lag, 10);
    CHECK_INT(rng_t::base_type::long_lag, 24);
    rng_t rng;
    float res = 0;
    for (int i = 0; i < 10000; ++i) {
        res = rng();
    }
    CHECK_DOUBLE(res, 8587295 / STD pow(2.0f, 24));
}
#endif // _HAS_TR1_NAMESPACE

static void tdiscard() {
    int i;
    typedef STD subtract_with_carry<Uint32, 1 << 24, 10, 24> rng_base_t;
    typedef STD discard_block<rng_base_t, 223, 24> rng_t;
    CHECK_INT(rng_t::block_size, 223);
    CHECK_INT(rng_t::used_block, 24);
    CHECK_INT(rng_t::base_type::modulus, 1 << 24);
    CHECK_INT(rng_t::base_type::long_lag, 24);
    CHECK_INT(rng_t::base_type::short_lag, 10);
    bool st = STD is_same<rng_t::result_type, Uint32>::value;
    CHECK(st);

    rng_t rng;
    CHECK_INT(rng.min(), 0);
    CHECK_INT(rng.max(), (1 << 24) - 1);
    CHECK_INT(rng(), 15039276);

    STD linear_congruential<unsigned int, 40014, 0, 2147483563> init_gen(19780503);

    rng_t rng2(init_gen);
    CHECK_INT(rng2.min(), 0);
    CHECK_INT(rng2.max(), (1 << 24) - 1);
    CHECK_INT(rng2(), 15039276);

    rng_t rng3(init_gen);
    CHECK_INT(rng3.min(), 0);
    CHECK_INT(rng3.max(), (1 << 24) - 1);
    CHECK_INT(rng3(), 6319224);

    init_gen.seed(19780503);
    rng3.seed(init_gen);
    CHECK_INT(rng3.min(), 0);
    CHECK_INT(rng3.max(), (1 << 24) - 1);
    CHECK_INT(rng3(), 15039276);

    rng3.seed(init_gen);
    CHECK_INT(rng3.min(), 0);
    CHECK_INT(rng3.max(), (1 << 24) - 1);
    CHECK_INT(rng3(), 6319224);

    rng_base_t rng4;
    rng_t rng5;
    for (i = 0; i < rng_t::used_block; ++i) {
        CHECK_INT(rng4(), rng5());
    }
    CHECK(rng4() != rng5());
    for (; i < rng_t::block_size; ++i) {
        (void) rng4();
    }

    CHECK_INT(rng4(), rng5());

    test_globals<rng_t>::test();
}

void test_main() { // test generators
    tlinear();
    tminstd_rand0();
    tminstd_rand();
    tmersenne();
    tmt19937();
    tsubtract();
#if _HAS_TR1_NAMESPACE
    tranlux3();
    tranlux4();
    tsubtract_01();
    tranlux_base_01();
    tranlux64_base_01();
    tranlux3_01();
    tranlux4_01();
#endif // _HAS_TR1_NAMESPACE
    tdiscard();
}
