// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstdint>
#include <vector>

#include "skewed_allocator.hpp"
#include "utility.hpp"

using namespace std;

enum class AlgType { Std, Rng };

template <class T, AlgType Alg>
void bm_rotate(benchmark::State& state) {
    const auto size = static_cast<size_t>(state.range(0));
    const auto n    = static_cast<size_t>(state.range(1));

    auto v = random_vector<T, not_highly_aligned_allocator>(size);
    benchmark::DoNotOptimize(v);

    for (auto _ : state) {
        if constexpr (Alg == AlgType::Std) {
            rotate(v.begin(), v.begin() + n, v.end());
        } else {
            ranges::rotate(v, v.begin() + n);
        }
        benchmark::DoNotOptimize(v);
    }
}

void common_args(auto bm) {
    bm->Args({3333, 2242})->Args({3332, 1666})->Args({3333, 1111})->Args({3333, 501});
    bm->Args({3333, 3300})->Args({3333, 12})->Args({3333, 5})->Args({3333, 1});
    bm->Args({333, 101})->Args({123, 32})->Args({23, 7})->Args({12, 5})->Args({3, 2});
}

struct color {
    uint16_t h;
    uint16_t s;
    uint16_t l;
};

BENCHMARK(bm_rotate<uint8_t, AlgType::Std>)->Apply(common_args);
BENCHMARK(bm_rotate<uint8_t, AlgType::Rng>)->Apply(common_args);
BENCHMARK(bm_rotate<uint16_t, AlgType::Std>)->Apply(common_args);
BENCHMARK(bm_rotate<uint16_t, AlgType::Rng>)->Apply(common_args);
BENCHMARK(bm_rotate<uint32_t, AlgType::Std>)->Apply(common_args);
BENCHMARK(bm_rotate<uint32_t, AlgType::Rng>)->Apply(common_args);
BENCHMARK(bm_rotate<uint64_t, AlgType::Std>)->Apply(common_args);
BENCHMARK(bm_rotate<uint64_t, AlgType::Rng>)->Apply(common_args);

BENCHMARK(bm_rotate<color, AlgType::Std>)->Apply(common_args);
BENCHMARK(bm_rotate<color, AlgType::Rng>)->Apply(common_args);

BENCHMARK(bm_rotate<uint8_t, AlgType::Std>)->Args({35000, 520})->Args({35000, 3000});

BENCHMARK_MAIN();
