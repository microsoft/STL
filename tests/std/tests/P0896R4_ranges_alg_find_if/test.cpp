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

constexpr auto matches = [](const int val) { return val == 42; };
constexpr auto equals  = [](auto x) { return [x](auto&& y) { return y == x; }; };

// Validate dangling story
static_assert(same_as<decltype(ranges::find_if(borrowed<false>{}, matches)), ranges::dangling>);
static_assert(same_as<decltype(ranges::find_if(borrowed<true>{}, matches)), int*>);

struct instantiator {
    static constexpr P haystack[3] = {{0, 42}, {2, 42}, {4, 42}};

    template <ranges::input_range Read>
    static constexpr void call() {
        using ranges::find_if, ranges::iterator_t;

        for (const auto& [value, _] : haystack) {
            { // Validate range overload [found case]
                Read wrapped_input{haystack};
                auto result = find_if(wrapped_input, equals(value), get_first);
                static_assert(same_as<decltype(result), iterator_t<Read>>);
                assert(result.peek()->first == value);
            }
            { // Validate iterator + sentinel overload [found case]
                Read wrapped_input{haystack};
                auto result = find_if(wrapped_input.begin(), wrapped_input.end(), equals(value), get_first);
                static_assert(same_as<decltype(result), iterator_t<Read>>);
                assert(result.peek()->first == value);
            }
        }
        { // Validate range overload [not found case]
            Read wrapped_input{haystack};
            auto result = find_if(wrapped_input, equals(42), get_first);
            static_assert(same_as<decltype(result), iterator_t<Read>>);
            assert(result == wrapped_input.end());
        }
        { // Validate iterator + sentinel overload [not found case]
            Read wrapped_input{haystack};
            auto result = find_if(wrapped_input.begin(), wrapped_input.end(), equals(42), get_first);
            static_assert(same_as<decltype(result), iterator_t<Read>>);
            assert(result == wrapped_input.end());
        }
    }
};

int main() {
    static_assert((test_in<instantiator, const P>(), true));
    test_in<instantiator, const P>();
}
