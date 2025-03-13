// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <type_traits>
#include <vector>

#include "skewed_allocator.hpp"

using namespace std;

enum class AlgType {
    std_func,
    str_member_first,
    str_member_last,
    str_member_first_not,
    str_member_last_not,
};

template <AlgType Alg, class T, T NeedleFillerBase = T{'a'}>
void bm(benchmark::State& state) {
    const size_t Pos   = static_cast<size_t>(state.range(0));
    const size_t NSize = static_cast<size_t>(state.range(1));
    const size_t HSize = Pos * 2;
    const size_t Which = 0;

    using container = conditional_t<Alg == AlgType::std_func, vector<T, not_highly_aligned_allocator<T>>,
        basic_string<T, char_traits<T>, not_highly_aligned_allocator<T>>>;

    constexpr size_t IncrementCap = 16;

    constexpr T HaystackFillerBase = T{' '};
    static_assert(
        NeedleFillerBase + IncrementCap <= HaystackFillerBase || HaystackFillerBase + IncrementCap <= NeedleFillerBase,
        "Would match where it shouldn't");

    container h(HSize, T{0});
    container n(NSize, T{0});

    for (size_t i = 0; i != NSize; ++i) {
        n[i] = NeedleFillerBase + i % IncrementCap;
    }

    if (Pos >= HSize || Which >= NSize) {
        abort();
    }

    if constexpr (Alg == AlgType::str_member_first_not || Alg == AlgType::str_member_last_not) {
        for (size_t i = 0; i != HSize; ++i) {
            h[i] = n[(i + Which) % NSize];
        }

        h[Pos] = HaystackFillerBase;
    } else {
        for (size_t i = 0; i != HSize; ++i) {
            h[i] = HaystackFillerBase + i % IncrementCap;
        }

        h[Pos] = n[Which];
    }

    for (auto _ : state) {
        benchmark::DoNotOptimize(h);
        benchmark::DoNotOptimize(n);
        if constexpr (Alg == AlgType::str_member_first) {
            benchmark::DoNotOptimize(h.find_first_of(n));
        } else if constexpr (Alg == AlgType::str_member_last) {
            benchmark::DoNotOptimize(h.find_last_of(n));
        } else if constexpr (Alg == AlgType::str_member_first_not) {
            benchmark::DoNotOptimize(h.find_first_not_of(n));
        } else if constexpr (Alg == AlgType::str_member_last_not) {
            benchmark::DoNotOptimize(h.find_last_not_of(n));
        } else {
            benchmark::DoNotOptimize(find_first_of(h.begin(), h.end(), n.begin(), n.end()));
        }
    }
}

void common_args(auto bm) {
    bm->Args({2, 3})->Args({6, 81})->Args({7, 4})->Args({9, 3})->Args({22, 5})->Args({58, 2});
    bm->Args({75, 85})->Args({102, 4})->Args({200, 46})->Args({325, 1})->Args({400, 50});
    bm->Args({1011, 11})->Args({1280, 46})->Args({1502, 23})->Args({2203, 54})->Args({3056, 7});
}

BENCHMARK(bm<AlgType::std_func, uint8_t>)->Apply(common_args);
BENCHMARK(bm<AlgType::std_func, uint16_t>)->Apply(common_args);
BENCHMARK(bm<AlgType::std_func, uint32_t>)->Apply(common_args);
BENCHMARK(bm<AlgType::std_func, uint64_t>)->Apply(common_args);

BENCHMARK(bm<AlgType::str_member_first, char>)->Apply(common_args);
BENCHMARK(bm<AlgType::str_member_first, wchar_t>)->Apply(common_args);
BENCHMARK(bm<AlgType::str_member_first, wchar_t, L'\x03B1'>)->Apply(common_args);
BENCHMARK(bm<AlgType::str_member_first, char32_t>)->Apply(common_args);
BENCHMARK(bm<AlgType::str_member_first, char32_t, U'\x03B1'>)->Apply(common_args);

BENCHMARK(bm<AlgType::str_member_last, char>)->Apply(common_args);
BENCHMARK(bm<AlgType::str_member_last, wchar_t>)->Apply(common_args);
BENCHMARK(bm<AlgType::str_member_last, wchar_t, L'\x03B1'>)->Apply(common_args);

BENCHMARK(bm<AlgType::str_member_first_not, char>)->Apply(common_args);
BENCHMARK(bm<AlgType::str_member_first_not, wchar_t>)->Apply(common_args);
BENCHMARK(bm<AlgType::str_member_first_not, wchar_t, L'\x03B1'>)->Apply(common_args);

BENCHMARK(bm<AlgType::str_member_last_not, char>)->Apply(common_args);
BENCHMARK(bm<AlgType::str_member_last_not, wchar_t>)->Apply(common_args);
BENCHMARK(bm<AlgType::str_member_last_not, wchar_t, L'\x03B1'>)->Apply(common_args);

BENCHMARK_MAIN();
