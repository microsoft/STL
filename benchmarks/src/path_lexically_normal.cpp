// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <benchmark/benchmark.h>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

namespace {
    void BM_lexically_normal_0(benchmark::State& state) {
        const fs::path& p(LR"(X:DriveRelative)"sv);
        for (auto _ : state) {
            benchmark::DoNotOptimize(p.lexically_normal());
        }
    }
    void BM_lexically_normal_1(benchmark::State& state) {
        const fs::path& p(LR"(\\server\\\share)"sv);
        for (auto _ : state) {
            benchmark::DoNotOptimize(p.lexically_normal());
        }
    }
    void BM_lexically_normal_2(benchmark::State& state) {
        const fs::path& p(LR"(STL/.github/workflows/../..)"sv);
        for (auto _ : state) {
            benchmark::DoNotOptimize(p.lexically_normal());
        }
    }
    void BM_lexically_normal_3(benchmark::State& state) {
        const fs::path& p(
            LR"(C:\Program Files\Azure Data Studio\resources\app\extensions\bat\snippets\batchfile.code-snippets)"sv);
        for (auto _ : state) {
            benchmark::DoNotOptimize(p.lexically_normal());
        }
    }
    void BM_lexically_normal_4(benchmark::State& state) {
        const fs::path& p(
            LR"(/\server/\share/\a/\b/\c/\./\./\d/\../\../\../\../\../\../\../\other/x/y/z/.././..\meow.txt)"sv);
        for (auto _ : state) {
            benchmark::DoNotOptimize(p.lexically_normal());
        }
    }
} // namespace

BENCHMARK(BM_lexically_normal_0);
BENCHMARK(BM_lexically_normal_1);
BENCHMARK(BM_lexically_normal_2);
BENCHMARK(BM_lexically_normal_3);
BENCHMARK(BM_lexically_normal_4);

BENCHMARK_MAIN();
