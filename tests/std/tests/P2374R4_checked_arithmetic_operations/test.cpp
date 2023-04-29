// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstdint>
#include <limits>
#include <ranges>
#include <utility>

using namespace std;

// Check MSVC-STL internal machinery

template <class Int>
constexpr void check_add_overflow() {
    constexpr bool is_int_signed = numeric_limits<Int>::is_signed;
    Int out;
    Int minval = numeric_limits<Int>::min();
    Int maxval = numeric_limits<Int>::max();

    assert(!_Add_overflow(Int{1}, Int{1}, out) && out == Int{2});
    assert(!_Add_overflow(maxval, Int{0}, out) && out == maxval);
    assert(!_Add_overflow(minval, maxval, out) && out == static_cast<Int>(-1));
    assert(_Add_overflow(maxval, Int{1}, out));
    assert(_Add_overflow(static_cast<Int>(maxval / 2), maxval, out));
    assert(!_Add_overflow(static_cast<Int>(maxval / 2), static_cast<Int>(maxval / 2), out));
    assert(_Add_overflow(maxval, maxval, out));

    if constexpr (is_int_signed) {
        assert(!_Add_overflow(Int{1}, Int{-1}, out) && out == Int{0});
        assert(_Add_overflow(minval, Int{-1}, out));
        assert(_Add_overflow(Int{100}, Int{28}, out) == (sizeof(Int) == 1));
    } else {
        assert(_Add_overflow(Int{200}, Int{56}, out) == (sizeof(Int) == 1));
    }
}

template <class Int>
constexpr void check_mul_overflow() {
    constexpr bool is_int_signed = numeric_limits<Int>::is_signed;
    Int out;
    Int minval = numeric_limits<Int>::min();
    Int maxval = numeric_limits<Int>::max();

    assert(!_Mul_overflow(Int{1}, Int{1}, out) && out == Int{1});
    assert(!_Mul_overflow(maxval, Int{0}, out) && out == Int{0});
    assert(!_Mul_overflow(maxval, Int{1}, out) && out == maxval);
    assert(_Mul_overflow(maxval, maxval, out));
    assert(_Mul_overflow(minval, maxval, out) == is_int_signed);
    assert(!_Mul_overflow(static_cast<Int>(maxval / 2), Int{2}, out));
    assert(_Mul_overflow(Int{0x77}, Int{0x78}, out) == (sizeof(Int) == 1));

    if constexpr (is_int_signed) {
        assert(_Mul_overflow(minval, Int{-1}, out));
        assert(!_Mul_overflow(maxval, Int{-1}, out) && out == minval + 1);
    }
}

template <class Int>
constexpr void check_add_and_mul() {
    check_add_overflow<Int>();
    check_mul_overflow<Int>();
}

constexpr bool test() {
    check_add_and_mul<int8_t>();
    check_add_and_mul<uint8_t>();
    check_add_and_mul<int16_t>();
    check_add_and_mul<uint16_t>();
    check_add_and_mul<int32_t>();
    check_add_and_mul<uint32_t>();
    check_add_and_mul<int64_t>();
    check_add_and_mul<uint64_t>();
    check_add_and_mul<_Signed128>();
    check_add_and_mul<_Unsigned128>();
    return true;
}

int main() {
    static_assert(test());
    test();
}
