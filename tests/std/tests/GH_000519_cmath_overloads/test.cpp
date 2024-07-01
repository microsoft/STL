// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cmath>
#include <limits>
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

#if _HAS_CXX23 // TRANSITION, UCRT should implement P0533R9 "constexpr For <cmath> And <cstdlib>"
    if (!std::is_constant_evaluated())
#endif // ^^^ _HAS_CXX23 ^^^
    {
        assert(!std::isgreater(false, true));
        assert(!std::isgreaterequal(false, true));
        assert(std::isless(false, true));
        assert(std::islessequal(false, true));
        assert(std::islessgreater(false, true));
        assert(!std::isunordered(false, true));
    }

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

#if _HAS_CXX23 // TRANSITION, UCRT should implement P0533R9 "constexpr For <cmath> And <cstdlib>"
    if (!std::is_constant_evaluated())
#endif // ^^^ _HAS_CXX23 ^^^
    {
        assert(!::isgreater(false, true));
        assert(!::isgreaterequal(false, true));
        assert(::isless(false, true));
        assert(::islessequal(false, true));
        assert(::islessgreater(false, true));
        assert(!::isunordered(false, true));
    }
}

template <class I>
CONSTEXPR23 void test_other_integral_overloads() {
    constexpr I imax                    = std::numeric_limits<I>::max();
    constexpr I imaxm1                  = static_cast<I>(imax - 1);
    constexpr bool narrower_than_double = std::numeric_limits<I>::digits < std::numeric_limits<double>::digits;

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

#if _HAS_CXX23 // TRANSITION, UCRT should implement P0533R9 "constexpr For <cmath> And <cstdlib>"
    if (!std::is_constant_evaluated())
#endif // ^^^ _HAS_CXX23 ^^^
    {
        assert(!std::isgreater(static_cast<I>(17), static_cast<I>(29)));
        assert(!std::isgreaterequal(static_cast<I>(17), static_cast<I>(29)));
        assert(std::isless(static_cast<I>(17), static_cast<I>(29)));
        assert(std::islessequal(static_cast<I>(17), static_cast<I>(29)));
        assert(std::islessgreater(static_cast<I>(17), static_cast<I>(29)));
        assert(!std::isunordered(static_cast<I>(17), static_cast<I>(29)));

        // test that integers are converted to double
        assert(std::isgreater(imax, imaxm1) == narrower_than_double);
        assert(std::isgreaterequal(imaxm1, imax) == !narrower_than_double);
        assert(std::isless(imaxm1, imax) == narrower_than_double);
        assert(std::islessequal(imax, imaxm1) == !narrower_than_double);
        assert(std::islessgreater(imax, imaxm1) == narrower_than_double);
        assert(!std::isunordered(imax, imaxm1));
    }

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

#if _HAS_CXX23 // TRANSITION, UCRT should implement P0533R9 "constexpr For <cmath> And <cstdlib>"
    if (!std::is_constant_evaluated())
#endif // ^^^ _HAS_CXX23 ^^^
    {
        assert(!::isgreater(static_cast<I>(17), static_cast<I>(29)));
        assert(!::isgreaterequal(static_cast<I>(17), static_cast<I>(29)));
        assert(::isless(static_cast<I>(17), static_cast<I>(29)));
        assert(::islessequal(static_cast<I>(17), static_cast<I>(29)));
        assert(::islessgreater(static_cast<I>(17), static_cast<I>(29)));
        assert(!::isunordered(static_cast<I>(17), static_cast<I>(29)));

        // test that integers are converted to double
        assert(::isgreater(imax, imaxm1) == narrower_than_double);
        assert(::isgreaterequal(imaxm1, imax) == !narrower_than_double);
        assert(::isless(imaxm1, imax) == narrower_than_double);
        assert(::islessequal(imax, imaxm1) == !narrower_than_double);
        assert(::islessgreater(imax, imaxm1) == narrower_than_double);
        assert(!::isunordered(imax, imaxm1));
    }
}

template <class F>
CONSTEXPR23 void test_floating_overloads() {
    constexpr F zero = 0;
    constexpr F inf  = std::numeric_limits<F>::infinity();
    constexpr F nan  = std::numeric_limits<F>::quiet_NaN();
    constexpr F lhs  = static_cast<F>(3.14);
    constexpr F rhs  = static_cast<F>(17.29);

    // test overloads in std

#if _HAS_CXX23 && !defined(__clang__) // TRANSITION, UCRT should implement P0533R9 "constexpr For <cmath> And <cstdlib>"
    if (!std::is_constant_evaluated())
#endif // ^^^ _HAS_CXX23 && !defined(__clang__) ^^^
    {
        assert(std::isfinite(zero));
        assert(!std::isfinite(inf));
        assert(!std::isfinite(nan));
        assert(std::isfinite(lhs));

        assert(!std::isinf(zero));
        assert(std::isinf(inf));
        assert(!std::isinf(nan));
        assert(!std::isinf(lhs));

        assert(!std::isnan(zero));
        assert(!std::isnan(inf));
        assert(std::isnan(nan));
        assert(!std::isnan(lhs));

        assert(!std::isnormal(zero));
        assert(!std::isnormal(inf));
        assert(!std::isnormal(nan));
        assert(std::isnormal(lhs));
    }

#if _HAS_CXX23 // TRANSITION, UCRT should implement P0533R9 "constexpr For <cmath> And <cstdlib>"
    if (!std::is_constant_evaluated())
#endif // ^^^ _HAS_CXX23 ^^^
    {
        assert(!std::isgreater(lhs, rhs));
        assert(!std::isgreaterequal(lhs, rhs));
        assert(std::isless(lhs, rhs));
        assert(std::islessequal(lhs, rhs));
        assert(std::islessgreater(lhs, rhs));
        assert(!std::isunordered(lhs, rhs));
    }

    // test overloads in the global namespace

#if _HAS_CXX23 && !defined(__clang__) // TRANSITION, UCRT should implement P0533R9 "constexpr For <cmath> And <cstdlib>"
    if (!std::is_constant_evaluated())
#endif // ^^^ _HAS_CXX23 && !defined(__clang__) ^^^
    {
        assert(::isfinite(zero));
        assert(!::isfinite(inf));
        assert(!::isfinite(nan));
        assert(::isfinite(lhs));

        assert(!::isinf(zero));
        assert(::isinf(inf));
        assert(!::isinf(nan));
        assert(!::isinf(lhs));

        assert(!::isnan(zero));
        assert(!::isnan(inf));
        assert(::isnan(nan));
        assert(!::isnan(lhs));

        assert(!::isnormal(zero));
        assert(!::isnormal(inf));
        assert(!::isnormal(nan));
        assert(::isnormal(lhs));
    }

#if _HAS_CXX23 // TRANSITION, UCRT should implement P0533R9 "constexpr For <cmath> And <cstdlib>"
    if (!std::is_constant_evaluated())
#endif // ^^^ _HAS_CXX23 ^^^
    {
        assert(!::isgreater(lhs, rhs));
        assert(!::isgreaterequal(lhs, rhs));
        assert(::isless(lhs, rhs));
        assert(::islessequal(lhs, rhs));
        assert(::islessgreater(lhs, rhs));
        assert(!::isunordered(lhs, rhs));
    }
}

CONSTEXPR23 bool test_all_overloads() {
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

    test_floating_overloads<float>();
    test_floating_overloads<double>();
    test_floating_overloads<long double>();

    return true;
}

#if _HAS_CXX23
static_assert(test_all_overloads());
#endif // _HAS_CXX23

int main() {
    test_all_overloads();
}
