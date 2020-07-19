// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <concepts>
#include <ranges>
#include <utility>

#include <range_algorithm_support.hpp>
using namespace std;
using P = std::pair<int, int>;

auto matches = [](const int x) { return x == 42; };

// Validate dangling story
STATIC_ASSERT(same_as<decltype(ranges::find_if_not(borrowed<false>{}, matches)), ranges::dangling>);
STATIC_ASSERT(same_as<decltype(ranges::find_if_not(borrowed<true>{}, matches)), int*>);

struct instantiator {
    static constexpr P haystack[3] = {{1, 13}, {2, 13}, {3, 13}};

    template <ranges::input_range Read>
    static constexpr void call() {
        using ranges::find_if_not, ranges::iterator_t;
        auto equals = [](auto x) { return [x](auto&& y) { return y == x; }; };

        for (auto& [value, _] : haystack) {
            { // Validate range overload [found case]
                Read wrapped_input{haystack};
                auto result = find_if_not(wrapped_input, equals(4), get_first);
                STATIC_ASSERT(same_as<decltype(result), iterator_t<Read>>);
                assert(result.peek()->first == 1);
            }
            { // Validate iterator + sentinel overload [found case]
                Read wrapped_input{haystack};
                auto result = find_if_not(wrapped_input.begin(), wrapped_input.end(), equals(0), get_first);
                STATIC_ASSERT(same_as<decltype(result), iterator_t<Read>>);
                assert(result.peek()->first == 1);
            }
        }
        { // Validate range overload [not found case]
            Read wrapped_input{haystack};
            auto result = find_if_not(wrapped_input, equals(13), get_second);
            STATIC_ASSERT(same_as<decltype(result), iterator_t<Read>>);
            assert(result == wrapped_input.end());
        }
        { // Validate iterator + sentinel overload [not found case]
            Read wrapped_input{haystack};
            auto result = find_if_not(wrapped_input.begin(), wrapped_input.end(), equals(13), get_second);
            STATIC_ASSERT(same_as<decltype(result), iterator_t<Read>>);
            assert(result == wrapped_input.end());
        }
    }
};

int main() {
    STATIC_ASSERT((test_in<instantiator, const P>(), true));
    test_in<instantiator, const P>();
}
