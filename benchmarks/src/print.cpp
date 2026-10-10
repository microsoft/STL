// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <benchmark/benchmark.h>

#include <print>

#include "lorem.hpp"

template <std::size_t s>
void print(benchmark::State& state) {
    constexpr auto sub = lorem_ipsum.substr(0, s);
    for (auto _ : state) {
        std::print(sub);
    }
}

BENCHMARK(print<0>);
BENCHMARK(print<4>);
BENCHMARK(print<16>);
BENCHMARK(print<64>);
BENCHMARK(print<256>);
BENCHMARK(print<1024>);

static_assert(lorem_ipsum.size() >= 1024);

BENCHMARK_MAIN();
