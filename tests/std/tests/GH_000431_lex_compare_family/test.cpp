// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifdef __clang__
#pragma clang diagnostic ignored "-Wsign-compare" // comparison of integers of different signs: 'X' and 'Y'
#endif // __clang__

#include <algorithm>
#include <cassert>
#include <functional>
#include <iterator>
#include <type_traits>

#if _HAS_CXX20
#include <compare>
#endif // _HAS_CXX20

#pragma warning(disable : 4018) // '<': signed/unsigned mismatch
#pragma warning(disable : 4365) // conversion from 'X' to 'Y', signed/unsigned mismatch
#pragma warning(disable : 4804) // '<': unsafe use of type 'bool' in operation

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

template <class Int1, class Int2, class PredType, class LexCompareFn>
void test_algorithms_for_integrals(LexCompareFn lex_compare_fn) {
    Int1 arr1[10] = {5, 7, 3, 4, 6, 4, 7, 1, 9, 5};
    Int2 arr2[10] = {5, 7, 3, 4, 6, 4, 7, 1, 9, 5};
    Int2 arr3[10] = {5, 7, 3, 4, 6, 4, 7, 1, 10, 5};
    Int2 arr4[10] = {5, 7, 2, 4, 6, 4, 7, 1, 9, 5};
    Int2 arr5[11] = {5, 7, 3, 4, 6, 4, 7, 1, 9, 5, 4};
    Int2 arr6[11] = {5, 7, 3, 4, 6, 4, 7, 1, 10, 5, 4};
    Int2 arr7[11] = {5, 7, 2, 4, 6, 4, 7, 1, 9, 5, 4};
    Int2 arr8[9]  = {5, 7, 3, 4, 6, 4, 7, 1, 9};
    Int2 arr9[9]  = {5, 7, 3, 4, 6, 4, 7, 1, 10};
    Int2 arr10[9] = {5, 7, 2, 4, 6, 4, 7, 1, 9};

    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), less<PredType>{}));
    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), greater<PredType>{}));

    assert(lex_compare_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), less<PredType>{}));
    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), greater<PredType>{}));

    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr4), end(arr4), less<PredType>{}));
    assert(lex_compare_fn(begin(arr1), end(arr1), begin(arr4), end(arr4), greater<PredType>{}));

    assert(lex_compare_fn(begin(arr1), end(arr1), begin(arr5), end(arr5), less<PredType>{}));
    assert(lex_compare_fn(begin(arr1), end(arr1), begin(arr5), end(arr5), greater<PredType>{}));

    assert(lex_compare_fn(begin(arr1), end(arr1), begin(arr6), end(arr6), less<PredType>{}));
    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr6), end(arr6), greater<PredType>{}));

    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr7), end(arr7), less<PredType>{}));
    assert(lex_compare_fn(begin(arr1), end(arr1), begin(arr7), end(arr7), greater<PredType>{}));

    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr8), end(arr8), less<PredType>{}));
    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr8), end(arr8), greater<PredType>{}));

    assert(lex_compare_fn(begin(arr1), end(arr1), begin(arr9), end(arr9), less<PredType>{}));
    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr9), end(arr9), greater<PredType>{}));

    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr10), end(arr10), less<PredType>{}));
    assert(lex_compare_fn(begin(arr1), end(arr1), begin(arr10), end(arr10), greater<PredType>{}));
}

template <class Int1, class Int2, class LexCompareFn>
void test_algorithms_for_1byte_integrals(LexCompareFn lex_compare_fn) {
    test_algorithms_for_integrals<Int1, Int2, void>(lex_compare_fn);
    test_algorithms_for_integrals<Int1, Int2, char>(lex_compare_fn);
    test_algorithms_for_integrals<Int1, Int2, signed char>(lex_compare_fn);
    test_algorithms_for_integrals<Int1, Int2, unsigned char>(lex_compare_fn);
#ifdef __cpp_lib_char8_t
    test_algorithms_for_integrals<Int1, Int2, char8_t>(lex_compare_fn);
#endif // __cpp_lib_char8_t
}

