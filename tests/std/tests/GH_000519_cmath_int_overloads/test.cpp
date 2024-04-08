// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cmath>
#include <math.h>
#include <type_traits>

// Also test the partial implementation of P0533R9 "constexpr For <cmath> And <cstdlib>"
#if _HAS_CXX23
#define CONSTEXPR23 constexpr
#else // ^^^ _HAS_CXX23 / !_HAS_CXX23 vvv
#define CONSTEXPR23 inline
#endif // ^^^ !_HAS_CXX23 ^^^

CONSTEXPR23 void test_bool_overloads() {
    // test overloads in std

    assert(std::fpclassify(false) == FP_ZERO);
    assert(std::fpclassify(true) == FP_NORMAL);

    assert(std::isfinite(false));
    assert(std::isfinite(true));

    assert(!std::isinf(false));
    assert(!std::isinf(true));

    assert(!std::isnan(false));
    assert(!std::isnan(true));

    assert(!std::isnormal(false));
    assert(std::isnormal(true));

    assert(!std::signbit(false));
    assert(!std::signbit(true));

    // test overloads in the global namespace

    assert(::fpclassify(false) == FP_ZERO);
    assert(::fpclassify(true) == FP_NORMAL);

    assert(::isfinite(false));
    assert(::isfinite(true));

    assert(!::isinf(false));
    assert(!::isinf(true));

    assert(!::isnan(false));
    assert(!::isnan(true));

    assert(!::isnormal(false));
    assert(::isnormal(true));

    assert(!::signbit(false));
    assert(!::signbit(true));
}

template <class I>
CONSTEXPR23 void test_other_integral_overloads() {
    // test overloads in std

    assert(std::fpclassify(I{}) == FP_ZERO);
    assert(std::fpclassify(static_cast<I>(42)) == FP_NORMAL);
    assert(std::fpclassify(static_cast<I>(-42)) == FP_NORMAL);

    assert(std::isfinite(I{}));
    assert(std::isfinite(static_cast<I>(42)));
    assert(std::isfinite(static_cast<I>(-42)));

    assert(!std::isinf(I{}));
    assert(!std::isinf(static_cast<I>(42)));
    assert(!std::isinf(static_cast<I>(-42)));

    assert(!std::isnan(I{}));
    assert(!std::isnan(static_cast<I>(42)));
    assert(!std::isnan(static_cast<I>(-42)));

    assert(!std::isnormal(I{}));
    assert(std::isnormal(static_cast<I>(42)));
    assert(std::isnormal(static_cast<I>(-42)));

    assert(!std::signbit(I{}));
    assert(!std::signbit(static_cast<I>(42)));
    assert(std::signbit(static_cast<I>(-42)) == std::is_signed_v<I>);

    // test overloads in the global namespace

    assert(::fpclassify(I{}) == FP_ZERO);
    assert(::fpclassify(static_cast<I>(42)) == FP_NORMAL);
    assert(::fpclassify(static_cast<I>(-42)) == FP_NORMAL);

    assert(::isfinite(I{}));
    assert(::isfinite(static_cast<I>(42)));
    assert(::isfinite(static_cast<I>(-42)));

    assert(!::isinf(I{}));
    assert(!::isinf(static_cast<I>(42)));
    assert(!::isinf(static_cast<I>(-42)));

    assert(!::isnan(I{}));
    assert(!::isnan(static_cast<I>(42)));
    assert(!::isnan(static_cast<I>(-42)));

    assert(!::isnormal(I{}));
    assert(::isnormal(static_cast<I>(42)));
    assert(::isnormal(static_cast<I>(-42)));

    assert(!::signbit(I{}));
    assert(!::signbit(static_cast<I>(42)));
    assert(::signbit(static_cast<I>(-42)) == std::is_signed_v<I>);
}

CONSTEXPR23 bool test_all_integral_overloads() {
    test_bool_overloads();
    test_other_integral_overloads<signed char>();
    test_other_integral_overloads<unsigned char>();
    test_other_integral_overloads<short>();
    test_other_integral_overloads<unsigned short>();
    test_other_integral_overloads<int>();
    test_other_integral_overloads<unsigned int>();
    test_other_integral_overloads<long>();
    test_other_integral_overloads<unsigned long>();
    test_other_integral_overloads<long long>();
    test_other_integral_overloads<unsigned long long>();
    test_other_integral_overloads<char>();
#ifdef __cpp_char8_t
    test_other_integral_overloads<char8_t>();
#endif // defined(__cpp_char8_t)
    test_other_integral_overloads<char16_t>();
    test_other_integral_overloads<char32_t>();
    test_other_integral_overloads<wchar_t>();

    return true;
}

#if _HAS_CXX23
static_assert(test_all_integral_overloads());
#endif // _HAS_CXX23

int main() {
    test_all_integral_overloads();
}
