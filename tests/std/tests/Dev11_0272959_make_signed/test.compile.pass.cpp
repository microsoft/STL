// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <string>
#include <type_traits>

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

enum e1 : signed char {};
enum e2 : short {};
enum e3 : int {};
enum e4 : long {};
enum e5 : long long {};
enum e6 : unsigned char {};
enum e7 : unsigned short {};
enum e8 : unsigned int {};
enum e9 : unsigned long {};
enum e10 : unsigned long long {};
enum e11 : char {};
enum e12 : wchar_t {};
enum e13 : char16_t {};
enum e14 : char32_t {};
enum e15 : bool {};
#ifdef __cpp_char8_t
enum e16 : char8_t {};
#endif // __cpp_char8_t

template <typename T, typename S, typename U>
void helper() {
    STATIC_ASSERT(is_same_v<make_signed_t<T>, S>);
    STATIC_ASSERT(is_same_v<make_unsigned_t<T>, U>);
}

template <typename T, typename S, typename U>
void test() {
    helper<T, S, U>();
    helper<const T, const S, const U>();
    helper<volatile T, volatile S, volatile U>();
    helper<const volatile T, const volatile S, const volatile U>();
}

void example() {
    test<signed char, signed char, unsigned char>();
    test<short, short, unsigned short>();
    test<int, int, unsigned int>();
    test<long, long, unsigned long>();
    test<long long, long long, unsigned long long>();

    test<unsigned char, signed char, unsigned char>();
    test<unsigned short, short, unsigned short>();
    test<unsigned int, int, unsigned int>();
    test<unsigned long, long, unsigned long>();
    test<unsigned long long, long long, unsigned long long>();

    test<char, signed char, unsigned char>();
    test<wchar_t, short, unsigned short>();
    test<char16_t, short, unsigned short>();
    test<char32_t, int, unsigned int>();
#ifdef __cpp_char8_t
    test<char8_t, signed char, unsigned char>();
#endif // __cpp_char8_t

    test<e1, signed char, unsigned char>();
    test<e2, short, unsigned short>();
    test<e3, int, unsigned int>();
    test<e4, int, unsigned int>();
    test<e5, long long, unsigned long long>();

    test<e6, signed char, unsigned char>();
    test<e7, short, unsigned short>();
    test<e8, int, unsigned int>();
    test<e9, int, unsigned int>();
    test<e10, long long, unsigned long long>();

    test<e11, signed char, unsigned char>();
    test<e12, short, unsigned short>();
    test<e13, short, unsigned short>();
    test<e14, int, unsigned int>();
    test<e15, signed char, unsigned char>();
#ifdef __cpp_char8_t
    test<e16, signed char, unsigned char>();
#endif // __cpp_char8_t


    // Bonus tests!

    STATIC_ASSERT(is_same_v<add_lvalue_reference_t<int>, int&>);
    STATIC_ASSERT(is_same_v<add_lvalue_reference_t<int&>, int&>);
    STATIC_ASSERT(is_same_v<add_lvalue_reference_t<int&&>, int&>);

    STATIC_ASSERT(is_same_v<add_rvalue_reference_t<int>, int&&>);
    STATIC_ASSERT(is_same_v<add_rvalue_reference_t<int&>, int&>);
    STATIC_ASSERT(is_same_v<add_rvalue_reference_t<int&&>, int&&>);

    struct X {
        X& operator=(int*) {
            return *this;
        }
        X& operator=(void (*)()) {
            return *this;
        }
    };

    typedef int arr_t[5];
    typedef void(func_t)();

    STATIC_ASSERT(is_assignable_v<X&, arr_t&>);
    STATIC_ASSERT(is_assignable_v<X&, func_t&>);
    STATIC_ASSERT(is_assignable_v<X&, X&>);
    STATIC_ASSERT(!is_assignable_v<X&, void>);

    STATIC_ASSERT(is_assignable_v<int*&, arr_t&>);
    STATIC_ASSERT(!is_assignable_v<int*&, func_t&>);
    STATIC_ASSERT(is_assignable_v<void (*&)(), func_t&>);
    STATIC_ASSERT(!is_assignable_v<void (*&)(), arr_t&>);

    STATIC_ASSERT(!is_assignable_v<arr_t&, arr_t&>);
    STATIC_ASSERT(!is_assignable_v<arr_t&, const arr_t&>);
    STATIC_ASSERT(!is_assignable_v<arr_t&, arr_t&&>);
    STATIC_ASSERT(!is_assignable_v<arr_t&, const arr_t&&>);
    STATIC_ASSERT(!is_assignable_v<func_t&, func_t&>);
    STATIC_ASSERT(!is_assignable_v<func_t&, func_t&&>);

    STATIC_ASSERT(!is_assignable_v<void, void>);
    STATIC_ASSERT(!is_assignable_v<void, int>);
    STATIC_ASSERT(!is_assignable_v<void, int&>);
    STATIC_ASSERT(!is_assignable_v<void, int&&>);

    STATIC_ASSERT(!is_convertible_v<int, void>);
    STATIC_ASSERT(!is_convertible_v<void, int>);

    STATIC_ASSERT(is_convertible_v<void, void>);
    STATIC_ASSERT(is_convertible_v<void, const void>);
    STATIC_ASSERT(is_convertible_v<void, volatile void>);
    STATIC_ASSERT(is_convertible_v<void, const volatile void>);
    STATIC_ASSERT(is_convertible_v<const void, void>);
    STATIC_ASSERT(is_convertible_v<const void, const void>);
    STATIC_ASSERT(is_convertible_v<const void, volatile void>);
    STATIC_ASSERT(is_convertible_v<const void, const volatile void>);
    STATIC_ASSERT(is_convertible_v<volatile void, void>);
    STATIC_ASSERT(is_convertible_v<volatile void, const void>);
    STATIC_ASSERT(is_convertible_v<volatile void, volatile void>);
    STATIC_ASSERT(is_convertible_v<volatile void, const volatile void>);
    STATIC_ASSERT(is_convertible_v<const volatile void, void>);
    STATIC_ASSERT(is_convertible_v<const volatile void, const void>);
    STATIC_ASSERT(is_convertible_v<const volatile void, volatile void>);
    STATIC_ASSERT(is_convertible_v<const volatile void, const volatile void>);

    STATIC_ASSERT(is_trivial_v<int>);

    STATIC_ASSERT(!is_trivial_v<int&>);
    STATIC_ASSERT(!is_trivial_v<int&&>);


    // DevDiv-517460 "is_*_constructible type traits are broken for reference types"
    STATIC_ASSERT(is_copy_constructible_v<int&>);
    STATIC_ASSERT(is_copy_constructible_v<const int&>);
    STATIC_ASSERT(is_move_constructible_v<int&>);
    STATIC_ASSERT(is_move_constructible_v<const int&>);
    STATIC_ASSERT(is_move_constructible_v<int&&>);
    STATIC_ASSERT(is_move_constructible_v<const int&&>);
    STATIC_ASSERT(is_constructible_v<const float&, int>);


    // More tests for is_assignable, now that it's powered by a compiler hook.
    STATIC_ASSERT(!is_assignable_v<int, int>);
    STATIC_ASSERT(!is_assignable_v<int, const int>);
    STATIC_ASSERT(!is_assignable_v<int, int&>);
    STATIC_ASSERT(!is_assignable_v<int, const int&>);
    STATIC_ASSERT(!is_assignable_v<int, int&&>);
    STATIC_ASSERT(!is_assignable_v<int, const int&&>);

    STATIC_ASSERT(!is_assignable_v<const int, int>);
    STATIC_ASSERT(!is_assignable_v<const int, const int>);
    STATIC_ASSERT(!is_assignable_v<const int, int&>);
    STATIC_ASSERT(!is_assignable_v<const int, const int&>);
    STATIC_ASSERT(!is_assignable_v<const int, int&&>);
    STATIC_ASSERT(!is_assignable_v<const int, const int&&>);

    STATIC_ASSERT(is_assignable_v<int&, int>);
    STATIC_ASSERT(is_assignable_v<int&, const int>);
    STATIC_ASSERT(is_assignable_v<int&, int&>);
    STATIC_ASSERT(is_assignable_v<int&, const int&>);
    STATIC_ASSERT(is_assignable_v<int&, int&&>);
    STATIC_ASSERT(is_assignable_v<int&, const int&&>);

    STATIC_ASSERT(!is_assignable_v<const int&, int>);
    STATIC_ASSERT(!is_assignable_v<const int&, const int>);
    STATIC_ASSERT(!is_assignable_v<const int&, int&>);
    STATIC_ASSERT(!is_assignable_v<const int&, const int&>);
    STATIC_ASSERT(!is_assignable_v<const int&, int&&>);
    STATIC_ASSERT(!is_assignable_v<const int&, const int&&>);

    STATIC_ASSERT(!is_assignable_v<int&&, int>);
    STATIC_ASSERT(!is_assignable_v<int&&, const int>);
    STATIC_ASSERT(!is_assignable_v<int&&, int&>);
    STATIC_ASSERT(!is_assignable_v<int&&, const int&>);
    STATIC_ASSERT(!is_assignable_v<int&&, int&&>);
    STATIC_ASSERT(!is_assignable_v<int&&, const int&&>);

    STATIC_ASSERT(!is_assignable_v<const int&&, int>);
    STATIC_ASSERT(!is_assignable_v<const int&&, const int>);
    STATIC_ASSERT(!is_assignable_v<const int&&, int&>);
    STATIC_ASSERT(!is_assignable_v<const int&&, const int&>);
    STATIC_ASSERT(!is_assignable_v<const int&&, int&&>);
    STATIC_ASSERT(!is_assignable_v<const int&&, const int&&>);

    STATIC_ASSERT(is_assignable_v<int&, short>);
    STATIC_ASSERT(!is_assignable_v<int&, void*>);

    STATIC_ASSERT(is_assignable_v<string&, string>);
    STATIC_ASSERT(is_assignable_v<string&, const string>);
    STATIC_ASSERT(is_assignable_v<string&, string&>);
    STATIC_ASSERT(is_assignable_v<string&, const string&>);
    STATIC_ASSERT(is_assignable_v<string&, string&&>);
    STATIC_ASSERT(is_assignable_v<string&, const string&&>);

    STATIC_ASSERT(is_assignable_v<string&, const char*>);
    STATIC_ASSERT(is_assignable_v<string&, const char(&)[5]>);
    STATIC_ASSERT(!is_assignable_v<string&, double*>);
}
