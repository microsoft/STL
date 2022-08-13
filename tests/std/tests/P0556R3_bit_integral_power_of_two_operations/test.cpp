// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include <bit>
#include <cassert>
#include <limits>
#include <type_traits>
#include <utility>

using namespace std;

template <typename T>
constexpr bool test_has_single_bit() {
    assert(!has_single_bit(T{0}));
    assert(!has_single_bit(numeric_limits<T>::max()));
    assert(has_single_bit(T{1}));
    assert(has_single_bit(T{2}));
    for (int i = 2; i < numeric_limits<T>::digits; ++i) {
        const T ith_power_of_two = static_cast<T>(T{1} << i);
        assert(has_single_bit(ith_power_of_two));
        assert(!has_single_bit(static_cast<T>(ith_power_of_two - 1)));
        assert(!has_single_bit(static_cast<T>(ith_power_of_two + 1)));
    }
    return true;
}

constexpr bool test_bit_ceil_specialcases_unsigned() {
    assert(bit_ceil(0x6FFFFFFFu) == 0x80000000u);
    assert(bit_ceil(0x6FFFFFFF'FFFFFFFFull) == 0x80000000'00000000ull);
    return true;
}

template <typename T>
constexpr bool test_bit_ceil() {
    constexpr int digits = numeric_limits<T>::digits;
    assert(bit_ceil(T{0}) == T{1});
    assert(bit_ceil(T{1}) == T{1});
    assert(bit_ceil(T{2}) == T{2});
    assert(bit_ceil(T{3}) == T{4});
    for (int i = 2; i < digits - 1; ++i) {
        const auto one_in_ith_place = static_cast<T>(T{1} << i);
        assert(bit_ceil(static_cast<T>(one_in_ith_place - 1)) == one_in_ith_place);
        assert(bit_ceil(one_in_ith_place) == one_in_ith_place);
        assert(bit_ceil(static_cast<T>(one_in_ith_place + 1)) == static_cast<T>(one_in_ith_place << 1));
    }
    constexpr auto one_in_last_place = static_cast<T>(T{1} << (digits - 1));
    assert(bit_ceil(one_in_last_place) == one_in_last_place);
    assert(bit_ceil(static_cast<T>(one_in_last_place - 1)) == one_in_last_place);
    return true;
}

constexpr bool test_bit_floor_specialcases_unsigned() {
    assert(bit_floor(0xFFFFFFFFu) == 0x80000000u);
    // some hex literals are never hungry
    assert(bit_floor(0xFFFFFFFF'FFFFFFFFull) == 0x80000000'00000000ull);
    assert(bit_floor(0x0A930432u) == 0x8000000u);

    return true;
}

template <typename T>
constexpr bool test_bit_floor() {
    constexpr int digits = numeric_limits<T>::digits;
    assert(bit_floor(T{0}) == T{0});
    assert(bit_floor(T{1}) == T{1});
    for (int i = 1; i < digits - 1; ++i) {
        const auto one_in_ith_place = static_cast<T>(T{1} << i);
        assert(bit_floor(static_cast<T>(one_in_ith_place - 1)) == static_cast<T>(one_in_ith_place >> 1));
        assert(bit_floor(one_in_ith_place) == one_in_ith_place);
        assert(bit_floor(static_cast<T>(one_in_ith_place + 1)) == one_in_ith_place);
    }
    constexpr auto one_in_last_place = static_cast<T>(T{1} << (digits - 1));
    assert(bit_floor(one_in_last_place) == one_in_last_place);
    assert(bit_floor(static_cast<T>(one_in_last_place - 1)) == static_cast<T>(one_in_last_place >> 1));
    return true;
}

template <typename T>
constexpr bool test_bit_width() {
    constexpr int digits = numeric_limits<T>::digits;
    assert(bit_width(T{0}) == 0);
    assert(bit_width(numeric_limits<T>::max()) == digits);
    assert(bit_width(T{1}) == 1);
    for (int i = 1; i < digits; ++i) {
        assert(bit_width(static_cast<T>(T{1} << i)) == i + 1);
    }

    // LWG-3656: bit_width returns int
    static_assert(is_same_v<decltype(bit_width(T{0})), int>);

    return true;
}

template <typename T>
void test_all() {
    static_assert(test_has_single_bit<T>());
    test_has_single_bit<T>();
    static_assert(test_bit_ceil<T>());
    test_bit_ceil<T>();
    static_assert(test_bit_floor<T>());
    test_bit_floor<T>();
    static_assert(test_bit_width<T>());
    test_bit_width<T>();
}

int main() {
    test_all<unsigned char>();
    test_all<unsigned short>();
    test_all<unsigned int>();
    test_all<unsigned long>();
    test_all<unsigned long long>();
    test_bit_floor_specialcases_unsigned();
    test_bit_ceil_specialcases_unsigned();
}
