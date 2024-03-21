// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <vector>

using namespace std;

template <class T, size_t Pos, size_t NSize, size_t HSize = Pos * 2, size_t Which = 0>
void bm(benchmark::State& state) {
    vector<T> h(HSize, T{'.'});
    vector<T> n(NSize);
    iota(n.begin(), n.end(), T{'a'});

    static_assert(Pos < HSize);
    static_assert(Which < NSize);
    h[Pos] = n[Which];

    for (auto _ : state) {
        benchmark::DoNotOptimize(find_first_of(h.begin(), h.end(), n.begin(), n.end()));
    }
}

BENCHMARK(bm<uint8_t, 2, 3>);
BENCHMARK(bm<uint16_t, 2, 3>);

BENCHMARK(bm<uint8_t, 7, 4>);
BENCHMARK(bm<uint16_t, 7, 4>);

BENCHMARK(bm<uint8_t, 9, 3>);
BENCHMARK(bm<uint16_t, 9, 3>);

BENCHMARK(bm<uint8_t, 22, 5>);
BENCHMARK(bm<uint16_t, 22, 5>);

BENCHMARK(bm<uint8_t, 3056, 7>);
BENCHMARK(bm<uint16_t, 3056, 7>);

BENCHMARK(bm<uint8_t, 1011, 11>);
BENCHMARK(bm<uint16_t, 1011, 11>);

BENCHMARK_MAIN();
