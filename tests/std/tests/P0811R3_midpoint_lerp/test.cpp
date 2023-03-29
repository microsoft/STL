// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <bit>
#include <cassert>
#include <cfenv>
#include <charconv>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iterator>
#include <limits>
#include <numeric>
#include <optional>
#include <type_traits>

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

template <typename Ty>
using limits = numeric_limits<Ty>;

#ifdef _M_FP_STRICT
// According to:
// https://docs.microsoft.com/en-us/cpp/build/reference/fp-specify-floating-point-behavior
// Under the default /fp:precise mode:
//  The compiler generates code intended to run in the default floating-point environment and assumes that the
//  floating-point environment is not accessed or modified at runtime.
// ... so we only do testing of rounding modes and floating-point exceptions when strict is enabled.

// TRANSITION, VSO-923474 -- should be #pragma STDC FENV_ACCESS ON
#pragma fenv_access(on)

void checked_fesetround(const int round) {
    [[maybe_unused]] const int setRound = fesetround(round);
    assert(setRound == 0);
}

class RoundGuard {
public:
    explicit RoundGuard(const int newRound) : oldRound(fegetround()) {
        checked_fesetround(newRound);
    }

    RoundGuard(const RoundGuard&)            = delete;
    RoundGuard& operator=(const RoundGuard&) = delete;

    ~RoundGuard() {
        checked_fesetround(oldRound);
    }

private:
    int oldRound;
};

void checked_feholdexcept(fenv_t* const env) {
    [[maybe_unused]] const int holdExcept = feholdexcept(env);
    assert(holdExcept == 0);
}

void checked_fesetenv(const fenv_t* const env) {
    [[maybe_unused]] const int setEnv = fesetenv(env);
    assert(setEnv == 0);
}

class ExceptGuard {
public:
    ExceptGuard() {
        checked_feholdexcept(&env);
    }

    ExceptGuard(const ExceptGuard&)            = delete;
    ExceptGuard& operator=(const ExceptGuard&) = delete;

    ~ExceptGuard() {
        checked_fesetenv(&env);
    }

private:
    fenv_t env;
};

constexpr int fe_invalid  = FE_INVALID;
constexpr int fe_overflow = FE_OVERFLOW;

// "major" floating point exceptions, excluding underflow and inexact
constexpr int fe_major_except = FE_INVALID | FE_DIVBYZERO | FE_OVERFLOW;

bool check_feexcept(const int expected_excepts, const int except_mask = fe_major_except) {
    return fetestexcept(except_mask) == (expected_excepts & except_mask);
}
#else // ^^^ defined(_M_FP_STRICT) / !defined(_M_FP_STRICT) vvv
class ExceptGuard {
public:
    ExceptGuard() {}

    ExceptGuard(const ExceptGuard&)            = delete;
    ExceptGuard& operator=(const ExceptGuard&) = delete;

    ~ExceptGuard() {}
};

// These values are ignored. (FE_INVALID and FE_OVERFLOW aren't available for /clr.)
constexpr int fe_invalid  = 0;
constexpr int fe_overflow = 0;

bool check_feexcept([[maybe_unused]] const int expected_excepts, [[maybe_unused]] const int except_mask = 0) {
    return true;
}
#endif // ^^^ !defined(_M_FP_STRICT) ^^^

template <class Ty>
constexpr Ty mint_nan(const bool sign, const unsigned long long payload);

template <>
constexpr float mint_nan<float>(const bool sign, const unsigned long long payload) {
    const unsigned int filteredPayload = payload & 0x3F'FFFFu; // bottom 22 bits
    assert(filteredPayload == payload); // if this assert fails, payload didn't fit

    const unsigned int result = (static_cast<unsigned int>(sign) << 31)
                              | 0x7FC0'0000u // turn on all exponent bits and the qNaN bit
                              | filteredPayload;

    return bit_cast<float>(result);
}

template <>
constexpr double mint_nan<double>(const bool sign, const unsigned long long payload) {
    const unsigned long long filteredPayload = payload & 0x7'FFFF'FFFF'FFFFllu; // bottom 51 bits
    assert(filteredPayload == payload); // if this assert fails, payload didn't fit

    const unsigned long long result = (static_cast<unsigned long long>(sign) << 63)
                                    | 0x7FF8'0000'0000'0000u // turn on all exponent bits and the qNaN bit
                                    | filteredPayload;

    return bit_cast<double>(result);
}

template <>
constexpr long double mint_nan<long double>(const bool sign, const unsigned long long payload) {
    return mint_nan<double>(sign, payload);
}

template <typename Ty>
void assert_bitwise_equal(const Ty& a, const Ty& b) {
    assert(memcmp(&a, &b, sizeof(Ty)) == 0);
}

// TRANSITION
// numeric_limits<T>::signaling_NaN() doesn't work on x86 hosted MSVC
// numeric_limits<float>::signaling_NaN() doesn't work on x64 hosted MSVC
void make_snan(float& x) {
    constexpr unsigned int bits = 0x7f80'0001U;
    memcpy(&x, &bits, sizeof(x));
}

void make_snan(double& x) {
    constexpr unsigned long long bits = 0x7ff0'0000'0000'0001ULL;
    memcpy(&x, &bits, sizeof(x));
}

void make_snan(long double& x) {
    constexpr unsigned long long bits = 0x7ff0'0000'0000'0001ULL;
    memcpy(&x, &bits, sizeof(x));
}

template <typename Ty>
struct constants; // not defined

template <>
struct constants<float> {
    static constexpr float TwoPlusUlp        = 0x1.000002p+1f;
    static constexpr float OnePlusUlp        = 0x1.000002p+0f;
    static constexpr float PointFivePlusUlp  = 0x1.000002p-1f;
    static constexpr float OneMinusUlp       = 0x1.fffffep-1f;
    static constexpr float PointFiveMinusUlp = 0x1.fffffep-2f;
    static constexpr float NegOneMinusUlp    = -OnePlusUlp;
    static constexpr float NegOnePlusUlp     = -OneMinusUlp;

    static constexpr float EighthPlusUlp  = 0x1.000002p-3f;
    static constexpr float EighthMinusUlp = 0x1.fffffep-4f;
};

template <>
struct constants<double> {
    static constexpr double TwoPlusUlp        = 0x1.0000000000001p+1;
    static constexpr double OnePlusUlp        = 0x1.0000000000001p+0;
    static constexpr double PointFivePlusUlp  = 0x1.0000000000001p-1;
    static constexpr double OneMinusUlp       = 0x1.fffffffffffffp-1;
    static constexpr double PointFiveMinusUlp = 0x1.fffffffffffffp-2;
    static constexpr double NegOneMinusUlp    = -OnePlusUlp;
    static constexpr double NegOnePlusUlp     = -OneMinusUlp;

    static constexpr double EighthPlusUlp  = 0x1.0000000000001p-3;
    static constexpr double EighthMinusUlp = 0x1.fffffffffffffp-4;
};

template <>
struct constants<long double> {
    static constexpr long double TwoPlusUlp        = 0x1.0000000000001p+1;
    static constexpr long double OnePlusUlp        = 0x1.0000000000001p+0;
    static constexpr long double PointFivePlusUlp  = 0x1.0000000000001p-1;
    static constexpr long double OneMinusUlp       = 0x1.fffffffffffffp-1;
    static constexpr long double PointFiveMinusUlp = 0x1.fffffffffffffp-2;
    static constexpr long double NegOneMinusUlp    = -OnePlusUlp;
    static constexpr long double NegOnePlusUlp     = -OneMinusUlp;

    static constexpr long double EighthPlusUlp  = 0x1.0000000000001p-3;
    static constexpr long double EighthMinusUlp = 0x1.fffffffffffffp-4;
};

template <typename Ty>
void test_constants() {
    assert(constants<Ty>::TwoPlusUlp == nextafter(Ty(2.0), Ty(3.0)));
    assert(constants<Ty>::OnePlusUlp == nextafter(Ty(1.0), Ty(3.0)));
    assert(constants<Ty>::PointFivePlusUlp == nextafter(Ty(0.5), Ty(3.0)));
    assert(constants<Ty>::OneMinusUlp == nextafter(Ty(1.0), Ty(0.0)));
    assert(constants<Ty>::PointFiveMinusUlp == nextafter(Ty(0.5), Ty(-2.0)));
    assert(constants<Ty>::NegOneMinusUlp == nextafter(Ty(-1.0), Ty(-2.0)));
    assert(constants<Ty>::NegOnePlusUlp == nextafter(Ty(-1.0), Ty(0.0)));

    assert(constants<Ty>::EighthPlusUlp == nextafter(Ty(0.125), Ty(1.0)));
    assert(constants<Ty>::EighthMinusUlp == nextafter(Ty(0.125), Ty(0.0)));
}

