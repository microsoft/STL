// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cstddef>
#include <functional>
#include <iterator>
#include <list>
#include <string>
#include <type_traits>
#include <vector>

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

template <bool Expected, class It1, class It2, class Pred = equal_to<>>
void assert_equal_memcmp_is_safe() {
    // Top level const should not change the answer
    STATIC_ASSERT(_Equal_memcmp_is_safe<It1, It2, Pred> == Expected);
    STATIC_ASSERT(_Equal_memcmp_is_safe<const It1, It2, Pred> == Expected);
    STATIC_ASSERT(_Equal_memcmp_is_safe<It1, const It2, Pred> == Expected);
    STATIC_ASSERT(_Equal_memcmp_is_safe<const It1, const It2, Pred> == Expected);
}

template <bool Expected, class It1, class ConstIt1, class It2, class ConstIt2, class Pred>
void test_equal_memcmp_is_safe_for_iterators() {
    // Const iterators should not change the answer
    assert_equal_memcmp_is_safe<Expected, It1, It2, Pred>();
    assert_equal_memcmp_is_safe<Expected, ConstIt1, It2, Pred>();
    assert_equal_memcmp_is_safe<Expected, It1, ConstIt2, Pred>();
    assert_equal_memcmp_is_safe<Expected, ConstIt1, ConstIt2, Pred>();
}

template <bool Expected, class Type1, class Type2, class Pred>
void test_equal_memcmp_is_safe_for_pred_helper() {
    test_equal_memcmp_is_safe_for_iterators<Expected, Type1*, const Type1*, Type2*, const Type2*, Pred>();
}

template <bool Expected, class Type1, class Type2, class Pred>
void test_equal_memcmp_is_safe_for_pred() {
    test_equal_memcmp_is_safe_for_pred_helper<Expected, Type1, Type2, Pred>();

    // No volatile
    test_equal_memcmp_is_safe_for_pred_helper<false, volatile Type1, Type2, Pred>();
    test_equal_memcmp_is_safe_for_pred_helper<false, Type1, volatile Type2, Pred>();
    test_equal_memcmp_is_safe_for_pred_helper<false, volatile Type1, volatile Type2, Pred>();
}

template <bool Expected, class Type1, class Type2>
void test_equal_memcmp_is_safe_for_types() {
    test_equal_memcmp_is_safe_for_pred<Expected, Type1, Type2, equal_to<>>();

    test_equal_memcmp_is_safe_for_pred<false, Type1, Type2, equal_to<list<int>>>();
    test_equal_memcmp_is_safe_for_pred<false, Type1, Type2, equal_to<volatile Type1>>();
    test_equal_memcmp_is_safe_for_pred<false, Type1, Type2, equal_to<const volatile Type1>>();

    test_equal_memcmp_is_safe_for_pred<false, Type1, Type2, not_equal_to<>>();

#ifdef __cpp_lib_concepts
    test_equal_memcmp_is_safe_for_pred<Expected, Type1, Type2, ranges::equal_to>();
    test_equal_memcmp_is_safe_for_pred<false, Type1, Type2, ranges::not_equal_to>();
#endif // __cpp_lib_concepts

    auto lambda = [](auto&&, auto&&) { return false; };
    test_equal_memcmp_is_safe_for_pred<false, Type1, Type2, decltype(lambda)>();
}

template <bool Expected, class Type1, class Type2>
void test_equal_memcmp_is_safe_for_pointers() {
    test_equal_memcmp_is_safe_for_types<Expected, Type1*, Type2*>();
    test_equal_memcmp_is_safe_for_types<Expected, const Type1*, Type2*>();
    test_equal_memcmp_is_safe_for_types<Expected, Type1*, const Type2*>();
    test_equal_memcmp_is_safe_for_types<Expected, const Type1*, const Type2*>();
}

template <bool Expected, class Type1, class Type2>
void test_equal_memcmp_is_safe_for_pointers_with_volatile() {
    test_equal_memcmp_is_safe_for_pointers<Expected, Type1, Type2>();
    test_equal_memcmp_is_safe_for_pointers<Expected, volatile Type1, Type2>();
    test_equal_memcmp_is_safe_for_pointers<Expected, Type1, volatile Type2>();
    test_equal_memcmp_is_safe_for_pointers<Expected, volatile Type1, volatile Type2>();
}

template <bool Expected, class Container1, class Container2, class Pred = equal_to<>>
void test_equal_memcmp_is_safe_for_containers() {
    using It1      = typename Container1::iterator;
    using ConstIt1 = typename Container1::const_iterator;
    using It2      = typename Container2::iterator;
    using ConstIt2 = typename Container2::const_iterator;
    test_equal_memcmp_is_safe_for_iterators<Expected, It1, ConstIt1, It2, ConstIt2, Pred>();
}

