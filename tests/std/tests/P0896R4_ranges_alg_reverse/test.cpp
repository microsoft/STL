// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <ranges>
#include <utility>

#include <range_algorithm_support.hpp>

using namespace std;

// Validate dangling story
STATIC_ASSERT(same_as<decltype(ranges::reverse(borrowed<false>{})), ranges::dangling>);
STATIC_ASSERT(same_as<decltype(ranges::reverse(borrowed<true>{})), int*>);

struct instantiator {
    static constexpr int expected[] = {1367, 42, 13};

    template <class Bidi>
    static constexpr void call() {
        using ranges::reverse, ranges::equal, ranges::iterator_t;

        { // Validate iterator + sentinel overload
            int input[] = {13, 42, 1367};
            Bidi wrapped_input{input};
            auto result = reverse(wrapped_input.begin(), wrapped_input.end());
            STATIC_ASSERT(same_as<decltype(result), iterator_t<Bidi>>);
            assert(result == wrapped_input.end());
            assert(equal(input, expected));
        }
        { // Validate range overload
            int input[] = {13, 42, 1367};
            Bidi wrapped_input{input};
            auto result = reverse(wrapped_input);
            STATIC_ASSERT(same_as<decltype(result), iterator_t<Bidi>>);
            assert(result == wrapped_input.end());
            assert(equal(input, expected));
        }
    }
};

int main() {
#if defined(__clang__) || defined(__EDG__) // TRANSITION, VSO-938163
    STATIC_ASSERT((test_bidi<instantiator, int>(), true));
#endif // TRANSITION, VSO-938163
    test_bidi<instantiator, int>();
}