template <class Int, class LexCompareFn>
void test_algorithms_for_signed_integrals(LexCompareFn lex_compare_fn) {
    using UInt = make_unsigned_t<Int>;

    Int arr1[10]  = {5, 7, -3, 4, 6, 4, 7, 1, 9, 5};
    Int arr2[10]  = {5, 7, +3, 4, 6, 4, 7, 1, 9, 5};
    UInt arr3[10] = {5, 7, static_cast<UInt>(-3), 4, 6, 4, 7, 1, 9, 5};
    UInt arr4[10] = {5, 7, static_cast<UInt>(+3), 4, 6, 4, 7, 1, 9, 5};

    // Test signed
    assert(lex_compare_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), less<>{}));
    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), greater<>{}));
    assert(!lex_compare_fn(begin(arr2), end(arr2), begin(arr1), end(arr1), less<>{}));
    assert(lex_compare_fn(begin(arr2), end(arr2), begin(arr1), end(arr1), greater<>{}));

    assert(lex_compare_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), less<Int>{}));
    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), greater<Int>{}));
    assert(!lex_compare_fn(begin(arr2), end(arr2), begin(arr1), end(arr1), less<Int>{}));
    assert(lex_compare_fn(begin(arr2), end(arr2), begin(arr1), end(arr1), greater<Int>{}));

    assert(lex_compare_fn(begin(arr3), end(arr3), begin(arr4), end(arr4), less<Int>{}));
    assert(!lex_compare_fn(begin(arr3), end(arr3), begin(arr4), end(arr4), greater<Int>{}));
    assert(!lex_compare_fn(begin(arr4), end(arr4), begin(arr3), end(arr3), less<Int>{}));
    assert(lex_compare_fn(begin(arr4), end(arr4), begin(arr3), end(arr3), greater<Int>{}));

    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), less<Int>{}));
    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), greater<Int>{}));
    assert(!lex_compare_fn(begin(arr3), end(arr3), begin(arr1), end(arr1), less<Int>{}));
    assert(!lex_compare_fn(begin(arr3), end(arr3), begin(arr1), end(arr1), greater<Int>{}));

    assert(lex_compare_fn(begin(arr1), end(arr1), begin(arr4), end(arr4), less<Int>{}));
    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr4), end(arr4), greater<Int>{}));
    assert(!lex_compare_fn(begin(arr4), end(arr4), begin(arr1), end(arr1), less<Int>{}));
    assert(lex_compare_fn(begin(arr4), end(arr4), begin(arr1), end(arr1), greater<Int>{}));

    assert(!lex_compare_fn(begin(arr2), end(arr2), begin(arr3), end(arr3), less<Int>{}));
    assert(lex_compare_fn(begin(arr2), end(arr2), begin(arr3), end(arr3), greater<Int>{}));
    assert(lex_compare_fn(begin(arr3), end(arr3), begin(arr2), end(arr2), less<Int>{}));
    assert(!lex_compare_fn(begin(arr3), end(arr3), begin(arr2), end(arr2), greater<Int>{}));

    assert(!lex_compare_fn(begin(arr2), end(arr2), begin(arr4), end(arr4), less<Int>{}));
    assert(!lex_compare_fn(begin(arr2), end(arr2), begin(arr4), end(arr4), greater<Int>{}));
    assert(!lex_compare_fn(begin(arr4), end(arr4), begin(arr2), end(arr2), less<Int>{}));
    assert(!lex_compare_fn(begin(arr4), end(arr4), begin(arr2), end(arr2), greater<Int>{}));

    // Test unsigned
    assert(!lex_compare_fn(begin(arr3), end(arr3), begin(arr4), end(arr4), less<>{}));
    assert(lex_compare_fn(begin(arr3), end(arr3), begin(arr4), end(arr4), greater<>{}));
    assert(lex_compare_fn(begin(arr4), end(arr4), begin(arr3), end(arr3), less<>{}));
    assert(!lex_compare_fn(begin(arr4), end(arr4), begin(arr3), end(arr3), greater<>{}));

    assert(!lex_compare_fn(begin(arr3), end(arr3), begin(arr4), end(arr4), less<UInt>{}));
    assert(lex_compare_fn(begin(arr3), end(arr3), begin(arr4), end(arr4), greater<UInt>{}));
    assert(lex_compare_fn(begin(arr4), end(arr4), begin(arr3), end(arr3), less<UInt>{}));
    assert(!lex_compare_fn(begin(arr4), end(arr4), begin(arr3), end(arr3), greater<UInt>{}));

    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), less<UInt>{}));
    assert(lex_compare_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), greater<UInt>{}));
    assert(lex_compare_fn(begin(arr2), end(arr2), begin(arr1), end(arr1), less<UInt>{}));
    assert(!lex_compare_fn(begin(arr2), end(arr2), begin(arr1), end(arr1), greater<UInt>{}));

    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), less<UInt>{}));
    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), greater<UInt>{}));
    assert(!lex_compare_fn(begin(arr3), end(arr3), begin(arr1), end(arr1), less<UInt>{}));
    assert(!lex_compare_fn(begin(arr3), end(arr3), begin(arr1), end(arr1), greater<UInt>{}));

    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr4), end(arr4), less<UInt>{}));
    assert(lex_compare_fn(begin(arr1), end(arr1), begin(arr4), end(arr4), greater<UInt>{}));
    assert(lex_compare_fn(begin(arr4), end(arr4), begin(arr1), end(arr1), less<UInt>{}));
    assert(!lex_compare_fn(begin(arr4), end(arr4), begin(arr1), end(arr1), greater<UInt>{}));

    assert(lex_compare_fn(begin(arr2), end(arr2), begin(arr3), end(arr3), less<UInt>{}));
    assert(!lex_compare_fn(begin(arr2), end(arr2), begin(arr3), end(arr3), greater<UInt>{}));
    assert(!lex_compare_fn(begin(arr3), end(arr3), begin(arr2), end(arr2), less<UInt>{}));
    assert(lex_compare_fn(begin(arr3), end(arr3), begin(arr2), end(arr2), greater<UInt>{}));

    assert(!lex_compare_fn(begin(arr2), end(arr2), begin(arr4), end(arr4), less<UInt>{}));
    assert(!lex_compare_fn(begin(arr2), end(arr2), begin(arr4), end(arr4), greater<UInt>{}));
    assert(!lex_compare_fn(begin(arr4), end(arr4), begin(arr2), end(arr2), less<UInt>{}));
    assert(!lex_compare_fn(begin(arr4), end(arr4), begin(arr2), end(arr2), greater<UInt>{}));

    // Test comparing signed and unsigned
    constexpr bool promoted = sizeof(Int) < sizeof(int);

    assert(promoted == lex_compare_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), less<>{}));
    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), greater<>{}));
    assert(!lex_compare_fn(begin(arr3), end(arr3), begin(arr1), end(arr1), less<>{}));
    assert(promoted == lex_compare_fn(begin(arr3), end(arr3), begin(arr1), end(arr1), greater<>{}));

    assert(promoted == lex_compare_fn(begin(arr1), end(arr1), begin(arr4), end(arr4), less<>{}));
    assert(promoted == !lex_compare_fn(begin(arr1), end(arr1), begin(arr4), end(arr4), greater<>{}));
    assert(promoted == !lex_compare_fn(begin(arr4), end(arr4), begin(arr1), end(arr1), less<>{}));
    assert(promoted == lex_compare_fn(begin(arr4), end(arr4), begin(arr1), end(arr1), greater<>{}));

    assert(lex_compare_fn(begin(arr2), end(arr2), begin(arr3), end(arr3), less<>{}));
    assert(!lex_compare_fn(begin(arr2), end(arr2), begin(arr3), end(arr3), greater<>{}));
    assert(!lex_compare_fn(begin(arr3), end(arr3), begin(arr2), end(arr2), less<>{}));
    assert(lex_compare_fn(begin(arr3), end(arr3), begin(arr2), end(arr2), greater<>{}));

    assert(!lex_compare_fn(begin(arr2), end(arr2), begin(arr4), end(arr4), less<>{}));
    assert(!lex_compare_fn(begin(arr2), end(arr2), begin(arr4), end(arr4), greater<>{}));
    assert(!lex_compare_fn(begin(arr4), end(arr4), begin(arr2), end(arr2), less<>{}));
    assert(!lex_compare_fn(begin(arr4), end(arr4), begin(arr2), end(arr2), greater<>{}));
}

