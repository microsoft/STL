// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <array>
#include <cstddef>
#include <string>
#if _HAS_CXX17
#include <string_view>
#endif // _HAS_CXX17

#include <test_death.hpp>

using namespace std;

template <class CharT>
void test_long_resize() {
    basic_string<CharT> s(sizeof(basic_string<CharT>), static_cast<CharT>('*'));
    auto it = s.begin() + 2;
    s.resize(1);
    (void) (it - 2);
}

template <class CharT>
void test_short_resize() {
    basic_string<CharT> s(2, static_cast<CharT>('*'));
    auto it = s.begin() + 1;
    s.resize(1);
    (void) (it - 1);
}

template <class CharT>
void test_long_resize_element() {
    basic_string<CharT> s(sizeof(basic_string<CharT>), static_cast<CharT>('*'));
    auto it = s.begin() + 2;
    s.resize(1, static_cast<CharT>(' '));
    (void) (it - 2);
}

template <class CharT>
void test_short_resize_element() {
    basic_string<CharT> s(2, static_cast<CharT>('*'));
    auto it = s.begin() + 1;
    s.resize(1, static_cast<CharT>(' '));
    (void) (it - 1);
}

#if _HAS_CXX23 // Test that resize_and_overwrite() orhpans all iterators even when the elements are unchanged.
template <class CharT>
void test_long_resize_and_overwrite_then_begin() {
    basic_string<CharT> s(sizeof(basic_string<CharT>), static_cast<CharT>('*'));
    auto it = s.begin();
    s.resize_and_overwrite(s.size(), [](const CharT*, size_t n) { return n; });
    (void) (it == s.begin());
}

template <class CharT>
void test_long_resize_and_overwrite_then_end() {
    basic_string<CharT> s(sizeof(basic_string<CharT>), static_cast<CharT>('*'));
    auto it = s.end();
    s.resize_and_overwrite(s.size(), [](const CharT*, size_t n) { return n; });
    (void) (it == s.end());
}

template <class CharT>
void test_short_resize_and_overwrite_then_begin() {
    basic_string<CharT> s(2, static_cast<CharT>('*'));
    auto it = s.begin();
    s.resize_and_overwrite(s.size(), [](const CharT*, size_t n) { return n; });
    (void) (it == s.begin());
}

template <class CharT>
void test_short_resize_and_overwrite_then_end() {
    basic_string<CharT> s(2, static_cast<CharT>('*'));
    auto it = s.end();
    s.resize_and_overwrite(s.size(), [](const CharT*, size_t n) { return n; });
    (void) (it == s.end());
}

template <class CharT>
void test_empty_resize_and_overwrite_then_begin() {
    basic_string<CharT> s;
    auto it = s.end();
    s.resize_and_overwrite(s.size(), [](const CharT*, size_t n) { return n; });
    (void) (it == s.end());
}

template <class CharT>
void test_empty_resize_and_overwrite_then_end() {
    basic_string<CharT> s;
    auto it = s.end();
    s.resize_and_overwrite(s.size(), [](const CharT*, size_t n) { return n; });
    (void) (it == s.end());
}
#endif // _HAS_CXX23

template <class CharT>
void test_long_clear_then_begin() {
    basic_string<CharT> s(sizeof(basic_string<CharT>), static_cast<CharT>('*'));
    auto it = s.begin();
    s.clear();
    (void) (it == s.begin());
}

template <class CharT>
void test_long_clear_then_end() {
    basic_string<CharT> s(sizeof(basic_string<CharT>), static_cast<CharT>('*'));
    auto it = s.end();
    s.clear();
    (void) (it == s.end());
}

template <class CharT>
void test_short_clear_then_begin() {
    basic_string<CharT> s(1, static_cast<CharT>('*'));
    auto it = s.begin();
    s.clear();
    (void) (it == s.begin());
}

template <class CharT>
void test_empty_clear_then_begin() {
    basic_string<CharT> s;
    auto it = s.begin();
    s.clear();
    (void) (it == s.begin());
}

template <class CharT>
void test_short_clear_then_end() {
    basic_string<CharT> s(1, static_cast<CharT>('*'));
    auto it = s.end();
    s.clear();
    (void) (it == s.end());
}

