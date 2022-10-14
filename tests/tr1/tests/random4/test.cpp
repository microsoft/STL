// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <random> C++11 header, part 4
#define TEST_NAME "<random>, part 4"

#include "tdefs.h"
#include <cmath>
#include <random>
#include <sstream>

typedef int Int32;
typedef unsigned int Uint32;

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

static void tseed_seq() { // test class seed_seq
    Uint32 arr1[5] = {'a', 'b', 'c', 'd', 'e'};
    Uint32 arr2[5] = {0};

    bool st = STD is_same<STD seed_seq::result_type, Uint32>::value;
    CHECK(st);

    STD seed_seq seq;
    CHECK_INT(seq.size(), 0);
    seq.param(&arr1[0]);
    CHECK_INT(arr1[0], 'a');

    STD seed_seq seq1(&arr1[0], &arr1[5]);
    CHECK_INT(seq1.size(), 5);
    seq1.param(&arr2[0]);
    CHECK_INT(arr2[0], 'a');
    CHECK_INT(arr2[4], 'e');

    seq.generate(&arr2[0], &arr2[4]);
    CHECK_INT(arr2[3], 3895714911U);
    seq1.generate(&arr2[0], &arr2[4]);
    CHECK_INT(arr2[3], 3734116661U);

    STD seed_seq seq2({arr1[0], arr1[1], arr1[2], arr1[3], arr1[4]});
    CHECK_INT(seq2.size(), 5);
    seq2.param(&arr2[0]);
    CHECK_INT(arr2[0], 'a');
    CHECK_INT(arr2[4], 'e');
}

static void tgenerate() { // test generate_canonical
    typedef STD ranlux24 rng_t;
    rng_t gen1, gen2;
    double x = STD generate_canonical<double, 40, rng_t>(gen1);
    CHECK(0.0 <= x && x < 1.0);
    double y = STD generate_canonical<double, 40>(gen2);
    CHECK_DOUBLE(x, y);
}

static void tlinear() { // test linear_congruential_engine
    typedef STD linear_congruential_engine<Uint32, 16807, 0, 2147483647> rng_t;

    CHECK_INT(rng_t::multiplier, 16807);
    CHECK_INT(rng_t::increment, 0);
    CHECK_INT(rng_t::modulus, 2147483647);
    CHECK_INT(rng_t::default_seed, 1);

    bool st = STD is_same<rng_t::result_type, Uint32>::value;
    CHECK(st);

    CHECK_INT(rng_t::min(), 1);
    CHECK_INT(rng_t::max(), 2147483646);

    rng_t rng;
    CHECK_INT(rng.min(), 1);
    CHECK_INT(rng.max(), 2147483646);
    CHECK_INT(rng(), 16807);

    rng_t rng1(2);
    CHECK_INT(rng1.min(), 1);
    CHECK_INT(rng1.max(), 2147483646);
    CHECK_INT(rng1(), 33614);

    STD seed_seq seq;
    rng_t rng2(seq);
    CHECK_INT(rng2(), 651595794U);

    rng.seed(1);
    CHECK_INT(rng.min(), 1);
    CHECK_INT(rng.max(), 2147483646);
    CHECK_INT(rng(), 16807);

    rng.seed(2);
    CHECK_INT(rng.min(), 1);
    CHECK_INT(rng.max(), 2147483646);
    CHECK_INT(rng(), 33614);
    rng.seed(seq);
    CHECK_INT(rng(), 651595794U);

    rng.seed(1);
    rng.discard(1);
    CHECK_INT(rng(), 282475249);

    test_globals<rng_t>::test();

    // check large values
    typedef unsigned long long int_type;
    const int_type max_val = (int_type) -1;

    typedef STD linear_congruential_engine<int_type, max_val - 1, 0, max_val> rng4_t;
    rng4_t rng4(1);
    CHECK(rng4() == max_val - 1);
    CHECK(rng4() == 1);
}

