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
#include "skewed_allocator.hpp"

using namespace std::string_view_literals;

template <size_t Size, bool Last_is_different>
constexpr auto make_fill_pattern_array() {
    std::array<char, Size> result;
    result.fill('*');

    if constexpr (Last_is_different) {
        result.back() = '!';
    }

    return result;
}

template <size_t Size, bool Last_is_different>
constexpr std::array fill_pattern_array = make_fill_pattern_array<Size, Last_is_different>();

template <size_t Size, bool Last_is_different>
constexpr std::string_view fill_pattern_view{
    fill_pattern_array<Size, Last_is_different>.data(), fill_pattern_array<Size, Last_is_different>.size()};

struct data_and_pattern {
    std::string_view data;
    std::string_view pattern;
};

constexpr data_and_pattern patterns[] = {
    /* 0. Small, closer to end   */ {lorem_ipsum, "aliquet"sv},
    /* 1. Large, closer to end   */ {lorem_ipsum, "aliquet malesuada"sv},
    /* 2. Small, closer to begin */ {lorem_ipsum, "pulvinar"sv},
    /* 3. Large, closer to begin */ {lorem_ipsum, "dapibus elit interdum"sv},

    /* 4. Small, evil */ {fill_pattern_view<3000, false>, fill_pattern_view<7, true>},
    /* 5. Large, evil */ {fill_pattern_view<3000, false>, fill_pattern_view<20, true>},
};

template <class T>
using not_highly_aligned_basic_string = std::basic_string<T, std::char_traits<T>, not_highly_aligned_allocator<T>>;

using not_highly_aligned_string    = not_highly_aligned_basic_string<char>;
using not_highly_aligned_wstring   = not_highly_aligned_basic_string<wchar_t>;
using not_highly_aligned_u32string = not_highly_aligned_basic_string<char32_t>;

void c_strstr(benchmark::State& state) {
    const auto& src_haystack = patterns[static_cast<size_t>(state.range())].data;
    const auto& src_needle   = patterns[static_cast<size_t>(state.range())].pattern;

    not_highly_aligned_string haystack(src_haystack);
    not_highly_aligned_string needle(src_needle);

    for (auto _ : state) {
        benchmark::DoNotOptimize(haystack);
        benchmark::DoNotOptimize(needle);
        auto res = std::strstr(haystack.c_str(), needle.c_str());
        benchmark::DoNotOptimize(res);
    }
}

template <class T>
void classic_search(benchmark::State& state) {
    const auto& src_haystack = patterns[static_cast<size_t>(state.range())].data;
    const auto& src_needle   = patterns[static_cast<size_t>(state.range())].pattern;

    std::vector<T, not_highly_aligned_allocator<T>> haystack(src_haystack.begin(), src_haystack.end());
    std::vector<T, not_highly_aligned_allocator<T>> needle(src_needle.begin(), src_needle.end());

    for (auto _ : state) {
        benchmark::DoNotOptimize(haystack);
        benchmark::DoNotOptimize(needle);
        auto res = std::search(haystack.begin(), haystack.end(), needle.begin(), needle.end());
        benchmark::DoNotOptimize(res);
    }
}

template <class T>
void ranges_search(benchmark::State& state) {
    const auto& src_haystack = patterns[static_cast<size_t>(state.range())].data;
    const auto& src_needle   = patterns[static_cast<size_t>(state.range())].pattern;

    std::vector<T, not_highly_aligned_allocator<T>> haystack(src_haystack.begin(), src_haystack.end());
    std::vector<T, not_highly_aligned_allocator<T>> needle(src_needle.begin(), src_needle.end());

    for (auto _ : state) {
        benchmark::DoNotOptimize(haystack);
        benchmark::DoNotOptimize(needle);
        auto res = std::ranges::search(haystack, needle);
        benchmark::DoNotOptimize(res);
    }
}

template <class T>
void search_default_searcher(benchmark::State& state) {
    const auto& src_haystack = patterns[static_cast<size_t>(state.range())].data;
    const auto& src_needle   = patterns[static_cast<size_t>(state.range())].pattern;

    std::vector<T, not_highly_aligned_allocator<T>> haystack(src_haystack.begin(), src_haystack.end());
    std::vector<T, not_highly_aligned_allocator<T>> needle(src_needle.begin(), src_needle.end());

    for (auto _ : state) {
        benchmark::DoNotOptimize(haystack);
        benchmark::DoNotOptimize(needle);
        auto res = std::search(haystack.begin(), haystack.end(), std::default_searcher{needle.begin(), needle.end()});
        benchmark::DoNotOptimize(res);
    }
}

