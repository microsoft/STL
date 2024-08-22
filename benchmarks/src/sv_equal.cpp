// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstddef>
#include <ranges>
#include <string_view>


using namespace std::string_view_literals;

constexpr auto haystack =
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam mollis imperdiet massa, at dapibus elit "
    "interdumac. In eget sollicitudin mi. Nam at tellus at sapien tincidunt sollicitudin vel non eros. Pellentesque "
    "nunc nunc,ullamcorper eu accumsan at, pulvinar non turpis. Quisque vel mauris pulvinar, pretium purus vel, "
    "ultricies erat.Curabitur a magna in ligula tristique ornare. Quisque commodo, massa viverra laoreet luctus, sem "
    "nisi aliquamvelit, fermentum pulvinar velit leo eget justo. Suspendisse vel erat efficitur, pulvinar eros "
    "volutpat, vulputateex. Phasellus non purus vel velit tristique tristique id at ligula. Quisque mollis sodales "
    "magna. Mauris et quameu quam viverra tempus. Nullam tempus maximus porta. Nunc mattis eleifend fermentum. Nullam "
    "aliquam liberoaccumsan velit elementum, eu laoreet metus convallis. Donec pellentesque lacus ut iaculis iaculis. "
    "Curabitur orcielit, bibendum sit amet feugiat at, iaculis sit amet massa. Maecenas imperdiet lacus at vehicula "
    "iaculis. Donecvolutpat nunc sit amet accumsan tempor. Quisque pretium vestibulum ultricies. Suspendisse potenti. "
    "Aenean at diamiaculis, condimentum felis venenatis, condimentum erat. Nam quis elit dui. Duis quis odio vitae "
    "metus hendreritrhoncus ut et magna. Cras ac augue quis nibh pharetra sagittis. Donec ullamcorper vel eros semper "
    "pretium. Proinvel sollicitudin eros. Nulla sollicitudin mattis turpis id suscipit. Aliquam sed risus velit. "
    "Aliquam iaculis necnibh ac egestas. Duis finibus semper est sed consequat. Sed in sapien quis nibh dignissim "
    "mattis. Vestibulum necmetus sodales, euismod mauris ac, sollicitudin libero. Maecenas non arcu ac velit "
    "ullamcorper fringilla et quisnulla. Curabitur posuere leo eget ipsum tincidunt dignissim. Cras ultricies suscipit "
    "neque, quis suscipit tortorvenenatis non. Cras nisl mi, bibendum in vulputate quis, vestibulum ornare enim. Nunc "
    "hendrerit placerat dui,aliquam mollis sem convallis et. Integer vitae urna diam. Phasellus et imperdiet est. "
    "Maecenas auctor facilisisnibh non commodo. Suspendisse iaculis quam "sv;


constexpr std::size_t count = 8u;

template <std::size_t length>
inline constexpr std::array<std::string_view, count> make_svs() {

    std::array<std::string_view, count> result{};
    if constexpr (length == 0) {
        return result;
    }

    using namespace std::views;

    std::ranges::copy(haystack | chunk(length) | transform([](auto&& t) { return std::string_view(t); }) | take(count),
        result.begin());

    return result;
}

template <std::size_t length>
void equal(benchmark::State& state) {
    constexpr auto arr = make_svs<length>();
    benchmark::DoNotOptimize(arr);

    for (auto _ : state) {
        for (auto& i : arr) {
            benchmark::DoNotOptimize(i);
            auto res = (i == arr[0]);
            benchmark::DoNotOptimize(res);
        }
    }
}

BENCHMARK(equal<0>);
BENCHMARK(equal<8>);
BENCHMARK(equal<16>);
BENCHMARK(equal<32>);
BENCHMARK(equal<64>);
BENCHMARK(equal<128>);
BENCHMARK(equal<256>);

BENCHMARK_MAIN();
