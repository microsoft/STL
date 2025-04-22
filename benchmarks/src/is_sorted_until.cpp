// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstddef>
#include <cstdint>
#include <random>
#include <type_traits>
#include <vector>

#include "skewed_allocator.hpp"
#include "utility.hpp"

enum class AlgType { Std, Rng };

template <class T, AlgType Alg>
void bm_is_sorted_until(benchmark::State& state) {
    const std::size_t size     = static_cast<std::size_t>(state.range(0));
    const std::size_t sort_pos = static_cast<std::size_t>(state.range(1));

    std::vector<T, not_highly_aligned_allocator<T>> v;
    if constexpr (std::is_integral_v<T>) {
        v = random_vector<T, not_highly_aligned_allocator>(size);
    } else if constexpr (std::is_floating_point_v<T>) {
        v.resize(size, 0.0);
        std::mt19937 gen;
        std::normal_distribution<T> dis(0, 100000.0);
        std::generate_n(v.begin(), size, [&dis, &gen] { return dis(gen); });
    } else {
        static_assert(false);
    }

    std::sort(v.begin(), v.begin() + sort_pos);

    for (auto _ : state) {
        benchmark::DoNotOptimize(v);
        if constexpr (Alg == AlgType::Std) {
            benchmark::DoNotOptimize(std::is_sorted_until(v.begin(), v.end()));
        } else {
            benchmark::DoNotOptimize(std::ranges::is_sorted_until(v));
        }
    }
}

void common_args(auto bm) {
    bm->ArgPair(3000, 1800);
}

BENCHMARK(bm_is_sorted_until<std::int8_t, AlgType::Std>)->Apply(common_args);
BENCHMARK(bm_is_sorted_until<std::int8_t, AlgType::Rng>)->Apply(common_args);
BENCHMARK(bm_is_sorted_until<std::int16_t, AlgType::Std>)->Apply(common_args);
BENCHMARK(bm_is_sorted_until<std::int16_t, AlgType::Rng>)->Apply(common_args);
BENCHMARK(bm_is_sorted_until<std::int32_t, AlgType::Std>)->Apply(common_args);
BENCHMARK(bm_is_sorted_until<std::int32_t, AlgType::Rng>)->Apply(common_args);
BENCHMARK(bm_is_sorted_until<std::int64_t, AlgType::Std>)->Apply(common_args);
BENCHMARK(bm_is_sorted_until<std::int64_t, AlgType::Rng>)->Apply(common_args);

BENCHMARK(bm_is_sorted_until<std::uint8_t, AlgType::Std>)->Apply(common_args);
BENCHMARK(bm_is_sorted_until<std::uint8_t, AlgType::Rng>)->Apply(common_args);
BENCHMARK(bm_is_sorted_until<std::uint16_t, AlgType::Std>)->Apply(common_args);
BENCHMARK(bm_is_sorted_until<std::uint16_t, AlgType::Rng>)->Apply(common_args);
BENCHMARK(bm_is_sorted_until<std::uint32_t, AlgType::Std>)->Apply(common_args);
BENCHMARK(bm_is_sorted_until<std::uint32_t, AlgType::Rng>)->Apply(common_args);
BENCHMARK(bm_is_sorted_until<std::uint64_t, AlgType::Std>)->Apply(common_args);
BENCHMARK(bm_is_sorted_until<std::uint64_t, AlgType::Rng>)->Apply(common_args);

BENCHMARK(bm_is_sorted_until<float, AlgType::Std>)->Apply(common_args);
BENCHMARK(bm_is_sorted_until<float, AlgType::Rng>)->Apply(common_args);
BENCHMARK(bm_is_sorted_until<double, AlgType::Std>)->Apply(common_args);
BENCHMARK(bm_is_sorted_until<double, AlgType::Rng>)->Apply(common_args);

BENCHMARK_MAIN();
