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

#ifdef __cpp_lib_concepts
#include <compare>
#endif // __cpp_lib_concepts

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

template <class Expected, class It1, class It2, class Pred>
void assert_lex_compare_memcmp_classify() {
    STATIC_ASSERT(is_same_v<_Lex_compare_memcmp_classify<It1, It2, Pred>, Expected>);
}

#ifdef __cpp_lib_concepts
template <class Expected, class It1, class It2, class Comp>
void assert_lex_compare_three_way_memcmp_classify() {
    STATIC_ASSERT(is_same_v<_Lex_compare_three_way_memcmp_classify<It1, It2, Comp>, Expected>);
}
#endif // __cpp_lib_concepts

template <class Expected, class It1, class ConstIt1, class It2, class ConstIt2, class Pred>
void test_lex_compare_memcmp_classify_for_iterators() {
    assert_lex_compare_memcmp_classify<Expected, It1, It2, Pred>();
    assert_lex_compare_memcmp_classify<Expected, ConstIt1, It2, Pred>();
    assert_lex_compare_memcmp_classify<Expected, It1, ConstIt2, Pred>();
    assert_lex_compare_memcmp_classify<Expected, ConstIt1, ConstIt2, Pred>();
}

#ifdef __cpp_lib_concepts
template <class Expected, class It1, class ConstIt1, class It2, class ConstIt2, class Comp>
void test_lex_compare_three_way_memcmp_classify_for_iterators() {
    assert_lex_compare_three_way_memcmp_classify<Expected, It1, It2, Comp>();
    assert_lex_compare_three_way_memcmp_classify<Expected, ConstIt1, It2, Comp>();
    assert_lex_compare_three_way_memcmp_classify<Expected, It1, ConstIt2, Comp>();
    assert_lex_compare_three_way_memcmp_classify<Expected, ConstIt1, ConstIt2, Comp>();
}
#endif // __cpp_lib_concepts

template <class Expected, class Type1, class Type2, class Pred>
void test_lex_compare_memcmp_classify_for_pred_helper() {
    test_lex_compare_memcmp_classify_for_iterators<Expected, Type1*, const Type1*, Type2*, const Type2*, Pred>();
}

#ifdef __cpp_lib_concepts
template <class Expected, class Type1, class Type2, class Comp>
void test_lex_compare_three_way_memcmp_classify_for_comp_helper() {
    test_lex_compare_three_way_memcmp_classify_for_iterators<Expected, Type1*, const Type1*, Type2*, const Type2*,
        Comp>();
}
#endif // __cpp_lib_concepts

template <class Expected, class Type1, class Type2, class Pred>
void test_lex_compare_memcmp_classify_for_pred() {
    test_lex_compare_memcmp_classify_for_pred_helper<Expected, Type1, Type2, Pred>();
}

#ifdef __cpp_lib_concepts
template <class Expected, class Type1, class Type2, class Comp>
void test_lex_compare_three_way_memcmp_classify_for_comp() {
    test_lex_compare_three_way_memcmp_classify_for_comp_helper<Expected, Type1, Type2, Comp>();

    // No volatile
    test_lex_compare_three_way_memcmp_classify_for_comp_helper<void, volatile Type1, Type2, Comp>();
    test_lex_compare_three_way_memcmp_classify_for_comp_helper<void, Type1, volatile Type2, Comp>();
    test_lex_compare_three_way_memcmp_classify_for_comp_helper<void, volatile Type1, volatile Type2, Comp>();
}
#endif // __cpp_lib_concepts

template <bool Expected, class Type1, class Type2, class PredType>
void test_lex_compare_memcmp_classify_for_opaque_preds_helper() {
    using expected_less    = conditional_t<Expected, less<int>, void>;
    using expected_greater = conditional_t<Expected, greater<int>, void>;

    test_lex_compare_memcmp_classify_for_pred<expected_less, Type1, Type2, less<PredType>>();
    test_lex_compare_memcmp_classify_for_pred<expected_greater, Type1, Type2, greater<PredType>>();
}

