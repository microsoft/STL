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
using namespace std::string_view_literals;

constexpr std::string_view common_src_data =
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam mollis imperdiet massa, at dapibus elit interdum "
    "ac. In eget sollicitudin mi. Nam at tellus at sapien tincidunt sollicitudin vel non eros. Pellentesque nunc nunc, "
    "ullamcorper eu accumsan at, pulvinar non turpis. Quisque vel mauris pulvinar, pretium purus vel, ultricies erat. "
    "Curabitur a magna in ligula tristique ornare. Quisque commodo, massa viverra laoreet luctus, sem nisi aliquam "
    "velit, fermentum pulvinar velit leo eget justo. Suspendisse vel erat efficitur, pulvinar eros volutpat, vulputate "
    "ex. Phasellus non purus vel velit tristique tristique id at ligula. Quisque mollis sodales magna. Mauris et quam "
    "eu quam viverra tempus. Nullam tempus maximus porta. Nunc mattis eleifend fermentum. Nullam aliquam libero "
    "accumsan velit elementum, eu laoreet metus convallis. Donec pellentesque lacus ut iaculis iaculis. Curabitur orci "
    "elit, bibendum sit amet feugiat at, iaculis sit amet massa. Maecenas imperdiet lacus at vehicula iaculis. Donec "
    "volutpat nunc sit amet accumsan tempor. Quisque pretium vestibulum ultricies. Suspendisse potenti. Aenean at diam "
    "iaculis, condimentum felis venenatis, condimentum erat. Nam quis elit dui. Duis quis odio vitae metus hendrerit "
    "rhoncus ut et magna. Cras ac augue quis nibh pharetra sagittis. Donec ullamcorper vel eros semper pretium. Proin "
    "vel sollicitudin eros. Nulla sollicitudin mattis turpis id suscipit. Aliquam sed risus velit. Aliquam iaculis nec "
    "nibh ac egestas. Duis finibus semper est sed consequat. Sed in sapien quis nibh dignissim mattis. Vestibulum nec "
    "metus sodales, euismod mauris ac, sollicitudin libero. Maecenas non arcu ac velit ullamcorper fringilla et quis "
    "nulla. Curabitur posuere leo eget ipsum tincidunt dignissim. Cras ultricies suscipit neque, quis suscipit tortor "
    "venenatis non. Cras nisl mi, bibendum in vulputate quis, vestibulum ornare enim. Nunc hendrerit placerat dui, "
    "aliquam mollis sem convallis et. Integer vitae urna diam. Phasellus et imperdiet est. Maecenas auctor facilisis "
    "nibh non commodo. Suspendisse iaculis quam id bibendum feugiat. Pellentesque felis erat, egestas a libero ac, "
    "laoreet consectetur elit. Cras ut suscipit ex. Etiam gravida sem quis ex porta, eu lacinia tortor fermentum. "
    "Nulla consequat odio enim, sed condimentum est sagittis a. Quisque nec commodo tellus. Phasellus elementum "
    "feugiat dolor et feugiat. Praesent sed mattis tortor. In vitae sodales purus. Morbi accumsan, ligula et interdum "
    "lacinia, leo risus suscipit urna, non luctus mi justo eu ipsum. Curabitur venenatis pretium orci id porttitor. "
    "Quisque dapibus nisl sit amet elit lobortis sagittis. Orci varius natoque penatibus et magnis dis parturient "
    "montes, nascetur ridiculus mus. Mauris varius dui sit amet tortor facilisis vestibulum. Curabitur condimentum "
    "justo nec orci mattis auctor. Quisque aliquet condimentum arcu ac sollicitudin. Maecenas elit elit, condimentum "
    "vitae auctor a, cursus et sem. Cras vehicula ante in consequat fermentum. Praesent at massa nisi. Mauris pretium "
    "euismod eros, ut posuere ligula ullamcorper id. Nullam aliquet malesuada est at dignissim. Pellentesque finibus "
    "sagittis libero nec bibendum. Phasellus dolor ipsum, finibus quis turpis quis, mollis interdum felis.";

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
constexpr std::string_view fill_pattern_view = fill_pattern_array<Size, Last_is_different>;

