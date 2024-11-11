// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <benchmark/benchmark.h>
#include <filesystem>
#include <system_error>

void symlink_status(benchmark::State& state) {
    const auto path = std::filesystem::temp_directory_path();

    for (auto _ : state) {
        std::error_code ec;
        const auto status = std::filesystem::symlink_status(path, ec);
        benchmark::DoNotOptimize(status);
        benchmark::DoNotOptimize(ec);
    }
}

BENCHMARK(symlink_status);

BENCHMARK_MAIN();
