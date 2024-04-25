// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <ranges>
#include <span>
#include <utility>

#include <range_algorithm_support.hpp>
using namespace std;
using P = pair<int, int>;

constexpr auto matches = [](const int val) { return val == 42; };
constexpr auto equals  = [](auto x) { return [x](auto&& y) { return y == x; }; };

// Validate dangling story
static_assert(same_as<decltype(ranges::find_last_if(borrowed<false>{}, matches)), ranges::dangling>);
static_assert(same_as<decltype(ranges::find_last_if(borrowed<true>{}, matches)), ranges::subrange<int*>>);

template <class T, class U>
constexpr void check_value(const T& found, const U& value) {
    if constexpr (same_as<T, P>) {
        assert(found.first == value);
        assert(found.second == 1729);
    } else {
        assert(found.peek().first == value);
        assert(found.peek().second == 1729);
    }
}

struct instantiator {
    static constexpr P haystack[6] = {{0, 42}, {2, 42}, {4, 42}, {0, 1729}, {2, 1729}, {4, 1729}};

    template <ranges::forward_range Read>
    static constexpr void call() {
        using ranges::find_last_if, ranges::common_range, ranges::iterator_t;

        for (const auto& [value, _] : haystack) {
            { // Validate range overload [found case]
                Read wrapped_input{haystack};
                const auto result = find_last_if(wrapped_input, equals(value), get_first);
                static_assert(same_as<decltype(result), const ranges::subrange<iterator_t<Read>>>);
                check_value(result.front(), value);
            }
            { // Validate iterator + sentinel overload [found case]
                Read wrapped_input{haystack};
                const auto result = find_last_if(wrapped_input.begin(), wrapped_input.end(), equals(value), get_first);
                static_assert(same_as<decltype(result), const ranges::subrange<iterator_t<Read>>>);
                check_value(result.front(), value);
            }
        }
        { // Validate range overload [not found case]
            Read wrapped_input{haystack};
            auto result = find_last_if(wrapped_input, equals(42), get_first);
            static_assert(same_as<iterator_t<decltype(result)>, iterator_t<Read>>);
            assert(result.begin() == wrapped_input.end());
        }
        { // Validate iterator + sentinel overload [not found case]
            Read wrapped_input{haystack};
            auto result = find_last_if(wrapped_input.begin(), wrapped_input.end(), equals(42), get_first);
            static_assert(same_as<iterator_t<decltype(result)>, iterator_t<Read>>);
            assert(result.begin() == wrapped_input.end());
        }
        { // Validate empty range
            Read wrapped_empty{span<P, 0>{}};
            auto iter_result  = find_last_if(wrapped_empty.begin(), wrapped_empty.end(), equals(0), get_first);
            auto range_result = find_last_if(wrapped_empty, equals(0), get_first);
            assert(iter_result.begin() == wrapped_empty.end());
            assert(range_result.begin() == wrapped_empty.end());
        }
    }
};

int main() {
    static_assert((test_fwd<instantiator, const P>(), true));
    test_fwd<instantiator, const P>();
}
