// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstddef>
#include <ranges>
#include <string>

using namespace std;

struct not_constructible {
    not_constructible()  = delete;
    ~not_constructible() = delete;
};

template <class T>
constexpr bool test_one_type() {
    // validate type properties
    using R = ranges::empty_view<T>;
    static_assert(ranges::view<R> && ranges::contiguous_range<R> && ranges::sized_range<R> && ranges::common_range<R>);
    static_assert(ranges::borrowed_range<R>);
    static_assert(same_as<const R, decltype(views::empty<T>)>);
    constexpr auto& r = views::empty<T>;

    // validate member size
    static_assert(same_as<decltype(R::size()), size_t>);
    static_assert(R::size() == 0);
    static_assert(noexcept(R::size()));
    static_assert(noexcept(ranges::size(r)));

    // validate members begin, data, and end
    static_assert(same_as<decltype(R::data()), T*>);
    static_assert(R::data() == nullptr);
    static_assert(noexcept(R::data()));
    static_assert(noexcept(ranges::data(r)));

    static_assert(same_as<decltype(R::begin()), T*>);
    static_assert(R::begin() == nullptr);
    static_assert(noexcept(R::begin()));
    static_assert(noexcept(ranges::begin(r)));

    static_assert(same_as<decltype(R::end()), T*>);
    static_assert(R::end() == nullptr);
    static_assert(noexcept(R::end()));
    static_assert(noexcept(ranges::end(r)));

    // validate member empty
    static_assert(same_as<decltype(R::empty()), bool>);
    static_assert(R::empty() == true);
    static_assert(noexcept(R::empty()));
    static_assert(noexcept(ranges::empty(r)));

#if _HAS_CXX23
    // validate members cbegin and cend inherited from view_interface
    static_assert(same_as<decltype(r.cbegin()), const_iterator<T*>>);
    static_assert(r.cbegin() == nullptr);

    static_assert(same_as<decltype(r.cend()), const_iterator<T*>>);
    static_assert(r.cend() == nullptr);
#endif // _HAS_CXX23

    // validate other members inherited from view_interface
    assert(!r);

    return true;
}

int main() {
    static_assert(test_one_type<int>());
    test_one_type<int>();
    static_assert(test_one_type<string>());
    test_one_type<string>();
    static_assert(test_one_type<not_constructible>());
    test_one_type<not_constructible>();
}
