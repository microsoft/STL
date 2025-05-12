// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstdint>
#include <vector>

#include "skewed_allocator.hpp"
#include "utility.hpp"

using namespace std;

template <class T>
void bm_rotate(benchmark::State& state) {
    const auto size = static_cast<size_t>(state.range(0));
    const auto n    = static_cast<size_t>(state.range(1));

    auto v = random_vector<T, not_highly_aligned_allocator>(size);
    benchmark::DoNotOptimize(v);

    for (auto _ : state) {
        rotate(v.begin(), v.begin() + n, v.end());
        benchmark::DoNotOptimize(v);
    }
}

void common_args(auto bm) {
    bm->Args({3333, 2242})->Args({3333, 1111})->Args({3333, 501})->Args({3333, 12})->Args({3333, 5})->Args({3333, 1});
    bm->Args({333, 101})->Args({123, 32})->Args({23, 7})->Args({12, 5})->Args({3, 2});
}

struct color {
    uint16_t h;
    uint16_t s;
    uint16_t l;
};

BENCHMARK(bm_rotate<std::uint8_t>)->Apply(common_args);
BENCHMARK(bm_rotate<std::uint16_t>)->Apply(common_args);
BENCHMARK(bm_rotate<std::uint32_t>)->Apply(common_args);
BENCHMARK(bm_rotate<std::uint64_t>)->Apply(common_args);

BENCHMARK(bm_rotate<color>)->Apply(common_args);

BENCHMARK_MAIN();
