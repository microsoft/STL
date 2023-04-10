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
constexpr bool test_string(
    const size_t presize, const int offset, const int length, Rng&& rng, const T (&expected)[N]) {
    assert(static_cast<size_t>(offset) + length <= presize);
    basic_string<T> str(from_range, views::iota(T{'a'}, T('a' + presize)));
    same_as<basic_string<T>> auto& result =
        str.replace_with_range(str.begin() + offset, str.begin() + offset + length, forward<Rng>(rng));
    assert(&result == &str);

    const auto n = static_cast<int>(ranges::size(expected));
    assert(ranges::equal(str | views::take(offset), views::iota(T{'a'}, T('a' + offset))));
    assert(ranges::equal(str | views::drop(offset) | views::take(n), expected));
    assert(ranges::equal(str | views::drop(offset + n), views::iota(T('a' + offset + length), T('a' + presize))));

    return true;
}

static constexpr char hw[]     = "Hello, World!";
static constexpr wchar_t whw[] = L"Hello, World!";

struct string_instantiator {
    template <ranges::input_range R>
    static void call() {
        // clang-format off
        constexpr auto& text = []() -> auto& {
            if constexpr (is_same_v<ranges::range_value_t<R>, char>) {
                return hw;
            } else {
                return whw;
            }
        }();
        // clang-format on

        // insert into empty
        test_string(0, 0, 0, R{text}, text);
        STATIC_ASSERT(test_string(0, 0, 0, R{text}, text));

        // prepend
        test_string(5, 0, 0, R{text}, text);
        STATIC_ASSERT(test_string(5, 0, 0, R{text}, text));

        // expansion
        test_string(5, 2, 1, R{text}, text);
        STATIC_ASSERT(test_string(5, 2, 1, R{text}, text));

        // append
        test_string(5, 5, 0, R{text}, text);
        STATIC_ASSERT(test_string(5, 5, 0, R{text}, text));

        // insert into non-empty
        test_string(26, 5, 0, R{text}, text);
        STATIC_ASSERT(test_string(26, 5, 0, R{text}, text));

        // contraction
        test_string(26, 5, 20, R{text}, text);
        STATIC_ASSERT(test_string(26, 5, 20, R{text}, text));
    }
};

template <class Category, test::Common IsCommon, bool is_random = derived_from<Category, random_access_iterator_tag>>
using move_only_view = test::range<Category, const char, test::Sized{is_random}, test::CanDifference{is_random},
    IsCommon, test::CanCompare{derived_from<Category, forward_iterator_tag>},
    test::ProxyRef{!derived_from<Category, contiguous_iterator_tag>}, test::CanView::yes, test::Copyability::move_only>;

constexpr bool test_copyable_views() {
    test_string(10, 4, 6, span<const char>{hw}, hw);
    test_string(10, 4, 6, span<const wchar_t>{whw}, whw);
    return true;
}

constexpr bool test_move_only_views() {
    test_string(4, 0, 1, move_only_view<input_iterator_tag, test::Common::no>{hw}, hw);
    test_string(4, 0, 1, move_only_view<forward_iterator_tag, test::Common::no>{hw}, hw);
    test_string(4, 0, 1, move_only_view<forward_iterator_tag, test::Common::yes>{hw}, hw);
    test_string(4, 0, 1, move_only_view<bidirectional_iterator_tag, test::Common::no>{hw}, hw);
    test_string(4, 0, 1, move_only_view<bidirectional_iterator_tag, test::Common::yes>{hw}, hw);
    test_string(4, 0, 1, move_only_view<random_access_iterator_tag, test::Common::no>{hw}, hw);
    test_string(4, 0, 1, move_only_view<random_access_iterator_tag, test::Common::yes>{hw}, hw);
    return true;
}

constexpr bool test_c_array() {
    test_string(2, 1, 1, hw, hw);
    test_string(2, 1, 1, whw, whw);
    return true;
}

constexpr bool test_lvalue_vector() {
    {
        vector vec(ranges::begin(hw), ranges::end(hw));
        test_string(0, 0, 0, vec, hw);
    }
    {
        vector vec(ranges::begin(whw), ranges::end(whw));
        test_string(1, 0, 0, vec, whw);
    }
    return true;
}

void test_lvalue_forward_list() {
    {
        forward_list lst(ranges::begin(hw), ranges::end(hw));
        test_string(20, 1, 18, lst, hw);
    }
    {
        forward_list lst(ranges::begin(whw), ranges::end(whw));
        test_string(20, 1, 18, lst, whw);
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

    test_in<string_instantiator, const char>();
    test_in<string_instantiator, const wchar_t>();
}
