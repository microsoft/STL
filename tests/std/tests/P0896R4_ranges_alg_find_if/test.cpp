// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <concepts>
#include <ranges>
#include <utility>

#include <range_algorithm_support.hpp>

constexpr void smoke_test() {
    using ranges::find_if, ranges::iterator_t, std::same_as;
    using P = std::pair<int, int>;

    std::array<P, 3> const pairs = {{{0, 42}, {2, 42}, {4, 42}}};
    constexpr auto equals        = [](auto x) { return [x](auto&& y) { return y == x; }; };

    // Validate dangling story
    STATIC_ASSERT(same_as<decltype(find_if(borrowed<false>{}, equals(42))), ranges::dangling>);
    STATIC_ASSERT(same_as<decltype(find_if(borrowed<true>{}, equals(42))), int*>);

    for (auto [value, _] : pairs) {
        {
            // Validate range overload [found case]
            auto result = find_if(basic_borrowed_range{pairs}, equals(value), get_first);
            STATIC_ASSERT(same_as<decltype(result), iterator_t<basic_borrowed_range<P const>>>);
            assert((*result).first == value);
        }
        {
            // Validate iterator + sentinel overload [found case]
            basic_borrowed_range wrapped_pairs{pairs};
            auto result = find_if(wrapped_pairs.begin(), wrapped_pairs.end(), equals(value), get_first);
            STATIC_ASSERT(same_as<decltype(result), iterator_t<basic_borrowed_range<P const>>>);
            assert((*result).first == value);
        }
    }
    {
        // Validate range overload [not found case]
        auto result = find_if(basic_borrowed_range{pairs}, equals(42), get_first);
        STATIC_ASSERT(same_as<decltype(result), iterator_t<basic_borrowed_range<P const>>>);
        assert(result == basic_borrowed_range{pairs}.end());
    }
    {
        // Validate iterator + sentinel overload [not found case]
        basic_borrowed_range wrapped_pairs{pairs};
        auto result = find_if(wrapped_pairs.begin(), wrapped_pairs.end(), equals(42), get_first);
        STATIC_ASSERT(same_as<decltype(result), iterator_t<basic_borrowed_range<P const>>>);
        assert(result == wrapped_pairs.end());
    }
}

int main() {
    STATIC_ASSERT((smoke_test(), true));
    smoke_test();
}

struct instantiator {
    template <class In>
    static void call(In&& in = {}) {
        using ranges::iterator_t;
        using I = iterator_t<In>;

        (void) ranges::find_if(in, UnaryPredicateFor<I>{});
        (void) ranges::find_if(in, ProjectedUnaryPredicate<>{}, ProjectionFor<I>{});
    }
};

template void test_in<instantiator>();
