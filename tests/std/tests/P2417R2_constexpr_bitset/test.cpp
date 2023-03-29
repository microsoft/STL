// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <bitset>
#include <cassert>
#include <string>

using namespace std;

constexpr bool test() {
    bitset<5> x50;
    const bitset<5> x51{0xf};
    const bitset<5> x52{0x15};

    assert(x50.to_ulong() == 0x00);
    assert(x51.to_ulong() == 0x0f);
    assert(x52.to_ulong() == 0x15);

    // bitset operators
    {
        x50 = x50 | x51;
        assert(x50.to_ulong() == 0x0f);
        x50 = x50 ^ x52;
        assert(x50.to_ulong() == 0x1a);
        x50 = x50 & x51;
        assert(x50.to_ulong() == 0x0a);
        x50 = 0x00;
    }

    // bit reference
    {
        bitset<5>::reference ref1 = x50[2];
        assert(~ref1);
        const bitset<5>::reference ref2 = ref1;
        assert(~ref2);
        ref1 = true;
        assert(ref1);
        ref1.flip();
        assert(~ref1);

        x50    = 0x02;
        x50[3] = x50[1];
        assert(x50.to_ulong() == 0x0a);
        x50 = 0x00;
    }

    // [bitset.cons], constructors
    {
        bitset<5> test_str1(string{"10101"});
        bitset<5> test_str2(string{"ab10101"}, 2);
        bitset<5> test_str3(string{"ab10101cd"}, 2, 5);
        bitset<5> test_str4(string{"ab1o1o1cd"}, 2, 5, 'o');
        bitset<5> test_str5(string{"abxoxoxcd"}, 2, 5, 'o', 'x');
        assert(test_str1.to_ulong() == 0x15);
        assert(test_str2.to_ulong() == 0x15);
        assert(test_str3.to_ulong() == 0x15);
        assert(test_str4.to_ulong() == 0x15);
        assert(test_str5.to_ulong() == 0x15);
    }
    {
        bitset<5> test_ptr1("10101");
        bitset<5> test_ptr2("10101abcd", 5);
        bitset<5> test_ptr3("1o1o1abcd", 5, 'o');
        bitset<5> test_ptr4("xoxoxabcd", 5, 'o', 'x');
        assert(test_ptr1.to_ulong() == 0x15);
        assert(test_ptr2.to_ulong() == 0x15);
        assert(test_ptr3.to_ulong() == 0x15);
        assert(test_ptr4.to_ulong() == 0x15);
    }

    // [bitset.members], bitset operations
    {
        x50 |= x51;
        assert(x50.to_ulong() == 0x0f);
        x50 ^= x52;
        assert(x50.to_ulong() == 0x1a);
        x50 &= x51;
        assert(x50.to_ulong() == 0x0a);
        x50 <<= 2;
        assert(x50.to_ulong() == 0x08);
        x50 >>= 3;
        assert(x50.to_ulong() == 0x01);
        x50.set(2);
        assert(x50.to_ulong() == 0x05);
        x50.set(0, false);
        assert(x50.to_ulong() == 0x04);
        x50.set();
        assert(x50.to_ulong() == 0x1f);
        x50.reset(3);
        assert(x50.to_ulong() == 0x17);
        x50.reset();
        assert(x50.to_ulong() == 0x00);
        assert((~x50).to_ulong() == 0x1f);
        assert(x50.to_ulong() == 0x00);
        x50.flip(2);
        assert(x50.to_ulong() == 0x04);
        x50.flip();
        assert(x50.to_ulong() == 0x1b);

        // element access
        bitset<5>::reference ref1 = x50[1];

        assert(x50.to_ulong() == 0x1b);
        assert(x50.to_ullong() == 0x1b);
        assert(x50.to_string() == "11011");
        assert(x50.to_string('o') == "11o11");
        assert(x50.to_string('o', 'x') == "xxoxx");

        assert(x50.count() == 4);
        assert(x50.size() == 5);

        assert(x50 == x50);
        assert(x50 != x51);

        assert(x50.test(1));
        assert(!x50.test(2));
        assert(x50.any());
        assert(!x50.all());
        assert(!x50.none());
        x50.reset();
        assert(!x50.any());
        assert(!x50.all());
        assert(x50.none());
        x50.flip();
        assert(x50.any());
        assert(x50.all());
        assert(!x50.none());

        x50 = x51;
        assert((x50 << 2).to_ulong() == 0x1c);
        assert((x50 >> 2).to_ulong() == 0x03);
    }

    {
        // bitset<150> stores two full 64-bit words and one partial 64-bit word.

        // Test to_ulong() and to_ullong() for large bitsets:
        bitset<150> big1{0x1234'5678};
        assert(big1.to_ulong() == 0x1234'5678);
        assert(big1.to_ullong() == 0x1234'5678);

        bitset<150> big2{0x1234'5678'90ab'cdef};
        assert(big2.to_ullong() == 0x1234'5678'90ab'cdef);

        // Test is_constant_evaluated() codepaths:
        assert(!big1.none());
        assert(big1.any());
        assert(!big1.all());

        big1.set();
        assert(!big1.none());
        assert(big1.any());
        assert(big1.all());

        big1.reset();
        assert(big1.none());
        assert(!big1.any());
        assert(!big1.all());

        // Test operator==() by setting bits in each word:
        big2.reset();
        assert(big1 == big2);

        big1[3] = true;
        assert(big1 != big2);
        big2[3] = true;
        assert(big1 == big2);

        big1[70] = true;
        assert(big1 != big2);
        big2[70] = true;
        assert(big1 == big2);

        big1[145] = true;
        assert(big1 != big2);
        big2[145] = true;
        assert(big1 == big2);
    }

    return true;
}

int main() {
    static_assert(test());
    assert(test());
}
