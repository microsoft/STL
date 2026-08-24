// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <benchmark/benchmark.h>

#include <__msvc_int128.hpp>
#include <array>
#include <cstdint>
#include <random>
#include <utility>
#include <vector>

using namespace std;

_NODISCARD constexpr _Signed128 old_mul128(_Signed128 _Left, _Signed128 _Right) {
    bool _negative = false;
    _Left._Strip_negative(_negative);
    _Right._Strip_negative(_negative);
    _Signed128 _Result{_Base128::_Multiply(_Left, _Right)};
    if (_negative) {
        _Result = -_Result;
    }
    return _Result;
}

vector<pair<_Signed128, _Signed128>> init_test_set() {
    vector<pair<_Signed128, _Signed128>> vec(1'000'000);
    mt19937_64 mt64{};
    for (auto& [val1, val2] : vec) {
        val1._Word[0] = mt64();
        val1._Word[1] = mt64();
        val2._Word[0] = mt64();
        val2._Word[1] = mt64();
    }
    return vec;
}

void bm_signed128_mul(benchmark::State& state) {
    const auto vec = init_test_set();

    auto it        = vec.begin();
    _Signed128 res = 0;

    for (auto _ : state) {
        res = it->first * it->second;

        benchmark::DoNotOptimize(res);
        ++it;
        if (it == vec.end()) {
            it = vec.begin();
        }
    }
}

void bm_signed128_oldmul(benchmark::State& state) {
    const auto vec = init_test_set();

    auto it        = vec.begin();
    _Signed128 res = 0;

    for (auto _ : state) {
        res = old_mul128(it->first, it->second);

        benchmark::DoNotOptimize(res);
        ++it;
        if (it == vec.end()) {
            it = vec.begin();
        }
    }
}

BENCHMARK(bm_signed128_mul);
BENCHMARK(bm_signed128_oldmul);

BENCHMARK_MAIN();