template <typename Ty>
constexpr bool test_midpoint_integer_signed() {
    STATIC_ASSERT(is_same_v<decltype(midpoint(Ty(), Ty())), decltype(Ty())>);
    STATIC_ASSERT(is_signed_v<Ty>);
    STATIC_ASSERT(noexcept(midpoint(Ty(), Ty())));

    assert(midpoint(Ty(1), Ty(3)) == Ty(2));
    assert(midpoint(Ty(3), Ty(1)) == Ty(2));

    assert(midpoint(Ty(0), Ty(0)) == Ty(0));
    assert(midpoint(Ty(0), Ty(2)) == Ty(1));
    assert(midpoint(Ty(2), Ty(0)) == Ty(1));
    assert(midpoint(Ty(2), Ty(2)) == Ty(2));

    assert(midpoint(Ty(1), Ty(4)) == Ty(2));
    assert(midpoint(Ty(4), Ty(1)) == Ty(3));
    assert(midpoint(Ty(3), Ty(4)) == Ty(3));
    assert(midpoint(Ty(4), Ty(3)) == Ty(4));

    assert(midpoint(Ty(3), Ty(4)) == Ty(3));
    assert(midpoint(Ty(4), Ty(3)) == Ty(4));
    assert(midpoint(Ty(-3), Ty(4)) == Ty(0));
    assert(midpoint(Ty(-4), Ty(3)) == Ty(-1));
    assert(midpoint(Ty(3), Ty(-4)) == Ty(0));
    assert(midpoint(Ty(4), Ty(-3)) == Ty(1));
    assert(midpoint(Ty(-3), Ty(-4)) == Ty(-3));
    assert(midpoint(Ty(-4), Ty(-3)) == Ty(-4));


    // test that we round toward the left parameter for integers
    assert(midpoint(limits<Ty>::min(), limits<Ty>::max()) == Ty(-1));
    assert(midpoint(limits<Ty>::max(), limits<Ty>::min()) == Ty(0));

    assert(midpoint(limits<Ty>::min(), limits<Ty>::min()) == limits<Ty>::min());
    assert(midpoint(limits<Ty>::max(), limits<Ty>::max()) == limits<Ty>::max());

    assert(midpoint(limits<Ty>::min(), Ty(6)) == limits<Ty>::min() / 2 + 3);
    assert(midpoint(Ty(6), limits<Ty>::min()) == limits<Ty>::min() / 2 + 3);
    assert(midpoint(limits<Ty>::max(), Ty(6)) == limits<Ty>::max() / 2 + 4);
    assert(midpoint(Ty(6), limits<Ty>::max()) == limits<Ty>::max() / 2 + 3);

    assert(midpoint(limits<Ty>::min(), Ty(-6)) == limits<Ty>::min() / 2 - 3);
    assert(midpoint(Ty(-6), limits<Ty>::min()) == limits<Ty>::min() / 2 - 3);
    assert(midpoint(limits<Ty>::max(), Ty(-6)) == limits<Ty>::max() / 2 - 2);
    assert(midpoint(Ty(-6), limits<Ty>::max()) == limits<Ty>::max() / 2 - 3);

    return true;
}

template <typename Ty>
constexpr bool test_midpoint_integer_unsigned() {
    STATIC_ASSERT(is_same_v<decltype(midpoint(Ty(), Ty())), decltype(Ty())>);
    STATIC_ASSERT(is_unsigned_v<Ty>);
    STATIC_ASSERT(noexcept(midpoint(Ty(), Ty())));

    assert(midpoint(Ty(1), Ty(3)) == Ty(2));
    assert(midpoint(Ty(3), Ty(1)) == Ty(2));

    assert(midpoint(Ty(0), Ty(0)) == Ty(0));
    assert(midpoint(Ty(0), Ty(2)) == Ty(1));
    assert(midpoint(Ty(2), Ty(0)) == Ty(1));
    assert(midpoint(Ty(2), Ty(2)) == Ty(2));

    assert(midpoint(Ty(1), Ty(4)) == Ty(2));
    assert(midpoint(Ty(4), Ty(1)) == Ty(3));
    assert(midpoint(Ty(3), Ty(4)) == Ty(3));
    assert(midpoint(Ty(4), Ty(3)) == Ty(4));

    assert(midpoint(limits<Ty>::min(), limits<Ty>::max()) == (limits<Ty>::max() - limits<Ty>::min()) / 2);
    assert(midpoint(limits<Ty>::max(), limits<Ty>::min()) == (limits<Ty>::max() - limits<Ty>::min()) / 2 + 1);
    assert(midpoint(limits<Ty>::min(), limits<Ty>::min()) == limits<Ty>::min());
    assert(midpoint(limits<Ty>::max(), limits<Ty>::max()) == limits<Ty>::max());

    assert(midpoint(limits<Ty>::min(), Ty(6)) == limits<Ty>::min() / 2 + 3);
    assert(midpoint(Ty(6), limits<Ty>::min()) == limits<Ty>::min() / 2 + 3);
    assert(midpoint(limits<Ty>::max(), Ty(6)) == (limits<Ty>::max() - limits<Ty>::min()) / 2 + 4);
    assert(midpoint(Ty(6), limits<Ty>::max()) == (limits<Ty>::max() - limits<Ty>::min()) / 2 + 3);

    return true;
}

template <typename Ty>
constexpr bool test_midpoint_floating_constexpr() {
    STATIC_ASSERT(is_same_v<decltype(midpoint(Ty(), Ty())), decltype(Ty())>);
    STATIC_ASSERT(is_signed_v<Ty>);
    STATIC_ASSERT(noexcept(midpoint(Ty(), Ty())));

    assert(midpoint(Ty(0.5), Ty(0.5)) == Ty(0.5));
    assert(midpoint(Ty(0.5), Ty(2.5)) == Ty(1.5));

    assert(midpoint(Ty(3.0), Ty(4.0)) == Ty(3.5));
    assert(midpoint(Ty(-3.0), Ty(4.0)) == Ty(0.5));
    assert(midpoint(Ty(3.0), Ty(-4.0)) == Ty(-0.5));
    assert(midpoint(Ty(-3.0), Ty(-4.0)) == Ty(-3.5));

    assert(midpoint(Ty(-0.0), Ty(-0.0)) == Ty(-0.0));
    assert(midpoint(Ty(0.0), Ty(0.0)) == Ty(0.0));
    assert(midpoint(Ty(-5.0), Ty(5.0)) == Ty(0.0));

    // float: midpoint(-0x1.0000000000000p-149, 0x1.0000000000000p-149) = 0x0.0000000000000p+0
    // double: midpoint(-0x0.0000000000001p-1022, 0x0.0000000000001p-1022) = 0x0.0000000000000p+0
    assert(midpoint(-limits<Ty>::denorm_min(), limits<Ty>::denorm_min()) == Ty(0.0));

    // float: midpoint(0x1.fffffe0000000p-4, 0x1.0000020000000p-3) = 0x1.0000000000000p-3
    // double: midpoint(0x1.fffffffffffffp-4, 0x1.0000000000001p-3) = 0x1.0000000000000p-3
    assert(midpoint(constants<Ty>::EighthMinusUlp, constants<Ty>::EighthPlusUlp) == Ty(0.125));

    // float: midpoint(-0x1.0000020000000p-3, -0x1.fffffe0000000p-4) = -0x1.0000000000000p-3
    // double: midpoint(-0x1.0000000000001p-3, -0x1.fffffffffffffp-4) = -0x1.0000000000000p-3
    assert(midpoint(-constants<Ty>::EighthPlusUlp, -constants<Ty>::EighthMinusUlp) == Ty(-0.125));

    // float: midpoint(0x1.0000000000000p-149, 0x1.0000020000000p+0) = 0x1.0000020000000p-1
    // double: midpoint(0x0.0000000000001p-1022, 0x1.0000000000001p+0) = 0x1.0000000000001p-1
    assert(midpoint(limits<Ty>::denorm_min(), constants<Ty>::OnePlusUlp) == constants<Ty>::PointFivePlusUlp);

    // float: midpoint(-0x1.0000000000000p-149, -0x1.0000020000000p+0) = -0x1.0000020000000p-1
    // double: midpoint(-0x0.0000000000001p-1022, -0x1.0000000000001p+0) = -0x1.0000000000001p-1
    assert(midpoint(-limits<Ty>::denorm_min(), constants<Ty>::NegOneMinusUlp) == -constants<Ty>::PointFivePlusUlp);

    assert(midpoint(Ty(1.0), constants<Ty>::OnePlusUlp) == Ty(1.0));
    assert(midpoint(constants<Ty>::OneMinusUlp, constants<Ty>::OnePlusUlp) == Ty(1.0));
    assert(midpoint(Ty(-1.0), constants<Ty>::NegOneMinusUlp) == Ty(-1.0));
    assert(midpoint(constants<Ty>::NegOneMinusUlp, constants<Ty>::NegOnePlusUlp) == Ty(-1.0));

    // in the default rounding mode, midpoint(huge magnitude, tiny magnitude) returns huge/2 without overflow
    assert(midpoint(limits<Ty>::max(), limits<Ty>::min()) == limits<Ty>::max() / 2);
    assert(midpoint(-limits<Ty>::max(), limits<Ty>::min()) == -limits<Ty>::max() / 2);
    assert(midpoint(limits<Ty>::max(), limits<Ty>::denorm_min()) == limits<Ty>::max() / 2);
    assert(midpoint(-limits<Ty>::max(), limits<Ty>::denorm_min()) == -limits<Ty>::max() / 2);
    assert(midpoint(limits<Ty>::max(), -limits<Ty>::min()) == limits<Ty>::max() / 2);
    assert(midpoint(-limits<Ty>::max(), -limits<Ty>::min()) == -limits<Ty>::max() / 2);
    assert(midpoint(limits<Ty>::max(), -limits<Ty>::denorm_min()) == limits<Ty>::max() / 2);
    assert(midpoint(-limits<Ty>::max(), -limits<Ty>::denorm_min()) == -limits<Ty>::max() / 2);

    // also midpoint(tiny magnitude, huge magnitude)
    assert(midpoint(limits<Ty>::min(), limits<Ty>::max()) == limits<Ty>::max() / 2);
    assert(midpoint(limits<Ty>::min(), -limits<Ty>::max()) == -limits<Ty>::max() / 2);
    assert(midpoint(limits<Ty>::denorm_min(), limits<Ty>::max()) == limits<Ty>::max() / 2);
    assert(midpoint(limits<Ty>::denorm_min(), -limits<Ty>::max()) == -limits<Ty>::max() / 2);
    assert(midpoint(-limits<Ty>::min(), limits<Ty>::max()) == limits<Ty>::max() / 2);
    assert(midpoint(-limits<Ty>::min(), -limits<Ty>::max()) == -limits<Ty>::max() / 2);
    assert(midpoint(-limits<Ty>::denorm_min(), limits<Ty>::max()) == limits<Ty>::max() / 2);
    assert(midpoint(-limits<Ty>::denorm_min(), -limits<Ty>::max()) == -limits<Ty>::max() / 2);

    assert(midpoint(limits<Ty>::lowest(), limits<Ty>::max()) == Ty(0));
    assert(midpoint(limits<Ty>::max(), limits<Ty>::max()) == limits<Ty>::max());
    assert(midpoint(limits<Ty>::lowest(), limits<Ty>::lowest()) == limits<Ty>::lowest());

    assert(midpoint(limits<Ty>::lowest(), Ty(5.0)) == limits<Ty>::lowest() / Ty(2.0) + Ty(2.5));
    assert(midpoint(limits<Ty>::max(), Ty(5.0)) == limits<Ty>::max() / Ty(2.0) + Ty(2.5));

    assert(midpoint(limits<Ty>::lowest(), Ty(-5.0)) == limits<Ty>::lowest() / Ty(2.0) + Ty(-2.5));
    assert(midpoint(limits<Ty>::max(), Ty(-5.0)) == limits<Ty>::max() / Ty(2.0) + Ty(-2.5));

    assert(midpoint(limits<Ty>::infinity(), limits<Ty>::infinity()) == limits<Ty>::infinity());
    assert(midpoint(-limits<Ty>::infinity(), -limits<Ty>::infinity()) == -limits<Ty>::infinity());

    const auto bigPositive = limits<Ty>::max() / Ty(8.0) * Ty(7.0);
    const auto bigNegative = -limits<Ty>::max() / Ty(4.0) * Ty(3.0);
    assert(midpoint(bigPositive, bigNegative) == (bigPositive + bigNegative) / Ty(2.0));

    return true;
}