template <class CharT>
void test_empty_clear_then_end() {
    basic_string<CharT> s;
    auto it = s.end();
    s.clear();
    (void) (it == s.end());
}

template <class CharT>
void test_long_erase_since_offset() {
    basic_string<CharT> s(sizeof(basic_string<CharT>), static_cast<CharT>('*'));
    auto it = s.begin() + 2;
    s.erase(1);
    (void) (it - 2);
}

template <class CharT>
void test_short_erase_since_offset() {
    basic_string<CharT> s(2, static_cast<CharT>('*'));
    auto it = s.begin() + 1;
    s.erase(1);
    (void) (it - 1);
}

template <class CharT>
void test_long_erase_offset_count() {
    basic_string<CharT> s(sizeof(basic_string<CharT>), static_cast<CharT>('*'));
    auto it = s.end() - 1;
    s.erase(0, s.size() / 2);
    (void) (it - 1);
}

template <class CharT>
void test_short_erase_offset_count() {
    basic_string<CharT> s(2, static_cast<CharT>('*'));
    auto it = s.end() - 1;
    s.erase(0, 1);
    (void) (it - 1);
}

template <class CharT>
void test_long_erase_iterator() {
    basic_string<CharT> s(sizeof(basic_string<CharT>), static_cast<CharT>('*'));
    auto it = s.end() - 1;
    s.erase(s.cbegin());
    (void) (it - 2);
}

template <class CharT>
void test_short_erase_iterator() {
    basic_string<CharT> s(2, static_cast<CharT>('*'));
    auto it = s.end() - 1;
    s.erase(s.cbegin());
    (void) (it - 1);
}

template <class CharT>
void test_long_erase_iterator_pair() {
    basic_string<CharT> s(sizeof(basic_string<CharT>), static_cast<CharT>('*'));
    auto it = s.end() - 1;
    s.erase(s.cbegin(), s.cbegin() + s.size() / 2);
    (void) (it - 1);
}

template <class CharT>
void test_short_erase_iterator_pair() {
    basic_string<CharT> s(2, static_cast<CharT>('*'));
    auto it = s.end() - 1;
    s.erase(s.cbegin(), s.cbegin() + 1);
    (void) (it - 1);
}

template <class CharT>
void test_long_pop_back() {
    basic_string<CharT> s(sizeof(basic_string<CharT>), static_cast<CharT>('*'));
    auto it = s.end() - 1;
    s.pop_back();
    (void) (it - 2);
}

template <class CharT>
void test_short_pop_back() {
    basic_string<CharT> s(2, static_cast<CharT>('*'));
    auto it = s.end() - 1;
    s.pop_back();
    (void) (it - 1);
}

template <class CharT>
void test_long_assign_lvalue() {
    basic_string<CharT> s(sizeof(basic_string<CharT>), static_cast<CharT>('*'));
    auto it = s.end() - 1;
    auto s2 = s.substr(0, s.size() / 2);
    s.assign(s2);
    (void) (it - 1);
}

template <class CharT>
void test_short_assign_lvalue() {
    basic_string<CharT> s(2, static_cast<CharT>('*'));
    auto it = s.end() - 1;
    auto s2 = s.substr(0, s.size() / 2);
    s.assign(s2);
    (void) (it - 1);
}

template <class CharT>
void test_long_assign_rvalue() {
    basic_string<CharT> s(sizeof(basic_string<CharT>), static_cast<CharT>('*'));
    auto it = s.end() - 1;
    s.assign(s.substr(0, s.size() / 2));
    (void) (it - 1);
}

template <class CharT>
void test_short_assign_rvalue() {
    basic_string<CharT> s(2, static_cast<CharT>('*'));
    auto it = s.end() - 1;
    s.assign(s.substr(0, s.size() / 2));
    (void) (it - 1);
}

template <class CharT>
void test_long_assign_string_offset_count() {
    basic_string<CharT> s(sizeof(basic_string<CharT>), static_cast<CharT>('*'));
    auto it = s.end() - 1;
    s.assign(s, 0, s.size() / 2);
    (void) (it - 1);
}