template <class Bool1, class Bool2, class PredType, class LexCompareFn>
void test_algorithms_for_bools_helper(LexCompareFn lex_compare_fn) {
    Bool1 arr1[5]  = {true, true, false, false, true};
    Bool2 arr2[5]  = {true, true, false, false, true};
    Bool2 arr3[5]  = {true, true, true, false, true};
    Bool2 arr4[5]  = {true, false, false, false, true};
    Bool2 arr5[6]  = {true, true, false, false, true, false};
    Bool2 arr6[6]  = {true, true, true, false, true, false};
    Bool2 arr7[6]  = {true, false, false, false, true, false};
    Bool2 arr8[4]  = {true, true, false, false};
    Bool2 arr9[4]  = {true, true, true, false};
    Bool2 arr10[4] = {true, false, false, false};

    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), less<PredType>{}));
    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), greater<PredType>{}));

    assert(lex_compare_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), less<PredType>{}));
    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), greater<PredType>{}));

    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr4), end(arr4), less<PredType>{}));
    assert(lex_compare_fn(begin(arr1), end(arr1), begin(arr4), end(arr4), greater<PredType>{}));

    assert(lex_compare_fn(begin(arr1), end(arr1), begin(arr5), end(arr5), less<PredType>{}));
    assert(lex_compare_fn(begin(arr1), end(arr1), begin(arr5), end(arr5), greater<PredType>{}));

    assert(lex_compare_fn(begin(arr1), end(arr1), begin(arr6), end(arr6), less<PredType>{}));
    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr6), end(arr6), greater<PredType>{}));

    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr7), end(arr7), less<PredType>{}));
    assert(lex_compare_fn(begin(arr1), end(arr1), begin(arr7), end(arr7), greater<PredType>{}));

    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr8), end(arr8), less<PredType>{}));
    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr8), end(arr8), greater<PredType>{}));

    assert(lex_compare_fn(begin(arr1), end(arr1), begin(arr9), end(arr9), less<PredType>{}));
    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr9), end(arr9), greater<PredType>{}));

    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr10), end(arr10), less<PredType>{}));
    assert(lex_compare_fn(begin(arr1), end(arr1), begin(arr10), end(arr10), greater<PredType>{}));
}