template <typename Ty>
void test_midpoint_floating() {
#ifdef _M_FP_STRICT
    {
        // test results exactly between 1 ULP:
        ExceptGuard except;
        RoundGuard round{FE_UPWARD};
        assert(midpoint(Ty(1.0), constants<Ty>::OnePlusUlp) == constants<Ty>::OnePlusUlp);
        assert(midpoint(Ty(1.0), constants<Ty>::OneMinusUlp) == Ty(1.0));
        assert(midpoint(Ty(-1.0), constants<Ty>::NegOnePlusUlp) == constants<Ty>::NegOnePlusUlp);
        assert(midpoint(Ty(-1.0), constants<Ty>::NegOneMinusUlp) == Ty(-1.0));

        // in upward round mode, adding even much less than an ULP moves up by 1 ULP
        assert(midpoint(limits<Ty>::max(), limits<Ty>::min()) == nextafter(limits<Ty>::max() / 2, limits<Ty>::max()));
        assert(midpoint(-limits<Ty>::max(), limits<Ty>::min()) == nextafter(-limits<Ty>::max() / 2, limits<Ty>::max()));
        assert(midpoint(limits<Ty>::max(), limits<Ty>::denorm_min())
               == nextafter(limits<Ty>::max() / 2, limits<Ty>::max()));
        assert(midpoint(-limits<Ty>::max(), limits<Ty>::denorm_min())
               == nextafter(-limits<Ty>::max() / 2, limits<Ty>::max()));
        // and adding a negative tiny value does nothing:
        assert(midpoint(limits<Ty>::max(), -limits<Ty>::min()) == limits<Ty>::max() / 2);
        assert(midpoint(-limits<Ty>::max(), -limits<Ty>::min()) == -limits<Ty>::max() / 2);
        assert(midpoint(limits<Ty>::max(), -limits<Ty>::denorm_min()) == limits<Ty>::max() / 2);
        assert(midpoint(-limits<Ty>::max(), -limits<Ty>::denorm_min()) == -limits<Ty>::max() / 2);

        // also midpoint(tiny magnitude, huge magnitude)
        assert(midpoint(limits<Ty>::min(), limits<Ty>::max()) == nextafter(limits<Ty>::max() / 2, limits<Ty>::max()));
        assert(midpoint(limits<Ty>::min(), -limits<Ty>::max()) == nextafter(-limits<Ty>::max() / 2, limits<Ty>::max()));
        assert(midpoint(limits<Ty>::denorm_min(), limits<Ty>::max())
               == nextafter(limits<Ty>::max() / 2, limits<Ty>::max()));
        assert(midpoint(limits<Ty>::denorm_min(), -limits<Ty>::max())
               == nextafter(-limits<Ty>::max() / 2, limits<Ty>::max()));
        assert(midpoint(-limits<Ty>::min(), limits<Ty>::max()) == limits<Ty>::max() / 2);
        assert(midpoint(-limits<Ty>::min(), -limits<Ty>::max()) == -limits<Ty>::max() / 2);
        assert(midpoint(-limits<Ty>::denorm_min(), limits<Ty>::max()) == limits<Ty>::max() / 2);
        assert(midpoint(-limits<Ty>::denorm_min(), -limits<Ty>::max()) == -limits<Ty>::max() / 2);

        assert(check_feexcept(0));
    }

    // ditto for the other rounding modes:
    {
        ExceptGuard except;
        RoundGuard round{FE_DOWNWARD};
        assert(midpoint(Ty(1.0), constants<Ty>::OnePlusUlp) == Ty(1.0));
        assert(midpoint(Ty(1.0), constants<Ty>::OneMinusUlp) == constants<Ty>::OneMinusUlp);
        assert(midpoint(Ty(-1.0), constants<Ty>::NegOnePlusUlp) == Ty(-1.0));
        assert(midpoint(Ty(-1.0), constants<Ty>::NegOneMinusUlp) == constants<Ty>::NegOneMinusUlp);

        // in downward round mode, adding a tiny positive value does nothing, just like the default:
        assert(midpoint(limits<Ty>::max(), limits<Ty>::min()) == limits<Ty>::max() / 2);
        assert(midpoint(-limits<Ty>::max(), limits<Ty>::min()) == -limits<Ty>::max() / 2);
        assert(midpoint(limits<Ty>::max(), limits<Ty>::denorm_min()) == limits<Ty>::max() / 2);
        assert(midpoint(-limits<Ty>::max(), limits<Ty>::denorm_min()) == -limits<Ty>::max() / 2);
        // but adding a negative tiny value moves down by 1 ULP:
        assert(
            midpoint(limits<Ty>::max(), -limits<Ty>::min()) == nextafter(limits<Ty>::max() / 2, limits<Ty>::lowest()));
        assert(midpoint(-limits<Ty>::max(), -limits<Ty>::min())
               == nextafter(-limits<Ty>::max() / 2, limits<Ty>::lowest()));
        assert(midpoint(limits<Ty>::max(), -limits<Ty>::denorm_min())
               == nextafter(limits<Ty>::max() / 2, limits<Ty>::lowest()));
        assert(midpoint(-limits<Ty>::max(), -limits<Ty>::denorm_min())
               == nextafter(-limits<Ty>::max() / 2, limits<Ty>::lowest()));

        // also midpoint(tiny magnitude, huge magnitude)
        assert(midpoint(limits<Ty>::min(), limits<Ty>::max()) == limits<Ty>::max() / 2);
        assert(midpoint(limits<Ty>::min(), -limits<Ty>::max()) == -limits<Ty>::max() / 2);
        assert(midpoint(limits<Ty>::denorm_min(), limits<Ty>::max()) == limits<Ty>::max() / 2);
        assert(midpoint(limits<Ty>::denorm_min(), -limits<Ty>::max()) == -limits<Ty>::max() / 2);
        assert(
            midpoint(-limits<Ty>::min(), limits<Ty>::max()) == nextafter(limits<Ty>::max() / 2, limits<Ty>::lowest()));
        assert(midpoint(-limits<Ty>::min(), -limits<Ty>::max())
               == nextafter(-limits<Ty>::max() / 2, limits<Ty>::lowest()));
        assert(midpoint(-limits<Ty>::denorm_min(), limits<Ty>::max())
               == nextafter(limits<Ty>::max() / 2, limits<Ty>::lowest()));
        assert(midpoint(-limits<Ty>::denorm_min(), -limits<Ty>::max())
               == nextafter(-limits<Ty>::max() / 2, limits<Ty>::lowest()));

        assert(check_feexcept(0));
    }

    {
        ExceptGuard except;
        RoundGuard round{FE_TOWARDZERO};
        assert(midpoint(Ty(1.0), constants<Ty>::OnePlusUlp) == Ty(1.0));
        assert(midpoint(Ty(1.0), constants<Ty>::OneMinusUlp) == constants<Ty>::OneMinusUlp);
        assert(midpoint(Ty(-1.0), constants<Ty>::NegOnePlusUlp) == constants<Ty>::NegOnePlusUlp);
        assert(midpoint(Ty(-1.0), constants<Ty>::NegOneMinusUlp) == Ty(-1.0));

        // tiny values closer to 0 move by 1 ULP as appropriate:
        assert(midpoint(limits<Ty>::max(), limits<Ty>::min()) == limits<Ty>::max() / 2);
        assert(midpoint(-limits<Ty>::max(), limits<Ty>::min()) == nextafter(-limits<Ty>::max() / 2, Ty(0)));
        assert(midpoint(limits<Ty>::max(), limits<Ty>::denorm_min()) == limits<Ty>::max() / 2);
        assert(midpoint(-limits<Ty>::max(), limits<Ty>::denorm_min()) == nextafter(-limits<Ty>::max() / 2, Ty(0)));
        assert(midpoint(limits<Ty>::max(), -limits<Ty>::min()) == nextafter(limits<Ty>::max() / 2, Ty(0)));
        assert(midpoint(-limits<Ty>::max(), -limits<Ty>::min()) == -limits<Ty>::max() / 2);
        assert(midpoint(limits<Ty>::max(), -limits<Ty>::denorm_min()) == nextafter(limits<Ty>::max() / 2, Ty(0)));
        assert(midpoint(-limits<Ty>::max(), -limits<Ty>::denorm_min()) == -limits<Ty>::max() / 2);

        // also midpoint(tiny magnitude, huge magnitude)
        assert(midpoint(limits<Ty>::min(), limits<Ty>::max()) == limits<Ty>::max() / 2);
        assert(midpoint(limits<Ty>::min(), -limits<Ty>::max()) == nextafter(-limits<Ty>::max() / 2, Ty(0)));
        assert(midpoint(limits<Ty>::denorm_min(), limits<Ty>::max()) == limits<Ty>::max() / 2);
        assert(midpoint(limits<Ty>::denorm_min(), -limits<Ty>::max()) == nextafter(-limits<Ty>::max() / 2, Ty(0)));
        assert(midpoint(-limits<Ty>::min(), limits<Ty>::max()) == nextafter(limits<Ty>::max() / 2, Ty(0)));
        assert(midpoint(-limits<Ty>::min(), -limits<Ty>::max()) == -limits<Ty>::max() / 2);
        assert(midpoint(-limits<Ty>::denorm_min(), limits<Ty>::max()) == nextafter(limits<Ty>::max() / 2, Ty(0)));
        assert(midpoint(-limits<Ty>::denorm_min(), -limits<Ty>::max()) == -limits<Ty>::max() / 2);

        assert(check_feexcept(0));
    }
#endif // _M_FP_STRICT

    {
        ExceptGuard except;

        assert(midpoint(limits<Ty>::denorm_min(), Ty(1.0)) == (limits<Ty>::denorm_min() + Ty(1.0)) / Ty(2.0));
        assert(midpoint(limits<Ty>::denorm_min(), limits<Ty>::max())
               == (limits<Ty>::denorm_min() + limits<Ty>::max()) / Ty(2.0));
        assert(midpoint(limits<Ty>::denorm_min(), limits<Ty>::lowest())
               == (limits<Ty>::denorm_min() + limits<Ty>::lowest()) / Ty(2.0));
        assert(midpoint(limits<Ty>::denorm_min(), limits<Ty>::infinity()) == limits<Ty>::infinity());
        assert(midpoint(limits<Ty>::denorm_min(), -limits<Ty>::infinity()) == -limits<Ty>::infinity());

#ifndef _M_CEE // TRANSITION, VSO-1666178
        assert_bitwise_equal(mint_nan<Ty>(0, 1), midpoint(mint_nan<Ty>(0, 1), Ty(0)));
        assert_bitwise_equal(mint_nan<Ty>(0, 1), midpoint(Ty(0), mint_nan<Ty>(0, 1)));
        assert_bitwise_equal(mint_nan<Ty>(0, 1), midpoint(mint_nan<Ty>(0, 1), limits<Ty>::max()));
        assert_bitwise_equal(mint_nan<Ty>(0, 1), midpoint(limits<Ty>::max(), mint_nan<Ty>(0, 1)));
        assert_bitwise_equal(mint_nan<Ty>(0, 1), midpoint(mint_nan<Ty>(0, 1), mint_nan<Ty>(0, 1)));
#endif // _M_CEE

        assert(isnan(midpoint(limits<Ty>::quiet_NaN(), Ty(2.0))));
        assert(isnan(midpoint(Ty(2.0), limits<Ty>::quiet_NaN())));
        assert(isnan(midpoint(limits<Ty>::quiet_NaN(), limits<Ty>::quiet_NaN())));

        assert(check_feexcept(0));
    }

    // cases where midpoint() should raise FE_INVALID and return NaN
    constexpr auto test_midpoint_fe_invalid = [](const Ty& a, const Ty& b) {
        ExceptGuard except;
        const auto answer = midpoint(a, b);
        return check_feexcept(fe_invalid) && isnan(answer);
    };

    Ty snan;
    make_snan(snan);

    assert(test_midpoint_fe_invalid(-limits<Ty>::infinity(), limits<Ty>::infinity()));
    assert(test_midpoint_fe_invalid(snan, limits<Ty>::quiet_NaN()));
    assert(test_midpoint_fe_invalid(limits<Ty>::quiet_NaN(), snan));
    assert(test_midpoint_fe_invalid(snan, snan));
}

