// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <benchmark/benchmark.h>
//
#include <algorithm>
#include <cstddef>
#include <functional>
#include <vector>

#include "skewed_allocator.hpp"

using namespace std;

enum class alg { any_, all_, none_ };
enum class content { ones_then_zeros, zeros_then_ones };

template <alg Alg, content Content, class Pred = identity>
void meow_of(benchmark::State& state) {
    const auto size = static_cast<size_t>(state.range(0));
    vector<bool, not_highly_aligned_allocator<bool>> source(size);

    if constexpr (Content == content::ones_then_zeros) {
        fill(source.begin(), source.begin() + source.size() / 2, true);
    } else {
        fill(source.begin() + source.size() / 2, source.end(), true);
    }

    for (auto _ : state) {
        benchmark::DoNotOptimize(source);
        bool result;
        if constexpr (Alg == alg::any_) {
            result = any_of(source.begin(), source.end(), Pred{});
        } else if constexpr (Alg == alg::all_) {
            result = all_of(source.begin(), source.end(), Pred{});
        } else {
            result = none_of(source.begin(), source.end(), Pred{});
        }
        benchmark::DoNotOptimize(result);
    }
}

void common_args(auto bm) {
    bm->RangeMultiplier(64)->Range(64, 64 << 10);
}

using not_ = logical_not<>;

BENCHMARK(meow_of<alg::any_, content::zeros_then_ones>)->Apply(common_args);
BENCHMARK(meow_of<alg::any_, content::ones_then_zeros, not_>)->Apply(common_args);
BENCHMARK(meow_of<alg::all_, content::ones_then_zeros>)->Apply(common_args);
BENCHMARK(meow_of<alg::none_, content::zeros_then_ones>)->Apply(common_args);

BENCHMARK_MAIN();
