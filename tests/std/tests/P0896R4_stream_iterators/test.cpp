// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Covers Ranges changes to istream_iterator, ostream_iterator, istreambuf_iterator, and ostreambuf_iterator
// and LWG-3719 changes for directory_iterator, recursive_directory_iterator, regex_iterator, and regex_token_iterator

#include <cassert>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <regex>
#include <string>
#include <type_traits>
#include <utility>

#if _HAS_CXX17
#include <filesystem>
#endif // _HAS_CXX17

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

template <class T, class CharT = char, class Traits = char_traits<CharT>, class Diff = ptrdiff_t>
void test_istream_iterator() {
    using I = istream_iterator<T, CharT, Traits, Diff>;

    // Also test strengthened noexcept on some pre-existing operations
    const I i{};
    STATIC_ASSERT(is_nothrow_default_constructible_v<I> == is_nothrow_default_constructible_v<T>);
    STATIC_ASSERT(noexcept(*i));
    STATIC_ASSERT(noexcept(i.operator->()));

    STATIC_ASSERT(noexcept(i == i));
    STATIC_ASSERT(noexcept(i != i));

#ifdef __cpp_lib_concepts
    STATIC_ASSERT(input_iterator<I>);

    if constexpr (is_scalar_v<T>) {
        constexpr I fromDefaultSentinel{default_sentinel};
        assert(fromDefaultSentinel == I{});
        assert(!(fromDefaultSentinel != I{}));
        STATIC_ASSERT(is_nothrow_constructible_v<I, default_sentinel_t>);
    }

    assert(i == default_sentinel);
    STATIC_ASSERT(noexcept(i == default_sentinel));
    assert(default_sentinel == i);
    STATIC_ASSERT(noexcept(default_sentinel == i));
    assert(!(i != default_sentinel));
    STATIC_ASSERT(noexcept(!(i != default_sentinel)));
    assert(!(default_sentinel != i));
    STATIC_ASSERT(noexcept(!(default_sentinel != i)));
#endif // __cpp_lib_concepts
}

template <class T, class CharT = char, class Traits = char_traits<CharT>>
void test_ostream_iterator(basic_ostream<CharT, Traits>& os) {
    using I = ostream_iterator<T, CharT, Traits>;

    // Also tests strengthened noexcept on some pre-existing operations
    I i{os};
    STATIC_ASSERT(is_nothrow_constructible_v<I, basic_ostream<CharT, Traits>&>);
    STATIC_ASSERT(is_nothrow_constructible_v<I, basic_ostream<CharT, Traits>&, const CharT*>);
    STATIC_ASSERT(noexcept(*i));
    STATIC_ASSERT(noexcept(++i));
    STATIC_ASSERT(noexcept(i++));

#ifdef __cpp_lib_concepts
    STATIC_ASSERT(output_iterator<I, const T&>);
    STATIC_ASSERT(is_same_v<typename I::difference_type, ptrdiff_t>);
#endif // __cpp_lib_concepts
}

template <class CharT = char, class Traits = char_traits<CharT>>
void test_istreambuf_iterator() {
#ifdef __cpp_lib_concepts
    using I = istreambuf_iterator<CharT, Traits>;
    STATIC_ASSERT(input_iterator<I>);

    constexpr I i{default_sentinel};
    assert(i == I{});
    assert(!(i != I{}));
    STATIC_ASSERT(is_nothrow_constructible_v<I, default_sentinel_t>);

    assert(i == default_sentinel);
    assert(default_sentinel == i);
    assert(!(i != default_sentinel));
    assert(!(default_sentinel != i));
#endif // __cpp_lib_concepts
}

template <class CharT = char, class Traits = char_traits<CharT>>
void test_ostreambuf_iterator(basic_ostream<CharT, Traits>& os) {
    using I = ostreambuf_iterator<CharT, Traits>;

    // Also tests strengthened noexcept on some pre-existing operations
    I i{os};
    STATIC_ASSERT(noexcept(*i));
    STATIC_ASSERT(noexcept(++i));
    STATIC_ASSERT(noexcept(i++));

#ifdef __cpp_lib_concepts
    STATIC_ASSERT(output_iterator<I, const CharT&>);
    STATIC_ASSERT(is_same_v<typename I::difference_type, ptrdiff_t>);
#endif // __cpp_lib_concepts
}

