// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <forward_list>
#include <new>
#include <ranges>
#include <span>
#include <stack>
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

static constexpr int some_ints[] = {0, 1, 2, 3, 4, 5, 6, 7};

template <ranges::input_range R, class... Args>
void test_stack(R&& r, Args&&... args) {
    accessible_adaptor<stack<int>> s{from_range, r, forward<Args>(args)...};
    assert(ranges::equal(s.get_container(), some_ints));
}

struct instantiator {
    template <ranges::input_range R>
    static void call() {
        test_stack(R{some_ints});
        test_stack(R{some_ints}, allocator<int>{});
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
        test_stack(s);
    }

    { // move-only views
        test_stack(move_only_view<input_iterator_tag, test::Common::no>{some_ints});
        test_stack(move_only_view<forward_iterator_tag, test::Common::no>{some_ints});
        test_stack(move_only_view<forward_iterator_tag, test::Common::yes>{some_ints});
        test_stack(move_only_view<bidirectional_iterator_tag, test::Common::no>{some_ints});
        test_stack(move_only_view<bidirectional_iterator_tag, test::Common::yes>{some_ints});
        test_stack(move_only_view<random_access_iterator_tag, test::Common::no>{some_ints});
        test_stack(move_only_view<random_access_iterator_tag, test::Common::yes>{some_ints});
    }

    // Validate non-views
    test_stack(some_ints); // C array

    { // lvalue vector
        vector vec(ranges::begin(some_ints), ranges::end(some_ints));
        test_stack(vec);
    }

    { // lvalue forward_list
        forward_list lst(ranges::begin(some_ints), ranges::end(some_ints));
        test_stack(lst);
    }

    test_in<instantiator, const int>();
}
