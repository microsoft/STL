// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <concepts>
#include <ranges>
#include <span>
#include <utility>

#include <range_algorithm_support.hpp>

constexpr void smoke_test() {
    using ranges::adjacent_find, ranges::iterator_t, std::same_as;
    using P = std::pair<int, int>;

    // Validate dangling story
    static_assert(same_as<decltype(adjacent_find(borrowed<false>{}, ranges::equal_to{})), ranges::dangling>);
    static_assert(same_as<decltype(adjacent_find(borrowed<true>{}, ranges::equal_to{})), int*>);

    const std::array pairs = {P{0, 42}, P{1, 42}, P{2, 42}, P{4, 42}, P{5, 42}};
    const auto pred        = [](const int x, const int y) { return y - x > 1; };

    {
        // Validate range overload [found case]
        const auto result = adjacent_find(pairs, pred, get_first);
        static_assert(same_as<decltype(result), const iterator_t<decltype(pairs)>>);
        assert(result == pairs.begin() + 2);
    }
    {
        // Validate iterator + sentinel overload [found case]
        const auto result = adjacent_find(pairs.begin(), pairs.end(), pred, get_first);
        static_assert(same_as<decltype(result), const iterator_t<decltype(pairs)>>);
        assert(result == pairs.begin() + 2);
    }
    {
        // Validate range overload [not found case]
        const auto result = adjacent_find(pairs, ranges::equal_to{}, get_first);
        static_assert(same_as<decltype(result), const iterator_t<decltype(pairs)>>);
        assert(result == pairs.end());
    }
    {
        // Validate iterator + sentinel overload [not found case]
        const auto result = adjacent_find(pairs.begin(), pairs.end(), ranges::equal_to{}, get_first);
        static_assert(same_as<decltype(result), const iterator_t<decltype(pairs)>>);
        assert(result == pairs.end());
    }
}

int main() {
    static_assert((smoke_test(), true));
    smoke_test();
}

struct instantiator {
    static constexpr int some_ints[] = {1, 2, 3};
    template <class Fwd>
    static void call() {
        Fwd fwd{std::span{some_ints}};
        using ranges::adjacent_find, ranges::iterator_t;

        (void) adjacent_find(fwd);
        (void) adjacent_find(begin(fwd), end(fwd));

        BinaryPredicateFor<iterator_t<Fwd>, iterator_t<Fwd>> pred{};
        (void) adjacent_find(fwd, pred);
        (void) adjacent_find(begin(fwd), end(fwd), pred);

        ProjectedBinaryPredicate<0, 0> projpred{};
        ProjectionFor<iterator_t<Fwd>, 0> proj{};
        (void) adjacent_find(fwd, projpred, proj);
        (void) adjacent_find(begin(fwd), end(fwd), projpred, proj);
    }
};

template void test_fwd<instantiator, const int>();
