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
void test_flist(const size_t presize, Rng&& rng, Expected&& expected) {
    forward_list<int> fl(presize, -1);
    const same_as<forward_list<int>::iterator> auto result =
        fl.insert_range_after(ranges::next(fl.before_begin(), presize != 0), forward<Rng>(rng));
    assert(result == ranges::next(fl.before_begin(), 8 + (presize != 0)));
    assert(ranges::equal(fl, expected));
}

static constexpr int some_ints[]    = {0, 1, 2, 3, 4, 5, 6, 7};
static constexpr int short_result[] = {
    -1, 0, 1, 2, 3, 4, 5, 6, 7, -1, -1, -1, -1}; // result of inserting some_ints at offset 1 in a sequence of 5 -1s
static constexpr int long_result[] = {
    -1, 0, 1, 2, 3, 4, 5, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}; // ditto, in a sequence of 11 -1s

struct instantiator {
    template <ranges::input_range R>
    static void call() {
        test_flist(0, R{some_ints}, some_ints);
        test_flist(5, R{some_ints}, short_result);
        test_flist(11, R{some_ints}, long_result);
    }
};

template <class Category, test::Common IsCommon, bool is_random = derived_from<Category, random_access_iterator_tag>>
using move_only_view = test::range<Category, const int, test::Sized{is_random}, test::CanDifference{is_random},
    IsCommon, test::CanCompare{derived_from<Category, forward_iterator_tag>},
    test::ProxyRef{!derived_from<Category, contiguous_iterator_tag>}, test::CanView::yes, test::Copyability::move_only>;

void test_copyable_views() {
    constexpr span<const int> s{some_ints};
    test_flist(0, s, some_ints);
}

void test_move_only_views() {
    test_flist(0, move_only_view<input_iterator_tag, test::Common::no>{some_ints}, some_ints);
    test_flist(0, move_only_view<forward_iterator_tag, test::Common::no>{some_ints}, some_ints);
    test_flist(0, move_only_view<forward_iterator_tag, test::Common::yes>{some_ints}, some_ints);
    test_flist(0, move_only_view<bidirectional_iterator_tag, test::Common::no>{some_ints}, some_ints);
    test_flist(0, move_only_view<bidirectional_iterator_tag, test::Common::yes>{some_ints}, some_ints);
    test_flist(0, move_only_view<random_access_iterator_tag, test::Common::no>{some_ints}, some_ints);
    test_flist(0, move_only_view<random_access_iterator_tag, test::Common::yes>{some_ints}, some_ints);
}

void test_c_array() {
    test_flist(0, some_ints, some_ints);
}

void test_lvalue_vector() {
    vector vec(ranges::begin(some_ints), ranges::end(some_ints));
    test_flist(0, vec, some_ints);
}

void test_lvalue_forward_list() {
    forward_list lst(ranges::begin(some_ints), ranges::end(some_ints));
    test_flist(0, lst, some_ints);
}

int main() {
    // Validate views
    test_copyable_views();
    test_move_only_views();

    // Validate non-views
    test_c_array();
    test_lvalue_vector();
    test_lvalue_forward_list();

    test_in<instantiator, const int>();
}
