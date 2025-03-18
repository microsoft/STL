// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <vector>

#include "skewed_allocator.hpp"

using namespace std;

// NB: This particular algorthm has std and ranges implementations with different perf charactterstitics!

enum class AlgType { Std, Rng };

template <class T, AlgType Alg>
void bm(benchmark::State& state) {
    const auto size = static_cast<size_t>(state.range(0));

    constexpr size_t count = 1;

    constexpr T no_match{'-'};
    constexpr T match{'*'};

    vector<T, not_highly_aligned_allocator<T>> v(size, no_match);

    fill(v.begin() + v.size() / 2, v.end(), match);

    for (auto _ : state) {
        if constexpr (Alg == AlgType::Std) {
            benchmark::DoNotOptimize(search_n(v.begin(), v.end(), count, match));
        } else if constexpr (Alg == AlgType::Rng) {
            benchmark::DoNotOptimize(ranges::search_n(v, count, match));
        }
    }
}

void common_args(auto bm) {
    bm->Arg(3000);
}

BENCHMARK(bm<uint8_t, AlgType::Std>)->Apply(common_args);
BENCHMARK(bm<uint8_t, AlgType::Rng>)->Apply(common_args);

BENCHMARK(bm<uint16_t, AlgType::Std>)->Apply(common_args);
BENCHMARK(bm<uint16_t, AlgType::Rng>)->Apply(common_args);

BENCHMARK(bm<uint32_t, AlgType::Std>)->Apply(common_args);
BENCHMARK(bm<uint32_t, AlgType::Rng>)->Apply(common_args);

BENCHMARK(bm<uint64_t, AlgType::Std>)->Apply(common_args);
BENCHMARK(bm<uint64_t, AlgType::Rng>)->Apply(common_args);

BENCHMARK_MAIN();
