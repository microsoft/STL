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
STATIC_ASSERT(same_as<decltype(ranges::find_last_if(borrowed<false>{}, matches)), ranges::dangling>);
STATIC_ASSERT(same_as<decltype(ranges::find_last_if(borrowed<true>{}, matches)), ranges::subrange<int*>>);

template <class T, class U>
constexpr void check_value(const T& found, const U& value) {
    if constexpr (same_as<T, P>) {
        assert(found.first == value);
    } else {
        assert(found.peek().first == value);
    }
}

struct instantiator {
    static constexpr P haystack[3] = {{0, 42}, {2, 42}, {4, 42}};

    template <ranges::forward_range Read>
    static constexpr void call() {
        using ranges::find_last_if, ranges::common_range, ranges::iterator_t;

        for (const auto& [value, _] : haystack) {
            { // Validate range overload [found case]
                Read wrapped_input{haystack};
                auto result = find_last_if(wrapped_input, equals(value), get_first);
                STATIC_ASSERT(same_as<iterator_t<decltype(result)>, iterator_t<Read>>);
                STATIC_ASSERT(common_range<decltype(result)>);
                check_value(result.front(), value);
            }
            { // Validate iterator + sentinel overload [found case]
                Read wrapped_input{haystack};
                auto result = find_last_if(wrapped_input.begin(), wrapped_input.end(), equals(value), get_first);
                STATIC_ASSERT(same_as<iterator_t<decltype(result)>, iterator_t<Read>>);
                STATIC_ASSERT(common_range<decltype(result)>);
                check_value(result.front(), value);
            }
        }
        { // Validate range overload [not found case]
            Read wrapped_input{haystack};
            auto result = find_last_if(wrapped_input, equals(42), get_first);
            STATIC_ASSERT(same_as<iterator_t<decltype(result)>, iterator_t<Read>>);
            assert(result.begin() == wrapped_input.end());
        }
        { // Validate iterator + sentinel overload [not found case]
            Read wrapped_input{haystack};
            auto result = find_last_if(wrapped_input.begin(), wrapped_input.end(), equals(42), get_first);
            STATIC_ASSERT(same_as<iterator_t<decltype(result)>, iterator_t<Read>>);
            assert(result.begin() == wrapped_input.end());
        }
    }
};

int main() {
    STATIC_ASSERT((test_fwd<instantiator, const P>(), true));
    test_fwd<instantiator, const P>();
}
