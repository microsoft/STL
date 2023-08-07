// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstddef>
#include <cstdint>
#include <random>
#include <ranges>
#include <type_traits>

enum class Op {
    Min,
    Max,
    Both,
};

using namespace std;

template <class T, size_t Size, Op Operation>
void bm(benchmark::State& state) {
    T a[Size];

    mt19937 gen(84710);

    if constexpr (is_floating_point_v<T>) {
        normal_distribution<T> dis(0, 10000.0);
        ranges::generate(a, [&] { return dis(gen); });
    } else {
        uniform_int_distribution<conditional_t<sizeof(T) != 1, T, int>> dis(1, 20);
        ranges::generate(a, [&] { return dis(gen); });
    }

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

BENCHMARK(bm<float, 8021, Op::Min>);
BENCHMARK(bm<float, 8021, Op::Max>);
BENCHMARK(bm<float, 8021, Op::Both>);

BENCHMARK(bm<double, 8021, Op::Min>);
BENCHMARK(bm<double, 8021, Op::Max>);
BENCHMARK(bm<double, 8021, Op::Both>);


BENCHMARK_MAIN();
