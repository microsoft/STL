// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <bitset>
#define TEST_NAME "<bitset>"

#include "tdefs.h"
#include <bitset>
#include <sstream>
#include <string>

void test_main() { // test basic workings of bitset definitions
    STD bitset<5> x50;
    STD bitset<5> x51(0xf);
    STD bitset<5> x52(STD string("xx10101ab"), 2, 5);
    STD bitset<5> x53(x52);
    STD string str;

    CHECK_INT(x50.to_ulong(), 0x00);
    CHECK_INT(x51.to_ulong(), 0x0f);
    CHECK_INT(x52.to_ulong(), 0x15);
    CHECK_INT(x53.to_ulong(), 0x15);

    // test arithmetic
    x50 |= x51;
    CHECK_INT(x50.to_ulong(), 0x0f);
    x50 ^= x52;
    CHECK_INT(x50.to_ulong(), 0x1a);
    x50 &= x51;
    CHECK_INT(x50.to_ulong(), 0x0a);
    x50 <<= 2;
    CHECK_INT(x50.to_ulong(), 0x08);
    x50 >>= 3;
    CHECK_INT(x50.to_ulong(), 0x01);
    x50.set(2);
    CHECK_INT(x50.to_ulong(), 0x05);
    x50.set(0, 0);
    CHECK_INT(x50.to_ulong(), 0x04);
    x50.set();
    CHECK_INT(x50.to_ulong(), 0x1f);
    x50.reset(3);
    CHECK_INT(x50.to_ulong(), 0x17);
    x50.reset();
    CHECK_INT(x50.to_ulong(), 0x00);
    x50.flip(2);
    CHECK_INT(x50.to_ulong(), 0x04);
    x50.flip();
    CHECK_INT(x50.to_ulong(), 0x1b);

    str = x50.to_string<char, STD char_traits<char>, STD allocator<char>>();
    CHECK_STR(str.c_str(), "11011");

    CHECK_INT(x50.count(), 4);
    CHECK_INT(x52.count(), 3);
    CHECK_INT(x50.size(), 5);
    CHECK_INT(x51.size(), 5);
    CHECK(x50 == x50);
    CHECK(x50 != x51);
    CHECK(x50.test(1));
    CHECK(!x50.test(2));
    CHECK(x50.any());
    CHECK(!x50.none());
    x50.reset();
    CHECK(!x50.any());
    CHECK(x50.none());

    // test friend arithmetic functions
    STD string sx1a("11010");
    STD bitset<5> bx05(0x05);
    STD bitset<5> bx1f(0x1f);
    STD bitset<5> bx1a(sx1a);

    x50 = x51;
    CHECK_INT((x50 << 2).to_ulong(), 0x1c);
    CHECK_INT((x50 >> 2).to_ulong(), 0x03);
    CHECK((x50 & x52) == bx05);
    CHECK((x50 | x52) == bx1f);
    CHECK((x50 ^ x52) == bx1a);

    {
        STD bitset<5> x54("11111");
        CHECK(!x51.all());
        CHECK(x54.all());
        CHECK_INT((int) x54.to_ullong(), 0x1f);

        STD hash<STD bitset<5>> hasher;
        CHECK_INT(hasher(x54), hasher(x54));
    }

    // test I/O
    STD istringstream ins("1 0101 11000");
    STD ostringstream outs;
    ins >> x50;
    CHECK_INT(x50.to_ulong(), 0x01);
    outs << x50 << ' ';
    ins >> x50;
    CHECK_INT(x50.to_ulong(), 0x05);
    outs << x50 << ' ';
    ins >> x50;
    CHECK_INT(x50.to_ulong(), 0x18);
    outs << x50;
    CHECK_STR(outs.str().c_str(), "00001 00101 11000");

    // test larger bitset
    STD bitset<153> x153a;
    STD bitset<153> x153b;

    (x153a.set(100) >>= 99) <<= 1;
    CHECK_INT(x153a.to_ulong(), 0x04);
    x153b.set(121);
    CHECK(x153b.any());
    CHECK(!x153b.test(105));
    CHECK(x153b.test(121));
    CHECK_INT(x153b.count(), 1);
    CHECK(((x153a | x153b) ^ x153b) == x153a);
}