template <class Bool1, class Bool2, class LexCompareFn>
void test_algorithms_for_bools(LexCompareFn lex_compare_fn) {
    test_algorithms_for_bools_helper<Bool1, Bool2, void>(lex_compare_fn);
    test_algorithms_for_bools_helper<Bool1, Bool2, bool>(lex_compare_fn);
    test_algorithms_for_bools_helper<Bool1, Bool2, char>(lex_compare_fn);
    test_algorithms_for_bools_helper<Bool1, Bool2, unsigned char>(lex_compare_fn);
    test_algorithms_for_bools_helper<Bool1, Bool2, signed char>(lex_compare_fn);
    test_algorithms_for_bools_helper<Bool1, Bool2, int>(lex_compare_fn);
}

template <class Char, class LexCompareFn>
void test_algorithms_for_bool_and_chars(LexCompareFn lex_compare_fn) {
    bool arr1[5]  = {true, true, false, false, true};
    Char arr2[5]  = {2, 1, 0, 0, 1};
    Char arr3[5]  = {2, 1, 1, 0, 1};
    Char arr4[5]  = {2, 0, 0, 0, 1};
    Char arr5[6]  = {2, 1, 0, 0, 1, 0};
    Char arr6[6]  = {2, 1, 1, 0, 1, 0};
    Char arr7[6]  = {2, 0, 0, 0, 1, 0};
    Char arr8[4]  = {2, 1, 0, 0};
    Char arr9[4]  = {2, 1, 1, 0};
    Char arr10[4] = {2, 0, 0, 0};

    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), less<bool>{}));
    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), greater<bool>{}));

    assert(lex_compare_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), less<bool>{}));
    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), greater<bool>{}));

    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr4), end(arr4), less<bool>{}));
    assert(lex_compare_fn(begin(arr1), end(arr1), begin(arr4), end(arr4), greater<bool>{}));

    assert(lex_compare_fn(begin(arr1), end(arr1), begin(arr5), end(arr5), less<bool>{}));
    assert(lex_compare_fn(begin(arr1), end(arr1), begin(arr5), end(arr5), greater<bool>{}));

    assert(lex_compare_fn(begin(arr1), end(arr1), begin(arr6), end(arr6), less<bool>{}));
    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr6), end(arr6), greater<bool>{}));

    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr7), end(arr7), less<bool>{}));
    assert(lex_compare_fn(begin(arr1), end(arr1), begin(arr7), end(arr7), greater<bool>{}));

    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr8), end(arr8), less<bool>{}));
    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr8), end(arr8), greater<bool>{}));

    assert(lex_compare_fn(begin(arr1), end(arr1), begin(arr9), end(arr9), less<bool>{}));
    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr9), end(arr9), greater<bool>{}));

    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr10), end(arr10), less<bool>{}));
    assert(lex_compare_fn(begin(arr1), end(arr1), begin(arr10), end(arr10), greater<bool>{}));

    assert(lex_compare_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), less<Char>{}));
    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), greater<Char>{}));
    assert(lex_compare_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), less<Char>{}));
    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), greater<Char>{}));
    assert(lex_compare_fn(begin(arr1), end(arr1), begin(arr4), end(arr4), less<Char>{}));
    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr4), end(arr4), greater<Char>{}));
    assert(lex_compare_fn(begin(arr1), end(arr1), begin(arr5), end(arr5), less<Char>{}));
    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr5), end(arr5), greater<Char>{}));
    assert(lex_compare_fn(begin(arr1), end(arr1), begin(arr6), end(arr6), less<Char>{}));
    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr6), end(arr6), greater<Char>{}));
    assert(lex_compare_fn(begin(arr1), end(arr1), begin(arr7), end(arr7), less<Char>{}));
    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr7), end(arr7), greater<Char>{}));
    assert(lex_compare_fn(begin(arr1), end(arr1), begin(arr8), end(arr8), less<Char>{}));
    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr8), end(arr8), greater<Char>{}));
    assert(lex_compare_fn(begin(arr1), end(arr1), begin(arr9), end(arr9), less<Char>{}));
    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr9), end(arr9), greater<Char>{}));
    assert(lex_compare_fn(begin(arr1), end(arr1), begin(arr10), end(arr10), less<Char>{}));
    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr10), end(arr10), greater<Char>{}));

    assert(lex_compare_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), less<>{}));
    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), greater<>{}));
    assert(lex_compare_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), less<>{}));
    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), greater<>{}));
    assert(lex_compare_fn(begin(arr1), end(arr1), begin(arr4), end(arr4), less<>{}));
    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr4), end(arr4), greater<>{}));
    assert(lex_compare_fn(begin(arr1), end(arr1), begin(arr5), end(arr5), less<>{}));
    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr5), end(arr5), greater<>{}));
    assert(lex_compare_fn(begin(arr1), end(arr1), begin(arr6), end(arr6), less<>{}));
    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr6), end(arr6), greater<>{}));
    assert(lex_compare_fn(begin(arr1), end(arr1), begin(arr7), end(arr7), less<>{}));
    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr7), end(arr7), greater<>{}));
    assert(lex_compare_fn(begin(arr1), end(arr1), begin(arr8), end(arr8), less<>{}));
    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr8), end(arr8), greater<>{}));
    assert(lex_compare_fn(begin(arr1), end(arr1), begin(arr9), end(arr9), less<>{}));
    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr9), end(arr9), greater<>{}));
    assert(lex_compare_fn(begin(arr1), end(arr1), begin(arr10), end(arr10), less<>{}));
    assert(!lex_compare_fn(begin(arr1), end(arr1), begin(arr10), end(arr10), greater<>{}));
}

