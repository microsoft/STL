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
    small()                            = default;
    small(const small&)                = default;
    small& operator=(const small&)     = default;
    small(small&&) noexcept            = default;
    small& operator=(small&&) noexcept = default;
    ~small() {}
};
static_assert(!std::is_trivially_copyable_v<small>);
static_assert(std::is_nothrow_move_constructible_v<small>);

using large = std::array<int, 32>;

template <class T>
void bm(benchmark::State& state) {
    std::any a = T{};
    std::any b = T{};

    for (auto _ : state) {
        a.swap(b);
        benchmark::DoNotOptimize(a);
        benchmark::DoNotOptimize(b);
    }
}

BENCHMARK(bm<trivial>);
BENCHMARK(bm<small>);
BENCHMARK(bm<large>);

BENCHMARK_MAIN();
