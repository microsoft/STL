// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstdint>
#include <vector>

#include "lorem.hpp"

enum class alg_type { std_fn, rng };

template <alg_type Type, class T>
void r(benchmark::State& state) {
    const std::vector<T> src(lorem_ipsum.begin(), lorem_ipsum.end());
    std::vector<T> v;
    v.reserve(lorem_ipsum.size());
    for (auto _ : state) {
        v = src;
        benchmark::DoNotOptimize(v);
        if constexpr (Type == alg_type::std_fn) {
            benchmark::DoNotOptimize(std::remove(v.begin(), v.end(), T{'l'}));
        } else {
            benchmark::DoNotOptimize(std::ranges::remove(v, T{'l'}));
        }
    }
}

BENCHMARK(r<alg_type::std_fn, std::uint8_t>);
BENCHMARK(r<alg_type::std_fn, std::uint16_t>);
BENCHMARK(r<alg_type::std_fn, std::uint32_t>);
BENCHMARK(r<alg_type::std_fn, std::uint64_t>);

BENCHMARK(r<alg_type::rng, std::uint8_t>);
BENCHMARK(r<alg_type::rng, std::uint16_t>);
BENCHMARK(r<alg_type::rng, std::uint32_t>);
BENCHMARK(r<alg_type::rng, std::uint64_t>);

BENCHMARK_MAIN();
