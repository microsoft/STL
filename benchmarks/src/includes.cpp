// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <random>
#include <ranges>
#include <type_traits>
#include <vector>

#include "skewed_allocator.hpp"
#include "utility.hpp"

using namespace std;

enum class alg_type { std_fn, rng };

enum class needle_spread { dense, dense_random, sparse, sparse_random };

template <class T, alg_type Alg>
void bm_includes(benchmark::State& state) {
    const auto hay_size       = static_cast<size_t>(state.range(0));
    const auto needle_size    = static_cast<size_t>(state.range(1));
    const auto spread         = static_cast<needle_spread>(state.range(2));
    const auto expected_match = static_cast<bool>(state.range(3));

    auto hay = random_vector<T, not_highly_aligned_allocator>(hay_size);
    ranges::sort(hay);

    vector<T, not_highly_aligned_allocator<T>> needle;
    switch (spread) {
    case needle_spread::dense:
        needle.assign(hay.begin() + hay_size / 2 - needle_size / 2, hay.begin() + hay_size / 2 + (needle_size + 1) / 2);
        break;

    case needle_spread::dense_random:
        {
            mt19937 gen{};
            geometric_distribution<size_t> dis_dis{};
            vector<size_t> idx(needle_size);
            const size_t mid = needle_size / 2;
            idx[mid]         = hay_size / 2;

            const size_t max_shift = hay_size / needle_size;

            for (size_t i = mid; i != 0; --i) {
                idx[i - 1] = idx[i] - min(dis_dis(gen) + 1, max_shift);
            }

            for (size_t i = mid; i != needle_size - 1; ++i) {
                idx[i + 1] = idx[i] + min(dis_dis(gen) + 1, max_shift);
            }

            needle.assign_range(idx | views::transform([&hay](const size_t i) { return hay[i]; }));
        }
        break;

    case needle_spread::sparse:
        needle.resize(needle_size);
        for (size_t i = 0; i != needle_size; ++i) {
            needle[i] = hay[hay_size * i / needle_size + hay_size / (needle_size * 2)];
        }
        break;

    case needle_spread::sparse_random:
        needle.resize(needle_size);
        ranges::sample(hay, needle.begin(), needle_size, mt19937{});
        break;
    }

    if (!expected_match) {
        const T v = needle[needle_size / 2];
        const T r = v != numeric_limits<T>::max() ? v + 1 : v - 1;
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
        if (found != expected_match) {
            cerr << "Unexpected 'includes' result: " << found << '\n';
            abort();
        }
    }
}

void common_args(auto bm) {
    for (const auto& spread :
        {needle_spread::dense, needle_spread::dense_random, needle_spread::sparse, needle_spread::sparse_random}) {
        for (const auto& expected_match : {true, false}) {
            for (const auto& needle_size : {3, 22, 105, 1504, 2750}) {
                bm->Args({3000, needle_size, static_cast<underlying_type_t<needle_spread>>(spread), expected_match});
            }

            for (const auto& needle_size : {3, 22, 105, 290}) {
                bm->Args({300, needle_size, static_cast<underlying_type_t<needle_spread>>(spread), expected_match});
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
