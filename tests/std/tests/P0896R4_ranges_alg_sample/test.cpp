// sampleright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <random>
#include <ranges>
#include <utility>

#include <range_algorithm_support.hpp>

using namespace std;

struct instantiator {
    static constexpr int reservoir[5] = {13, 42, 71, 112, 1729};

    template <ranges::input_range Read, indirectly_writable<ranges::range_reference_t<Read>> Write>
    static void call() {
        using ranges::sample, ranges::equal, ranges::is_sorted, ranges::iterator_t;

        if constexpr (forward_iterator<iterator_t<Read>> || random_access_iterator<Write>) {
            mt19937 gen{random_device{}()};
            auto copy_gen = gen;

            { // Validate iterator + sentinel overload
                int output1[3] = {-1, -1, -1};
                int output2[3] = {-1, -1, -1};
                Read wrapped_input{reservoir};

                auto result1 = sample(wrapped_input.begin(), wrapped_input.end(), Write{output1}, 3, gen);
                STATIC_ASSERT(same_as<decltype(result1), Write>);
                assert(result1.peek() == end(output1));

                // check repeatability
                auto result2 = sample(wrapped_input.begin(), wrapped_input.end(), Write{output2}, 3, copy_gen);
                assert(equal(output1, output2));

                if (ranges::forward_range<Read>) {
                    // verify stability
                    assert(is_sorted(output1));
                } else {
                    // ensure ordering for set_difference
                    sort(begin(output1), end(output1));
                }
                assert(includes(cbegin(reservoir), cend(reservoir), cbegin(output1), cend(output1)));
            }
            { // Validate range overload
                int output1[3] = {-1, -1, -1};
                int output2[3] = {-1, -1, -1};
                Read wrapped_input{reservoir};

                auto result1 = sample(wrapped_input, Write{output1}, 3, gen);
                STATIC_ASSERT(same_as<decltype(result1), Write>);
                assert(result1.peek() == end(output1));

                // check repeatability
                auto result2 = sample(wrapped_input, Write{output2}, 3, copy_gen);
                assert(equal(output1, output2));

                if (ranges::forward_range<Read>) {
                    // verify stability
                    assert(is_sorted(output1));
                } else {
                    // ensure ordering for set_difference
                    sort(begin(output1), end(output1));
                }
                assert(includes(cbegin(reservoir), cend(reservoir), cbegin(output1), cend(output1)));
            }
        }
    }
};

int main() {
    test_in_write<instantiator, int const, int>();
}
