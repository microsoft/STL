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

constexpr auto matches = [](const int val) { return val == 47; };

// Validate that remove_copy_if_result aliases in_out_result
STATIC_ASSERT(same_as<ranges::remove_copy_if_result<int, double>, ranges::in_out_result<int, double>>);

// Validate dangling story
STATIC_ASSERT(same_as<decltype(ranges::remove_copy_if(borrowed<false>{}, static_cast<int*>(nullptr), matches)),
    ranges::remove_copy_if_result<ranges::dangling, int*>>);
STATIC_ASSERT(same_as<decltype(ranges::remove_copy_if(borrowed<true>{}, static_cast<int*>(nullptr), matches)),
    ranges::remove_copy_if_result<int*, int*>>);

struct instantiator {
    static constexpr P input[5]    = {{0, 99}, {1, 47}, {2, 99}, {3, 47}, {4, 99}};
    static constexpr P expected[3] = {{0, 99}, {2, 99}, {4, 99}};

    template <ranges::input_range Read, indirectly_writable<ranges::range_reference_t<Read>> Write>
    static constexpr void call() {
        // Fails checking the indirect_binary_predicate requirement in C1XX's permissive mode with proxy iterators
        // (probably related to VSO-566808)
        constexpr bool non_proxy =
            is_reference_v<ranges::range_reference_t<Read>> && is_reference_v<iter_reference_t<Write>>;
        if constexpr (non_proxy || !is_permissive) {
            using ranges::remove_copy_if, ranges::remove_copy_if_result, ranges::equal, ranges::iterator_t;

            size_t projectionCounter = 0;
            auto projection          = [&projectionCounter](const P& val) {
                ++projectionCounter;
                return val.second;
            };

            { // Validate iterator + sentinel overload
                P output[3] = {{-1, -1}, {-1, -1}, {-1, -1}};
                Read wrapped_input{input};
                auto result =
                    remove_copy_if(wrapped_input.begin(), wrapped_input.end(), Write{output}, matches, projection);
                STATIC_ASSERT(same_as<decltype(result), remove_copy_if_result<iterator_t<Read>, Write>>);
                assert(result.in == wrapped_input.end());
                assert(result.out.peek() == output + 3);
                assert(equal(output, expected));
                assert(projectionCounter == ranges::size(input));
            }

            projectionCounter = 0;

            { // Validate range overload
                P output[3] = {{-1, -1}, {-1, -1}, {-1, -1}};
                Read wrapped_input{input};
                auto result = remove_copy_if(wrapped_input, Write{output}, matches, projection);
                STATIC_ASSERT(same_as<decltype(result), remove_copy_if_result<iterator_t<Read>, Write>>);
                assert(result.in == wrapped_input.end());
                assert(result.out.peek() == output + 3);
                assert(equal(output, expected));
                assert(projectionCounter == ranges::size(input));
            }
        }
    }
};

int main() {
#ifndef _PREFAST_ // TRANSITION, GH-1030
    STATIC_ASSERT((test_in_write<instantiator, const P, P>(), true));
#endif // TRANSITION, GH-1030
    test_in_write<instantiator, const P, P>();
}