enum int_enum : int {};
enum short_enum : short {};
enum char_enum : char {};
enum bool_enum : bool {};

enum class int_enum_class : int {};
enum class short_enum_class : short {};
enum class char_enum_class : char {};
enum class bool_enum_class : bool {};

struct EmptyBase {};

struct EmptyDerived : EmptyBase {};

struct EmptyPrivatelyDerived : private EmptyBase {};

struct StatefulBase {
    int i;

    bool operator==(const StatefulBase& right) const noexcept {
        return i == right.i;
    }
};

struct StatefulDerived : StatefulBase, EmptyBase {};

struct StatefulPrivatelyDerived : private StatefulBase, private EmptyBase {};

struct StatefulDerived2 : EmptyBase, StatefulBase {};

struct StatefulPrivatelyDerived2 : private EmptyBase, private StatefulBase {};

#ifdef __cpp_lib_is_pointer_interconvertible
STATIC_ASSERT(is_pointer_interconvertible_base_of_v<EmptyBase, EmptyDerived>);
STATIC_ASSERT(is_pointer_interconvertible_base_of_v<EmptyBase, EmptyPrivatelyDerived>);
STATIC_ASSERT(is_pointer_interconvertible_base_of_v<StatefulBase, StatefulDerived>);
STATIC_ASSERT(!is_pointer_interconvertible_base_of_v<EmptyBase, StatefulDerived>);
STATIC_ASSERT(is_pointer_interconvertible_base_of_v<StatefulBase, StatefulPrivatelyDerived>);
STATIC_ASSERT(!is_pointer_interconvertible_base_of_v<EmptyBase, StatefulPrivatelyDerived>);
STATIC_ASSERT(is_pointer_interconvertible_base_of_v<StatefulBase, StatefulDerived2>);
STATIC_ASSERT(is_pointer_interconvertible_base_of_v<EmptyBase, StatefulDerived2>);
STATIC_ASSERT(is_pointer_interconvertible_base_of_v<StatefulBase, StatefulPrivatelyDerived2>);
STATIC_ASSERT(is_pointer_interconvertible_base_of_v<EmptyBase, StatefulPrivatelyDerived2>);
#endif // __cpp_lib_is_pointer_interconvertible

