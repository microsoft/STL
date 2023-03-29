// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <string>

using namespace std;

template <typename C>
constexpr void test_assign(const C a, const C b) {

    C c = a;
    assert(c == a);
    char_traits<C>::assign(c, b);
    assert(c == b);
}

template <typename C>
constexpr void test_length(const C* const str, const size_t expected) {

    assert(char_traits<C>::length(str) == expected);
}

template <typename C>
constexpr void test_compare(const C* const str1, const C* const str2, const size_t n, const int expected) {

    const int actual = char_traits<C>::compare(str1, str2, n);
    assert(clamp(actual, -1, 1) == expected);
}

template <typename C>
constexpr void test_find(const C* const str, const size_t n, const C c, const ptrdiff_t expected) {

    const C* const actual = char_traits<C>::find(str, n, c);

    if (expected == -1) {
        assert(actual == nullptr);
    } else {
        assert(actual - str == expected);
    }
}

constexpr bool test() {
    test_assign('A', 'B');
    test_assign(L'C', L'D');
#ifdef __cpp_char8_t
    test_assign(u8'E', u8'F');
#endif // __cpp_char8_t
    test_assign(u'E', u'F');
    test_assign(U'G', U'H');

    test_length("", 0);
    test_length(L"", 0);
#ifdef __cpp_char8_t
    test_length(u8"", 0);
#endif // __cpp_char8_t
    test_length(u"", 0);
    test_length(U"", 0);

    test_length("cat", 3);
    test_length(L"wolf", 4);
#ifdef __cpp_char8_t
    test_length(u8"tiger", 5);
#endif // __cpp_char8_t
    test_length(u"tiger", 5);
    test_length(U"kitten", 6);

    test_compare("", "", 0, 0);
    test_compare("STUFF", "BLARG", 0, 0);
    test_compare("equal", "equal", 5, 0);
    test_compare("equalSTUFF", "equalBLARG", 5, 0);
    test_compare("greenapple", "greenzebra", 10, -1);
    test_compare("greenzebra", "greenapple", 10, 1);

    test_compare(L"", L"", 0, 0);
    test_compare(L"STUFF", L"BLARG", 0, 0);
    test_compare(L"equal", L"equal", 5, 0);
    test_compare(L"equalSTUFF", L"equalBLARG", 5, 0);
    test_compare(L"greenapple", L"greenzebra", 10, -1);
    test_compare(L"greenzebra", L"greenapple", 10, 1);

#ifdef __cpp_char8_t
    test_compare(u8"", u8"", 0, 0);
    test_compare(u8"STUFF", u8"BLARG", 0, 0);
    test_compare(u8"equal", u8"equal", 5, 0);
    test_compare(u8"equalSTUFF", u8"equalBLARG", 5, 0);
    test_compare(u8"greenapple", u8"greenzebra", 10, -1);
    test_compare(u8"greenzebra", u8"greenapple", 10, 1);
#endif // __cpp_char8_t

    test_compare(u"", u"", 0, 0);
    test_compare(u"STUFF", u"BLARG", 0, 0);
    test_compare(u"equal", u"equal", 5, 0);
    test_compare(u"equalSTUFF", u"equalBLARG", 5, 0);
    test_compare(u"greenapple", u"greenzebra", 10, -1);
    test_compare(u"greenzebra", u"greenapple", 10, 1);

    test_compare(U"", U"", 0, 0);
    test_compare(U"STUFF", U"BLARG", 0, 0);
    test_compare(U"equal", U"equal", 5, 0);
    test_compare(U"equalSTUFF", U"equalBLARG", 5, 0);
    test_compare(U"greenapple", U"greenzebra", 10, -1);
    test_compare(U"greenzebra", U"greenapple", 10, 1);

    test_find("abcdeVWXYZ", 5, 'X', -1);
    test_find("abcdeVWXYZ", 10, 'X', 7);

    test_find(L"abcdeVWXYZ", 5, L'X', -1);
    test_find(L"abcdeVWXYZ", 10, L'X', 7);

#ifdef __cpp_char8_t
    test_find(u8"abcdeVWXYZ", 5, u8'X', -1);
    test_find(u8"abcdeVWXYZ", 10, u8'X', 7);
#endif // __cpp_char8_t

    test_find(u"abcdeVWXYZ", 5, u'X', -1);
    test_find(u"abcdeVWXYZ", 10, u'X', 7);

    test_find(U"abcdeVWXYZ", 5, U'X', -1);
    test_find(U"abcdeVWXYZ", 10, U'X', 7);

    return true;
}

static_assert(test()); // test at compile-time

int main() {
    assert(test()); // test at run-time
}
