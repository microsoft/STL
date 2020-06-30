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
    using ranges::find_first_of, ranges::iterator_t, std::array, std::same_as, std::to_address;
    using P = std::pair<int, int>;

    // Validate dangling story
    STATIC_ASSERT(same_as<decltype(find_first_of(borrowed<false>{}, array<int, 42>{})), ranges::dangling>);
    STATIC_ASSERT(same_as<decltype(find_first_of(borrowed<true>{}, array<int, 42>{})), int*>);

    const array pairs = {P{0, 42}, P{1, 42}, P{2, 42}, P{3, 42}, P{4, 42}, P{5, 42}, P{6, 42}};

    const auto pred = [](const int x, const int y) { return x == y + 1; };

    const array good_needle = {29, 1};
    {
        // Validate range overload [found case]
        auto result = find_first_of(basic_borrowed_range{pairs}, good_needle, pred, get_first);
        STATIC_ASSERT(same_as<decltype(result), iterator_t<basic_borrowed_range<const P>>>);
        assert(to_address(std::move(result).base()) == pairs.data() + 2);
    }
    {
        // Validate iterator + sentinel overload [found case]
        basic_borrowed_range wrapped_pairs{pairs};
        auto result = find_first_of(
            wrapped_pairs.begin(), wrapped_pairs.end(), good_needle.begin(), good_needle.end(), pred, get_first);
        STATIC_ASSERT(same_as<decltype(result), iterator_t<basic_borrowed_range<const P>>>);
        assert(to_address(std::move(result).base()) == pairs.data() + 2);
    }

    const array bad_needle = {29, 17};
    {
        // Validate range overload [not found case]
        auto result = find_first_of(basic_borrowed_range{pairs}, bad_needle, pred, get_first);
        STATIC_ASSERT(same_as<decltype(result), iterator_t<basic_borrowed_range<const P>>>);
        assert(to_address(std::move(result).base()) == pairs.data() + pairs.size());
    }
    {
        // Validate iterator + sentinel overload [not found case]
        basic_borrowed_range wrapped_pairs{pairs};
        auto result = find_first_of(
            wrapped_pairs.begin(), wrapped_pairs.end(), bad_needle.begin(), bad_needle.end(), pred, get_first);
        STATIC_ASSERT(same_as<decltype(result), iterator_t<basic_borrowed_range<const P>>>);
        assert(to_address(std::move(result).base()) == pairs.data() + pairs.size());
    }
}

int main() {
    STATIC_ASSERT((smoke_test(), true));
    smoke_test();
}

struct instantiator {
    template <class In1, class Fwd2>
    static void call(In1&& in1 = {}, Fwd2&& fwd2 = {}) {
        if constexpr (!is_permissive) { // These fail to compile in C1XX's permissive mode due to VSO-566808
            using ranges::iterator_t;

            (void) ranges::find_first_of(in1, fwd2);
            (void) ranges::find_first_of(ranges::begin(in1), ranges::end(in1), ranges::begin(fwd2), ranges::end(fwd2));

            BinaryPredicateFor<iterator_t<In1>, iterator_t<Fwd2>> pred{};
            (void) ranges::find_first_of(in1, fwd2, pred);
            (void) ranges::find_first_of(
                ranges::begin(in1), ranges::end(in1), ranges::begin(fwd2), ranges::end(fwd2), pred);

            HalfProjectedBinaryPredicateFor<iterator_t<Fwd2>> halfpred{};
            ProjectionFor<iterator_t<In1>> halfproj{};
            (void) ranges::find_first_of(in1, fwd2, halfpred, halfproj);
            (void) ranges::find_first_of(
                ranges::begin(in1), ranges::end(in1), ranges::begin(fwd2), ranges::end(fwd2), halfpred, halfproj);

            ProjectedBinaryPredicate<0, 1> projpred{};
            ProjectionFor<iterator_t<In1>, 0> proj1{};
            ProjectionFor<iterator_t<Fwd2>, 1> proj2{};
            (void) ranges::find_first_of(in1, fwd2, projpred, proj1, proj2);
            (void) ranges::find_first_of(
                ranges::begin(in1), ranges::end(in1), ranges::begin(fwd2), ranges::end(fwd2), projpred, proj1, proj2);
        }
    }
};

template void test_in_fwd<instantiator>();
