// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <deque>
#include <forward_list>
#include <queue>
#include <ranges>
#include <span>
#include <type_traits>
#include <utility>
#include <vector>

#include <range_algorithm_support.hpp>

using namespace std;

template <class T>
struct accessible_adaptor : T {
    using T::T;

    auto& get_container() {
        return this->c;
    }
    auto& get_container() const {
        return this->c;
    }
};

template <ranges::input_range Rng>
void test_priority_queue(const size_t presize, Rng&& rng) {
    accessible_adaptor<priority_queue<int>> pq{less<int>{}, vector<int>(presize, -1)};

    pq.push_range(forward<Rng>(rng));
    const auto& container = pq.get_container();
    assert(ranges::is_heap(container));
}

static constexpr int some_ints[] = {0, 7, 1, 6, 2, 5, 3, 4};

template <ranges::input_range Rng>
void test_priority_queue() {
    test_priority_queue(0, Rng{some_ints});
    test_priority_queue(3, Rng{some_ints});
}

struct instantiator {
    template <ranges::input_range R>
    static void call() {
        test_priority_queue<R>();
    }
};

template <class Category, test::Common IsCommon, bool is_random = derived_from<Category, random_access_iterator_tag>>
using move_only_view = test::range<Category, const int, test::Sized{is_random}, test::CanDifference{is_random},
    IsCommon, test::CanCompare{derived_from<Category, forward_iterator_tag>},
    test::ProxyRef{!derived_from<Category, contiguous_iterator_tag>}, test::CanView::yes, test::Copyability::move_only>;

void test_copyable_view() {
    test_priority_queue<span<const int>>();
}

void test_move_only_view() {
    test_priority_queue<move_only_view<input_iterator_tag, test::Common::no>>();
    test_priority_queue<move_only_view<forward_iterator_tag, test::Common::no>>();
    test_priority_queue<move_only_view<forward_iterator_tag, test::Common::yes>>();
    test_priority_queue<move_only_view<bidirectional_iterator_tag, test::Common::no>>();
    test_priority_queue<move_only_view<bidirectional_iterator_tag, test::Common::yes>>();
    test_priority_queue<move_only_view<random_access_iterator_tag, test::Common::no>>();
    test_priority_queue<move_only_view<random_access_iterator_tag, test::Common::yes>>();
}

void test_c_array() {
    test_priority_queue(0, some_ints);
    test_priority_queue(3, some_ints);
}

void test_lvalue_vector() {
    vector vec(from_range, some_ints);
    test_priority_queue(0, vec);
    test_priority_queue(3, vec);
}

void test_lvalue_forward_list() {
    forward_list lst(from_range, some_ints);
    test_priority_queue(0, lst);
    test_priority_queue(3, lst);
}

int main() {
    // Validate views
    test_copyable_view();
    test_move_only_view();

    // Validate non-views
    test_c_array();
    test_lvalue_vector();
    test_lvalue_forward_list();

    test_in<instantiator, const int>();
}
