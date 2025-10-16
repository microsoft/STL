// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <benchmark/benchmark.h>
#include <regex>
#include <string>

#include "lorem.hpp"

using namespace std;
using namespace regex_constants;

void bm_lorem_search(benchmark::State& state, const char* pattern, syntax_option_type syntax = ECMAScript) {
    string repeated_lorem{lorem_ipsum};
    for (long long i = 0; i < state.range(); ++i) {
        repeated_lorem += repeated_lorem;
    }
    regex re{pattern, syntax};

    for (auto _ : state) {
        benchmark::DoNotOptimize(repeated_lorem);
        const char* pos = repeated_lorem.data();
        const char* end = repeated_lorem.data() + repeated_lorem.size();
        cmatch match;
        for (; regex_search(pos, end, match, re); ++pos) {
            benchmark::DoNotOptimize(match);
            pos = match[0].second;
            if (pos == end) {
                break;
            }
        }
    }
}

BENCHMARK_CAPTURE(bm_lorem_search, "^bibe", "^bibe")->Arg(2)->Arg(3)->Arg(4);
BENCHMARK_CAPTURE(bm_lorem_search, "bibe", "bibe")->Arg(2)->Arg(3)->Arg(4);
BENCHMARK_CAPTURE(bm_lorem_search, "bibe".collate, "bibe", regex_constants::collate)->Arg(2)->Arg(3)->Arg(4);
BENCHMARK_CAPTURE(bm_lorem_search, "(bibe)", "(bibe)")->Arg(2)->Arg(3)->Arg(4);
BENCHMARK_CAPTURE(bm_lorem_search, "(bibe)+", "(bibe)+")->Arg(2)->Arg(3)->Arg(4);
BENCHMARK_CAPTURE(bm_lorem_search, "(?:bibe)+", "(?:bibe)+")->Arg(2)->Arg(3)->Arg(4);
BENCHMARK_CAPTURE(bm_lorem_search, R"(\bbibe)", R"(\bbibe)")->Arg(2)->Arg(3)->Arg(4);
BENCHMARK_CAPTURE(bm_lorem_search, R"(\Bibe)", R"(\Bibe)")->Arg(2)->Arg(3)->Arg(4);
BENCHMARK_CAPTURE(bm_lorem_search, R"((?=....)bibe)", R"((?=....)bibe)")->Arg(2)->Arg(3)->Arg(4);
BENCHMARK_CAPTURE(bm_lorem_search, R"((?=bibe)....)", R"((?=bibe)....)")->Arg(2)->Arg(3)->Arg(4);
BENCHMARK_CAPTURE(bm_lorem_search, R"((?!lorem)bibe)", R"((?!lorem)bibe)")->Arg(2)->Arg(3)->Arg(4);

BENCHMARK_MAIN();
