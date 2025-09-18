// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <benchmark/benchmark.h>
#include <cstdint>
#include <limits>
#include <ranges>

using namespace std;

struct Data {
    _Signed128 num;
    _Signed128 den;
};

constexpr Data d1{_Signed128{0x0000'1111'2222'3333LL, 0x4444'5555'6666'7777LL}, 0x7777'8888};
static_assert(d1.num > numeric_limits<uint64_t>::max());
static_assert(d1.den <= numeric_limits<uint32_t>::max());

constexpr Data d2{_Signed128{0x0000'1111'2222'3333LL, 0x4444'5555'6666'7777LL}, 0x7777'8888'9999'AAAA};
static_assert(d2.num > numeric_limits<uint64_t>::max());
static_assert(d2.den <= numeric_limits<uint64_t>::max());

constexpr Data d3{
    _Signed128{0x0000'1111'2222'3333LL, 0x4444'5555'6666'7777LL}, _Signed128{0x7777'8888'9999'AAAA, 0xBBBB}};
static_assert(d3.num > numeric_limits<uint64_t>::max());
static_assert(d3.den > numeric_limits<uint64_t>::max());

void bm(benchmark::State& state, const Data& data) {
    for (auto _ : state) {
        benchmark::DoNotOptimize(ranges::_Div_ceil(data.num, data.den));
    }
}

BENCHMARK_CAPTURE(bm, div_ceil_int128_uint32, d1);
BENCHMARK_CAPTURE(bm, div_ceil_int128_uint64, d2);
BENCHMARK_CAPTURE(bm, div_ceil_int128_int128, d3);

BENCHMARK_MAIN();
