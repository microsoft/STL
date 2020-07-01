// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <ranges>
#include <utility>

#include <range_algorithm_support.hpp>

using ranges::copy, ranges::copy_result, ranges::iterator_t;
using std::same_as;

// Validate that copy_result aliases in_out_result
STATIC_ASSERT(same_as<copy_result<int, double>, ranges::in_out_result<int, double>>);

// Validate dangling story
STATIC_ASSERT(
    same_as<decltype(copy(borrowed<false>{}, static_cast<int*>(nullptr))), copy_result<ranges::dangling, int*>>);
STATIC_ASSERT(same_as<decltype(copy(borrowed<true>{}, static_cast<int*>(nullptr))), copy_result<int*, int*>>);

struct instantiator {
    static constexpr int input[3] = {13, 42, 1729};

    template <class In, class Write>
    static constexpr void call() {
        { // Validate iterator + sentinel overload
            int output[3] = {-1, -1, -1};
            In wrapped_input{input};
            auto result = copy(wrapped_input.begin(), wrapped_input.end(), Write{output});
            STATIC_ASSERT(same_as<decltype(result), copy_result<iterator_t<In>, Write>>);
            assert(result.in == wrapped_input.end());
            if constexpr (std::equality_comparable<Write>) {
                assert(result.out == Write{output + 3});
            }
            assert(ranges::equal(output, input));
        }
        { // Validate range overload
            int output[3] = {-1, -1, -1};
            In wrapped_input{input};
            auto result = copy(wrapped_input, Write{output});
            STATIC_ASSERT(same_as<decltype(result), copy_result<iterator_t<In>, Write>>);
            assert(result.in == wrapped_input.end());
            if constexpr (std::equality_comparable<Write>) {
                assert(result.out == Write{output + 3});
            }
            assert(ranges::equal(output, input));
        }
    }
};

int main() {
    STATIC_ASSERT((test_in_write<instantiator, const int, int>(), true));
    test_in_write<instantiator, const int, int>();
}
