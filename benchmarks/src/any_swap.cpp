// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <any>
#include <array>
#include <benchmark/benchmark.h>
#include <type_traits>

using trivial = std::array<int, 2>;
static_assert(std::is_trivially_copyable_v<trivial>);

struct small {
    std::array<int, 4> c{};
    small()                                  = default;
    small(const small& other)                = default;
    small& operator=(const small& other)     = default;
    small(small&&) noexcept                  = default;
    small& operator=(small&& other) noexcept = default;
    ~small() {}
};
static_assert(!std::is_trivially_copyable_v<small>);
static_assert(std::is_nothrow_move_constructible_v<small>);

using large = std::array<int, 32>;

template <class A, class B>
void bm(benchmark::State& state) {
    std::any a = A{};
    std::any b = B{};

    for (auto _ : state) {
        a.swap(b);
        benchmark::DoNotOptimize(a);
        benchmark::DoNotOptimize(b);
    }
}

BENCHMARK(bm<trivial, trivial>);
BENCHMARK(bm<trivial, small>);
BENCHMARK(bm<trivial, large>);

BENCHMARK(bm<small, trivial>);
BENCHMARK(bm<small, small>);
BENCHMARK(bm<small, large>);

BENCHMARK(bm<large, trivial>);
BENCHMARK(bm<large, small>);
BENCHMARK(bm<large, large>);

BENCHMARK_MAIN();
