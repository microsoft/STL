// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cstddef>
#include <functional>
#include <iterator>
#include <list>
#include <regex>
#include <string>
#include <type_traits>
#include <vector>

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

template <bool Expected, class It1, class It2, class Pred = equal_to<>>
[[nodiscard]] constexpr bool assert_equal_memcmp_is_safe() {
    // Top level const should not change the answer
    STATIC_ASSERT(_Equal_memcmp_is_safe<It1, It2, Pred> == Expected);
    STATIC_ASSERT(_Equal_memcmp_is_safe<const It1, It2, Pred> == Expected);
    STATIC_ASSERT(_Equal_memcmp_is_safe<It1, const It2, Pred> == Expected);
    STATIC_ASSERT(_Equal_memcmp_is_safe<const It1, const It2, Pred> == Expected);
    return true;
}

template <bool Expected, class It1, class ConstIt1, class It2, class ConstIt2, class Pred>
[[nodiscard]] constexpr bool test_equal_memcmp_is_safe_for_iterators() {
    // Const iterators should not change the answer
    STATIC_ASSERT(assert_equal_memcmp_is_safe<Expected, It1, It2, Pred>());
    STATIC_ASSERT(assert_equal_memcmp_is_safe<Expected, ConstIt1, It2, Pred>());
    STATIC_ASSERT(assert_equal_memcmp_is_safe<Expected, It1, ConstIt2, Pred>());
    STATIC_ASSERT(assert_equal_memcmp_is_safe<Expected, ConstIt1, ConstIt2, Pred>());
    return true;
}

template <bool Expected, class Type1, class Type2, class Pred>
[[nodiscard]] constexpr bool test_equal_memcmp_is_safe_for_pred_helper() {
    STATIC_ASSERT(
        test_equal_memcmp_is_safe_for_iterators<Expected, Type1*, const Type1*, Type2*, const Type2*, Pred>());
    return true;
}

template <bool Expected, class Type1, class Type2, class Pred>
[[nodiscard]] constexpr bool test_equal_memcmp_is_safe_for_pred() {
    STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred_helper<Expected, Type1, Type2, Pred>());

    // No volatile
    STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred_helper<false, volatile Type1, Type2, Pred>());
    STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred_helper<false, Type1, volatile Type2, Pred>());
    STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred_helper<false, volatile Type1, volatile Type2, Pred>());
    return true;
}

template <bool Expected, class Type1, class Type2>
[[nodiscard]] constexpr bool test_equal_memcmp_is_safe_for_types() {
    STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<Expected, Type1, Type2, equal_to<>>());

    STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, Type1, Type2, equal_to<list<int>>>());
    STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, Type1, Type2, equal_to<volatile Type1>>());
    STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, Type1, Type2, equal_to<const volatile Type1>>());

    STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, Type1, Type2, not_equal_to<>>());

#ifdef __cpp_lib_concepts
    STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<Expected, Type1, Type2, ranges::equal_to>());
    STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, Type1, Type2, ranges::not_equal_to>());
#endif // __cpp_lib_concepts

#if _HAS_CXX17
    auto lambda = [](auto&&, auto&&) { return false; };
    STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, Type1, Type2, decltype(lambda)>());
#endif // _HAS_CXX17

    return true;
}

template <bool Expected, class Type1, class Type2>
[[nodiscard]] constexpr bool test_equal_memcmp_is_safe_for_pointers() {
    STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<Expected, Type1*, Type2*>());
    STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<Expected, const Type1*, Type2*>());
    STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<Expected, Type1*, const Type2*>());
    STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<Expected, const Type1*, const Type2*>());
    return true;
}

template <bool Expected, class Type1, class Type2>
[[nodiscard]] constexpr bool test_equal_memcmp_is_safe_for_pointers_with_volatile() {
    STATIC_ASSERT(test_equal_memcmp_is_safe_for_pointers<Expected, Type1, Type2>());
    STATIC_ASSERT(test_equal_memcmp_is_safe_for_pointers<Expected, volatile Type1, Type2>());
    STATIC_ASSERT(test_equal_memcmp_is_safe_for_pointers<Expected, Type1, volatile Type2>());
    STATIC_ASSERT(test_equal_memcmp_is_safe_for_pointers<Expected, volatile Type1, volatile Type2>());
    return true;
}