template <bool Expected, class Type1, class Type2, class PredType>
void test_lex_compare_memcmp_classify_for_opaque_preds() {
    enum Enum1 : Type1 {};
    enum Enum2 : Type2 {};
    enum class EnumClass1 : Type1 {};
    enum class EnumClass2 : Type2 {};

    enum EnumPred : PredType {};
    enum class EnumClassPred : PredType {};

    test_lex_compare_memcmp_classify_for_opaque_preds_helper<Expected, Type1, Type2, PredType>();
    test_lex_compare_memcmp_classify_for_opaque_preds_helper<Expected, Enum1, Type2, PredType>();
    test_lex_compare_memcmp_classify_for_opaque_preds_helper<Expected, Type1, Enum2, PredType>();
    test_lex_compare_memcmp_classify_for_opaque_preds_helper<Expected, Enum1, Enum2, PredType>();

    // Enum classes are not convertible
    test_lex_compare_memcmp_classify_for_opaque_preds_helper<false, EnumClass1, Type2, PredType>();
    test_lex_compare_memcmp_classify_for_opaque_preds_helper<false, Type1, EnumClass2, PredType>();
    test_lex_compare_memcmp_classify_for_opaque_preds_helper<false, EnumClass1, Enum2, PredType>();
    test_lex_compare_memcmp_classify_for_opaque_preds_helper<false, Enum1, EnumClass2, PredType>();
    test_lex_compare_memcmp_classify_for_opaque_preds_helper<false, EnumClass1, EnumClass2, PredType>();

    // Don't allow converting to and between enums
    test_lex_compare_memcmp_classify_for_opaque_preds_helper<false, Type1, Type2, EnumPred>();
    test_lex_compare_memcmp_classify_for_opaque_preds_helper<false, Enum1, Type2, EnumPred>();
    test_lex_compare_memcmp_classify_for_opaque_preds_helper<false, Type1, Enum2, EnumPred>();
    test_lex_compare_memcmp_classify_for_opaque_preds_helper<false, Enum1, Enum2, EnumPred>();
    test_lex_compare_memcmp_classify_for_opaque_preds_helper<false, Type1, Type2, EnumClassPred>();
    test_lex_compare_memcmp_classify_for_opaque_preds_helper<false, Enum1, Type2, EnumClassPred>();
    test_lex_compare_memcmp_classify_for_opaque_preds_helper<false, Type1, Enum2, EnumClassPred>();
    test_lex_compare_memcmp_classify_for_opaque_preds_helper<false, Enum1, Enum2, EnumClassPred>();
}

