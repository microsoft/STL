// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include <assert.h>
#include <bit>
#include <limits>

using namespace std;

template <typename T>
constexpr bool test_countl_zero() {
    constexpr int digits = numeric_limits<T>::digits;
    assert(countl_zero(T{0}) == digits);
    assert(countl_zero(numeric_limits<T>::max()) == 0);
    assert(countl_zero(T{1}) == digits - 1);
    for (int i = 1; i < digits; ++i) {
        const auto one_in_ith_place = static_cast<T>(T{1} << i);
        assert(countl_zero(one_in_ith_place) == digits - i - 1);
        assert(countl_zero(static_cast<T>(one_in_ith_place - 1)) == digits - i);
        assert(countl_zero(static_cast<T>(one_in_ith_place + 1)) == digits - i - 1);
    }
    return true;
}

template <typename T>
constexpr bool test_countr_zero() {
    constexpr int digits = numeric_limits<T>::digits;
    assert(countr_zero(T{0}) == digits);
    assert(countr_zero(numeric_limits<T>::max()) == 0);
    assert(countr_zero(T{1}) == 0);
    for (int i = 1; i < digits; ++i) {
        const auto one_in_ith_place = static_cast<T>(T{1} << i);
        assert(countr_zero(one_in_ith_place) == i);
        assert(countr_zero(static_cast<T>(one_in_ith_place + 1)) == 0);
        assert(countr_zero(static_cast<T>(one_in_ith_place - 1)) == 0);
    }
    return true;
}

template <typename T>
constexpr bool test_countl_one() {
    constexpr int digits = numeric_limits<T>::digits;
    assert(countl_one(numeric_limits<T>::max()) == digits);
    assert(countl_one(T{0}) == 0);
    assert(countl_one(static_cast<T>(~T{1})) == digits - 1);
    assert(countl_one(static_cast<T>(~T{1} + 1)) == digits);
    for (int i = 1; i < digits; ++i) {
        const auto zero_in_ith_place = static_cast<T>(~(T{1} << i));
        assert(countl_one(zero_in_ith_place) == digits - i - 1);
        assert(countl_one(static_cast<T>(zero_in_ith_place + 1)) == digits - i);
        assert(countl_one(static_cast<T>(zero_in_ith_place - 1)) == digits - i - 1);
    }
    return true;
}

template <typename T>
constexpr bool test_countr_one() {
    constexpr int digits = numeric_limits<T>::digits;
    assert(countr_one(T{0}) == 0);
    assert(countr_one(numeric_limits<T>::max()) == digits);
    assert(countr_one(static_cast<T>(~T{1})) == 0);
    for (int i = 1; i < digits; ++i) {
        const auto zero_in_ith_place = static_cast<T>(~(T{1} << i));
        assert(countr_one(zero_in_ith_place) == i);
        assert(countr_one(static_cast<T>(zero_in_ith_place - 1)) == 0);
        assert(countr_one(static_cast<T>(zero_in_ith_place + 1)) == 0);
    }
    return true;
}

template <typename T>
constexpr bool test_popcount() {
    constexpr int digits = numeric_limits<T>::digits;
    assert(popcount(T{0}) == 0);
    assert(popcount(numeric_limits<T>::max()) == digits);
    assert(popcount(T{1}) == 1);
    for (int i = 1; i < digits; ++i) {
        const auto one_in_ith_place = static_cast<T>(T{1} << i);
        assert(popcount(one_in_ith_place) == 1);
        assert(popcount(static_cast<T>(one_in_ith_place - 1)) == i);
        assert(popcount(static_cast<T>(one_in_ith_place + 1)) == 2);
    }
    return true;
}

