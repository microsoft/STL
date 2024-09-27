// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <list>
#include <memory>
#include <string>
#include <type_traits>
#if _HAS_CXX17
#include <string_view>
#endif // _HAS_CXX17

#if _HAS_CXX20
#define CONSTEXPR20 constexpr
#else // ^^^ _HAS_CXX20 / !_HAS_CXX20 vvv
#define CONSTEXPR20 inline
#endif // ^^^ !_HAS_CXX20 ^^^

using namespace std;

template <class>
constexpr bool is_initializer_list = false;
template <class T>
constexpr bool is_initializer_list<initializer_list<T>> = true;

template <class T>
constexpr initializer_list<T> ilist42 = {T{'4'}, T{'2'}};

template <class>
constexpr bool is_basic_string_or_cstr_or_view = false;
template <class C, class T, class A>
constexpr bool is_basic_string_or_cstr_or_view<basic_string<C, T, A>> = true;
template <class C>
constexpr bool is_basic_string_or_cstr_or_view<const C*> = true;
#if _HAS_CXX17
template <class C, class T>
constexpr bool is_basic_string_or_cstr_or_view<basic_string_view<C, T>> = true;
#endif // _HAS_CXX17

struct nasty_string_source {
    template <class IList, enable_if_t<is_initializer_list<IList>, int> = 0>
    constexpr operator IList() const {
        return ilist42<typename IList::value_type>;
    }

    template <class T, enable_if_t<!is_initializer_list<T> && !is_basic_string_or_cstr_or_view<T>, int> = 0>
    constexpr operator T() const {
        return T{};
    }
};

CONSTEXPR20 bool test_nasty_conversion_to_basic_string() {
    assert(string(nasty_string_source{}, allocator<char>{}) == "42"s);
#ifdef __cpp_char8_t
    assert(u8string(nasty_string_source{}, allocator<char8_t>{}) == u8"42"s);
#endif // defined(__cpp_char8_t)
    assert(u16string(nasty_string_source{}, allocator<char16_t>{}) == u"42"s);
    assert(u32string(nasty_string_source{}, allocator<char32_t>{}) == U"42"s);
    assert(wstring(nasty_string_source{}, allocator<wchar_t>{}) == L"42"s);

    return true;
}

template <class>
constexpr bool is_list = false;
template <class T, class A>
constexpr bool is_list<list<T, A>> = true;

struct nasty_list_source {
    template <class IList, enable_if_t<is_initializer_list<IList>, int> = 0>
    constexpr operator IList() const {
        return ilist42<typename IList::value_type>;
    }

    template <class T, enable_if_t<!is_initializer_list<T> && !is_list<T> && !is_integral_v<T>, int> = 0>
    constexpr operator T() const {
        return T{};
    }
};

void test_nasty_conversion_to_list() {
    allocator<int> ator{};
    assert((list<int>{nasty_list_source{}, ator} == list<int>{int{'4'}, int{'2'}}));
}

#if _HAS_CXX20
static_assert(test_nasty_conversion_to_basic_string());
#endif // _HAS_CXX20

int main() {
    test_nasty_conversion_to_basic_string();
    test_nasty_conversion_to_list();
}
