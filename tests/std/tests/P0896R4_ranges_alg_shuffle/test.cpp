// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <cstdint>
#include <cstdio>
#include <numeric>
#include <random>
#include <ranges>
#include <utility>
#include <vector>

#include <range_algorithm_support.hpp>
using namespace std;

const unsigned int seed = random_device{}();
mt19937 gen{seed};
mt19937_64 gen64{seed}; // 64-bit generator for batched random path

// Validate dangling story
static_assert(same_as<decltype(ranges::shuffle(borrowed<false>{}, gen)), ranges::dangling>);
static_assert(same_as<decltype(ranges::shuffle(borrowed<true>{}, gen)), int*>);

struct instantiator {
    static constexpr int expected[7] = {1, 2, 3, 4, 5, 6, 7};

    template <ranges::random_access_range ReadWrite>
    static void call() {
        using ranges::shuffle, ranges::equal, ranges::is_sorted, ranges::iterator_t;

        { // Validate iterator + sentinel overload
            int input[7] = {1, 2, 3, 4, 5, 6, 7};
            ReadWrite wrapped_input{input};

            auto result = shuffle(wrapped_input.begin(), wrapped_input.end(), gen);
            static_assert(same_as<decltype(result), iterator_t<ReadWrite>>);
            assert(result == wrapped_input.end());

            sort(begin(input), end(input));
            assert(equal(input, expected));
        }
        { // Validate range overload
            int input[7] = {1, 2, 3, 4, 5, 6, 7};
            ReadWrite wrapped_input{input};

            auto result = shuffle(wrapped_input, gen);
            static_assert(same_as<decltype(result), iterator_t<ReadWrite>>);
            assert(result == wrapped_input.end());

            sort(begin(input), end(input));
            assert(equal(input, expected));
        }
    }
};

void test_urbg() { // COMPILE-ONLY
    struct RandGen {
        using result_type = uint16_t; // N5014 [rand.req.urng]/3
        static constexpr result_type min() {
            return 3;
        }
        static constexpr result_type max() {
            return 1729;
        }
        result_type operator()() & {
            return 4;
        }
    };

    static_assert(uniform_random_bit_generator<RandGen>);

    int arr[1] = {};
    ranges::shuffle(arr, RandGen{});
}

// Test that shuffle produces a valid permutation for various sizes.
// This exercises both the batched path (for 64-bit RNGs) and the fallback path.
void test_shuffle_permutation() {
    // Test with 64-bit generator (batched random path)
    {
        vector<int> v(100);
        iota(v.begin(), v.end(), 0);
        vector<int> original = v;

        shuffle(v.begin(), v.end(), gen64);

        // Verify it's still a permutation
        vector<int> sorted_v = v;
        sort(sorted_v.begin(), sorted_v.end());
        assert(sorted_v == original);
    }

    // Test with ranges::shuffle and 64-bit generator
    {
        vector<int> v(100);
        iota(v.begin(), v.end(), 0);
        vector<int> original = v;

        ranges::shuffle(v, gen64);

        // Verify it's still a permutation
        vector<int> sorted_v = v;
        sort(sorted_v.begin(), sorted_v.end());
        assert(sorted_v == original);
    }

    // Test with 32-bit generator (non-batched path)
    {
        vector<int> v(100);
        iota(v.begin(), v.end(), 0);
        vector<int> original = v;

        shuffle(v.begin(), v.end(), gen);

        // Verify it's still a permutation
        vector<int> sorted_v = v;
        sort(sorted_v.begin(), sorted_v.end());
        assert(sorted_v == original);
    }

    // Test with ranges::shuffle and 32-bit generator
    {
        vector<int> v(100);
        iota(v.begin(), v.end(), 0);
        vector<int> original = v;

        ranges::shuffle(v, gen);

        // Verify it's still a permutation
        vector<int> sorted_v = v;
        sort(sorted_v.begin(), sorted_v.end());
        assert(sorted_v == original);
    }
}

// Test edge cases for shuffle
void test_shuffle_edge_cases() {
    // Empty range
    {
        vector<int> v;
        shuffle(v.begin(), v.end(), gen64);
        assert(v.empty());
    }

    // Single element
    {
        vector<int> v = {42};
        shuffle(v.begin(), v.end(), gen64);
        assert(v.size() == 1);
        assert(v[0] == 42);
    }

    // Two elements
    {
        vector<int> v        = {1, 2};
        vector<int> original = v;
        shuffle(v.begin(), v.end(), gen64);
        sort(v.begin(), v.end());
        assert(v == original);
    }

    // Three elements (odd count, tests batching boundary)
    {
        vector<int> v        = {1, 2, 3};
        vector<int> original = v;
        shuffle(v.begin(), v.end(), gen64);
        sort(v.begin(), v.end());
        assert(v == original);
    }

    // Four elements (even count)
    {
        vector<int> v        = {1, 2, 3, 4};
        vector<int> original = v;
        shuffle(v.begin(), v.end(), gen64);
        sort(v.begin(), v.end());
        assert(v == original);
    }

    // Large array to ensure batching is effective
    {
        vector<int> v(10000);
        iota(v.begin(), v.end(), 0);
        vector<int> original = v;
        shuffle(v.begin(), v.end(), gen64);
        sort(v.begin(), v.end());
        assert(v == original);
    }
}

