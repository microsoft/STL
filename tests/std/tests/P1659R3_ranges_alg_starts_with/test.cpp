// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <functional>
#include <ranges>
#include <utility>

#include <range_algorithm_support.hpp>

using namespace std;

struct instantiator {
    static constexpr pair<int, int> haystack[]       = {{0, 42}, {2, 42}, {4, 42}};
    static constexpr pair<int, int> short_haystack[] = {{0, 42}};
    static constexpr pair<long, long> needle[]       = {{13, 0}, {13, 2}};
    static constexpr pair<long, long> wrong_needle[] = {{13, 0}, {13, 3}};

    template <ranges::input_range In1, ranges::input_range In2>
    static constexpr void test() {
        using ranges::begin, ranges::end, ranges::equal_to, ranges::starts_with;

        // Validate range overload
        {
            const same_as<bool> auto match = starts_with(In1{haystack}, In2{needle}, equal_to{}, get_first, get_second);
            assert(match);
        }
        {
            const same_as<bool> auto match =
                starts_with(In1{haystack}, In2{wrong_needle}, equal_to{}, get_first, get_second);
            assert(!match);
        }
        {
            const same_as<bool> auto match =
                starts_with(In1{short_haystack}, In2{needle}, equal_to{}, get_first, get_second);
            assert(!match);
        }

        // Validate iterator + sentinel overload
        {
            In1 h{haystack};
            In2 n{needle};
            const same_as<bool> auto match =
                starts_with(begin(h), end(h), begin(n), end(n), equal_to{}, get_first, get_second);
            assert(match);
        }
        {
            In1 h{haystack};
            In2 n{wrong_needle};
            const same_as<bool> auto match =
                starts_with(begin(h), end(h), begin(n), end(n), equal_to{}, get_first, get_second);
            assert(!match);
        }
        {
            In1 h{short_haystack};
            In2 n{needle};
            const same_as<bool> auto match =
                starts_with(begin(h), end(h), begin(n), end(n), equal_to{}, get_first, get_second);
            assert(!match);
        }
    }

    template <ranges::input_range In1, ranges::input_range In2>
    static void call() {
        test<In1, In2>();
        static_assert((test<In1, In2>(), true));
    }
};

int main() {
    { // infinite haystack
        const same_as<bool> auto match = ranges::starts_with(views::iota(0), views::iota(0, 5));
        assert(match);
    }
    { // infinite needle
        const same_as<bool> auto match = ranges::starts_with(views::iota(0, 5), views::iota(0));
        assert(!match);
    }

#if !defined(_PREFAST_) && !defined(__EDG__) // TRANSITION, GH-1030 and GH-3567
    test_in_in<instantiator, const pair<int, int>, const pair<long, long>>();
#endif // ^^^ no workaround ^^^
}