template <class LexCompareFn>
void test_algorithms(LexCompareFn lex_compare_fn) {
    { // Test ints
        test_algorithms_for_integrals<int, int, void>(lex_compare_fn);
        test_algorithms_for_integrals<unsigned int, unsigned int, void>(lex_compare_fn);
        test_algorithms_for_integrals<int, unsigned int, void>(lex_compare_fn);
        test_algorithms_for_integrals<unsigned int, int, void>(lex_compare_fn);

        test_algorithms_for_integrals<int, int, int>(lex_compare_fn);
        test_algorithms_for_integrals<unsigned int, unsigned int, int>(lex_compare_fn);
        test_algorithms_for_integrals<int, unsigned int, int>(lex_compare_fn);
        test_algorithms_for_integrals<unsigned int, int, int>(lex_compare_fn);

        test_algorithms_for_integrals<int, int, unsigned int>(lex_compare_fn);
        test_algorithms_for_integrals<unsigned int, unsigned int, unsigned int>(lex_compare_fn);
        test_algorithms_for_integrals<int, unsigned int, unsigned int>(lex_compare_fn);
        test_algorithms_for_integrals<unsigned int, int, unsigned int>(lex_compare_fn);
    }

    { // Test chars
        test_algorithms_for_1byte_integrals<char, char>(lex_compare_fn);
        test_algorithms_for_1byte_integrals<signed char, signed char>(lex_compare_fn);
        test_algorithms_for_1byte_integrals<char, signed char>(lex_compare_fn);
        test_algorithms_for_1byte_integrals<signed char, char>(lex_compare_fn);
        test_algorithms_for_1byte_integrals<unsigned char, unsigned char>(lex_compare_fn);
        test_algorithms_for_1byte_integrals<unsigned char, char>(lex_compare_fn);
        test_algorithms_for_1byte_integrals<char, unsigned char>(lex_compare_fn);
        test_algorithms_for_1byte_integrals<unsigned char, signed char>(lex_compare_fn);
        test_algorithms_for_1byte_integrals<signed char, unsigned char>(lex_compare_fn);
#ifdef __cpp_lib_char8_t
        test_algorithms_for_1byte_integrals<char8_t, char8_t>(lex_compare_fn);
        test_algorithms_for_1byte_integrals<char, char8_t>(lex_compare_fn);
        test_algorithms_for_1byte_integrals<char8_t, char>(lex_compare_fn);
        test_algorithms_for_1byte_integrals<signed char, char8_t>(lex_compare_fn);
        test_algorithms_for_1byte_integrals<char8_t, signed char>(lex_compare_fn);
        test_algorithms_for_1byte_integrals<unsigned char, char8_t>(lex_compare_fn);
        test_algorithms_for_1byte_integrals<char8_t, unsigned char>(lex_compare_fn);
#endif // __cpp_lib_char8_t
    }

    { // Test signedness
        test_algorithms_for_signed_integrals<int>(lex_compare_fn);
        test_algorithms_for_signed_integrals<signed char>(lex_compare_fn);
    }

    { // Test bool
        test_algorithms_for_bools<bool, bool>(lex_compare_fn);
        test_algorithms_for_bools<char, char>(lex_compare_fn);
        test_algorithms_for_bools<unsigned char, unsigned char>(lex_compare_fn);
        test_algorithms_for_bools<signed char, signed char>(lex_compare_fn);
        test_algorithms_for_bools<char, bool>(lex_compare_fn);
        test_algorithms_for_bools<bool, char>(lex_compare_fn);
        test_algorithms_for_bools<unsigned char, bool>(lex_compare_fn);
        test_algorithms_for_bools<bool, unsigned char>(lex_compare_fn);
        test_algorithms_for_bools<signed char, bool>(lex_compare_fn);
        test_algorithms_for_bools<bool, signed char>(lex_compare_fn);
    }

    { // Test comparing bool and char
        test_algorithms_for_bool_and_chars<char>(lex_compare_fn);
        test_algorithms_for_bool_and_chars<unsigned char>(lex_compare_fn);
        test_algorithms_for_bool_and_chars<signed char>(lex_compare_fn);
    }
}

