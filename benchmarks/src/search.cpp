// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <benchmark/benchmark.h>
#include <cstdint>
#include <cstring>
#include <functional>
#include <string>
#include <string_view>
#include <vector>

#include "lorem.hpp"
using namespace std::string_view_literals;


constexpr std::array patterns = {
    "aliquet"sv,
    "aliquet malesuada"sv,
};

void c_strstr(benchmark::State& state) {
    const auto& src_needle = patterns[static_cast<size_t>(state.range())];

    const std::string haystack(lorem_ipsum);
    const std::string needle(std::begin(src_needle), std::end(src_needle));

    for (auto _ : state) {
        benchmark::DoNotOptimize(haystack);
        benchmark::DoNotOptimize(needle);
        auto res = std::strstr(haystack.c_str(), needle.c_str());
        benchmark::DoNotOptimize(res);
    }
}

template <class T>
void classic_search(benchmark::State& state) {
    const auto& src_needle = patterns[static_cast<size_t>(state.range())];

    const std::vector<T> haystack(lorem_ipsum.begin(), lorem_ipsum.end());
    const std::vector<T> needle(std::begin(src_needle), std::end(src_needle));

    for (auto _ : state) {
        benchmark::DoNotOptimize(haystack);
        benchmark::DoNotOptimize(needle);
        auto res = std::search(haystack.begin(), haystack.end(), needle.begin(), needle.end());
        benchmark::DoNotOptimize(res);
    }
}

template <class T>
void ranges_search(benchmark::State& state) {
    const auto& src_needle = patterns[static_cast<size_t>(state.range())];

    const std::vector<T> haystack(lorem_ipsum.begin(), lorem_ipsum.end());
    const std::vector<T> needle(std::begin(src_needle), std::end(src_needle));

    for (auto _ : state) {
        benchmark::DoNotOptimize(haystack);
        benchmark::DoNotOptimize(needle);
        auto res = std::ranges::search(haystack, needle);
        benchmark::DoNotOptimize(res);
    }
}

template <class T>
void search_default_searcher(benchmark::State& state) {
    const auto& src_needle = patterns[static_cast<size_t>(state.range())];

    const std::vector<T> haystack(lorem_ipsum.begin(), lorem_ipsum.end());
    const std::vector<T> needle(std::begin(src_needle), std::end(src_needle));

    for (auto _ : state) {
        benchmark::DoNotOptimize(haystack);
        benchmark::DoNotOptimize(needle);
        auto res = std::search(haystack.begin(), haystack.end(), std::default_searcher{needle.begin(), needle.end()});
        benchmark::DoNotOptimize(res);
    }
}

void common_args(auto bm) {
    bm->Range(0, patterns.size() - 1);
}

BENCHMARK(c_strstr)->Apply(common_args);

BENCHMARK(classic_search<std::uint8_t>)->Apply(common_args);
BENCHMARK(classic_search<std::uint16_t>)->Apply(common_args);
BENCHMARK(classic_search<std::uint32_t>)->Apply(common_args);
BENCHMARK(classic_search<std::uint64_t>)->Apply(common_args);

BENCHMARK(ranges_search<std::uint8_t>)->Apply(common_args);
BENCHMARK(ranges_search<std::uint16_t>)->Apply(common_args);
BENCHMARK(ranges_search<std::uint32_t>)->Apply(common_args);
BENCHMARK(ranges_search<std::uint64_t>)->Apply(common_args);

BENCHMARK(search_default_searcher<std::uint8_t>)->Apply(common_args);
BENCHMARK(search_default_searcher<std::uint16_t>)->Apply(common_args);
BENCHMARK(search_default_searcher<std::uint32_t>)->Apply(common_args);
BENCHMARK(search_default_searcher<std::uint64_t>)->Apply(common_args);


BENCHMARK_MAIN();
