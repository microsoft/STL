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

template <class T>
struct c_string {
    const T* ptr_ = nullptr;

    struct sentinel {
        constexpr bool operator==(const T* p) const {
            return *p == 0;
        }
    };

    constexpr const T* begin() const {
        return ptr_;
    }
    constexpr sentinel end() const {
        return {};
    }
};

template <ranges::input_range Rng, class T>
constexpr bool test_string(Rng&& rng, const T* expected) {
    basic_string<T> str(from_range, forward<Rng>(rng));
    assert(ranges::equal(str, c_string<T>{expected}));

    return true;
}

static constexpr char hw[]    = "Hello, world!";
static constexpr auto span_hw = span{hw}.first<span{hw}.size() - 1>();

struct string_instantiator {
    template <ranges::input_range R>
    static void call() {
        test_string(R{span_hw}, hw);
        STATIC_ASSERT(test_string(R{span_hw}, hw));
    }
};

static constexpr wchar_t whw[] = L"Hello, world!";
static constexpr auto span_whw = span{whw}.first<span{whw}.size() - 1>();

struct wstring_instantiator {
    template <ranges::input_range R>
    static void call() {
        test_string(R{span_whw}, whw);
        STATIC_ASSERT(test_string(R{span_whw}, whw));
    }
};

template <class Category, class CharT, test::Common IsCommon,
    bool is_random = derived_from<Category, random_access_iterator_tag>>
using move_only_view = test::range<Category, const CharT, test::Sized{is_random}, test::CanDifference{is_random},
    IsCommon, test::CanCompare{derived_from<Category, forward_iterator_tag>},
    test::ProxyRef{!derived_from<Category, contiguous_iterator_tag>}, test::CanView::yes, test::Copyability::move_only>;

constexpr bool test_copyable_views() {
    test_string(span_hw, hw);
    test_string(span_whw, whw);

    return true;
}

constexpr bool test_move_only_views() {
    test_string(move_only_view<input_iterator_tag, char, test::Common::no>{span_hw}, hw);
    test_string(move_only_view<forward_iterator_tag, char, test::Common::no>{span_hw}, hw);
    test_string(move_only_view<forward_iterator_tag, char, test::Common::yes>{span_hw}, hw);
    test_string(move_only_view<bidirectional_iterator_tag, char, test::Common::no>{span_hw}, hw);
    test_string(move_only_view<bidirectional_iterator_tag, char, test::Common::yes>{span_hw}, hw);
    test_string(move_only_view<random_access_iterator_tag, char, test::Common::no>{span_hw}, hw);
    test_string(move_only_view<random_access_iterator_tag, char, test::Common::yes>{span_hw}, hw);

    test_string(move_only_view<input_iterator_tag, wchar_t, test::Common::no>{span_whw}, whw);
    test_string(move_only_view<forward_iterator_tag, wchar_t, test::Common::no>{span_whw}, whw);
    test_string(move_only_view<forward_iterator_tag, wchar_t, test::Common::yes>{span_whw}, whw);
    test_string(move_only_view<bidirectional_iterator_tag, wchar_t, test::Common::no>{span_whw}, whw);
    test_string(move_only_view<bidirectional_iterator_tag, wchar_t, test::Common::yes>{span_whw}, whw);
    test_string(move_only_view<random_access_iterator_tag, wchar_t, test::Common::no>{span_whw}, whw);
    test_string(move_only_view<random_access_iterator_tag, wchar_t, test::Common::yes>{span_whw}, whw);

    return true;
}

constexpr bool test_c_array() {
    test_string(span_hw, hw);
    test_string(span_whw, whw);
    return true;
}

constexpr bool test_lvalue_vector() {
    {
        vector vec(span_hw.data(), span_hw.data() + span_hw.size());
        test_string(vec, hw);
    }
    {
        vector vec(span_whw.data(), span_whw.data() + span_whw.size());
        test_string(vec, whw);
    }

    return true;
}

void test_lvalue_forward_list() {
    {
        forward_list lst(span_hw.data(), span_hw.data() + span_hw.size());
        test_string(lst, hw);
    }
    {
        forward_list lst(span_whw.data(), span_whw.data() + span_whw.size());
        test_string(lst, whw);
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
    test_in<wstring_instantiator, const wchar_t>();
}
