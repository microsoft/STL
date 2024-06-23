// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <numeric>
#include <string>
#include <type_traits>
#include <vector>

using namespace std;

enum class AlgType : bool { std, str_member };

template <AlgType Alg, class T, T Start = T{'a'}>
void bm(benchmark::State& state) {
    const size_t Pos   = static_cast<size_t>(state.range(0));
    const size_t NSize = static_cast<size_t>(state.range(1));
    const size_t HSize = Pos * 2;
    const size_t Which = 0;

    using container = conditional_t<Alg == AlgType::str_member, basic_string<T>, vector<T>>;

    container h(HSize, T{'.'});
    container n(NSize, T{0});
    iota(n.begin(), n.end(), Start);

    if (Pos >= HSize || Which >= NSize) {
        abort();
    }

    h[Pos] = n[Which];

    for (auto _ : state) {
        benchmark::DoNotOptimize(h);
        benchmark::DoNotOptimize(n);
        if constexpr (Alg == AlgType::str_member) {
            benchmark::DoNotOptimize(h.find_first_of(n.data(), n.size()));
        } else {
            benchmark::DoNotOptimize(find_first_of(h.begin(), h.end(), n.begin(), n.end()));
        }
    }
}

void common_args(auto bm) {
    bm->Args({2, 3})->Args({7, 4})->Args({9, 3})->Args({22, 5})->Args({58, 2});
    bm->Args({102, 4})->Args({325, 1})->Args({1011, 11})->Args({1502, 23})->Args({3056, 7});
}

BENCHMARK(bm<AlgType::std, uint8_t>)->Apply(common_args);
BENCHMARK(bm<AlgType::std, uint16_t>)->Apply(common_args);
BENCHMARK(bm<AlgType::std, uint32_t>)->Apply(common_args);
BENCHMARK(bm<AlgType::std, uint64_t>)->Apply(common_args);

BENCHMARK(bm<AlgType::str_member, char>)->Apply(common_args);
BENCHMARK(bm<AlgType::str_member, wchar_t>)->Apply(common_args);
BENCHMARK(bm<AlgType::str_member, wchar_t, L'\x03B1'>)->Apply(common_args);

BENCHMARK_MAIN();
