// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstdint>
#include <random>
#include <type_traits>
#include <vector>

#include "skewed_allocator.hpp"

enum class alg_type { std_fn, rng };

template <alg_type Type, class T>
void u(benchmark::State& state) {
    std::mt19937_64 gen(22033);
    using TD = std::conditional_t<sizeof(T) == 1, int, T>;
    std::binomial_distribution<TD> dis(5);

    std::vector<T, not_highly_aligned_allocator<T>> src(2552);
    std::generate(src.begin(), src.end(), [&] { return static_cast<T>(dis(gen)); });

    std::vector<T, not_highly_aligned_allocator<T>> v;
    v.reserve(src.size());
    for (auto _ : state) {
        v = src;
        benchmark::DoNotOptimize(v);
        if constexpr (Type == alg_type::std_fn) {
            benchmark::DoNotOptimize(std::unique(v.begin(), v.end()));
        } else {
            benchmark::DoNotOptimize(std::ranges::unique(v));
        }
    }
}

template <alg_type Type, class T>
void uc(benchmark::State& state) {
    std::mt19937_64 gen(22033);
    using TD = std::conditional_t<sizeof(T) == 1, int, T>;
    std::binomial_distribution<TD> dis(5);

    std::vector<T, not_highly_aligned_allocator<T>> src(2552);
    std::generate(src.begin(), src.end(), [&] { return static_cast<T>(dis(gen)); });

    std::vector<T, not_highly_aligned_allocator<T>> v;
    v.resize(src.size());
    for (auto _ : state) {
        benchmark::DoNotOptimize(src);
        benchmark::DoNotOptimize(v);
        if constexpr (Type == alg_type::std_fn) {
            benchmark::DoNotOptimize(std::unique_copy(src.begin(), src.end(), v.begin()));
        } else {
            benchmark::DoNotOptimize(std::ranges::unique_copy(src, v.begin()));
        }
    }
}

BENCHMARK(u<alg_type::std_fn, std::uint8_t>);
BENCHMARK(u<alg_type::std_fn, std::uint16_t>);
BENCHMARK(u<alg_type::std_fn, std::uint32_t>);
BENCHMARK(u<alg_type::std_fn, std::uint64_t>);

BENCHMARK(u<alg_type::rng, std::uint8_t>);
BENCHMARK(u<alg_type::rng, std::uint16_t>);
BENCHMARK(u<alg_type::rng, std::uint32_t>);
BENCHMARK(u<alg_type::rng, std::uint64_t>);

BENCHMARK(uc<alg_type::std_fn, std::uint8_t>);
BENCHMARK(uc<alg_type::std_fn, std::uint16_t>);
BENCHMARK(uc<alg_type::std_fn, std::uint32_t>);
BENCHMARK(uc<alg_type::std_fn, std::uint64_t>);

BENCHMARK(uc<alg_type::rng, std::uint8_t>);
BENCHMARK(uc<alg_type::rng, std::uint16_t>);
BENCHMARK(uc<alg_type::rng, std::uint32_t>);
BENCHMARK(uc<alg_type::rng, std::uint64_t>);

BENCHMARK_MAIN();
