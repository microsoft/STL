// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <numeric>
#include <vector>

using namespace std;

template <class T>
void bm(benchmark::State& state) {
    const size_t Pos   = static_cast<size_t>(state.range(0));
    const size_t NSize = static_cast<size_t>(state.range(1));
    const size_t HSize = Pos * 2;
    const size_t Which = 0;

    vector<T> h(HSize, T{'.'});
    vector<T> n(NSize);
    iota(n.begin(), n.end(), T{'a'});

    if (Pos >= HSize || Which >= NSize) {
        abort();
    }

    h[Pos] = n[Which];

    for (auto _ : state) {
        benchmark::DoNotOptimize(find_first_of(h.begin(), h.end(), n.begin(), n.end()));
    }
}

void common_args(auto bm) {
    bm->Args({2, 3})->Args({7, 4})->Args({9, 3})->Args({22, 5})->Args({58, 2});
    bm->Args({102, 4})->Args({325, 1})->Args({1011, 11})->Args({3056, 7});
}

BENCHMARK(bm<uint8_t>)->Apply(common_args);
BENCHMARK(bm<uint16_t>)->Apply(common_args);
BENCHMARK(bm<uint32_t>)->Apply(common_args);
BENCHMARK(bm<uint64_t>)->Apply(common_args);

BENCHMARK_MAIN();
