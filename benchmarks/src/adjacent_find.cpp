// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <vector>

#include "skewed_allocator.hpp"

using namespace std;

enum class AlgType { Std, Rng };

template <AlgType Alg, class T>
void bm(benchmark::State& state) {
    const size_t size = static_cast<size_t>(state.range(0));
    const size_t pos  = static_cast<size_t>(state.range(1));

    vector<T, not_highly_aligned_allocator<T>> v(size);

    for (size_t i = 0; i != size; ++i) {
        v[i] = static_cast<T>(i & 3);
    }

    if (pos == 0 || pos >= size) {
        abort();
    }

    v[pos] = v[pos - 1];

    for (auto _ : state) {
        benchmark::DoNotOptimize(v);
        if constexpr (Alg == AlgType::Std) {
            benchmark::DoNotOptimize(adjacent_find(v.begin(), v.end()));
        } else {
            benchmark::DoNotOptimize(ranges::adjacent_find(v));
        }
    }
}

void common_args(auto bm) {
    bm->ArgPair(2525, 1142);
}

BENCHMARK(bm<AlgType::Std, int8_t>)->Apply(common_args);
BENCHMARK(bm<AlgType::Std, int16_t>)->Apply(common_args);
BENCHMARK(bm<AlgType::Std, int32_t>)->Apply(common_args);
BENCHMARK(bm<AlgType::Std, int64_t>)->Apply(common_args);

BENCHMARK(bm<AlgType::Rng, int8_t>)->Apply(common_args);
BENCHMARK(bm<AlgType::Rng, int16_t>)->Apply(common_args);
BENCHMARK(bm<AlgType::Rng, int32_t>)->Apply(common_args);
BENCHMARK(bm<AlgType::Rng, int64_t>)->Apply(common_args);

BENCHMARK_MAIN();