template <class T>
void member_find(benchmark::State& state) {
    const auto& src_haystack = patterns[static_cast<size_t>(state.range())].data;
    const auto& src_needle   = patterns[static_cast<size_t>(state.range())].pattern;

    T haystack(src_haystack.begin(), src_haystack.end());
    T needle(src_needle.begin(), src_needle.end());

    for (auto _ : state) {
        benchmark::DoNotOptimize(haystack);
        benchmark::DoNotOptimize(needle);
        auto res = haystack.find(needle);
        benchmark::DoNotOptimize(res);
    }
}

template <class T>
void classic_find_end(benchmark::State& state) {
    const auto& src_haystack = patterns[static_cast<size_t>(state.range())].data;
    const auto& src_needle   = patterns[static_cast<size_t>(state.range())].pattern;

    std::vector<T, not_highly_aligned_allocator<T>> haystack(src_haystack.begin(), src_haystack.end());
    std::vector<T, not_highly_aligned_allocator<T>> needle(src_needle.begin(), src_needle.end());

    for (auto _ : state) {
        benchmark::DoNotOptimize(haystack);
        benchmark::DoNotOptimize(needle);
        auto res = std::find_end(haystack.begin(), haystack.end(), needle.begin(), needle.end());
        benchmark::DoNotOptimize(res);
    }
}

template <class T>
void ranges_find_end(benchmark::State& state) {
    const auto& src_haystack = patterns[static_cast<size_t>(state.range())].data;
    const auto& src_needle   = patterns[static_cast<size_t>(state.range())].pattern;

    std::vector<T, not_highly_aligned_allocator<T>> haystack(src_haystack.begin(), src_haystack.end());
    std::vector<T, not_highly_aligned_allocator<T>> needle(src_needle.begin(), src_needle.end());

    for (auto _ : state) {
        benchmark::DoNotOptimize(haystack);
        benchmark::DoNotOptimize(needle);
        auto res = std::ranges::find_end(haystack, needle);
        benchmark::DoNotOptimize(res);
    }
}

template <class T>
void member_rfind(benchmark::State& state) {
    const auto& src_haystack = patterns[static_cast<size_t>(state.range())].data;
    const auto& src_needle   = patterns[static_cast<size_t>(state.range())].pattern;

    T haystack(src_haystack.begin(), src_haystack.end());
    T needle(src_needle.begin(), src_needle.end());

    for (auto _ : state) {
        benchmark::DoNotOptimize(haystack);
        benchmark::DoNotOptimize(needle);
        auto res = haystack.rfind(needle);
        benchmark::DoNotOptimize(res);
    }
}

void common_args(auto bm) {
    bm->DenseRange(0, std::size(patterns) - 1, 1);
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

BENCHMARK(member_find<not_highly_aligned_string>)->Apply(common_args);
BENCHMARK(member_find<not_highly_aligned_wstring>)->Apply(common_args);
BENCHMARK(member_find<not_highly_aligned_u32string>)->Apply(common_args);

BENCHMARK(classic_find_end<std::uint8_t>)->Apply(common_args);
BENCHMARK(classic_find_end<std::uint16_t>)->Apply(common_args);
BENCHMARK(classic_find_end<std::uint32_t>)->Apply(common_args);
BENCHMARK(classic_find_end<std::uint64_t>)->Apply(common_args);

BENCHMARK(ranges_find_end<std::uint8_t>)->Apply(common_args);
BENCHMARK(ranges_find_end<std::uint16_t>)->Apply(common_args);
BENCHMARK(ranges_find_end<std::uint32_t>)->Apply(common_args);
BENCHMARK(ranges_find_end<std::uint64_t>)->Apply(common_args);

BENCHMARK(member_rfind<not_highly_aligned_string>)->Apply(common_args);
BENCHMARK(member_rfind<not_highly_aligned_wstring>)->Apply(common_args);
BENCHMARK(member_rfind<not_highly_aligned_u32string>)->Apply(common_args);

BENCHMARK_MAIN();