// Regression test: consecutive shuffles with the same URNG must produce different results.
// The batched random implementation had a bug where _Unsigned128 brace-initialization zeroed the high word
// of the multiplication result, making all random indices 0 and the shuffle deterministic.
void test_gh_6112() {
    // Test std::shuffle with mt19937_64 (exercises the batched random path)
    {
        mt19937_64 urng(6);
        vector<int> v(100);

        iota(v.begin(), v.end(), 0);
        shuffle(v.begin(), v.end(), urng);
        const auto first_shuffle = v;

        iota(v.begin(), v.end(), 0);
        shuffle(v.begin(), v.end(), urng);

        assert(v != first_shuffle); // "should be vanishingly impossible" for these to be equal
    }

    // Test ranges::shuffle with mt19937_64
    {
        mt19937_64 urng(6);
        vector<int> v(100);

        iota(v.begin(), v.end(), 0);
        ranges::shuffle(v, urng);
        const auto first_shuffle = v;

        iota(v.begin(), v.end(), 0);
        ranges::shuffle(v, urng);

        assert(v != first_shuffle);
    }
}

// Shuffle quality tests adapted from Lemire's cpp_batched_random test suite.
// These verify that the shuffle produces a proper uniform random permutation,
// not just a valid permutation.

// Every element must be able to reach every position over many trials.
void test_everyone_can_move_everywhere() {
    constexpr size_t size   = 64;
    constexpr size_t trials = size * size; // 4096 trials; probability of missing any (pos,val): ~e^-64

    // Test std::shuffle with mt19937_64 (batched path)
    {
        mt19937_64 urng(42);
        vector<int> input(size);
        vector<char> seen(size * size, 0); // seen[position * size + value]

        for (size_t trial = 0; trial < trials; ++trial) {
            iota(input.begin(), input.end(), 0);
            shuffle(input.begin(), input.end(), urng);
            for (size_t i = 0; i < size; ++i) {
                seen[i * size + static_cast<size_t>(input[i])] = 1;
            }
        }

        for (size_t pos = 0; pos < size; ++pos) {
            for (size_t val = 0; val < size; ++val) {
                assert(seen[pos * size + val]);
            }
        }
    }

    // Test ranges::shuffle with mt19937_64 (batched path)
    {
        mt19937_64 urng(42);
        vector<int> input(size);
        vector<char> seen(size * size, 0);

        for (size_t trial = 0; trial < trials; ++trial) {
            iota(input.begin(), input.end(), 0);
            ranges::shuffle(input, urng);
            for (size_t i = 0; i < size; ++i) {
                seen[i * size + static_cast<size_t>(input[i])] = 1;
            }
        }

        for (size_t pos = 0; pos < size; ++pos) {
            for (size_t val = 0; val < size; ++val) {
                assert(seen[pos * size + val]);
            }
        }
    }
}

// Check that the distribution of values across positions is roughly uniform.
// Uses the "relative gap" metric: (max_count - min_count) / mean_count < 0.6.
void test_uniformity() {
    constexpr size_t size   = 32;
    constexpr size_t trials = size * size * 16; // 16384 trials; expected count per cell = 512

    mt19937_64 urng(42);
    vector<int> input(size);
    vector<size_t> count(size * size, 0); // count[position * size + value]

    for (size_t trial = 0; trial < trials; ++trial) {
        iota(input.begin(), input.end(), 0);
        shuffle(input.begin(), input.end(), urng);
        for (size_t i = 0; i < size; ++i) {
            ++count[i * size + static_cast<size_t>(input[i])];
        }
    }

    size_t overall_min = SIZE_MAX;
    size_t overall_max = 0;
    size_t total       = 0;

    for (size_t cell = 0; cell < size * size; ++cell) {
        total += count[cell];
        if (count[cell] > overall_max) {
            overall_max = count[cell];
        }
        if (count[cell] < overall_min) {
            overall_min = count[cell];
        }
    }

    const double mean         = static_cast<double>(total) / static_cast<double>(size * size);
    const double relative_gap = static_cast<double>(overall_max - overall_min) / mean;

    assert(relative_gap < 0.6);
}

// Every distinct pair of values must be able to appear at the first two positions.
void test_any_possible_pair_at_start() {
    constexpr size_t size   = 32;
    constexpr size_t trials = size * size * size; // 32768 trials; expected count per pair ~33

    mt19937_64 urng(42);
    vector<int> input(size);
    vector<char> seen(size * size, 0); // seen[first * size + second]

    for (size_t trial = 0; trial < trials; ++trial) {
        iota(input.begin(), input.end(), 0);
        shuffle(input.begin(), input.end(), urng);
        seen[static_cast<size_t>(input[0]) * size + static_cast<size_t>(input[1])] = 1;
    }

    for (size_t i = 0; i < size; ++i) {
        for (size_t j = 0; j < size; ++j) {
            if (i == j) {
                assert(!seen[i * size + j]); // same value can't occupy both positions
            } else {
                assert(seen[i * size + j]); // every distinct pair must appear
            }
        }
    }
}

// Every distinct pair of values must be able to appear at the last two positions.
void test_any_possible_pair_at_end() {
    constexpr size_t size   = 32;
    constexpr size_t trials = size * size * size;

    mt19937_64 urng(42);
    vector<int> input(size);
    vector<char> seen(size * size, 0);

    for (size_t trial = 0; trial < trials; ++trial) {
        iota(input.begin(), input.end(), 0);
        shuffle(input.begin(), input.end(), urng);
        seen[static_cast<size_t>(input[size - 2]) * size + static_cast<size_t>(input[size - 1])] = 1;
    }

    for (size_t i = 0; i < size; ++i) {
        for (size_t j = 0; j < size; ++j) {
            if (i == j) {
                assert(!seen[i * size + j]);
            } else {
                assert(seen[i * size + j]);
            }
        }
    }
}

int main() {
    printf("Using seed: %u\n", seed);

    test_random<instantiator, int>();
    test_shuffle_permutation();
    test_shuffle_edge_cases();
    test_gh_6112();
    test_everyone_can_move_everywhere();
    test_uniformity();
    test_any_possible_pair_at_start();
    test_any_possible_pair_at_end();
}
