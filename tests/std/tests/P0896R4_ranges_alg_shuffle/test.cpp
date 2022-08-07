// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <cstdio>
#include <random>
#include <ranges>
#include <utility>

#include <range_algorithm_support.hpp>
using namespace std;

const unsigned int seed = random_device{}();
mt19937 gen{seed};

// Validate dangling story
STATIC_ASSERT(same_as<decltype(ranges::shuffle(borrowed<false>{}, gen)), ranges::dangling>);
STATIC_ASSERT(same_as<decltype(ranges::shuffle(borrowed<true>{}, gen)), int*>);

struct instantiator {
    static constexpr int expected[7] = {1, 2, 3, 4, 5, 6, 7};

    template <ranges::random_access_range ReadWrite>
    static void call() {
        using ranges::shuffle, ranges::equal, ranges::is_sorted, ranges::iterator_t;

        { // Validate iterator + sentinel overload
            int input[7] = {1, 2, 3, 4, 5, 6, 7};
            ReadWrite wrapped_input{input};

            auto result = shuffle(wrapped_input.begin(), wrapped_input.end(), gen);
            STATIC_ASSERT(same_as<decltype(result), iterator_t<ReadWrite>>);
            assert(result == wrapped_input.end());

            sort(begin(input), end(input));
            assert(equal(input, expected));
        }
        { // Validate range overload
            int input[7] = {1, 2, 3, 4, 5, 6, 7};
            ReadWrite wrapped_input{input};

            auto result = shuffle(wrapped_input, gen);
            STATIC_ASSERT(same_as<decltype(result), iterator_t<ReadWrite>>);
            assert(result == wrapped_input.end());

            sort(begin(input), end(input));
            assert(equal(input, expected));
        }
    }
};

void test_urbg() { // COMPILE-ONLY
    struct RandGen {
        static constexpr bool min() {
            return false;
        }
        static constexpr bool max() {
            return true;
        }
        bool operator()() & {
            return false;
        }
    };

    STATIC_ASSERT(uniform_random_bit_generator<RandGen>);

    int arr[1] = {};
    ranges::shuffle(arr, RandGen{});
}

int main() {
    printf("Using seed: %u\n", seed);

    test_random<instantiator, int>();
}
