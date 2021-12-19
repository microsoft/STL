// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <forward_list>
#include <ranges>
#include <span>
#include <type_traits>
#include <utility>
#include <vector>

#include <range_algorithm_support.hpp>

using namespace std;

template <ranges::input_range Rng, ranges::random_access_range Expected>
void test_flist(Rng&& rng, Expected&& expected) {
    forward_list<int> fl{from_range, forward<Rng>(rng)};
    static_assert(same_as<decltype(fl), forward_list<int>>);
    assert(ranges::equal(fl, expected));
}

static constexpr int some_ints[] = {0, 1, 2, 3, 4, 5, 6, 7};

struct instantiator {
    template <ranges::input_range R>
    static void call() {
        test_flist(R{some_ints}, some_ints);
    }
};

template <class Category, test::Common IsCommon, bool is_random = derived_from<Category, random_access_iterator_tag>>
using move_only_view = test::range<Category, const int, test::Sized{is_random}, test::CanDifference{is_random},
    IsCommon, test::CanCompare{derived_from<Category, forward_iterator_tag>},
    test::ProxyRef{!derived_from<Category, contiguous_iterator_tag>}, test::CanView::yes, test::Copyability::move_only>;

int main() {
    // Validate views
    { // copyable views
        constexpr span<const int> s{some_ints};
        test_flist(s, some_ints);
    }

    { // move-only views
        test_flist(move_only_view<input_iterator_tag, test::Common::no>{some_ints}, some_ints);
        test_flist(move_only_view<forward_iterator_tag, test::Common::no>{some_ints}, some_ints);
        test_flist(move_only_view<forward_iterator_tag, test::Common::yes>{some_ints}, some_ints);
        test_flist(move_only_view<bidirectional_iterator_tag, test::Common::no>{some_ints}, some_ints);
        test_flist(move_only_view<bidirectional_iterator_tag, test::Common::yes>{some_ints}, some_ints);
        test_flist(move_only_view<random_access_iterator_tag, test::Common::no>{some_ints}, some_ints);
        test_flist(move_only_view<random_access_iterator_tag, test::Common::yes>{some_ints}, some_ints);
    }

    // Validate non-views
    test_flist(some_ints, some_ints); // C array

    { // lvalue vector
        vector vec(ranges::begin(some_ints), ranges::end(some_ints));
        test_flist(vec, some_ints);
    }

    { // lvalue forward_list
        forward_list lst(ranges::begin(some_ints), ranges::end(some_ints));
        test_flist(lst, some_ints);
    }

    test_in<instantiator, const int>();
}