struct data_and_pattern {
    std::string_view data;
    std::string_view pattern;
};

constexpr data_and_pattern patterns[] = {
    /* 0. Small, closer to end   */ {common_src_data, "aliquet"sv},
    /* 1. Large, closer to end   */ {common_src_data, "aliquet malesuada"sv},
    /* 2. Small, closer to begin */ {common_src_data, "pulvinar"sv},
    /* 3. Large, closer to begin */ {common_src_data, "dapibus elit interdum"sv},

    /* 4. Small, evil */ {fill_pattern_view<3000, false>, fill_pattern_view<7, true>},
    /* 5. Large, evil */ {fill_pattern_view<3000, false>, fill_pattern_view<20, true>},
};

void c_strstr(benchmark::State& state) {
    const auto& src_haystack = patterns[static_cast<size_t>(state.range())].data;
    const auto& src_needle   = patterns[static_cast<size_t>(state.range())].pattern;

    const std::string haystack(std::begin(src_haystack), std::end(src_haystack));
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
    const auto& src_haystack = patterns[static_cast<size_t>(state.range())].data;
    const auto& src_needle   = patterns[static_cast<size_t>(state.range())].pattern;

    const std::vector<T> haystack(std::begin(src_haystack), std::end(src_haystack));
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
    const auto& src_haystack = patterns[static_cast<size_t>(state.range())].data;
    const auto& src_needle   = patterns[static_cast<size_t>(state.range())].pattern;

    const std::vector<T> haystack(std::begin(src_haystack), std::end(src_haystack));
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
    const auto& src_haystack = patterns[static_cast<size_t>(state.range())].data;
    const auto& src_needle   = patterns[static_cast<size_t>(state.range())].pattern;

    const std::vector<T> haystack(std::begin(src_haystack), std::end(src_haystack));
    const std::vector<T> needle(std::begin(src_needle), std::end(src_needle));

    for (auto _ : state) {
        benchmark::DoNotOptimize(haystack);
        benchmark::DoNotOptimize(needle);
        auto res = std::search(haystack.begin(), haystack.end(), std::default_searcher{needle.begin(), needle.end()});
        benchmark::DoNotOptimize(res);
    }
}

template <class T>
void classic_find_end(benchmark::State& state) {
    const auto& src_haystack = patterns[static_cast<size_t>(state.range())].data;
    const auto& src_needle   = patterns[static_cast<size_t>(state.range())].pattern;

    const std::vector<T> haystack(std::begin(src_haystack), std::end(src_haystack));
    const std::vector<T> needle(std::begin(src_needle), std::end(src_needle));

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

    const std::vector<T> haystack(std::begin(src_haystack), std::end(src_haystack));
    const std::vector<T> needle(std::begin(src_needle), std::end(src_needle));

    for (auto _ : state) {
        benchmark::DoNotOptimize(haystack);
        benchmark::DoNotOptimize(needle);
        auto res = std::ranges::find_end(haystack, needle);
        benchmark::DoNotOptimize(res);
    }
}

void common_args(auto bm) {
    bm->DenseRange(0, std::size(patterns) - 1, 1);
}

BENCHMARK(c_strstr)->Apply(common_args);

BENCHMARK(classic_search<std::uint8_t>)->Apply(common_args);
BENCHMARK(classic_search<std::uint16_t>)->Apply(common_args);

BENCHMARK(ranges_search<std::uint8_t>)->Apply(common_args);
BENCHMARK(ranges_search<std::uint16_t>)->Apply(common_args);

BENCHMARK(search_default_searcher<std::uint8_t>)->Apply(common_args);
BENCHMARK(search_default_searcher<std::uint16_t>)->Apply(common_args);

BENCHMARK(classic_find_end<std::uint8_t>)->Apply(common_args);
BENCHMARK(classic_find_end<std::uint16_t>)->Apply(common_args);

BENCHMARK(ranges_find_end<std::uint8_t>)->Apply(common_args);
BENCHMARK(ranges_find_end<std::uint16_t>)->Apply(common_args);

BENCHMARK_MAIN();