template <class CharT>
void test_short_assign_string_offset_count() {
    basic_string<CharT> s(2, static_cast<CharT>('*'));
    auto it = s.end() - 1;
    s.assign(s, 0, s.size() / 2);
    (void) (it - 1);
}

#if _HAS_CXX17
template <class CharT>
void test_long_assign_string_view() {
    basic_string<CharT> s(sizeof(basic_string<CharT>), static_cast<CharT>('*'));
    auto it = s.end() - 1;
    s.assign(basic_string_view<CharT>{s}.substr(0, s.size() / 2));
    (void) (it - 1);
}

template <class CharT>
void test_short_assign_string_view() {
    basic_string<CharT> s(2, static_cast<CharT>('*'));
    auto it = s.end() - 1;
    s.assign(basic_string_view<CharT>{s}.substr(0, s.size() / 2));
    (void) (it - 1);
}

template <class CharT>
void test_long_assign_string_view_offset_count() {
    basic_string<CharT> s(sizeof(basic_string<CharT>), static_cast<CharT>('*'));
    auto it = s.end() - 1;
    s.assign(basic_string_view<CharT>{s}, s.size() / 2, s.size() / 2);
    (void) (it - 1);
}

template <class CharT>
void test_short_assign_string_view_offset_count() {
    basic_string<CharT> s(2, static_cast<CharT>('*'));
    auto it = s.end() - 1;
    s.assign(basic_string_view<CharT>{s}, s.size() / 2, s.size() / 2);
    (void) (it - 1);
}
#endif // _HAS_CXX17

template <class CharT>
void test_long_assign_pointer_count() {
    basic_string<CharT> s(sizeof(basic_string<CharT>), static_cast<CharT>('*'));
    auto it = s.end() - 1;
    s.assign(s.data(), s.size() / 2);
    (void) (it - 1);
}

template <class CharT>
void test_short_assign_pointer_count() {
    basic_string<CharT> s(2, static_cast<CharT>('*'));
    auto it = s.end() - 1;
    s.assign(s.data(), s.size() / 2);
    (void) (it - 1);
}

template <class CharT>
void test_long_assign_ntcts() {
    basic_string<CharT> s(sizeof(basic_string<CharT>), static_cast<CharT>('*'));
    auto it = s.end() - 1;
    s.assign(s.substr(0, s.size() / 2).c_str());
    (void) (it - 1);
}

template <class CharT>
void test_short_assign_ntcts() {
    basic_string<CharT> s(2, static_cast<CharT>('*'));
    auto it = s.end() - 1;
    s.assign(s.substr(0, s.size() / 2).c_str());
    (void) (it - 1);
}

template <class CharT>
void test_long_assign_ntcts_offset_count() {
    basic_string<CharT> s(sizeof(basic_string<CharT>), static_cast<CharT>('*'));
    auto it = s.end() - 1;
    s.assign(s.data(), s.size() / 2, s.size() / 2);
    (void) (it - 1);
}

template <class CharT>
void test_short_assign_ntcts_offset_count() {
    basic_string<CharT> s(2, static_cast<CharT>('*'));
    auto it = s.end() - 1;
    s.assign(s.data(), 1, 1);
    (void) (it - 1);
}

template <class CharT>
void test_long_assign_repitition_element() {
    basic_string<CharT> s(sizeof(basic_string<CharT>), static_cast<CharT>('*'));
    auto it = s.end() - 1;
    s.assign(s.size() / 2, ' ');
    (void) (it - 1);
}

template <class CharT>
void test_short_assign_repitition_element() {
    basic_string<CharT> s(2, static_cast<CharT>('*'));
    auto it = s.end() - 1;
    s.assign(1, ' ');
    (void) (it - 1);
}

template <class CharT>
void test_long_assign_iterator_pair() {
    basic_string<CharT> s(sizeof(basic_string<CharT>), static_cast<CharT>('*'));
    auto it = s.end() - 1;
    s.assign(s.cbegin(), s.cbegin() + s.size() / 2);
    (void) (it - 1);
}

template <class CharT>
void test_short_assign_iterator_pair() {
    basic_string<CharT> s(2, static_cast<CharT>('*'));
    auto it = s.end() - 1;
    s.assign(s.cbegin(), s.cbegin() + s.size() / 2);
    (void) (it - 1);
}

