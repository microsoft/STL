// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef WCHAR_TEST_CASES_HPP
#define WCHAR_TEST_CASES_HPP

#include <charconv>

#include "test.hpp"
using namespace std;

// The wchar_t machinery is currently limited to a subset of the Ryu code. It is known to not handle: negative numbers,
// infinity, NaN, or hex formatting.

inline constexpr DoublePrecisionToWideTestCase wide_digit_pairs_test_cases[] = {
    {0.0001020304, chars_format::fixed, 10, L"0.0001020304"},
    {0.0506070809, chars_format::fixed, 10, L"0.0506070809"},
    {0.1011121314, chars_format::fixed, 10, L"0.1011121314"},
    {0.1516171819, chars_format::fixed, 10, L"0.1516171819"},
    {0.2021222324, chars_format::fixed, 10, L"0.2021222324"},
    {0.2526272829, chars_format::fixed, 10, L"0.2526272829"},
    {0.3031323334, chars_format::fixed, 10, L"0.3031323334"},
    {0.3536373839, chars_format::fixed, 10, L"0.3536373839"},
    {0.4041424344, chars_format::fixed, 10, L"0.4041424344"},
    {0.4546474849, chars_format::fixed, 10, L"0.4546474849"},
    {0.5051525354, chars_format::fixed, 10, L"0.5051525354"},
    {0.5556575859, chars_format::fixed, 10, L"0.5556575859"},
    {0.6061626364, chars_format::fixed, 10, L"0.6061626364"},
    {0.6566676869, chars_format::fixed, 10, L"0.6566676869"},
    {0.7071727374, chars_format::fixed, 10, L"0.7071727374"},
    {0.7576777879, chars_format::fixed, 10, L"0.7576777879"},
    {0.8081828384, chars_format::fixed, 10, L"0.8081828384"},
    {0.8586878889, chars_format::fixed, 10, L"0.8586878889"},
    {0.9091929394, chars_format::fixed, 10, L"0.9091929394"},
    {0.9596979899, chars_format::fixed, 10, L"0.9596979899"},
};

inline constexpr DoubleToWideTestCase double_to_wide_test_cases[] = {
    // Test special cases (zero, inf, nan) and an ordinary case. Also test negative signs.
    {0.0, chars_format::scientific, L"0e+00"},
    // {-0.0, chars_format::scientific, L"-0e+00"},
    // {double_inf, chars_format::scientific, L"inf"},
    // {-double_inf, chars_format::scientific, L"-inf"},
    // {double_nan, chars_format::scientific, L"nan"},
    // {-double_nan, chars_format::scientific, L"-nan(ind)"},
    // {double_nan_payload, chars_format::scientific, L"nan"},
    // {-double_nan_payload, chars_format::scientific, L"-nan"},
    {2.018, chars_format::scientific, L"2.018e+00"},
    // {-2.018, chars_format::scientific, L"-2.018e+00"},
    {0.2018, chars_format::scientific, L"2.018e-01"},
    // {-0.2018, chars_format::scientific, L"-2.018e-01"},

    // Ditto for fixed, which doesn't emit exponents.
    {0.0, chars_format::fixed, L"0"},
    // {-0.0, chars_format::fixed, L"-0"},
    // {double_inf, chars_format::fixed, L"inf"},
    // {-double_inf, chars_format::fixed, L"-inf"},
    // {double_nan, chars_format::fixed, L"nan"},
    // {-double_nan, chars_format::fixed, L"-nan(ind)"},
    // {double_nan_payload, chars_format::fixed, L"nan"},
    // {-double_nan_payload, chars_format::fixed, L"-nan"},
    {2.018, chars_format::fixed, L"2.018"},
    // {-2.018, chars_format::fixed, L"-2.018"},

    // Ditto for general, which selects fixed for the scientific exponent 0.
    {0.0, chars_format::general, L"0"},
    // {-0.0, chars_format::general, L"-0"},
    // {double_inf, chars_format::general, L"inf"},
    // {-double_inf, chars_format::general, L"-inf"},
    // {double_nan, chars_format::general, L"nan"},
    // {-double_nan, chars_format::general, L"-nan(ind)"},
    // {double_nan_payload, chars_format::general, L"nan"},
    // {-double_nan_payload, chars_format::general, L"-nan"},
    {2.018, chars_format::general, L"2.018"},
    // {-2.018, chars_format::general, L"-2.018"},

    // Ditto for plain, which selects fixed because it's shorter for these values.
    {0.0, chars_format{}, L"0"},
    // {-0.0, chars_format{}, L"-0"},
    // {double_inf, chars_format{}, L"inf"},
    // {-double_inf, chars_format{}, L"-inf"},
    // {double_nan, chars_format{}, L"nan"},
    // {-double_nan, chars_format{}, L"-nan(ind)"},
    // {double_nan_payload, chars_format{}, L"nan"},
    // {-double_nan_payload, chars_format{}, L"-nan"},
    {2.018, chars_format{}, L"2.018"},
    // {-2.018, chars_format{}, L"-2.018"},

    // Ditto for hex.
    // {0.0, chars_format::hex, L"0p+0"},
    // {-0.0, chars_format::hex, L"-0p+0"},
    // {double_inf, chars_format::hex, L"inf"},
    // {-double_inf, chars_format::hex, L"-inf"},
    // {double_nan, chars_format::hex, L"nan"},
    // {-double_nan, chars_format::hex, L"-nan(ind)"},
    // {double_nan_payload, chars_format::hex, L"nan"},
    // {-double_nan_payload, chars_format::hex, L"-nan"},
    // {0x1.729p+0, chars_format::hex, L"1.729p+0"},
    // {-0x1.729p+0, chars_format::hex, L"-1.729p+0"},
    // {0x1.729p-1, chars_format::hex, L"1.729p-1"},
    // {-0x1.729p-1, chars_format::hex, L"-1.729p-1"},
};

