// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <ranges>
#include <utility>

#include <range_algorithm_support.hpp>
using namespace std;
using P = pair<int, int>;

// Validate that remove_copy_result aliases in_out_result
STATIC_ASSERT(same_as<ranges::remove_copy_result<int, double>, ranges::in_out_result<int, double>>);

// Validate dangling story
STATIC_ASSERT(same_as<decltype(ranges::remove_copy(borrowed<false>{}, static_cast<int*>(nullptr), 42)),
    ranges::remove_copy_result<ranges::dangling, int*>>);
STATIC_ASSERT(same_as<decltype(ranges::remove_copy(borrowed<true>{}, static_cast<int*>(nullptr), 42)),
    ranges::remove_copy_result<int*, int*>>);

struct instantiator {
    static constexpr P input[5]    = {{0, 99}, {1, 47}, {2, 99}, {3, 47}, {4, 99}};
    static constexpr P expected[3] = {{0, 99}, {2, 99}, {4, 99}};

    static constexpr void eq(P const (&output)[3]) {
        // Extracted into a separate function to keep /analyze from exhausting the compiler heap
        assert(ranges::equal(output, expected));
    }

    template <ranges::input_range Read, indirectly_writable<ranges::range_reference_t<Read>> Write>
    static constexpr void call() {
        using ranges::remove_copy, ranges::remove_copy_result, ranges::iterator_t;
        { // Validate iterator + sentinel overload
            P output[3] = {{-1, -1}, {-1, -1}, {-1, -1}};
            Read wrapped_input{input};
            size_t comparisonsCounter = 0;
            auto projection           = [&comparisonsCounter](const P& data) {
                ++comparisonsCounter;
                return data.second;
            };

            auto result = remove_copy(wrapped_input.begin(), wrapped_input.end(), Write{output}, 47, projection);
            STATIC_ASSERT(same_as<decltype(result), remove_copy_result<iterator_t<Read>, Write>>);
            assert(result.in == wrapped_input.end());
            assert(result.out.peek() == output + 3);
            eq(output);
            assert(comparisonsCounter == ranges::size(input));
        }
        { // Validate range overload
            P output[3] = {{-1, -1}, {-1, -1}, {-1, -1}};
            Read wrapped_input{input};
            size_t comparisonsCounter = 0;
            auto projection           = [&comparisonsCounter](const P& data) {
                ++comparisonsCounter;
                return data.second;
            };

            auto result = remove_copy(wrapped_input, Write{output}, 47, projection);
            STATIC_ASSERT(same_as<decltype(result), remove_copy_result<iterator_t<Read>, Write>>);
            assert(result.in == wrapped_input.end());
            assert(result.out.peek() == output + 3);
            eq(output);
            assert(comparisonsCounter == ranges::size(input));
        }
    }
};

int main() {
    STATIC_ASSERT((test_in_write<instantiator, const P, P>(), true));
    test_in_write<instantiator, const P, P>();
}
