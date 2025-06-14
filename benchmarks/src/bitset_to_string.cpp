// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <array>
#include <benchmark/benchmark.h>
#include <bit>
#include <bitset>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <random>

using namespace std;

template <size_t Elems>
auto random_bits_init() {
    mt19937_64 rnd{};
    array<uint64_t, Elems> arr;
    for (auto& d : arr) {
        d = rnd();
    }
    return arr;
}

template <size_t Elems = 32>
const auto random_bits = random_bits_init<Elems>();

template <size_t N, class charT>
void BM_bitset_to_string(benchmark::State& state) {
    static_assert(N <= 64);

    for (auto _ : state) {
        // make a copy, so that it can be potentially modified by DoNotOptimize
        for (auto bits : random_bits<>) {
            benchmark::DoNotOptimize(bits);
            bitset<N> bs{bits};
            benchmark::DoNotOptimize(bs.template to_string<charT>());
        }
    }
}

template <size_t N, class charT>
void BM_bitset_to_string_large_single(benchmark::State& state) {
    static_assert(N % 64 == 0 && N >= 64);
    const auto& bitset_data = random_bits<N / 64>;

    auto large_bitset = bit_cast<bitset<N>>(bitset_data);
    for (auto _ : state) {
        benchmark::DoNotOptimize(large_bitset);
        benchmark::DoNotOptimize(large_bitset.template to_string<charT>());
    }
}

BENCHMARK(BM_bitset_to_string<15, char>);
BENCHMARK(BM_bitset_to_string<64, char>);
BENCHMARK(BM_bitset_to_string_large_single<512, char>);
BENCHMARK(BM_bitset_to_string_large_single<2048, char>);
BENCHMARK(BM_bitset_to_string<7, wchar_t>);
BENCHMARK(BM_bitset_to_string<64, wchar_t>);
BENCHMARK(BM_bitset_to_string_large_single<512, wchar_t>);
BENCHMARK(BM_bitset_to_string_large_single<2048, wchar_t>);

BENCHMARK_MAIN();
