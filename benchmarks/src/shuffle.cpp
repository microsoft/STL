// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <random>
#include <type_traits>
#include <vector>
using namespace std;

enum class alg_type { std_fn, rng };

template <class T, alg_type Alg>
void bm_shuffle(benchmark::State& state) {
    static_assert(is_unsigned_v<T>, "T must be unsigned so iota() doesn't have to worry about overflow.");

    const auto n = static_cast<size_t>(state.range(0));
    vector<T> v(n);
    iota(v.begin(), v.end(), T{0});
    mt19937_64 urbg;

    for (auto _ : state) {
        benchmark::DoNotOptimize(v);
        if constexpr (Alg == alg_type::rng) {
            ranges::shuffle(v, urbg);
        } else {
            shuffle(v.begin(), v.end(), urbg);
        }
    }
}

void common_args(auto bm) {
    bm->Arg(1 << 20);
}

BENCHMARK(bm_shuffle<uint8_t, alg_type::std_fn>)->Apply(common_args);
BENCHMARK(bm_shuffle<uint16_t, alg_type::std_fn>)->Apply(common_args);
BENCHMARK(bm_shuffle<uint32_t, alg_type::std_fn>)->Apply(common_args);
BENCHMARK(bm_shuffle<uint64_t, alg_type::std_fn>)->Apply(common_args);

BENCHMARK(bm_shuffle<uint8_t, alg_type::rng>)->Apply(common_args);
BENCHMARK(bm_shuffle<uint16_t, alg_type::rng>)->Apply(common_args);
BENCHMARK(bm_shuffle<uint32_t, alg_type::rng>)->Apply(common_args);
BENCHMARK(bm_shuffle<uint64_t, alg_type::rng>)->Apply(common_args);

BENCHMARK_MAIN();