inline constexpr FloatToWideTestCase float_to_wide_test_cases[] = {
    // Test special cases (zero, inf, nan) and an ordinary case. Also test negative signs.
    {0.0f, chars_format::scientific, L"0e+00"},
    // {-0.0f, chars_format::scientific, L"-0e+00"},
    // {float_inf, chars_format::scientific, L"inf"},
    // {-float_inf, chars_format::scientific, L"-inf"},
    // {float_nan, chars_format::scientific, L"nan"},
    // {-float_nan, chars_format::scientific, L"-nan(ind)"},
    // {float_nan_payload, chars_format::scientific, L"nan"},
    // {-float_nan_payload, chars_format::scientific, L"-nan"},
    {2.018f, chars_format::scientific, L"2.018e+00"},
    // {-2.018f, chars_format::scientific, L"-2.018e+00"},
    {0.2018f, chars_format::scientific, L"2.018e-01"},
    // {-0.2018f, chars_format::scientific, L"-2.018e-01"},

    // Ditto for fixed, which doesn't emit exponents.
    {0.0f, chars_format::fixed, L"0"},
    // {-0.0f, chars_format::fixed, L"-0"},
    // {float_inf, chars_format::fixed, L"inf"},
    // {-float_inf, chars_format::fixed, L"-inf"},
    // {float_nan, chars_format::fixed, L"nan"},
    // {-float_nan, chars_format::fixed, L"-nan(ind)"},
    // {float_nan_payload, chars_format::fixed, L"nan"},
    // {-float_nan_payload, chars_format::fixed, L"-nan"},
    {2.018f, chars_format::fixed, L"2.018"},
    // {-2.018f, chars_format::fixed, L"-2.018"},

    // Ditto for general, which selects fixed for the scientific exponent 0.
    {0.0f, chars_format::general, L"0"},
    // {-0.0f, chars_format::general, L"-0"},
    // {float_inf, chars_format::general, L"inf"},
    // {-float_inf, chars_format::general, L"-inf"},
    // {float_nan, chars_format::general, L"nan"},
    // {-float_nan, chars_format::general, L"-nan(ind)"},
    // {float_nan_payload, chars_format::general, L"nan"},
    // {-float_nan_payload, chars_format::general, L"-nan"},
    {2.018f, chars_format::general, L"2.018"},
    // {-2.018f, chars_format::general, L"-2.018"},

    // Ditto for plain, which selects fixed because it's shorter for these values.
    {0.0f, chars_format{}, L"0"},
    // {-0.0f, chars_format{}, L"-0"},
    // {float_inf, chars_format{}, L"inf"},
    // {-float_inf, chars_format{}, L"-inf"},
    // {float_nan, chars_format{}, L"nan"},
    // {-float_nan, chars_format{}, L"-nan(ind)"},
    // {float_nan_payload, chars_format{}, L"nan"},
    // {-float_nan_payload, chars_format{}, L"-nan"},
    {2.018f, chars_format{}, L"2.018"},
    // {-2.018f, chars_format{}, L"-2.018"},

    // Ditto for hex.
    // {0.0f, chars_format::hex, L"0p+0"},
    // {-0.0f, chars_format::hex, L"-0p+0"},
    // {float_inf, chars_format::hex, L"inf"},
    // {-float_inf, chars_format::hex, L"-inf"},
    // {float_nan, chars_format::hex, L"nan"},
    // {-float_nan, chars_format::hex, L"-nan(ind)"},
    // {float_nan_payload, chars_format::hex, L"nan"},
    // {-float_nan_payload, chars_format::hex, L"-nan"},
    // {0x1.729p+0f, chars_format::hex, L"1.729p+0"},
    // {-0x1.729p+0f, chars_format::hex, L"-1.729p+0"},
    // {0x1.729p-1f, chars_format::hex, L"1.729p-1"},
    // {-0x1.729p-1f, chars_format::hex, L"-1.729p-1"},
};

#endif // WCHAR_TEST_CASES_HPP