template <class BidIt, class CharT = typename iterator_traits<BidIt>::value_type, class Traits = regex_traits<CharT>>
void test_regex_iterator() {
    using I = regex_iterator<BidIt, CharT, Traits>;
    STATIC_ASSERT(is_same_v<typename I::difference_type, ptrdiff_t>);
    STATIC_ASSERT(is_same_v<typename I::iterator_category, forward_iterator_tag>);

#ifdef __cpp_lib_concepts
    const I i{};

    assert(i == default_sentinel);
    assert(default_sentinel == i);
    assert(!(i != default_sentinel));
    assert(!(default_sentinel != i));

    STATIC_ASSERT(forward_iterator<I>);
#endif // __cpp_lib_concepts
}

template <class BidIt, class CharT = typename iterator_traits<BidIt>::value_type, class Traits = regex_traits<CharT>>
void test_regex_token_iterator() {
    using I = regex_token_iterator<BidIt, CharT, Traits>;
    STATIC_ASSERT(is_same_v<typename I::difference_type, ptrdiff_t>);
    STATIC_ASSERT(is_same_v<typename I::iterator_category, forward_iterator_tag>);

#ifdef __cpp_lib_concepts
    const I i{};

    assert(i == default_sentinel);
    assert(default_sentinel == i);
    assert(!(i != default_sentinel));
    assert(!(default_sentinel != i));

    STATIC_ASSERT(forward_iterator<I>);
#endif // __cpp_lib_concepts
}

#if _HAS_CXX17
void test_directory_iterator() {
    using I = filesystem::directory_iterator;
    STATIC_ASSERT(is_same_v<typename I::difference_type, ptrdiff_t>);
    STATIC_ASSERT(is_same_v<typename I::iterator_category, input_iterator_tag>);

#ifdef __cpp_lib_concepts
    const I i{};

    assert(i == default_sentinel);
    assert(default_sentinel == i);
    assert(!(i != default_sentinel));
    assert(!(default_sentinel != i));

    STATIC_ASSERT(input_iterator<I>);
#endif // __cpp_lib_concepts
}

void test_recursive_directory_iterator() {
    using I = filesystem::recursive_directory_iterator;
    STATIC_ASSERT(is_same_v<typename I::difference_type, ptrdiff_t>);
    STATIC_ASSERT(is_same_v<typename I::iterator_category, input_iterator_tag>);

#ifdef __cpp_lib_concepts
    const I i{};

    assert(i == default_sentinel);
    assert(default_sentinel == i);
    assert(!(i != default_sentinel));
    assert(!(default_sentinel != i));

    STATIC_ASSERT(input_iterator<I>);
#endif // __cpp_lib_concepts
}
#endif // _HAS_CXX17

int main() {
    test_istream_iterator<int>();
    test_istream_iterator<string>();
    test_istream_iterator<int, wchar_t>();
    test_istream_iterator<string, wchar_t>();

    test_ostream_iterator<int>(cout);
    test_ostream_iterator<string>(cout);
    test_ostream_iterator<int>(wcout);
    test_ostream_iterator<string>(wcout);

    test_istreambuf_iterator<char>();
    test_istreambuf_iterator<wchar_t>();

    test_ostreambuf_iterator(cout);
    test_ostreambuf_iterator(wcout);

    test_regex_iterator<const char*>();
    test_regex_iterator<const wchar_t*>();
    test_regex_iterator<string::const_iterator>();
    test_regex_iterator<wstring::const_iterator>();

    test_regex_token_iterator<const char*>();
    test_regex_token_iterator<const wchar_t*>();
    test_regex_token_iterator<string::const_iterator>();
    test_regex_token_iterator<wstring::const_iterator>();

#if _HAS_CXX17
    test_directory_iterator();
    test_recursive_directory_iterator();
#endif // _HAS_CXX17
}
