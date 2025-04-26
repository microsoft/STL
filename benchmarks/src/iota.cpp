// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <benchmark/benchmark.h>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <vector>

#include "skewed_allocator.hpp"

enum class Alg {
    Std,
    Rng,
};

template <class T, Alg Algorithm>
void bm(benchmark::State& state) {
    const auto size = static_cast<std::size_t>(state.range(0));

    std::vector<T, not_highly_aligned_allocator<T>> a(size);

    for (auto _ : state) {
        if constexpr (Algorithm == Alg::Std) {
            std::iota(a.begin(), a.end(), T{22});
        } else if constexpr (Algorithm == Alg::Rng) {
            std::ranges::iota(a, T{22});
        }

        benchmark::DoNotOptimize(a);
    }
}

void common_args(auto bm) {
    bm->Arg(7)->Arg(18)->Arg(43)->Arg(131)->Arg(315)->Arg(1212);
}

BENCHMARK(bm<std::uint32_t, Alg::Std>)->Apply(common_args);
BENCHMARK(bm<std::uint64_t, Alg::Std>)->Apply(common_args);

BENCHMARK(bm<std::uint32_t, Alg::Rng>)->Apply(common_args);
BENCHMARK(bm<std::uint64_t, Alg::Rng>)->Apply(common_args);

BENCHMARK_MAIN();
