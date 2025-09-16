// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstdint>
#include <vector>

#include "skewed_allocator.hpp"
#include "utility.hpp"

template <class T>
void r(benchmark::State& state) {
    const auto size = static_cast<size_t>(state.range(0));
    auto v          = random_vector<T, not_highly_aligned_allocator>(size);

    for (auto _ : state) {
        benchmark::DoNotOptimize(v);
        std::reverse(v.begin(), v.end());
    }
}

template <class T>
void rc(benchmark::State& state) {
    const auto size = static_cast<size_t>(state.range(0));
    auto v          = random_vector<T, not_highly_aligned_allocator>(size);
    std::vector<T, not_highly_aligned_allocator<T>> d(size);

    for (auto _ : state) {
        benchmark::DoNotOptimize(v);
        std::reverse_copy(v.begin(), v.end(), d.begin());
        benchmark::DoNotOptimize(d);
    }
}

void common_args(auto bm) {
    bm->Arg(3449);
    // AVX tail tests
    bm->Arg(63)->Arg(31)->Arg(15)->Arg(7);
}


BENCHMARK(r<std::uint8_t>)->Apply(common_args);
BENCHMARK(r<std::uint16_t>)->Apply(common_args);
BENCHMARK(r<std::uint32_t>)->Apply(common_args);
BENCHMARK(r<std::uint64_t>)->Apply(common_args);

BENCHMARK(rc<std::uint8_t>)->Apply(common_args);
BENCHMARK(rc<std::uint16_t>)->Apply(common_args);
BENCHMARK(rc<std::uint32_t>)->Apply(common_args);
BENCHMARK(rc<std::uint64_t>)->Apply(common_args);

BENCHMARK_MAIN();
