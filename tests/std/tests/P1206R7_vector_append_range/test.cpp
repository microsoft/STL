// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <forward_list>
#include <ranges>
#include <span>
#include <type_traits>
#include <utility>
#include <vector>

#include <range_algorithm_support.hpp>

using namespace std;

template <ranges::input_range Rng, size_t N>
constexpr bool test_vector(const size_t capacity, const size_t presize, Rng&& rng, const int (&expected)[N]) {
    // We need to test both reallocating and non-reallocating code paths,
    // so we accept distinct capacity and size values for the initial container.
    vector<int> vec;
    vec.reserve(capacity);
    vec.resize(presize, -1);

    vec.append_range(forward<Rng>(rng));
    const auto length = static_cast<ptrdiff_t>(presize);
    assert(ranges::count(vec | views::take(length), -1) == length);
    assert(ranges::equal(vec | views::drop(length), expected));

    return true;
}

template <ranges::input_range Rng, size_t N>
constexpr bool test_vector(const int (&expected)[N]) {
    test_vector(0, 0, Rng{expected}, expected);
    test_vector(ranges::size(expected) + 2, 2, Rng{expected}, expected);

    return true;
}

static constexpr int some_ints[] = {0, 1, 2, 3, 4, 5, 6, 7};

struct vector_instantiator {
    template <ranges::input_range R>
    static constexpr void call() {
        test_vector<R>(some_ints);
        STATIC_ASSERT(test_vector<R>(some_ints));
    }
};

template <ranges::input_range Rng, size_t N>
constexpr bool test_vector_bool(const size_t capacity, const size_t presize, Rng&& rng, const int (&expected)[N]) {
    // We need to test both reallocating and non-reallocating code paths,
    // so we accept distinct capacity and size values for the initial container.
    vector<bool> vec;
    vec.reserve(capacity);
    vec.resize(presize, true);

    vec.append_range(forward<Rng>(rng));
    const auto length = static_cast<ptrdiff_t>(presize);
    assert(ranges::count(vec | views::take(length), true) == length);
    assert(ranges::equal(vec | views::drop(length), expected, equal_to<bool>{}));

    return true;
}

template <ranges::input_range Rng, size_t N>
constexpr bool test_vector_bool(const int (&expected)[N]) {
    test_vector_bool(0, 0, Rng{expected}, expected);
    test_vector_bool(ranges::size(expected) + 2, 2, Rng{expected}, expected);

    return true;
}

static constexpr int other_ints[80] = { //
    0, 7, 0, 2, 3, 0, 0, 0, 4, 0, 0, 2, 5, 1, 0, 7, //
    6, 3, 0, 7, 0, 5, 7, 0, 3, 4, 5, 0, 3, 0, 0, 2, //
    2, 0, 3, 0, 0, 0, 0, 0, 0, 1, 0, 0, 5, 0, 0, 0, //
    0, 4, 2, 4, 0, 7, 0, 2, 0, 7, 1, 0, 6, 0, 0, 6, //
    3, 5, 0, 7, 0, 7, 0, 0, 2, 3, 5, 2, 2, 0, 7, 3};

struct vector_boo_instantiator {
    template <ranges::input_range R>
    static constexpr void call() {
        test_vector_bool<R>(other_ints);
        STATIC_ASSERT(test_vector_bool<R>(other_ints));
    }
};

template <class Category, test::Common IsCommon, bool is_random = derived_from<Category, random_access_iterator_tag>>
using move_only_view = test::range<Category, const int, test::Sized{is_random}, test::CanDifference{is_random},
    IsCommon, test::CanCompare{derived_from<Category, forward_iterator_tag>},
    test::ProxyRef{!derived_from<Category, contiguous_iterator_tag>}, test::CanView::yes, test::Copyability::move_only>;

constexpr bool test_copyable_view() {
    test_vector<span<const int>>(some_ints);
    test_vector_bool<span<const int>>(other_ints);
    return true;
}

constexpr bool test_move_only_view() {
    test_vector<move_only_view<input_iterator_tag, test::Common::no>>(some_ints);
    test_vector<move_only_view<forward_iterator_tag, test::Common::no>>(some_ints);
    test_vector<move_only_view<forward_iterator_tag, test::Common::yes>>(some_ints);
    test_vector<move_only_view<bidirectional_iterator_tag, test::Common::no>>(some_ints);
    test_vector<move_only_view<bidirectional_iterator_tag, test::Common::yes>>(some_ints);
    test_vector<move_only_view<random_access_iterator_tag, test::Common::no>>(some_ints);
    test_vector<move_only_view<random_access_iterator_tag, test::Common::yes>>(some_ints);

    test_vector_bool<move_only_view<input_iterator_tag, test::Common::no>>(other_ints);
    test_vector_bool<move_only_view<forward_iterator_tag, test::Common::no>>(other_ints);
    test_vector_bool<move_only_view<forward_iterator_tag, test::Common::yes>>(other_ints);
    test_vector_bool<move_only_view<bidirectional_iterator_tag, test::Common::no>>(other_ints);
    test_vector_bool<move_only_view<bidirectional_iterator_tag, test::Common::yes>>(other_ints);
    test_vector_bool<move_only_view<random_access_iterator_tag, test::Common::no>>(other_ints);
    test_vector_bool<move_only_view<random_access_iterator_tag, test::Common::yes>>(other_ints);

    return true;
}

constexpr bool test_c_array() {
    test_vector(0, 0, some_ints, some_ints);
    test_vector_bool(0, 0, other_ints, other_ints);
    return true;
}

constexpr bool test_lvalue_vector() {
    {
        vector vec(from_range, some_ints);
        test_vector(0, 0, vec, some_ints);
    }
    {
        vector vec(from_range, other_ints);
        test_vector_bool(0, 0, vec, other_ints);
    }
    return true;
}

void test_lvalue_forward_list() {
    {
        forward_list lst(ranges::begin(some_ints), ranges::end(some_ints));
        test_vector(0, 0, lst, some_ints);
    }
    {
        forward_list lst(ranges::begin(other_ints), ranges::end(other_ints));
        test_vector_bool(0, 0, lst, other_ints);
    }
}

int main() {
    // Validate views
    test_copyable_view();
    STATIC_ASSERT(test_copyable_view());
    test_move_only_view();
    STATIC_ASSERT(test_move_only_view());

    // Validate non-views
    test_c_array();
    STATIC_ASSERT(test_c_array());
    test_lvalue_vector();
    STATIC_ASSERT(test_lvalue_vector());
    test_lvalue_forward_list();

    test_in<vector_instantiator, const int>();
    test_in<vector_boo_instantiator, const int>();
}
