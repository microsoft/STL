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
        using ranges::unique_copy, ranges::unique_copy_result, ranges::iterator_t;

        size_t comparisonCounter = 0;
        auto countedEq           = [&comparisonCounter](const int a, const int b) {
            ++comparisonCounter;
            return a == b;
        };

        { // Validate iterator + sentinel overload
            P output[4] = {{-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}};
            Read wrapped_input{input};

            auto result = unique_copy(wrapped_input.begin(), wrapped_input.end(), Write{output}, countedEq, get_second);
            STATIC_ASSERT(same_as<decltype(result), unique_copy_result<iterator_t<Read>, Write>>);
            assert(result.in == wrapped_input.end());
            assert(result.out.peek() == output + 4);
            assert(ranges::equal(expected, output));
            assert(comparisonCounter == 5);
        }

        comparisonCounter = 0;

        { // Validate range overload
            P output[4] = {{-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}};
            Read wrapped_input{input};

            auto result = unique_copy(wrapped_input, Write{output}, countedEq, get_second);
            STATIC_ASSERT(same_as<decltype(result), unique_copy_result<iterator_t<Read>, Write>>);
            assert(result.in == wrapped_input.end());
            assert(result.out.peek() == output + 4);
            assert(ranges::equal(expected, output));
            assert(comparisonCounter == 5);
        }
    }
};

int main() {
    // with store
    STATIC_ASSERT((input_range_output_iterator_permutations<instantiator, const P, P>(), true));
    input_range_output_iterator_permutations<instantiator, const P, P>();

    // with readable out
    STATIC_ASSERT((input_range_input_iterator_permutations<instantiator, const P, P>(), true));
    input_range_input_iterator_permutations<instantiator, const P, P>();

    // with readable in
    STATIC_ASSERT((forward_range_output_iterator_permutations<instantiator, const P, P>(), true));
    forward_range_output_iterator_permutations<instantiator, const P, P>();
}