template <bool Expected, class Container1, class Container2, class Pred = equal_to<>>
[[nodiscard]] constexpr bool test_equal_memcmp_is_safe_for_containers() {
    using It1      = typename Container1::iterator;
    using ConstIt1 = typename Container1::const_iterator;
    using It2      = typename Container2::iterator;
    using ConstIt2 = typename Container2::const_iterator;
    STATIC_ASSERT(test_equal_memcmp_is_safe_for_iterators<Expected, It1, ConstIt1, It2, ConstIt2, Pred>());
    return true;
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

// Test identical integrals
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<true, bool, bool>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<true, signed char, signed char>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<true, unsigned char, unsigned char>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<true, short, short>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<true, unsigned short, unsigned short>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<true, int, int>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<true, unsigned int, unsigned int>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<true, long, long>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<true, unsigned long, unsigned long>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<true, long long, long long>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<true, unsigned long long, unsigned long long>());

STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<true, char, char>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<true, wchar_t, wchar_t>());
#ifdef __cpp_lib_char8_t
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<true, char8_t, char8_t>());
#endif // __cpp_lib_char8_t
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<true, char16_t, char16_t>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<true, char32_t, char32_t>());

// Don't allow diffrent size integrals
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, short, int>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, long long, int>());

// Signedness must be the same if usual arithmetic conversions are not bits-preserving
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<is_signed_v<char>, char, signed char>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<is_signed_v<char>, signed char, char>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<is_unsigned_v<char>, char, unsigned char>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<is_unsigned_v<char>, unsigned char, char>());
#ifdef __cpp_lib_char8_t
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<is_unsigned_v<char>, char, char8_t>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<is_unsigned_v<char>, char8_t, char>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, char8_t, signed char>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, signed char, char8_t>());
#endif // __cpp_lib_char8_t
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, signed char, unsigned char>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, unsigned char, signed char>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, short, unsigned short>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, unsigned short, short>());

// But if UACs don't change bits the signedness can differ
#ifdef __cpp_lib_char8_t
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<true, unsigned char, char8_t>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<true, char8_t, unsigned char>());
#endif // __cpp_lib_char8_t
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<true, int, unsigned int>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<true, unsigned int, int>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<true, long, unsigned long>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<true, unsigned long, long>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<sizeof(int) == sizeof(long), int, long>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<sizeof(int) == sizeof(long), long, int>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<sizeof(int) == sizeof(long), unsigned int, long>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<sizeof(int) == sizeof(long), unsigned long, int>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<sizeof(int) == sizeof(long), int, unsigned long>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<sizeof(int) == sizeof(long), long, unsigned int>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<sizeof(int) == sizeof(long), unsigned int, unsigned long>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<sizeof(int) == sizeof(long), unsigned long, unsigned int>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<true, long long, unsigned long long>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<true, unsigned long long, long long>());

// It can also differ if we explicitly convert them
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, char, signed char, equal_to<char>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, signed char, char, equal_to<unsigned char>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, char, unsigned char, equal_to<char>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, unsigned char, char, equal_to<signed char>>());
#ifdef __cpp_lib_char8_t
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, char, char8_t, equal_to<unsigned char>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, char8_t, char, equal_to<signed char>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, char8_t, signed char, equal_to<char>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, signed char, char8_t, equal_to<char8_t>>());
#endif // __cpp_lib_char8_t
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, signed char, unsigned char, equal_to<char>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, unsigned char, signed char, equal_to<char>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, short, unsigned short, equal_to<short>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, unsigned short, short, equal_to<unsigned short>>());

STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<sizeof(int) == sizeof(long), int, int, equal_to<unsigned long>>());
STATIC_ASSERT(
    test_equal_memcmp_is_safe_for_pred<sizeof(int) == sizeof(long), unsigned int, int, equal_to<unsigned long>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<sizeof(int) == sizeof(long), int, unsigned int, equal_to<long>>());
STATIC_ASSERT(
    test_equal_memcmp_is_safe_for_pred<sizeof(int) == sizeof(long), unsigned int, unsigned int, equal_to<long>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<sizeof(int) == sizeof(long), long, long, equal_to<unsigned int>>());
