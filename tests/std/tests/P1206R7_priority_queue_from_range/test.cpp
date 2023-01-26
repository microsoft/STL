// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
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

template <ranges::input_range R, class... Args>
void test_priority_queue(R&& r, Args&&... args) {
    accessible_adaptor<priority_queue<int>> s{from_range, r, forward<Args>(args)...};
    assert(ranges::is_heap(s.get_container()));
}

static constexpr int some_ints[] = {0, 7, 1, 6, 2, 5, 3, 4};

struct instantiator {
    template <ranges::input_range R>
    static void call() {
        test_priority_queue(R{some_ints});
        test_priority_queue(R{some_ints}, less<int>{});
        test_priority_queue(R{some_ints}, less<int>{}, allocator<int>{});
        test_priority_queue(R{some_ints}, allocator<int>{});
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
        test_priority_queue(s);
    }

    { // move-only views
        test_priority_queue(move_only_view<input_iterator_tag, test::Common::no>{some_ints});
        test_priority_queue(move_only_view<forward_iterator_tag, test::Common::no>{some_ints});
        test_priority_queue(move_only_view<forward_iterator_tag, test::Common::yes>{some_ints});
        test_priority_queue(move_only_view<bidirectional_iterator_tag, test::Common::no>{some_ints});
        test_priority_queue(move_only_view<bidirectional_iterator_tag, test::Common::yes>{some_ints});
        test_priority_queue(move_only_view<random_access_iterator_tag, test::Common::no>{some_ints});
        test_priority_queue(move_only_view<random_access_iterator_tag, test::Common::yes>{some_ints});
    }

    // Validate non-views
    test_priority_queue(some_ints); // C array

    { // lvalue vector
        vector vec(ranges::begin(some_ints), ranges::end(some_ints));
        test_priority_queue(vec);
    }

    { // lvalue forward_list
        forward_list lst(ranges::begin(some_ints), ranges::end(some_ints));
        test_priority_queue(lst);
    }

    test_in<instantiator, const int>();
}
