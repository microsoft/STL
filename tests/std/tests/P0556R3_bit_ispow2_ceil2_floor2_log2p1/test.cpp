// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include <assert.h>
#include <bit>
#include <limits>
#include <utility>

using namespace std;

#ifdef __cpp_lib_bitops // TRANSITION, VSO-1020212
template <typename T>
constexpr bool test_ispow2() {
    assert(!ispow2(T{0}));
    assert(!ispow2(numeric_limits<T>::max()));
    assert(ispow2(T{1}));
    assert(ispow2(T{2}));
    for (int i = 2; i < numeric_limits<T>::digits; ++i) {
        const T ith_power_of_two = static_cast<T>(T{1} << i);
        assert(ispow2(ith_power_of_two));
        assert(!ispow2(static_cast<T>(ith_power_of_two - 1)));
        assert(!ispow2(static_cast<T>(ith_power_of_two + 1)));
    }
    return true;
}

constexpr bool test_ceil2_specialcases_unsigned() {
    assert(ceil2(0x6FFFFFFFu) == 0x80000000u);
    assert(ceil2(0x6FFFFFFF'FFFFFFFFull) == 0x80000000'00000000ull);
    return true;
}

template <typename T>
constexpr bool test_ceil2() {
    constexpr int digits = numeric_limits<T>::digits;
    assert(ceil2(T{0}) == T{1});
    assert(ceil2(T{1}) == T{1});
    assert(ceil2(T{2}) == T{2});
    assert(ceil2(T{3}) == T{4});
    for (int i = 2; i < digits - 1; ++i) {
        const auto one_in_ith_place = static_cast<T>(T{1} << i);
        assert(ceil2(static_cast<T>(one_in_ith_place - 1)) == one_in_ith_place);
        assert(ceil2(one_in_ith_place) == one_in_ith_place);
        assert(ceil2(static_cast<T>(one_in_ith_place + 1)) == static_cast<T>(one_in_ith_place << 1));
    }
    constexpr auto one_in_last_place = static_cast<T>(T{1} << (digits - 1));
    assert(ceil2(one_in_last_place) == one_in_last_place);
    assert(ceil2(static_cast<T>(one_in_last_place - 1)) == one_in_last_place);
    return true;
}

constexpr bool test_floor2_specialcases_unsigned() {
    assert(floor2(0xFFFFFFFFu) == 0x80000000u);
    // some hex literals are never hungry
    assert(floor2(0xFFFFFFFF'FFFFFFFFull) == 0x80000000'00000000ull);
    assert(floor2(0x0A930432u) == 0x8000000u);

    return true;
}

template <typename T>
constexpr bool test_floor2() {
    constexpr int digits = numeric_limits<T>::digits;
    assert(floor2(T{0}) == T{0});
    assert(floor2(T{1}) == T{1});
    for (int i = 1; i < digits - 1; ++i) {
        const auto one_in_ith_place = static_cast<T>(T{1} << i);
        assert(floor2(static_cast<T>(one_in_ith_place - 1)) == static_cast<T>(one_in_ith_place >> 1));
        assert(floor2(one_in_ith_place) == one_in_ith_place);
        assert(floor2(static_cast<T>(one_in_ith_place + 1)) == one_in_ith_place);
    }
    constexpr auto one_in_last_place = static_cast<T>(T{1} << (digits - 1));
    assert(floor2(one_in_last_place) == one_in_last_place);
    assert(floor2(static_cast<T>(one_in_last_place - 1)) == static_cast<T>(one_in_last_place >> 1));
    return true;
}

template <typename T>
constexpr bool test_bit_length() {
    constexpr int digits = numeric_limits<T>::digits;
    assert(bit_length(T{0}) == T{0});
    assert(bit_length(numeric_limits<T>::max()) == digits);
    assert(bit_length(T{1}) == T{1});
    for (int i = 1; i < digits; ++i) {
        assert(bit_length(static_cast<T>(T{1} << i)) == static_cast<T>(i + 1));
    }
    return true;
}

template <typename T>
void test_all() {
    static_assert(test_ispow2<T>());
    test_ispow2<T>();
    static_assert(test_ceil2<T>());
    test_ceil2<T>();
    static_assert(test_floor2<T>());
    test_floor2<T>();
    static_assert(test_bit_length<T>());
    test_bit_length<T>();
}
#endif // __cpp_lib_bitops

int main() {
#ifdef __cpp_lib_bitops // TRANSITION, VSO-1020212
    test_all<unsigned char>();
    test_all<unsigned short>();
    test_all<unsigned int>();
    test_all<unsigned long>();
    test_all<unsigned long long>();
    test_floor2_specialcases_unsigned();
    test_ceil2_specialcases_unsigned();
#endif // __cpp_lib_bitops
}
