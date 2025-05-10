// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <numeric>
#include <random>
#include <type_traits>
#include <vector>

#include "skewed_allocator.hpp"

using namespace std;

template <class T>
void bm(benchmark::State& state) {
    mt19937 gen(96337);

    const size_t size = static_cast<size_t>(state.range(0));

    vector<T, not_highly_aligned_allocator<T>> input(size);
    vector<T, not_highly_aligned_allocator<T>> output(size);

    if constexpr (is_floating_point_v<T>) {
        normal_distribution<T> dis(0, 100000.0);
        ranges::generate(input, [&] { return dis(gen); });
    } else {
        static_assert(is_unsigned_v<T>, "This avoids signed integers to avoid UB; they shouldn't perform differently");
        uniform_int_distribution<conditional_t<sizeof(T) != 1, T, unsigned int>> dis(0, numeric_limits<T>::max());
        ranges::generate(input, [&] { return static_cast<T>(dis(gen)); });
    }

    for (auto _ : state) {
        benchmark::DoNotOptimize(input);
        adjacent_difference(input.begin(), input.end(), output.begin());
        benchmark::DoNotOptimize(output);
    }
}

void common_args(auto bm) {
    bm->Arg(2255);
}

#pragma warning(push)
#pragma warning(disable : 4244) // warning C4244: '=': conversion from 'int' to 'unsigned char', possible loss of data
BENCHMARK(bm<uint8_t>)->Apply(common_args);
BENCHMARK(bm<uint16_t>)->Apply(common_args);
#pragma warning(pop)

BENCHMARK(bm<uint32_t>)->Apply(common_args);
BENCHMARK(bm<uint64_t>)->Apply(common_args);

BENCHMARK(bm<float>)->Apply(common_args);
BENCHMARK(bm<double>)->Apply(common_args);

BENCHMARK_MAIN();
