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
    std::vector<T> src(std::begin(lorem_ipsum), std::end(lorem_ipsum));
    std::vector<T> v;
    v.reserve(std::size(lorem_ipsum));
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
