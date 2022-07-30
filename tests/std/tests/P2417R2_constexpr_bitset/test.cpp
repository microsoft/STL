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
    }

    // [bitset.cons], constructors
    {
        bitset<5> x53(string("xx10101ab"), 2, 5);
        assert(x53.to_ulong() == 0x15);
        bitset<5> x54("xoxox", 5, 'o', 'x');
        assert(x54.to_ulong() == 0x15);
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
        assert(x50.none());

        x50 = x51;
        assert((x50 << 2).to_ulong() == 0x1c);
        assert((x50 >> 2).to_ulong() == 0x03);
    }

    return true;
}

int main() {
    static_assert(test());
    assert(test());
}
