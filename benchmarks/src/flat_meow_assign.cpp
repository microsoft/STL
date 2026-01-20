// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <benchmark/benchmark.h>
#include <flat_map>
#include <flat_set>
#include <string>

using namespace std;

void flat_map_strings(benchmark::State& state) {
    flat_map<string, int> pieces;
    for (auto _ : state) {
        pieces = {{"soldier"s, 1}, {"soldier"s, 2}, {"soldier"s, 3}, {"soldier"s, 4}, {"soldier"s, 5}, {"soldier"s, 6},
            {"soldier"s, 7}, {"soldier"s, 8}, {"tower"s, 9}, {"horse"s, 10}, {"elephant"s, 11}, {"vizier"s, 12},
            {"king"s, 13}, {"elephant"s, 14}, {"horse"s, 15}, {"tower"s, 16}};
        benchmark::DoNotOptimize(pieces);
    }
}

void flat_set_strings(benchmark::State& state) {
    flat_set<string> pieces;
    for (auto _ : state) {
        pieces = {"soldier"s, "soldier"s, "soldier"s, "soldier"s, "soldier"s, "soldier"s, "soldier"s, "soldier"s,
            "tower"s, "horse"s, "elephant"s, "vizier"s, "king"s, "elephant"s, "horse"s, "tower"s};
        benchmark::DoNotOptimize(pieces);
    }
}

void flat_map_integers(benchmark::State& state) {
    flat_map<int, int> pieces;
    for (auto _ : state) {
        pieces = {{'s', 1}, {'s', 2}, {'s', 3}, {'s', 4}, {'s', 5}, {'s', 6}, {'s', 7}, {'s', 8}, {'T', 9}, {'H', 10},
            {'E', 11}, {'V', 12}, {'K', 13}, {'E', 14}, {'H', 15}, {'T', 16}};
        benchmark::DoNotOptimize(pieces);
    }
}

void flat_set_integers(benchmark::State& state) {
    flat_set<int> pieces;
    for (auto _ : state) {
        pieces = {'s', 's', 's', 's', 's', 's', 's', 's', 'T', 'H', 'E', 'V', 'K', 'E', 'H', 'T'};
        benchmark::DoNotOptimize(pieces);
    }
}

BENCHMARK(flat_map_strings);
BENCHMARK(flat_set_strings);
BENCHMARK(flat_map_integers);
BENCHMARK(flat_set_integers);

BENCHMARK_MAIN();
