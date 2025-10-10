// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <random> header, part 1
#define TEST_NAME "<random>, part 1"

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

static void tlinear() {
    typedef STD linear_congruential_engine<Uint32, 16807, 0, 2147483647> rng_t;

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

    rng.seed(1);
    CHECK_INT(rng.min(), 1);
    CHECK_INT(rng.max(), 2147483646);
    CHECK_INT(rng(), 16807);

    rng.seed(2);
    CHECK_INT(rng.min(), 1);
    CHECK_INT(rng.max(), 2147483646);
    CHECK_INT(rng(), 33614);

    test_globals<rng_t>::test();

    // check large values
    typedef unsigned long long int_type;
    const int_type max_val = (int_type) -1;

    typedef STD linear_congruential_engine<int_type, max_val - 1, 0, max_val> rng4_t;
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
    typedef STD mt19937 rng_t;
    CHECK_INT(rng_t::word_size, 32);
    CHECK_INT(rng_t::state_size, 624);
    CHECK_INT(rng_t::shift_size, 397);
    CHECK_INT(rng_t::mask_bits, 31);
    CHECK_INT(rng_t::xor_mask, 0x9908b0df);
    CHECK_INT(rng_t::tempering_u, 11);
    CHECK_INT(rng_t::tempering_d, 0xffffffff);
    CHECK_INT(rng_t::tempering_s, 7);
    CHECK_INT(rng_t::tempering_b, 0x9d2c5680);
    CHECK_INT(rng_t::tempering_t, 15);
    CHECK_INT(rng_t::tempering_c, 0xefc60000);
    CHECK_INT(rng_t::tempering_l, 18);
    CHECK_INT(rng_t::initialization_multiplier, 1812433253);
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

    rng.seed(1);
    CHECK_INT(rng.min(), 0);
    CHECK_INT(rng.max(), 0xffffffff);
    CHECK_INT(rng(), 1791095845);

    rng.seed(2);
    CHECK_INT(rng.min(), 0);
    CHECK_INT(rng.max(), 0xffffffff);
    CHECK_INT(rng(), 1872583848);

    test_globals<rng_t>::test();
}

static void tmt19937() {
    typedef STD mt19937 rng_t;
    CHECK_INT(rng_t::word_size, 32);
    CHECK_INT(rng_t::state_size, 624);
    CHECK_INT(rng_t::shift_size, 397);
    CHECK_INT(rng_t::mask_bits, 31);
    CHECK_INT((int) rng_t::xor_mask, (int) 0x9908b0df);
    CHECK_INT(rng_t::tempering_u, 11);
    CHECK_INT((int) rng_t::tempering_d, (int) 0xffffffff);
    CHECK_INT(rng_t::tempering_s, 7);
    CHECK_INT((int) rng_t::tempering_b, (int) 0x9d2c5680);
    CHECK_INT(rng_t::tempering_t, 15);
    CHECK_INT((int) rng_t::tempering_c, (int) 0xefc60000);
    CHECK_INT(rng_t::tempering_l, 18);
    CHECK_INT(rng_t::initialization_multiplier, 1812433253);
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
    typedef STD subtract_with_carry_engine<Uint32, 24, 10, 24> rng_t;
    CHECK_INT(rng_t::word_size, 24);
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

    rng.seed(1);
    CHECK_INT(rng.min(), 0);
    CHECK_INT(rng.max(), (1 << 24) - 1);
    CHECK_INT(rng(), 8871692);

    rng.seed(2);
    CHECK_INT(rng.min(), 0);
    CHECK_INT(rng.max(), (1 << 24) - 1);
    CHECK_INT(rng(), 966168);

    test_globals<rng_t>::test();
}

static void tdiscard() {
    int i;
    typedef STD subtract_with_carry_engine<Uint32, 24, 10, 24> rng_base_t;
    typedef STD discard_block_engine<rng_base_t, 223, 24> rng_t;
    CHECK_INT(rng_t::block_size, 223);
    CHECK_INT(rng_t::used_block, 24);
    bool st = STD is_same<rng_t::result_type, Uint32>::value;
    CHECK(st);

    rng_t rng;
    CHECK_INT(rng.min(), 0);
    CHECK_INT(rng.max(), (1 << 24) - 1);
    CHECK_INT(rng(), 15039276);

    rng_base_t rng4;
    rng_t rng5;
    for (i = 0; i < static_cast<int>(rng_t::used_block); ++i) {
        CHECK_INT(rng4(), rng5());
    }
    CHECK(rng4() != rng5());
    for (; i < static_cast<int>(rng_t::block_size); ++i) {
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
    tdiscard();
}
