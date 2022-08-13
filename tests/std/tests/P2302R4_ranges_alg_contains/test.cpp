// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <ranges>
#include <utility>

#include <range_algorithm_support.hpp>

using namespace std;

struct instantiator {
    static constexpr pair<int, int> haystack[3] = {{0, 42}, {2, 42}, {4, 42}};

    template <ranges::input_range In>
    static constexpr void call() {
        using ranges::contains, ranges::begin, ranges::end;

        for (const auto& [value, _] : haystack) {
            { // Validate range overload [found case]
                const same_as<bool> auto result = contains(In{haystack}, value, get_first);
                assert(result);
            }
            { // Validate iterator + sentinel overload [found case]
                const In wrapped{haystack};
                const same_as<bool> auto result = contains(begin(wrapped), end(wrapped), value, get_first);
                assert(result);
            }
        }
        {
            // Validate range overload [not found case]
            const same_as<bool> auto result = contains(In{haystack}, 42, get_first);
            assert(!result);
        }
        {
            // Validate iterator + sentinel overload [not found case]
            const In wrapped{haystack};
            const same_as<bool> auto result = contains(begin(wrapped), end(wrapped), 42, get_first);
            assert(!result);
        }
        { // Validate memchr case
            const char arr[5]{4, 8, 1, -15, 125};

            // found case
            same_as<bool> auto result = contains(arr, 1);
            assert(result);

            // not found case
            result = contains(arr, 10);
            assert(!result);
        }
        { // unreachable_sentinel case
            const In wrapped{haystack};
            const same_as<bool> auto result = contains(begin(wrapped), unreachable_sentinel, 2, get_first);
            assert(result);
        }
    }
};

int main() {
    STATIC_ASSERT((test_in<instantiator, const pair<int, int>>(), true));
    test_in<instantiator, const pair<int, int>>();
}
