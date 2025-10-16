// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstddef>
#include <cstdint>
#include <ranges>
#include <vector>

#include "skewed_allocator.hpp"

using namespace std;

constexpr int64_t no_pos = -1;

enum class op {
    mismatch,
    lexi,
};

struct color {
    uint16_t h;
    uint16_t s;
    uint16_t l;

    bool operator==(const color&) const = default;
};

constexpr color c1{30000, 40000, 20000};
constexpr color c2{30000, 40000, 30000};

template <class T, op Op, T MatchVal = T{'.'}, T MismatchVal = T{'x'}>
void bm(benchmark::State& state) {
    vector<T, not_highly_aligned_allocator<T>> a(static_cast<size_t>(state.range(0)), MatchVal);
    vector<T, not_highly_aligned_allocator<T>> b(static_cast<size_t>(state.range(0)), MatchVal);

    if (state.range(1) != no_pos) {
        b.at(static_cast<size_t>(state.range(1))) = MismatchVal;
    }

    for (auto _ : state) {
        if constexpr (Op == op::mismatch) {
            benchmark::DoNotOptimize(ranges::mismatch(a, b));
        } else if constexpr (Op == op::lexi) {
            benchmark::DoNotOptimize(ranges::lexicographical_compare(a, b));
        }
    }
}

void common_args(auto bm) {
    bm->Args({8, 3})->Args({24, 22})->Args({105, -1})->Args({4021, 3056});
}

BENCHMARK(bm<uint8_t, op::mismatch>)->Apply(common_args);
BENCHMARK(bm<uint16_t, op::mismatch>)->Apply(common_args);
BENCHMARK(bm<uint32_t, op::mismatch>)->Apply(common_args);
BENCHMARK(bm<uint64_t, op::mismatch>)->Apply(common_args);
BENCHMARK(bm<color, op::mismatch, c1, c2>)->Apply(common_args);

BENCHMARK(bm<uint8_t, op::lexi>)->Apply(common_args); // still optimized without vector algorithms using memcmp
BENCHMARK(bm<int8_t, op::lexi>)->Apply(common_args); // optimized with vector algorithms only
BENCHMARK(bm<uint16_t, op::lexi>)->Apply(common_args);
BENCHMARK(bm<uint32_t, op::lexi>)->Apply(common_args);
BENCHMARK(bm<uint64_t, op::lexi>)->Apply(common_args);

BENCHMARK_MAIN();
