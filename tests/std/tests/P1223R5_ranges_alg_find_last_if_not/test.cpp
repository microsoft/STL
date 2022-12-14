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

constexpr auto not_matches = [](const int val) { return val != 42; };
constexpr auto not_equals  = [](auto x) { return [x](auto&& y) { return y != x; }; };

// Validate dangling story
STATIC_ASSERT(same_as<decltype(ranges::find_last_if_not(borrowed<false>{}, not_matches)), ranges::dangling>);
STATIC_ASSERT(same_as<decltype(ranges::find_last_if_not(borrowed<true>{}, not_matches)), ranges::subrange<int*>>);

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
        using ranges::find_last_if_not, ranges::common_range, ranges::iterator_t;

        for (const auto& [value, _] : haystack) {
            { // Validate range overload [found case]
                Read wrapped_input{haystack};
                const auto result = find_last_if_not(wrapped_input, not_equals(value), get_first);
                STATIC_ASSERT(same_as<decltype(result), const ranges::subrange<iterator_t<Read>>>);
                check_value(result.front(), value);
            }
            { // Validate iterator + sentinel overload [found case]
                Read wrapped_input{haystack};
                const auto result =
                    find_last_if_not(wrapped_input.begin(), wrapped_input.end(), not_equals(value), get_first);
                STATIC_ASSERT(same_as<decltype(result), const ranges::subrange<iterator_t<Read>>>);
                check_value(result.front(), value);
            }
        }
        { // Validate range overload [not found case]
            Read wrapped_input{haystack};
            auto result = find_last_if_not(wrapped_input, not_equals(42), get_first);
            STATIC_ASSERT(same_as<iterator_t<decltype(result)>, iterator_t<Read>>);
            assert(result.begin() == wrapped_input.end());
        }
        { // Validate iterator + sentinel overload [not found case]
            Read wrapped_input{haystack};
            auto result = find_last_if_not(wrapped_input.begin(), wrapped_input.end(), not_equals(42), get_first);
            STATIC_ASSERT(same_as<iterator_t<decltype(result)>, iterator_t<Read>>);
            assert(result.begin() == wrapped_input.end());
        }
        { // Validate empty range
            Read wrapped_empty{span<P, 0>{}};
            auto iter_result  = find_last_if_not(wrapped_empty.begin(), wrapped_empty.end(), not_equals(0), get_first);
            auto range_result = find_last_if_not(wrapped_empty, not_equals(0), get_first);
            assert(iter_result.begin() == wrapped_empty.end());
            assert(range_result.begin() == wrapped_empty.end());
        }
    }
};

int main() {
    STATIC_ASSERT((test_fwd<instantiator, const P>(), true));
    test_fwd<instantiator, const P>();
}
