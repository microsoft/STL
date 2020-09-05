// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Covers Ranges changes to istream_iterator, ostream_iterator, istreambuf_iterator, and ostreambuf_iterator

#include <cassert>
#include <iostream>
#include <iterator>
#include <string>
#include <type_traits>
#include <utility>

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

template <class T, class CharT = char, class Traits = char_traits<CharT>, class Diff = ptrdiff_t>
void test_istream() {
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
void test_ostream(basic_ostream<CharT, Traits>& os) {
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
    { [[maybe_unused]] constexpr I constexprConstructed{}; }
    STATIC_ASSERT(is_nothrow_default_constructible_v<I>);
#endif // __cpp_lib_concepts
}

template <class CharT = char, class Traits = char_traits<CharT>>
void test_istreambuf() {
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
void test_ostreambuf(basic_ostream<CharT, Traits>& os) {
    using I = ostreambuf_iterator<CharT, Traits>;

    // Also tests strengthened noexcept on some pre-existing operations
    I i{os};
    STATIC_ASSERT(noexcept(*i));
    STATIC_ASSERT(noexcept(++i));
    STATIC_ASSERT(noexcept(i++));

#ifdef __cpp_lib_concepts
    STATIC_ASSERT(output_iterator<I, const CharT&>);

    STATIC_ASSERT(is_same_v<typename I::difference_type, ptrdiff_t>);
    { [[maybe_unused]] constexpr I constexprConstructed{}; }
    STATIC_ASSERT(is_nothrow_default_constructible_v<I>);
#endif // __cpp_lib_concepts
}

int main() {
    test_istream<int>();
    test_istream<string>();
    test_istream<int, wchar_t>();
    test_istream<string, wchar_t>();

    test_ostream<int>(cout);
    test_ostream<string>(cout);
    test_ostream<int>(wcout);
    test_ostream<string>(wcout);

    test_istreambuf<char>();
    test_istreambuf<wchar_t>();

    test_ostreambuf(cout);
    test_ostreambuf(wcout);
}
