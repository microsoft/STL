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
constexpr bool test_vector(const size_t presize, Rng&& rng, Expected&& expected) {
    vector<int> vec(presize, -1);
    const same_as<vector<int>::iterator> auto result =
        vec.insert_range(ranges::next(vec.begin(), presize != 0), forward<Rng>(rng));
    assert(result == ranges::next(vec.begin(), presize != 0));
    assert(ranges::equal(vec, expected));

    return true;
}

static constexpr int some_ints[]    = {0, 1, 2, 3, 4, 5, 6, 7};
static constexpr int short_result[] = {
    -1, 0, 1, 2, 3, 4, 5, 6, 7, -1, -1, -1, -1}; // result of inserting some_ints at offset 1 in a sequence of 5 -1s
static constexpr int long_result[] = {
    -1, 0, 1, 2, 3, 4, 5, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}; // ditto, in a sequence of 11 -1s

struct vector_instantiator {
    template <ranges::input_range R>
    static void call() {
        test_vector(0, R{some_ints}, some_ints);
        STATIC_ASSERT(test_vector(0, R{some_ints}, some_ints));
        test_vector(5, R{some_ints}, short_result);
        STATIC_ASSERT(test_vector(5, R{some_ints}, short_result));
        test_vector(11, R{some_ints}, long_result);
        STATIC_ASSERT(test_vector(11, R{some_ints}, long_result));
    }
};

template <ranges::input_range Rng, ranges::random_access_range Expected>
constexpr bool test_vector_bool(const size_t presize, Rng&& rng, Expected&& expected) {
    vector<bool> vec(presize, true);
    const same_as<vector<bool>::iterator> auto result =
        vec.insert_range(ranges::next(vec.begin(), presize != 0), forward<Rng>(rng));
    assert(result == ranges::next(vec.begin(), presize != 0));
    assert(ranges::equal(vec, expected, equal_to<bool>{}));

    return true;
}

static constexpr int other_ints[80]        = { //
    0, 7, 0, 2, 3, 0, 0, 0, 4, 0, 0, 2, 5, 1, 0, 7, //
    6, 3, 0, 7, 0, 5, 7, 0, 3, 4, 5, 0, 3, 0, 0, 2, //
    2, 0, 3, 0, 0, 0, 0, 0, 0, 1, 0, 0, 5, 0, 0, 0, //
    0, 4, 2, 4, 0, 7, 0, 2, 0, 7, 1, 0, 6, 0, 0, 6, //
    3, 5, 0, 7, 0, 7, 0, 0, 2, 3, 5, 2, 2, 0, 7, 3};
static constexpr bool other_short_result[] = {true, false, true, true, true, true, true, true, true, true, true, true,
    true}; // result of inserting other_ints at offset 1 in a sequence of 5 trues
static constexpr bool other_long_result[]  = {true, false, true, true, true, true, true, true, true, true, true, true,
     true, true, true, true, true, true, true}; // ditto, in a sequence of 11 trues

struct vector_boo_instantiator {
    template <ranges::input_range R>
    static void call() {
        test_vector_bool(0, R{some_ints}, some_ints);
        STATIC_ASSERT(test_vector_bool(0, R{some_ints}, some_ints));
        test_vector_bool(5, R{some_ints}, other_short_result);
        STATIC_ASSERT(test_vector_bool(5, R{some_ints}, other_short_result));
        test_vector_bool(11, R{some_ints}, other_long_result);
        STATIC_ASSERT(test_vector_bool(11, R{some_ints}, other_long_result));
    }
};

template <class Category, test::Common IsCommon, bool is_random = derived_from<Category, random_access_iterator_tag>>
using move_only_view = test::range<Category, const int, test::Sized{is_random}, test::CanDifference{is_random},
    IsCommon, test::CanCompare{derived_from<Category, forward_iterator_tag>},
    test::ProxyRef{!derived_from<Category, contiguous_iterator_tag>}, test::CanView::yes, test::Copyability::move_only>;

constexpr bool test_copyable_views() {
    {
        constexpr span<const int> s{some_ints};
        test_vector(0, s, some_ints);
    }
    {
        constexpr span<const int> s{other_ints};
        test_vector_bool(0, s, other_ints);
    }

    return true;
}

constexpr bool test_move_only_views() {
    {
        test_vector(0, move_only_view<input_iterator_tag, test::Common::no>{some_ints}, some_ints);
        test_vector(0, move_only_view<forward_iterator_tag, test::Common::no>{some_ints}, some_ints);
        test_vector(0, move_only_view<forward_iterator_tag, test::Common::yes>{some_ints}, some_ints);
        test_vector(0, move_only_view<bidirectional_iterator_tag, test::Common::no>{some_ints}, some_ints);
        test_vector(0, move_only_view<bidirectional_iterator_tag, test::Common::yes>{some_ints}, some_ints);
        test_vector(0, move_only_view<random_access_iterator_tag, test::Common::no>{some_ints}, some_ints);
        test_vector(0, move_only_view<random_access_iterator_tag, test::Common::yes>{some_ints}, some_ints);
    }
    {
        test_vector_bool(0, move_only_view<input_iterator_tag, test::Common::no>{other_ints}, other_ints);
        test_vector_bool(0, move_only_view<forward_iterator_tag, test::Common::no>{other_ints}, other_ints);
        test_vector_bool(0, move_only_view<forward_iterator_tag, test::Common::yes>{other_ints}, other_ints);
        test_vector_bool(0, move_only_view<bidirectional_iterator_tag, test::Common::no>{other_ints}, other_ints);
        test_vector_bool(0, move_only_view<bidirectional_iterator_tag, test::Common::yes>{other_ints}, other_ints);
        test_vector_bool(0, move_only_view<random_access_iterator_tag, test::Common::no>{other_ints}, other_ints);
        test_vector_bool(0, move_only_view<random_access_iterator_tag, test::Common::yes>{other_ints}, other_ints);
    }

    return true;
}

constexpr bool test_c_array() {
    test_vector(0, some_ints, some_ints);
    test_vector_bool(0, other_ints, other_ints);
    return true;
}

constexpr bool test_lvalue_vector() {
    {
        vector vec(ranges::begin(some_ints), ranges::end(some_ints));
        test_vector(0, vec, some_ints);
    }
    {
        vector vec(ranges::begin(other_ints), ranges::end(other_ints));
        test_vector_bool(0, vec, other_ints);
    }
    return true;
}

void test_lvalue_forward_list() {
    {
        forward_list lst(ranges::begin(some_ints), ranges::end(some_ints));
        test_vector(0, lst, some_ints);
    }
    {
        forward_list lst(ranges::begin(other_ints), ranges::end(other_ints));
        test_vector_bool(0, lst, other_ints);
    }
}

int main() {
    // Validate views
    test_copyable_views();
    STATIC_ASSERT(test_copyable_views());
    test_move_only_views();
    STATIC_ASSERT(test_move_only_views());

    // Validate non-views
    test_c_array();
    STATIC_ASSERT(test_c_array());
    test_lvalue_vector();
    STATIC_ASSERT(test_lvalue_vector());
    test_lvalue_forward_list();

    test_in<vector_instantiator, const int>();
    test_in<vector_boo_instantiator, const int>();
}
