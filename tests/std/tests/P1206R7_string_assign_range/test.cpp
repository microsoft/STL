// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <forward_list>
#include <ranges>
#include <span>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <range_algorithm_support.hpp>

using namespace std;

template <ranges::input_range Rng, class T, size_t N>
constexpr bool test_string(const size_t capacity, const size_t presize, Rng&& rng, const T (&expected)[N]) {
    basic_string<T> str;
    str.reserve(capacity);
    str.resize(presize, T{'x'});

    str.assign_range(forward<Rng>(rng));
    assert(ranges::equal(str, expected));

    return true;
}

template <ranges::input_range Rng, class T, size_t N>
constexpr bool test_string(const T (&expected)[N]) {
    test_string(0, 0, Rng{expected}, expected);
    test_string(ranges::size(expected) + 2, 2, Rng{expected}, expected);

    return true;
}

static constexpr char some_chars[] = "Hello, world!";

struct string_instantiator {
    template <ranges::input_range R>
    static constexpr void call() {
        test_string<R>(some_chars);
        STATIC_ASSERT(test_string<R>(some_chars));
    }
};

static constexpr wchar_t some_wchars[] = L"Hello, world!";

struct wstring_instantiator {
    template <ranges::input_range R>
    static constexpr void call() {
        test_string<R>(some_wchars);
        STATIC_ASSERT(test_string<R>(some_wchars));
    }
};

template <class Category, test::Common IsCommon, bool is_random = derived_from<Category, random_access_iterator_tag>>
using move_only_view = test::range<Category, const char, test::Sized{is_random}, test::CanDifference{is_random},
    IsCommon, test::CanCompare{derived_from<Category, forward_iterator_tag>},
    test::ProxyRef{!derived_from<Category, contiguous_iterator_tag>}, test::CanView::yes, test::Copyability::move_only>;

constexpr bool test_copyable_view() {
    test_string<span<const char>>(some_chars);
    test_string<span<const wchar_t>>(some_wchars);
    return true;
}

constexpr bool test_move_only_view() {
    test_string<move_only_view<input_iterator_tag, test::Common::no>>(some_chars);
    test_string<move_only_view<forward_iterator_tag, test::Common::no>>(some_chars);
    test_string<move_only_view<forward_iterator_tag, test::Common::yes>>(some_chars);
    test_string<move_only_view<bidirectional_iterator_tag, test::Common::no>>(some_chars);
    test_string<move_only_view<bidirectional_iterator_tag, test::Common::yes>>(some_chars);
    test_string<move_only_view<random_access_iterator_tag, test::Common::no>>(some_chars);
    test_string<move_only_view<random_access_iterator_tag, test::Common::yes>>(some_chars);

    return true;
}

constexpr bool test_c_array() {
    test_string(0, 0, some_chars, some_chars);
    test_string(0, 0, some_wchars, some_wchars);
    return true;
}

constexpr bool test_lvalue_vector() {
    {
        vector vec(from_range, some_chars);
        test_string(0, 0, vec, some_chars);
    }
    {
        vector vec(from_range, some_wchars);
        test_string(0, 0, vec, some_wchars);
    }
    return true;
}

void test_lvalue_forward_list() {
    {
        forward_list lst(ranges::begin(some_chars), ranges::end(some_chars));
        test_string(0, 0, lst, some_chars);
    }
    {
        forward_list lst(ranges::begin(some_wchars), ranges::end(some_wchars));
        test_string(0, 0, lst, some_wchars);
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

    test_in<string_instantiator, const char>();
    test_in<wstring_instantiator, const wchar_t>();
}
