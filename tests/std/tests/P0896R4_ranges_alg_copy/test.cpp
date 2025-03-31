// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <ranges>
#include <utility>

#include <range_algorithm_support.hpp>

using namespace std;

// Validate that copy_result aliases in_out_result
static_assert(same_as<ranges::copy_result<int, double>, ranges::in_out_result<int, double>>);

// Validate dangling story
static_assert(
    same_as<decltype(ranges::copy(borrowed<false>{}, nullptr_to<int>)), ranges::copy_result<ranges::dangling, int*>>);
static_assert(same_as<decltype(ranges::copy(borrowed<true>{}, nullptr_to<int>)), ranges::copy_result<int*, int*>>);

struct instantiator {
    static constexpr int input[3] = {13, 42, 1729};

    template <ranges::input_range Read, indirectly_writable<ranges::range_reference_t<Read>> Write>
    static constexpr void call() {
        using ranges::copy, ranges::copy_result, ranges::iterator_t;
        { // Validate iterator + sentinel overload
            int output[3] = {-1, -1, -1};
            Read wrapped_input{input};

            auto result = copy(wrapped_input.begin(), wrapped_input.end(), Write{output});
            static_assert(same_as<decltype(result), copy_result<iterator_t<Read>, Write>>);
            assert(result.in == wrapped_input.end());
            assert(result.out.peek() == output + 3);
            assert(ranges::equal(output, input));
        }
        { // Validate range overload
            int output[3] = {-1, -1, -1};
            Read wrapped_input{input};

            auto result = copy(wrapped_input, Write{output});
            static_assert(same_as<decltype(result), copy_result<iterator_t<Read>, Write>>);
            assert(result.in == wrapped_input.end());
            assert(result.out.peek() == output + 3);
            assert(ranges::equal(output, input));
        }
    }
};

int main() {
#ifndef _PREFAST_ // TRANSITION, GH-1030
    static_assert((test_in_write<instantiator, int const, int>(), true));
#endif
    test_in_write<instantiator, int const, int>();
}