template <typename T>
constexpr bool test_rotl() {
    constexpr int digits  = numeric_limits<T>::digits;
    constexpr auto maxval = numeric_limits<T>::max();

    constexpr T every_fourth[4] = {
        static_cast<T>(0x1111'1111'1111'1111ULL),
        static_cast<T>(0x2222'2222'2222'2222ULL),
        static_cast<T>(0x4444'4444'4444'4444ULL),
        static_cast<T>(0x8888'8888'8888'8888ULL),
    };

    for (int i = 0; i < digits * 2; ++i) {
        assert(rotl(maxval, i) == maxval);
        assert(rotl(T{0}, i) == 0);

        for (int j = 0; j < 4; ++j) {
            assert(rotl(every_fourth[j], i) == every_fourth[(i + j) % 4]);
        }
    }
    return true;
}

template <typename T>
constexpr bool test_rotr() {
    constexpr int digits  = numeric_limits<T>::digits;
    constexpr auto maxval = numeric_limits<T>::max();

    constexpr T every_fourth[4] = {
        static_cast<T>(0x1111'1111'1111'1111ULL),
        static_cast<T>(0x2222'2222'2222'2222ULL),
        static_cast<T>(0x4444'4444'4444'4444ULL),
        static_cast<T>(0x8888'8888'8888'8888ULL),
    };

    for (int i = 0; i < digits * 2; ++i) {
        assert(rotr(maxval, i) == maxval);
        assert(rotr(T{0}, i) == 0);

        for (int j = 0; j < 4; ++j) {
            assert(rotr(every_fourth[(i + j) % 4], i) == every_fourth[j]);
        }
    }
    return true;
}

// Tests functions for 64-bit operands that have either high or low halves as zero.
// These may be split into two operations on 32-bit platforms and we need to check
// if we handle the == zero or == ones case correctly.
constexpr bool test_64bit_split_ops() {
    constexpr unsigned long long zero_one = 0x0000'0000'FFFF'FFFF;
    constexpr unsigned long long one_zero = 0xFFFF'FFFF'0000'0000;
    assert(popcount(zero_one) == 32);
    assert(popcount(one_zero) == 32);
    assert(countr_zero(zero_one) == 0);
    assert(countr_zero(one_zero) == 32);
    assert(countl_zero(zero_one) == 32);
    assert(countl_zero(one_zero) == 0);
    assert(countr_one(zero_one) == 32);
    assert(countr_one(one_zero) == 0);
    assert(countl_one(zero_one) == 0);
    assert(countl_one(one_zero) == 32);
    return true;
}

template <typename T>
constexpr bool test_popcount_specialcases() {
    constexpr int digits = numeric_limits<T>::digits;
    assert(popcount(T{0x0}) == 0);
    assert(popcount(T{0xFF}) == 8);
    assert(popcount(T{0xAA}) == 4);
    assert(popcount(T{0x80}) == 1);
    assert(popcount(T{0x55}) == 4);

    if constexpr (digits >= 16) {
        assert(popcount(T{0xFFFF}) == 16);
        assert(popcount(T{0xAAAA}) == 8);
        assert(popcount(T{0x8000}) == 1);
        assert(popcount(T{0x5555}) == 8);
    }

    if constexpr (digits >= 32) {
        assert(popcount(T{0xFFFFFFFF}) == 32);
        assert(popcount(T{0xAAAAAAAA}) == 16);
        assert(popcount(T{0x80000000}) == 1);
        assert(popcount(T{0x55555555}) == 16);
    }

    if constexpr (digits >= 64) {
        assert(popcount(T{0xFFFFFFFF'FFFFFFFF}) == 64);
        assert(popcount(T{0xAAAAAAAA'AAAAAAAA}) == 32);
        assert(popcount(T{0x80000000'00000000}) == 1);
        assert(popcount(T{0x55555555'55555555}) == 32);
    }
    return true;
}

template <typename T>
void test_all() {
    static_assert(test_countl_zero<T>());
    test_countl_zero<T>();
    static_assert(test_countr_zero<T>());
    test_countr_zero<T>();
    static_assert(test_countl_one<T>());
    test_countl_one<T>();
    static_assert(test_countr_one<T>());
    test_countr_one<T>();
    static_assert(test_popcount<T>());
    test_popcount<T>();
    static_assert(test_rotl<T>());
    test_rotl<T>();
    static_assert(test_rotr<T>());
    test_rotr<T>();
    static_assert(test_64bit_split_ops());
    test_64bit_split_ops();
    static_assert(test_popcount_specialcases<T>());
    test_popcount_specialcases<T>();
}

int main() {
    test_all<unsigned char>();
    test_all<unsigned short>();
    test_all<unsigned int>();
    test_all<unsigned long>();
    test_all<unsigned long long>();
}