void equal_memcmp_is_safe_test_cases() {
    // Test identical integrals
    test_equal_memcmp_is_safe_for_types<true, bool, bool>();
    test_equal_memcmp_is_safe_for_types<true, signed char, signed char>();
    test_equal_memcmp_is_safe_for_types<true, unsigned char, unsigned char>();
    test_equal_memcmp_is_safe_for_types<true, short, short>();
    test_equal_memcmp_is_safe_for_types<true, unsigned short, unsigned short>();
    test_equal_memcmp_is_safe_for_types<true, int, int>();
    test_equal_memcmp_is_safe_for_types<true, unsigned int, unsigned int>();
    test_equal_memcmp_is_safe_for_types<true, long, long>();
    test_equal_memcmp_is_safe_for_types<true, unsigned long, unsigned long>();
    test_equal_memcmp_is_safe_for_types<true, long long, long long>();
    test_equal_memcmp_is_safe_for_types<true, unsigned long long, unsigned long long>();

    test_equal_memcmp_is_safe_for_types<true, char, char>();
    test_equal_memcmp_is_safe_for_types<true, wchar_t, wchar_t>();
#ifdef __cpp_lib_char8_t
    test_equal_memcmp_is_safe_for_types<true, char8_t, char8_t>();
#endif // __cpp_lib_char8_t
    test_equal_memcmp_is_safe_for_types<true, char16_t, char16_t>();
    test_equal_memcmp_is_safe_for_types<true, char32_t, char32_t>();

    // Don't allow diffrent size integrals
    test_equal_memcmp_is_safe_for_types<false, short, int>();
    test_equal_memcmp_is_safe_for_types<false, long long, int>();

    // Signedness must be the same if usual arithmetic conversions are not bits-preserving
    test_equal_memcmp_is_safe_for_types<is_signed_v<char>, char, signed char>();
    test_equal_memcmp_is_safe_for_types<is_signed_v<char>, signed char, char>();
    test_equal_memcmp_is_safe_for_types<is_unsigned_v<char>, char, unsigned char>();
    test_equal_memcmp_is_safe_for_types<is_unsigned_v<char>, unsigned char, char>();
#ifdef __cpp_lib_char8_t
    test_equal_memcmp_is_safe_for_types<is_unsigned_v<char>, char, char8_t>();
    test_equal_memcmp_is_safe_for_types<is_unsigned_v<char>, char8_t, char>();
    test_equal_memcmp_is_safe_for_types<false, char8_t, signed char>();
    test_equal_memcmp_is_safe_for_types<false, signed char, char8_t>();
#endif // __cpp_lib_char8_t
    test_equal_memcmp_is_safe_for_types<false, signed char, unsigned char>();
    test_equal_memcmp_is_safe_for_types<false, unsigned char, signed char>();
    test_equal_memcmp_is_safe_for_types<false, short, unsigned short>();
    test_equal_memcmp_is_safe_for_types<false, unsigned short, short>();

    // But if UACs don't change bits the signedness can differ
#ifdef __cpp_lib_char8_t
    test_equal_memcmp_is_safe_for_types<true, unsigned char, char8_t>();
    test_equal_memcmp_is_safe_for_types<true, char8_t, unsigned char>();
#endif // __cpp_lib_char8_t
    test_equal_memcmp_is_safe_for_types<true, int, unsigned int>();
    test_equal_memcmp_is_safe_for_types<true, unsigned int, int>();
    test_equal_memcmp_is_safe_for_types<true, long, unsigned long>();
    test_equal_memcmp_is_safe_for_types<true, unsigned long, long>();
    test_equal_memcmp_is_safe_for_types<sizeof(int) == sizeof(long), int, long>();
    test_equal_memcmp_is_safe_for_types<sizeof(int) == sizeof(long), long, int>();
    test_equal_memcmp_is_safe_for_types<sizeof(int) == sizeof(long), unsigned int, long>();
    test_equal_memcmp_is_safe_for_types<sizeof(int) == sizeof(long), unsigned long, int>();
    test_equal_memcmp_is_safe_for_types<sizeof(int) == sizeof(long), int, unsigned long>();
    test_equal_memcmp_is_safe_for_types<sizeof(int) == sizeof(long), long, unsigned int>();
    test_equal_memcmp_is_safe_for_types<sizeof(int) == sizeof(long), unsigned int, unsigned long>();
    test_equal_memcmp_is_safe_for_types<sizeof(int) == sizeof(long), unsigned long, unsigned int>();
    test_equal_memcmp_is_safe_for_types<true, long long, unsigned long long>();
    test_equal_memcmp_is_safe_for_types<true, unsigned long long, long long>();

    // It can also differ if we explicitly convert them
    test_equal_memcmp_is_safe_for_pred<true, char, signed char, equal_to<char>>();
    test_equal_memcmp_is_safe_for_pred<true, signed char, char, equal_to<unsigned char>>();
    test_equal_memcmp_is_safe_for_pred<true, char, unsigned char, equal_to<char>>();
    test_equal_memcmp_is_safe_for_pred<true, unsigned char, char, equal_to<signed char>>();
#ifdef __cpp_lib_char8_t
    test_equal_memcmp_is_safe_for_pred<true, char, char8_t, equal_to<unsigned char>>();
    test_equal_memcmp_is_safe_for_pred<true, char8_t, char, equal_to<signed char>>();
    test_equal_memcmp_is_safe_for_pred<true, char8_t, signed char, equal_to<char>>();
    test_equal_memcmp_is_safe_for_pred<true, signed char, char8_t, equal_to<char8_t>>();
#endif // __cpp_lib_char8_t
    test_equal_memcmp_is_safe_for_pred<true, signed char, unsigned char, equal_to<char>>();
    test_equal_memcmp_is_safe_for_pred<true, unsigned char, signed char, equal_to<char>>();
    test_equal_memcmp_is_safe_for_pred<true, short, unsigned short, equal_to<short>>();
    test_equal_memcmp_is_safe_for_pred<true, unsigned short, short, equal_to<unsigned short>>();

    test_equal_memcmp_is_safe_for_pred<sizeof(int) == sizeof(long), int, int, equal_to<unsigned long>>();
    test_equal_memcmp_is_safe_for_pred<sizeof(int) == sizeof(long), unsigned int, int, equal_to<unsigned long>>();
    test_equal_memcmp_is_safe_for_pred<sizeof(int) == sizeof(long), int, unsigned int, equal_to<long>>();
    test_equal_memcmp_is_safe_for_pred<sizeof(int) == sizeof(long), unsigned int, unsigned int, equal_to<long>>();
    test_equal_memcmp_is_safe_for_pred<sizeof(int) == sizeof(long), long, long, equal_to<unsigned int>>();
    test_equal_memcmp_is_safe_for_pred<sizeof(int) == sizeof(long), unsigned long, long, equal_to<unsigned int>>();
    test_equal_memcmp_is_safe_for_pred<sizeof(int) == sizeof(long), long, unsigned long, equal_to<int>>();
    test_equal_memcmp_is_safe_for_pred<sizeof(int) == sizeof(long), unsigned long, unsigned long, equal_to<int>>();

    // It is safe to use memcmp between bool and other integral types with the same size because we don't care about
    // representations other than 0 and 1
    test_equal_memcmp_is_safe_for_types<true, bool, char>();
    test_equal_memcmp_is_safe_for_types<true, char, bool>();

    // Need to be careful when pred is equal_to<bool> or equal_to<const bool>
    test_equal_memcmp_is_safe_for_pred<true, bool, bool, equal_to<bool>>();
    test_equal_memcmp_is_safe_for_pred<false, char, bool, equal_to<bool>>();
    test_equal_memcmp_is_safe_for_pred<false, bool, char, equal_to<bool>>();

    test_equal_memcmp_is_safe_for_pred<true, bool, bool, equal_to<const bool>>();
    test_equal_memcmp_is_safe_for_pred<false, char, bool, equal_to<const bool>>();
    test_equal_memcmp_is_safe_for_pred<false, bool, char, equal_to<const bool>>();

    test_equal_memcmp_is_safe_for_pred<false, char, char, equal_to<bool>>();
    test_equal_memcmp_is_safe_for_pred<false, signed char, signed char, equal_to<bool>>();
    test_equal_memcmp_is_safe_for_pred<false, unsigned char, unsigned char, equal_to<bool>>();

    // Don't allow enums and enum classes
    test_equal_memcmp_is_safe_for_types<false, int_enum, int_enum>();
    test_equal_memcmp_is_safe_for_types<false, short_enum, short_enum>();
    test_equal_memcmp_is_safe_for_types<false, char_enum, char_enum>();
    test_equal_memcmp_is_safe_for_types<false, bool_enum, bool_enum>();
    test_equal_memcmp_is_safe_for_types<false, int_enum, int>();
    test_equal_memcmp_is_safe_for_types<false, int, int_enum>();
    test_equal_memcmp_is_safe_for_types<false, short_enum, short>();
    test_equal_memcmp_is_safe_for_types<false, short, short_enum>();
    test_equal_memcmp_is_safe_for_types<false, char_enum, char>();
    test_equal_memcmp_is_safe_for_types<false, char, char_enum>();
    test_equal_memcmp_is_safe_for_types<false, bool_enum, bool>();
    test_equal_memcmp_is_safe_for_types<false, bool, bool_enum>();

    test_equal_memcmp_is_safe_for_types<false, int_enum_class, int_enum_class>();
    test_equal_memcmp_is_safe_for_types<false, short_enum_class, short_enum_class>();
    test_equal_memcmp_is_safe_for_types<false, char_enum_class, char_enum_class>();
    test_equal_memcmp_is_safe_for_types<false, bool_enum_class, bool_enum_class>();
    test_equal_memcmp_is_safe_for_types<false, int_enum_class, int>();
    test_equal_memcmp_is_safe_for_types<false, int, int_enum_class>();
    test_equal_memcmp_is_safe_for_types<false, short_enum_class, short>();
    test_equal_memcmp_is_safe_for_types<false, short, short_enum_class>();
    test_equal_memcmp_is_safe_for_types<false, char_enum_class, char>();
    test_equal_memcmp_is_safe_for_types<false, char, char_enum_class>();
    test_equal_memcmp_is_safe_for_types<false, bool_enum_class, bool>();
    test_equal_memcmp_is_safe_for_types<false, bool, bool_enum_class>();

    test_equal_memcmp_is_safe_for_types<false, bool_enum, char_enum>();
    test_equal_memcmp_is_safe_for_types<false, char_enum, bool_enum>();
    test_equal_memcmp_is_safe_for_types<false, bool_enum, char>();
    test_equal_memcmp_is_safe_for_types<false, char, bool_enum>();
    test_equal_memcmp_is_safe_for_types<false, char_enum, bool>();
    test_equal_memcmp_is_safe_for_types<false, bool, char_enum>();

    // But allow enums if we explicitly convert them
    test_equal_memcmp_is_safe_for_pred<true, int_enum, int_enum, equal_to<int>>();
    test_equal_memcmp_is_safe_for_pred<true, short_enum, short_enum, equal_to<short>>();
    test_equal_memcmp_is_safe_for_pred<true, char_enum, char_enum, equal_to<char>>();
    test_equal_memcmp_is_safe_for_pred<true, bool_enum, bool_enum, equal_to<bool>>();
    test_equal_memcmp_is_safe_for_pred<true, int_enum, int, equal_to<int>>();
    test_equal_memcmp_is_safe_for_pred<true, int, int_enum, equal_to<int>>();
    test_equal_memcmp_is_safe_for_pred<true, short_enum, short, equal_to<short>>();
    test_equal_memcmp_is_safe_for_pred<true, short, short_enum, equal_to<short>>();
    test_equal_memcmp_is_safe_for_pred<true, char_enum, char, equal_to<char>>();
    test_equal_memcmp_is_safe_for_pred<true, char, char_enum, equal_to<char>>();
    test_equal_memcmp_is_safe_for_pred<true, bool_enum, bool, equal_to<char>>();
    test_equal_memcmp_is_safe_for_pred<true, bool, bool_enum, equal_to<char>>();
    test_equal_memcmp_is_safe_for_pred<true, bool_enum, bool, equal_to<bool>>();
    test_equal_memcmp_is_safe_for_pred<true, bool, bool_enum, equal_to<bool>>();

    test_equal_memcmp_is_safe_for_pred<true, bool_enum, char_enum, equal_to<char>>();
    test_equal_memcmp_is_safe_for_pred<true, char_enum, bool_enum, equal_to<char>>();
    test_equal_memcmp_is_safe_for_pred<true, bool_enum, char, equal_to<char>>();
    test_equal_memcmp_is_safe_for_pred<true, char, bool_enum, equal_to<char>>();
    test_equal_memcmp_is_safe_for_pred<true, char_enum, bool, equal_to<char>>();
    test_equal_memcmp_is_safe_for_pred<true, bool, char_enum, equal_to<char>>();

    // Again need to be careful with equal_to<bool> and equal_to<const bool>
    test_equal_memcmp_is_safe_for_pred<false, bool_enum, char_enum, equal_to<bool>>();
    test_equal_memcmp_is_safe_for_pred<false, char_enum, bool_enum, equal_to<bool>>();
    test_equal_memcmp_is_safe_for_pred<false, bool_enum, char, equal_to<bool>>();
    test_equal_memcmp_is_safe_for_pred<false, char, bool_enum, equal_to<bool>>();
    test_equal_memcmp_is_safe_for_pred<false, char_enum, bool, equal_to<bool>>();
    test_equal_memcmp_is_safe_for_pred<false, bool, char_enum, equal_to<bool>>();

    test_equal_memcmp_is_safe_for_pred<false, bool_enum, char_enum, equal_to<const bool>>();
    test_equal_memcmp_is_safe_for_pred<false, char_enum, bool_enum, equal_to<const bool>>();
    test_equal_memcmp_is_safe_for_pred<false, bool_enum, char, equal_to<const bool>>();
    test_equal_memcmp_is_safe_for_pred<false, char, bool_enum, equal_to<const bool>>();
    test_equal_memcmp_is_safe_for_pred<false, char_enum, bool, equal_to<const bool>>();
    test_equal_memcmp_is_safe_for_pred<false, bool, char_enum, equal_to<const bool>>();

    // Enum classes are not convertible
    test_equal_memcmp_is_safe_for_pred<false, int_enum_class, int_enum_class, equal_to<int>>();
    test_equal_memcmp_is_safe_for_pred<false, short_enum_class, short_enum_class, equal_to<short>>();
    test_equal_memcmp_is_safe_for_pred<false, char_enum_class, char_enum_class, equal_to<char>>();
    test_equal_memcmp_is_safe_for_pred<false, bool_enum_class, bool_enum_class, equal_to<bool>>();
    test_equal_memcmp_is_safe_for_pred<false, int_enum_class, int, equal_to<int>>();
    test_equal_memcmp_is_safe_for_pred<false, int, int_enum_class, equal_to<int>>();
    test_equal_memcmp_is_safe_for_pred<false, short_enum_class, short, equal_to<short>>();
    test_equal_memcmp_is_safe_for_pred<false, short, short_enum_class, equal_to<short>>();
    test_equal_memcmp_is_safe_for_pred<false, char_enum_class, char, equal_to<char>>();
    test_equal_memcmp_is_safe_for_pred<false, char, char_enum_class, equal_to<char>>();
    test_equal_memcmp_is_safe_for_pred<false, bool_enum_class, bool, equal_to<char>>();
    test_equal_memcmp_is_safe_for_pred<false, bool, bool_enum_class, equal_to<char>>();
    test_equal_memcmp_is_safe_for_pred<false, bool_enum_class, bool, equal_to<bool>>();
    test_equal_memcmp_is_safe_for_pred<false, bool, bool_enum_class, equal_to<bool>>();

    test_equal_memcmp_is_safe_for_pred<false, bool_enum_class, char_enum_class, equal_to<char>>();
    test_equal_memcmp_is_safe_for_pred<false, char_enum_class, bool_enum_class, equal_to<char>>();
    test_equal_memcmp_is_safe_for_pred<false, bool_enum_class, char, equal_to<char>>();
    test_equal_memcmp_is_safe_for_pred<false, char, bool_enum_class, equal_to<char>>();
    test_equal_memcmp_is_safe_for_pred<false, char_enum_class, bool, equal_to<char>>();
    test_equal_memcmp_is_safe_for_pred<false, bool, char_enum_class, equal_to<char>>();

#ifdef __cpp_lib_byte
    // Test std::byte
    test_equal_memcmp_is_safe_for_types<true, byte, byte>();
    test_equal_memcmp_is_safe_for_types<false, byte, unsigned char>();
    test_equal_memcmp_is_safe_for_types<false, unsigned char, byte>();
#endif // __cpp_lib_byte

    // Don't allow floating point
    test_equal_memcmp_is_safe_for_types<false, float, float>();
    test_equal_memcmp_is_safe_for_types<false, double, double>();
    test_equal_memcmp_is_safe_for_types<false, double, long double>();
    test_equal_memcmp_is_safe_for_types<false, long double, double>();

    test_equal_memcmp_is_safe_for_types<false, float, int>();
    test_equal_memcmp_is_safe_for_types<false, int, float>();

    // Test pointers
    test_equal_memcmp_is_safe_for_pointers_with_volatile<true, int, int>();
    test_equal_memcmp_is_safe_for_pointers_with_volatile<true, void, void>();
    test_equal_memcmp_is_safe_for_pointers_with_volatile<true, int, void>();
    test_equal_memcmp_is_safe_for_pointers_with_volatile<true, void, int>();

    test_equal_memcmp_is_safe_for_pointers<false, int, long>();
    test_equal_memcmp_is_safe_for_pointers<false, unsigned int, int>();

    test_equal_memcmp_is_safe_for_pred<true, int*, int*, equal_to<int*>>();
    test_equal_memcmp_is_safe_for_pred<true, int*, int*, equal_to<const int*>>();
    test_equal_memcmp_is_safe_for_pred<true, int*, int*, equal_to<volatile int*>>();
    test_equal_memcmp_is_safe_for_pred<true, int*, int*, equal_to<const volatile int*>>();

    test_equal_memcmp_is_safe_for_pred<false, const int*, int*, equal_to<int*>>();
    test_equal_memcmp_is_safe_for_pred<true, const int*, int*, equal_to<const int*>>();
    test_equal_memcmp_is_safe_for_pred<false, const int*, int*, equal_to<volatile int*>>();
    test_equal_memcmp_is_safe_for_pred<true, const int*, int*, equal_to<const volatile int*>>();

    test_equal_memcmp_is_safe_for_pred<false, volatile int*, int*, equal_to<int*>>();
    test_equal_memcmp_is_safe_for_pred<false, volatile int*, int*, equal_to<const int*>>();
    test_equal_memcmp_is_safe_for_pred<true, volatile int*, int*, equal_to<volatile int*>>();
    test_equal_memcmp_is_safe_for_pred<true, volatile int*, int*, equal_to<const volatile int*>>();

    test_equal_memcmp_is_safe_for_pred<true, int*, int*, equal_to<void*>>();
    test_equal_memcmp_is_safe_for_pred<true, int*, int*, equal_to<const void*>>();
    test_equal_memcmp_is_safe_for_pred<true, int*, int*, equal_to<volatile void*>>();
    test_equal_memcmp_is_safe_for_pred<true, int*, int*, equal_to<const volatile void*>>();

    test_equal_memcmp_is_safe_for_pred<false, const int*, int*, equal_to<void*>>();
    test_equal_memcmp_is_safe_for_pred<true, const int*, int*, equal_to<const void*>>();
    test_equal_memcmp_is_safe_for_pred<false, const int*, int*, equal_to<volatile void*>>();
    test_equal_memcmp_is_safe_for_pred<true, const int*, int*, equal_to<const volatile void*>>();

    test_equal_memcmp_is_safe_for_pred<false, volatile int*, int*, equal_to<void*>>();
    test_equal_memcmp_is_safe_for_pred<false, volatile int*, int*, equal_to<const void*>>();
    test_equal_memcmp_is_safe_for_pred<true, volatile int*, int*, equal_to<volatile void*>>();
    test_equal_memcmp_is_safe_for_pred<true, volatile int*, int*, equal_to<const volatile void*>>();

    test_equal_memcmp_is_safe_for_pred<true, int*, void*, equal_to<void*>>();
    test_equal_memcmp_is_safe_for_pred<true, int*, void*, equal_to<const void*>>();
    test_equal_memcmp_is_safe_for_pred<true, int*, void*, equal_to<volatile void*>>();
    test_equal_memcmp_is_safe_for_pred<true, int*, void*, equal_to<const volatile void*>>();

    test_equal_memcmp_is_safe_for_pred<false, const int*, void*, equal_to<void*>>();
    test_equal_memcmp_is_safe_for_pred<true, const int*, void*, equal_to<const void*>>();
    test_equal_memcmp_is_safe_for_pred<false, const int*, void*, equal_to<volatile void*>>();
    test_equal_memcmp_is_safe_for_pred<true, const int*, void*, equal_to<const volatile void*>>();

    test_equal_memcmp_is_safe_for_pred<false, volatile int*, void*, equal_to<void*>>();
    test_equal_memcmp_is_safe_for_pred<false, volatile int*, void*, equal_to<const void*>>();
    test_equal_memcmp_is_safe_for_pred<true, volatile int*, void*, equal_to<volatile void*>>();
    test_equal_memcmp_is_safe_for_pred<true, volatile int*, void*, equal_to<const volatile void*>>();

    test_equal_memcmp_is_safe_for_pred<true, void*, void*, equal_to<void*>>();
    test_equal_memcmp_is_safe_for_pred<true, void*, void*, equal_to<const void*>>();
    test_equal_memcmp_is_safe_for_pred<true, void*, void*, equal_to<volatile void*>>();
    test_equal_memcmp_is_safe_for_pred<true, void*, void*, equal_to<const volatile void*>>();

    test_equal_memcmp_is_safe_for_pred<false, const void*, void*, equal_to<void*>>();
    test_equal_memcmp_is_safe_for_pred<true, const void*, void*, equal_to<const void*>>();
    test_equal_memcmp_is_safe_for_pred<false, const void*, void*, equal_to<volatile void*>>();
    test_equal_memcmp_is_safe_for_pred<true, const void*, void*, equal_to<const volatile void*>>();

    test_equal_memcmp_is_safe_for_pred<false, volatile void*, void*, equal_to<void*>>();
    test_equal_memcmp_is_safe_for_pred<false, volatile void*, void*, equal_to<const void*>>();
    test_equal_memcmp_is_safe_for_pred<true, volatile void*, void*, equal_to<volatile void*>>();
    test_equal_memcmp_is_safe_for_pred<true, volatile void*, void*, equal_to<const volatile void*>>();

    // Test pointers to derived classes
    constexpr bool enable_derived_to_base =
#ifdef __cpp_lib_is_pointer_interconvertible
        true
#else // ^^^ __cpp_lib_is_pointer_interconvertible ^^^ / vvv !__cpp_lib_is_pointer_interconvertible vvv
        false
#endif // ^^^ !__cpp_lib_is_pointer_interconvertible ^^^
        ;

    test_equal_memcmp_is_safe_for_pointers<true, EmptyBase, EmptyBase>();
    test_equal_memcmp_is_safe_for_pointers<true, EmptyDerived, EmptyDerived>();
    test_equal_memcmp_is_safe_for_pointers<enable_derived_to_base, EmptyDerived, EmptyBase>();
    test_equal_memcmp_is_safe_for_pointers<enable_derived_to_base, EmptyBase, EmptyDerived>();
    test_equal_memcmp_is_safe_for_pointers<enable_derived_to_base, StatefulDerived, StatefulBase>();
    test_equal_memcmp_is_safe_for_pointers<enable_derived_to_base, StatefulBase, StatefulDerived>();
    test_equal_memcmp_is_safe_for_pointers<false, StatefulDerived, EmptyBase>();
    test_equal_memcmp_is_safe_for_pointers<false, EmptyBase, StatefulDerived>();
    test_equal_memcmp_is_safe_for_pointers<enable_derived_to_base, StatefulDerived2, StatefulBase>();
    test_equal_memcmp_is_safe_for_pointers<enable_derived_to_base, StatefulBase, StatefulDerived2>();
    test_equal_memcmp_is_safe_for_pointers<enable_derived_to_base, StatefulDerived2, EmptyBase>();
    test_equal_memcmp_is_safe_for_pointers<enable_derived_to_base, EmptyBase, StatefulDerived2>();

    // Don't allow privately derived
    test_equal_memcmp_is_safe_for_pointers<false, EmptyPrivatelyDerived, EmptyBase>();
    test_equal_memcmp_is_safe_for_pointers<false, EmptyBase, EmptyPrivatelyDerived>();
    test_equal_memcmp_is_safe_for_pointers<false, StatefulPrivatelyDerived, StatefulBase>();
    test_equal_memcmp_is_safe_for_pointers<false, StatefulBase, StatefulPrivatelyDerived>();
    test_equal_memcmp_is_safe_for_pointers<false, StatefulPrivatelyDerived2, StatefulBase>();
    test_equal_memcmp_is_safe_for_pointers<false, StatefulBase, StatefulPrivatelyDerived2>();
    test_equal_memcmp_is_safe_for_pointers<false, StatefulPrivatelyDerived2, EmptyBase>();
    test_equal_memcmp_is_safe_for_pointers<false, EmptyBase, StatefulPrivatelyDerived2>();

    // Test function pointers
    test_equal_memcmp_is_safe_for_types<true, void (*)(int), void (*)(int)>();
    test_equal_memcmp_is_safe_for_types<false, void (*)(int), void (*)()>();
    test_equal_memcmp_is_safe_for_types<false, void (*)(), void (*)(int)>();

    // Converting from function pointers to void pointers is a non-standard extension
    test_equal_memcmp_is_safe_for_types<is_convertible_v<void (*)(int), void*>, void (*)(int), void*>();
    test_equal_memcmp_is_safe_for_types<is_convertible_v<void (*)(int), void*>, void*, void (*)(int)>();

    // Don't allow member object pointers
    test_equal_memcmp_is_safe_for_types<false, int EmptyBase::*, int EmptyBase::*>();
    test_equal_memcmp_is_safe_for_types<false, int EmptyDerived::*, int EmptyDerived::*>();
    test_equal_memcmp_is_safe_for_types<false, int EmptyBase::*, int EmptyDerived::*>();
    test_equal_memcmp_is_safe_for_types<false, int EmptyDerived::*, int EmptyBase::*>();

    // Don't allow member function pointers
    test_equal_memcmp_is_safe_for_types<false, int (EmptyBase::*)(), int (EmptyBase::*)()>();
    test_equal_memcmp_is_safe_for_types<false, int (EmptyDerived::*)(), int (EmptyDerived::*)()>();
    test_equal_memcmp_is_safe_for_types<false, int (EmptyBase::*)(), int (EmptyDerived::*)()>();
    test_equal_memcmp_is_safe_for_types<false, int (EmptyDerived::*)(), int (EmptyBase::*)()>();

    // Don't allow user-defined types
    test_equal_memcmp_is_safe_for_types<false, StatefulBase, StatefulBase>();

    // Test _Char_traits_eq
    test_equal_memcmp_is_safe_for_pred<true, char, char, _Char_traits_eq<char_traits<char>>>();
    test_equal_memcmp_is_safe_for_pred<true, wchar_t, wchar_t, _Char_traits_eq<char_traits<wchar_t>>>();
#ifdef __cpp_lib_char8_t
    test_equal_memcmp_is_safe_for_pred<true, char8_t, char8_t, _Char_traits_eq<char_traits<char8_t>>>();
#endif // __cpp_lib_char8_t
    test_equal_memcmp_is_safe_for_pred<true, char16_t, char16_t, _Char_traits_eq<char_traits<char16_t>>>();
    test_equal_memcmp_is_safe_for_pred<true, char32_t, char32_t, _Char_traits_eq<char_traits<char32_t>>>();

    // Test different containers
#ifdef __cpp_lib_concepts
    test_equal_memcmp_is_safe_for_containers<true, vector<int>, vector<int>>();
    test_equal_memcmp_is_safe_for_containers<true, array<int, 8>, array<int, 8>>();
    test_equal_memcmp_is_safe_for_containers<true, vector<int>, array<int, 8>>();
    test_equal_memcmp_is_safe_for_containers<true, array<int, 8>, vector<int>>();
#endif // __cpp_lib_concepts

    test_equal_memcmp_is_safe_for_containers<false, list<int>, list<int>>();
    test_equal_memcmp_is_safe_for_containers<false, vector<int>, list<int>>();
    test_equal_memcmp_is_safe_for_containers<false, list<int>, vector<int>>();

#ifdef __cpp_lib_concepts
    // Test counted_iterator
    assert_equal_memcmp_is_safe<true, counted_iterator<int*>, int*>();
    assert_equal_memcmp_is_safe<true, int*, counted_iterator<int*>>();
    assert_equal_memcmp_is_safe<true, counted_iterator<int*>, counted_iterator<int*>>();
#endif // __cpp_lib_concepts
}

int main() {} // COMPILE-ONLY