STATIC_ASSERT(
    test_equal_memcmp_is_safe_for_pred<sizeof(int) == sizeof(long), unsigned long, long, equal_to<unsigned int>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<sizeof(int) == sizeof(long), long, unsigned long, equal_to<int>>());
STATIC_ASSERT(
    test_equal_memcmp_is_safe_for_pred<sizeof(int) == sizeof(long), unsigned long, unsigned long, equal_to<int>>());

// It is safe to use memcmp between bool and other integral types with the same size because we don't care about
// representations other than 0 and 1
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<true, bool, char>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<true, char, bool>());

// Need to be careful when pred is equal_to<bool> or equal_to<const bool>
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, bool, bool, equal_to<bool>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, char, bool, equal_to<bool>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, bool, char, equal_to<bool>>());

STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, bool, bool, equal_to<const bool>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, char, bool, equal_to<const bool>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, bool, char, equal_to<const bool>>());

STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, char, char, equal_to<bool>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, signed char, signed char, equal_to<bool>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, unsigned char, unsigned char, equal_to<bool>>());

// Don't allow enums and enum classes
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, int_enum, int_enum>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, short_enum, short_enum>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, char_enum, char_enum>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, bool_enum, bool_enum>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, int_enum, int>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, int, int_enum>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, short_enum, short>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, short, short_enum>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, char_enum, char>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, char, char_enum>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, bool_enum, bool>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, bool, bool_enum>());

STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, int_enum_class, int_enum_class>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, short_enum_class, short_enum_class>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, char_enum_class, char_enum_class>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, bool_enum_class, bool_enum_class>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, int_enum_class, int>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, int, int_enum_class>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, short_enum_class, short>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, short, short_enum_class>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, char_enum_class, char>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, char, char_enum_class>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, bool_enum_class, bool>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, bool, bool_enum_class>());

STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, bool_enum, char_enum>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, char_enum, bool_enum>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, bool_enum, char>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, char, bool_enum>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, char_enum, bool>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, bool, char_enum>());

// But allow enums if we explicitly convert them
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, int_enum, int_enum, equal_to<int>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, short_enum, short_enum, equal_to<short>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, char_enum, char_enum, equal_to<char>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, bool_enum, bool_enum, equal_to<bool>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, int_enum, int, equal_to<int>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, int, int_enum, equal_to<int>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, short_enum, short, equal_to<short>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, short, short_enum, equal_to<short>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, char_enum, char, equal_to<char>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, char, char_enum, equal_to<char>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, bool_enum, bool, equal_to<char>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, bool, bool_enum, equal_to<char>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, bool_enum, bool, equal_to<bool>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, bool, bool_enum, equal_to<bool>>());

STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, bool_enum, char_enum, equal_to<char>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, char_enum, bool_enum, equal_to<char>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, bool_enum, char, equal_to<char>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, char, bool_enum, equal_to<char>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, char_enum, bool, equal_to<char>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, bool, char_enum, equal_to<char>>());

// Again need to be careful with equal_to<bool> and equal_to<const bool>
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, bool_enum, char_enum, equal_to<bool>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, char_enum, bool_enum, equal_to<bool>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, bool_enum, char, equal_to<bool>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, char, bool_enum, equal_to<bool>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, char_enum, bool, equal_to<bool>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, bool, char_enum, equal_to<bool>>());

STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, bool_enum, char_enum, equal_to<const bool>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, char_enum, bool_enum, equal_to<const bool>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, bool_enum, char, equal_to<const bool>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, char, bool_enum, equal_to<const bool>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, char_enum, bool, equal_to<const bool>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, bool, char_enum, equal_to<const bool>>());

// Enum classes are not convertible
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, int_enum_class, int_enum_class, equal_to<int>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, short_enum_class, short_enum_class, equal_to<short>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, char_enum_class, char_enum_class, equal_to<char>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, bool_enum_class, bool_enum_class, equal_to<bool>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, int_enum_class, int, equal_to<int>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, int, int_enum_class, equal_to<int>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, short_enum_class, short, equal_to<short>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, short, short_enum_class, equal_to<short>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, char_enum_class, char, equal_to<char>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, char, char_enum_class, equal_to<char>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, bool_enum_class, bool, equal_to<char>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, bool, bool_enum_class, equal_to<char>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, bool_enum_class, bool, equal_to<bool>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, bool, bool_enum_class, equal_to<bool>>());

STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, bool_enum_class, char_enum_class, equal_to<char>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, char_enum_class, bool_enum_class, equal_to<char>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, bool_enum_class, char, equal_to<char>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, char, bool_enum_class, equal_to<char>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, char_enum_class, bool, equal_to<char>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, bool, char_enum_class, equal_to<char>>());

#ifdef __cpp_lib_byte
// Test std::byte
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<true, byte, byte>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, byte, unsigned char>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, unsigned char, byte>());
#endif // __cpp_lib_byte

// Don't allow floating point
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, float, float>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, double, double>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, double, long double>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, long double, double>());

STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, float, int>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, int, float>());

// Test pointers
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pointers_with_volatile<true, int, int>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pointers_with_volatile<true, void, void>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pointers_with_volatile<true, int, void>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pointers_with_volatile<true, void, int>());

STATIC_ASSERT(test_equal_memcmp_is_safe_for_pointers<false, int, long>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pointers<false, unsigned int, int>());

STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, int*, int*, equal_to<int*>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, int*, int*, equal_to<const int*>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, int*, int*, equal_to<volatile int*>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, int*, int*, equal_to<const volatile int*>>());

STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, const int*, int*, equal_to<int*>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, const int*, int*, equal_to<const int*>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, const int*, int*, equal_to<volatile int*>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, const int*, int*, equal_to<const volatile int*>>());

STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, volatile int*, int*, equal_to<int*>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, volatile int*, int*, equal_to<const int*>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, volatile int*, int*, equal_to<volatile int*>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, volatile int*, int*, equal_to<const volatile int*>>());

STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, int*, int*, equal_to<void*>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, int*, int*, equal_to<const void*>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, int*, int*, equal_to<volatile void*>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, int*, int*, equal_to<const volatile void*>>());

STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, const int*, int*, equal_to<void*>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, const int*, int*, equal_to<const void*>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, const int*, int*, equal_to<volatile void*>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, const int*, int*, equal_to<const volatile void*>>());

STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, volatile int*, int*, equal_to<void*>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, volatile int*, int*, equal_to<const void*>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, volatile int*, int*, equal_to<volatile void*>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, volatile int*, int*, equal_to<const volatile void*>>());

STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, int*, void*, equal_to<void*>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, int*, void*, equal_to<const void*>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, int*, void*, equal_to<volatile void*>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, int*, void*, equal_to<const volatile void*>>());

STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, const int*, void*, equal_to<void*>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, const int*, void*, equal_to<const void*>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, const int*, void*, equal_to<volatile void*>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, const int*, void*, equal_to<const volatile void*>>());

STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, volatile int*, void*, equal_to<void*>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, volatile int*, void*, equal_to<const void*>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, volatile int*, void*, equal_to<volatile void*>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, volatile int*, void*, equal_to<const volatile void*>>());

STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, void*, void*, equal_to<void*>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, void*, void*, equal_to<const void*>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, void*, void*, equal_to<volatile void*>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, void*, void*, equal_to<const volatile void*>>());

STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, const void*, void*, equal_to<void*>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, const void*, void*, equal_to<const void*>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, const void*, void*, equal_to<volatile void*>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, const void*, void*, equal_to<const volatile void*>>());

STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, volatile void*, void*, equal_to<void*>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<false, volatile void*, void*, equal_to<const void*>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, volatile void*, void*, equal_to<volatile void*>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, volatile void*, void*, equal_to<const volatile void*>>());

// Test pointers to derived classes
constexpr bool enable_derived_to_base =
#ifdef __cpp_lib_is_pointer_interconvertible
    true
#else // ^^^ __cpp_lib_is_pointer_interconvertible / !__cpp_lib_is_pointer_interconvertible vvv
    false
#endif // ^^^ !__cpp_lib_is_pointer_interconvertible ^^^
    ;

