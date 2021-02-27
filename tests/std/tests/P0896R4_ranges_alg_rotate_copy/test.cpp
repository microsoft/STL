// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <ranges>

#include <range_algorithm_support.hpp>
using namespace std;

// Validate that rotate_copy_result aliases in_out_result
STATIC_ASSERT(same_as<ranges::rotate_copy_result<int, double>, ranges::in_out_result<int, double>>);

// Validate dangling story
STATIC_ASSERT(same_as<decltype(ranges::rotate_copy(borrowed<false>{}, nullptr_to<int>, nullptr_to<int>)),
    ranges::rotate_copy_result<ranges::dangling, int*>>);
STATIC_ASSERT(same_as<decltype(ranges::rotate_copy(borrowed<true>{}, nullptr_to<int>, nullptr_to<int>)),
    ranges::rotate_copy_result<int*, int*>>);

struct instantiator {
    static constexpr int input[5]    = {1, 2, 3, 4, 5};
    static constexpr int expected[5] = {4, 5, 1, 2, 3};

    template <ranges::forward_range Read, indirectly_writable<ranges::range_reference_t<Read>> Write>
    static constexpr void call() {
        using ranges::rotate_copy, ranges::rotate_copy_result, ranges::equal, ranges::iterator_t;
        { // Validate iterator overload
            int output[5] = {-1, -1, -1, -1, -1};
            Read wrapped_input{input};
            iterator_t<Read> mid = next(wrapped_input.begin(), 3);

            auto result = rotate_copy(wrapped_input.begin(), mid, wrapped_input.end(), Write{output});
            STATIC_ASSERT(same_as<decltype(result), rotate_copy_result<iterator_t<Read>, Write>>);
            assert(result.in == wrapped_input.end());
            assert(result.out.peek() == end(output));
            assert(equal(expected, output));
        }
        { // Validate range overload
            int output[5] = {-1, -1, -1, -1, -1};
            Read wrapped_input{input};
            iterator_t<Read> mid = next(wrapped_input.begin(), 3);

            auto result = rotate_copy(wrapped_input, mid, Write{output});
            STATIC_ASSERT(same_as<decltype(result), rotate_copy_result<iterator_t<Read>, Write>>);
            assert(result.in == wrapped_input.end());
            assert(result.out.peek() == end(output));
            assert(equal(expected, output));
        }
    }
};

int main() {
    STATIC_ASSERT((test_fwd_write<instantiator, const int, int>(), true));
    test_fwd_write<instantiator, const int, int>();
}
