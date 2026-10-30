// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <benchmark/benchmark.h>

#include <array>
#include <compare>
#include <cstddef>
#include <cstdint>
#include <cstring>

using namespace std;

enum class comparison {
    spaceship_less,
    memcmp_less,
    spaceship_equal,
    memcmp_equal,
};

enum class scenario {
    equal,
    different_first,
    different_last,
};

template <size_t Size, comparison Comparison, scenario Scenario>
void bm(benchmark::State& state) {
    array<uint8_t, Size> left{};
    array<uint8_t, Size> right{};

    for (size_t index = 0; index < Size; ++index) {
        left[index]  = static_cast<uint8_t>(index * 13 + 17);
        right[index] = left[index];
    }

    if constexpr (Scenario == scenario::different_first) {
        left.front()  = 0x40;
        right.front() = 0x80;
    } else if constexpr (Scenario == scenario::different_last) {
        left.back()  = 0x40;
        right.back() = 0x80;
    }

    benchmark::DoNotOptimize(left);
    benchmark::DoNotOptimize(right);

    for (auto _ : state) {
        if constexpr (Comparison == comparison::spaceship_less) {
            benchmark::DoNotOptimize((left <=> right) < 0);
        } else if constexpr (Comparison == comparison::memcmp_less) {
            benchmark::DoNotOptimize(memcmp(left.data(), right.data(), Size) < 0);
        } else if constexpr (Comparison == comparison::spaceship_equal) {
            benchmark::DoNotOptimize((left <=> right) == 0);
        } else {
            benchmark::DoNotOptimize(memcmp(left.data(), right.data(), Size) == 0);
        }
    }
}

#define BENCHMARK_SCENARIOS(Size, Comparison)                               \
    BENCHMARK(bm<Size, comparison::Comparison, scenario::equal>);           \
    BENCHMARK(bm<Size, comparison::Comparison, scenario::different_first>); \
    BENCHMARK(bm<Size, comparison::Comparison, scenario::different_last>)

#define BENCHMARK_COMPARISONS(Size)             \
    BENCHMARK_SCENARIOS(Size, spaceship_less);  \
    BENCHMARK_SCENARIOS(Size, memcmp_less);     \
    BENCHMARK_SCENARIOS(Size, spaceship_equal); \
    BENCHMARK_SCENARIOS(Size, memcmp_equal)

BENCHMARK_COMPARISONS(1);
BENCHMARK_COMPARISONS(2);
BENCHMARK_COMPARISONS(3);
BENCHMARK_COMPARISONS(4);
BENCHMARK_COMPARISONS(7);
BENCHMARK_COMPARISONS(8);
BENCHMARK_COMPARISONS(15);
BENCHMARK_COMPARISONS(16);
BENCHMARK_COMPARISONS(31);
BENCHMARK_COMPARISONS(32);
BENCHMARK_COMPARISONS(63);
BENCHMARK_COMPARISONS(64);
BENCHMARK_COMPARISONS(127);
BENCHMARK_COMPARISONS(128);
BENCHMARK_COMPARISONS(255);
BENCHMARK_COMPARISONS(256);

BENCHMARK_MAIN();
