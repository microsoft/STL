// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstring>
#include <vector>

#include <skewed_allocator.hpp>

template <typename Contained, Contained Value>
void handwritten_loop(benchmark::State& state) {
    const size_t r0 = static_cast<size_t>(state.range(0));
    std::vector<Contained, not_highly_aligned_allocator<Contained>> buffer(r0);
    for ([[maybe_unused]] auto _ : state) {
        benchmark::DoNotOptimize(buffer.data());
        Contained* ptr                 = buffer.data();
        const Contained* const ptr_end = ptr + r0;
        while (ptr != ptr_end) {
            *ptr++ = Value;
        }
        benchmark::DoNotOptimize(buffer.data());
    }
}

template <typename Contained, Contained Value>
void handwritten_loop_n(benchmark::State& state) {
    const size_t r0 = static_cast<size_t>(state.range(0));
    std::vector<Contained, not_highly_aligned_allocator<Contained>> buffer(r0);
    for ([[maybe_unused]] auto _ : state) {
        benchmark::DoNotOptimize(buffer.data());
        Contained* ptr = buffer.data();
        for (size_t idx = 0; idx < r0; ++idx) {
            ptr[idx] = Value;
        }
        benchmark::DoNotOptimize(buffer.data());
    }
}

// Ensure that Contained and Value are ok for std::memset.
template <typename Contained, Contained Value>
void memset_call(benchmark::State& state) {
    const size_t r0 = static_cast<size_t>(state.range(0));
    std::vector<Contained, not_highly_aligned_allocator<Contained>> buffer(r0);
    for ([[maybe_unused]] auto _ : state) {
        benchmark::DoNotOptimize(buffer.data());
        Contained* ptr = buffer.data();
        std::memset(ptr, Value, r0 * sizeof(Contained));
        benchmark::DoNotOptimize(buffer.data());
    }
}

template <typename Contained, Contained Value>
void std_fill_call(benchmark::State& state) {
    const size_t r0 = static_cast<size_t>(state.range(0));
    std::vector<Contained, not_highly_aligned_allocator<Contained>> buffer(r0);
    for ([[maybe_unused]] auto _ : state) {
        benchmark::DoNotOptimize(buffer.data());
        auto begin_it = buffer.data();
        auto end_it   = buffer.data() + r0;
        std::fill(begin_it, end_it, Value);
        benchmark::DoNotOptimize(buffer.data());
    }
}

template <typename Contained, Contained Value>
void std_fill_n_call(benchmark::State& state) {
    const size_t r0 = static_cast<size_t>(state.range(0));
    std::vector<Contained, not_highly_aligned_allocator<Contained>> buffer(r0);
    for ([[maybe_unused]] auto _ : state) {
        benchmark::DoNotOptimize(buffer.data());
        auto begin_it = buffer.data();
        std::fill_n(begin_it, r0, Value);
        benchmark::DoNotOptimize(buffer.data());
    }
}

BENCHMARK(handwritten_loop<char, 0>)->Range(0, 1 << 18);
BENCHMARK(handwritten_loop<char, 1>)->Range(0, 1 << 18);
BENCHMARK(handwritten_loop_n<char, 0>)->Range(0, 1 << 18);
BENCHMARK(handwritten_loop_n<char, 1>)->Range(0, 1 << 18);
BENCHMARK(memset_call<char, 0>)->Range(0, 1 << 18);
BENCHMARK(memset_call<char, 1>)->Range(0, 1 << 18);
BENCHMARK(std_fill_call<char, 0>)->Range(0, 1 << 18);
BENCHMARK(std_fill_call<char, 1>)->Range(0, 1 << 18);
BENCHMARK(std_fill_n_call<char, 0>)->Range(0, 1 << 18);
BENCHMARK(std_fill_n_call<char, 1>)->Range(0, 1 << 18);

BENCHMARK_MAIN();