template <typename Ty>
union U {
    Ty value;
};

template <typename Ty>
class C {
    Ty value;
};

template <typename Ty>
constexpr bool test_midpoint_pointer() {
    STATIC_ASSERT(is_same_v<decltype(midpoint(Ty(), Ty())), decltype(Ty())>);
    STATIC_ASSERT(is_signed_v<Ty>);
    STATIC_ASSERT(noexcept(midpoint(Ty(), Ty())));

    constexpr U<Ty> u[1000] = {};

    assert(midpoint(u, u) == u);
    assert(midpoint(u, u + 1000) == u + 500);

    assert(midpoint(u, u + 9) == u + 4);
    assert(midpoint(u, u + 10) == u + 5);
    assert(midpoint(u, u + 11) == u + 5);
    assert(midpoint(u + 9, u) == u + 5);
    assert(midpoint(u + 10, u) == u + 5);
    assert(midpoint(u + 11, u) == u + 6);

    STATIC_ASSERT(midpoint(u, u + 9) == u + 4);
    STATIC_ASSERT(midpoint(u, u + 10) == u + 5);
    STATIC_ASSERT(midpoint(u, u + 11) == u + 5);
    STATIC_ASSERT(midpoint(u + 9, u) == u + 5);
    STATIC_ASSERT(midpoint(u + 10, u) == u + 5);
    STATIC_ASSERT(midpoint(u + 11, u) == u + 6);

    constexpr C<Ty> c[1000] = {};

    assert(midpoint(c, c) == c);
    assert(midpoint(c, c + 1000) == c + 500);

    assert(midpoint(c, c + 9) == c + 4);
    assert(midpoint(c, c + 10) == c + 5);
    assert(midpoint(c, c + 11) == c + 5);
    assert(midpoint(c + 9, c) == c + 5);
    assert(midpoint(c + 10, c) == c + 5);
    assert(midpoint(c + 11, c) == c + 6);

    return true;
}

template <typename Ty>
constexpr int cmp(const Ty x, const Ty y) {
    if (x > y) {
        return 1;
    } else if (x < y) {
        return -1;
    } else {
        return 0;
    }
}

template <typename Ty>
struct LerpTestCase {
    Ty x;
    Ty y;
    Ty t;
    Ty expected;
};

template <typename Ty>
struct LerpNaNTestCase {
    Ty x;
    Ty y;
    Ty t;
    optional<Ty> expected_list[3] = {};
};

