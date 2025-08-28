// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <benchmark/benchmark.h>
#include <charconv>
#include <chrono>
#include <random>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <system_error>
#include <type_traits>
#include <vector>

using namespace std;
using namespace std::chrono;

void verify(const bool b) {
    if (!b) {
        puts("FAIL");
        exit(EXIT_FAILURE);
    }
}

enum class RoundTrip { Sci, Fix, Gen, Hex, Lossy };

constexpr chars_format chars_format_from_RoundTrip(const RoundTrip rt) {
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
        puts("FAIL");
        exit(EXIT_FAILURE);
    }
}

template <RoundTrip Rt, typename Floating, typename... Args>
void test_to_chars(benchmark::State& state, const Args&... args) {
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
        Floating flt;
        static_assert(sizeof(flt) == sizeof(val));
        memcpy(&flt, &val, sizeof(flt));
        vec.push_back(flt);
    }

    char buf[BufSize];

    auto it = vec.begin();
    for (auto _ : state) {
        auto result = to_chars(buf, buf + BufSize, *it, args...);

        benchmark::DoNotOptimize(result.ptr);
        benchmark::DoNotOptimize(buf);

        ++it;
        if (it == vec.end()) {
            it = vec.begin();
        }
    }

    for (const auto& elem : vec) {
        const auto result = to_chars(buf, buf + BufSize, elem, args...);
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

constexpr auto STL_float_plain_shortest  = test_to_chars<RoundTrip::Gen, float>;
constexpr auto STL_double_plain_shortest = test_to_chars<RoundTrip::Gen, double>;

void STL_float_scientific_shortest(benchmark::State& state) {
    test_to_chars<RoundTrip::Sci, float>(state, chars_format::scientific);
}

void STL_double_scientific_shortest(benchmark::State& state) {
    test_to_chars<RoundTrip::Sci, double>(state, chars_format::scientific);
}

void STL_float_fixed_shortest(benchmark::State& state) {
    test_to_chars<RoundTrip::Fix, float>(state, chars_format::fixed);
}

void STL_double_fixed_shortest(benchmark::State& state) {
    test_to_chars<RoundTrip::Fix, double>(state, chars_format::fixed);
}

void STL_float_general_shortest(benchmark::State& state) {
    test_to_chars<RoundTrip::Gen, float>(state, chars_format::general);
}

void STL_double_general_shortest(benchmark::State& state) {
    test_to_chars<RoundTrip::Gen, double>(state, chars_format::general);
}

void STL_float_hex_shortest(benchmark::State& state) {
    test_to_chars<RoundTrip::Hex, float>(state, chars_format::hex);
}

void STL_double_hex_shortest(benchmark::State& state) {
    test_to_chars<RoundTrip::Hex, double>(state, chars_format::hex);
}

void STL_float_scientific_8(benchmark::State& state) {
    test_to_chars<RoundTrip::Sci, float>(state, chars_format::scientific, 8);
}

void STL_double_scientific_16(benchmark::State& state) {
    test_to_chars<RoundTrip::Sci, double>(state, chars_format::scientific, 16);
}

void STL_float_fixed_6_lossy(benchmark::State& state) {
    test_to_chars<RoundTrip::Lossy, float>(state, chars_format::fixed, 6);
}

void STL_double_fixed_6_lossy(benchmark::State& state) {
    test_to_chars<RoundTrip::Lossy, double>(state, chars_format::fixed, 6);
}

void STL_float_general_9(benchmark::State& state) {
    test_to_chars<RoundTrip::Gen, float>(state, chars_format::general, 9);
}

void STL_double_general_17(benchmark::State& state) {
    test_to_chars<RoundTrip::Gen, double>(state, chars_format::general, 17);
}

void STL_float_hex_6(benchmark::State& state) {
    test_to_chars<RoundTrip::Hex, float>(state, chars_format::hex, 6);
}

void STL_double_hex_13(benchmark::State& state) {
    test_to_chars<RoundTrip::Hex, double>(state, chars_format::hex, 13);
}

BENCHMARK(STL_float_plain_shortest);
BENCHMARK(STL_double_plain_shortest);

BENCHMARK(STL_float_scientific_shortest);
BENCHMARK(STL_double_scientific_shortest);

BENCHMARK(STL_float_fixed_shortest);
BENCHMARK(STL_double_fixed_shortest);

BENCHMARK(STL_float_general_shortest);
BENCHMARK(STL_double_general_shortest);

BENCHMARK(STL_float_hex_shortest);
BENCHMARK(STL_double_hex_shortest);

BENCHMARK(STL_float_scientific_8);
BENCHMARK(STL_double_scientific_16);

BENCHMARK(STL_float_fixed_6_lossy);
BENCHMARK(STL_double_fixed_6_lossy);

BENCHMARK(STL_float_general_9);
BENCHMARK(STL_double_general_17);

BENCHMARK(STL_float_hex_6);
BENCHMARK(STL_double_hex_13);

BENCHMARK_MAIN();
