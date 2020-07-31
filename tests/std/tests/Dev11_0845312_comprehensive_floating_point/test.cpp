// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Derived from: qa/VC/LibsWin/devcrt/tests/C/Dev14_845312_accurate_fp_parsing

// Basic regression test for DevDiv-845312: Floating point conversion accuracy
// improvements. This test verifies scanf-printf round-tripping of a set of
// diverse floating point values (both single and double precision).

#include <assert.h>
#include <float.h>
#include <iomanip>
#include <ios>
#include <math.h>
#include <sstream>
#include <stdint.h>
#include <stdio.h>

#include <floating_point_test_cases.hpp>

template <typename FloatingType>
static FloatingType parse_as(char const*);

template <>
float parse_as(char const* const string) {
    float f = 0.0f;
    std::istringstream iss(string);
    iss >> f;
    return f;
}

template <>
double parse_as(char const* const string) {
    double d = 0.0;
    std::istringstream iss(string);
    iss >> d;
    return d;
}

template <typename FloatingType>
static FloatingType parse_as_hex(char const*);

template <>
float parse_as_hex(char const* const string) {
    float f = 0.0f;
    std::istringstream iss(string);
    iss >> std::hexfloat >> f;
    return f;
}

template <>
double parse_as_hex(char const* const string) {
    double d = 0.0;
    std::istringstream iss(string);
    iss >> std::hexfloat >> d;
    return d;
}

static uint32_t as_bits(float const value) {
    return reinterpret_cast<uint32_t const&>(value);
}

static uint64_t as_bits(double const value) {
    return reinterpret_cast<uint64_t const&>(value);
}

static float from_bits(uint32_t const bits) {
    return reinterpret_cast<float const&>(bits);
}

static double from_bits(uint64_t const bits) {
    return reinterpret_cast<double const&>(bits);
}

int g_failed = 0;

static void assert_equal(uint64_t const actual, uint64_t const expected) {
    if (actual != expected) {
        ++g_failed;
    }
}

template <typename FloatingType>
static void parse_and_verify_exact_bits(char const* const string, uint64_t const expected_bits) {
    FloatingType const parsed_value = parse_as<FloatingType>(string);
    uint64_t const actual_bits      = as_bits(parsed_value);

    assert_equal(actual_bits, expected_bits);
}

template <typename FloatingType>
static void parse_and_verify_exact_bits_hex(char const* const string, uint64_t const expected_bits) {
    FloatingType const parsed_value = parse_as_hex<FloatingType>(string);
    uint64_t const actual_bits      = as_bits(parsed_value);

    assert_equal(actual_bits, expected_bits);
}

template <typename FloatingType>
static void verify_round_trip_decimal(FloatingType const original_value) {
    std::ostringstream oss;
    oss << std::setprecision(1024) << std::fixed << original_value;

    FloatingType const round_tripped_value = parse_as<FloatingType>(oss.str().c_str());

    uint64_t const original_value_bits      = as_bits(original_value);
    uint64_t const round_tripped_value_bits = as_bits(round_tripped_value);

    assert_equal(round_tripped_value_bits, original_value_bits);
}

template <typename FloatingType>
static void verify_round_trip_hex(FloatingType const original_value) {
    std::ostringstream oss;
    oss << std::setprecision(32) << std::hexfloat << original_value;

    FloatingType const round_tripped_value = parse_as_hex<FloatingType>(oss.str().c_str());

    uint64_t const original_value_bits      = as_bits(original_value);
    uint64_t const round_tripped_value_bits = as_bits(round_tripped_value);

    assert_equal(round_tripped_value_bits, original_value_bits);
}

template <typename FloatingType>
static void verify_round_trip(FloatingType const original_value) {
    // Round-trip the value using both decimal and hexadecimal formatting:
    verify_round_trip_decimal(original_value);
    verify_round_trip_hex(original_value);
}

int main() {
    // Verify zeroes:
    parse_and_verify_exact_bits<float>(" 0.0", 0x00000000);
    parse_and_verify_exact_bits<float>("+0.0", 0x00000000);
    parse_and_verify_exact_bits<float>("-0.0", 0x80000000);

    parse_and_verify_exact_bits<double>(" 0.0", 0x0000000000000000);
    parse_and_verify_exact_bits<double>("+0.0", 0x0000000000000000);
    parse_and_verify_exact_bits<double>("-0.0", 0x8000000000000000);

    // Verify the smallest denormals:
    for (uint32_t i = 0x00000001; i != 0x00000100; ++i) {
        verify_round_trip(from_bits(i));
    }

    for (uint64_t i = 0x0000000000000001; i != 0x0000000000000100; ++i) {
        verify_round_trip(from_bits(i));
    }

    // Verify the largest denormals and the smallest normals:
    for (uint32_t i = 0x007fff00; i != 0x00800100; ++i) {
        verify_round_trip(from_bits(i));
    }

    for (uint64_t i = 0x000fffffffffff00; i != 0x0010000000000100; ++i) {
        verify_round_trip(from_bits(i));
    }

    // Verify the largest normals:
    for (uint32_t i = 0x7f7fff00; i != 0x7f800000; ++i) {
        verify_round_trip(from_bits(i));
    }

    for (uint64_t i = 0x7fefffffffffff00; i != 0x7ff0000000000000; ++i) {
        verify_round_trip(from_bits(i));
    }

    // Verify all representable powers of two and nearby values:
    for (int32_t i = FLT_MIN_EXP; i != FLT_MAX_EXP; ++i) {
        auto const f = powf(2.0f, static_cast<float>(i));
        verify_round_trip(from_bits(as_bits(f) - 1));
        verify_round_trip(f);
        verify_round_trip(from_bits(as_bits(f) + 1));
    }

    for (int32_t i = DBL_MIN_EXP; i != DBL_MAX_EXP; ++i) {
        auto const f = pow(2.0, static_cast<double>(i));
        verify_round_trip(from_bits(as_bits(f) - 1));
        verify_round_trip(f);
        verify_round_trip(from_bits(as_bits(f) + 1));
    }

    // Verify all representable powers of ten and nearby values:
    for (int32_t i = FLT_MIN_10_EXP; i <= FLT_MAX_10_EXP; ++i) {
        auto const f = powf(10.0f, static_cast<float>(i));
        verify_round_trip(from_bits(as_bits(f) - 1));
        verify_round_trip(f);
        verify_round_trip(from_bits(as_bits(f) + 1));
    }

    for (int32_t i = DBL_MIN_10_EXP; i <= DBL_MAX_10_EXP; ++i) {
        auto const f = pow(10.0, static_cast<double>(i));
        verify_round_trip(from_bits(as_bits(f) - 1));
        verify_round_trip(f);
        verify_round_trip(from_bits(as_bits(f) + 1));
    }

    // Verify a few large integer values:
    verify_round_trip(static_cast<float>(INT64_MAX));
    verify_round_trip(static_cast<float>(UINT64_MAX));

    verify_round_trip(static_cast<double>(INT64_MAX));
    verify_round_trip(static_cast<double>(UINT64_MAX));

    // https://www.exploringbinary.com/nondeterministic-floating-point-conversions-in-java/
    parse_and_verify_exact_bits_hex<double>("0x0.0000008p-1022", 0x0000000008000000ULL);

    // See floating_point_test_cases.hpp.
    for (const auto& p : floating_point_test_cases_double) {
        parse_and_verify_exact_bits<double>(p.first, p.second);
    }

    for (const auto& p : floating_point_test_cases_float) {
        parse_and_verify_exact_bits<float>(p.first, p.second);
    }

    printf("%d failed.\n", g_failed);

    assert(g_failed == 0);
}
