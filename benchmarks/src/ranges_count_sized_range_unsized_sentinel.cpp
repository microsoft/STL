// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <benchmark/benchmark.h>

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <random>
#include <ranges>
#include <vector>

#include "skewed_allocator.hpp"

using namespace std;

using input_vector = vector<int, not_highly_aligned_allocator<int>>;

// no operator-; so this unsized sentinel can not be subtracted
struct unsized_sentinel {
    const int* last;

    friend bool operator==(const int* current, const unsized_sentinel sentinel) noexcept {
        return current == sentinel.last;
    }
};

// this range knows its length and can jump to any elements, but ranges::count can not get the distance to the end
// since the sentinel has no operator-
class sized_range_unsized_sentinel {
public:
    sized_range_unsized_sentinel(const int* const first, const size_t count) noexcept : first_{first}, count_{count} {}

    const int* begin() const noexcept {
        return first_;
    }

    unsized_sentinel end() const noexcept {
        return {first_ + count_};
    }

    size_t size() const noexcept {
        return count_;
    }

private:
    const int* first_;
    size_t count_;
};

static_assert(ranges::random_access_range<sized_range_unsized_sentinel>);
static_assert(ranges::sized_range<sized_range_unsized_sentinel>);
static_assert(!ranges::common_range<sized_range_unsized_sentinel>);
static_assert(!sized_sentinel_for<unsized_sentinel, const int*>);

input_vector make_input(const size_t size) {
    input_vector input(size);
    mt19937 gen(115'116'108);
    uniform_int_distribution<int> dist(0, 7);

    for (auto& value : input) {
        value = dist(gen);
    }

    return input;
}

void count_vector(benchmark::State& state) {
    const auto input = make_input(static_cast<size_t>(state.range(0)));

    for (auto _ : state) {
        benchmark::DoNotOptimize(input.data());
        benchmark::DoNotOptimize(ranges::count(input, 1));
    }
}

void count_sized_range_unsized_sentinel(benchmark::State& state) {
    const auto size  = static_cast<size_t>(state.range(0));
    const auto input = make_input(size);

    // make it so only the range type differs from count_vector
    const sized_range_unsized_sentinel source{input.data(), size};

    for (auto _ : state) {
        benchmark::DoNotOptimize(input.data());
        benchmark::DoNotOptimize(ranges::count(source, 1));
    }
}

void common_args(benchmark::Benchmark* const bm) {
    bm->Arg(7)->Arg(63)->Arg(512)->Arg(8021)->Arg(1 << 16)->Arg(1 << 20);
}

BENCHMARK(count_vector)->Apply(common_args);
BENCHMARK(count_sized_range_unsized_sentinel)->Apply(common_args);

BENCHMARK_MAIN();
