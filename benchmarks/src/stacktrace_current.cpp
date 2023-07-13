// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <benchmark/benchmark.h>
#include <stacktrace>

namespace {

    std::stacktrace get_current(int extra_depth) {
        if (extra_depth > 0) {
            return get_current(extra_depth - 1);
        } else {
            return std::stacktrace::current();
        }
    }

    void BM_stacktrace_current(benchmark::State& state) {
        int extra_depth = state.range(0);
        for (auto _ : state) {
            benchmark::DoNotOptimize(get_current(extra_depth));
        }
    }

} // namespace

// <NOTICE> this is subject to the final decision of _Try_frames. Currently: 100 < _Try_frames < 150
BENCHMARK(BM_stacktrace_current)->Arg(0)->Arg(50)->Arg(100)->Arg(150)->Arg(300);
BENCHMARK_MAIN();
