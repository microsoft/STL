// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifdef __clang__
#pragma clang diagnostic ignored "-Wnan-infinity-disabled"
#endif // __clang__

#include <cassert>
#include <complex>
#include <limits>

#include "floating_point_utils.hpp"
#include "log_test_cases.hpp"
#include "sqrt_test_cases.hpp"

using namespace std;
using namespace fputil;

void test_sqrt(const rounding_mode mode) {
#if FP_PRESET_FAST
    constexpr int ulp_tolerance = 4;
#else // ^^^ fp:fast / not fp:fast vvv
    const int ulp_tolerance = is_directed_rounding_mode(mode) ? 3 : 2;
#endif // ^^^ not fp:fast ^^^

    const auto check_result = [&](const auto& result, const auto& test_case) {
        const int case_real_ulp_tolerance = test_case.result_exactness.real ? 0 : ulp_tolerance;
        const int case_imag_ulp_tolerance = test_case.result_exactness.imag ? 0 : ulp_tolerance;
        return near_equal(result.real(), test_case.expected_result.real(), case_real_ulp_tolerance)
            && near_equal(result.imag(), test_case.expected_result.imag(), case_imag_ulp_tolerance);
    };

    for (const auto& c : sqrt_double_cases) {
        const auto result = [&] {
            rounding_guard guard(mode);
            return sqrt(c.input);
        }();

        assert(check_result(result, c));
    }

    for (const auto& c : sqrt_float_cases) {
        const auto result = [&] {
            rounding_guard guard(mode);
            return sqrt(c.input);
        }();

        assert(check_result(result, c));
    }

    for (const auto& c : sqrt_double_cases) {
        const auto result = [&] {
            rounding_guard guard(mode);
            return sqrt(static_cast<complex<long double>>(c.input));
        }();

        assert(check_result(static_cast<complex<double>>(result), c));
    }
}

void test_log(const rounding_mode mode) {
#if FP_PRESET_FAST
    constexpr int ulp_tolerance = 4;
    // under /fp:fast, allow inaccurate real(log(z)) when |z| is close to 1
    constexpr double real_absolute_epsilon_tolerance = 4;
#else // ^^^ fp:fast / not fp:fast vvv
    const int ulp_tolerance                          = is_directed_rounding_mode(mode) ? 3 : 2;
    constexpr double real_absolute_epsilon_tolerance = 0;
#endif // ^^^ not fp:fast ^^^

    const auto check_result = [&](const auto& result, const auto& test_case) {
        using Float = decltype(result.real());

        constexpr auto epsilon            = static_cast<double>(numeric_limits<Float>::epsilon());
        const int case_real_ulp_tolerance = test_case.result_exactness.real ? 0 : ulp_tolerance;
        const int case_imag_ulp_tolerance = test_case.result_exactness.imag ? 0 : ulp_tolerance;
        const double case_real_absolute_tolerance =
            test_case.result_exactness.real ? 0.0 : real_absolute_epsilon_tolerance * epsilon;

        // TRANSITION: under rounding toward negative mode, log(1.0) returns +0.0 on x86, -0.0 on x64
        const auto is_mod_exactly_one = [](const auto& z) {
            // no other complex<binary_floating_point> has mod of exactly 1
            return (abs(real(z)) == 1 && imag(z) == 0) || (real(z) == 0 && abs(imag(z)) == 1);
        };

        if (mode == rounding_mode::toward_negative && is_mod_exactly_one(test_case.input)) {
            return abs(result.real()) <= case_real_absolute_tolerance
                && near_equal(result.imag(), test_case.expected_result.imag(), case_imag_ulp_tolerance);
        }

        return near_equal(result.real(), test_case.expected_result.real(), case_real_ulp_tolerance,
                   case_real_absolute_tolerance)
            && near_equal(result.imag(), test_case.expected_result.imag(), case_imag_ulp_tolerance);
    };

    for (const auto& c : log_double_cases) {
        const auto result = [&] {
            rounding_guard guard(mode);
            return log(c.input);
        }();

        assert(check_result(result, c));
    }

    for (const auto& c : log_float_cases) {
        const auto result = [&] {
            rounding_guard guard(mode);
            return log(c.input);
        }();

        assert(check_result(result, c));
    }

    for (const auto& c : log_double_cases) {
        const auto result = [&] {
            rounding_guard guard(mode);
            return log(static_cast<complex<long double>>(c.input));
        }();

        assert(check_result(static_cast<complex<double>>(result), c));
    }
}

int main() {
    for (const auto& mode : all_rounding_modes) {
        test_sqrt(mode);
        test_log(mode);
    }
}
