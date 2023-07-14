// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <benchmark/benchmark.h>
#include <filesystem>
#include <string_view>

namespace {
    void BM_lexically_normal(benchmark::State& state) {
        using namespace std::literals;
        static constexpr std::wstring_view args[5]{
            LR"(X:DriveRelative)"sv,
            LR"(\\server\\\share)"sv,
            LR"(STL/.github/workflows/../..)"sv,
            LR"(C:\Program Files\Azure Data Studio\resources\app\extensions\bat\snippets\batchfile.code-snippets)"sv,
            LR"(/\server/\share/\a/\b/\c/\./\./\d/\../\../\../\../\../\../\../\other/x/y/z/.././..\meow.txt)"sv,
        };

        const auto index = state.range(0);
        const std::filesystem::path p(args[index]);
        for (auto _ : state) {
            benchmark::DoNotOptimize(p.lexically_normal());
        }
    }
} // namespace

BENCHMARK(BM_lexically_normal)->DenseRange(0, 4, 1);

BENCHMARK_MAIN();
