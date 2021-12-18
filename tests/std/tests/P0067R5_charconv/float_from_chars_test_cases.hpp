// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef FLOAT_FROM_CHARS_TEST_CASES_HPP
#define FLOAT_FROM_CHARS_TEST_CASES_HPP

#include <charconv>
#include <stddef.h>
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
};

#endif // FLOAT_FROM_CHARS_TEST_CASES_HPP