#if _HAS_CXX23
template <class CharT>
void test_long_assign_range() {
    CharT arr[sizeof(basic_string<CharT>) / 2];
    for (auto& c : arr) {
        c = static_cast<CharT>(' ');
    }

    basic_string<CharT> s(sizeof(basic_string<CharT>), static_cast<CharT>('*'));
    auto it = s.end() - 1;
    s.assign_range(arr);
    (void) (it - 1);
}

template <class CharT>
void test_short_assign_range() {
    CharT arr[1]{static_cast<CharT>(' ')};
    basic_string<CharT> s(2, static_cast<CharT>('*'));
    auto it = s.end() - 1;
    s.assign_range(arr);
    (void) (it - 1);
}
#endif // _HAS_CXX23

template <class CharT>
void test_long_assign_initializer_list() {
    basic_string<CharT> s(42, static_cast<CharT>('*'));
    auto it = s.end() - 1;
    s.assign({
        static_cast<CharT>('T'),
        static_cast<CharT>('h'),
        static_cast<CharT>('e'),
        static_cast<CharT>(' '),
        static_cast<CharT>('q'),
        static_cast<CharT>('u'),
        static_cast<CharT>('i'),
        static_cast<CharT>('c'),
        static_cast<CharT>('k'),
        static_cast<CharT>(' '),
        static_cast<CharT>('b'),
        static_cast<CharT>('r'),
        static_cast<CharT>('o'),
        static_cast<CharT>('w'),
        static_cast<CharT>('n'),
        static_cast<CharT>(' '),
        static_cast<CharT>('f'),
        static_cast<CharT>('o'),
        static_cast<CharT>('x'),
        static_cast<CharT>(' '),
        static_cast<CharT>('j'),
        static_cast<CharT>('u'),
        static_cast<CharT>('m'),
        static_cast<CharT>('p'),
        static_cast<CharT>('s'),
    });
    (void) (it - 1);
}

template <class CharT>
void test_short_assign_initializer_list() {
    basic_string<CharT> s(2, static_cast<CharT>('*'));
    auto it = s.end() - 1;
    s.assign({' '});
    (void) (it - 1);
}

template <class CharT>
void test_long_replace_offset_count_string() {
    basic_string<CharT> s(sizeof(basic_string<CharT>), static_cast<CharT>('*'));
    auto it = s.end() - 1;
    s.replace(0, s.size() / 2, basic_string<CharT>{});
    (void) (it - 1);
}

template <class CharT>
void test_short_replace_offset_count_string() {
    basic_string<CharT> s(2, static_cast<CharT>('*'));
    auto it = s.end() - 1;
    s.replace(0, s.size() / 2, basic_string<CharT>{});
    (void) (it - 1);
}

template <class CharT>
void test_long_replace_offset_count_string_offset_count() {
    basic_string<CharT> s(sizeof(basic_string<CharT>), static_cast<CharT>('*'));
    auto it = s.end() - 1;
    s.replace(0, s.size() / 2, s, 0, 0);
    (void) (it - 1);
}

template <class CharT>
void test_short_replace_offset_count_string_offset_count() {
    basic_string<CharT> s(2, static_cast<CharT>('*'));
    auto it = s.end() - 1;
    s.replace(0, s.size() / 2, s, 0, 0);
    (void) (it - 1);
}

#if _HAS_CXX17
template <class CharT>
void test_long_replace_offset_count_string_view() {
    basic_string<CharT> s(sizeof(basic_string<CharT>), static_cast<CharT>('*'));
    auto it = s.end() - 1;
    s.replace(0, s.size() / 2, basic_string_view<CharT>{});
    (void) (it - 1);
}

template <class CharT>
void test_short_replace_offset_count_string_view() {
    basic_string<CharT> s(2, static_cast<CharT>('*'));
    auto it = s.end() - 1;
    s.replace(0, s.size() / 2, basic_string_view<CharT>{});
    (void) (it - 1);
}

