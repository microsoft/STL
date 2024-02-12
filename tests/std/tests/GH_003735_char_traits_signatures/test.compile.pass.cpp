// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cstdint>
#include <cstdio>
#include <cwchar>
#include <string>
#include <type_traits>

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

using namespace std;

// Also ensure that the member type int_type is correct for standard specializations.
STATIC_ASSERT(is_same_v<char_traits<char>::int_type, int>);
#ifdef __cpp_char8_t
STATIC_ASSERT(is_same_v<char_traits<char8_t>::int_type, unsigned int>);
#endif // __cpp_char8_t
STATIC_ASSERT(is_same_v<char_traits<char16_t>::int_type, uint_least16_t>);
STATIC_ASSERT(is_same_v<char_traits<char32_t>::int_type, uint_least32_t>);
STATIC_ASSERT(is_same_v<char_traits<wchar_t>::int_type, wint_t>);

template <class CharT>
void test_n2349_char_traits_signatures() {
    using IntT = typename char_traits<CharT>::int_type;

    volatile CharT c{};
    volatile IntT i{};

    (void) char_traits<CharT>::eq(c, c);
    (void) char_traits<CharT>::lt(c, c);
    (void) char_traits<CharT>::not_eof(i);
    (void) char_traits<CharT>::to_char_type(i);
    (void) char_traits<CharT>::to_int_type(c);
    (void) char_traits<CharT>::eq_int_type(i, i);
}

void test_n2349_char_traits_signatures_all() {
    test_n2349_char_traits_signatures<char>();
#ifdef __cpp_char8_t
    test_n2349_char_traits_signatures<char8_t>();
#endif // __cpp_char8_t
    test_n2349_char_traits_signatures<char16_t>();
    test_n2349_char_traits_signatures<char32_t>();
    test_n2349_char_traits_signatures<wchar_t>();
}