template <bool Expected, class Type1, class Type2>
void test_lex_compare_memcmp_classify_for_types() {
    using expected_less    = conditional_t<Expected, less<int>, void>;
    using expected_greater = conditional_t<Expected, greater<int>, void>;

    test_lex_compare_memcmp_classify_for_pred<expected_less, Type1, Type2, less<>>();
    test_lex_compare_memcmp_classify_for_pred<expected_greater, Type1, Type2, greater<>>();

    test_lex_compare_memcmp_classify_for_pred<void, Type1, Type2, less<int>>();
    test_lex_compare_memcmp_classify_for_pred<void, Type1, Type2, greater<int>>();

    test_lex_compare_memcmp_classify_for_pred<void, Type1, Type2, less<volatile Type1>>();
    test_lex_compare_memcmp_classify_for_pred<void, Type1, Type2, greater<volatile Type1>>();
    test_lex_compare_memcmp_classify_for_pred<void, Type1, Type2, less<const volatile Type1>>();
    test_lex_compare_memcmp_classify_for_pred<void, Type1, Type2, greater<const volatile Type1>>();

    test_lex_compare_memcmp_classify_for_pred<void, Type1, Type2, equal_to<>>();
    test_lex_compare_memcmp_classify_for_pred<void, Type1, Type2, less_equal<>>();
    test_lex_compare_memcmp_classify_for_pred<void, Type1, Type2, greater_equal<>>();

    auto lambda = [](auto&&, auto&&) { return false; };
    test_lex_compare_memcmp_classify_for_pred<void, Type1, Type2, decltype(lambda)>();

#ifdef __cpp_lib_concepts
    test_lex_compare_memcmp_classify_for_pred<expected_less, Type1, Type2, ranges::less>();
    test_lex_compare_memcmp_classify_for_pred<expected_greater, Type1, Type2, ranges::greater>();

    test_lex_compare_memcmp_classify_for_pred<void, Type1, Type2, ranges::equal_to>();
    test_lex_compare_memcmp_classify_for_pred<void, Type1, Type2, ranges::less_equal>();
    test_lex_compare_memcmp_classify_for_pred<void, Type1, Type2, ranges::greater_equal>();

    // Test three way version
    using strong_order_t  = remove_const_t<decltype(strong_order)>;
    using weak_order_t    = remove_const_t<decltype(weak_order)>;
    using partial_order_t = remove_const_t<decltype(partial_order)>;
    using expected_compare_three_way =
        conditional_t<Expected && is_same_v<Type1, bool> == is_same_v<Type2, bool>, compare_three_way, void>;
    using expected_strong_order =
        conditional_t<Expected && is_same_v<decay_t<Type1>, decay_t<Type2>>, strong_order_t, void>;
    using expected_weak_order =
        conditional_t<Expected && is_same_v<decay_t<Type1>, decay_t<Type2>>, weak_order_t, void>;
    using expected_partial_order =
        conditional_t<Expected && is_same_v<decay_t<Type1>, decay_t<Type2>>, partial_order_t, void>;

    test_lex_compare_three_way_memcmp_classify_for_comp<expected_compare_three_way, Type1, Type2, compare_three_way>();
    test_lex_compare_three_way_memcmp_classify_for_comp<expected_strong_order, Type1, Type2, strong_order_t>();
    test_lex_compare_three_way_memcmp_classify_for_comp<expected_weak_order, Type1, Type2, weak_order_t>();
    test_lex_compare_three_way_memcmp_classify_for_comp<expected_partial_order, Type1, Type2, partial_order_t>();

    auto three_way_lambda = [](auto&&, auto&&) { return strong_ordering::equal; };
    test_lex_compare_memcmp_classify_for_pred<void, Type1, Type2, decltype(three_way_lambda)>();
#endif // __cpp_lib_concepts
}

template <bool Expected, class Type1, class Type2>
void test_lex_compare_memcmp_classify_for_1byte_integrals() {
    test_lex_compare_memcmp_classify_for_types<Expected, Type1, Type2>();

    test_lex_compare_memcmp_classify_for_opaque_preds<is_unsigned_v<char>, Type1, Type2, char>();
    test_lex_compare_memcmp_classify_for_opaque_preds<true, Type1, Type2, unsigned char>();
    test_lex_compare_memcmp_classify_for_opaque_preds<false, Type1, Type2, signed char>();
#ifdef __cpp_lib_char8_t
    test_lex_compare_memcmp_classify_for_opaque_preds<true, Type1, Type2, char8_t>();
#endif // __cpp_lib_char8_t
    test_lex_compare_memcmp_classify_for_opaque_preds<is_same_v<Type1, bool> && is_same_v<Type2, bool>, Type1, Type2,
        bool>();
}

template <bool Expected, class Container1, class Container2>
void test_lex_compare_memcmp_classify_for_containers() {
    using expected_less    = conditional_t<Expected, less<int>, void>;
    using expected_greater = conditional_t<Expected, greater<int>, void>;

    using It1      = typename Container1::iterator;
    using ConstIt1 = typename Container1::const_iterator;
    using It2      = typename Container2::iterator;
    using ConstIt2 = typename Container2::const_iterator;
    test_lex_compare_memcmp_classify_for_iterators<expected_less, It1, ConstIt1, It2, ConstIt2, less<>>();
    test_lex_compare_memcmp_classify_for_iterators<expected_greater, It1, ConstIt1, It2, ConstIt2, greater<>>();
}