template <class CharT>
void test_long_replace_offset_count_string_view_offset_count() {
    basic_string<CharT> s(sizeof(basic_string<CharT>), static_cast<CharT>('*'));
    auto it = s.end() - 1;
    s.replace(0, s.size() / 2, basic_string_view{s}, 0, 0);
    (void) (it - 1);
}

template <class CharT>
void test_short_replace_offset_count_string_view_offset_count() {
    basic_string<CharT> s(2, static_cast<CharT>('*'));
    auto it = s.end() - 1;
    s.replace(0, s.size() / 2, basic_string_view<CharT>{s}, 0, 0);
    (void) (it - 1);
}
#endif // _HAS_CXX17

template <class CharT>
void test_long_replace_offset_count_pointer_count() {
    basic_string<CharT> s(sizeof(basic_string<CharT>), static_cast<CharT>('*'));
    auto it = s.end() - 1;
    s.replace(0, s.size() / 2, s.data(), 0);
    (void) (it - 1);
}

template <class CharT>
void test_short_replace_offset_count_pointer_count() {
    basic_string<CharT> s(2, static_cast<CharT>('*'));
    auto it = s.end() - 1;
    s.replace(0, s.size() / 2, s.data(), 0);
    (void) (it - 1);
}

template <class CharT>
void test_long_replace_offset_count_ntcts() {
    basic_string<CharT> s(sizeof(basic_string<CharT>), static_cast<CharT>('*'));
    auto it = s.end() - 1;
    s.replace(0, s.size() / 2, basic_string<CharT>{}.c_str());
    (void) (it - 1);
}

template <class CharT>
void test_short_replace_offset_count_ntcts() {
    basic_string<CharT> s(2, static_cast<CharT>('*'));
    auto it = s.end() - 1;
    s.replace(0, s.size() / 2, basic_string<CharT>{}.c_str());
    (void) (it - 1);
}

template <class CharT>
void test_long_replace_offset_count_repitition_element() {
    basic_string<CharT> s(sizeof(basic_string<CharT>), static_cast<CharT>('*'));
    auto it = s.end() - 1;
    s.replace(0, s.size() / 2, 0, ' ');
    (void) (it - 1);
}

template <class CharT>
void test_short_replace_offset_count_repitition_element() {
    basic_string<CharT> s(2, static_cast<CharT>('*'));
    auto it = s.end() - 1;
    s.replace(0, s.size() / 2, 0, ' ');
    (void) (it - 1);
}

template <class CharT>
void test_long_replace_iterator_pair_string() {
    basic_string<CharT> s(sizeof(basic_string<CharT>), static_cast<CharT>('*'));
    auto it = s.end() - 1;
    s.replace(s.cbegin(), s.cbegin() + s.size() / 2, basic_string<CharT>{});
    (void) (it - 1);
}

template <class CharT>
void test_short_replace_iterator_pair_string() {
    basic_string<CharT> s(2, static_cast<CharT>('*'));
    auto it = s.end() - 1;
    s.replace(s.cbegin(), s.cbegin() + 1, basic_string<CharT>{});
    (void) (it - 1);
}

#if _HAS_CXX17
template <class CharT>
void test_long_replace_iterator_pair_string_view() {
    basic_string<CharT> s(sizeof(basic_string<CharT>), static_cast<CharT>('*'));
    auto it = s.end() - 1;
    s.replace(s.cbegin(), s.cbegin() + s.size() / 2, basic_string_view<CharT>{});
    (void) (it - 1);
}

template <class CharT>
void test_short_replace_iterator_pair_string_view() {
    basic_string<CharT> s(2, static_cast<CharT>('*'));
    auto it = s.end() - 1;
    s.replace(s.cbegin(), s.cbegin() + 1, basic_string_view<CharT>{});
    (void) (it - 1);
}
#endif // _HAS_CXX17

template <class CharT>
void test_long_replace_iterator_pair_pointer_count() {
    basic_string<CharT> s(sizeof(basic_string<CharT>), static_cast<CharT>('*'));
    auto it = s.end() - 1;
    s.replace(s.cbegin(), s.cbegin() + s.size() / 2, s.data(), 0);
    (void) (it - 1);
}

template <class CharT>
void test_short_replace_iterator_pair_pointer_count() {
    basic_string<CharT> s(2, static_cast<CharT>('*'));
    auto it = s.end() - 1;
    s.replace(s.cbegin(), s.cbegin() + 1, s.data(), 0);
    (void) (it - 1);
}