#if _HAS_CXX20
template <class Int1, class Int2, class LexCompareThreeWayFn, class Comp = compare_three_way>
void test_three_way_algorithms_for_integrals(
    LexCompareThreeWayFn lex_compare_three_way_fn, Comp comp = compare_three_way{}) {
    Int1 arr1[10] = {5, 7, 3, 4, 6, 4, 7, 1, 9, 5};
    Int2 arr2[10] = {5, 7, 3, 4, 6, 4, 7, 1, 9, 5};
    Int2 arr3[10] = {5, 7, 3, 4, 6, 4, 7, 1, 10, 5};
    Int2 arr4[10] = {5, 7, 2, 4, 6, 4, 7, 1, 9, 5};
    Int2 arr5[11] = {5, 7, 3, 4, 6, 4, 7, 1, 9, 5, 4};
    Int2 arr6[11] = {5, 7, 3, 4, 6, 4, 7, 1, 10, 5, 4};
    Int2 arr7[11] = {5, 7, 2, 4, 6, 4, 7, 1, 9, 5, 4};
    Int2 arr8[9]  = {5, 7, 3, 4, 6, 4, 7, 1, 9};
    Int2 arr9[9]  = {5, 7, 3, 4, 6, 4, 7, 1, 10};
    Int2 arr10[9] = {5, 7, 2, 4, 6, 4, 7, 1, 9};

    assert(lex_compare_three_way_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), comp) == 0);
    assert(lex_compare_three_way_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), comp) < 0);
    assert(lex_compare_three_way_fn(begin(arr1), end(arr1), begin(arr4), end(arr4), comp) > 0);

    assert(lex_compare_three_way_fn(begin(arr1), end(arr1), begin(arr5), end(arr5), comp) < 0);
    assert(lex_compare_three_way_fn(begin(arr1), end(arr1), begin(arr6), end(arr6), comp) < 0);
    assert(lex_compare_three_way_fn(begin(arr1), end(arr1), begin(arr7), end(arr7), comp) > 0);

    assert(lex_compare_three_way_fn(begin(arr1), end(arr1), begin(arr8), end(arr8), comp) > 0);
    assert(lex_compare_three_way_fn(begin(arr1), end(arr1), begin(arr9), end(arr9), comp) < 0);
    assert(lex_compare_three_way_fn(begin(arr1), end(arr1), begin(arr10), end(arr10), comp) > 0);
}

