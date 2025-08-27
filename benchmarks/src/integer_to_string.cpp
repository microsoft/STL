// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <benchmark/benchmark.h>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <random>
#include <type_traits>

using namespace std;

template <class T, double M, double S>
auto generate_array() {
    array<T, 2000> a;

    mt19937 gen;
    lognormal_distribution<double> dis(M, S);
    ranges::generate(a, [&] { return static_cast<T>(dis(gen)); });

    if constexpr (is_signed_v<T>) {
        bernoulli_distribution b(0.5);
        ranges::for_each(a, [&](T& v) { v *= (b(gen) ? -1 : 1); });
    }

    return a;
}

template <class T, double M, double S>
void internal_integer_to_buff(benchmark::State& state) {
    auto a = generate_array<T, M, S>();

    char buff[24];

    auto it = a.begin();
    for (auto _ : state) {
        auto i = *it;
        benchmark::DoNotOptimize(i);
        auto s = std::_UIntegral_to_buff(buff, i);
        benchmark::DoNotOptimize(s);

        ++it;
        if (it == a.end()) {
            it = a.begin();
        }
    }
}

template <class T, double M, double S>
void integer_to_string(benchmark::State& state) {
    auto a = generate_array<T, M, S>();

    auto it = a.begin();
    for (auto _ : state) {
        auto i = *it;
        benchmark::DoNotOptimize(i);
        auto s = to_string(i);
        benchmark::DoNotOptimize(s);

        ++it;
        if (it == a.end()) {
            it = a.begin();
        }
    }
}

BENCHMARK(internal_integer_to_buff<uint8_t, 2.5, 1.5>);
BENCHMARK(internal_integer_to_buff<uint16_t, 5.0, 3.0>);
BENCHMARK(internal_integer_to_buff<uint32_t, 10.0, 6.0>);
BENCHMARK(internal_integer_to_buff<uint64_t, 20.0, 12.0>);

BENCHMARK(integer_to_string<uint8_t, 2.5, 1.5>);
BENCHMARK(integer_to_string<uint16_t, 5.0, 3.0>);
BENCHMARK(integer_to_string<uint32_t, 10.0, 6.0>);
BENCHMARK(integer_to_string<uint64_t, 20.0, 12.0>);

BENCHMARK(integer_to_string<int8_t, 2.5, 1.5>);
BENCHMARK(integer_to_string<int16_t, 5.0, 3.0>);
BENCHMARK(integer_to_string<int32_t, 10.0, 6.0>);
BENCHMARK(integer_to_string<int64_t, 20.0, 12.0>);

BENCHMARK_MAIN();
