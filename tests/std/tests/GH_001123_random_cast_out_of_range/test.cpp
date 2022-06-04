// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cmath>
#include <cstdint>
#include <limits>
#include <random>

#pragma warning(disable : 4984) // if constexpr is a C++17 language extension
#ifdef __clang__
#pragma clang diagnostic ignored "-Wc++17-extensions"
#endif // __clang__

template <class T>
using lim = std::numeric_limits<T>;

template <class IntType, class FltType>
void CheckUpperBound(IntType i, FltType fmax) {
    const auto x{std::_Float_upper_bound<FltType>(i)};
    const auto y{std::nextafter(x, FltType{0})}; // lower bound, <= i

    assert(y < fmax);
    assert(static_cast<IntType>(y) <= i);
    assert(x <= fmax);
    if (x < fmax) {
        assert(static_cast<IntType>(x) > i);
    }
}

template <class IntType, class FltType>
void TestUpperBoundExhaustive() {
    const auto fmax{exp2(static_cast<FltType>(lim<IntType>::digits))};
    IntType i{0};
    do {
        CheckUpperBound(i, fmax);
    } while (++i != IntType{0});
}

template <class T>
constexpr T FillLsb(int n) {
    if (n <= 0) {
        return 0;
    }
    T x{T{1} << (n - 1)};
    return (x - 1) ^ x;
}

template <class IntType, class FltType>
void TestUpperBoundSelective() {
    const auto fmax{exp2(static_cast<FltType>(lim<IntType>::digits))};
    CheckUpperBound(IntType{0}, fmax);
    CheckUpperBound(IntType{1}, fmax);
    CheckUpperBound(lim<IntType>::max(), fmax);

    constexpr int diff{lim<IntType>::digits - lim<FltType>::digits};
    if constexpr (diff > 0) {
        // crossover from ulp < 1 to ulp = 1
        constexpr auto a{FillLsb<IntType>(lim<FltType>::digits - 1)};
        CheckUpperBound(a - 1, fmax);
        CheckUpperBound(a, fmax);

        // crossover from ulp = 1 to ulp > 1
        constexpr auto b{FillLsb<IntType>(lim<FltType>::digits)};
        CheckUpperBound(b, fmax);
        CheckUpperBound(b + 1, fmax);
        CheckUpperBound(b + 2, fmax);

        // saturation at the largest representable IntType
        constexpr auto c{FillLsb<IntType>(lim<FltType>::digits) << diff};
        CheckUpperBound(c - 1, fmax);
        CheckUpperBound(c, fmax);
        CheckUpperBound(c + 1, fmax);
    }
}

int main() {
    TestUpperBoundExhaustive<std::uint8_t, float>();
    TestUpperBoundExhaustive<std::uint16_t, float>();
    TestUpperBoundSelective<std::uint32_t, float>();

    TestUpperBoundExhaustive<unsigned short, double>();
    TestUpperBoundSelective<unsigned int, double>();
    TestUpperBoundSelective<unsigned long, double>();
    TestUpperBoundSelective<unsigned long long, double>();
}
