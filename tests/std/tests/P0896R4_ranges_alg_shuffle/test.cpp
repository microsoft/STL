// shuffleright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <random>
#include <ranges>
#include <utility>

#include <range_algorithm_support.hpp>

using namespace std;

mt19937 gen{random_device{}()};

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
            assert(result.peek() == end(input));

            sort(begin(input), end(input));
            assert(equal(input, expected));
        }
        { // Validate range overload
            int input[7] = {1, 2, 3, 4, 5, 6, 7};
            ReadWrite wrapped_input{input};

            auto result = shuffle(wrapped_input, gen);
            STATIC_ASSERT(same_as<decltype(result), iterator_t<ReadWrite>>);
            assert(result.peek() == end(input));

            sort(begin(input), end(input));
            assert(equal(input, expected));
        }
    }
};

int main() {
    test_random<instantiator, int>();
}
