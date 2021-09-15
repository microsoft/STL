// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <ranges>
#include <utility>

#include <range_algorithm_support.hpp>

using namespace std;

struct int_wrapper {
    int val                 = 10;
    constexpr int_wrapper() = default;
    constexpr int_wrapper(int x) : val{x} {}
    constexpr int_wrapper(int_wrapper&& that) : val{exchange(that.val, -1)} {}
    constexpr int_wrapper& operator=(int_wrapper&& that) {
        val = exchange(that.val, -1);
        return *this;
    }
    auto operator<=>(const int_wrapper&) const = default;
};

// Validate that move_result aliases in_out_result
STATIC_ASSERT(same_as<ranges::move_result<int, double>, ranges::in_out_result<int, double>>);

// Validate dangling story
STATIC_ASSERT(
    same_as<decltype(ranges::move(borrowed<false>{}, nullptr_to<int>)), ranges::move_result<ranges::dangling, int*>>);
STATIC_ASSERT(same_as<decltype(ranges::move(borrowed<true>{}, nullptr_to<int>)), ranges::move_result<int*, int*>>);

struct instantiator {
    static constexpr int_wrapper expected_output[3] = {13, 55, 12345};
    static constexpr int_wrapper expected_input[3]  = {-1, -1, -1};

    template <ranges::input_range Read, indirectly_writable<ranges::range_rvalue_reference_t<Read>> Write>
    static constexpr void call() {
        using ranges::move, ranges::move_result, ranges::equal, ranges::iterator_t;
        {
            int_wrapper input[3]  = {13, 55, 12345};
            int_wrapper output[3] = {-2, -2, -2};
            Read wrapped_input{input};

            auto result = move(wrapped_input, Write{output});
            STATIC_ASSERT(same_as<decltype(result), move_result<iterator_t<Read>, Write>>);
            assert(result.in == wrapped_input.end());
            assert(result.out.peek() == output + 3);
            assert(equal(output, expected_output));
            assert(equal(input, expected_input));
        }
        {
            int_wrapper input[3]  = {13, 55, 12345};
            int_wrapper output[3] = {-2, -2, -2};
            Read wrapped_input{input};

            auto result = move(wrapped_input.begin(), wrapped_input.end(), Write{output});
            STATIC_ASSERT(same_as<decltype(result), move_result<iterator_t<Read>, Write>>);
            assert(result.in == wrapped_input.end());
            assert(result.out.peek() == output + 3);
            assert(equal(output, expected_output));
            assert(equal(input, expected_input));
        }
    }
};

int main() {
#ifndef _PREFAST_ // TRANSITION, GH-1030
    STATIC_ASSERT((test_in_write<instantiator, int_wrapper, int_wrapper>(), true));
#endif // TRANSITION, GH-1030
    test_in_write<instantiator, int_wrapper, int_wrapper>();
}
