// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <benchmark/benchmark.h>
#include <regex>
#include <string>


using namespace std;
using namespace regex_constants;

void bm_match_sequence_of_as(benchmark::State& state, const char* pattern, syntax_option_type syntax = ECMAScript) {
    string input(static_cast<size_t>(state.range()), 'a');
    regex re{pattern, syntax};

    for (auto _ : state) {
        benchmark::DoNotOptimize(input);
        const char* pos = input.data();
        const char* end = input.data() + input.size();
        cmatch match;
        regex_match(pos, end, match, re);
    }
}

BENCHMARK_CAPTURE(bm_match_sequence_of_as, "a*", "a*")->Arg(100)->Arg(200)->Arg(400);
BENCHMARK_CAPTURE(bm_match_sequence_of_as, "a*?", "a*?")->Arg(100)->Arg(200)->Arg(400);
BENCHMARK_CAPTURE(bm_match_sequence_of_as, "(?:a)*", "(?:a)*")->Arg(100)->Arg(200)->Arg(400);
BENCHMARK_CAPTURE(bm_match_sequence_of_as, "(a)*", "(a)*")->Arg(100)->Arg(200)->Arg(400);
BENCHMARK_CAPTURE(bm_match_sequence_of_as, "(?:b|a)*", "(?:b|a)*")->Arg(100)->Arg(200)->Arg(400);
BENCHMARK_CAPTURE(bm_match_sequence_of_as, "(b|a)*", "(b|a)*")->Arg(100)->Arg(200)->Arg(400);
BENCHMARK_CAPTURE(bm_match_sequence_of_as, "(a)(?:b|a)*", "(a)(?:b|a)*")->Arg(100)->Arg(200)->Arg(400);
BENCHMARK_CAPTURE(bm_match_sequence_of_as, "(a)(b|a)*", "(a)(b|a)*")->Arg(100)->Arg(200)->Arg(400);
BENCHMARK_CAPTURE(bm_match_sequence_of_as, "(a)(?:b|a)*c", "(a)(?:b|a)*c")->Arg(100)->Arg(200)->Arg(400);

BENCHMARK_MAIN();