template <class Int, class LexCompareThreeWayFn>
void test_three_way_algorithms_for_signed_integrals(LexCompareThreeWayFn lex_compare_three_way_fn) {
    using UInt = make_unsigned_t<Int>;

    Int arr1[10]  = {5, 7, -3, 4, 6, 4, 7, 1, 9, 5};
    Int arr2[10]  = {5, 7, +3, 4, 6, 4, 7, 1, 9, 5};
    UInt arr3[10] = {5, 7, static_cast<UInt>(-3), 4, 6, 4, 7, 1, 9, 5};
    UInt arr4[10] = {5, 7, static_cast<UInt>(+3), 4, 6, 4, 7, 1, 9, 5};

    // Test signed
    assert(lex_compare_three_way_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), compare_three_way{}) < 0);
    assert(lex_compare_three_way_fn(begin(arr2), end(arr2), begin(arr1), end(arr1), compare_three_way{}) > 0);

    // Test unsigned
    assert(lex_compare_three_way_fn(begin(arr3), end(arr3), begin(arr4), end(arr4), compare_three_way{}) > 0);
    assert(lex_compare_three_way_fn(begin(arr4), end(arr4), begin(arr3), end(arr3), compare_three_way{}) < 0);

    // Test comparing signed and unsigned

    // N4892 [expr.spaceship]/4, /4.1:
    // "If both operands have arithmetic types, or one operand has integral type and the other operand has unscoped
    // enumeration type, the usual arithmetic conversions (7.4) are applied to the operands. Then:
    // - If a narrowing conversion (9.4.5) is required, other than from an integral type to a floating-point type,
    // the program is ill-formed."

    constexpr bool promoted = sizeof(Int) < sizeof(int);

    if constexpr (promoted) {
        assert(lex_compare_three_way_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), compare_three_way{})
               == (promoted ? strong_ordering::less : strong_ordering::equal));
        assert(lex_compare_three_way_fn(begin(arr3), end(arr3), begin(arr1), end(arr1), compare_three_way{})
               == (promoted ? strong_ordering::greater : strong_ordering::equal));

        assert(lex_compare_three_way_fn(begin(arr1), end(arr1), begin(arr4), end(arr4), compare_three_way{})
               == (promoted ? strong_ordering::less : strong_ordering::greater));
        assert(lex_compare_three_way_fn(begin(arr4), end(arr4), begin(arr1), end(arr1), compare_three_way{})
               == (promoted ? strong_ordering::greater : strong_ordering::less));

        assert(lex_compare_three_way_fn(begin(arr2), end(arr2), begin(arr3), end(arr3), compare_three_way{}) < 0);
        assert(lex_compare_three_way_fn(begin(arr3), end(arr3), begin(arr2), end(arr2), compare_three_way{}) > 0);

        assert(lex_compare_three_way_fn(begin(arr2), end(arr2), begin(arr4), end(arr4), compare_three_way{}) == 0);
        assert(lex_compare_three_way_fn(begin(arr4), end(arr4), begin(arr2), end(arr2), compare_three_way{}) == 0);
    }
}

template <class LexCompareThreeWayFn>
void test_three_way_algorithms_for_bools(LexCompareThreeWayFn lex_compare_three_way_fn) {
    bool arr1[5]  = {true, true, false, false, true};
    bool arr2[5]  = {true, true, false, false, true};
    bool arr3[5]  = {true, true, true, false, true};
    bool arr4[5]  = {true, false, false, false, true};
    bool arr5[6]  = {true, true, false, false, true, false};
    bool arr6[6]  = {true, true, true, false, true, false};
    bool arr7[6]  = {true, false, false, false, true, false};
    bool arr8[4]  = {true, true, false, false};
    bool arr9[4]  = {true, true, true, false};
    bool arr10[4] = {true, false, false, false};

    assert(lex_compare_three_way_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), compare_three_way{}) == 0);
    assert(lex_compare_three_way_fn(begin(arr2), end(arr2), begin(arr1), end(arr1), compare_three_way{}) == 0);

    assert(lex_compare_three_way_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), compare_three_way{}) < 0);
    assert(lex_compare_three_way_fn(begin(arr3), end(arr3), begin(arr1), end(arr1), compare_three_way{}) > 0);

    assert(lex_compare_three_way_fn(begin(arr1), end(arr1), begin(arr4), end(arr4), compare_three_way{}) > 0);
    assert(lex_compare_three_way_fn(begin(arr4), end(arr4), begin(arr1), end(arr1), compare_three_way{}) < 0);

    assert(lex_compare_three_way_fn(begin(arr1), end(arr1), begin(arr5), end(arr5), compare_three_way{}) < 0);
    assert(lex_compare_three_way_fn(begin(arr5), end(arr5), begin(arr1), end(arr1), compare_three_way{}) > 0);

    assert(lex_compare_three_way_fn(begin(arr1), end(arr1), begin(arr6), end(arr6), compare_three_way{}) < 0);
    assert(lex_compare_three_way_fn(begin(arr6), end(arr6), begin(arr1), end(arr1), compare_three_way{}) > 0);

    assert(lex_compare_three_way_fn(begin(arr1), end(arr1), begin(arr7), end(arr7), compare_three_way{}) > 0);
    assert(lex_compare_three_way_fn(begin(arr7), end(arr7), begin(arr1), end(arr1), compare_three_way{}) < 0);

    assert(lex_compare_three_way_fn(begin(arr1), end(arr1), begin(arr8), end(arr8), compare_three_way{}) > 0);
    assert(lex_compare_three_way_fn(begin(arr8), end(arr8), begin(arr1), end(arr1), compare_three_way{}) < 0);

    assert(lex_compare_three_way_fn(begin(arr1), end(arr1), begin(arr9), end(arr9), compare_three_way{}) < 0);
    assert(lex_compare_three_way_fn(begin(arr9), end(arr9), begin(arr1), end(arr1), compare_three_way{}) > 0);

    assert(lex_compare_three_way_fn(begin(arr1), end(arr1), begin(arr10), end(arr10), compare_three_way{}) > 0);
    assert(lex_compare_three_way_fn(begin(arr10), end(arr10), begin(arr1), end(arr1), compare_three_way{}) < 0);
}

