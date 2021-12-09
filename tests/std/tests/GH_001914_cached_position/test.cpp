// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <ranges>
#include <utility>

#include <range_algorithm_support.hpp>
using namespace std;

template <class View>
class test_cache : public ranges::_Cached_position<View, test_cache<View>> {
public:
    constexpr test_cache(View view) : _view(move(view)) {}

    constexpr void set_cache() {
        this->_Set_cache(_view, _view.begin());
    }

    [[nodiscard]] constexpr bool has_cache() const noexcept {
        return this->_Has_cache();
    }

private:
    View _view;
};

template <ranges::forward_range R>
constexpr bool test_one() {
    constexpr int some_ints[] = {0, 1, 2, 3, 4, 5, 6, 7};
    { // Propagate cache of random access range after copy
        test_cache cache{R{some_ints}};
        cache.set_cache();

        test_cache new_cache = cache;
        assert(cache.has_cache());
        assert(new_cache.has_cache() == ranges::random_access_range<R>);
    }

    { // Propagate cache of random access range after move
        test_cache cache{R{some_ints}};
        cache.set_cache();

        test_cache new_cache = move(cache);
        assert(!cache.has_cache());
        assert(new_cache.has_cache() == ranges::random_access_range<R>);
    }
    return true;
}

template <class Category>
using test_range = test::range<Category, const int, test::Sized::no,
    test::CanDifference{derived_from<Category, random_access_iterator_tag>}, test::Common::yes, test::CanCompare::yes,
    test::ProxyRef::no, test::CanView::yes, test::Copyability::copyable>;

int main() {
    test_one<test_range<forward_iterator_tag>>();
    static_assert(test_one<test_range<forward_iterator_tag>>());

    test_one<test_range<random_access_iterator_tag>>();
    static_assert(test_one<test_range<random_access_iterator_tag>>());
}