template <typename Ty>
struct LerpCases { // TRANSITION, VSO-934633
    static inline constexpr LerpTestCase<Ty> lerpTestCases[] = {
        {Ty(-1.0), Ty(1.0), Ty(2.0), Ty(3.0)},
        {Ty(0.0), Ty(1.0), Ty(2.0), Ty(2.0)},
        {Ty(-1.0), Ty(0.0), Ty(2.0), Ty(1.0)},

        {Ty(1.0), Ty(-1.0), Ty(2.0), Ty(-3.0)},
        {Ty(0.0), Ty(-1.0), Ty(2.0), Ty(-2.0)},
        {Ty(1.0), Ty(0.0), Ty(2.0), Ty(-1.0)},

        {Ty(1.0), Ty(2.0), Ty(1.0), Ty(2.0)},

        {Ty(1.0), Ty(2.0), Ty(2.0), Ty(3.0)},
        {Ty(1.0), Ty(2.0), Ty(0.5), Ty(1.5)},

        {Ty(1.0), Ty(2.0), Ty(0.0), Ty(1.0)},
        {Ty(1.0), Ty(1.0), Ty(2.0), Ty(1.0)},

        {Ty(-0.0), Ty(-0.0), Ty(0.5), Ty(-0.0)},
        {Ty(0.0), Ty(0.0), Ty(0.5), Ty(0.0)},
        {Ty(-5.0), Ty(5.0), Ty(0.5), Ty(0.0)},

        // float: lerp(-0x1.0000000000000p-149, 0x1.0000000000000p-149, 0.5) = 0x0.0000000000000p+0
        // double: lerp(-0x0.0000000000001p-1022, 0x0.0000000000001p-1022, 0.5) = 0x0.0000000000000p+0
        {-limits<Ty>::denorm_min(), limits<Ty>::denorm_min(), Ty(0.5), Ty(0.0)},

        // float: lerp(0x1.fffffe0000000p-4, 0x1.0000020000000p-3, 0.5) = 0x1.0000000000000p-3
        // double: lerp(0x1.fffffffffffffp-4, 0x1.0000000000001p-3, 0.5) = 0x1.0000000000000p-3
        {constants<Ty>::EighthMinusUlp, constants<Ty>::EighthPlusUlp, Ty(0.5), Ty(0.125)},

        // float: lerp(-0x1.0000020000000p-3, -0x1.fffffe0000000p-4, 0.5) = -0x1.0000000000000p-3
        // double: lerp(-0x1.0000000000001p-3, -0x1.fffffffffffffp-4, 0.5) = -0x1.0000000000000p-3
        {-constants<Ty>::EighthPlusUlp, -constants<Ty>::EighthMinusUlp, Ty(0.5), Ty(-0.125)},

        // float: lerp(0x1.0000000000000p-149, 0x1.0000020000000p+0, 0.5) = 0x1.0000020000000p-1
        // double: lerp(0x0.0000000000001p-1022, 0x1.0000000000001p+0, 0.5) = 0x1.0000000000001p-1
        {limits<Ty>::denorm_min(), constants<Ty>::OnePlusUlp, Ty(0.5), constants<Ty>::PointFivePlusUlp},

        // float: lerp(-0x1.0000000000000p-149, -0x1.0000020000000p+0, 0.5) = -0x1.0000020000000p-1
        // double: lerp(-0x0.0000000000001p-1022, -0x1.0000000000001p+0, 0.5) = -0x1.0000000000001p-1
        {-limits<Ty>::denorm_min(), constants<Ty>::NegOneMinusUlp, Ty(0.5), -constants<Ty>::PointFivePlusUlp},

        {Ty(1.0), constants<Ty>::OnePlusUlp, constants<Ty>::PointFiveMinusUlp, Ty(1.0)},
        {Ty(-1.0), constants<Ty>::NegOneMinusUlp, constants<Ty>::PointFiveMinusUlp, Ty(-1.0)},

        {Ty(1.0), constants<Ty>::OnePlusUlp, Ty(0.5), Ty(1.0)},
        {Ty(-1.0), constants<Ty>::NegOneMinusUlp, Ty(0.5), Ty(-1.0)},

        {Ty(1.0), constants<Ty>::OnePlusUlp, constants<Ty>::PointFivePlusUlp, constants<Ty>::OnePlusUlp},
        {Ty(-1.0), constants<Ty>::NegOneMinusUlp, constants<Ty>::PointFivePlusUlp, constants<Ty>::NegOneMinusUlp},

        {Ty(-1.0), Ty(1.0), constants<Ty>::TwoPlusUlp, Ty(2.0) * constants<Ty>::TwoPlusUlp - Ty(1.0)},
        {Ty(0.0), Ty(1.0), constants<Ty>::TwoPlusUlp, constants<Ty>::TwoPlusUlp},
        {Ty(-1.0), Ty(0.0), constants<Ty>::TwoPlusUlp, constants<Ty>::TwoPlusUlp - Ty(1.0)},

        {Ty(1.0), Ty(-1.0), constants<Ty>::TwoPlusUlp, Ty(1.0) - Ty(2.0) * constants<Ty>::TwoPlusUlp},
        {Ty(0.0), Ty(-1.0), constants<Ty>::TwoPlusUlp, -constants<Ty>::TwoPlusUlp},
        {Ty(1.0), Ty(0.0), constants<Ty>::TwoPlusUlp, Ty(1.0) - constants<Ty>::TwoPlusUlp},

        {Ty(1.0), Ty(2.0), constants<Ty>::OnePlusUlp, Ty(2.0)},

        {Ty(1.0), Ty(2.0), constants<Ty>::TwoPlusUlp, constants<Ty>::TwoPlusUlp + Ty(1.0)},
        {Ty(1.0), Ty(2.0), Ty(0.5), constants<Ty>::PointFivePlusUlp + Ty(1.0)},

        {limits<Ty>::max(), limits<Ty>::max(), Ty(2.0), limits<Ty>::max()},
        {limits<Ty>::lowest(), limits<Ty>::lowest(), Ty(2.0), limits<Ty>::lowest()},

        {limits<Ty>::denorm_min(), Ty(1.0), Ty(0.5), limits<Ty>::denorm_min() + Ty(1.0) / Ty(2.0)},
        {limits<Ty>::denorm_min(), limits<Ty>::max(), Ty(0.5), limits<Ty>::denorm_min() + limits<Ty>::max() / Ty(2.0)},
        {limits<Ty>::denorm_min(), limits<Ty>::lowest(), Ty(0.5),
            (limits<Ty>::denorm_min() + limits<Ty>::lowest()) / Ty(2.0)},
        {limits<Ty>::denorm_min(), limits<Ty>::infinity(), Ty(0.5), limits<Ty>::infinity()},
        {limits<Ty>::denorm_min(), -limits<Ty>::infinity(), Ty(0.5), -limits<Ty>::infinity()},

        // the following handling of infinities isn't in the spec, but seems like the right behavior:

        // if the values differ and T is an infinity, the appropriate infinity according to direction
        {Ty(0), Ty(1), limits<Ty>::infinity(), limits<Ty>::infinity()},
        {Ty(0), Ty(1), -limits<Ty>::infinity(), -limits<Ty>::infinity()},
        {Ty(0), -Ty(1), limits<Ty>::infinity(), -limits<Ty>::infinity()},
        {Ty(0), -Ty(1), -limits<Ty>::infinity(), limits<Ty>::infinity()},

        // when the inputs are infinity of the same sign and 0 < T < 1, return that infinity
        {limits<Ty>::infinity(), limits<Ty>::infinity(), limits<Ty>::denorm_min(), limits<Ty>::infinity()},
        {-limits<Ty>::infinity(), -limits<Ty>::infinity(), limits<Ty>::denorm_min(), -limits<Ty>::infinity()},
        {limits<Ty>::infinity(), limits<Ty>::infinity(), Ty(0.5), limits<Ty>::infinity()},
        {-limits<Ty>::infinity(), -limits<Ty>::infinity(), Ty(0.5), -limits<Ty>::infinity()},
        {limits<Ty>::infinity(), limits<Ty>::infinity(), constants<Ty>::OneMinusUlp, limits<Ty>::infinity()},
        {-limits<Ty>::infinity(), -limits<Ty>::infinity(), constants<Ty>::OneMinusUlp, -limits<Ty>::infinity()},

        // when the inputs are infinity of opposite signs and T > 1, return the second parameter
        {-limits<Ty>::infinity(), limits<Ty>::infinity(), constants<Ty>::OnePlusUlp, limits<Ty>::infinity()},
        {limits<Ty>::infinity(), -limits<Ty>::infinity(), constants<Ty>::OnePlusUlp, -limits<Ty>::infinity()},
        {-limits<Ty>::infinity(), limits<Ty>::infinity(), Ty(2.0), limits<Ty>::infinity()},
        {limits<Ty>::infinity(), -limits<Ty>::infinity(), Ty(2.0), -limits<Ty>::infinity()},
        {-limits<Ty>::infinity(), limits<Ty>::infinity(), limits<Ty>::max(), limits<Ty>::infinity()},
        {limits<Ty>::infinity(), -limits<Ty>::infinity(), limits<Ty>::max(), -limits<Ty>::infinity()},
        {-limits<Ty>::infinity(), limits<Ty>::infinity(), limits<Ty>::infinity(), limits<Ty>::infinity()},
        {limits<Ty>::infinity(), -limits<Ty>::infinity(), limits<Ty>::infinity(), -limits<Ty>::infinity()},

        // when the inputs are infinity of opposite signs and T < 0, return the first parameter
        {-limits<Ty>::infinity(), limits<Ty>::infinity(), -limits<Ty>::infinity(), -limits<Ty>::infinity()},
        {limits<Ty>::infinity(), -limits<Ty>::infinity(), -limits<Ty>::infinity(), limits<Ty>::infinity()},
        {-limits<Ty>::infinity(), limits<Ty>::infinity(), -limits<Ty>::max(), -limits<Ty>::infinity()},
        {limits<Ty>::infinity(), -limits<Ty>::infinity(), -limits<Ty>::max(), limits<Ty>::infinity()},
        {-limits<Ty>::infinity(), limits<Ty>::infinity(), -Ty(2.0), -limits<Ty>::infinity()},
        {limits<Ty>::infinity(), -limits<Ty>::infinity(), -Ty(2.0), limits<Ty>::infinity()},
        {-limits<Ty>::infinity(), limits<Ty>::infinity(), -limits<Ty>::denorm_min(), -limits<Ty>::infinity()},
        {limits<Ty>::infinity(), -limits<Ty>::infinity(), -limits<Ty>::denorm_min(), limits<Ty>::infinity()},

        // if a is an infinity, b is finite and T != 1, return that infinity or the other according to "direction" of t
        {limits<Ty>::infinity(), Ty(1.0), -limits<Ty>::infinity(), limits<Ty>::infinity()},
        {limits<Ty>::infinity(), Ty(1.0), -limits<Ty>::max(), limits<Ty>::infinity()},
        {limits<Ty>::infinity(), Ty(1.0), -Ty(1.0), limits<Ty>::infinity()},
        {limits<Ty>::infinity(), Ty(1.0), -limits<Ty>::denorm_min(), limits<Ty>::infinity()},
        {limits<Ty>::infinity(), Ty(1.0), -Ty(0.0), limits<Ty>::infinity()},
        {limits<Ty>::infinity(), Ty(1.0), Ty(0.0), limits<Ty>::infinity()},
        {limits<Ty>::infinity(), Ty(1.0), limits<Ty>::denorm_min(), limits<Ty>::infinity()},
        {limits<Ty>::infinity(), Ty(1.0), Ty(0.5), limits<Ty>::infinity()},
        {limits<Ty>::infinity(), Ty(1.0), constants<Ty>::OneMinusUlp, limits<Ty>::infinity()},

        {limits<Ty>::infinity(), Ty(1.0), constants<Ty>::OnePlusUlp, -limits<Ty>::infinity()},
        {limits<Ty>::infinity(), Ty(1.0), Ty(2.0), -limits<Ty>::infinity()},
        {limits<Ty>::infinity(), Ty(1.0), limits<Ty>::max(), -limits<Ty>::infinity()},
        {limits<Ty>::infinity(), Ty(1.0), limits<Ty>::infinity(), -limits<Ty>::infinity()},

        {-limits<Ty>::infinity(), Ty(1.0), -limits<Ty>::infinity(), -limits<Ty>::infinity()},
        {-limits<Ty>::infinity(), Ty(1.0), -limits<Ty>::max(), -limits<Ty>::infinity()},
        {-limits<Ty>::infinity(), Ty(1.0), -Ty(1.0), -limits<Ty>::infinity()},
        {-limits<Ty>::infinity(), Ty(1.0), -limits<Ty>::denorm_min(), -limits<Ty>::infinity()},
        {-limits<Ty>::infinity(), Ty(1.0), -Ty(0.0), -limits<Ty>::infinity()},
        {-limits<Ty>::infinity(), Ty(1.0), Ty(0.0), -limits<Ty>::infinity()},
        {-limits<Ty>::infinity(), Ty(1.0), limits<Ty>::denorm_min(), -limits<Ty>::infinity()},
        {-limits<Ty>::infinity(), Ty(1.0), Ty(0.5), -limits<Ty>::infinity()},
        {-limits<Ty>::infinity(), Ty(1.0), constants<Ty>::OneMinusUlp, -limits<Ty>::infinity()},

        {-limits<Ty>::infinity(), Ty(1.0), constants<Ty>::OnePlusUlp, limits<Ty>::infinity()},
        {-limits<Ty>::infinity(), Ty(1.0), Ty(2.0), limits<Ty>::infinity()},
        {-limits<Ty>::infinity(), Ty(1.0), limits<Ty>::max(), limits<Ty>::infinity()},
        {-limits<Ty>::infinity(), Ty(1.0), limits<Ty>::infinity(), limits<Ty>::infinity()},

        // if b is an infinity, a is finite and T != 0, return that infinity or the other according to "direction" of t
        {Ty(1.0), limits<Ty>::infinity(), -limits<Ty>::infinity(), -limits<Ty>::infinity()},
        {Ty(1.0), limits<Ty>::infinity(), -limits<Ty>::max(), -limits<Ty>::infinity()},
        {Ty(1.0), limits<Ty>::infinity(), -Ty(1.0), -limits<Ty>::infinity()},
        {Ty(1.0), limits<Ty>::infinity(), -limits<Ty>::denorm_min(), -limits<Ty>::infinity()},

        {Ty(1.0), limits<Ty>::infinity(), limits<Ty>::denorm_min(), limits<Ty>::infinity()},
        {Ty(1.0), limits<Ty>::infinity(), Ty(0.5), limits<Ty>::infinity()},
        {Ty(1.0), limits<Ty>::infinity(), constants<Ty>::OneMinusUlp, limits<Ty>::infinity()},
        {Ty(1.0), limits<Ty>::infinity(), Ty(1.0), limits<Ty>::infinity()},
        {Ty(1.0), limits<Ty>::infinity(), constants<Ty>::OnePlusUlp, limits<Ty>::infinity()},
        {Ty(1.0), limits<Ty>::infinity(), Ty(2.0), limits<Ty>::infinity()},
        {Ty(1.0), limits<Ty>::infinity(), limits<Ty>::max(), limits<Ty>::infinity()},
        {Ty(1.0), limits<Ty>::infinity(), limits<Ty>::infinity(), limits<Ty>::infinity()},

        {Ty(1.0), -limits<Ty>::infinity(), -limits<Ty>::infinity(), limits<Ty>::infinity()},
        {Ty(1.0), -limits<Ty>::infinity(), -limits<Ty>::max(), limits<Ty>::infinity()},
        {Ty(1.0), -limits<Ty>::infinity(), -Ty(1.0), limits<Ty>::infinity()},
        {Ty(1.0), -limits<Ty>::infinity(), -limits<Ty>::denorm_min(), limits<Ty>::infinity()},

        {Ty(1.0), -limits<Ty>::infinity(), limits<Ty>::denorm_min(), -limits<Ty>::infinity()},
        {Ty(1.0), -limits<Ty>::infinity(), Ty(0.5), -limits<Ty>::infinity()},
        {Ty(1.0), -limits<Ty>::infinity(), constants<Ty>::OneMinusUlp, -limits<Ty>::infinity()},
        {Ty(1.0), -limits<Ty>::infinity(), Ty(1.0), -limits<Ty>::infinity()},
        {Ty(1.0), -limits<Ty>::infinity(), constants<Ty>::OnePlusUlp, -limits<Ty>::infinity()},
        {Ty(1.0), -limits<Ty>::infinity(), Ty(2.0), -limits<Ty>::infinity()},
        {Ty(1.0), -limits<Ty>::infinity(), limits<Ty>::max(), -limits<Ty>::infinity()},
        {Ty(1.0), -limits<Ty>::infinity(), limits<Ty>::infinity(), -limits<Ty>::infinity()},
    };

