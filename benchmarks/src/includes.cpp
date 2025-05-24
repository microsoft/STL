// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstdint>
#include <iostream>
#include <vector>

#include "skewed_allocator.hpp"
#include "utility.hpp"

using namespace std;

enum class alg_type { std_fn, rng };

enum needle_spread { dense, sparse, random_pick };

template <class T, alg_type Alg>
void bm_includes(benchmark::State& state) {
    const auto hay_size    = static_cast<size_t>(state.range(0));
    const auto needle_size = static_cast<size_t>(state.range(1));
    const auto s           = static_cast<needle_spread>(state.range(2));
    const auto m           = static_cast<bool>(state.range(3));

    auto hay = random_vector<T, not_highly_aligned_allocator>(hay_size);
    ranges::sort(hay);

    vector<T, not_highly_aligned_allocator<T>> needle(needle_size);
    switch (s) {
    case needle_spread::dense:
        needle.assign(hay.begin() + hay_size / 2 - needle_size / 2, hay.begin() + hay_size / 2 + (needle_size + 1) / 2);
        break;

    case needle_spread::sparse:
        for (size_t i = 0; i != needle_size; ++i) {
            needle[i] = hay[hay_size * i / needle_size + hay_size / (needle_size * 2)];
        }
        break;

    case needle_spread::random_pick:
        ranges::sample(hay, needle.begin(), needle_size, std::mt19937{});
        break;
    }

    if (!m) {
        const T v = needle[needle_size / 2];
        const T r = static_cast<T>(static_cast<make_unsigned_t<T>>(v + 1));
        ranges::replace(hay, v, r);
    }

    for (auto _ : state) {
        benchmark::DoNotOptimize(hay);
        benchmark::DoNotOptimize(needle);
        bool found;
        if constexpr (Alg == alg_type::rng) {
            found = ranges::includes(hay, needle);
        } else {
            found = includes(hay.begin(), hay.end(), needle.begin(), needle.end());
        }
        benchmark::DoNotOptimize(found);
        if (found != m) {
            cerr << "Unexpected 'includes' result: " << found << '\n';
            abort();
        }
    }
}

void common_args(auto bm) {
    for (const auto& hay_size : {300, 3000}) {
        for (const auto& needle_size : {3, 22, 105, 290}) {
            for (const auto& s : {needle_spread::dense, needle_spread::sparse, needle_spread::random_pick}) {
                for (const auto& m : {true, false}) {
                    bm->Args({hay_size, needle_size, s, m});
                }
            }
        }
    }
}

BENCHMARK(bm_includes<int8_t, alg_type::std_fn>)->Apply(common_args);
BENCHMARK(bm_includes<int16_t, alg_type::std_fn>)->Apply(common_args);
BENCHMARK(bm_includes<int32_t, alg_type::std_fn>)->Apply(common_args);
BENCHMARK(bm_includes<int64_t, alg_type::std_fn>)->Apply(common_args);

BENCHMARK(bm_includes<int8_t, alg_type::rng>)->Apply(common_args);
BENCHMARK(bm_includes<int16_t, alg_type::rng>)->Apply(common_args);
BENCHMARK(bm_includes<int32_t, alg_type::rng>)->Apply(common_args);
BENCHMARK(bm_includes<int64_t, alg_type::rng>)->Apply(common_args);

BENCHMARK_MAIN();