static void tmersenne() {
    typedef STD mersenne_twister_engine<Uint32, 32, 624, 397, 31, 0x9908b0df, 11, 0xffffffff, 7, 0x9d2c5680, 15,
        0xefc60000, 18, 1812433253>
        rng_t;
    CHECK_INT(rng_t::word_size, 32);
    CHECK_INT(rng_t::state_size, 624);
    CHECK_INT(rng_t::shift_size, 397);
    CHECK_INT(rng_t::mask_bits, 31);
    CHECK_INT((int) rng_t::xor_mask, (int) 0x9908b0df);
    CHECK_INT(rng_t::tempering_u, 11);
    CHECK_INT(rng_t::tempering_d, (int) 0xffffffff);
    CHECK_INT(rng_t::tempering_s, 7);
    CHECK_INT((int) rng_t::tempering_b, (int) 0x9d2c5680);
    CHECK_INT(rng_t::tempering_t, 15);
    CHECK_INT((int) rng_t::tempering_c, (int) 0xefc60000);
    CHECK_INT(rng_t::tempering_l, 18);
    CHECK_INT(rng_t::initialization_multiplier, 1812433253);
    CHECK_INT(rng_t::default_seed, 5489);

    bool st = STD is_same<rng_t::result_type, Uint32>::value;
    CHECK(st);

    CHECK_INT(rng_t::min(), 0);
    CHECK_INT(rng_t::max(), 0xffffffff);

    rng_t rng;
    CHECK_INT(rng.min(), 0);
    CHECK_INT(rng.max(), 0xffffffff);
    CHECK_INT(rng(), (int) 3499211612u);

    rng_t rng1(1);
    CHECK_INT(rng1.min(), 0);
    CHECK_INT(rng1.max(), 0xffffffff);
    CHECK_INT(rng1(), 1791095845);

    STD seed_seq seq;
    rng_t rng2(seq);
    CHECK_INT(rng2(), 2872601305U);

    rng.seed(1);
    CHECK_INT(rng.min(), 0);
    CHECK_INT(rng.max(), 0xffffffff);
    CHECK_INT(rng(), 1791095845);

    rng.seed(2);
    CHECK_INT(rng.min(), 0);
    CHECK_INT(rng.max(), 0xffffffff);
    CHECK_INT(rng(), 1872583848);
    rng.seed(seq);
    CHECK_INT(rng(), 2872601305U);

    rng.seed(1);
    rng.discard(1);
    CHECK_INT(rng(), 4282876139U);

    test_globals<rng_t>::test();
}

static void tsubtract() {
    typedef STD subtract_with_carry_engine<Uint32, 24, 10, 24> rng_t;
    CHECK_INT(rng_t::word_size, 24);
    CHECK_INT(rng_t::short_lag, 10);
    CHECK_INT(rng_t::long_lag, 24);
    CHECK_INT(rng_t::default_seed, 19780503);
    bool st = STD is_same<rng_t::result_type, Uint32>::value;
    CHECK(st);

    CHECK_INT(rng_t::min(), 0);
    CHECK_INT(rng_t::max(), (1 << 24) - 1);

    rng_t rng;
    CHECK_INT(rng.min(), 0);
    CHECK_INT(rng.max(), (1 << 24) - 1);
    CHECK_INT(rng(), 15039276);

    rng_t rng1(1);
    CHECK_INT(rng1.min(), 0);
    CHECK_INT(rng1.max(), (1 << 24) - 1);
    CHECK_INT(rng1(), 8871692);

    STD seed_seq seq;
    rng_t rng2(seq);
    CHECK_INT(rng2(), 13077165U);

    rng.seed(1);
    CHECK_INT(rng.min(), 0);
    CHECK_INT(rng.max(), (1 << 24) - 1);
    CHECK_INT(rng(), 8871692);

    rng.seed(2);
    CHECK_INT(rng.min(), 0);
    CHECK_INT(rng.max(), (1 << 24) - 1);
    CHECK_INT(rng(), 966168);

    rng.seed(1);
    rng.discard(1);
    CHECK_INT(rng(), 3740959);

    test_globals<rng_t>::test();
}