template <class CharT>
void test_long_replace_iterator_pair_ntcts() {
    basic_string<CharT> s(sizeof(basic_string<CharT>), static_cast<CharT>('*'));
    auto it = s.end() - 1;
    s.replace(s.cbegin(), s.cbegin() + s.size() / 2, basic_string<CharT>{}.c_str());
    (void) (it - 1);
}

template <class CharT>
void test_short_replace_iterator_pair_ntcts() {
    basic_string<CharT> s(2, static_cast<CharT>('*'));
    auto it = s.end() - 1;
    s.replace(s.cbegin(), s.cbegin() + 1, basic_string<CharT>{}.c_str());
    (void) (it - 1);
}

template <class CharT>
void test_long_replace_iterator_pair_repitition_element() {
    basic_string<CharT> s(sizeof(basic_string<CharT>), static_cast<CharT>('*'));
    auto it = s.end() - 1;
    s.replace(s.cbegin(), s.cbegin() + s.size() / 2, 0, ' ');
    (void) (it - 1);
}

template <class CharT>
void test_short_replace_iterator_pair_repitition_element() {
    basic_string<CharT> s(2, static_cast<CharT>('*'));
    auto it = s.end() - 1;
    s.replace(s.cbegin(), s.cbegin() + 1, 0, ' ');
    (void) (it - 1);
}

template <class CharT>
void test_long_replace_iterator_pair_iterator_pair() {
    basic_string<CharT> s(sizeof(basic_string<CharT>), static_cast<CharT>('*'));
    basic_string<CharT> empty_str{};
    auto it = s.end() - 1;
    s.replace(s.cbegin(), s.cbegin() + s.size() / 2, empty_str.data(), empty_str.data());
    (void) (it - 1);
}

template <class CharT>
void test_short_replace_iterator_pair_iterator_pair() {
    basic_string<CharT> s(2, static_cast<CharT>('*'));
    basic_string<CharT> empty_str{};
    auto it = s.end() - 1;
    s.replace(s.cbegin(), s.cbegin() + 1, empty_str.data(), empty_str.data());
    (void) (it - 1);
}

#if _HAS_CXX23
template <class CharT>
void test_long_replace_with_range() {
    basic_string<CharT> s(sizeof(basic_string<CharT>), static_cast<CharT>('*'));
    basic_string<CharT> empty_str{};
    auto it = s.end() - 1;
    s.replace_with_range(s.cbegin(), s.cbegin() + s.size() / 2, array<CharT, 0>{});
    (void) (it - 1);
}

template <class CharT>
void test_short_replace_with_range() {
    basic_string<CharT> s(2, static_cast<CharT>('*'));
    basic_string<CharT> empty_str{};
    auto it = s.end() - 1;
    s.replace_with_range(s.cbegin(), s.cbegin() + 1, array<CharT, 0>{});
    (void) (it - 1);
}
#endif // _HAS_CXX23

template <class CharT>
void test_long_replace_iterator_pair_initializer_list() {
    basic_string<CharT> s(42, static_cast<CharT>('*'));
    basic_string<CharT> empty_str{};
    auto it = s.end() - 1;
    s.replace(s.cbegin(), s.cbegin() + s.size() / 2,
        {
            static_cast<CharT>('Q'),
            static_cast<CharT>('u'),
            static_cast<CharT>('i'),
            static_cast<CharT>('c'),
            static_cast<CharT>('k'),
        });
    (void) (it - 1);
}

template <class CharT>
void test_short_replace_iterator_pair_initializer_list() {
    basic_string<CharT> s(2, static_cast<CharT>('*'));
    basic_string<CharT> empty_str{};
    auto it = s.end() - 1;
    s.replace_with_range(s.cbegin(), s.cbegin() + 1, initializer_list<CharT>{});
    (void) (it - 1);
}

