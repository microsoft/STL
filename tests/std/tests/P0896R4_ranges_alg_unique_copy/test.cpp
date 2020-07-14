// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <ranges>
#include <span>
#include <utility>

#include <range_algorithm_support.hpp>
using namespace std;
using P = pair<int, int>;

// Validate that unique_copy_result aliases in_out_result
STATIC_ASSERT(same_as<ranges::unique_copy_result<int, double>, ranges::in_out_result<int, double>>);

// Validate dangling story
STATIC_ASSERT(same_as<decltype(ranges::unique_copy(borrowed<false>{}, nullptr_to<int>)),
    ranges::unique_copy_result<ranges::dangling, int*>>);
STATIC_ASSERT(
    same_as<decltype(ranges::unique_copy(borrowed<true>{}, nullptr_to<int>)), ranges::unique_copy_result<int*, int*>>);

struct instantiator {
    static constexpr P expected[4] = {{0, 99}, {1, 47}, {3, 99}, {4, 47}};
    static constexpr P input[6]    = {{0, 99}, {1, 47}, {2, 47}, {3, 99}, {4, 47}, {5, 47}};

    template <ranges::input_range Read, weakly_incrementable Write>
    static constexpr void call() {
        using ranges::unique_copy, ranges::unique_copy_result, ranges::equal, ranges::equal_to, ranges::size,
            ranges::iterator_t;

        { // Validate iterator + sentinel overload
            P output[4] = {{-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}};
            Read wrapped_input{input};

            // Tests which implementation strategy was chosen
            size_t inputCounter    = 0;
            size_t outputCounter   = 0;
            size_t storeCounter    = 0;
            auto countedProjection = [&](const P& val) {
                if (output <= addressof(val) && addressof(val) < end(output)) {
                    ++outputCounter;
                } else if (input <= addressof(val) && addressof(val) < end(input)) {
                    ++inputCounter;
                } else {
                    ++storeCounter;
                }
                return val.second;
            };

            auto result =
                unique_copy(wrapped_input.begin(), wrapped_input.end(), Write{output}, equal_to{}, countedProjection);
            STATIC_ASSERT(same_as<decltype(result), unique_copy_result<iterator_t<Read>, Write>>);
            assert(result.in == wrapped_input.end());
            assert(result.out.peek() == output + 4);
            assert(equal(expected, output));
            if constexpr (input_iterator<Write>) {
                assert(inputCounter == size(input) - 1);
                assert(outputCounter == size(input) - 1);
                assert(storeCounter == 0);
            } else if constexpr (ranges::forward_range<Read>) {
                assert(inputCounter == 2 * (size(input) - 1));
                assert(outputCounter == 0);
                assert(storeCounter == 0);
            } else {
                assert(inputCounter == size(input) - 1);
                assert(outputCounter == 0);
                assert(storeCounter == size(input) - 1);
            }
        }

        { // Validate range overload
            P output[4] = {{-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}};
            Read wrapped_input{input};

            // Tests which implementation strategy was chosen
            size_t inputCounter    = 0;
            size_t outputCounter   = 0;
            size_t storeCounter    = 0;
            auto countedProjection = [&](const P& val) {
                if (output <= addressof(val) && addressof(val) < end(output)) {
                    ++outputCounter;
                } else if (input <= addressof(val) && addressof(val) < end(input)) {
                    ++inputCounter;
                } else {
                    ++storeCounter;
                }
                return val.second;
            };

            auto result = unique_copy(wrapped_input, Write{output}, equal_to{}, countedProjection);
            STATIC_ASSERT(same_as<decltype(result), unique_copy_result<iterator_t<Read>, Write>>);
            assert(result.in == wrapped_input.end());
            assert(result.out.peek() == output + 4);
            assert(equal(expected, output));
            if constexpr (input_iterator<Write>) {
                assert(inputCounter == size(input) - 1);
                assert(outputCounter == size(input) - 1);
                assert(storeCounter == 0);
            } else if constexpr (ranges::forward_range<Read>) {
                assert(inputCounter == 2 * (size(input) - 1));
                assert(outputCounter == 0);
                assert(storeCounter == 0);
            } else {
                assert(inputCounter == size(input) - 1);
                assert(outputCounter == 0);
                assert(storeCounter == size(input) - 1);
            }
        }
    }
};

int main() {
    STATIC_ASSERT((test_in_write<instantiator, const P, P>(), true));
    test_in_write<instantiator, const P, P>();
}
