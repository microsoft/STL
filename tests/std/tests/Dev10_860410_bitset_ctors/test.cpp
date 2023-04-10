// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <bitset>
#include <cassert>
#include <cstdlib>
#include <stdexcept>
#include <string>

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

const char parsedStr[] = "1000110111110011110111111111111111010111110111100101010100001001"
                         "1111111111111111111111111111111111111111111111111111111111111111"
                         "0111111111111111111111111111111111111111111111111111111111111111"
                         "1111111111111111111111111111111100000000000000000000000000000000";

struct isZero {
    bool operator()(const char c) const noexcept {
        return c == '0';
    }
};

void test_DevDiv917456();
void test_DevDiv931383();
void test_VSO_742338();
void test_Getword();

int main() {
    {
        bitset<5> b;

        assert(b.to_string() == "00000");
    }


    {
        bitset<5> b(23);

        assert(b.to_string() == "10111");
    }

    {
        bitset<5> b(29UL);

        assert(b.to_string() == "11101");
    }

    {
        bitset<5> b(25ULL);

        assert(b.to_string() == "11001");
    }


    {
        wstring w(L"10011");

        bitset<5> b(w);

        assert(b.to_string() == "10011");
    }

    {
        wstring w(L"zzz01000");

        bitset<5> b(w, 3);

        assert(b.to_string() == "01000");
    }

    {
        wstring w(L"zzz00010zzz");

        bitset<5> b(w, 3, 5);

        assert(b.to_string() == "00010");
    }

    {
        wstring w(L"zzz.11..zzz");

        bitset<5> b(w, 3, 5, L'.');

        assert(b.to_string() == "01100");
    }

    {
        wstring w(L"zzz..!!.zzz");

        bitset<5> b(w, 3, 5, L'.', L'!');

        assert(b.to_string() == "00110");
    }


    {
        bitset<5> b(L"10011");

        assert(b.to_string() == "10011");
    }

    {
        const wchar_t* const ptr = L"zzz01000";

        bitset<5> b(ptr + 3);

        assert(b.to_string() == "01000");
    }

    {
        const wchar_t* const ptr = L"zzz00010zzz";

        bitset<5> b(ptr + 3, 5);

        assert(b.to_string() == "00010");
    }

    {
        const wchar_t* const ptr = L"zzz.11..zzz";

        bitset<5> b(ptr + 3, 5, L'.');

        assert(b.to_string() == "01100");
    }

    {
        const wchar_t* const ptr = L"zzz..!!.zzz";

        bitset<5> b(ptr + 3, 5, L'.', L'!');

        assert(b.to_string() == "00110");
    }

    test_DevDiv917456();
    test_DevDiv931383();
    test_VSO_742338();
    test_Getword();
}

// Also test DevDiv-917456 "<bitset>: none() and any() return incorrect count after call set() function in a
// std::bitset<0> object [libcxx]".
void test_bitset0(const bitset<0>& b) {
    assert(b.to_ulong() == 0);
    assert(b.to_ullong() == 0);
    assert(b.to_string() == "");
    assert(b.count() == 0);
    assert(b.all());
    assert(!b.any());
    assert(b.none());
}

void test_DevDiv917456() {
    {
        bitset<0> b;
        test_bitset0(b);
    }

    {
        bitset<0> b;
        b.set();
        test_bitset0(b);
    }

    {
        bitset<0> b;
        b.reset();
        test_bitset0(b);
    }

    {
        bitset<0> b;
        b.flip();
        test_bitset0(b);
    }

    {
        bitset<0> b(0xFFFFFFFFFFFFFFFFULL);
        test_bitset0(b);
    }
}

// Also test DevDiv-931383 "<bitset>: We need to validate all characters and use traits::eq()".
void test(const string& str, const size_t pos, const size_t n, const string& expected) noexcept {
    try {
        bitset<8> b(str, pos, n, 'o', 'i');
        assert(b.to_string() == expected);
    } catch (const invalid_argument&) {
        assert(expected == "invalid_argument");
    } catch (const out_of_range&) {
        assert(expected == "out_of_range");
    }
}

void test_DevDiv931383() {
    // N3936 20.6.1 [bitset.cons]/4: "Throws: out_of_range if pos > str.size()."
    test("AAAA", 5, 0, "out_of_range");

    // /5: "Effects: Determines the effective length rlen of the initializing string
    // as the smaller of n and str.size() - pos. The function then throws invalid_argument
    // if any of the rlen characters in str beginning at position pos is other than zero or one."
    test("oiooiiioBBB", 0, string::npos, "invalid_argument");

    // "Otherwise, the function constructs an object of class bitset<N>, initializing
    // the first M bit positions to values determined from the corresponding characters
    // in the string str. M is the smaller of N and rlen."
    test("oiiiiiooiiiiCCC", 0, 12, "01111100");
    test("DDDoiooiiioEEE", 3, 8, "01001110");

    // /6-7: "Character position pos + M - 1 corresponds to bit position zero.
    // Subsequent decreasing character positions correspond to increasing bit positions.
    // If M < N, remaining bit positions are initialized to zero."
    test("FFiioiiiGG", 2, 6, "00110111");
}

// Also test Dev10-479284 "C6326 when running static analysis with <bitset>".
template class std::bitset<7>;
template class std::bitset<32>;

template class std::bitset<47>;
template class std::bitset<64>;

template class std::bitset<1729>;
template class std::bitset<2048>;

template <size_t bitsetBits>
void test_VSO_742338_case(size_t useLength) {
    // This tests for a case where a bitset which needs multiple words internally reversed the order of those words.
    const bitset<bitsetBits> bits(parsedStr, useLength);
    auto asStr          = bits.to_string();
    const auto division = asStr.begin() + static_cast<ptrdiff_t>(bitsetBits - useLength);
    assert(all_of(asStr.begin(), division, isZero{}));
    assert(equal(division, asStr.end(), parsedStr));
    reverse(asStr.begin(), asStr.end());
    for (size_t idx = 0; idx < useLength; ++idx) {
        assert(bits[idx] == (asStr[idx] == '1'));
    }
    for (size_t idx = useLength; idx < bitsetBits; ++idx) {
        assert(!bits[idx]);
    }
}

void test_VSO_742338() {
    for (size_t idx = 0; idx <= 25; ++idx) {
        test_VSO_742338_case<25>(idx);
    }

    for (size_t idx = 0; idx <= 256; ++idx) {
        test_VSO_742338_case<256>(idx);
    }
}

void test_Getword() {
    // Tests nonstandard extension _Getword which we provide for performance-sensitive scenarios where DirectX needed
    // access to the array inside the bitset.
    STATIC_ASSERT(is_same_v<unsigned long, decltype(bitset<16>{}._Getword(0))>);
    STATIC_ASSERT(is_same_v<unsigned long, decltype(bitset<32>{}._Getword(0))>);
    STATIC_ASSERT(is_same_v<unsigned long long, decltype(bitset<33>{}._Getword(0))>);
    STATIC_ASSERT(is_same_v<unsigned long long, decltype(bitset<64>{}._Getword(0))>);
    STATIC_ASSERT(is_same_v<unsigned long long, decltype(bitset<65>{}._Getword(0))>);
    const bitset<16> bits16(parsedStr);
    assert(bits16._Getword(0) == 36339ul);
    const bitset<256> bits256(parsedStr);
    assert(bits256._Getword(0) == 18446744069414584320ull);
    assert(bits256._Getword(1) == 9223372036854775807ull);
    assert(bits256._Getword(2) == 18446744073709551615ull);
    assert(bits256._Getword(3) == 10228765468619855113ull);
}
