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

namespace {
    const auto random_bits = [] {
        mt19937_64 rnd{};
        array<uint64_t, 32> arr;
        for (auto& d : arr) {
            d = rnd();
        }
        return arr;
    }();

    template <size_t N, class charT>
    void BM_bitset_to_string(benchmark::State& state) {
        for (auto _ : state) {
            for (const auto& bits : random_bits) {
                bitset<N> bs{bits};
                benchmark::DoNotOptimize(bs.to_string<charT>());
            }
        }
    }

    template <class charT>
    void BM_bitset_to_string_large_single(benchmark::State& state) {
        const auto large_bitset = bit_cast<bitset<CHAR_BIT * sizeof(random_bits)>>(random_bits);
        for (auto _ : state) {
            benchmark::DoNotOptimize(large_bitset.to_string<charT>());
        }
    }
} // namespace

BENCHMARK(BM_bitset_to_string<15, char>);
BENCHMARK(BM_bitset_to_string<64, char>);
BENCHMARK(BM_bitset_to_string<512, char>);
BENCHMARK(BM_bitset_to_string_large_single<char>);
BENCHMARK(BM_bitset_to_string<7, wchar_t>);
BENCHMARK(BM_bitset_to_string<64, wchar_t>);
BENCHMARK(BM_bitset_to_string<512, wchar_t>);
BENCHMARK(BM_bitset_to_string_large_single<wchar_t>);

BENCHMARK_MAIN();
