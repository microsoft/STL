// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstddef>
#include <cstdint>
#include <ranges>

enum class Op {
    FindSized,
    FindUnsized,
    Count,
};

using namespace std;

template <class T, size_t Size, size_t Pos, Op Operation>
void bm(benchmark::State& state) {
    T a[Size];

    fill_n(a, Size, T{'0'});
    if constexpr (Pos < Size) {
        a[Pos] = T{'1'};
    } else {
        static_assert(Operation != Op::FindUnsized);
    }

    for (auto _ : state) {
        if constexpr (Operation == Op::FindSized) {
            benchmark::DoNotOptimize(ranges::find(a, a + Size, T{'1'}));
        } else if constexpr (Operation == Op::FindUnsized) {
            benchmark::DoNotOptimize(ranges::find(a, unreachable_sentinel, T{'1'}));
        } else if constexpr (Operation == Op::Count) {
            benchmark::DoNotOptimize(ranges::count(a, a + Size, T{'1'}));
        }
    }
}

BENCHMARK(bm<uint8_t, 8021, 3056, Op::FindSized>);
BENCHMARK(bm<uint8_t, 8021, 3056, Op::FindUnsized>);
BENCHMARK(bm<uint8_t, 8021, 3056, Op::Count>);

BENCHMARK(bm<uint16_t, 8021, 3056, Op::FindSized>);
BENCHMARK(bm<uint16_t, 8021, 3056, Op::FindUnsized>);
BENCHMARK(bm<uint16_t, 8021, 3056, Op::Count>);

BENCHMARK(bm<uint32_t, 8021, 3056, Op::FindSized>);
BENCHMARK(bm<uint32_t, 8021, 3056, Op::FindUnsized>);
BENCHMARK(bm<uint32_t, 8021, 3056, Op::Count>);

BENCHMARK(bm<uint64_t, 8021, 3056, Op::FindSized>);
BENCHMARK(bm<uint64_t, 8021, 3056, Op::FindUnsized>);
BENCHMARK(bm<uint64_t, 8021, 3056, Op::Count>);

BENCHMARK(bm<int8_t, 8021, 3056, Op::FindSized>);
BENCHMARK(bm<int8_t, 8021, 3056, Op::FindUnsized>);
BENCHMARK(bm<int8_t, 8021, 3056, Op::Count>);

BENCHMARK(bm<int16_t, 8021, 3056, Op::FindSized>);
BENCHMARK(bm<int16_t, 8021, 3056, Op::FindUnsized>);
BENCHMARK(bm<int16_t, 8021, 3056, Op::Count>);

BENCHMARK(bm<int32_t, 8021, 3056, Op::FindSized>);
BENCHMARK(bm<int32_t, 8021, 3056, Op::FindUnsized>);
BENCHMARK(bm<int32_t, 8021, 3056, Op::Count>);

BENCHMARK(bm<int64_t, 8021, 3056, Op::FindSized>);
BENCHMARK(bm<int64_t, 8021, 3056, Op::FindUnsized>);
BENCHMARK(bm<int64_t, 8021, 3056, Op::Count>);

BENCHMARK_MAIN();