int main(int argc, char** argv) {
    std_testing::death_test_executive exec;

#if _ITERATOR_DEBUG_LEVEL != 0
    exec.add_death_tests({
        test_long_resize<char>,
        test_short_resize<char>,
        test_long_resize_element<char>,
        test_short_resize_element<char>,

        test_long_clear_then_begin<char>,
        test_long_clear_then_end<char>,
        test_short_clear_then_begin<char>,
        test_short_clear_then_end<char>,
        test_empty_clear_then_begin<char>,
        test_empty_clear_then_end<char>,

        test_long_erase_since_offset<char>,
        test_short_erase_since_offset<char>,
        test_long_erase_offset_count<char>,
        test_short_erase_offset_count<char>,
        test_long_erase_iterator<char>,
        test_short_erase_iterator<char>,
        test_long_erase_iterator_pair<char>,
        test_short_erase_iterator_pair<char>,

        test_long_pop_back<char>,
        test_short_pop_back<char>,

        test_long_assign_lvalue<char>,
        test_short_assign_lvalue<char>,
        test_long_assign_rvalue<char>,
        test_short_assign_rvalue<char>,
        test_long_assign_string_offset_count<char>,
        test_short_assign_string_offset_count<char>,
        test_long_assign_pointer_count<char>,
        test_short_assign_pointer_count<char>,
        test_long_assign_ntcts<char>,
        test_short_assign_ntcts<char>,
        test_long_assign_ntcts_offset_count<char>,
        test_short_assign_ntcts_offset_count<char>,
        test_long_assign_repitition_element<char>,
        test_short_assign_repitition_element<char>,
        test_long_assign_iterator_pair<char>,
        test_short_assign_iterator_pair<char>,
        test_long_assign_initializer_list<char>,
        test_short_assign_initializer_list<char>,

        test_long_replace_offset_count_string<char>,
        test_short_replace_offset_count_string<char>,
        test_long_replace_offset_count_string_offset_count<char>,
        test_short_replace_offset_count_string_offset_count<char>,
        test_long_replace_offset_count_pointer_count<char>,
        test_short_replace_offset_count_pointer_count<char>,
        test_long_replace_offset_count_ntcts<char>,
        test_short_replace_offset_count_ntcts<char>,
        test_long_replace_offset_count_repitition_element<char>,
        test_short_replace_offset_count_repitition_element<char>,
        test_long_replace_iterator_pair_string<char>,
        test_short_replace_iterator_pair_string<char>,
        test_long_replace_iterator_pair_pointer_count<char>,
        test_short_replace_iterator_pair_pointer_count<char>,
        test_long_replace_iterator_pair_ntcts<char>,
        test_short_replace_iterator_pair_ntcts<char>,
        test_long_replace_iterator_pair_repitition_element<char>,
        test_short_replace_iterator_pair_repitition_element<char>,
        test_long_replace_iterator_pair_iterator_pair<char>,
        test_short_replace_iterator_pair_iterator_pair<char>,
        test_long_replace_iterator_pair_initializer_list<char>,
        test_short_replace_iterator_pair_initializer_list<char>,

#if _HAS_CXX17
        test_long_assign_string_view<char>,
        test_short_assign_string_view<char>,
        test_long_assign_string_view_offset_count<char>,
        test_short_assign_string_view_offset_count<char>,

        test_long_replace_offset_count_string_view<char>,
        test_short_replace_offset_count_string_view<char>,
        test_long_replace_offset_count_string_view_offset_count<char>,
        test_short_replace_offset_count_string_view_offset_count<char>,
        test_long_replace_iterator_pair_string_view<char>,
        test_short_replace_iterator_pair_string_view<char>,
#endif // _HAS_CXX17

#if _HAS_CXX23
        test_long_resize_and_overwrite_then_begin<char>,
        test_short_resize_and_overwrite_then_begin<char>,
        test_empty_resize_and_overwrite_then_begin<char>,
        test_long_resize_and_overwrite_then_end<char>,
        test_short_resize_and_overwrite_then_end<char>,
        test_empty_resize_and_overwrite_then_end<char>,

        test_long_assign_range<char>,
        test_short_assign_range<char>,

        test_long_replace_with_range<char>,
        test_short_replace_with_range<char>,
#endif // _HAS_CXX23
    });
#endif // _ITERATOR_DEBUG_LEVEL != 0

    return exec.run(argc, argv);
}