STATIC_ASSERT(test_equal_memcmp_is_safe_for_pointers<true, EmptyBase, EmptyBase>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pointers<true, EmptyDerived, EmptyDerived>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pointers<enable_derived_to_base, EmptyDerived, EmptyBase>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pointers<enable_derived_to_base, EmptyBase, EmptyDerived>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pointers<enable_derived_to_base, StatefulDerived, StatefulBase>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pointers<enable_derived_to_base, StatefulBase, StatefulDerived>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pointers<false, StatefulDerived, EmptyBase>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pointers<false, EmptyBase, StatefulDerived>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pointers<enable_derived_to_base, StatefulDerived2, StatefulBase>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pointers<enable_derived_to_base, StatefulBase, StatefulDerived2>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pointers<enable_derived_to_base, StatefulDerived2, EmptyBase>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pointers<enable_derived_to_base, EmptyBase, StatefulDerived2>());

// Don't allow privately derived
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pointers<false, EmptyPrivatelyDerived, EmptyBase>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pointers<false, EmptyBase, EmptyPrivatelyDerived>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pointers<false, StatefulPrivatelyDerived, StatefulBase>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pointers<false, StatefulBase, StatefulPrivatelyDerived>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pointers<false, StatefulPrivatelyDerived2, StatefulBase>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pointers<false, StatefulBase, StatefulPrivatelyDerived2>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pointers<false, StatefulPrivatelyDerived2, EmptyBase>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pointers<false, EmptyBase, StatefulPrivatelyDerived2>());

// Test function pointers
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<true, void (*)(int), void (*)(int)>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, void (*)(int), void (*)()>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, void (*)(), void (*)(int)>());

// Converting from function pointers to void pointers is a non-standard extension
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<is_convertible_v<void (*)(int), void*>, void (*)(int), void*>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<is_convertible_v<void (*)(int), void*>, void*, void (*)(int)>());

// Don't allow member object pointers
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, int EmptyBase::*, int EmptyBase::*>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, int EmptyDerived::*, int EmptyDerived::*>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, int EmptyBase::*, int EmptyDerived::*>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, int EmptyDerived::*, int EmptyBase::*>());

// Don't allow member function pointers
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, int (EmptyBase::*)(), int (EmptyBase::*)()>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, int (EmptyDerived::*)(), int (EmptyDerived::*)()>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, int (EmptyBase::*)(), int (EmptyDerived::*)()>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, int (EmptyDerived::*)(), int (EmptyBase::*)()>());

// Don't allow user-defined types
STATIC_ASSERT(test_equal_memcmp_is_safe_for_types<false, StatefulBase, StatefulBase>());

// Test _Char_traits_eq
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, char, char, _Char_traits_eq<char_traits<char>>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, wchar_t, wchar_t, _Char_traits_eq<char_traits<wchar_t>>>());
#ifdef __cpp_lib_char8_t
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, char8_t, char8_t, _Char_traits_eq<char_traits<char8_t>>>());
#endif // __cpp_lib_char8_t
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, char16_t, char16_t, _Char_traits_eq<char_traits<char16_t>>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_pred<true, char32_t, char32_t, _Char_traits_eq<char_traits<char32_t>>>());

// Test different containers
#ifdef __cpp_lib_concepts
STATIC_ASSERT(test_equal_memcmp_is_safe_for_containers<true, vector<int>, vector<int>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_containers<true, array<int, 8>, array<int, 8>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_containers<true, vector<int>, array<int, 8>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_containers<true, array<int, 8>, vector<int>>());
#endif // __cpp_lib_concepts

STATIC_ASSERT(test_equal_memcmp_is_safe_for_containers<false, list<int>, list<int>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_containers<false, vector<int>, list<int>>());
STATIC_ASSERT(test_equal_memcmp_is_safe_for_containers<false, list<int>, vector<int>>());

#ifdef __cpp_lib_concepts
// Test counted_iterator
STATIC_ASSERT(assert_equal_memcmp_is_safe<true, counted_iterator<int*>, int*>());
STATIC_ASSERT(assert_equal_memcmp_is_safe<true, int*, counted_iterator<int*>>());
STATIC_ASSERT(assert_equal_memcmp_is_safe<true, counted_iterator<int*>, counted_iterator<int*>>());
#endif // __cpp_lib_concepts