    static inline constexpr LerpTestCase<Ty> lerpOverflowTestCases[] = {
        {limits<Ty>::lowest(), limits<Ty>::max(), Ty(2.0), limits<Ty>::infinity()},
        {limits<Ty>::max(), limits<Ty>::lowest(), Ty(2.0), -limits<Ty>::infinity()},
    };

    static inline constexpr LerpNaNTestCase<Ty> lerpInvalidTestCases[] = {
        // if the values are equal and T is an infinity, NaN
        {Ty(0), Ty(0), limits<Ty>::infinity()},
        {Ty(0), Ty(0), -limits<Ty>::infinity()},

        // when the inputs are infinity of the same sign and T <= 0, return NaN
        {limits<Ty>::infinity(), limits<Ty>::infinity(), -limits<Ty>::infinity()},
        {-limits<Ty>::infinity(), -limits<Ty>::infinity(), -limits<Ty>::infinity()},
        {limits<Ty>::infinity(), limits<Ty>::infinity(), -limits<Ty>::max()},
        {-limits<Ty>::infinity(), -limits<Ty>::infinity(), -limits<Ty>::max()},
        {limits<Ty>::infinity(), limits<Ty>::infinity(), -Ty(1.0)},
        {-limits<Ty>::infinity(), -limits<Ty>::infinity(), -Ty(1.0)},
        {limits<Ty>::infinity(), limits<Ty>::infinity(), -limits<Ty>::denorm_min()},
        {-limits<Ty>::infinity(), -limits<Ty>::infinity(), -limits<Ty>::denorm_min()},
        {limits<Ty>::infinity(), limits<Ty>::infinity(), -Ty(0.0)},
        {-limits<Ty>::infinity(), -limits<Ty>::infinity(), -Ty(0.0)},
        {limits<Ty>::infinity(), limits<Ty>::infinity(), Ty(0.0)},
        {-limits<Ty>::infinity(), -limits<Ty>::infinity(), Ty(0.0)},

        // when the inputs are infinity of the same sign and T >= 1, return NaN
        {limits<Ty>::infinity(), limits<Ty>::infinity(), Ty(1.0)},
        {-limits<Ty>::infinity(), -limits<Ty>::infinity(), Ty(1.0)},
        {limits<Ty>::infinity(), limits<Ty>::infinity(), constants<Ty>::OnePlusUlp},
        {-limits<Ty>::infinity(), -limits<Ty>::infinity(), constants<Ty>::OnePlusUlp},
        {limits<Ty>::infinity(), limits<Ty>::infinity(), Ty(2.0)},
        {-limits<Ty>::infinity(), -limits<Ty>::infinity(), Ty(2.0)},
        {limits<Ty>::infinity(), limits<Ty>::infinity(), limits<Ty>::max()},
        {-limits<Ty>::infinity(), -limits<Ty>::infinity(), limits<Ty>::max()},
        {limits<Ty>::infinity(), limits<Ty>::infinity(), limits<Ty>::infinity()},
        {-limits<Ty>::infinity(), -limits<Ty>::infinity(), limits<Ty>::infinity()},

        // when the inputs are infinity of opposite signs and 0 <= T <= 1, return NaN
        {-limits<Ty>::infinity(), limits<Ty>::infinity(), -Ty(0.0)},
        {limits<Ty>::infinity(), -limits<Ty>::infinity(), -Ty(0.0)},
        {-limits<Ty>::infinity(), limits<Ty>::infinity(), Ty(0.0)},
        {limits<Ty>::infinity(), -limits<Ty>::infinity(), Ty(0.0)},
        {-limits<Ty>::infinity(), limits<Ty>::infinity(), limits<Ty>::denorm_min()},
        {limits<Ty>::infinity(), -limits<Ty>::infinity(), limits<Ty>::denorm_min()},
        {-limits<Ty>::infinity(), limits<Ty>::infinity(), Ty(0.5)},
        {limits<Ty>::infinity(), -limits<Ty>::infinity(), Ty(0.5)},
        {-limits<Ty>::infinity(), limits<Ty>::infinity(), constants<Ty>::OneMinusUlp},
        {limits<Ty>::infinity(), -limits<Ty>::infinity(), constants<Ty>::OneMinusUlp},
        {-limits<Ty>::infinity(), limits<Ty>::infinity(), Ty(1.0)},
        {limits<Ty>::infinity(), -limits<Ty>::infinity(), Ty(1.0)},

        // if a is an infinity, b is finite and T = 1, return NaN
        {limits<Ty>::infinity(), Ty(1.0), Ty(1.0)},
        {-limits<Ty>::infinity(), Ty(1.0), Ty(1.0)},

        // if b is an infinity, a is finite and T = 0, return NaN
        {Ty(1.0), limits<Ty>::infinity(), Ty(0.0)},
        {Ty(1.0), limits<Ty>::infinity(), -Ty(0.0)},
        {Ty(1.0), -limits<Ty>::infinity(), Ty(0.0)},
        {Ty(1.0), -limits<Ty>::infinity(), -Ty(0.0)},
    };

