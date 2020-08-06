// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <complex>

#include "floating_point_utils.hpp"
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

int main() {
    for (const auto& mode : all_rounding_modes) {
        test_sqrt(mode);
    }
}
