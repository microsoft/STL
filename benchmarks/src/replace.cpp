// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstdint>
#include <vector>

#include "lorem.hpp"

template <class T>
void r(benchmark::State& state) {
    const std::vector<T> a(lorem_ipsum.begin(), lorem_ipsum.end());
    std::vector<T> b(lorem_ipsum.size());

    for (auto _ : state) {
        b = a;
        std::replace(std::begin(b), std::end(b), T{'m'}, T{'w'});
    }
}

template <class T>
void rc(benchmark::State& state) {
    const std::vector<T> a(lorem_ipsum.begin(), lorem_ipsum.end());
    std::vector<T> b(lorem_ipsum.size());

    for (auto _ : state) {
        std::replace_copy(std::begin(a), std::end(a), std::begin(b), T{'m'}, T{'w'});
    }
}

template <class T>
void rc_if(benchmark::State& state) {
    const std::vector<T> a(lorem_ipsum.begin(), lorem_ipsum.end());
    std::vector<T> b(lorem_ipsum.size());

    for (auto _ : state) {
        (void) std::replace_copy_if(
            std::begin(a), std::end(a), std::begin(b), [](auto x) { return x <= T{'Z'}; }, T{'X'});
    }
}

// replace() is vectorized for 4 and 8 bytes only.
BENCHMARK(r<std::uint32_t>);
BENCHMARK(r<std::uint64_t>);

BENCHMARK(rc<std::uint8_t>);
BENCHMARK(rc<std::uint16_t>);
BENCHMARK(rc<std::uint32_t>);
BENCHMARK(rc<std::uint64_t>);

BENCHMARK(rc_if<std::uint8_t>);
BENCHMARK(rc_if<std::uint16_t>);
BENCHMARK(rc_if<std::uint32_t>);
BENCHMARK(rc_if<std::uint64_t>);

BENCHMARK_MAIN();