    static inline constexpr LerpNaNTestCase<Ty> lerpNaNTestCases[] = {
        {mint_nan<Ty>(0, 42), mint_nan<Ty>(1, 42), mint_nan<Ty>(0, 1729),
            {mint_nan<Ty>(0, 42), mint_nan<Ty>(1, 42), mint_nan<Ty>(0, 1729)}},
        {Ty(1.0), mint_nan<Ty>(1, 42), mint_nan<Ty>(0, 1729), {mint_nan<Ty>(1, 42), mint_nan<Ty>(0, 1729)}},
        {mint_nan<Ty>(1, 42), Ty(1.0), mint_nan<Ty>(0, 1729), {mint_nan<Ty>(1, 42), mint_nan<Ty>(0, 1729)}},
        {Ty(1.0), Ty(1.0), mint_nan<Ty>(0, 1729), {mint_nan<Ty>(0, 1729)}},

        {limits<Ty>::infinity(), mint_nan<Ty>(1, 42), mint_nan<Ty>(0, 1729),
            {mint_nan<Ty>(1, 42), mint_nan<Ty>(0, 1729)}},
        {mint_nan<Ty>(1, 42), limits<Ty>::infinity(), mint_nan<Ty>(0, 1729),
            {mint_nan<Ty>(1, 42), mint_nan<Ty>(0, 1729)}},
        {limits<Ty>::infinity(), limits<Ty>::infinity(), mint_nan<Ty>(0, 1729), {mint_nan<Ty>(0, 1729)}},

        {mint_nan<Ty>(0, 42), mint_nan<Ty>(1, 42), -Ty(0.0), {mint_nan<Ty>(0, 42), mint_nan<Ty>(1, 42)}},
        {mint_nan<Ty>(0, 42), mint_nan<Ty>(1, 42), Ty(0.0), {mint_nan<Ty>(0, 42), mint_nan<Ty>(1, 42)}},
        {mint_nan<Ty>(0, 42), mint_nan<Ty>(1, 42), Ty(1.0), {mint_nan<Ty>(0, 42), mint_nan<Ty>(1, 42)}},
        {mint_nan<Ty>(0, 42), Ty(1.0), -Ty(0.0), {mint_nan<Ty>(0, 42)}},
        {mint_nan<Ty>(0, 42), Ty(1.0), Ty(0.0), {mint_nan<Ty>(0, 42)}},
        {mint_nan<Ty>(0, 42), Ty(1.0), Ty(1.0), {mint_nan<Ty>(0, 42)}},
        {mint_nan<Ty>(0, 42), limits<Ty>::infinity(), -Ty(0.0), {mint_nan<Ty>(0, 42)}},
        {mint_nan<Ty>(0, 42), limits<Ty>::infinity(), Ty(0.0), {mint_nan<Ty>(0, 42)}},
        {mint_nan<Ty>(0, 42), limits<Ty>::infinity(), Ty(1.0), {mint_nan<Ty>(0, 42)}},
        {Ty(1.0), mint_nan<Ty>(1, 42), -Ty(0.0), {mint_nan<Ty>(1, 42)}},
        {Ty(1.0), mint_nan<Ty>(1, 42), Ty(0.0), {mint_nan<Ty>(1, 42)}},
        {Ty(1.0), mint_nan<Ty>(1, 42), Ty(1.0), {mint_nan<Ty>(1, 42)}},
        {limits<Ty>::infinity(), mint_nan<Ty>(1, 42), -Ty(0.0), {mint_nan<Ty>(1, 42)}},
        {limits<Ty>::infinity(), mint_nan<Ty>(1, 42), Ty(0.0), {mint_nan<Ty>(1, 42)}},
        {limits<Ty>::infinity(), mint_nan<Ty>(1, 42), Ty(1.0), {mint_nan<Ty>(1, 42)}},
    };
};

template <typename Ty>
void print_lerp_result(const LerpTestCase<Ty>& testCase, const Ty answer) {
    char failureMessageBuffer[1000]{};
    char* cursor = failureMessageBuffer;
    char* end    = cursor + sizeof(failureMessageBuffer);
    memcpy(cursor, "lerp(", 5);
    cursor += 5;
    cursor = to_chars(cursor, end, testCase.x).ptr;
    memcpy(cursor, ", ", 2);
    cursor += 2;
    cursor = to_chars(cursor, end, testCase.y).ptr;
    memcpy(cursor, ", ", 2);
    cursor += 2;
    cursor = to_chars(cursor, end, testCase.t).ptr;
    memcpy(cursor, ") == ", 5);
    cursor += 5;
    cursor = to_chars(cursor, end, answer).ptr;
    memcpy(cursor, "; expected ", 11);
    cursor += 11;
    cursor    = to_chars(cursor, end, testCase.expected).ptr;
    cursor[0] = '\0';
    puts(failureMessageBuffer);
}

template <typename Ty>
void print_lerp_result(const LerpNaNTestCase<Ty>& testCase, const Ty answer) {
    char failureMessageBuffer[1000]{};
    char* cursor = failureMessageBuffer;
    char* end    = cursor + sizeof(failureMessageBuffer);
    memcpy(cursor, "lerp(", 5);
    cursor += 5;
    cursor = to_chars(cursor, end, testCase.x).ptr;
    memcpy(cursor, ", ", 2);
    cursor += 2;
    cursor = to_chars(cursor, end, testCase.y).ptr;
    memcpy(cursor, ", ", 2);
    cursor += 2;
    cursor = to_chars(cursor, end, testCase.t).ptr;
    memcpy(cursor, ") == ", 5);
    cursor += 5;
    cursor = to_chars(cursor, end, answer).ptr;
    memcpy(cursor, "; expected a NaN", 17);
    puts(failureMessageBuffer);
}

template <typename Ty>
bool test_lerp() {
    STATIC_ASSERT(is_same_v<decltype(lerp(Ty(), Ty(), Ty())), decltype(Ty())>);
    STATIC_ASSERT(is_signed_v<Ty>);
    STATIC_ASSERT(noexcept(lerp(Ty(), Ty(), Ty())));

    constexpr auto test_lerp_constexpr = [] {
        using bit_type = conditional_t<sizeof(Ty) == 4, unsigned int, unsigned long long>;

        for (const auto& testCase : LerpCases<Ty>::lerpTestCases) {
            const auto answer = lerp(testCase.x, testCase.y, testCase.t);
            assert(bit_cast<bit_type>(answer) == bit_cast<bit_type>(testCase.expected));
        }

        for (auto&& testCase : LerpCases<Ty>::lerpNaNTestCases) {
            const auto answer = lerp(testCase.x, testCase.y, testCase.t);
            assert(any_of(begin(testCase.expected_list), end(testCase.expected_list), [&](const auto& expected) {
                return expected.has_value() && bit_cast<bit_type>(answer) == bit_cast<bit_type>(expected.value());
            }));
        }

        return true;
    };

    STATIC_ASSERT(test_lerp_constexpr());

    for (auto&& testCase : LerpCases<Ty>::lerpTestCases) {
        ExceptGuard except;
        const auto answer = lerp(testCase.x, testCase.y, testCase.t);
        if (!check_feexcept(0) || memcmp(&answer, &testCase.expected, sizeof(Ty)) != 0) {
            print_lerp_result(testCase, answer);
            abort();
        }
    }

    for (auto&& testCase : LerpCases<Ty>::lerpOverflowTestCases) {
        ExceptGuard except;
        const auto answer = lerp(testCase.x, testCase.y, testCase.t);
        if (!check_feexcept(fe_overflow) || memcmp(&answer, &testCase.expected, sizeof(Ty)) != 0) {
            print_lerp_result(testCase, answer);
            abort();
        }
    }

    for (auto&& testCase : LerpCases<Ty>::lerpInvalidTestCases) {
        ExceptGuard except;
        const auto answer = lerp(testCase.x, testCase.y, testCase.t);
        if (!check_feexcept(fe_invalid) || !isnan(answer)) {
            print_lerp_result(testCase, answer);
            abort();
        }
    }

    for (auto&& testCase : LerpCases<Ty>::lerpNaNTestCases) {
        ExceptGuard except;
        const auto answer = lerp(testCase.x, testCase.y, testCase.t);
        if (!check_feexcept(0)
            || none_of(begin(testCase.expected_list), end(testCase.expected_list), [&answer](const auto& expected) {
                   return expected.has_value() && memcmp(&answer, &expected.value(), sizeof(Ty)) == 0;
               })) {
            print_lerp_result(testCase, answer);
            abort();
        }
    }

    constexpr auto test_lerp_snan = [](const Ty& a, const Ty& b, const Ty& t) {
        ExceptGuard except;
        const auto answer = lerp(a, b, t);
        return check_feexcept(fe_invalid) && isnan(answer);
    };

    Ty snan;
    make_snan(snan);

    assert(test_lerp_snan(snan, limits<Ty>::quiet_NaN(), limits<Ty>::quiet_NaN()));
    assert(test_lerp_snan(limits<Ty>::quiet_NaN(), snan, limits<Ty>::quiet_NaN()));
    assert(test_lerp_snan(snan, snan, limits<Ty>::quiet_NaN()));
    assert(test_lerp_snan(limits<Ty>::quiet_NaN(), limits<Ty>::quiet_NaN(), snan));
    assert(test_lerp_snan(snan, limits<Ty>::quiet_NaN(), snan));
    assert(test_lerp_snan(limits<Ty>::quiet_NaN(), snan, snan));
    assert(test_lerp_snan(snan, snan, snan));

    assert(test_lerp_snan(Ty{0}, Ty{0}, snan));
    assert(test_lerp_snan(Ty{1}, Ty{1}, snan));
    assert(test_lerp_snan(Ty{0}, snan, Ty{0}));
    assert(test_lerp_snan(Ty{0}, snan, Ty{1}));
    assert(test_lerp_snan(snan, Ty{0}, Ty{0}));
    assert(test_lerp_snan(snan, Ty{0}, Ty{1}));

    STATIC_ASSERT(cmp(lerp(Ty(1.0), Ty(2.0), Ty(4.0)), lerp(Ty(1.0), Ty(2.0), Ty(3.0))) * cmp(Ty(4.0), Ty(3.0))
                      * cmp(Ty(2.0), Ty(1.0))
                  >= 0);

    assert(cmp(lerp(Ty(1.0), Ty(2.0), Ty(4.0)), lerp(Ty(1.0), Ty(2.0), Ty(3.0))) * cmp(Ty(4.0), Ty(3.0))
               * cmp(Ty(2.0), Ty(1.0))
           >= 0);

    return true;
}

