// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <forward_list>
#include <ranges>
#include <span>
#include <type_traits>
#include <unordered_set>
#include <utility>
#include <vector>

#include <range_algorithm_support.hpp>

using namespace std;

static constexpr int expected_vals[] = {0, 1, 2, 3, 4, 5, 6, 7};

template <ranges::input_range Rng>
void test_unordered_set(Rng&& rng) {
    if constexpr (is_reference_v<ranges::range_reference_t<Rng>>) {
        unordered_set<int> m;
        m.insert_range(forward<Rng>(rng));
        unordered_set<int> e{begin(expected_vals), end(expected_vals)};
        assert(m == e);
    }
}

static constexpr int some_ints[] = {4, 2, 3, 4, 0, 5, 4, 6, 7, 1};

struct unordered_set_instantiator {
    template <ranges::input_range R>
    static void call() {
        test_unordered_set(R{some_ints});
    }
};

template <class Category, test::Common IsCommon, bool is_random = derived_from<Category, random_access_iterator_tag>>
using move_only_view = test::range<Category, const int, test::Sized{is_random}, test::CanDifference{is_random},
    IsCommon, test::CanCompare{derived_from<Category, forward_iterator_tag>}, test::ProxyRef::no, test::CanView::yes,
    test::Copyability::move_only>;

void test_copyable_views() {
    constexpr span<const int> s{some_ints};
    test_unordered_set(s);
}

void test_move_only_views() {
    test_unordered_set(move_only_view<input_iterator_tag, test::Common::no>{some_ints});
    test_unordered_set(move_only_view<forward_iterator_tag, test::Common::no>{some_ints});
    test_unordered_set(move_only_view<forward_iterator_tag, test::Common::yes>{some_ints});
    test_unordered_set(move_only_view<bidirectional_iterator_tag, test::Common::no>{some_ints});
    test_unordered_set(move_only_view<bidirectional_iterator_tag, test::Common::yes>{some_ints});
    test_unordered_set(move_only_view<random_access_iterator_tag, test::Common::no>{some_ints});
    test_unordered_set(move_only_view<random_access_iterator_tag, test::Common::yes>{some_ints});
}

void test_c_array() {
    test_unordered_set(some_ints);
}

void test_lvalue_vector() {
    vector vec(ranges::begin(some_ints), ranges::end(some_ints));
    test_unordered_set(vec);
}

void test_lvalue_forward_list() {
    forward_list lst(ranges::begin(some_ints), ranges::end(some_ints));
    test_unordered_set(lst);
}

int main() {
    // Validate views
    test_copyable_views();
    test_move_only_views();

    // Validate non-views
    test_c_array();
    test_lvalue_vector();
    test_lvalue_forward_list();

    test_in<unordered_set_instantiator, const int>();
}
