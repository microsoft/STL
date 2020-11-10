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

// Validate that unary_transform_result aliases in_out_result
STATIC_ASSERT(same_as<ranges::unary_transform_result<int, double>, ranges::in_out_result<int, double>>);

constexpr auto minus_one = [](const int x) { return x - 1; };

// Validate dangling story
STATIC_ASSERT(same_as<decltype(ranges::transform(borrowed<false>{}, nullptr_to<int>, minus_one)),
    ranges::unary_transform_result<ranges::dangling, int*>>);
STATIC_ASSERT(same_as<decltype(ranges::transform(borrowed<true>{}, nullptr_to<int>, minus_one)),
    ranges::unary_transform_result<int*, int*>>);

struct instantiator {
    static constexpr P input[3]      = {{1, 99}, {4, 98}, {5, 97}};
    static constexpr int expected[3] = {0, 3, 4};

    template <ranges::input_range Read, weakly_incrementable Write>
    static constexpr void call() {
        using ranges::transform, ranges::unary_transform_result, ranges::iterator_t;
        { // Validate iterator + sentinel overload
            int output[3] = {-1, -1, -1};
            Read wrapped_in{input};

            auto result = transform(wrapped_in.begin(), wrapped_in.end(), Write{output}, minus_one, get_first);
            STATIC_ASSERT(same_as<decltype(result), unary_transform_result<iterator_t<Read>, Write>>);
            assert(result.in == wrapped_in.end());
            assert(result.out.peek() == output + 3);
            assert(ranges::equal(output, expected));
        }
        { // Validate range overload
            int output[3] = {-1, -1, -1};
            Read wrapped_in{input};

            auto result = transform(wrapped_in, Write{output}, minus_one, get_first);
            STATIC_ASSERT(same_as<decltype(result), unary_transform_result<iterator_t<Read>, Write>>);
            assert(result.in == wrapped_in.end());
            assert(result.out.peek() == output + 3);
            assert(ranges::equal(output, expected));
        }
    }
};

int main() {
    STATIC_ASSERT((test_in_write<instantiator, const P, int>(), true));
    test_in_write<instantiator, const P, int>();
}