void test_gh_1917() {
    // GH-1917 <cmath>: lerp(1e+308, 5e+307, 4.0) spuriously overflows
    using bit_type       = unsigned long long;
    using float_bit_type = unsigned int;
    STATIC_ASSERT(bit_cast<bit_type>(lerp(1e+308, 5e+307, 4.0)) == bit_cast<bit_type>(-1e+308));
    {
        ExceptGuard except;

        assert(bit_cast<bit_type>(lerp(1e+308, 5e+307, 4.0)) == bit_cast<bit_type>(-1e+308));
        assert(check_feexcept(0));
    }
    STATIC_ASSERT(bit_cast<float_bit_type>(lerp(2e+38f, 1e+38f, 4.0f)) == bit_cast<float_bit_type>(-2e+38f));
    {
        ExceptGuard except;

        assert(bit_cast<float_bit_type>(lerp(2e+38f, 1e+38f, 4.0f)) == bit_cast<float_bit_type>(-2e+38f));
        assert(check_feexcept(0));
    }
#ifdef _M_FP_STRICT
    {
        ExceptGuard except;
        RoundGuard round{FE_UPWARD};

        assert(bit_cast<bit_type>(lerp(1e+308, 5e+307, 4.0)) == bit_cast<bit_type>(-1e+308));
        assert(check_feexcept(0));
    }
    {
        ExceptGuard except;
        RoundGuard round{FE_UPWARD};

        assert(bit_cast<float_bit_type>(lerp(2e+38f, 1e+38f, 4.0f)) == bit_cast<float_bit_type>(-2e+38f));
        assert(check_feexcept(0));
    }
    {
        ExceptGuard except;
        RoundGuard round{FE_DOWNWARD};

        assert(bit_cast<bit_type>(lerp(1e+308, 5e+307, 4.0)) == bit_cast<bit_type>(-1e+308));
        assert(check_feexcept(0));
    }
    {
        ExceptGuard except;
        RoundGuard round{FE_DOWNWARD};

        assert(bit_cast<float_bit_type>(lerp(2e+38f, 1e+38f, 4.0f)) == bit_cast<float_bit_type>(-2e+38f));
        assert(check_feexcept(0));
    }
    {
        ExceptGuard except;
        RoundGuard round{FE_TOWARDZERO};

        assert(bit_cast<bit_type>(lerp(1e+308, 5e+307, 4.0)) == bit_cast<bit_type>(-1e+308));
        assert(check_feexcept(0));
    }
    {
        ExceptGuard except;
        RoundGuard round{FE_TOWARDZERO};

        assert(bit_cast<float_bit_type>(lerp(2e+38f, 1e+38f, 4.0f)) == bit_cast<float_bit_type>(-2e+38f));
        assert(check_feexcept(0));
    }
    {
        ExceptGuard except;
        const int r = feraiseexcept(FE_OVERFLOW);

        assert(r == 0);
        assert(bit_cast<bit_type>(lerp(1e+308, 5e+307, 4.0)) == bit_cast<bit_type>(-1e+308));
        assert(check_feexcept(FE_OVERFLOW));
    }
    {
        ExceptGuard except;
        const int r = feraiseexcept(FE_OVERFLOW);

        assert(r == 0);
        assert(bit_cast<float_bit_type>(lerp(2e+38f, 1e+38f, 4.0f)) == bit_cast<float_bit_type>(-2e+38f));
        assert(check_feexcept(FE_OVERFLOW));
    }
#endif // _M_FP_STRICT
}

constexpr bool test_gh_2112() {
    // GH-2112 <cmath>: std::lerp is missing Arithmetic overloads
    assert(lerp(0, 0, 0) == 0.0);
    assert(lerp(0.0f, 0.0f, 0.0) == 0.0);
    assert(lerp(0.0L, 0, 0) == 0.0L);

    STATIC_ASSERT(is_same_v<double, decltype(lerp(0, 0, 0))>);
    STATIC_ASSERT(is_same_v<long double, decltype(lerp(0.0L, 0, 0))>);
    STATIC_ASSERT(is_same_v<long double, decltype(lerp(0, 0.0L, 0))>);
    STATIC_ASSERT(is_same_v<long double, decltype(lerp(0, 0, 0.0L))>);

    return true;
}

int main() {
    test_constants<float>();
    test_constants<double>();
    test_constants<long double>();

    STATIC_ASSERT(test_midpoint_integer_signed<char>());
    STATIC_ASSERT(test_midpoint_integer_signed<signed char>());
    STATIC_ASSERT(test_midpoint_integer_signed<short>());
    STATIC_ASSERT(test_midpoint_integer_signed<int>());
    STATIC_ASSERT(test_midpoint_integer_signed<long>());
    STATIC_ASSERT(test_midpoint_integer_signed<long long>());

    STATIC_ASSERT(test_midpoint_integer_unsigned<wchar_t>());
#ifdef __cpp_char8_t
    STATIC_ASSERT(test_midpoint_integer_unsigned<char8_t>());
#endif // __cpp_char8_t
    STATIC_ASSERT(test_midpoint_integer_unsigned<char16_t>());
    STATIC_ASSERT(test_midpoint_integer_unsigned<char32_t>());
    STATIC_ASSERT(test_midpoint_integer_unsigned<unsigned char>());
    STATIC_ASSERT(test_midpoint_integer_unsigned<unsigned short>());
    STATIC_ASSERT(test_midpoint_integer_unsigned<unsigned int>());
    STATIC_ASSERT(test_midpoint_integer_unsigned<unsigned long>());
    STATIC_ASSERT(test_midpoint_integer_unsigned<unsigned long long>());

    test_midpoint_integer_signed<char>();
    test_midpoint_integer_signed<signed char>();
    test_midpoint_integer_signed<short>();
    test_midpoint_integer_signed<int>();
    test_midpoint_integer_signed<long>();
    test_midpoint_integer_signed<long long>();

    test_midpoint_integer_unsigned<wchar_t>();
#ifdef __cpp_char8_t
    test_midpoint_integer_unsigned<char8_t>();
#endif // __cpp_char8_t
    test_midpoint_integer_unsigned<char16_t>();
    test_midpoint_integer_unsigned<char32_t>();
    test_midpoint_integer_unsigned<unsigned char>();
    test_midpoint_integer_unsigned<unsigned short>();
    test_midpoint_integer_unsigned<unsigned int>();
    test_midpoint_integer_unsigned<unsigned long>();
    test_midpoint_integer_unsigned<unsigned long long>();

    STATIC_ASSERT(test_midpoint_floating_constexpr<float>());
    STATIC_ASSERT(test_midpoint_floating_constexpr<double>());
    STATIC_ASSERT(test_midpoint_floating_constexpr<long double>());

    {
        ExceptGuard except;
        test_midpoint_floating_constexpr<float>();
        test_midpoint_floating_constexpr<double>();
        test_midpoint_floating_constexpr<long double>();
        assert(check_feexcept(0));
    }

    test_midpoint_floating<float>();
    test_midpoint_floating<double>();
    test_midpoint_floating<long double>();

    STATIC_ASSERT(test_midpoint_pointer<char>());
    STATIC_ASSERT(test_midpoint_pointer<int>());
    STATIC_ASSERT(test_midpoint_pointer<double>());

    test_midpoint_pointer<char>();
    test_midpoint_pointer<int>();
    test_midpoint_pointer<double>();

    test_lerp<float>();
    test_lerp<double>();
    test_lerp<long double>();

    test_gh_1917();
    test_gh_2112();
    STATIC_ASSERT(test_gh_2112());
}
