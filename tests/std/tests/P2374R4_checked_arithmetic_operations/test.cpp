// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstdint>
#include <limits>
#include <ranges>
#include <utility>

using namespace std;

// Check MSVC-STL internal machinery

template <class T>
constexpr void check_add_overflow() {
    constexpr bool is_T_signed = numeric_limits<T>::is_signed;
    T out;
    const T minval = numeric_limits<T>::min();
    const T maxval = numeric_limits<T>::max();

    assert(!_Add_overflow(T{0}, T{0}, out) && out == T{0});
    assert(!_Add_overflow(T{1}, T{1}, out) && out == T{2});
    assert(!_Add_overflow(minval, T{0}, out) && out == minval);
    assert(!_Add_overflow(maxval, T{0}, out) && out == maxval);
    assert(!_Add_overflow(minval, maxval, out) && out == static_cast<T>(-1));
    assert(_Add_overflow(maxval, T{1}, out));
    assert(_Add_overflow(static_cast<T>(maxval / 2), maxval, out));
    assert(!_Add_overflow(static_cast<T>(maxval / 2), static_cast<T>(maxval / 2), out)
           && out == static_cast<T>(maxval - 1));
    assert(_Add_overflow(maxval, maxval, out));

    if constexpr (is_T_signed) {
        assert(!_Add_overflow(T{1}, T{-1}, out) && out == T{0});
        assert(_Add_overflow(minval, T{-1}, out));
        assert(_Add_overflow(minval, minval, out));
        assert(_Add_overflow(T{100}, T{28}, out) == (sizeof(T) == 1));
    } else {
        assert(_Add_overflow(T{200}, T{56}, out) == (sizeof(T) == 1));
    }
}

template <class T>
constexpr void check_mul_overflow() {
    constexpr bool is_T_signed = numeric_limits<T>::is_signed;
    T out;
    const T minval = numeric_limits<T>::min();
    const T maxval = numeric_limits<T>::max();

    assert(!_Mul_overflow(T{0}, T{0}, out) && out == T{0});
    assert(!_Mul_overflow(T{1}, T{1}, out) && out == T{1});
    assert(!_Mul_overflow(minval, T{0}, out) && out == T{0});
    assert(!_Mul_overflow(minval, T{1}, out) && out == minval);
    assert(!_Mul_overflow(maxval, T{0}, out) && out == T{0});
    assert(!_Mul_overflow(maxval, T{1}, out) && out == maxval);
    assert(_Mul_overflow(maxval, maxval, out));
    assert(_Mul_overflow(minval, maxval, out) == is_T_signed);
    assert(_Mul_overflow(minval, minval, out) == is_T_signed);
    assert(!_Mul_overflow(static_cast<T>(maxval / 2), T{2}, out) && out == static_cast<T>(maxval - 1));
    assert(_Mul_overflow(T{0x77}, T{0x78}, out) == (sizeof(T) == 1));

    if constexpr (is_T_signed) {
        assert(_Mul_overflow(minval, T{-1}, out));
        assert(!_Mul_overflow(maxval, T{-1}, out) && out == minval + 1);
    }
}

template <class T>
constexpr void check_add_and_mul() {
    check_add_overflow<T>();
    check_mul_overflow<T>();
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
