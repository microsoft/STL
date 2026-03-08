// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <benchmark/benchmark.h>
//
#include <algorithm>
#include <cstddef>
#include <random>
#include <vector>

#include "utility.hpp"

using namespace std;

enum class equality { eq, neq };
enum class args { three, four };


template <equality Eq, args Args>
void perm_arr_check(benchmark::State& state) {
    const auto size = static_cast<size_t>(state.range(0));
    vector<bool> v1 = random_vector<bool>(size);
    vector<bool> v2;
    if constexpr (Eq == equality::eq) {
        v2 = v1;
    } else {
        v2 = random_vector<bool>(size, 1u);
    }

    auto a1 = make_unique<bool[]>(size);
    auto a2 = make_unique<bool[]>(size);
    copy(v1.begin(), v1.end(), a1.get());
    copy(v2.begin(), v2.end(), a2.get());

    for (auto _ : state) {
        benchmark::DoNotOptimize(v1);
        benchmark::DoNotOptimize(v2);
        bool r;
        if constexpr (Args == args::three) {
            r = is_permutation(a1.get(), a1.get() + size, a2.get());
        } else {
            r = is_permutation(a1.get(), a1.get() + size, a2.get(), a2.get() + size);
        }
        benchmark::DoNotOptimize(r);
    }
}


template <equality Eq, args Args>
void perm_vbool_check(benchmark::State& state) {
    const auto size = static_cast<size_t>(state.range(0));
    vector<bool> v1 = random_vector<bool>(size);
    vector<bool> v2;
    if constexpr (Eq == equality::eq) {
        v2 = v1;
    }
    else
    {
        v2 = random_vector<bool>(size, 1u);
    }

    for (auto _ : state) {
        benchmark::DoNotOptimize(v1);
        benchmark::DoNotOptimize(v2);
        bool r;
        if constexpr (Args == args::three) {
            r = is_permutation(v1.begin(), v1.end(), v2.begin());
        } else {
            r = is_permutation(v1.begin(), v1.end(), v2.begin(), v2.end());
        }
        benchmark::DoNotOptimize(r);
    }
}

BENCHMARK(perm_arr_check<equality::eq, args::three>)->RangeMultiplier(64)->Range(64, 64 << 10);
BENCHMARK(perm_arr_check<equality::eq, args::four>)->RangeMultiplier(64)->Range(64, 64 << 10);
BENCHMARK(perm_arr_check<equality::neq, args::three>)->RangeMultiplier(64)->Range(64, 64 << 10);
BENCHMARK(perm_arr_check<equality::neq, args::four>)->RangeMultiplier(64)->Range(64, 64 << 10);

BENCHMARK(perm_vbool_check<equality::eq, args::three>)->RangeMultiplier(64)->Range(64, 64 << 10);
BENCHMARK(perm_vbool_check<equality::eq, args::four>)->RangeMultiplier(64)->Range(64, 64 << 10);
BENCHMARK(perm_vbool_check<equality::neq, args::three>)->RangeMultiplier(64)->Range(64, 64 << 10);
BENCHMARK(perm_vbool_check<equality::neq, args::four>)->RangeMultiplier(64)->Range(64, 64 << 10);

BENCHMARK_MAIN();
