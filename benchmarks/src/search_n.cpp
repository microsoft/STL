// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstddef>
#include <cstdint>
#include <random>
#include <vector>

#include "skewed_allocator.hpp"

using namespace std;

// NB: This particular algorithm has std and ranges non-vectorized implementations with different perf characteristics!

enum class AlgType { Std, Rng };

enum class PatternType {
    TwoZones,
    DenseSmallSequences,
};

template <class T, AlgType Alg, PatternType Pattern>
void bm(benchmark::State& state) {
    const auto size = static_cast<size_t>(state.range(0));
    const auto n    = static_cast<size_t>(state.range(1));

    constexpr T no_match{'-'};
    constexpr T match{'*'};

    vector<T, not_highly_aligned_allocator<T>> v(size, no_match);

    if constexpr (Pattern == PatternType::TwoZones) {
        fill(v.begin() + v.size() / 2, v.end(), match);
    } else if constexpr (Pattern == PatternType::DenseSmallSequences) {
        if (size != 0 && n != 0) {
            mt19937 gen{7687239};

            uniform_int_distribution<size_t> len_dis(0, n - 1);

            size_t cur_len = len_dis(gen);

            for (size_t i = 0; i != size; ++i) {
                if (cur_len != 0) {
                    v[i] = match;
                    --cur_len;
                } else {
                    cur_len = len_dis(gen);
                }
            }
        }
    }

    for (auto _ : state) {
        if constexpr (Alg == AlgType::Std) {
            benchmark::DoNotOptimize(search_n(v.begin(), v.end(), n, match));
        } else if constexpr (Alg == AlgType::Rng) {
            benchmark::DoNotOptimize(ranges::search_n(v, n, match));
        }
    }
}

void common_args(auto bm) {
    for (const auto& n : {40, 18, 16, 14, 10, 8, 5, 4, 3, 2, 1}) {
        bm->ArgPair(3000, n);
    }
}

BENCHMARK(bm<uint8_t, AlgType::Std, PatternType::TwoZones>)->Apply(common_args);
BENCHMARK(bm<uint8_t, AlgType::Rng, PatternType::TwoZones>)->Apply(common_args);
BENCHMARK(bm<uint8_t, AlgType::Std, PatternType::DenseSmallSequences>)->Apply(common_args);
BENCHMARK(bm<uint8_t, AlgType::Rng, PatternType::DenseSmallSequences>)->Apply(common_args);

BENCHMARK(bm<uint16_t, AlgType::Std, PatternType::TwoZones>)->Apply(common_args);
BENCHMARK(bm<uint16_t, AlgType::Rng, PatternType::TwoZones>)->Apply(common_args);
BENCHMARK(bm<uint16_t, AlgType::Std, PatternType::DenseSmallSequences>)->Apply(common_args);
BENCHMARK(bm<uint16_t, AlgType::Rng, PatternType::DenseSmallSequences>)->Apply(common_args);

BENCHMARK(bm<uint32_t, AlgType::Std, PatternType::TwoZones>)->Apply(common_args);
BENCHMARK(bm<uint32_t, AlgType::Rng, PatternType::TwoZones>)->Apply(common_args);
BENCHMARK(bm<uint32_t, AlgType::Std, PatternType::DenseSmallSequences>)->Apply(common_args);
BENCHMARK(bm<uint32_t, AlgType::Rng, PatternType::DenseSmallSequences>)->Apply(common_args);

BENCHMARK(bm<uint64_t, AlgType::Std, PatternType::TwoZones>)->Apply(common_args);
BENCHMARK(bm<uint64_t, AlgType::Rng, PatternType::TwoZones>)->Apply(common_args);
BENCHMARK(bm<uint64_t, AlgType::Std, PatternType::DenseSmallSequences>)->Apply(common_args);
BENCHMARK(bm<uint64_t, AlgType::Rng, PatternType::DenseSmallSequences>)->Apply(common_args);


BENCHMARK_MAIN();
