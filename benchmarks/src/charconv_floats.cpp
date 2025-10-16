// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <benchmark/benchmark.h>
#include <bit>
#include <charconv>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <random>
#include <system_error>
#include <type_traits>
#include <vector>

using namespace std;

void verify(const bool b) {
    if (!b) {
        puts("FAIL");
        exit(EXIT_FAILURE);
    }
}

enum class RoundTrip { Sci, Fix, Gen, Hex, Lossy };

consteval chars_format chars_format_from_RoundTrip(const RoundTrip rt) {
    switch (rt) {
    case RoundTrip::Sci:
        return chars_format::scientific;
    case RoundTrip::Fix:
        return chars_format::fixed;
    case RoundTrip::Gen:
        return chars_format::general;
    case RoundTrip::Hex:
        return chars_format::hex;
    case RoundTrip::Lossy:
    default:
        exit(EXIT_FAILURE);
    }
}

template <RoundTrip Rt, typename Floating, auto... Args>
void test_to_chars(benchmark::State& state) {
    constexpr size_t n = 2'000'000; // how many floating-point values to test

    constexpr size_t BufSize = 2'000; // more than enough

    mt19937_64 mt64;

    vector<Floating> vec;

    vec.reserve(n);
    while (vec.size() < n) {
        using Integral             = conditional_t<sizeof(Floating) == 4, uint32_t, uint64_t>;
        const Integral val         = static_cast<Integral>(mt64());
        constexpr Integral inf_nan = sizeof(Floating) == 4 ? 0x7F800000U : 0x7FF0000000000000ULL;
        if ((val & inf_nan) == inf_nan) {
            continue; // skip INF/NAN
        }
        vec.push_back(bit_cast<Floating>(val));
    }

    char buf[BufSize];

    auto it = vec.begin();
    for (auto _ : state) {
        auto result = to_chars(buf, buf + BufSize, *it, Args...);

        benchmark::DoNotOptimize(result.ptr);
        benchmark::DoNotOptimize(buf);

        ++it;
        if (it == vec.end()) {
            it = vec.begin();
        }
    }

    for (const auto& elem : vec) {
        const auto result = to_chars(buf, buf + BufSize, elem, Args...);
        verify(result.ec == errc{});

        if constexpr (Rt == RoundTrip::Lossy) {
            // skip lossy conversions
        } else {
            Floating round_trip;
            const auto from_result = from_chars(buf, result.ptr, round_trip, chars_format_from_RoundTrip(Rt));
            verify(from_result.ec == errc{});
            verify(from_result.ptr == result.ptr);
            verify(round_trip == elem);
        }
    }
}

BENCHMARK(test_to_chars<RoundTrip::Gen, float>)->Name("STL_float_plain_shortest");
BENCHMARK(test_to_chars<RoundTrip::Gen, double>)->Name("STL_double_plain_shortest");
BENCHMARK(test_to_chars<RoundTrip::Sci, float, chars_format::scientific>)->Name("STL_float_scientific_shortest");
BENCHMARK(test_to_chars<RoundTrip::Sci, double, chars_format::scientific>)->Name("STL_double_scientific_shortest");
BENCHMARK(test_to_chars<RoundTrip::Fix, float, chars_format::fixed>)->Name("STL_float_fixed_shortest");
BENCHMARK(test_to_chars<RoundTrip::Fix, double, chars_format::fixed>)->Name("STL_double_fixed_shortest");
BENCHMARK(test_to_chars<RoundTrip::Gen, float, chars_format::general>)->Name("STL_float_general_shortest");
BENCHMARK(test_to_chars<RoundTrip::Gen, double, chars_format::general>)->Name("STL_double_general_shortest");
BENCHMARK(test_to_chars<RoundTrip::Hex, float, chars_format::hex>)->Name("STL_float_hex_shortest");
BENCHMARK(test_to_chars<RoundTrip::Hex, double, chars_format::hex>)->Name("STL_double_hex_shortest");
BENCHMARK(test_to_chars<RoundTrip::Sci, float, chars_format::scientific, 8>)->Name("STL_float_scientific_8");
BENCHMARK(test_to_chars<RoundTrip::Sci, double, chars_format::scientific, 16>)->Name("STL_double_scientific_16");
BENCHMARK(test_to_chars<RoundTrip::Lossy, float, chars_format::fixed, 6>)->Name("STL_float_fixed_6_lossy");
BENCHMARK(test_to_chars<RoundTrip::Lossy, double, chars_format::fixed, 6>)->Name("STL_double_fixed_6_lossy");
BENCHMARK(test_to_chars<RoundTrip::Gen, float, chars_format::general, 9>)->Name("STL_float_general_9");
BENCHMARK(test_to_chars<RoundTrip::Gen, double, chars_format::general, 17>)->Name("STL_double_general_17");
BENCHMARK(test_to_chars<RoundTrip::Hex, float, chars_format::hex, 6>)->Name("STL_float_hex_6");
BENCHMARK(test_to_chars<RoundTrip::Hex, double, chars_format::hex, 13>)->Name("STL_double_hex_13");

BENCHMARK_MAIN();
