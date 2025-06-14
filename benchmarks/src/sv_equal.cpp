// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <benchmark/benchmark.h>
#include <cstddef>
#include <ranges>
#include <string_view>

#include "lorem.hpp"

constexpr std::string_view haystack = lorem_ipsum.substr(0, 2048);

constexpr std::size_t Count = 8u;

template <std::size_t Length>
constexpr std::array<std::string_view, Count> make_svs() {
    std::array<std::string_view, Count> result{};

    if constexpr (Length != 0) {
        using namespace std::views;

        std::ranges::copy(
            haystack | chunk(Length) | transform([](auto&& t) { return std::string_view(t); }) | take(Count),
            result.begin());
    }

    return result;
}

template <std::size_t Length>
void sv_equal(benchmark::State& state) {
    auto arr = make_svs<Length>();
    benchmark::DoNotOptimize(arr);

    for (auto _ : state) {
        for (auto& i : arr) {
            benchmark::DoNotOptimize(i);
            auto res = (i == arr[0]);
            benchmark::DoNotOptimize(res);
        }
    }
}

BENCHMARK(sv_equal<0>);
BENCHMARK(sv_equal<8>);
BENCHMARK(sv_equal<16>);
BENCHMARK(sv_equal<32>);
BENCHMARK(sv_equal<64>);
BENCHMARK(sv_equal<128>);
BENCHMARK(sv_equal<256>);

static_assert(haystack.size() >= Count * 256, "haystack is too small");

BENCHMARK_MAIN();
