#include <benchmark/benchmark.h>
#include <random>

/// Test URBGs alone

static void BM_mt19937(benchmark::State& state) {
    std::mt19937 gen;
    for (auto _ : state) {
        benchmark::DoNotOptimize(gen());
    }
}
BENCHMARK(BM_mt19937);

static void BM_mt19937_64(benchmark::State& state) {
    std::mt19937_64 gen;
    for (auto _ : state) {
        benchmark::DoNotOptimize(gen());
    }
}
BENCHMARK(BM_mt19937_64);

static void BM_lcg(benchmark::State& state) {
    std::minstd_rand gen;
    for (auto _ : state) {
        benchmark::DoNotOptimize(gen());
    }
}
BENCHMARK(BM_lcg);

uint32_t GetMax() {
    std::random_device gen;
    std::uniform_int_distribution<uint32_t> dist(10'000'000, 20'000'000);
    return dist(gen);
}

static const uint32_t max = GetMax(); // random divisor to prevent strength reduction

/// Test mt19937

static void BM_raw_mt19937_old(benchmark::State& state) {
    std::mt19937 gen;
    std::_Rng_from_urng<uint32_t, decltype(gen)> rng(gen);
    for (auto _ : state) {
        benchmark::DoNotOptimize(rng(max));
    }
}
BENCHMARK(BM_raw_mt19937_old);

static void BM_raw_mt19937_new(benchmark::State& state) {
    std::mt19937 gen;
    std::_Rng_from_urng_v2<uint32_t, decltype(gen)> rng(gen);
    for (auto _ : state) {
        benchmark::DoNotOptimize(rng(max));
    }
}
BENCHMARK(BM_raw_mt19937_new);

/// Test mt19937_64

static void BM_raw_mt19937_64_old(benchmark::State& state) {
    std::mt19937_64 gen;
    std::_Rng_from_urng<uint64_t, decltype(gen)> rng(gen);
    for (auto _ : state) {
        benchmark::DoNotOptimize(rng(max));
    }
}
BENCHMARK(BM_raw_mt19937_64_old);

static void BM_raw_mt19937_64_new(benchmark::State& state) {
    std::mt19937_64 gen;
    std::_Rng_from_urng_v2<uint64_t, decltype(gen)> rng(gen);
    for (auto _ : state) {
        benchmark::DoNotOptimize(rng(max));
    }
}
BENCHMARK(BM_raw_mt19937_64_new);

/// Test minstd_rand

static void BM_raw_lcg_old(benchmark::State& state) {
    std::minstd_rand gen;
    std::_Rng_from_urng<uint32_t, decltype(gen)> rng(gen);
    for (auto _ : state) {
        benchmark::DoNotOptimize(rng(max));
    }
}
BENCHMARK(BM_raw_lcg_old);

static void BM_raw_lcg_new(benchmark::State& state) {
    std::minstd_rand gen;
    std::_Rng_from_urng_v2<uint32_t, decltype(gen)> rng(gen);
    for (auto _ : state) {
        benchmark::DoNotOptimize(rng(max));
    }
}
BENCHMARK(BM_raw_lcg_new);

BENCHMARK_MAIN();