enum char_enum : char {};
enum schar_enum : signed char {};
enum uchar_enum : unsigned char {};
#ifdef __cpp_lib_char8_t
enum char8_t_enum : char8_t {};
#endif // __cpp_lib_char8_t

enum class char_enum_class : char {};
enum class schar_enum_class : signed char {};
enum class uchar_enum_class : unsigned char {};
#ifdef __cpp_lib_char8_t
enum class char8_t_enum_class : char8_t {};
#endif // __cpp_lib_char8_t

struct user_struct {};

bool operator<(const user_struct&, const user_struct&) {
    return false;
}

void lex_compare_memcmp_classify_test_cases() {
    // Allow unsigned 1 byte integrals
    test_lex_compare_memcmp_classify_for_1byte_integrals<is_unsigned_v<char>, char, char>();
    test_lex_compare_memcmp_classify_for_1byte_integrals<is_unsigned_v<char>, unsigned char, char>();
    test_lex_compare_memcmp_classify_for_1byte_integrals<is_unsigned_v<char>, char, unsigned char>();
    test_lex_compare_memcmp_classify_for_1byte_integrals<true, unsigned char, unsigned char>();
    test_lex_compare_memcmp_classify_for_1byte_integrals<false, signed char, signed char>();
    test_lex_compare_memcmp_classify_for_1byte_integrals<false, char, signed char>();
    test_lex_compare_memcmp_classify_for_1byte_integrals<false, signed char, char>();
    test_lex_compare_memcmp_classify_for_1byte_integrals<false, unsigned char, signed char>();
    test_lex_compare_memcmp_classify_for_1byte_integrals<false, signed char, unsigned char>();
#ifdef __cpp_lib_char8_t
    test_lex_compare_memcmp_classify_for_1byte_integrals<true, char8_t, char8_t>();
    test_lex_compare_memcmp_classify_for_1byte_integrals<true, unsigned char, char8_t>();
    test_lex_compare_memcmp_classify_for_1byte_integrals<true, char8_t, unsigned char>();
    test_lex_compare_memcmp_classify_for_1byte_integrals<is_unsigned_v<char>, char8_t, char>();
    test_lex_compare_memcmp_classify_for_1byte_integrals<is_unsigned_v<char>, char, char8_t>();
    test_lex_compare_memcmp_classify_for_1byte_integrals<false, char8_t, signed char>();
    test_lex_compare_memcmp_classify_for_1byte_integrals<false, signed char, char8_t>();
#endif // __cpp_lib_char8_t

    // Test bool
    test_lex_compare_memcmp_classify_for_1byte_integrals<true, bool, bool>();
    test_lex_compare_memcmp_classify_for_1byte_integrals<true, unsigned char, bool>();
    test_lex_compare_memcmp_classify_for_1byte_integrals<true, bool, unsigned char>();
#ifdef __cpp_lib_char8_t
    test_lex_compare_memcmp_classify_for_1byte_integrals<true, bool, char8_t>();
    test_lex_compare_memcmp_classify_for_1byte_integrals<true, char8_t, bool>();
#endif // __cpp_lib_char8_t
    test_lex_compare_memcmp_classify_for_1byte_integrals<false, char, bool>();
    test_lex_compare_memcmp_classify_for_1byte_integrals<false, bool, char>();
    test_lex_compare_memcmp_classify_for_1byte_integrals<false, signed char, bool>();
    test_lex_compare_memcmp_classify_for_1byte_integrals<false, bool, signed char>();

    // Don't allow enums
    test_lex_compare_memcmp_classify_for_types<false, char_enum, char_enum>();
    test_lex_compare_memcmp_classify_for_types<false, schar_enum, schar_enum>();
    test_lex_compare_memcmp_classify_for_types<false, uchar_enum, uchar_enum>();
#ifdef __cpp_lib_char8_t
    test_lex_compare_memcmp_classify_for_types<false, char8_t_enum, char8_t_enum>();
#endif // __cpp_lib_char8_t

    // Don't allow enum classes
    test_lex_compare_memcmp_classify_for_types<false, char_enum_class, char_enum_class>();
    test_lex_compare_memcmp_classify_for_types<false, schar_enum_class, schar_enum_class>();
    test_lex_compare_memcmp_classify_for_types<false, uchar_enum_class, uchar_enum_class>();
#ifdef __cpp_lib_char8_t
    test_lex_compare_memcmp_classify_for_types<false, char8_t_enum_class, char8_t_enum_class>();
#endif // __cpp_lib_char8_t

#ifdef __cpp_lib_byte
    // Test std::byte
    test_lex_compare_memcmp_classify_for_types<true, byte, byte>();
#endif // __cpp_lib_byte

    // Don't allow bigger integrals
    test_lex_compare_memcmp_classify_for_types<false, unsigned char, int>();
    test_lex_compare_memcmp_classify_for_types<false, int, unsigned char>();
    test_lex_compare_memcmp_classify_for_types<false, int, int>();
    test_lex_compare_memcmp_classify_for_types<false, unsigned int, unsigned int>();
    test_lex_compare_memcmp_classify_for_types<false, short, short>();
    test_lex_compare_memcmp_classify_for_types<false, unsigned short, unsigned short>();

    // Don't allow pointers
    test_lex_compare_memcmp_classify_for_types<false, int*, int*>();

    // Don't allow user-defined types
    test_lex_compare_memcmp_classify_for_types<false, user_struct, user_struct>();

    // Test _Char_traits_lt
    test_lex_compare_memcmp_classify_for_pred<less<int>, char, char, _Char_traits_lt<char_traits<char>>>();
#ifdef __cpp_lib_char8_t
    test_lex_compare_memcmp_classify_for_pred<less<int>, char8_t, char8_t, _Char_traits_lt<char_traits<char8_t>>>();
#endif // __cpp_lib_char8_t

    test_lex_compare_memcmp_classify_for_pred<void, wchar_t, wchar_t, _Char_traits_lt<char_traits<wchar_t>>>();
    test_lex_compare_memcmp_classify_for_pred<void, char16_t, char16_t, _Char_traits_lt<char_traits<char16_t>>>();
    test_lex_compare_memcmp_classify_for_pred<void, char32_t, char32_t, _Char_traits_lt<char_traits<char32_t>>>();

    // Test different containers
#ifdef __cpp_lib_concepts
    test_lex_compare_memcmp_classify_for_containers<true, vector<unsigned char>, vector<unsigned char>>();
    test_lex_compare_memcmp_classify_for_containers<true, array<unsigned char, 8>, array<unsigned char, 8>>();
    test_lex_compare_memcmp_classify_for_containers<true, vector<unsigned char>, array<unsigned char, 8>>();
    test_lex_compare_memcmp_classify_for_containers<true, vector<unsigned char>, array<unsigned char, 8>>();
#endif // __cpp_lib_concepts

    test_lex_compare_memcmp_classify_for_containers<false, list<unsigned char>, list<unsigned char>>();
    test_lex_compare_memcmp_classify_for_containers<false, vector<unsigned char>, list<unsigned char>>();
    test_lex_compare_memcmp_classify_for_containers<false, list<unsigned char>, vector<unsigned char>>();

#ifdef __cpp_lib_concepts
    // Test counted_iterator
    assert_lex_compare_memcmp_classify<less<int>, counted_iterator<unsigned char*>, unsigned char*, less<>>();
    assert_lex_compare_memcmp_classify<less<int>, unsigned char*, counted_iterator<unsigned char*>, less<>>();
    assert_lex_compare_memcmp_classify<less<int>, counted_iterator<unsigned char*>, counted_iterator<unsigned char*>,
        less<>>();
#endif // __cpp_lib_concepts

    // No volatile
    test_lex_compare_memcmp_classify_for_pred_helper<void, volatile unsigned char, unsigned char, less<>>();
    test_lex_compare_memcmp_classify_for_pred_helper<void, unsigned char, volatile unsigned char, less<>>();
    test_lex_compare_memcmp_classify_for_pred_helper<void, volatile unsigned char, volatile unsigned char, less<>>();
}
