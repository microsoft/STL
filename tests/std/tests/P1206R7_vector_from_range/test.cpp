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
constexpr bool test_vector(Rng&& rng, Expected&& expected) {
    vector<int> vec(from_range, forward<Rng>(rng));
    assert(ranges::equal(vec, expected));

    return true;
}

static constexpr int some_ints[] = {0, 1, 2, 3, 4, 5, 6, 7};

struct vector_instantiator {
    template <ranges::input_range R>
    static void call() {
        test_vector(R{some_ints}, some_ints);
        STATIC_ASSERT(test_vector(R{some_ints}, some_ints));
    }
};

template <ranges::input_range Rng, ranges::random_access_range Expected>
constexpr bool test_vector_bool(Rng&& rng, Expected&& expected) {
    vector<bool> vec(from_range, forward<Rng>(rng));
    assert(ranges::equal(vec, expected, equal_to<bool>{}));

    return true;
}

static constexpr int other_ints[80] = { //
    0, 7, 0, 2, 3, 0, 0, 0, 4, 0, 0, 2, 5, 1, 0, 7, //
    6, 3, 0, 7, 0, 5, 7, 0, 3, 4, 5, 0, 3, 0, 0, 2, //
    2, 0, 3, 0, 0, 0, 0, 0, 0, 1, 0, 0, 5, 0, 0, 0, //
    0, 4, 2, 4, 0, 7, 0, 2, 0, 7, 1, 0, 6, 0, 0, 6, //
    3, 5, 0, 7, 0, 7, 0, 0, 2, 3, 5, 2, 2, 0, 7, 3};
static constexpr auto first64_ints  = span{other_ints}.first<64>();

struct vector_boo_instantiator {
    template <ranges::input_range R>
    static void call() {
        test_vector_bool(R{other_ints}, other_ints);
        STATIC_ASSERT(test_vector_bool(R{other_ints}, other_ints));
        test_vector_bool(R{first64_ints}, first64_ints);
        STATIC_ASSERT(test_vector_bool(R{first64_ints}, first64_ints));
    }
};

template <class Category, test::Common IsCommon, bool is_random = derived_from<Category, random_access_iterator_tag>>
using move_only_view = test::range<Category, const int, test::Sized{is_random}, test::CanDifference{is_random},
    IsCommon, test::CanCompare{derived_from<Category, forward_iterator_tag>},
    test::ProxyRef{!derived_from<Category, contiguous_iterator_tag>}, test::CanView::yes, test::Copyability::move_only>;

constexpr bool test_copyable_views() {
    {
        constexpr span<const int> s{some_ints};
        test_vector(s, some_ints);
    }
    {
        constexpr span<const int> s{other_ints};
        test_vector_bool(s, other_ints);
        test_vector_bool(first64_ints, first64_ints);
    }
    return true;
}

constexpr bool test_move_only_views() {
    test_vector(move_only_view<input_iterator_tag, test::Common::no>{some_ints}, some_ints);
    test_vector(move_only_view<forward_iterator_tag, test::Common::no>{some_ints}, some_ints);
    test_vector(move_only_view<forward_iterator_tag, test::Common::yes>{some_ints}, some_ints);
    test_vector(move_only_view<bidirectional_iterator_tag, test::Common::no>{some_ints}, some_ints);
    test_vector(move_only_view<bidirectional_iterator_tag, test::Common::yes>{some_ints}, some_ints);
    test_vector(move_only_view<random_access_iterator_tag, test::Common::no>{some_ints}, some_ints);
    test_vector(move_only_view<random_access_iterator_tag, test::Common::yes>{some_ints}, some_ints);

    test_vector_bool(move_only_view<input_iterator_tag, test::Common::no>{other_ints}, other_ints);
    test_vector_bool(move_only_view<forward_iterator_tag, test::Common::no>{other_ints}, other_ints);
    test_vector_bool(move_only_view<forward_iterator_tag, test::Common::yes>{other_ints}, other_ints);
    test_vector_bool(move_only_view<bidirectional_iterator_tag, test::Common::no>{other_ints}, other_ints);
    test_vector_bool(move_only_view<bidirectional_iterator_tag, test::Common::yes>{other_ints}, other_ints);
    test_vector_bool(move_only_view<random_access_iterator_tag, test::Common::no>{other_ints}, other_ints);
    test_vector_bool(move_only_view<random_access_iterator_tag, test::Common::yes>{other_ints}, other_ints);
    return true;
}

constexpr bool test_c_array() {
    test_vector(some_ints, some_ints);
    test_vector_bool(other_ints, other_ints);
    return true;
}

constexpr bool test_lvalue_vector() {
    {
        vector vec(ranges::begin(some_ints), ranges::end(some_ints));
        test_vector(vec, some_ints);
    }
    {
        vector vec(ranges::begin(other_ints), ranges::end(other_ints));
        test_vector_bool(vec, other_ints);
    }
    return true;
}

void test_lvalue_forward_list() {
    {
        forward_list lst(ranges::begin(some_ints), ranges::end(some_ints));
        test_vector(lst, some_ints);
    }
    {
        forward_list lst(ranges::begin(other_ints), ranges::end(other_ints));
        test_vector_bool(lst, other_ints);
    }
}

struct counted_item {
    static int count;

    counted_item() {
        ++count;
    }

    counted_item(const counted_item&) {
        ++count;
    }

    counted_item& operator=(const counted_item&) = default;

    ~counted_item() {
        --count;
    }
};

int counted_item::count = 0;

static_assert(!is_trivially_copy_constructible_v<counted_item>);
static_assert(is_trivially_copy_assignable_v<counted_item>);

void test_vso1591034() {
    // _Uninitialized_copy was incorrectly using memmove for types that are
    // trivially assignable but not trivially constructible.

    counted_item::count = 0;
    vector<counted_item> vec;

    for (int j = 0; j != 6; ++j) {
        vec.push_back(counted_item());
    }

    assert(counted_item::count == 6);
}

void test_LWG_3743() { // COMPILE-ONLY
    ranges::subrange r(views::iota(0ULL) | views::take(5), 5);
    (void) ranges::to<vector<unsigned long long>>(r, size_t{0});
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

    test_vso1591034();
}
