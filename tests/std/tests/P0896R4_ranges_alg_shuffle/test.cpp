// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <cstddef>
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
        sort(v.begin(), v.end());
        assert(v == original); // Verify it's still a permutation
    }

    // Test with ranges::shuffle and 64-bit generator (batched random path)
    {
        vector<int> v = original;
        ranges::shuffle(v, gen64);
        sort(v.begin(), v.end());
        assert(v == original); // Verify it's still a permutation
    }

    // Test with 32-bit generator (non-batched path)
    {
        vector<int> v = original;
        shuffle(v.begin(), v.end(), gen);
        sort(v.begin(), v.end());
        assert(v == original); // Verify it's still a permutation
    }

    // Test with ranges::shuffle and 32-bit generator (non-batched path)
    {
        vector<int> v = original;
        ranges::shuffle(v, gen);
        sort(v.begin(), v.end());
        assert(v == original); // Verify it's still a permutation
    }
}

[[nodiscard]] bool shuffle_is_a_permutation(const size_t n) {
    vector<int> v(n);
    iota(v.begin(), v.end(), 0);
    const vector<int> original = v;
    shuffle(v.begin(), v.end(), gen64);
    sort(v.begin(), v.end());
    return v == original; // have the caller assert() for clearer diagnostics
}

// Test edge cases for shuffle
void test_shuffle_edge_cases() {
    // Test both even and odd sizes to exercise the batching boundary.
    // Test large sizes to ensure batching is effective.
    assert(shuffle_is_a_permutation(0));
    assert(shuffle_is_a_permutation(1));
    assert(shuffle_is_a_permutation(2));
    assert(shuffle_is_a_permutation(3));
    assert(shuffle_is_a_permutation(4));
    assert(shuffle_is_a_permutation(1729));
    assert(shuffle_is_a_permutation(10000));
}

int main() {
    printf("Using seed: %u\n", seed);

    test_random<instantiator, int>();
    test_shuffle_permutation();
    test_shuffle_edge_cases();
}
