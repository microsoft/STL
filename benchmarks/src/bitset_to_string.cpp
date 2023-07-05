// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <array>
#include <benchmark/benchmark.h>
#include <bitset>
#include <random>

using namespace std;

namespace {
    auto random_bits = [] {
        mt19937_64 rnd(12345);
        array<uint64_t, 100> data;
        for (auto& d : data) {
            d = rnd();
        }
        return data;
    }();

    auto large_bitset = [] {
        constexpr size_t len = 2000;
        bitset<len> bs;
        mt19937_64 rnd(12345);
        for (int i = 0; i < len; i++) {
            bs[i] = rnd() & 1;
        }
        return bs;
    }();

    void BM_to_string_small(benchmark::State& state) {
        for (auto _ : state) {
            for (auto bits : random_bits) {
                std::bitset<15> bs{bits};
                benchmark::DoNotOptimize(bs.to_string());
            }
        }
    }
    void BM_to_string_medium(benchmark::State& state) {
        for (auto _ : state) {
            for (auto bits : random_bits) {
                std::bitset<64> bs{bits};
                benchmark::DoNotOptimize(bs.to_string());
            }
        }
    }
    void BM_to_string_large(benchmark::State& state) {
        for (auto _ : state) {
            benchmark::DoNotOptimize(large_bitset.to_string());
        }
    }
    void BM_to_wstring_small(benchmark::State& state) {
        for (auto _ : state) {
            for (auto bits : random_bits) {
                std::bitset<7> bs{bits};
                benchmark::DoNotOptimize(bs.to_string<wchar_t>());
            }
        }
    }
    void BM_to_wstring_medium(benchmark::State& state) {
        for (auto _ : state) {
            for (auto bits : random_bits) {
                std::bitset<64> bs{bits};
                benchmark::DoNotOptimize(bs.to_string<wchar_t>());
            }
        }
    }
    void BM_to_wstring_large(benchmark::State& state) {
        for (auto _ : state) {
            benchmark::DoNotOptimize(large_bitset.to_string<wchar_t>());
        }
    }
} // namespace

BENCHMARK(BM_to_string_small);
BENCHMARK(BM_to_string_medium);
BENCHMARK(BM_to_string_large);
BENCHMARK(BM_to_wstring_small);
BENCHMARK(BM_to_wstring_medium);
BENCHMARK(BM_to_wstring_large);

BENCHMARK_MAIN();
