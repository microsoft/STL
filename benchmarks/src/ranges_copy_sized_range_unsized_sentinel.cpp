// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <benchmark/benchmark.h>

#include <algorithm>
#include <cstddef>
#include <ranges>
#include <string>
#include <vector>

using namespace std;

struct c_string_sentinel {
    inline friend bool operator==(const char* current, c_string_sentinel) noexcept {
        return *current == '\0';
    }
};

class sized_c_string_range {
public:
    inline sized_c_string_range(const char* data, const size_t length) noexcept : data_{data}, length_{length} {}

    inline const char* begin() const noexcept {
        return data_;
    }

    inline c_string_sentinel end() const noexcept {
        return {};
    }

    inline size_t size() const noexcept {
        return length_;
    }

private:
    const char* data_;
    size_t length_;
};

static_assert(ranges::random_access_range<sized_c_string_range>);
static_assert(ranges::sized_range<sized_c_string_range>);
static_assert(!ranges::common_range<sized_c_string_range>);
static_assert(!sized_sentinel_for<c_string_sentinel, const char*>);

void copy_sized_range_unsized_sentinel(benchmark::State& state) {
    const size_t size = static_cast<size_t>(state.range(0));
    string input(size, 'x');
    for (size_t i = 0; i != size; ++i) {
        input[i] = static_cast<char>('a' + (i % 26));
    }

    const sized_c_string_range source{input.data(), size};
    vector<char> output(size + 1);

    for (auto _ : state) {
        benchmark::DoNotOptimize(input.data());
        benchmark::DoNotOptimize(output.data());
        benchmark::DoNotOptimize(ranges::copy(source, output.data()));
        benchmark::ClobberMemory();
    }
}

BENCHMARK(copy_sized_range_unsized_sentinel)->Arg(0)->RangeMultiplier(8)->Range(1, 1 << 16);

BENCHMARK_MAIN();
