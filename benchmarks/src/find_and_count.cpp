// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <ranges>
#include <string>
#include <type_traits>
#include <vector>

#include "skewed_allocator.hpp"

enum class Op {
    FindSized,
    FindUnsized,
    Count,
    StringFind,
    StringRFind,
    StringFindNotFirstOne,
    StringFindNotLastOne,
};

using namespace std;

template <class T, template <class> class Alloc, Op Operation>
void bm(benchmark::State& state) {
    const auto size = static_cast<size_t>(state.range(0));
    const auto pos  = static_cast<size_t>(state.range(1));

    using Container =
        conditional_t<Operation >= Op::StringFind, basic_string<T, char_traits<T>, Alloc<T>>, vector<T, Alloc<T>>>;

    Container a(size, T{'0'});

    if (pos < size) {
        if constexpr (Operation == Op::StringRFind || Operation == Op::StringFindNotLastOne) {
            a[size - pos - 1] = T{'1'};
        } else {
            a[pos] = T{'1'};
        }
    } else {
        if constexpr (Operation == Op::FindUnsized) {
            abort();
        }
    }

    for (auto _ : state) {
        if constexpr (Operation == Op::FindSized) {
            benchmark::DoNotOptimize(ranges::find(a.begin(), a.end(), T{'1'}));
        } else if constexpr (Operation == Op::FindUnsized) {
            benchmark::DoNotOptimize(ranges::find(a.begin(), unreachable_sentinel, T{'1'}));
        } else if constexpr (Operation == Op::Count) {
            benchmark::DoNotOptimize(ranges::count(a.begin(), a.end(), T{'1'}));
        } else if constexpr (Operation == Op::StringFind) {
            benchmark::DoNotOptimize(a.find(T{'1'}));
        } else if constexpr (Operation == Op::StringRFind) {
            benchmark::DoNotOptimize(a.rfind(T{'1'}));
        } else if constexpr (Operation == Op::StringFindNotFirstOne) {
            benchmark::DoNotOptimize(a.find_first_not_of(T{'0'}));
        } else if constexpr (Operation == Op::StringFindNotLastOne) {
            benchmark::DoNotOptimize(a.find_last_not_of(T{'0'}));
        }
    }
}

void common_args(auto bm) {
    bm->Args({8021, 3056});
    // AVX tail tests
    bm->Args({63, 62})->Args({31, 30})->Args({15, 14})->Args({7, 6});
}

BENCHMARK(bm<uint8_t, not_highly_aligned_allocator, Op::FindSized>)->Apply(common_args);
BENCHMARK(bm<uint8_t, highly_aligned_allocator, Op::FindSized>)->Apply(common_args);
BENCHMARK(bm<uint8_t, not_highly_aligned_allocator, Op::FindUnsized>)->Apply(common_args);
BENCHMARK(bm<uint8_t, highly_aligned_allocator, Op::FindUnsized>)->Apply(common_args);
BENCHMARK(bm<uint8_t, not_highly_aligned_allocator, Op::Count>)->Apply(common_args);
BENCHMARK(bm<char, not_highly_aligned_allocator, Op::StringFind>)->Apply(common_args);
BENCHMARK(bm<char, not_highly_aligned_allocator, Op::StringRFind>)->Apply(common_args);
BENCHMARK(bm<char, not_highly_aligned_allocator, Op::StringFindNotFirstOne>)->Apply(common_args);
BENCHMARK(bm<char, not_highly_aligned_allocator, Op::StringFindNotLastOne>)->Apply(common_args);

BENCHMARK(bm<uint16_t, not_highly_aligned_allocator, Op::FindSized>)->Apply(common_args);
BENCHMARK(bm<uint16_t, not_highly_aligned_allocator, Op::Count>)->Apply(common_args);
BENCHMARK(bm<wchar_t, not_highly_aligned_allocator, Op::StringFind>)->Apply(common_args);
BENCHMARK(bm<wchar_t, not_highly_aligned_allocator, Op::StringRFind>)->Apply(common_args);
BENCHMARK(bm<wchar_t, not_highly_aligned_allocator, Op::StringFindNotFirstOne>)->Apply(common_args);
BENCHMARK(bm<wchar_t, not_highly_aligned_allocator, Op::StringFindNotLastOne>)->Apply(common_args);

BENCHMARK(bm<uint32_t, not_highly_aligned_allocator, Op::FindSized>)->Apply(common_args);
BENCHMARK(bm<uint32_t, not_highly_aligned_allocator, Op::Count>)->Apply(common_args);
BENCHMARK(bm<char32_t, not_highly_aligned_allocator, Op::StringFind>)->Apply(common_args);
BENCHMARK(bm<char32_t, not_highly_aligned_allocator, Op::StringRFind>)->Apply(common_args);
BENCHMARK(bm<char32_t, not_highly_aligned_allocator, Op::StringFindNotFirstOne>)->Apply(common_args);
BENCHMARK(bm<char32_t, not_highly_aligned_allocator, Op::StringFindNotLastOne>)->Apply(common_args);

BENCHMARK(bm<uint64_t, not_highly_aligned_allocator, Op::FindSized>)->Apply(common_args);
BENCHMARK(bm<uint64_t, not_highly_aligned_allocator, Op::Count>)->Apply(common_args);

BENCHMARK_MAIN();
