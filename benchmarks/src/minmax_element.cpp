// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstddef>
#include <cstdint>
#include <random>
#include <ranges>
#include <type_traits>
#include <vector>

#include "skewed_allocator.hpp"

enum class Op {
    Min,
    Max,
    Both,
    Min_val,
    Max_val,
    Both_val,
};

using namespace std;

template <class T, Op Operation>
void bm(benchmark::State& state) {
    vector<T, not_highly_aligned_allocator<T>> a(static_cast<size_t>(state.range()));

    mt19937 gen(84710);

    if constexpr (is_floating_point_v<T>) {
        normal_distribution<T> dis(0, 100000.0);
        ranges::generate(a, [&] { return dis(gen); });
    } else {
        uniform_int_distribution<conditional_t<sizeof(T) != 1, T, int>> dis(1, 20);
        ranges::generate(a, [&] { return static_cast<T>(dis(gen)); });
    }

    for (auto _ : state) {
        benchmark::DoNotOptimize(a);

        if constexpr (Operation == Op::Min) {
            benchmark::DoNotOptimize(ranges::min_element(a));
        } else if constexpr (Operation == Op::Max) {
            benchmark::DoNotOptimize(ranges::max_element(a));
        } else if constexpr (Operation == Op::Both) {
            benchmark::DoNotOptimize(ranges::minmax_element(a));
        } else if constexpr (Operation == Op::Min_val) {
            benchmark::DoNotOptimize(ranges::min(a));
        } else if constexpr (Operation == Op::Max_val) {
            benchmark::DoNotOptimize(ranges::max(a));
        } else if constexpr (Operation == Op::Both_val) {
            benchmark::DoNotOptimize(ranges::minmax(a));
        }
    }
}

template <size_t ElementSize>
void common_arg(auto bm) {
    bm->Arg(8021);
    // AVX tail tests
    bm->Arg(63 / ElementSize);
}

BENCHMARK(bm<uint8_t, Op::Min>)->Apply(common_arg<1>);
BENCHMARK(bm<uint8_t, Op::Max>)->Apply(common_arg<1>);
BENCHMARK(bm<uint8_t, Op::Both>)->Apply(common_arg<1>);
BENCHMARK(bm<uint8_t, Op::Min_val>)->Apply(common_arg<1>);
BENCHMARK(bm<uint8_t, Op::Max_val>)->Apply(common_arg<1>);
BENCHMARK(bm<uint8_t, Op::Both_val>)->Apply(common_arg<1>);

BENCHMARK(bm<uint16_t, Op::Min>)->Apply(common_arg<2>);
BENCHMARK(bm<uint16_t, Op::Max>)->Apply(common_arg<2>);
BENCHMARK(bm<uint16_t, Op::Both>)->Apply(common_arg<2>);
BENCHMARK(bm<uint16_t, Op::Min_val>)->Apply(common_arg<2>);
BENCHMARK(bm<uint16_t, Op::Max_val>)->Apply(common_arg<2>);
BENCHMARK(bm<uint16_t, Op::Both_val>)->Apply(common_arg<2>);

BENCHMARK(bm<uint32_t, Op::Min>)->Apply(common_arg<4>);
BENCHMARK(bm<uint32_t, Op::Max>)->Apply(common_arg<4>);
BENCHMARK(bm<uint32_t, Op::Both>)->Apply(common_arg<4>);
BENCHMARK(bm<uint32_t, Op::Min_val>)->Apply(common_arg<4>);
BENCHMARK(bm<uint32_t, Op::Max_val>)->Apply(common_arg<4>);
BENCHMARK(bm<uint32_t, Op::Both_val>)->Apply(common_arg<4>);

BENCHMARK(bm<uint64_t, Op::Min>)->Apply(common_arg<8>);
BENCHMARK(bm<uint64_t, Op::Max>)->Apply(common_arg<8>);
BENCHMARK(bm<uint64_t, Op::Both>)->Apply(common_arg<8>);
BENCHMARK(bm<uint64_t, Op::Min_val>)->Apply(common_arg<8>);
BENCHMARK(bm<uint64_t, Op::Max_val>)->Apply(common_arg<8>);
BENCHMARK(bm<uint64_t, Op::Both_val>)->Apply(common_arg<8>);

BENCHMARK(bm<int8_t, Op::Min>)->Apply(common_arg<1>);
BENCHMARK(bm<int8_t, Op::Max>)->Apply(common_arg<1>);
BENCHMARK(bm<int8_t, Op::Both>)->Apply(common_arg<1>);
BENCHMARK(bm<int8_t, Op::Min_val>)->Apply(common_arg<1>);
BENCHMARK(bm<int8_t, Op::Max_val>)->Apply(common_arg<1>);
BENCHMARK(bm<int8_t, Op::Both_val>)->Apply(common_arg<1>);

BENCHMARK(bm<int16_t, Op::Min>)->Apply(common_arg<2>);
BENCHMARK(bm<int16_t, Op::Max>)->Apply(common_arg<2>);
BENCHMARK(bm<int16_t, Op::Both>)->Apply(common_arg<2>);
BENCHMARK(bm<int16_t, Op::Min_val>)->Apply(common_arg<2>);
BENCHMARK(bm<int16_t, Op::Max_val>)->Apply(common_arg<2>);
BENCHMARK(bm<int16_t, Op::Both_val>)->Apply(common_arg<2>);

BENCHMARK(bm<int32_t, Op::Min>)->Apply(common_arg<4>);
BENCHMARK(bm<int32_t, Op::Max>)->Apply(common_arg<4>);
BENCHMARK(bm<int32_t, Op::Both>)->Apply(common_arg<4>);
BENCHMARK(bm<int32_t, Op::Min_val>)->Apply(common_arg<4>);
BENCHMARK(bm<int32_t, Op::Max_val>)->Apply(common_arg<4>);
BENCHMARK(bm<int32_t, Op::Both_val>)->Apply(common_arg<4>);

BENCHMARK(bm<int64_t, Op::Min>)->Apply(common_arg<8>);
BENCHMARK(bm<int64_t, Op::Max>)->Apply(common_arg<8>);
BENCHMARK(bm<int64_t, Op::Both>)->Apply(common_arg<8>);
BENCHMARK(bm<int64_t, Op::Min_val>)->Apply(common_arg<8>);
BENCHMARK(bm<int64_t, Op::Max_val>)->Apply(common_arg<8>);
BENCHMARK(bm<int64_t, Op::Both_val>)->Apply(common_arg<8>);

BENCHMARK(bm<float, Op::Min>)->Apply(common_arg<4>);
BENCHMARK(bm<float, Op::Max>)->Apply(common_arg<4>);
BENCHMARK(bm<float, Op::Both>)->Apply(common_arg<4>);
BENCHMARK(bm<float, Op::Min_val>)->Apply(common_arg<4>);
BENCHMARK(bm<float, Op::Max_val>)->Apply(common_arg<4>);
BENCHMARK(bm<float, Op::Both_val>)->Apply(common_arg<4>);

BENCHMARK(bm<double, Op::Min>)->Apply(common_arg<8>);
BENCHMARK(bm<double, Op::Max>)->Apply(common_arg<8>);
BENCHMARK(bm<double, Op::Both>)->Apply(common_arg<8>);
BENCHMARK(bm<double, Op::Min_val>)->Apply(common_arg<8>);
BENCHMARK(bm<double, Op::Max_val>)->Apply(common_arg<8>);
BENCHMARK(bm<double, Op::Both_val>)->Apply(common_arg<8>);

BENCHMARK_MAIN();
