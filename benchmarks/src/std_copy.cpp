// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstddef>
#include <cstring>
#include <type_traits>
#include <vector>

#include <udt.hpp>
#include <utility.hpp>

namespace {

    template <typename Contained>
    void handwritten_loop(benchmark::State& state) {
        const size_t r0      = static_cast<size_t>(state.range(0));
        const auto in_buffer = random_vector<Contained>(r0);
        std::vector<Contained> out_buffer(r0);
        for ([[maybe_unused]] auto _ : state) {
            benchmark::DoNotOptimize(in_buffer.data());
            const Contained* in_ptr           = in_buffer.data();
            const Contained* const in_ptr_end = in_ptr + r0;
            Contained* out_ptr                = out_buffer.data();
            while (in_ptr != in_ptr_end) {
                *out_ptr++ = *in_ptr++;
            }

            benchmark::DoNotOptimize(out_buffer.data());
        }
    }

    template <typename Contained>
    void handwritten_loop_n(benchmark::State& state) {
        const size_t r0      = static_cast<size_t>(state.range(0));
        const auto in_buffer = random_vector<Contained>(r0);
        std::vector<Contained> out_buffer(r0);
        for ([[maybe_unused]] auto _ : state) {
            benchmark::DoNotOptimize(in_buffer.data());
            const Contained* const in_ptr = in_buffer.data();
            Contained* const out_ptr      = out_buffer.data();
            for (size_t idx = 0; idx < r0; ++idx) {
                out_ptr[idx] = in_ptr[idx];
            }

            benchmark::DoNotOptimize(out_buffer.data());
        }
    }

    template <typename Contained>
    void memcpy_call(benchmark::State& state) {
        static_assert(
            std::is_trivially_copyable_v<Contained>, "memcpy must only be called on trivially copyable types");
        const size_t r0      = static_cast<size_t>(state.range(0));
        const auto in_buffer = random_vector<Contained>(r0);
        std::vector<Contained> out_buffer(r0);
        for ([[maybe_unused]] auto _ : state) {
            benchmark::DoNotOptimize(in_buffer.data());
            memcpy(out_buffer.data(), in_buffer.data(), r0 * sizeof(Contained));
            benchmark::DoNotOptimize(out_buffer.data());
        }
    }

    template <typename Contained>
    void std_copy_call(benchmark::State& state) {
        const size_t r0      = static_cast<size_t>(state.range(0));
        const auto in_buffer = random_vector<Contained>(r0);
        std::vector<Contained> out_buffer(r0);
        for ([[maybe_unused]] auto _ : state) {
            benchmark::DoNotOptimize(in_buffer.data());
            std::copy(in_buffer.begin(), in_buffer.end(), out_buffer.begin());
            benchmark::DoNotOptimize(out_buffer.data());
        }
    }

    template <typename Contained>
    void std_copy_n_call(benchmark::State& state) {
        const size_t r0      = static_cast<size_t>(state.range(0));
        const auto in_buffer = random_vector<Contained>(r0);
        std::vector<Contained> out_buffer(r0);
        for ([[maybe_unused]] auto _ : state) {
            benchmark::DoNotOptimize(in_buffer.data());
            std::copy_n(in_buffer.begin(), r0, out_buffer.begin());
            benchmark::DoNotOptimize(out_buffer.data());
        }
    }
} // namespace

BENCHMARK_TEMPLATE1(handwritten_loop, char)->Range(0, 1 << 18);
BENCHMARK_TEMPLATE1(handwritten_loop_n, char)->Range(0, 1 << 18);
BENCHMARK_TEMPLATE1(memcpy_call, char)->Range(0, 1 << 18);
BENCHMARK_TEMPLATE1(std_copy_call, char)->Range(0, 1 << 18);
BENCHMARK_TEMPLATE1(std_copy_n_call, char)->Range(0, 1 << 18);

BENCHMARK_TEMPLATE1(handwritten_loop, aggregate<char>)->Range(0, 1 << 18);
BENCHMARK_TEMPLATE1(handwritten_loop_n, aggregate<char>)->Range(0, 1 << 18);
BENCHMARK_TEMPLATE1(memcpy_call, aggregate<char>)->Range(0, 1 << 18);
BENCHMARK_TEMPLATE1(std_copy_call, aggregate<char>)->Range(0, 1 << 18);
BENCHMARK_TEMPLATE1(std_copy_n_call, aggregate<char>)->Range(0, 1 << 18);

BENCHMARK_TEMPLATE1(handwritten_loop, non_trivial<char>)->Range(0, 1 << 18);
BENCHMARK_TEMPLATE1(handwritten_loop_n, non_trivial<char>)->Range(0, 1 << 18);
BENCHMARK_TEMPLATE1(std_copy_call, non_trivial<char>)->Range(0, 1 << 18);
BENCHMARK_TEMPLATE1(std_copy_n_call, non_trivial<char>)->Range(0, 1 << 18);

BENCHMARK_TEMPLATE1(handwritten_loop, int)->Range(0, 1 << 15);
BENCHMARK_TEMPLATE1(handwritten_loop_n, int)->Range(0, 1 << 15);
BENCHMARK_TEMPLATE1(memcpy_call, int)->Range(0, 1 << 15);
BENCHMARK_TEMPLATE1(std_copy_call, int)->Range(0, 1 << 15);
BENCHMARK_TEMPLATE1(std_copy_n_call, int)->Range(0, 1 << 15);

BENCHMARK_TEMPLATE1(handwritten_loop, aggregate<int>)->Range(0, 1 << 15);
BENCHMARK_TEMPLATE1(handwritten_loop_n, aggregate<int>)->Range(0, 1 << 15);
BENCHMARK_TEMPLATE1(memcpy_call, aggregate<int>)->Range(0, 1 << 15);
BENCHMARK_TEMPLATE1(std_copy_call, aggregate<int>)->Range(0, 1 << 15);
BENCHMARK_TEMPLATE1(std_copy_n_call, aggregate<int>)->Range(0, 1 << 15);

BENCHMARK_TEMPLATE1(handwritten_loop, non_trivial<int>)->Range(0, 1 << 15);
BENCHMARK_TEMPLATE1(handwritten_loop_n, non_trivial<int>)->Range(0, 1 << 15);
BENCHMARK_TEMPLATE1(std_copy_call, non_trivial<int>)->Range(0, 1 << 15);
BENCHMARK_TEMPLATE1(std_copy_n_call, non_trivial<int>)->Range(0, 1 << 15);

BENCHMARK_MAIN();
