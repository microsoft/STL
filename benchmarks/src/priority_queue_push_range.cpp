// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstddef>
#include <cstdint>
#include <queue>
#include <random>
#include <span>
#include <string>
#include <string_view>
#include <vector>
using namespace std;

constexpr size_t vec_size = 10'000;

template <class T, class Fn>
auto create_vec(Fn transformation) {
    vector<T> vec(vec_size);
    for (mt19937_64 rnd(1); auto& e : vec) {
        e = transformation(rnd());
    }
    return vec;
}

template <class T>
T cast_to(uint64_t val) {
    return static_cast<T>(val);
}

const auto vec_u8     = create_vec<uint8_t>(cast_to<uint8_t>);
const auto vec_u16    = create_vec<uint16_t>(cast_to<uint16_t>);
const auto vec_u32    = create_vec<uint32_t>(cast_to<uint32_t>);
const auto vec_u64    = create_vec<uint64_t>(cast_to<uint64_t>);
const auto vec_float  = create_vec<float>(cast_to<float>);
const auto vec_double = create_vec<double>(cast_to<double>);

const auto vec_str  = create_vec<string>([](uint64_t val) { return to_string(static_cast<uint32_t>(val)); });
const auto vec_wstr = create_vec<wstring>([](uint64_t val) { return to_wstring(static_cast<uint32_t>(val)); });

template <class T, const auto& Data>
void BM_push_range(benchmark::State& state) {
    const size_t frag_size = static_cast<size_t>(state.range(0));

    for (auto _ : state) {
        priority_queue<T> que;
        span spn{Data};

        while (!spn.empty()) {
            const size_t take_size = min(spn.size(), frag_size);
            que.push_range(spn.first(take_size));
            spn = spn.subspan(take_size);
        }
        benchmark::DoNotOptimize(que);
    }
}

void common_args(auto bm) {
    bm->RangeMultiplier(100)->Range(1, vec_size)->Arg(vec_size / 2 + 1);
}

BENCHMARK(BM_push_range<uint8_t, vec_u8>)->Apply(common_args);
BENCHMARK(BM_push_range<uint16_t, vec_u16>)->Apply(common_args);
BENCHMARK(BM_push_range<uint32_t, vec_u32>)->Apply(common_args);
BENCHMARK(BM_push_range<uint64_t, vec_u64>)->Apply(common_args);
BENCHMARK(BM_push_range<float, vec_float>)->Apply(common_args);
BENCHMARK(BM_push_range<double, vec_double>)->Apply(common_args);

BENCHMARK(BM_push_range<string_view, vec_str>)->Apply(common_args);
BENCHMARK(BM_push_range<string, vec_str>)->Apply(common_args);
BENCHMARK(BM_push_range<wstring_view, vec_wstr>)->Apply(common_args);
BENCHMARK(BM_push_range<wstring, vec_wstr>)->Apply(common_args);

BENCHMARK_MAIN();
