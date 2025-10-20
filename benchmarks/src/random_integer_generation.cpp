// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <benchmark/benchmark.h>
#include <cstdint>
#include <random>

/// Test URBGs alone

void BM_mt19937(benchmark::State& state) {
    std::mt19937 gen;
    for (auto _ : state) {
        benchmark::DoNotOptimize(gen());
    }
}
BENCHMARK(BM_mt19937);

void BM_mt19937_64(benchmark::State& state) {
    std::mt19937_64 gen;
    for (auto _ : state) {
        benchmark::DoNotOptimize(gen());
    }
}
BENCHMARK(BM_mt19937_64);

void BM_lcg(benchmark::State& state) {
    std::minstd_rand gen;
    for (auto _ : state) {
        benchmark::DoNotOptimize(gen());
    }
}
BENCHMARK(BM_lcg);

/// Test discard()

template <class Engine>
void BM_discard(benchmark::State& state) {
    Engine gen;
    const auto n = static_cast<unsigned long long>(state.range(0));
    for (auto _ : state) {
        gen.discard(n);
        benchmark::DoNotOptimize(gen());
    }
}
BENCHMARK(BM_discard<std::mt19937>)->Range(0, 1 << 18);
BENCHMARK(BM_discard<std::mt19937_64>)->Range(0, 1 << 18);
BENCHMARK(BM_discard<std::minstd_rand>)->Range(0, 1 << 18);

/// Support machinery for testing _Rng_from_urng_v2

std::uint32_t GetMax() {
    std::mt19937 gen;
    std::uniform_int_distribution<std::uint32_t> dist(10'000'000, 20'000'000);
    return dist(gen);
}

const std::uint32_t maximum = GetMax(); // random divisor to prevent strength reduction

/// Test mt19937

void BM_raw_mt19937_new(benchmark::State& state) {
    std::mt19937 gen;
    std::_Rng_from_urng_v2<std::uint32_t, decltype(gen)> rng(gen);
    for (auto _ : state) {
        benchmark::DoNotOptimize(rng(maximum));
    }
}
BENCHMARK(BM_raw_mt19937_new);

/// Test mt19937_64

void BM_raw_mt19937_64_new(benchmark::State& state) {
    std::mt19937_64 gen;
    std::_Rng_from_urng_v2<std::uint64_t, decltype(gen)> rng(gen);
    for (auto _ : state) {
        benchmark::DoNotOptimize(rng(maximum));
    }
}
BENCHMARK(BM_raw_mt19937_64_new);

/// Test minstd_rand

void BM_raw_lcg_new(benchmark::State& state) {
    std::minstd_rand gen;
    std::_Rng_from_urng_v2<std::uint32_t, decltype(gen)> rng(gen);
    for (auto _ : state) {
        benchmark::DoNotOptimize(rng(maximum));
    }
}
BENCHMARK(BM_raw_lcg_new);

BENCHMARK_MAIN();