template <class LexCompareThreeWayFn>
void test_three_way_algorithms(LexCompareThreeWayFn lex_compare_three_way_fn) {
    { // Test ints
        test_three_way_algorithms_for_integrals<int, int>(lex_compare_three_way_fn, compare_three_way{});
        test_three_way_algorithms_for_integrals<int, int>(lex_compare_three_way_fn, strong_order);
        test_three_way_algorithms_for_integrals<int, int>(lex_compare_three_way_fn, weak_order);
        test_three_way_algorithms_for_integrals<int, int>(lex_compare_three_way_fn, partial_order);
        test_three_way_algorithms_for_integrals<int, int>(lex_compare_three_way_fn, compare_strong_order_fallback);
        test_three_way_algorithms_for_integrals<int, int>(lex_compare_three_way_fn, compare_weak_order_fallback);
        test_three_way_algorithms_for_integrals<int, int>(lex_compare_three_way_fn, compare_partial_order_fallback);
    }

    { // Test chars
        test_three_way_algorithms_for_integrals<char, char>(lex_compare_three_way_fn);
        test_three_way_algorithms_for_integrals<signed char, signed char>(lex_compare_three_way_fn);
        test_three_way_algorithms_for_integrals<char, signed char>(lex_compare_three_way_fn);
        test_three_way_algorithms_for_integrals<signed char, char>(lex_compare_three_way_fn);
        test_three_way_algorithms_for_integrals<unsigned char, unsigned char>(lex_compare_three_way_fn);
        test_three_way_algorithms_for_integrals<unsigned char, char>(lex_compare_three_way_fn);
        test_three_way_algorithms_for_integrals<char, unsigned char>(lex_compare_three_way_fn);
        test_three_way_algorithms_for_integrals<unsigned char, signed char>(lex_compare_three_way_fn);
        test_three_way_algorithms_for_integrals<signed char, unsigned char>(lex_compare_three_way_fn);
#ifdef __cpp_lib_char8_t
        test_three_way_algorithms_for_integrals<char8_t, char8_t>(lex_compare_three_way_fn);
        test_three_way_algorithms_for_integrals<char, char8_t>(lex_compare_three_way_fn);
        test_three_way_algorithms_for_integrals<char8_t, char>(lex_compare_three_way_fn);
        test_three_way_algorithms_for_integrals<signed char, char8_t>(lex_compare_three_way_fn);
        test_three_way_algorithms_for_integrals<char8_t, signed char>(lex_compare_three_way_fn);
        test_three_way_algorithms_for_integrals<unsigned char, char8_t>(lex_compare_three_way_fn);
        test_three_way_algorithms_for_integrals<char8_t, unsigned char>(lex_compare_three_way_fn);
#endif // __cpp_lib_char8_t
    }

    { // Test signedness
        test_three_way_algorithms_for_signed_integrals<int>(lex_compare_three_way_fn);
        test_three_way_algorithms_for_signed_integrals<signed char>(lex_compare_three_way_fn);
    }

    { // Test bool
        test_three_way_algorithms_for_bools(lex_compare_three_way_fn);
    }
}
#endif // _HAS_CXX20

int main() {
    test_algorithms([](auto begin1, auto end1, auto begin2, auto end2, auto pred) {
        return lexicographical_compare(begin1, end1, begin2, end2, pred);
    });

#if _HAS_CXX20
    test_algorithms([](auto begin1, auto end1, auto begin2, auto end2, auto pred) {
        return ranges::lexicographical_compare(begin1, end1, begin2, end2, pred);
    });

    test_algorithms([](auto begin1, auto end1, auto begin2, auto end2, auto pred) {
        return lexicographical_compare_three_way(begin1, end1, begin2, end2, [&](const auto& left, const auto& right) {
            return pred(left, right) ? strong_ordering::less
                 : pred(right, left) ? strong_ordering::greater
                                     : strong_ordering::equal;
        }) < 0;
    });

    test_three_way_algorithms([](auto begin1, auto end1, auto begin2, auto end2, auto comp) {
        return lexicographical_compare_three_way(begin1, end1, begin2, end2, comp);
    });
#endif // _HAS_CXX20
}
