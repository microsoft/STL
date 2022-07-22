// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef FLOAT_FROM_CHARS_TEST_CASES_HPP
#define FLOAT_FROM_CHARS_TEST_CASES_HPP

#include <charconv>
#include <cstddef>
#include <system_error>
using namespace std;

inline constexpr FloatFromCharsTestCase float_from_chars_test_cases[] = {
    {"1.a0000400", chars_format::hex, 10, errc{}, 0x1.a00004p0f}, // exact
    {"1.a0000401", chars_format::hex, 10, errc{}, 0x1.a00004p0f}, // below midpoint, round down
    {"1.a0000500", chars_format::hex, 10, errc{}, 0x1.a00004p0f}, // midpoint, round down to even
    {"1.a0000501", chars_format::hex, 10, errc{}, 0x1.a00006p0f}, // above midpoint, round up
    {"1.a0000600", chars_format::hex, 10, errc{}, 0x1.a00006p0f}, // exact
    {"1.a0000601", chars_format::hex, 10, errc{}, 0x1.a00006p0f}, // below midpoint, round down
    {"1.a0000700", chars_format::hex, 10, errc{}, 0x1.a00008p0f}, // midpoint, round up to even
    {"1.a0000701", chars_format::hex, 10, errc{}, 0x1.a00008p0f}, // above midpoint, round up

    {"1.0000040", chars_format::hex, 9, errc{}, 0x1.000004p0f}, // exact
    {"1.0000041", chars_format::hex, 9, errc{}, 0x1.000004p0f}, // below midpoint, round down
    {"1.0000050", chars_format::hex, 9, errc{}, 0x1.000004p0f}, // midpoint, round down to even
    {"1.0000051", chars_format::hex, 9, errc{}, 0x1.000006p0f}, // above midpoint, round up
    {"1.0000060", chars_format::hex, 9, errc{}, 0x1.000006p0f}, // exact
    {"1.0000061", chars_format::hex, 9, errc{}, 0x1.000006p0f}, // below midpoint, round down
    {"1.0000070", chars_format::hex, 9, errc{}, 0x1.000008p0f}, // midpoint, round up to even
    {"1.0000071", chars_format::hex, 9, errc{}, 0x1.000008p0f}, // above midpoint, round up

    {"1.0000002384185791015625000000", chars_format::general, 30, errc{}, 0x1.000004p0f}, // exact
    {"1.0000002421438694000244140625", chars_format::general, 30, errc{}, 0x1.000004p0f}, // below midpoint, round down
    {"1.0000002980232238769531249999", chars_format::general, 30, errc{}, 0x1.000004p0f}, // below midpoint, round down
    {"1.0000002980232238769531250000", chars_format::general, 30, errc{},
        0x1.000004p0f}, // midpoint, round down to even
    {"1.0000002980232238769531250001", chars_format::general, 30, errc{}, 0x1.000006p0f}, // above midpoint, round up
    {"1.0000003017485141754150390625", chars_format::general, 30, errc{}, 0x1.000006p0f}, // above midpoint, round up
    {"1.0000003576278686523437500000", chars_format::general, 30, errc{}, 0x1.000006p0f}, // exact
    {"1.0000003613531589508056640625", chars_format::general, 30, errc{}, 0x1.000006p0f}, // below midpoint, round down
    {"1.0000004172325134277343749999", chars_format::general, 30, errc{}, 0x1.000006p0f}, // below midpoint, round down
    {"1.0000004172325134277343750000", chars_format::general, 30, errc{}, 0x1.000008p0f}, // midpoint, round up to even
    {"1.0000004172325134277343750001", chars_format::general, 30, errc{}, 0x1.000008p0f}, // above midpoint, round up
    {"1.0000004209578037261962890625", chars_format::general, 30, errc{}, 0x1.000008p0f}, // above midpoint, round up

    // VSO-838635 "<charconv>: from_chars() mishandles certain subnormals"
    // This bug didn't actually affect float, but we should have similar test cases.
    // These values change on half-ulp boundaries:
    // 1 * 2^-150 ~= 7.01e-46 (half-ulp between zero and min subnormal)
    // 2 * 2^-150 ~= 1.40e-45 (min subnormal)
    // 3 * 2^-150 ~= 2.10e-45 (half-ulp between min subnormal and next subnormal)
    // 4 * 2^-150 ~= 2.80e-45 (next subnormal)
    {"6."
     "6666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666"
     "6666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666"
     "6666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666"
     "6666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666"
     "6666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666"
     "6666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666"
     "6666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666"
     "6666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666"
     "6666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666"
     "6666666666666666666e-46",
        chars_format::scientific, 1006, errc::result_out_of_range, 0x0.000000p+0f},
    {"7."
     "7777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777"
     "7777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777"
     "7777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777"
     "7777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777"
     "7777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777"
     "7777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777"
     "7777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777"
     "7777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777"
     "7777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777"
     "7777777777777777777e-46",
        chars_format::scientific, 1006, errc{}, 0x0.000002p-126f},
    {"8."
     "8888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888"
     "8888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888"
     "8888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888"
     "8888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888"
     "8888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888"
     "8888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888"
     "8888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888"
     "8888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888"
     "8888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888"
     "8888888888888888888e-46",
        chars_format::scientific, 1006, errc{}, 0x0.000002p-126f},
    {"9."
     "9999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999"
     "9999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999"
     "9999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999"
     "9999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999"
     "9999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999"
     "9999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999"
     "9999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999"
     "9999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999"
     "9999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999"
     "9999999999999999999e-46",
        chars_format::scientific, 1006, errc{}, 0x0.000002p-126f},
    {"1."
     "1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111"
     "1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111"
     "1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111"
     "1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111"
     "1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111"
     "1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111"
     "1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111"
     "1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111"
     "1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111"
     "1111111111111111111e-45",
        chars_format::scientific, 1006, errc{}, 0x0.000002p-126f},
    {"2."
     "2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222"
     "2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222"
     "2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222"
     "2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222"
     "2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222"
     "2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222"
     "2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222"
     "2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222"
     "2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222"
     "2222222222222222222e-45",
        chars_format::scientific, 1006, errc{}, 0x0.000004p-126f},

    // VSO-733765 "<charconv>: [Feedback] double std::from_chars behavior on exponent out of range"
    // LWG-3081 "Floating point from_chars API does not distinguish between overflow and underflow"
    // These test cases exercise every overflow/underflow codepath.
    {"1e+1000", chars_format::scientific, 7, errc::result_out_of_range, float_inf},
    {"1e-1000", chars_format::scientific, 7, errc::result_out_of_range, 0.0f},
    {"1.ffffffp+127", chars_format::hex, 13, errc::result_out_of_range, float_inf},
    {"1e+2000", chars_format::scientific, 7, errc::result_out_of_range, float_inf},
    {"1e-2000", chars_format::scientific, 7, errc::result_out_of_range, 0.0f},
    {"1e+9999", chars_format::scientific, 7, errc::result_out_of_range, float_inf},
    {"1e-9999", chars_format::scientific, 7, errc::result_out_of_range, 0.0f},
    {"10e+5199", chars_format::scientific, 8, errc::result_out_of_range, float_inf},
    {"0.001e-5199", chars_format::scientific, 11, errc::result_out_of_range, 0.0f},

    // GH-931 "<charconv>: hex from_chars(\"0.fffffffffffff8p-1022\") is incorrect"
    // Test cases for values close to std::numeric_limits<float>::min() and std::numeric_limits<float>::max()
    {"7.fffffp-129", chars_format::hex, 12, errc{}, 0x0.fffffep-126f},
    {"8.00000p-129", chars_format::hex, 12, errc{}, 0x1.000000p-126f},

    {"0.fffffep-126", chars_format::hex, 13, errc{}, 0x0.fffffep-126f},
    {"0.ffffffp-126", chars_format::hex, 13, errc{}, 0x1.000000p-126f},
    {"1.000000p-126", chars_format::hex, 13, errc{}, 0x1.000000p-126f},

    {"1.fffffcp-127", chars_format::hex, 13, errc{}, 0x0.fffffep-126f},
    {"1.fffffdp-127", chars_format::hex, 13, errc{}, 0x0.fffffep-126f},
    {"1.fffffep-127", chars_format::hex, 13, errc{}, 0x1.000000p-126f},
    {"1.ffffffp-127", chars_format::hex, 13, errc{}, 0x1.000000p-126f},
    {"2.000000p-127", chars_format::hex, 13, errc{}, 0x1.000000p-126f},

    {"3.fffff8p-128", chars_format::hex, 13, errc{}, 0x0.fffffep-126f},
    {"3.fffff9p-128", chars_format::hex, 13, errc{}, 0x0.fffffep-126f},
    {"3.fffffbp-128", chars_format::hex, 13, errc{}, 0x0.fffffep-126f},
    {"3.fffffcp-128", chars_format::hex, 13, errc{}, 0x1.000000p-126f},
    {"3.fffffdp-128", chars_format::hex, 13, errc{}, 0x1.000000p-126f},
    {"3.fffffep-128", chars_format::hex, 13, errc{}, 0x1.000000p-126f},
    {"3.ffffffp-128", chars_format::hex, 13, errc{}, 0x1.000000p-126f},
    {"4.000000p-128", chars_format::hex, 13, errc{}, 0x1.000000p-126f},

    {"7.fffff0p-129", chars_format::hex, 13, errc{}, 0x0.fffffep-126f},
    {"7.fffff1p-129", chars_format::hex, 13, errc{}, 0x0.fffffep-126f},
    {"7.fffff7p-129", chars_format::hex, 13, errc{}, 0x0.fffffep-126f},
    {"7.fffff8p-129", chars_format::hex, 13, errc{}, 0x1.000000p-126f},
    {"7.fffff9p-129", chars_format::hex, 13, errc{}, 0x1.000000p-126f},
    {"7.fffffbp-129", chars_format::hex, 13, errc{}, 0x1.000000p-126f},
    {"7.fffffcp-129", chars_format::hex, 13, errc{}, 0x1.000000p-126f},
    {"7.fffffdp-129", chars_format::hex, 13, errc{}, 0x1.000000p-126f},
    {"7.ffffffp-129", chars_format::hex, 13, errc{}, 0x1.000000p-126f},
    {"8.000000p-129", chars_format::hex, 13, errc{}, 0x1.000000p-126f},

    {"0.fffffe0p-126", chars_format::hex, 14, errc{}, 0x0.fffffep-126f},
    {"0.fffffe1p-126", chars_format::hex, 14, errc{}, 0x0.fffffep-126f},
    {"0.fffffefp-126", chars_format::hex, 14, errc{}, 0x0.fffffep-126f},
    {"0.ffffff0p-126", chars_format::hex, 14, errc{}, 0x1.000000p-126f},
    {"0.ffffff1p-126", chars_format::hex, 14, errc{}, 0x1.000000p-126f},
    {"0.ffffff7p-126", chars_format::hex, 14, errc{}, 0x1.000000p-126f},
    {"0.ffffff8p-126", chars_format::hex, 14, errc{}, 0x1.000000p-126f},
    {"0.ffffff9p-126", chars_format::hex, 14, errc{}, 0x1.000000p-126f},
    {"0.fffffffp-126", chars_format::hex, 14, errc{}, 0x1.000000p-126f},
    {"1.0000000p-126", chars_format::hex, 14, errc{}, 0x1.000000p-126f},

    {"1.fffffc0p-127", chars_format::hex, 14, errc{}, 0x0.fffffep-126f},
    {"1.fffffc1p-127", chars_format::hex, 14, errc{}, 0x0.fffffep-126f},
    {"1.fffffdfp-127", chars_format::hex, 14, errc{}, 0x0.fffffep-126f},
    {"1.fffffe0p-127", chars_format::hex, 14, errc{}, 0x1.000000p-126f},
    {"1.fffffe1p-127", chars_format::hex, 14, errc{}, 0x1.000000p-126f},
    {"1.fffffefp-127", chars_format::hex, 14, errc{}, 0x1.000000p-126f},
    {"1.ffffff0p-127", chars_format::hex, 14, errc{}, 0x1.000000p-126f},
    {"1.ffffff1p-127", chars_format::hex, 14, errc{}, 0x1.000000p-126f},
    {"1.fffffffp-127", chars_format::hex, 14, errc{}, 0x1.000000p-126f},
    {"2.0000000p-127", chars_format::hex, 14, errc{}, 0x1.000000p-126f},

    {"3.fffff80p-128", chars_format::hex, 14, errc{}, 0x0.fffffep-126f},
    {"3.fffff81p-128", chars_format::hex, 14, errc{}, 0x0.fffffep-126f},
    {"3.fffffbfp-128", chars_format::hex, 14, errc{}, 0x0.fffffep-126f},
    {"3.fffffc0p-128", chars_format::hex, 14, errc{}, 0x1.000000p-126f},
    {"3.fffffc1p-128", chars_format::hex, 14, errc{}, 0x1.000000p-126f},
    {"3.fffffdfp-128", chars_format::hex, 14, errc{}, 0x1.000000p-126f},
    {"3.fffffe0p-128", chars_format::hex, 14, errc{}, 0x1.000000p-126f},
    {"3.fffffe1p-128", chars_format::hex, 14, errc{}, 0x1.000000p-126f},
    {"3.fffffffp-128", chars_format::hex, 14, errc{}, 0x1.000000p-126f},
    {"4.0000000p-128", chars_format::hex, 14, errc{}, 0x1.000000p-126f},

    {"0.ffffffp+128", chars_format::hex, 13, errc{}, 0x1.fffffep+127f},
    {"1.000000p+128", chars_format::hex, 13, errc::result_out_of_range, float_inf},

    {"1.fffffep+127", chars_format::hex, 13, errc{}, 0x1.fffffep+127f},
    {"1.ffffffp+127", chars_format::hex, 13, errc::result_out_of_range, float_inf},
    {"2.000000p+127", chars_format::hex, 13, errc::result_out_of_range, float_inf},

    {"3.fffffcp+126", chars_format::hex, 13, errc{}, 0x1.fffffep+127f},
    {"3.fffffdp+126", chars_format::hex, 13, errc{}, 0x1.fffffep+127f},
    {"3.fffffep+126", chars_format::hex, 13, errc::result_out_of_range, float_inf},
    {"3.ffffffp+126", chars_format::hex, 13, errc::result_out_of_range, float_inf},
    {"4.000000p+126", chars_format::hex, 13, errc::result_out_of_range, float_inf},

    {"7.fffff8p+125", chars_format::hex, 13, errc{}, 0x1.fffffep+127f},
    {"7.fffff9p+125", chars_format::hex, 13, errc{}, 0x1.fffffep+127f},
    {"7.fffffbp+125", chars_format::hex, 13, errc{}, 0x1.fffffep+127f},
    {"7.fffffcp+125", chars_format::hex, 13, errc::result_out_of_range, float_inf},
    {"7.fffffdp+125", chars_format::hex, 13, errc::result_out_of_range, float_inf},
    {"7.ffffffp+125", chars_format::hex, 13, errc::result_out_of_range, float_inf},
    {"8.000000p+125", chars_format::hex, 13, errc::result_out_of_range, float_inf},

    {"0.ffffff0p+128", chars_format::hex, 14, errc{}, 0x1.fffffep+127f},
    {"0.ffffff1p+128", chars_format::hex, 14, errc{}, 0x1.fffffep+127f},
    {"0.ffffff7p+128", chars_format::hex, 14, errc{}, 0x1.fffffep+127f},
    {"0.ffffff8p+128", chars_format::hex, 14, errc::result_out_of_range, float_inf},
    {"0.ffffff9p+128", chars_format::hex, 14, errc::result_out_of_range, float_inf},
    {"0.fffffffp+128", chars_format::hex, 14, errc::result_out_of_range, float_inf},
    {"1.0000000p+128", chars_format::hex, 14, errc::result_out_of_range, float_inf},

    {"1.fffffe0p+127", chars_format::hex, 14, errc{}, 0x1.fffffep+127f},
    {"1.fffffe1p+127", chars_format::hex, 14, errc{}, 0x1.fffffep+127f},
    {"1.fffffefp+127", chars_format::hex, 14, errc{}, 0x1.fffffep+127f},
    {"1.ffffff0p+127", chars_format::hex, 14, errc::result_out_of_range, float_inf},
    {"1.ffffff1p+127", chars_format::hex, 14, errc::result_out_of_range, float_inf},
    {"1.fffffffp+127", chars_format::hex, 14, errc::result_out_of_range, float_inf},
    {"2.0000000p+127", chars_format::hex, 14, errc::result_out_of_range, float_inf},

    {"3.fffffc0p+126", chars_format::hex, 14, errc{}, 0x1.fffffep+127f},
    {"3.fffffc1p+126", chars_format::hex, 14, errc{}, 0x1.fffffep+127f},
    {"3.fffffdfp+126", chars_format::hex, 14, errc{}, 0x1.fffffep+127f},
    {"3.fffffe0p+126", chars_format::hex, 14, errc::result_out_of_range, float_inf},
    {"3.fffffe1p+126", chars_format::hex, 14, errc::result_out_of_range, float_inf},
    {"3.fffffffp+126", chars_format::hex, 14, errc::result_out_of_range, float_inf},
    {"4.0000000p+126", chars_format::hex, 14, errc::result_out_of_range, float_inf},

    {"7.fffff80p+125", chars_format::hex, 14, errc{}, 0x1.fffffep+127f},
    {"7.fffff81p+125", chars_format::hex, 14, errc{}, 0x1.fffffep+127f},
    {"7.fffffbfp+125", chars_format::hex, 14, errc{}, 0x1.fffffep+127f},
    {"7.fffffc0p+125", chars_format::hex, 14, errc::result_out_of_range, float_inf},
    {"7.fffffc1p+125", chars_format::hex, 14, errc::result_out_of_range, float_inf},
    {"7.fffffffp+125", chars_format::hex, 14, errc::result_out_of_range, float_inf},
    {"8.0000000p+125", chars_format::hex, 14, errc::result_out_of_range, float_inf},

    // GH-2363 "<charconv>: from_chars() incorrectly tiebreaks floats"

    // Exhaustive testing found all affected tiebreaker values, which were in a narrow range of exponents.
    // Let's test the smallest, the largest, and 20 randomly selected values in between.
    {/* 0x1.000005p+15 in decimal: */ "32768.009765625", chars_format::general, 15, errc{}, 0x1.000004p+15f},
    {/* 0x1.016d75p+15 in decimal: */ "32950.728515625", chars_format::general, 15, errc{}, 0x1.016d74p+15f},
    {/* 0x1.372ad9p+15 in decimal: */ "39829.423828125", chars_format::general, 15, errc{}, 0x1.372ad8p+15f},
    {/* 0x1.464ad5p+15 in decimal: */ "41765.416015625", chars_format::general, 15, errc{}, 0x1.464ad4p+15f},
    {/* 0x1.4aff95p+15 in decimal: */ "42367.791015625", chars_format::general, 15, errc{}, 0x1.4aff94p+15f},
    {/* 0x1.83c87dp+15 in decimal: */ "49636.244140625", chars_format::general, 15, errc{}, 0x1.83c87cp+15f},
    {/* 0x1.861e69p+15 in decimal: */ "49935.205078125", chars_format::general, 15, errc{}, 0x1.861e68p+15f},
    {/* 0x1.96a091p+15 in decimal: */ "52048.283203125", chars_format::general, 15, errc{}, 0x1.96a090p+15f},
    {/* 0x1.d472b9p+15 in decimal: */ "59961.361328125", chars_format::general, 15, errc{}, 0x1.d472b8p+15f},
    {/* 0x1.d9baa1p+15 in decimal: */ "60637.314453125", chars_format::general, 15, errc{}, 0x1.d9baa0p+15f},
    {/* 0x1.ecfe65p+15 in decimal: */ "63103.197265625", chars_format::general, 15, errc{}, 0x1.ecfe64p+15f},
    {/* 0x1.fc8e3dp+15 in decimal: */ "65095.119140625", chars_format::general, 15, errc{}, 0x1.fc8e3cp+15f},
    {/* 0x1.fd1359p+15 in decimal: */ "65161.673828125", chars_format::general, 15, errc{}, 0x1.fd1358p+15f},
    {/* 0x1.0adaa5p+16 in decimal: */ "68314.64453125", chars_format::general, 14, errc{}, 0x1.0adaa4p+16f},
    {/* 0x1.1e315dp+16 in decimal: */ "73265.36328125", chars_format::general, 14, errc{}, 0x1.1e315cp+16f},
    {/* 0x1.28a1f5p+16 in decimal: */ "75937.95703125", chars_format::general, 14, errc{}, 0x1.28a1f4p+16f},
    {/* 0x1.32aca9p+16 in decimal: */ "78508.66015625", chars_format::general, 14, errc{}, 0x1.32aca8p+16f},
    {/* 0x1.3464b5p+16 in decimal: */ "78948.70703125", chars_format::general, 14, errc{}, 0x1.3464b4p+16f},
    {/* 0x1.a7bd5dp+16 in decimal: */ "108477.36328125", chars_format::general, 15, errc{}, 0x1.a7bd5cp+16f},
    {/* 0x1.b85e8dp+16 in decimal: */ "112734.55078125", chars_format::general, 15, errc{}, 0x1.b85e8cp+16f},
    {/* 0x1.dd5e49p+16 in decimal: */ "122206.28515625", chars_format::general, 15, errc{}, 0x1.dd5e48p+16f},
    {/* 0x1.fffffdp+16 in decimal: */ "131071.98828125", chars_format::general, 15, errc{}, 0x1.fffffcp+16f},

    // Let's also test unaffected tiebreaker values, randomly selected for each exponent.
    {/* 0x0.799f73p-127 in decimal: */ "2."
                                       "7923236592830528266962495740723380649230754633916493372624425609233151344967804"
                                       "874386274605058133602142333984375e-39",
        chars_format::general, 118, errc{}, 0x0.799f74p-127f},
    {/* 0x1.5b6b41p-127 in decimal: */ "7."
                                       "9763481546894792284140799767319269123329871825641034117366124028155574716969500"
                                       "315144614432938396930694580078125e-39",
        chars_format::general, 118, errc{}, 0x1.5b6b40p-127f},
    {/* 0x1.9eea7bp-126 in decimal: */ "1."
                                       "9052005576163193690090080545739076218779912192716113693014974768909805362404608"
                                       "075621581519953906536102294921875e-38",
        chars_format::general, 118, errc{}, 0x1.9eea7cp-126f},
    {/* 0x1.cc3a7dp-125 in decimal: */ "4."
                                       "2265309874582292803629410309475053960841952808552612374871283027795302103535313"
                                       "16513908677734434604644775390625e-38",
        chars_format::general, 117, errc{}, 0x1.cc3a7cp-125f},
    {/* 0x1.13100dp-124 in decimal: */ "5."
                                       "0521038757674987704720499293483845499537646835188976896441474466307747920312820"
                                       "6108885933645069599151611328125e-38",
        chars_format::general, 116, errc{}, 0x1.13100cp-124f},
    {/* 0x1.edfadfp-123 in decimal: */ "1."
                                       "8145958078865600010341721150591273787373501139097227567208027653410367524555724"
                                       "1169080953113734722137451171875e-37",
        chars_format::general, 116, errc{}, 0x1.edfae0p-123f},
    {/* 0x1.ab296bp-122 in decimal: */ "3."
                                       "1382891861663586697238059780935979911377608740250859087647163317316527375577805"
                                       "969442124478518962860107421875e-37",
        chars_format::general, 115, errc{}, 0x1.ab296cp-122f},
    {/* 0x1.db050dp-121 in decimal: */ "6."
                                       "9797876086336315967200058909332747298975660039248050725410743583146735979916286"
                                       "39647387899458408355712890625e-37",
        chars_format::general, 114, errc{}, 0x1.db050cp-121f},
    {/* 0x1.ab219fp-120 in decimal: */ "1."
                                       "2552261707310301131735683507814953029319672167192828266905128652272324862870789"
                                       "27489346824586391448974609375e-36",
        chars_format::general, 114, errc{}, 0x1.ab21a0p-120f},
    {/* 0x1.fedb07p-119 in decimal: */ "3."
                                       "0025392157931951816712390941704640072310061660787724992609610478577070180605801"
                                       "3971778564155101776123046875e-36",
        chars_format::general, 113, errc{}, 0x1.fedb08p-119f},
    {/* 0x1.06fc5dp-118 in decimal: */ "3."
                                       "0913831527272194861570692538875246399375580080555006409365282411989828137421909"
                                       "559634514153003692626953125e-36",
        chars_format::general, 112, errc{}, 0x1.06fc5cp-118f},
    {/* 0x1.505a0fp-117 in decimal: */ "7."
                                       "9075926131660942698878670560324347031415391499103386907449834237679442416890651"
                                       "65701322257518768310546875e-36",
        chars_format::general, 111, errc{}, 0x1.505a10p-117f},
    {/* 0x1.cfa54dp-116 in decimal: */ "2."
                                       "1800516335751559288984023755321342596236508175851387819513764855863802161195508"
                                       "23350436985492706298828125e-35",
        chars_format::general, 111, errc{}, 0x1.cfa54cp-116f},
    {/* 0x1.587015p-115 in decimal: */ "3."
                                       "2390776970430309122944828427039938718649816467607669767768809130498164350342449"
                                       "3246711790561676025390625e-35",
        chars_format::general, 110, errc{}, 0x1.587014p-115f},
    {/* 0x1.15d717p-114 in decimal: */
        "5.2255932688598390793241011093237458992089588485788678365369526977735059602991896099410951137542724609375e-35",
        chars_format::general, 109, errc{}, 0x1.15d718p-114f},
    {/* 0x1.7c8469p-113 in decimal: */
        "1.4313467229831937522513906733248592628671955253540919930841626879491901291885369573719799518585205078125e-34",
        chars_format::general, 109, errc{}, 0x1.7c8468p-113f},
    {/* 0x1.925f4bp-112 in decimal: */
        "3.027112274456549045009474867635856246621392661089931372514201350643059384992739069275557994842529296875e-34",
        chars_format::general, 108, errc{}, 0x1.925f4cp-112f},
    {/* 0x1.f46529p-111 in decimal: */
        "7.52910950487264879701434086363339665372666073168583801862173594809934940030871075578033924102783203125e-34",
        chars_format::general, 107, errc{}, 0x1.f46528p-111f},
    {/* 0x1.7c2ffdp-110 in decimal: */
        "1.14408500401507174876899455686135431757543544372026740870503143387981737078007427044212818145751953125e-33",
        chars_format::general, 107, errc{}, 0x1.7c2ffcp-110f},
    {/* 0x1.474a3fp-109 in decimal: */
        "1.9698051791961815762996994737792056679747397605386597391609153351321737090984242968261241912841796875e-33",
        chars_format::general, 106, errc{}, 0x1.474a40p-109f},
    {/* 0x1.b5b0ebp-108 in decimal: */
        "5.268514803520621083584672887951853477662515936333867229489184846880078794129076413810253143310546875e-33",
        chars_format::general, 105, errc{}, 0x1.b5b0ecp-108f},
    {/* 0x1.37b391p-107 in decimal: */
        "7.50393900249692617077193139238627039193050960649333913751908575751059515823726542294025421142578125e-33",
        chars_format::general, 104, errc{}, 0x1.37b390p-107f},
    {/* 0x1.c0279dp-106 in decimal: */
        "2.15778658072693470652383973052161677135134628896443638739095771672538148777675814926624298095703125e-32",
        chars_format::general, 104, errc{}, 0x1.c0279cp-106f},
    {/* 0x1.1aed1fp-105 in decimal: */
        "2.7244807257832482176334571757971227665613016308952893588905819211021253067883662879467010498046875e-32",
        chars_format::general, 103, errc{}, 0x1.1aed20p-105f},
    {/* 0x1.7b06a7p-104 in decimal: */
        "7.299774955947486471988674091977596908356736543597116976221650386236206031753681600093841552734375e-32",
        chars_format::general, 102, errc{}, 0x1.7b06a8p-104f},
    {/* 0x1.b1ee17p-103 in decimal: */
        "1.671437709648194711041387021312558712669432301572066952403960993223108744132332503795623779296875e-31",
        chars_format::general, 102, errc{}, 0x1.b1ee18p-103f},
    {/* 0x1.517dd9p-102 in decimal: */
        "2.59994065518403809358388114510645093428958941661859592155148224179583849036134779453277587890625e-31",
        chars_format::general, 101, errc{}, 0x1.517dd8p-102f},
    {/* 0x1.399b9dp-101 in decimal: */
        "4.8318942144363800695431588797193225258328589630883579709996755724432659917511045932769775390625e-31",
        chars_format::general, 100, errc{}, 0x1.399b9cp-101f},
    {/* 0x1.e64a5dp-100 in decimal: */
        "1.4984982401937787548278964816524034785339731991004566698244548916818530415184795856475830078125e-30",
        chars_format::general, 100, errc{}, 0x1.e64a5cp-100f},
    {/* 0x1.ccf2a7p-99 in decimal: */
        "2.840810520824309549915000327081469707228919041030741447773078078853359329514205455780029296875e-30",
        chars_format::general, 99, errc{}, 0x1.ccf2a8p-99f},
    {/* 0x1.fa99f9p-98 in decimal: */
        "6.24434504563584252360873555936105890961500091202091879127866747012376436032354831695556640625e-30",
        chars_format::general, 98, errc{}, 0x1.fa99f8p-98f},
    {/* 0x1.cfe385p-97 in decimal: */
        "1.14357405563248806830394721780106763644256610240970174763308619958479539491236209869384765625e-29",
        chars_format::general, 98, errc{}, 0x1.cfe384p-97f},
    {/* 0x1.9fd427p-96 in decimal: */
        "2.0501938784329999627247695294069037358553918019474747447272733325007720850408077239990234375e-29",
        chars_format::general, 97, errc{}, 0x1.9fd428p-96f},
    {/* 0x1.27ed4fp-95 in decimal: */
        "2.918065382537752045370514999791991168758113433623173349484858363211969844996929168701171875e-29",
        chars_format::general, 96, errc{}, 0x1.27ed50p-95f},
    {/* 0x1.c7d9afp-94 in decimal: */
        "8.99006253095320733224329531803019808826400019788858963831756909712566994130611419677734375e-29",
        chars_format::general, 95, errc{}, 0x1.c7d9b0p-94f},
    {/* 0x1.5dd31bp-93 in decimal: */
        "1.37981487436018183129367585316693438412586256137069840033149148439406417310237884521484375e-28",
        chars_format::general, 95, errc{}, 0x1.5dd31cp-93f},
    {/* 0x1.944a29p-92 in decimal: */
        "3.1892832933425246734052751375699547957011568640774135108273412697599269449710845947265625e-28",
        chars_format::general, 94, errc{}, 0x1.944a28p-92f},
    {/* 0x1.0aba31p-91 in decimal: */
        "4.208214947125685004407385115883586561941223061662775695168647871469147503376007080078125e-28",
        chars_format::general, 93, errc{}, 0x1.0aba30p-91f},
    {/* 0x1.2e0653p-90 in decimal: */
        "9.53021925521481334308520547568831890378451203275911307599699284764938056468963623046875e-28",
        chars_format::general, 92, errc{}, 0x1.2e0654p-90f},
    {/* 0x1.fc13d5p-89 in decimal: */
        "3.20641961613457467701456314930996842119172184938646896767977523268200457096099853515625e-27",
        chars_format::general, 92, errc{}, 0x1.fc13d4p-89f},
    {/* 0x1.f77519p-88 in decimal: */
        "6.3545259254129926268702973984743504130327470570682724115840755985118448734283447265625e-27",
        chars_format::general, 91, errc{}, 0x1.f77518p-88f},
    {/* 0x1.03ca9bp-87 in decimal: */
        "6.558057624156852395650497230653342756577658077821180171440573758445680141448974609375e-27",
        chars_format::general, 90, errc{}, 0x1.03ca9cp-87f},
    {/* 0x1.4dd65dp-86 in decimal: */
        "1.685447931509345893369802034392883334937914170670314462086025741882622241973876953125e-26",
        chars_format::general, 90, errc{}, 0x1.4dd65cp-86f},
    {/* 0x1.84523dp-85 in decimal: */
        "3.92104252793916778850995974691044876903598705164544213630506419576704502105712890625e-26",
        chars_format::general, 89, errc{}, 0x1.84523cp-85f},
    {/* 0x1.67e04bp-84 in decimal: */
        "7.2676408587794702077274095979725639958611387025921857230059686116874217987060546875e-26",
        chars_format::general, 88, errc{}, 0x1.67e04cp-84f},
    {/* 0x1.69a8e5p-83 in decimal: */
        "1.4607320741942756095091207439691928681086731389715538398377248086035251617431640625e-25",
        chars_format::general, 88, errc{}, 0x1.69a8e4p-83f},
    {/* 0x1.8e5021p-82 in decimal: */
        "3.217546818907087816811047188015620590059197303478555340916500426828861236572265625e-25",
        chars_format::general, 87, errc{}, 0x1.8e5020p-82f},
    {/* 0x1.67d5edp-81 in decimal: */
        "5.81345842551030848951581432599921978585967574648663003245019353926181793212890625e-25", chars_format::general,
        86, errc{}, 0x1.67d5ecp-81f},
    {/* 0x1.bc4d5bp-80 in decimal: */
        "1.43561773817828810465400997910748526263484403653336585193756036460399627685546875e-24", chars_format::general,
        86, errc{}, 0x1.bc4d5cp-80f},
    {/* 0x1.830155p-79 in decimal: */
        "2.5009625084618796563722303923603734938409248744761725902208127081394195556640625e-24", chars_format::general,
        85, errc{}, 0x1.830154p-79f},
    {/* 0x1.f228bbp-78 in decimal: */
        "6.438555504592931467616905908606949357313176118822184434975497424602508544921875e-24", chars_format::general,
        84, errc{}, 0x1.f228bcp-78f},
    {/* 0x1.205c01p-77 in decimal: */
        "7.45391553342758431936910945007871841412605551369097156566567718982696533203125e-24", chars_format::general,
        83, errc{}, 0x1.205c00p-77f},
    {/* 0x1.613789p-76 in decimal: */
        "1.82608874999407002016879928138061808286229936726385858491994440555572509765625e-23", chars_format::general,
        83, errc{}, 0x1.613788p-76f},
    {/* 0x1.2557d9p-75 in decimal: */ "3.0330971320549669261149152074471678670886110040783023578114807605743408203125e-"
                                      "23",
        chars_format::general, 82, errc{}, 0x1.2557d8p-75f},
    {/* 0x1.b5840fp-74 in decimal: */ "9.047615800390950046412361683906950236833122147572794347070157527923583984375e-"
                                      "23",
        chars_format::general, 81, errc{}, 0x1.b58410p-74f},
    {/* 0x1.8a1713p-73 in decimal: */ "1.629918590838835760835440489828636854985344228907706565223634243011474609375e-"
                                      "22",
        chars_format::general, 81, errc{}, 0x1.8a1714p-73f},
    {/* 0x1.dc6007p-72 in decimal: */ "3.94048252657369943370189521798427866039471467729526921175420284271240234375e-"
                                      "22",
        chars_format::general, 80, errc{}, 0x1.dc6008p-72f},
    {/* 0x1.14c34dp-71 in decimal: */ "4.5786579984697799420054304420112968598832736688564182259142398834228515625e-22",
        chars_format::general, 79, errc{}, 0x1.14c34cp-71f},
    {/* 0x1.0e62a9p-70 in decimal: */ "8.946302141897925829519555652514541110864598749685683287680149078369140625e-22",
        chars_format::general, 78, errc{}, 0x1.0e62a8p-70f},
    {/* 0x1.0de299p-69 in decimal: */ "1.785950090342239162572248012514526671612458130766754038631916046142578125e-21",
        chars_format::general, 78, errc{}, 0x1.0de298p-69f},
    {/* 0x1.799ce9p-68 in decimal: */ "4.99766551986752957756650266409916005816427286845282651484012603759765625e-21",
        chars_format::general, 77, errc{}, 0x1.799ce8p-68f},
    {/* 0x1.7699a3p-67 in decimal: */ "9.9155832354254179919041673591482977112576691069989465177059173583984375e-21",
        chars_format::general, 76, errc{}, 0x1.7699a4p-67f},
    {/* 0x1.d0120bp-66 in decimal: */ "2.4567686668860434866995250016534491077635493638808839023113250732421875e-20",
        chars_format::general, 76, errc{}, 0x1.d0120cp-66f},
    {/* 0x1.4c97cdp-65 in decimal: */ "3.521465064266366565038455919457727194554763627820648252964019775390625e-20",
        chars_format::general, 75, errc{}, 0x1.4c97ccp-65f},
    {/* 0x1.ecb09bp-64 in decimal: */ "1.043311371320997812921560553116318015298702448490075767040252685546875e-19",
        chars_format::general, 75, errc{}, 0x1.ecb09cp-64f},
    {/* 0x1.f34fabp-63 in decimal: */ "2.11466519938330895530585545015467285878685288480482995510101318359375e-19",
        chars_format::general, 74, errc{}, 0x1.f34facp-63f},
    {/* 0x1.bc3793p-62 in decimal: */ "3.7626650824614047437024762048085602117453163373284041881561279296875e-19",
        chars_format::general, 73, errc{}, 0x1.bc3794p-62f},
    {/* 0x1.35e485p-61 in decimal: */ "5.249785768098752555444624391667307605757741839624941349029541015625e-19",
        chars_format::general, 72, errc{}, 0x1.35e484p-61f},
    {/* 0x1.4d2243p-60 in decimal: */ "1.128701335814771964497524141257667196214242721907794475555419921875e-18",
        chars_format::general, 72, errc{}, 0x1.4d2244p-60f},
    {/* 0x1.dd0c7fp-59 in decimal: */ "3.23260849556985473582620786714425520358417998068034648895263671875e-18",
        chars_format::general, 71, errc{}, 0x1.dd0c80p-59f},
    {/* 0x1.2b6729p-58 in decimal: */ "4.0576668728638010490692746012886260587038123048841953277587890625e-18",
        chars_format::general, 70, errc{}, 0x1.2b6728p-58f},
    {/* 0x1.092477p-57 in decimal: */ "7.186700258225558131971837105833600389814819209277629852294921875e-18",
        chars_format::general, 69, errc{}, 0x1.092478p-57f},
    {/* 0x1.8a701fp-56 in decimal: */ "2.138252536308654690329865422082633585887379013001918792724609375e-17",
        chars_format::general, 69, errc{}, 0x1.8a7020p-56f},
    {/* 0x1.36725fp-55 in decimal: */ "3.36587053893605176488161567771584259389783255755901336669921875e-17",
        chars_format::general, 68, errc{}, 0x1.367260p-55f},
    {/* 0x1.ce168dp-54 in decimal: */ "1.00199381992365684217356769369899893717956729233264923095703125e-16",
        chars_format::general, 68, errc{}, 0x1.ce168cp-54f},
    {/* 0x1.b88fbfp-53 in decimal: */ "1.9106309771233949130857243670078560171532444655895233154296875e-16",
        chars_format::general, 67, errc{}, 0x1.b88fc0p-53f},
    {/* 0x1.03a4a5p-52 in decimal: */ "2.252045275092124797418347004196448324364610016345977783203125e-16",
        chars_format::general, 66, errc{}, 0x1.03a4a4p-52f},
    {/* 0x1.68bd9fp-51 in decimal: */ "6.25785373862855725905773862649539296398870646953582763671875e-16",
        chars_format::general, 65, errc{}, 0x1.68bda0p-51f},
    {/* 0x1.7ed3e9p-50 in decimal: */ "1.32820065379350552882409264299212736659683287143707275390625e-15",
        chars_format::general, 65, errc{}, 0x1.7ed3e8p-50f},
    {/* 0x1.e28397p-49 in decimal: */ "3.3481136115450535299664647226336455787532031536102294921875e-15",
        chars_format::general, 64, errc{}, 0x1.e28398p-49f},
    {/* 0x1.26d23dp-48 in decimal: */ "4.091466655560993710694095426561034400947391986846923828125e-15",
        chars_format::general, 63, errc{}, 0x1.26d23cp-48f},
    {/* 0x1.fb5971p-47 in decimal: */ "1.408177409382050000730135508320017834194004535675048828125e-14",
        chars_format::general, 63, errc{}, 0x1.fb5970p-47f},
    {/* 0x1.f732b1p-46 in decimal: */ "2.79331010158792060471544260735754505731165409088134765625e-14",
        chars_format::general, 62, errc{}, 0x1.f732b0p-46f},
    {/* 0x1.2ed21dp-45 in decimal: */ "3.3619857454079450642669968374320887960493564605712890625e-14",
        chars_format::general, 61, errc{}, 0x1.2ed21cp-45f},
    {/* 0x1.93dbddp-44 in decimal: */ "8.967467678253245033648255457592313177883625030517578125e-14",
        chars_format::general, 60, errc{}, 0x1.93dbdcp-44f},
    {/* 0x1.593589p-43 in decimal: */ "1.533036460906085640676366210755077190697193145751953125e-13",
        chars_format::general, 60, errc{}, 0x1.593588p-43f},
    {/* 0x1.22a9f5p-42 in decimal: */ "2.58161398617069715033522925295983441174030303955078125e-13",
        chars_format::general, 59, errc{}, 0x1.22a9f4p-42f},
    {/* 0x1.7213d5p-41 in decimal: */ "6.5738964293883539458107634345651604235172271728515625e-13",
        chars_format::general, 58, errc{}, 0x1.7213d4p-41f},
    {/* 0x1.ea12ebp-40 in decimal: */ "1.7410922421683128202829493602621369063854217529296875e-12",
        chars_format::general, 58, errc{}, 0x1.ea12ecp-40f},
    {/* 0x1.82499fp-39 in decimal: */ "2.744738355868470147669313519145362079143524169921875e-12",
        chars_format::general, 57, errc{}, 0x1.8249a0p-39f},
    {/* 0x1.ce1b23p-38 in decimal: */ "6.56692126892177707730979818734340369701385498046875e-12", chars_format::general,
        56, errc{}, 0x1.ce1b24p-38f},
    {/* 0x1.fce121p-37 in decimal: */ "1.44632227201679786077193057280965149402618408203125e-11", chars_format::general,
        56, errc{}, 0x1.fce120p-37f},
    {/* 0x1.9a078fp-36 in decimal: */ "2.3307480077894293657436719513498246669769287109375e-11", chars_format::general,
        55, errc{}, 0x1.9a0790p-36f},
    {/* 0x1.71a5b7p-35 in decimal: */ "4.202403529329767906119741383008658885955810546875e-11", chars_format::general,
        54, errc{}, 0x1.71a5b8p-35f},
    {/* 0x1.0f6261p-34 in decimal: */ "6.17056440666008398920894251205027103424072265625e-11", chars_format::general,
        53, errc{}, 0x1.0f6260p-34f},
    {/* 0x1.4c4a83p-33 in decimal: */ "1.51108479895523117875200114212930202484130859375e-10", chars_format::general,
        53, errc{}, 0x1.4c4a84p-33f},
    {/* 0x1.ecaaddp-32 in decimal: */ "4.4807842158878230520713259465992450714111328125e-10", chars_format::general, 52,
        errc{}, 0x1.ecaadcp-32f},
    {/* 0x1.d7136bp-31 in decimal: */ "8.568819820364836914450279437005519866943359375e-10", chars_format::general, 51,
        errc{}, 0x1.d7136cp-31f},
    {/* 0x1.f66321p-30 in decimal: */ "1.827674067644835531609714962542057037353515625e-09", chars_format::general, 51,
        errc{}, 0x1.f66320p-30f},
    {/* 0x1.e90f7bp-29 in decimal: */ "3.55838325472035421626060269773006439208984375e-09", chars_format::general, 50,
        errc{}, 0x1.e90f7cp-29f},
    {/* 0x1.d441ddp-28 in decimal: */ "6.8140402209593275983934290707111358642578125e-09", chars_format::general, 49,
        errc{}, 0x1.d441dcp-28f},
    {/* 0x1.68c41fp-27 in decimal: */ "1.0499675351383075394551269710063934326171875e-08", chars_format::general, 49,
        errc{}, 0x1.68c420p-27f},
    {/* 0x1.73daffp-26 in decimal: */ "2.164483614564005620195530354976654052734375e-08", chars_format::general, 48,
        errc{}, 0x1.73db00p-26f},
    {/* 0x1.2231c1p-25 in decimal: */ "3.37830687868745371815748512744903564453125e-08", chars_format::general, 47,
        errc{}, 0x1.2231c0p-25f},
    {/* 0x1.8ba059p-24 in decimal: */ "9.2113939587079585180617868900299072265625e-08", chars_format::general, 46,
        errc{}, 0x1.8ba058p-24f},
    {/* 0x1.f22fc5p-23 in decimal: */ "2.3198621335041025304235517978668212890625e-07", chars_format::general, 46,
        errc{}, 0x1.f22fc4p-23f},
    {/* 0x1.eea199p-22 in decimal: */ "4.606612407087595784105360507965087890625e-07", chars_format::general, 45,
        errc{}, 0x1.eea198p-22f},
    {/* 0x1.a7aa29p-21 in decimal: */ "7.89136976209192653186619281768798828125e-07", chars_format::general, 44, errc{},
        0x1.a7aa28p-21f},
    {/* 0x1.b1b287p-20 in decimal: */ "1.61564861400620429776608943939208984375e-06", chars_format::general, 44, errc{},
        0x1.b1b288p-20f},
    {/* 0x1.df0cc5p-19 in decimal: */ "3.5691997481990256346762180328369140625e-06", chars_format::general, 43, errc{},
        0x1.df0cc4p-19f},
    {/* 0x1.ba305bp-18 in decimal: */ "6.589127906408975832164287567138671875e-06", chars_format::general, 42, errc{},
        0x1.ba305cp-18f},
    {/* 0x1.35f055p-17 in decimal: */ "9.23689594856114126741886138916015625e-06", chars_format::general, 41, errc{},
        0x1.35f054p-17f},
    {/* 0x1.28b507p-16 in decimal: */ "1.76851235664798878133296966552734375e-05", chars_format::general, 41, errc{},
        0x1.28b508p-16f},
    {/* 0x1.7bf969p-15 in decimal: */ "4.5296461394173093140125274658203125e-05", chars_format::general, 40, errc{},
        0x1.7bf968p-15f},
    {/* 0x1.56d069p-14 in decimal: */ "8.173325113602913916110992431640625e-05", chars_format::general, 39, errc{},
        0x1.56d068p-14f},
    {/* 0x1.94fbffp-13 in decimal: */ "0.0001931115912157110869884490966796875", chars_format::general, 39, errc{},
        0x1.94fc00p-13f},
    {/* 0x1.9370a9p-12 in decimal: */ "0.000384750441298820078372955322265625", chars_format::general, 38, errc{},
        0x1.9370a8p-12f},
    {/* 0x1.af2d51p-11 in decimal: */ "0.00082240489427931606769561767578125", chars_format::general, 37, errc{},
        0x1.af2d50p-11f},
    {/* 0x1.549d27p-10 in decimal: */ "0.0012993388227187097072601318359375", chars_format::general, 36, errc{},
        0x1.549d28p-10f},
    {/* 0x1.448f29p-9 in decimal: */ "0.002476190333254635334014892578125", chars_format::general, 35, errc{},
        0x1.448f28p-9f},
    {/* 0x1.b0b13bp-8 in decimal: */ "0.00660236063413321971893310546875", chars_format::general, 34, errc{},
        0x1.b0b13cp-8f},
    {/* 0x1.6db90dp-7 in decimal: */ "0.0111609757877886295318603515625", chars_format::general, 33, errc{},
        0x1.6db90cp-7f},
    {/* 0x1.d3d659p-6 in decimal: */ "0.028554522432386875152587890625", chars_format::general, 32, errc{},
        0x1.d3d658p-6f},
    {/* 0x1.0004a1p-5 in decimal: */ "0.03125220723450183868408203125", chars_format::general, 31, errc{},
        0x1.0004a0p-5f},
    {/* 0x1.0717cdp-4 in decimal: */ "0.0642316825687885284423828125", chars_format::general, 30, errc{},
        0x1.0717ccp-4f},
    {/* 0x1.6fbc2bp-3 in decimal: */ "0.179558120667934417724609375", chars_format::general, 29, errc{},
        0x1.6fbc2cp-3f},
    {/* 0x1.52282fp-2 in decimal: */ "0.33023141324520111083984375", chars_format::general, 28, errc{}, 0x1.522830p-2f},
    {/* 0x1.73fdf7p-1 in decimal: */ "0.7265469729900360107421875", chars_format::general, 27, errc{}, 0x1.73fdf8p-1f},
    {/* 0x1.12b187p+0 in decimal: */ "1.073021352291107177734375", chars_format::general, 26, errc{}, 0x1.12b188p+0f},
    {/* 0x1.8672e3p+1 in decimal: */ "3.05038106441497802734375", chars_format::general, 25, errc{}, 0x1.8672e4p+1f},
    {/* 0x1.7722bdp+2 in decimal: */ "5.8614952564239501953125", chars_format::general, 24, errc{}, 0x1.7722bcp+2f},
    {/* 0x1.b00fedp+3 in decimal: */ "13.501944065093994140625", chars_format::general, 24, errc{}, 0x1.b00fecp+3f},
    {/* 0x1.1ebb99p+4 in decimal: */ "17.92080020904541015625", chars_format::general, 23, errc{}, 0x1.1ebb98p+4f},
    {/* 0x1.26c1abp+5 in decimal: */ "36.8445644378662109375", chars_format::general, 22, errc{}, 0x1.26c1acp+5f},
    {/* 0x1.495441p+6 in decimal: */ "82.332279205322265625", chars_format::general, 21, errc{}, 0x1.495440p+6f},
    {/* 0x1.c5ea61p+7 in decimal: */ "226.95777130126953125", chars_format::general, 21, errc{}, 0x1.c5ea60p+7f},
    {/* 0x1.b2f6a1p+8 in decimal: */ "434.9633941650390625", chars_format::general, 20, errc{}, 0x1.b2f6a0p+8f},
    {/* 0x1.a0db29p+9 in decimal: */ "833.712188720703125", chars_format::general, 19, errc{}, 0x1.a0db28p+9f},
    {/* 0x1.4a944fp+10 in decimal: */ "1322.31732177734375", chars_format::general, 19, errc{}, 0x1.4a9450p+10f},
    {/* 0x1.2741c5p+11 in decimal: */ "2362.0552978515625", chars_format::general, 18, errc{}, 0x1.2741c4p+11f},
    {/* 0x1.520bb5p+12 in decimal: */ "5408.731689453125", chars_format::general, 17, errc{}, 0x1.520bb4p+12f},
    {/* 0x1.c988cfp+13 in decimal: */ "14641.10107421875", chars_format::general, 17, errc{}, 0x1.c988d0p+13f},
    {/* 0x1.870e67p+14 in decimal: */ "25027.6005859375", chars_format::general, 16, errc{}, 0x1.870e68p+14f},
    {/* 0x1.a93953p+15 in decimal: */ "54428.662109375", chars_format::general, 15, errc{}, 0x1.a93954p+15f},
    {/* 0x1.1b3127p+16 in decimal: */ "72497.15234375", chars_format::general, 14, errc{}, 0x1.1b3128p+16f},
    {/* 0x1.cccddbp+17 in decimal: */ "235931.7109375", chars_format::general, 14, errc{}, 0x1.cccddcp+17f},
    {/* 0x1.2c3e23p+18 in decimal: */ "307448.546875", chars_format::general, 13, errc{}, 0x1.2c3e24p+18f},
    {/* 0x1.7043d5p+19 in decimal: */ "754206.65625", chars_format::general, 12, errc{}, 0x1.7043d4p+19f},
    {/* 0x1.ec3171p+20 in decimal: */ "2016023.0625", chars_format::general, 12, errc{}, 0x1.ec3170p+20f},
    {/* 0x1.c2c4c3p+21 in decimal: */ "3692696.375", chars_format::general, 11, errc{}, 0x1.c2c4c4p+21f},
    {/* 0x1.62efbfp+22 in decimal: */ "5815279.75", chars_format::general, 10, errc{}, 0x1.62efc0p+22f},
    {/* 0x1.12e7b9p+23 in decimal: */ "9008092.5", chars_format::general, 9, errc{}, 0x1.12e7b8p+23f},
    {/* 0x1.71e537p+24 in decimal: */ "24241463", chars_format::general, 8, errc{}, 0x1.71e538p+24f},
    {/* 0x1.6e14b3p+25 in decimal: */ "47982950", chars_format::general, 8, errc{}, 0x1.6e14b4p+25f},
    {/* 0x1.2ddcb1p+26 in decimal: */ "79131332", chars_format::general, 8, errc{}, 0x1.2ddcb0p+26f},
    {/* 0x1.f8e071p+27 in decimal: */ "264700808", chars_format::general, 9, errc{}, 0x1.f8e070p+27f},
    {/* 0x1.ed182dp+28 in decimal: */ "517046992", chars_format::general, 9, errc{}, 0x1.ed182cp+28f},
    {/* 0x1.f3f655p+29 in decimal: */ "1048496800", chars_format::general, 10, errc{}, 0x1.f3f654p+29f},
    {/* 0x1.968721p+30 in decimal: */ "1705101376", chars_format::general, 10, errc{}, 0x1.968720p+30f},
    {/* 0x1.753579p+31 in decimal: */ "3130702976", chars_format::general, 10, errc{}, 0x1.753578p+31f},
    {/* 0x1.074e6fp+32 in decimal: */ "4417548032", chars_format::general, 10, errc{}, 0x1.074e70p+32f},
    {/* 0x1.32e079p+33 in decimal: */ "10297078272", chars_format::general, 11, errc{}, 0x1.32e078p+33f},
    {/* 0x1.6b8addp+34 in decimal: */ "24396919808", chars_format::general, 11, errc{}, 0x1.6b8adcp+34f},
    {/* 0x1.9a1087p+35 in decimal: */ "55037933568", chars_format::general, 11, errc{}, 0x1.9a1088p+35f},
    {/* 0x1.196afbp+36 in decimal: */ "75542540288", chars_format::general, 11, errc{}, 0x1.196afcp+36f},
    {/* 0x1.628433p+37 in decimal: */ "190329544704", chars_format::general, 12, errc{}, 0x1.628434p+37f},
    {/* 0x1.0bd82dp+38 in decimal: */ "287595773952", chars_format::general, 12, errc{}, 0x1.0bd82cp+38f},
    {/* 0x1.2fc49fp+39 in decimal: */ "652336922624", chars_format::general, 12, errc{}, 0x1.2fc4a0p+39f},
    {/* 0x1.50b97bp+40 in decimal: */ "1446220857344", chars_format::general, 13, errc{}, 0x1.50b97cp+40f},
    {/* 0x1.6306a9p+41 in decimal: */ "3049650257920", chars_format::general, 13, errc{}, 0x1.6306a8p+41f},
    {/* 0x1.51dcbfp+42 in decimal: */ "5804429934592", chars_format::general, 13, errc{}, 0x1.51dcc0p+42f},
    {/* 0x1.5ac8f9p+43 in decimal: */ "11915443568640", chars_format::general, 14, errc{}, 0x1.5ac8f8p+43f},
    {/* 0x1.4f94cdp+44 in decimal: */ "23060968112128", chars_format::general, 14, errc{}, 0x1.4f94ccp+44f},
    {/* 0x1.3b3c25p+45 in decimal: */ "43325560193024", chars_format::general, 14, errc{}, 0x1.3b3c24p+45f},
    {/* 0x1.a0dccbp+46 in decimal: */ "114586283933696", chars_format::general, 15, errc{}, 0x1.a0dcccp+46f},
    {/* 0x1.2db2b3p+47 in decimal: */ "165860253630464", chars_format::general, 15, errc{}, 0x1.2db2b4p+47f},
    {/* 0x1.ed97bfp+48 in decimal: */ "542710977003520", chars_format::general, 15, errc{}, 0x1.ed97c0p+48f},
    {/* 0x1.b6fe61p+49 in decimal: */ "965357284098048", chars_format::general, 15, errc{}, 0x1.b6fe60p+49f},
    {/* 0x1.e39d93p+50 in decimal: */ "2126963569328128", chars_format::general, 16, errc{}, 0x1.e39d94p+50f},
    {/* 0x1.c83801p+51 in decimal: */ "4012942697693184", chars_format::general, 16, errc{}, 0x1.c83800p+51f},
    {/* 0x1.6f6693p+52 in decimal: */ "6463381124939776", chars_format::general, 16, errc{}, 0x1.6f6694p+52f},
    {/* 0x1.e75311p+53 in decimal: */ "17146205767204864", chars_format::general, 17, errc{}, 0x1.e75310p+53f},
    {/* 0x1.3248b9p+54 in decimal: */ "21552825569902592", chars_format::general, 17, errc{}, 0x1.3248b8p+54f},
    {/* 0x1.66c585p+55 in decimal: */ "50492608341868544", chars_format::general, 17, errc{}, 0x1.66c584p+55f},
    {/* 0x1.004ae3p+56 in decimal: */ "72139932855959552", chars_format::general, 17, errc{}, 0x1.004ae4p+56f},
    {/* 0x1.0954b7p+57 in decimal: */ "149368027568144384", chars_format::general, 18, errc{}, 0x1.0954b8p+57f},
    {/* 0x1.5fd4dfp+58 in decimal: */ "396127084272943104", chars_format::general, 18, errc{}, 0x1.5fd4e0p+58f},
    {/* 0x1.c84d97p+59 in decimal: */ "1027503202523676672", chars_format::general, 19, errc{}, 0x1.c84d98p+59f},
    {/* 0x1.a387c3p+60 in decimal: */ "1889396589282197504", chars_format::general, 19, errc{}, 0x1.a387c4p+60f},
    {/* 0x1.e3d705p+61 in decimal: */ "4358042567233765376", chars_format::general, 19, errc{}, 0x1.e3d704p+61f},
    {/* 0x1.71cd7bp+62 in decimal: */ "6661772452537827328", chars_format::general, 19, errc{}, 0x1.71cd7cp+62f},
    {/* 0x1.b2a203p+63 in decimal: */ "15659299028611366912", chars_format::general, 20, errc{}, 0x1.b2a204p+63f},
    {/* 0x1.e1d3f5p+64 in decimal: */ "34719363332678090752", chars_format::general, 20, errc{}, 0x1.e1d3f4p+64f},
    {/* 0x1.84a0fdp+65 in decimal: */ "56007321318863142912", chars_format::general, 20, errc{}, 0x1.84a0fcp+65f},
    {/* 0x1.7c30adp+66 in decimal: */ "109582346995225329664", chars_format::general, 21, errc{}, 0x1.7c30acp+66f},
    {/* 0x1.64a58dp+67 in decimal: */ "205592815038392958976", chars_format::general, 21, errc{}, 0x1.64a58cp+67f},
    {/* 0x1.9ce943p+68 in decimal: */ "476054177287663255552", chars_format::general, 21, errc{}, 0x1.9ce944p+68f},
    {/* 0x1.c81229p+69 in decimal: */ "1051627984347285422080", chars_format::general, 22, errc{}, 0x1.c81228p+69f},
    {/* 0x1.ab6e69p+70 in decimal: */ "1971178902422676307968", chars_format::general, 22, errc{}, 0x1.ab6e68p+70f},
    {/* 0x1.f472edp+71 in decimal: */ "4615826656072290009088", chars_format::general, 22, errc{}, 0x1.f472ecp+71f},
    {/* 0x1.329c43p+72 in decimal: */ "5655963530048479166464", chars_format::general, 22, errc{}, 0x1.329c44p+72f},
    {/* 0x1.449e0bp+73 in decimal: */ "11976266551929262309376", chars_format::general, 23, errc{}, 0x1.449e0cp+73f},
    {/* 0x1.f0a57bp+74 in decimal: */ "36646036739993324486656", chars_format::general, 23, errc{}, 0x1.f0a57cp+74f},
    {/* 0x1.f1d3ebp+75 in decimal: */ "73466416828761415614464", chars_format::general, 23, errc{}, 0x1.f1d3ecp+75f},
    {/* 0x1.8c7125p+76 in decimal: */ "117009017214230505455616", chars_format::general, 24, errc{}, 0x1.8c7124p+76f},
    {/* 0x1.ba8913p+77 in decimal: */ "261226819807596014206976", chars_format::general, 24, errc{}, 0x1.ba8914p+77f},
    {/* 0x1.8c9acfp+78 in decimal: */ "468228210431424156663808", chars_format::general, 24, errc{}, 0x1.8c9ad0p+78f},
    {/* 0x1.3addbbp+79 in decimal: */ "743456640415721750265856", chars_format::general, 24, errc{}, 0x1.3addbcp+79f},
    {/* 0x1.2388e1p+80 in decimal: */ "1376733616667749789990912", chars_format::general, 25, errc{}, 0x1.2388e0p+80f},
    {/* 0x1.763dadp+81 in decimal: */ "3534605563891024308207616", chars_format::general, 25, errc{}, 0x1.763dacp+81f},
    {/* 0x1.06c3e1p+82 in decimal: */ "4963493386259515769356288", chars_format::general, 25, errc{}, 0x1.06c3e0p+82f},
    {/* 0x1.c6f137p+83 in decimal: */ "17187232093698040119951360", chars_format::general, 26, errc{}, 0x1.c6f138p+83f},
    {/* 0x1.e552a5p+84 in decimal: */ "36669956267341413919752192", chars_format::general, 26, errc{}, 0x1.e552a4p+84f},
    {/* 0x1.16ddebp+85 in decimal: */ "42141169478804802988146688", chars_format::general, 26, errc{}, 0x1.16ddecp+85f},
    {/* 0x1.c626f5p+86 in decimal: */ "137259072870922187668717568", chars_format::general, 27, errc{},
        0x1.c626f4p+86f},
    {/* 0x1.3404a1p+87 in decimal: */ "186185505916516565814083584", chars_format::general, 27, errc{},
        0x1.3404a0p+87f},
    {/* 0x1.653319p+88 in decimal: */ "431827819461650810014793728", chars_format::general, 27, errc{},
        0x1.653318p+88f},
    {/* 0x1.f4ee0dp+89 in decimal: */ "1211174145675821042276237312", chars_format::general, 28, errc{},
        0x1.f4ee0cp+89f},
    {/* 0x1.164573p+90 in decimal: */ "1345637370063013570745991168", chars_format::general, 28, errc{},
        0x1.164574p+90f},
    {/* 0x1.ec0f05p+91 in decimal: */ "4758899447850887737451216896", chars_format::general, 28, errc{},
        0x1.ec0f04p+91f},
    {/* 0x1.1fcd01p+92 in decimal: */ "5566877020882094785904508928", chars_format::general, 28, errc{},
        0x1.1fcd00p+92f},
    {/* 0x1.c96155p+93 in decimal: */ "17694039586751044919626825728", chars_format::general, 29, errc{},
        0x1.c96154p+93f},
    {/* 0x1.f2cfc5p+94 in decimal: */ "38593678210471799446053060608", chars_format::general, 29, errc{},
        0x1.f2cfc4p+94f},
    {/* 0x1.23d719p+95 in decimal: */ "45160087484195316006301728768", chars_format::general, 29, errc{},
        0x1.23d718p+95f},
    {/* 0x1.a12f23p+96 in decimal: */ "129112233891849681667027369984", chars_format::general, 30, errc{},
        0x1.a12f24p+96f},
    {/* 0x1.5e0715p+97 in decimal: */ "216656630175808433440891600896", chars_format::general, 30, errc{},
        0x1.5e0714p+97f},
    {/* 0x1.54e25fp+98 in decimal: */ "421994276797224408704817823744", chars_format::general, 30, errc{},
        0x1.54e260p+98f},
    {/* 0x1.61449dp+99 in decimal: */ "874649254673651316624210264064", chars_format::general, 30, errc{},
        0x1.61449cp+99f},
    {/* 0x1.b5720dp+100 in decimal: */ "2166125251618050241024537657344", chars_format::general, 31, errc{},
        0x1.b5720cp+100f},
    {/* 0x1.8d6ba3p+101 in decimal: */ "3935861558640302891443247120384", chars_format::general, 31, errc{},
        0x1.8d6ba4p+101f},
    {/* 0x1.731b17p+102 in decimal: */ "7350508048337774175132848226304", chars_format::general, 31, errc{},
        0x1.731b18p+102f},
    {/* 0x1.c07785p+103 in decimal: */ "17765603154846586025383087308800", chars_format::general, 32, errc{},
        0x1.c07784p+103f},
    {/* 0x1.f4cb37p+104 in decimal: */ "39676973205045980650327714562048", chars_format::general, 32, errc{},
        0x1.f4cb38p+104f},
    {/* 0x1.e4820fp+105 in decimal: */ "76773363684136016947294228185088", chars_format::general, 32, errc{},
        0x1.e48210p+105f},
    {/* 0x1.fc14cbp+106 in decimal: */ "161017366677536368674469150982144", chars_format::general, 33, errc{},
        0x1.fc14ccp+106f},
    {/* 0x1.a87125p+107 in decimal: */ "269022059539305734989663994642432", chars_format::general, 33, errc{},
        0x1.a87124p+107f},
    {/* 0x1.38ff8fp+108 in decimal: */ "396772452133553939418920234516480", chars_format::general, 33, errc{},
        0x1.38ff90p+108f},
    {/* 0x1.13c1b5p+109 in decimal: */ "699126211644530005899810908930048", chars_format::general, 33, errc{},
        0x1.13c1b4p+109f},
    {/* 0x1.55eee7p+110 in decimal: */ "1733807367140220814406037893283840", chars_format::general, 34, errc{},
        0x1.55eee8p+110f},
    {/* 0x1.68ea01p+111 in decimal: */ "3660103578413974514481484236062720", chars_format::general, 34, errc{},
        0x1.68ea00p+111f},
    {/* 0x1.a20bfdp+112 in decimal: */ "8478997023821539945225867390615552", chars_format::general, 34, errc{},
        0x1.a20bfcp+112f},
    {/* 0x1.58562fp+113 in decimal: */ "13967954142855725924178258573656064", chars_format::general, 35, errc{},
        0x1.585630p+113f},
    {/* 0x1.8e458dp+114 in decimal: */ "32311637611412935510718602954145792", chars_format::general, 35, errc{},
        0x1.8e458cp+114f},
    {/* 0x1.635bb5p+115 in decimal: */ "57660169510975349749437807131099136", chars_format::general, 35, errc{},
        0x1.635bb4p+115f},
    {/* 0x1.019409p+116 in decimal: */ "83588925144890861008382505730965504", chars_format::general, 35, errc{},
        0x1.019408p+116f},
    {/* 0x1.b0eb7bp+117 in decimal: */ "280981044275986616573540753022648320", chars_format::general, 36, errc{},
        0x1.b0eb7cp+117f},
    {/* 0x1.a072a1p+118 in decimal: */ "540580110894867345113842255044542464", chars_format::general, 36, errc{},
        0x1.a072a0p+118f},
    {/* 0x1.211b6fp+119 in decimal: */ "750565105750951431544116449153908736", chars_format::general, 36, errc{},
        0x1.211b70p+119f},
    {/* 0x1.e54565p+120 in decimal: */ "2519671464696457305794640027949465600", chars_format::general, 37, errc{},
        0x1.e54564p+120f},
    {/* 0x1.93c0a7p+121 in decimal: */ "4192806175473153074327132029524639744", chars_format::general, 37, errc{},
        0x1.93c0a8p+121f},
    {/* 0x1.f33db1p+122 in decimal: */ "10368829531117867053281330031553937408", chars_format::general, 38, errc{},
        0x1.f33db0p+122f},
    {/* 0x1.f97c0fp+123 in decimal: */ "20997008966187027788044272068598956032", chars_format::general, 38, errc{},
        0x1.f97c10p+123f},
    {/* 0x1.46fa2fp+124 in decimal: */ "27164209632110478318892728203406737408", chars_format::general, 38, errc{},
        0x1.46fa30p+124f},
    {/* 0x1.73d8d9p+125 in decimal: */ "61783690480066413003444625390581055488", chars_format::general, 38, errc{},
        0x1.73d8d8p+125f},
    {/* 0x1.a19fe7p+126 in decimal: */ "138779583669858850032422956217074188288", chars_format::general, 39, errc{},
        0x1.a19fe8p+126f},
    {/* 0x1.f0b0b5p+127 in decimal: */ "330107300636279331787628201852738207744", chars_format::general, 39, errc{},
        0x1.f0b0b4p+127f},
};

#endif // FLOAT_FROM_CHARS_TEST_CASES_HPP