static void tdiscard() {
    int i;
    typedef STD subtract_with_carry_engine<Uint32, 24, 10, 24> rng_base_t;
    typedef STD discard_block_engine<rng_base_t, 223, 24> rng_t;
    CHECK_INT(rng_t::block_size, 223);
    CHECK_INT(rng_t::used_block, 24);
    bool st = STD is_same<rng_t::result_type, rng_base_t::result_type>::value;
    CHECK(st);

    CHECK_INT(rng_t::min(), 0);
    CHECK_INT(rng_t::max(), (1 << 24) - 1);

    rng_t rng;
    CHECK_INT(rng.min(), 0);
    CHECK_INT(rng.max(), (1 << 24) - 1);
    CHECK_INT(rng(), 15039276);

    rng_base_t urng;
    rng_t rng0(urng);
    CHECK_INT(rng0.min(), 0);
    CHECK_INT(rng0.max(), (1 << 24) - 1);
    CHECK_INT(rng0(), 15039276);

    rng_t rng1(1);
    CHECK_INT(rng1.min(), 0);
    CHECK_INT(rng1.max(), (1 << 24) - 1);
    CHECK_INT(rng1(), 8871692);

    STD seed_seq seq;
    rng_t rng2(seq);
    CHECK_INT(rng2(), 13077165U);

    rng.seed();
    CHECK_INT(rng.min(), 0);
    CHECK_INT(rng.max(), (1 << 24) - 1);
    CHECK_INT(rng(), 15039276);

    rng.seed(1);
    CHECK_INT(rng.min(), 0);
    CHECK_INT(rng.max(), (1 << 24) - 1);
    CHECK_INT(rng(), 8871692);

    rng.seed(2);
    CHECK_INT(rng.min(), 0);
    CHECK_INT(rng.max(), (1 << 24) - 1);
    CHECK_INT(rng(), 966168);

    rng.seed(1);
    rng.discard(1);
    CHECK_INT(rng(), 3740959);

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

static void tindependent() {
    typedef STD subtract_with_carry_engine<Uint32, 24, 10, 24> rng_base_t;
    typedef STD independent_bits_engine<rng_base_t, 24, Uint32> rng_t;
    bool st = STD is_same<rng_t::result_type, rng_base_t::result_type>::value;
    CHECK(st);

    CHECK_INT(rng_t::min(), 0);
    CHECK_INT(rng_t::max(), (1 << 24) - 1);

    rng_t rng;
    CHECK_INT(rng.min(), 0);
    CHECK_INT(rng.max(), (1 << 24) - 1);
    CHECK_INT(rng(), 15039276);

    rng_base_t urng;
    rng_t rng0(urng);
    CHECK_INT(rng0.min(), 0);
    CHECK_INT(rng0.max(), (1 << 24) - 1);
    CHECK_INT(rng0(), 15039276);

    rng_t rng1(1);
    CHECK_INT(rng1.min(), 0);
    CHECK_INT(rng1.max(), (1 << 24) - 1);
    CHECK_INT(rng1(), 8871692);

    STD seed_seq seq;
    rng_t rng2(seq);
    CHECK_INT(rng2(), 13077165U);

    rng.seed();
    CHECK_INT(rng.min(), 0);
    CHECK_INT(rng.max(), (1 << 24) - 1);
    CHECK_INT(rng(), 15039276);

    rng.seed(1);
    CHECK_INT(rng.min(), 0);
    CHECK_INT(rng.max(), (1 << 24) - 1);
    CHECK_INT(rng(), 8871692);

    rng.seed(2);
    CHECK_INT(rng.min(), 0);
    CHECK_INT(rng.max(), (1 << 24) - 1);
    CHECK_INT(rng(), 966168);

    rng.seed(1);
    rng.discard(1);
    CHECK_INT(rng(), 3740959);

    test_globals<rng_t>::test();
}

static void tshuffle() {
    typedef STD subtract_with_carry_engine<Uint32, 24, 10, 24> rng_base_t;
    typedef STD shuffle_order_engine<rng_base_t, 5> rng_t;
    CHECK_INT(rng_t::table_size, 5);
    bool st = STD is_same<rng_t::result_type, rng_base_t::result_type>::value;
    CHECK(st);

    CHECK_INT(rng_t::min(), 0);
    CHECK_INT(rng_t::max(), (1 << 24) - 1);

    rng_t rng;
    const int defaultResult = 14283486;
    CHECK_INT(rng.min(), 0);
    CHECK_INT(rng.max(), (1 << 24) - 1);
    CHECK_INT(rng(), defaultResult);

    rng_base_t urng;
    rng_t rng0(urng);
    CHECK_INT(rng0.min(), 0);
    CHECK_INT(rng0.max(), (1 << 24) - 1);
    CHECK_INT(rng0(), defaultResult);

    rng_t rng1(1);
    const int oneResult = 11575129;
    CHECK_INT(rng1.min(), 0);
    CHECK_INT(rng1.max(), (1 << 24) - 1);
    CHECK_INT(rng1(), oneResult);

    STD seed_seq seq;
    rng_t rng2(seq);
    CHECK_INT(rng2(), 747473);

    rng.seed();
    CHECK_INT(rng.min(), 0);
    CHECK_INT(rng.max(), (1 << 24) - 1);
    CHECK_INT(rng(), defaultResult);

    rng.seed(1);
    CHECK_INT(rng.min(), 0);
    CHECK_INT(rng.max(), (1 << 24) - 1);
    CHECK_INT(rng(), oneResult);

    rng.seed(2);
    CHECK_INT(rng.min(), 0);
    CHECK_INT(rng.max(), (1 << 24) - 1);
    CHECK_INT(rng(), 6373042);

    rng.seed(1);
    rng.discard(1);
    CHECK_INT(rng(), 1619564);

    test_globals<rng_t>::test();
}

static void tmt19937_64() {
    typedef STD mt19937_64 rng_t;
    rng_t rng;
    rng_t::result_type res = 0;
    for (int i = 0; i < 10000; ++i) {
        res = rng();
    }
    CHECK(res == 9981545732273789042ULL);
}

static void tranlux24_base() {
    typedef STD ranlux24_base rng_t;
    rng_t rng;
    rng_t::result_type res = 0;
    for (int i = 0; i < 10000; ++i) {
        res = rng();
    }
    CHECK_INT(res, 7937952);
}

static void tranlux24() {
    typedef STD ranlux24 rng_t;
    rng_t rng;
    rng_t::result_type res = 0;
    for (int i = 0; i < 10000; ++i) {
        res = rng();
    }
    CHECK_INT(res, 9901578);
}

static void tranlux48_base() {
    typedef STD ranlux48_base rng_t;
    rng_t rng;
    rng_t::result_type res = 0;
    for (int i = 0; i < 10000; ++i) {
        res = rng();
    }
    CHECK(res == 61839128582725ULL);
}

static void tranlux48() {
    typedef STD ranlux48 rng_t;
    rng_t rng;
    rng_t::result_type res = 0;
    for (int i = 0; i < 10000; ++i) {
        res = rng();
    }
    CHECK(res == 249142670248501ULL);
}

static void tknuth() {
    typedef STD knuth_b rng_t;
    rng_t rng;
    rng_t::result_type res = 0;
    for (int i = 0; i < 10000; ++i) {
        res = rng();
    }
    CHECK_INT(res, 1112339016U);
}

void test_main() { // test generators
    tseed_seq();
    tgenerate();
    tlinear();
    tmersenne();
    tsubtract();
    tdiscard();
    tindependent();
    tshuffle();

    tmt19937_64();
    tranlux24_base();
    tranlux24();
    tranlux48_base();
    tranlux48();
    tknuth();
}
