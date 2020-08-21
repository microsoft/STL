// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cmath>
#include <cstdint>
#include <limits>
#include <random>

template <class T>
using lim = std::numeric_limits<T>;

template <class T>
void CheckUpperBound(T i, float fmax) {
    const float x{std::_Float_upper_bound<float>(i)};
    const float y{std::nextafter(x, 0.0f)}; // lower bound, <= i

    assert(y < fmax);
    assert(static_cast<T>(y) <= i);
    assert(x <= fmax);
    if (x < fmax) {
        assert(static_cast<T>(x) > i);
    }
}

template <class T>
void TestUpperBoundExhaustive() {
    const float fmax{exp2(static_cast<float>(lim<T>::digits))};
    T i{0};
    do {
        CheckUpperBound(i, fmax);
    } while (++i != T{0});
}

template <class T>
constexpr T FillLsb(int n) {
    if (n <= 0) {
        return 0;
    }
    T x{T{1} << (n - 1)};
    return (x - 1) ^ x;
}

template <class T>
void TestUpperBoundSelective() {
    const float fmax{exp2(static_cast<float>(lim<T>::digits))};
    CheckUpperBound(T{0}, fmax);
    CheckUpperBound(T{1}, fmax);
    CheckUpperBound(lim<T>::max(), fmax);

    // crossover from ulp < 1 to ulp = 1
    constexpr T a{FillLsb<T>(lim<float>::digits - 1)};
    CheckUpperBound(a - 1, fmax);
    CheckUpperBound(a, fmax);

    // crossover from ulp = 1 to ulp > 1
    constexpr T b{FillLsb<T>(lim<float>::digits)};
    CheckUpperBound(b, fmax);
    CheckUpperBound(b + 1, fmax);
    CheckUpperBound(b + 2, fmax);

    // saturation at the largest representable T
    constexpr int diff{lim<T>::digits - lim<float>::digits};
    constexpr T c{FillLsb<T>(lim<float>::digits) << diff};
    CheckUpperBound(c - 1, fmax);
    CheckUpperBound(c, fmax);
    CheckUpperBound(c + 1, fmax);
}

int main() {
    TestUpperBoundExhaustive<std::uint8_t>();
    TestUpperBoundExhaustive<std::uint16_t>();
    TestUpperBoundSelective<std::uint32_t>();
}
