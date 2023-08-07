// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <benchmark/benchmark.h>
#include <cstddef>
#include <cstdint>
#include <random>

#include <algorithm>
#include <ranges>

enum class Op {
    Min,
    Max,
    Both,
};

using namespace std;

template <class T, size_t Size, Op Operation>
void bm(benchmark::State& state) {
    mt19937 gen(84710);
    uniform_int_distribution<conditional_t<sizeof(T) != 1, T, int>> dis(1, 20);

    T a[Size];
    ranges::generate(a, [&] { return dis(gen); });

    for (auto _ : state) {
        if constexpr (Operation == Op::Min) {
            benchmark::DoNotOptimize(ranges::min_element(a));
        } else if constexpr (Operation == Op::Max) {
            benchmark::DoNotOptimize(ranges::max_element(a));
        } else if constexpr (Operation == Op::Both) {
            benchmark::DoNotOptimize(ranges::minmax_element(a));
        }
    }
}

template <class T, size_t Size, Op Operation>
void bmf(benchmark::State& state) {
    mt19937 gen(84710);
    normal_distribution<T> dis(0, 10000.0);

    T a[Size];
    generate(begin(a), end(a), [&] { return dis(gen); });

    for (auto _ : state) {
        if constexpr (Operation == Op::Min) {
            benchmark::DoNotOptimize(ranges::min_element(a));
        } else if constexpr (Operation == Op::Max) {
            benchmark::DoNotOptimize(ranges::max_element(a));
        } else if constexpr (Operation == Op::Both) {
            benchmark::DoNotOptimize(ranges::minmax_element(a));
        }
    }
}

BENCHMARK(bm<uint8_t, 8021, Op::Min>);
BENCHMARK(bm<uint8_t, 8021, Op::Max>);
BENCHMARK(bm<uint8_t, 8021, Op::Both>);

BENCHMARK(bm<uint16_t, 8021, Op::Min>);
BENCHMARK(bm<uint16_t, 8021, Op::Max>);
BENCHMARK(bm<uint16_t, 8021, Op::Both>);

BENCHMARK(bm<uint32_t, 8021, Op::Min>);
BENCHMARK(bm<uint32_t, 8021, Op::Max>);
BENCHMARK(bm<uint32_t, 8021, Op::Both>);

BENCHMARK(bm<uint64_t, 8021, Op::Min>);
BENCHMARK(bm<uint64_t, 8021, Op::Max>);
BENCHMARK(bm<uint64_t, 8021, Op::Both>);

BENCHMARK(bm<int8_t, 8021, Op::Min>);
BENCHMARK(bm<int8_t, 8021, Op::Max>);
BENCHMARK(bm<int8_t, 8021, Op::Both>);

BENCHMARK(bm<int16_t, 8021, Op::Min>);
BENCHMARK(bm<int16_t, 8021, Op::Max>);
BENCHMARK(bm<int16_t, 8021, Op::Both>);

BENCHMARK(bm<int32_t, 8021, Op::Min>);
BENCHMARK(bm<int32_t, 8021, Op::Max>);
BENCHMARK(bm<int32_t, 8021, Op::Both>);

BENCHMARK(bm<int64_t, 8021, Op::Min>);
BENCHMARK(bm<int64_t, 8021, Op::Max>);
BENCHMARK(bm<int64_t, 8021, Op::Both>);

BENCHMARK(bmf<float, 8021, Op::Min>);
BENCHMARK(bmf<float, 8021, Op::Max>);
BENCHMARK(bmf<float, 8021, Op::Both>);

BENCHMARK(bmf<double, 8021, Op::Min>);
BENCHMARK(bmf<double, 8021, Op::Max>);
BENCHMARK(bmf<double, 8021, Op::Both>);


BENCHMARK_MAIN();
