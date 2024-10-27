// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <ranges>
#include <vector>

enum class Op {
    FindSized,
    FindUnsized,
    Count,
};

using namespace std;

template <class T, Op Operation>
void bm(benchmark::State& state) {
    const auto size = static_cast<size_t>(state.range(0));
    const auto pos  = static_cast<size_t>(state.range(1));

    vector<T> a(size, T{'0'});

    if (pos < size) {
        a[pos] = T{'1'};
    } else {
        if constexpr (Operation == Op::FindUnsized) {
            abort();
        }
    }

    for (auto _ : state) {
        if constexpr (Operation == Op::FindSized) {
            benchmark::DoNotOptimize(ranges::find(a.begin(), a.end(), T{'1'}));
        } else if constexpr (Operation == Op::FindUnsized) {
            benchmark::DoNotOptimize(ranges::find(a.begin(), unreachable_sentinel, T{'1'}));
        } else if constexpr (Operation == Op::Count) {
            benchmark::DoNotOptimize(ranges::count(a.begin(), a.end(), T{'1'}));
        }
    }
}

void common_args(auto bm) {
    bm->Args({8021, 3056});
    // AVX tail tests
    bm->Args({63, 62})->Args({31, 30})->Args({15, 14})->Args({7, 6});
}


BENCHMARK(bm<uint8_t, Op::FindSized>)->Apply(common_args);
BENCHMARK(bm<uint8_t, Op::FindUnsized>)->Apply(common_args);
BENCHMARK(bm<uint8_t, Op::Count>)->Apply(common_args);

BENCHMARK(bm<uint16_t, Op::FindSized>)->Apply(common_args);
BENCHMARK(bm<uint16_t, Op::Count>)->Apply(common_args);

BENCHMARK(bm<uint32_t, Op::FindSized>)->Apply(common_args);
BENCHMARK(bm<uint32_t, Op::Count>)->Apply(common_args);

BENCHMARK(bm<uint64_t, Op::FindSized>)->Apply(common_args);
BENCHMARK(bm<uint64_t, Op::Count>)->Apply(common_args);

BENCHMARK_MAIN();
