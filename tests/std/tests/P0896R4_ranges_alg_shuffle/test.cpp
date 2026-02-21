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
    const vector<int> original = [] {
        vector<int> ret(100);
        iota(ret.begin(), ret.end(), 0);
        return ret;
    }();

    // Test with 64-bit generator (batched random path)
    {
        vector<int> v = original;

        shuffle(v.begin(), v.end(), gen64);

        // Verify it's still a permutation
        sort(v.begin(), v.end());
        assert(v == original);
    }

    // Test with ranges::shuffle and 64-bit generator (batched random path)
    {
        vector<int> v = original;

        ranges::shuffle(v, gen64);

        // Verify it's still a permutation
        sort(v.begin(), v.end());
        assert(v == original);
    }

    // Test with 32-bit generator (non-batched path)
    {
        vector<int> v = original;

        shuffle(v.begin(), v.end(), gen);

        // Verify it's still a permutation
        sort(v.begin(), v.end());
        assert(v == original);
    }

    // Test with ranges::shuffle and 32-bit generator (non-batched path)
    {
        vector<int> v = original;

        ranges::shuffle(v, gen);

        // Verify it's still a permutation
        sort(v.begin(), v.end());
        assert(v == original);
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
        vector<int> v              = {1, 2};
        const vector<int> original = v;
        shuffle(v.begin(), v.end(), gen64);
        sort(v.begin(), v.end());
        assert(v == original);
    }

    // Three elements (odd count, tests batching boundary)
    {
        vector<int> v              = {1, 2, 3};
        const vector<int> original = v;
        shuffle(v.begin(), v.end(), gen64);
        sort(v.begin(), v.end());
        assert(v == original);
    }

    // Four elements (even count)
    {
        vector<int> v              = {1, 2, 3, 4};
        const vector<int> original = v;
        shuffle(v.begin(), v.end(), gen64);
        sort(v.begin(), v.end());
        assert(v == original);
    }

    // Large array to ensure batching is effective
    {
        vector<int> v(10000);
        iota(v.begin(), v.end(), 0);
        const vector<int> original = v;
        shuffle(v.begin(), v.end(), gen64);
        sort(v.begin(), v.end());
        assert(v == original);
    }
}

int main() {
    printf("Using seed: %u\n", seed);

    test_random<instantiator, int>();
    test_shuffle_permutation();
    test_shuffle_edge_cases();
}
