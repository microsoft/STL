// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifdef __clang__
#pragma clang diagnostic ignored "-Wsign-compare" // comparison of integers of different signs: 'X' and 'Y'
#pragma clang diagnostic ignored "-Wenum-compare" // comparison of different enumeration types ('X' and 'Y')
#pragma clang diagnostic ignored \
    "-Wdeprecated-enum-compare" // comparison of different enumeration types ('X' and 'Y') is deprecated
#pragma clang diagnostic ignored "-Wc++17-extensions" // constexpr if is a C++17 extension
#endif // __clang__

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <functional>
#include <iterator>
#include <list>
#include <type_traits>
#include <vector>

#pragma warning(disable : 4242) // conversion from 'X' to 'Y', possible loss of data
#pragma warning(disable : 4244) // conversion from 'X' to 'Y', possible loss of data (Yes, duplicated message.)
#pragma warning(disable : 4365) // conversion from 'X' to 'Y', signed/unsigned mismatch
#pragma warning(disable : 4389) // '==': signed/unsigned mismatch
#pragma warning(disable : 4805) // '==': unsafe mix of type 'X' and type 'Y' in operation
#pragma warning(disable : 4984) // 'if constexpr' is a C++17 language extension
#pragma warning(disable : 5054) // operator '==': deprecated between enumerations of different types

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

enum int_enum : int {};
enum short_enum : short {};
enum char_enum : char {};
enum bool_enum : bool { false_e = false, true_e = true };

constexpr int_enum operator""_e(unsigned long long value) noexcept {
    return static_cast<int_enum>(value);
}

constexpr short_enum operator""_se(unsigned long long value) noexcept {
    return static_cast<short_enum>(value);
}

constexpr char_enum operator""_ce(unsigned long long value) noexcept {
    return static_cast<char_enum>(value);
}

enum bad_int_enum : int {};
enum bad_short_enum : short {};
enum bad_char_enum : char {};
enum bad_bool_enum : bool { false_bad = false, true_bad = true };

bool operator==(bad_int_enum, bad_int_enum) {
    return false;
}
bool operator==(int, bad_int_enum) {
    return false;
}
bool operator==(bad_int_enum, int) {
    return false;
}

bool operator==(bad_short_enum, bad_short_enum) {
    return false;
}
bool operator==(short, bad_short_enum) {
    return false;
}
bool operator==(bad_short_enum, short) {
    return false;
}

bool operator==(bad_char_enum, bad_char_enum) {
    return false;
}
bool operator==(char, bad_char_enum) {
    return false;
}
bool operator==(bad_char_enum, char) {
    return false;
}

bool operator==(bad_bool_enum, bad_bool_enum) {
    return false;
}
bool operator==(bool, bad_bool_enum) {
    return false;
}
bool operator==(bad_bool_enum, bool) {
    return false;
}

constexpr bad_int_enum operator""_be(unsigned long long value) noexcept {
    return static_cast<bad_int_enum>(value);
}

constexpr bad_short_enum operator""_bse(unsigned long long value) noexcept {
    return static_cast<bad_short_enum>(value);
}

constexpr bad_char_enum operator""_bce(unsigned long long value) noexcept {
    return static_cast<bad_char_enum>(value);
}

struct EmptyBase {};

struct EmptyDerived : EmptyBase {};

struct StatefulBase {
    int i;

    int get_i() {
        return i;
    }

    int get_i2() {
        return i * 2;
    }
};

struct StatefulDerived : StatefulBase, EmptyBase {
    int get_i3() {
        return i * 3;
    }
};

struct StatefulDerived2 : EmptyBase, StatefulBase {};

#ifdef __cpp_lib_is_pointer_interconvertible
STATIC_ASSERT(is_pointer_interconvertible_base_of_v<EmptyBase, EmptyDerived>);
STATIC_ASSERT(is_pointer_interconvertible_base_of_v<StatefulBase, StatefulDerived>);
STATIC_ASSERT(!is_pointer_interconvertible_base_of_v<EmptyBase, StatefulDerived>);
STATIC_ASSERT(is_pointer_interconvertible_base_of_v<StatefulBase, StatefulDerived2>);
STATIC_ASSERT(is_pointer_interconvertible_base_of_v<EmptyBase, StatefulDerived2>);
#endif // __cpp_lib_is_pointer_interconvertible

template <class EqualFn>
void test_algorithms(EqualFn equal_fn) {

    { // Test ints
        int arr1[10]          = {5, 7, 3, 4, 6, 4, 7, 1, 9, 5};
        int arr2[10]          = {5, 7, 3, 4, 6, 4, 7, 1, 9, 5};
        int arr3[10]          = {5, 7, 2, 4, 6, 4, 7, 1, 9, 5};
        int_enum arr4[10]     = {5_e, 7_e, 3_e, 4_e, 6_e, 4_e, 7_e, 1_e, 9_e, 5_e};
        int_enum arr5[10]     = {5_e, 7_e, 2_e, 4_e, 6_e, 4_e, 7_e, 1_e, 9_e, 5_e};
        bad_int_enum arr6[10] = {5_be, 7_be, 3_be, 4_be, 6_be, 4_be, 7_be, 1_be, 9_be, 5_be};

        assert(equal_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), equal_to<>{}));
        assert(equal_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), equal_to<int>{}));
        assert(equal_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), equal_to<unsigned int>{}));

        assert(!equal_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), equal_to<>{}));
        assert(!equal_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), equal_to<int>{}));
        assert(!equal_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), equal_to<unsigned int>{}));

        assert(equal_fn(begin(arr1), end(arr1), begin(arr4), end(arr4), equal_to<>{}));
        assert(equal_fn(begin(arr1), end(arr1), begin(arr4), end(arr4), equal_to<int>{}));
        assert(equal_fn(begin(arr1), end(arr1), begin(arr4), end(arr4), equal_to<unsigned int>{}));

        assert(!equal_fn(begin(arr1), end(arr1), begin(arr5), end(arr5), equal_to<>{}));
        assert(!equal_fn(begin(arr1), end(arr1), begin(arr5), end(arr5), equal_to<int>{}));
        assert(!equal_fn(begin(arr1), end(arr1), begin(arr5), end(arr5), equal_to<unsigned int>{}));

        assert(!equal_fn(begin(arr4), end(arr4), begin(arr5), end(arr5), equal_to<>{}));
        assert(!equal_fn(begin(arr4), end(arr4), begin(arr5), end(arr5), equal_to<int>{}));
        assert(!equal_fn(begin(arr4), end(arr4), begin(arr5), end(arr5), equal_to<unsigned int>{}));

        assert(equal_fn(begin(arr3), end(arr3), begin(arr5), end(arr5), equal_to<>{}));
        assert(equal_fn(begin(arr3), end(arr3), begin(arr5), end(arr5), equal_to<int>{}));
        assert(equal_fn(begin(arr3), end(arr3), begin(arr5), end(arr5), equal_to<unsigned int>{}));

        assert(!equal_fn(begin(arr1), end(arr1), begin(arr6), end(arr6), equal_to<>{}));
        assert(equal_fn(begin(arr1), end(arr1), begin(arr6), end(arr6), equal_to<int>{}));
        assert(equal_fn(begin(arr1), end(arr1), begin(arr6), end(arr6), equal_to<unsigned int>{}));
    }

    { // Test int == long
        int arr1[10]  = {5, 7, 3, 4, 6, 4, 7, 1, 9, 5};
        long arr2[10] = {5, 7, 3, 4, 6, 4, 7, 1, 9, 5};
        long arr3[10] = {5, 7, 3, 4, 6, 4, 7, 1, 8, 5};

        assert(equal_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), equal_to<>{}));
        assert(equal_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), equal_to<int>{}));
        assert(equal_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), equal_to<long>{}));

        assert(!equal_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), equal_to<>{}));
        assert(!equal_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), equal_to<int>{}));
        assert(!equal_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), equal_to<long>{}));
    }

    { // Test int == unsigned int
        int arr1[10]          = {5, 7, 3, 4, 6, -4, 7, 1, 9, 5};
        unsigned int arr2[10] = {5, 7, 3, 4, 6, static_cast<unsigned int>(-4), 7, 1, 9, 5};
        unsigned int arr3[10] = {4, 7, 3, 4, 6, static_cast<unsigned int>(-4), 7, 1, 9, 5};

        assert(equal_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), equal_to<>{}));
        assert(equal_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), equal_to<int>{}));
        assert(equal_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), equal_to<unsigned int>{}));

        assert(!equal_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), equal_to<>{}));
        assert(!equal_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), equal_to<int>{}));
        assert(!equal_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), equal_to<unsigned int>{}));
    }

    { // Test long == unsigned int
        long arr1[10]         = {5, 7, 3, 4, 6, -4, 7, 1, 9, 5};
        unsigned int arr2[10] = {5, 7, 3, 4, 6, static_cast<unsigned int>(-4), 7, 1, 9, 5};
        unsigned int arr3[10] = {4, 7, 3, 4, 6, static_cast<unsigned int>(-4), 7, 1, 9, 5};

        assert(equal_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), equal_to<>{}));
        assert(equal_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), equal_to<int>{}));
        assert(equal_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), equal_to<unsigned int>{}));

        assert(!equal_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), equal_to<>{}));
        assert(!equal_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), equal_to<int>{}));
        assert(!equal_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), equal_to<unsigned int>{}));
    }

    { // Test short == int
        short arr1[10]      = {5, 7, 3, 4, 6, -4, 7, 1, 9, 5};
        int arr2[10]        = {5, 7, 3, 4, 6, -4, 7, 1, 9, 5};
        short_enum arr3[10] = {5_se, 7_se, 3_se, 4_se, 6_se, static_cast<short_enum>(-4), 7_se, 1_se, 9_se, 5_se};
        int_enum arr4[10]   = {5_e, 7_e, 3_e, 4_e, 6_e, static_cast<int_enum>(-4), 7_e, 1_e, 9_e, 5_e};

        assert(equal_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), equal_to<>{}));
        assert(equal_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), equal_to<short>{}));
        assert(equal_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), equal_to<int>{}));

        assert(equal_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), equal_to<>{}));
        assert(equal_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), equal_to<short>{}));
        assert(equal_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), equal_to<int>{}));

        assert(equal_fn(begin(arr1), end(arr1), begin(arr4), end(arr4), equal_to<>{}));
        assert(equal_fn(begin(arr1), end(arr1), begin(arr4), end(arr4), equal_to<short>{}));
        assert(equal_fn(begin(arr1), end(arr1), begin(arr4), end(arr4), equal_to<int>{}));

        assert(equal_fn(begin(arr2), end(arr2), begin(arr3), end(arr3), equal_to<>{}));
        assert(equal_fn(begin(arr2), end(arr2), begin(arr3), end(arr3), equal_to<short>{}));
        assert(equal_fn(begin(arr2), end(arr2), begin(arr3), end(arr3), equal_to<int>{}));

        assert(equal_fn(begin(arr3), end(arr3), begin(arr4), end(arr4), equal_to<>{}));
        assert(equal_fn(begin(arr3), end(arr3), begin(arr4), end(arr4), equal_to<short>{}));
        assert(equal_fn(begin(arr3), end(arr3), begin(arr4), end(arr4), equal_to<int>{}));
    }

    { // Test short == unsigned short
        short arr1[10]          = {5, 7, 3, 4, 6, -4, 7, 1, 9, 5};
        unsigned short arr2[10] = {5, 7, 3, 4, 6, static_cast<unsigned short>(-4), 7, 1, 9, 5};
        short_enum arr3[10]     = {5_se, 7_se, 3_se, 4_se, 6_se, static_cast<short_enum>(-4), 7_se, 1_se, 9_se, 5_se};
        bad_short_enum arr4[10] = {
            5_bse, 7_bse, 3_bse, 4_bse, 6_bse, static_cast<bad_short_enum>(-4), 7_bse, 1_bse, 9_bse, 5_bse};

        assert(!equal_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), equal_to<>{}));
        assert(equal_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), equal_to<short>{}));
        assert(equal_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), equal_to<unsigned short>{}));

        assert(!equal_fn(begin(arr3), end(arr3), begin(arr2), end(arr2), equal_to<>{}));
        assert(equal_fn(begin(arr3), end(arr3), begin(arr2), end(arr2), equal_to<short>{}));
        assert(equal_fn(begin(arr3), end(arr3), begin(arr2), end(arr2), equal_to<unsigned short>{}));

        assert(equal_fn(begin(arr3), end(arr3), begin(arr1), end(arr1), equal_to<>{}));
        assert(equal_fn(begin(arr3), end(arr3), begin(arr1), end(arr1), equal_to<short>{}));
        assert(equal_fn(begin(arr3), end(arr3), begin(arr1), end(arr1), equal_to<unsigned short>{}));

        assert(!equal_fn(begin(arr1), end(arr1), begin(arr4), end(arr4), equal_to<>{}));
        assert(equal_fn(begin(arr1), end(arr1), begin(arr4), end(arr4), equal_to<short>{}));
        assert(equal_fn(begin(arr1), end(arr1), begin(arr4), end(arr4), equal_to<unsigned short>{}));

        assert(!equal_fn(begin(arr3), end(arr3), begin(arr4), end(arr4), equal_to<>{}));
        assert(equal_fn(begin(arr3), end(arr3), begin(arr4), end(arr4), equal_to<short>{}));
        assert(equal_fn(begin(arr3), end(arr3), begin(arr4), end(arr4), equal_to<unsigned short>{}));
    }

    { // Test bool
        bool arr1[7]          = {true, true, true, false, true, false, false};
        bool arr2[7]          = {true, true, true, false, true, false, false};
        bool arr3[7]          = {true, true, true, false, true, false, true};
        bool_enum arr4[7]     = {true_e, true_e, true_e, false_e, true_e, false_e, false_e};
        bad_bool_enum arr5[7] = {true_bad, true_bad, true_bad, false_bad, true_bad, false_bad, false_bad};

        assert(equal_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), equal_to<>{}));
        assert(equal_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), equal_to<bool>{}));
        assert(equal_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), equal_to<char>{}));

        assert(!equal_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), equal_to<>{}));
        assert(!equal_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), equal_to<bool>{}));
        assert(!equal_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), equal_to<char>{}));

        assert(equal_fn(begin(arr1), end(arr1), begin(arr4), end(arr4), equal_to<>{}));
        assert(equal_fn(begin(arr1), end(arr1), begin(arr4), end(arr4), equal_to<bool>{}));
        assert(equal_fn(begin(arr1), end(arr1), begin(arr4), end(arr4), equal_to<char>{}));

        assert(!equal_fn(begin(arr3), end(arr3), begin(arr4), end(arr4), equal_to<>{}));
        assert(!equal_fn(begin(arr3), end(arr3), begin(arr4), end(arr4), equal_to<bool>{}));
        assert(!equal_fn(begin(arr3), end(arr3), begin(arr4), end(arr4), equal_to<char>{}));

        assert(!equal_fn(begin(arr1), end(arr1), begin(arr5), end(arr5), equal_to<>{}));
        assert(equal_fn(begin(arr1), end(arr1), begin(arr5), end(arr5), equal_to<bool>{}));
        assert(equal_fn(begin(arr1), end(arr1), begin(arr5), end(arr5), equal_to<char>{}));

        assert(!equal_fn(begin(arr4), end(arr4), begin(arr5), end(arr5), equal_to<>{}));
        assert(equal_fn(begin(arr4), end(arr4), begin(arr5), end(arr5), equal_to<bool>{}));
        assert(equal_fn(begin(arr4), end(arr4), begin(arr5), end(arr5), equal_to<char>{}));
    }

    { // Test bool == char
        bool arr1[7]      = {true, true, true, false, true, false, false};
        char arr2[7]      = {1, 1, 1, 0, 1, 0, 0};
        char arr3[7]      = {5, 2, 1, 0, -1, 0, 0};
        bool_enum arr4[7] = {true_e, true_e, true_e, false_e, true_e, false_e, false_e};
        char_enum arr5[7] = {1_ce, 1_ce, 1_ce, 0_ce, 1_ce, 0_ce, 0_ce};

        assert(equal_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), equal_to<>{}));
        assert(equal_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), equal_to<char>{}));
        assert(equal_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), equal_to<bool>{}));

        assert(!equal_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), equal_to<>{}));
        assert(!equal_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), equal_to<char>{}));
        assert(equal_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), equal_to<bool>{}));
        assert(equal_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), equal_to<const bool>{}));

        assert(equal_fn(begin(arr1), end(arr1), begin(arr5), end(arr5), equal_to<>{}));
        assert(equal_fn(begin(arr1), end(arr1), begin(arr5), end(arr5), equal_to<char>{}));
        assert(equal_fn(begin(arr1), end(arr1), begin(arr5), end(arr5), equal_to<bool>{}));
        assert(equal_fn(begin(arr1), end(arr1), begin(arr5), end(arr5), equal_to<const bool>{}));

        assert(equal_fn(begin(arr2), end(arr2), begin(arr4), end(arr4), equal_to<>{}));
        assert(equal_fn(begin(arr2), end(arr2), begin(arr4), end(arr4), equal_to<char>{}));
        assert(equal_fn(begin(arr2), end(arr2), begin(arr4), end(arr4), equal_to<bool>{}));

        assert(!equal_fn(begin(arr3), end(arr3), begin(arr4), end(arr4), equal_to<>{}));
        assert(!equal_fn(begin(arr3), end(arr3), begin(arr4), end(arr4), equal_to<char>{}));
        assert(equal_fn(begin(arr3), end(arr3), begin(arr4), end(arr4), equal_to<bool>{}));
        assert(equal_fn(begin(arr3), end(arr3), begin(arr4), end(arr4), equal_to<const bool>{}));
    }

    { // Test floating points
        int arr1[10]    = {5, 7, 3, 4, 0, 4, 7, 1, 9, 5};
        float arr2[10]  = {5.0f, 7.0f, 3.0f, 4.0f, 0.0f, 4.0f, 7.0f, 1.0f, 9.0f, 5.0f};
        float arr3[10]  = {5.0f, 7.0f, 3.0f, 4.0f, -0.0f, 4.0f, 7.0f, 1.0f, 9.0f, 5.0f};
        double arr4[10] = {5.0, 7.0, 3.0, 4.0, 0.0, 4.0, 7.0, 1.0, 9.0, 5.0};
        double arr5[10] = {5.0, 7.0, 3.0, 4.0, -0.0, 4.0, 7.0, 1.0, 9.0, 5.0};
        float arr6[10]  = {5.0f, 3.0f, 3.0f, 4.0f, -0.0f, 4.0f, 7.0f, 1.0f, 9.0f, 5.0f};
        double arr7[10] = {5.0, 3.0, 3.0, 4.0, -0.0, 4.0, 7.0, 1.0, 9.0, 5.0};

        assert(equal_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), equal_to<>{}));
        assert(equal_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), equal_to<>{}));
        assert(equal_fn(begin(arr1), end(arr1), begin(arr4), end(arr4), equal_to<>{}));
        assert(equal_fn(begin(arr1), end(arr1), begin(arr5), end(arr5), equal_to<>{}));
        assert(!equal_fn(begin(arr1), end(arr1), begin(arr6), end(arr6), equal_to<>{}));
        assert(!equal_fn(begin(arr1), end(arr1), begin(arr7), end(arr7), equal_to<>{}));

        assert(equal_fn(begin(arr2), end(arr2), begin(arr3), end(arr3), equal_to<>{}));
        assert(equal_fn(begin(arr2), end(arr2), begin(arr4), end(arr4), equal_to<>{}));
        assert(equal_fn(begin(arr2), end(arr2), begin(arr5), end(arr5), equal_to<>{}));
        assert(!equal_fn(begin(arr2), end(arr2), begin(arr6), end(arr6), equal_to<>{}));
        assert(!equal_fn(begin(arr2), end(arr2), begin(arr7), end(arr7), equal_to<>{}));

        assert(equal_fn(begin(arr3), end(arr3), begin(arr4), end(arr4), equal_to<>{}));
        assert(equal_fn(begin(arr3), end(arr3), begin(arr5), end(arr5), equal_to<>{}));
        assert(!equal_fn(begin(arr3), end(arr3), begin(arr6), end(arr6), equal_to<>{}));
        assert(!equal_fn(begin(arr3), end(arr3), begin(arr7), end(arr7), equal_to<>{}));

        assert(equal_fn(begin(arr4), end(arr4), begin(arr5), end(arr5), equal_to<>{}));
        assert(!equal_fn(begin(arr4), end(arr4), begin(arr6), end(arr6), equal_to<>{}));
        assert(!equal_fn(begin(arr4), end(arr4), begin(arr7), end(arr7), equal_to<>{}));

        assert(!equal_fn(begin(arr5), end(arr5), begin(arr6), end(arr6), equal_to<>{}));
        assert(!equal_fn(begin(arr5), end(arr5), begin(arr7), end(arr7), equal_to<>{}));

        assert(equal_fn(begin(arr6), end(arr6), begin(arr7), end(arr7), equal_to<>{}));
    }

    { // Test pointers
        int objs[5]{};
        int* arr1[5]        = {&objs[0], &objs[1], nullptr, &objs[3], &objs[4]};
        int* arr2[5]        = {&objs[0], &objs[1], nullptr, &objs[3], &objs[4]};
        const int* arr3[5]  = {&objs[0], &objs[1], nullptr, &objs[3], &objs[4]};
        void* arr4[5]       = {&objs[0], &objs[1], nullptr, &objs[3], &objs[4]};
        void* arr5[5]       = {&objs[0], &objs[1], nullptr, &objs[3], &objs[4]};
        const void* arr6[5] = {&objs[0], &objs[1], nullptr, &objs[3], &objs[4]};
        int* arr7[5]        = {&objs[0], &objs[2], nullptr, &objs[3], &objs[4]};
        void* arr8[5]       = {&objs[0], &objs[2], nullptr, &objs[3], &objs[4]};

        assert(equal_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), equal_to<>{}));
        assert(equal_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), equal_to<int*>{}));
        assert(equal_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), equal_to<const int*>{}));
        assert(equal_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), equal_to<void*>{}));
        assert(equal_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), equal_to<const void*>{}));

        assert(equal_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), equal_to<>{}));
        assert(equal_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), equal_to<const int*>{}));
        assert(equal_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), equal_to<const void*>{}));

        assert(equal_fn(begin(arr4), end(arr4), begin(arr5), end(arr5), equal_to<>{}));
        assert(equal_fn(begin(arr4), end(arr4), begin(arr5), end(arr5), equal_to<void*>{}));
        assert(equal_fn(begin(arr4), end(arr4), begin(arr5), end(arr5), equal_to<const void*>{}));

        assert(equal_fn(begin(arr4), end(arr4), begin(arr6), end(arr6), equal_to<>{}));
        assert(equal_fn(begin(arr4), end(arr4), begin(arr6), end(arr6), equal_to<const void*>{}));

        assert(equal_fn(begin(arr1), end(arr1), begin(arr4), end(arr4), equal_to<>{}));
        assert(equal_fn(begin(arr1), end(arr1), begin(arr4), end(arr4), equal_to<void*>{}));
        assert(equal_fn(begin(arr1), end(arr1), begin(arr4), end(arr4), equal_to<const void*>{}));

        assert(equal_fn(begin(arr1), end(arr1), begin(arr6), end(arr6), equal_to<>{}));
        assert(equal_fn(begin(arr1), end(arr1), begin(arr6), end(arr6), equal_to<const void*>{}));

        assert(equal_fn(begin(arr2), end(arr2), begin(arr5), end(arr5), equal_to<>{}));
        assert(equal_fn(begin(arr2), end(arr2), begin(arr5), end(arr5), equal_to<void*>{}));
        assert(equal_fn(begin(arr2), end(arr2), begin(arr5), end(arr5), equal_to<const void*>{}));

        assert(equal_fn(begin(arr2), end(arr2), begin(arr6), end(arr6), equal_to<>{}));
        assert(equal_fn(begin(arr2), end(arr2), begin(arr6), end(arr6), equal_to<const void*>{}));

        assert(equal_fn(begin(arr3), end(arr3), begin(arr4), end(arr4), equal_to<>{}));
        assert(equal_fn(begin(arr3), end(arr3), begin(arr4), end(arr4), equal_to<const void*>{}));

        assert(equal_fn(begin(arr3), end(arr3), begin(arr6), end(arr6), equal_to<>{}));
        assert(equal_fn(begin(arr3), end(arr3), begin(arr6), end(arr6), equal_to<const void*>{}));

        assert(!equal_fn(begin(arr1), end(arr1), begin(arr7), end(arr7), equal_to<>{}));
        assert(!equal_fn(begin(arr1), end(arr1), begin(arr7), end(arr7), equal_to<int*>{}));
        assert(!equal_fn(begin(arr1), end(arr1), begin(arr7), end(arr7), equal_to<const int*>{}));
        assert(!equal_fn(begin(arr1), end(arr1), begin(arr7), end(arr7), equal_to<void*>{}));
        assert(!equal_fn(begin(arr1), end(arr1), begin(arr7), end(arr7), equal_to<const void*>{}));

        assert(!equal_fn(begin(arr3), end(arr3), begin(arr7), end(arr7), equal_to<>{}));
        assert(!equal_fn(begin(arr3), end(arr3), begin(arr7), end(arr7), equal_to<const int*>{}));
        assert(!equal_fn(begin(arr3), end(arr3), begin(arr7), end(arr7), equal_to<const void*>{}));

        assert(!equal_fn(begin(arr4), end(arr4), begin(arr8), end(arr8), equal_to<>{}));
        assert(!equal_fn(begin(arr4), end(arr4), begin(arr8), end(arr8), equal_to<void*>{}));
        assert(!equal_fn(begin(arr4), end(arr4), begin(arr8), end(arr8), equal_to<const void*>{}));

        assert(!equal_fn(begin(arr6), end(arr6), begin(arr8), end(arr8), equal_to<>{}));
        assert(!equal_fn(begin(arr6), end(arr6), begin(arr8), end(arr8), equal_to<const void*>{}));

        assert(equal_fn(begin(arr7), end(arr7), begin(arr8), end(arr8), equal_to<>{}));
        assert(equal_fn(begin(arr7), end(arr7), begin(arr8), end(arr8), equal_to<void*>{}));
        assert(equal_fn(begin(arr7), end(arr7), begin(arr8), end(arr8), equal_to<const void*>{}));
    }

    { // Test pointers to derived classes
        EmptyDerived objs[5]{};

        EmptyBase* arr1[5]    = {&objs[0], nullptr, &objs[2], &objs[3], &objs[4]};
        EmptyBase* arr2[5]    = {&objs[0], nullptr, &objs[2], &objs[3], &objs[4]};
        EmptyBase* arr3[5]    = {&objs[0], nullptr, &objs[1], &objs[3], &objs[4]};
        EmptyDerived* arr4[5] = {&objs[0], nullptr, &objs[2], &objs[3], &objs[4]};
        EmptyDerived* arr5[5] = {&objs[0], nullptr, &objs[1], &objs[3], &objs[4]};

        assert(equal_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), equal_to<>{}));
        assert(equal_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), equal_to<EmptyBase*>{}));
        assert(equal_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), equal_to<void*>{}));

        assert(!equal_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), equal_to<>{}));
        assert(!equal_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), equal_to<EmptyBase*>{}));
        assert(!equal_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), equal_to<void*>{}));

        assert(equal_fn(begin(arr1), end(arr1), begin(arr4), end(arr4), equal_to<>{}));
        assert(equal_fn(begin(arr1), end(arr1), begin(arr4), end(arr4), equal_to<EmptyBase*>{}));
        assert(equal_fn(begin(arr1), end(arr1), begin(arr4), end(arr4), equal_to<void*>{}));

        assert(!equal_fn(begin(arr1), end(arr1), begin(arr5), end(arr5), equal_to<>{}));
        assert(!equal_fn(begin(arr1), end(arr1), begin(arr5), end(arr5), equal_to<EmptyBase*>{}));
        assert(!equal_fn(begin(arr1), end(arr1), begin(arr5), end(arr5), equal_to<void*>{}));

        assert(equal_fn(begin(arr3), end(arr3), begin(arr5), end(arr5), equal_to<>{}));
        assert(equal_fn(begin(arr3), end(arr3), begin(arr5), end(arr5), equal_to<EmptyBase*>{}));
        assert(equal_fn(begin(arr3), end(arr3), begin(arr5), end(arr5), equal_to<void*>{}));

        assert(!equal_fn(begin(arr4), end(arr4), begin(arr5), end(arr5), equal_to<>{}));
        assert(!equal_fn(begin(arr4), end(arr4), begin(arr5), end(arr5), equal_to<EmptyBase*>{}));
        assert(!equal_fn(begin(arr4), end(arr4), begin(arr5), end(arr5), equal_to<void*>{}));
    }

    { // Test pointers to derived classes more
        StatefulDerived objs[5]{};

        EmptyBase* arr1[5]       = {&objs[0], nullptr, &objs[2], &objs[3], &objs[4]};
        EmptyBase* arr2[5]       = {&objs[0], nullptr, &objs[1], &objs[3], &objs[4]};
        StatefulBase* arr3[5]    = {&objs[0], nullptr, &objs[2], &objs[3], &objs[4]};
        StatefulBase* arr4[5]    = {&objs[0], nullptr, &objs[1], &objs[3], &objs[4]};
        StatefulDerived* arr5[5] = {&objs[0], nullptr, &objs[2], &objs[3], &objs[4]};
        StatefulDerived* arr6[5] = {&objs[0], nullptr, &objs[1], &objs[3], &objs[4]};

        assert(equal_fn(begin(arr1), end(arr1), begin(arr5), end(arr5), equal_to<>{}));
        assert(equal_fn(begin(arr1), end(arr1), begin(arr5), end(arr5), equal_to<EmptyBase*>{}));
        assert(!equal_fn(begin(arr1), end(arr1), begin(arr5), end(arr5), equal_to<void*>{}));

        assert(!equal_fn(begin(arr1), end(arr1), begin(arr6), end(arr6), equal_to<>{}));
        assert(!equal_fn(begin(arr1), end(arr1), begin(arr6), end(arr6), equal_to<EmptyBase*>{}));
        assert(!equal_fn(begin(arr1), end(arr1), begin(arr6), end(arr6), equal_to<void*>{}));

        assert(!equal_fn(begin(arr2), end(arr2), begin(arr5), end(arr5), equal_to<>{}));
        assert(!equal_fn(begin(arr2), end(arr2), begin(arr5), end(arr5), equal_to<EmptyBase*>{}));
        assert(!equal_fn(begin(arr2), end(arr2), begin(arr5), end(arr5), equal_to<void*>{}));

        assert(equal_fn(begin(arr2), end(arr2), begin(arr6), end(arr6), equal_to<>{}));
        assert(equal_fn(begin(arr2), end(arr2), begin(arr6), end(arr6), equal_to<EmptyBase*>{}));
        assert(!equal_fn(begin(arr2), end(arr2), begin(arr6), end(arr6), equal_to<void*>{}));

        assert(equal_fn(begin(arr3), end(arr3), begin(arr5), end(arr5), equal_to<>{}));
        assert(equal_fn(begin(arr3), end(arr3), begin(arr5), end(arr5), equal_to<StatefulBase*>{}));
        assert(equal_fn(begin(arr3), end(arr3), begin(arr5), end(arr5), equal_to<void*>{}));

        assert(!equal_fn(begin(arr3), end(arr3), begin(arr6), end(arr6), equal_to<>{}));
        assert(!equal_fn(begin(arr3), end(arr3), begin(arr6), end(arr6), equal_to<StatefulBase*>{}));
        assert(!equal_fn(begin(arr3), end(arr3), begin(arr6), end(arr6), equal_to<void*>{}));

        assert(!equal_fn(begin(arr4), end(arr4), begin(arr5), end(arr5), equal_to<>{}));
        assert(!equal_fn(begin(arr4), end(arr4), begin(arr5), end(arr5), equal_to<StatefulBase*>{}));
        assert(!equal_fn(begin(arr4), end(arr4), begin(arr5), end(arr5), equal_to<void*>{}));

        assert(equal_fn(begin(arr4), end(arr4), begin(arr6), end(arr6), equal_to<>{}));
        assert(equal_fn(begin(arr4), end(arr4), begin(arr6), end(arr6), equal_to<StatefulBase*>{}));
        assert(equal_fn(begin(arr4), end(arr4), begin(arr6), end(arr6), equal_to<void*>{}));
    }

    { // Test pointers to derived classes even more
        StatefulDerived2 objs[5]{};

        EmptyBase* arr1[5]        = {&objs[0], nullptr, &objs[2], &objs[3], &objs[4]};
        EmptyBase* arr2[5]        = {&objs[0], nullptr, &objs[1], &objs[3], &objs[4]};
        StatefulBase* arr3[5]     = {&objs[0], nullptr, &objs[2], &objs[3], &objs[4]};
        StatefulBase* arr4[5]     = {&objs[0], nullptr, &objs[1], &objs[3], &objs[4]};
        StatefulDerived2* arr5[5] = {&objs[0], nullptr, &objs[2], &objs[3], &objs[4]};
        StatefulDerived2* arr6[5] = {&objs[0], nullptr, &objs[1], &objs[3], &objs[4]};

        assert(equal_fn(begin(arr1), end(arr1), begin(arr5), end(arr5), equal_to<>{}));
        assert(equal_fn(begin(arr1), end(arr1), begin(arr5), end(arr5), equal_to<EmptyBase*>{}));
        assert(equal_fn(begin(arr1), end(arr1), begin(arr5), end(arr5), equal_to<void*>{}));

        assert(!equal_fn(begin(arr1), end(arr1), begin(arr6), end(arr6), equal_to<>{}));
        assert(!equal_fn(begin(arr1), end(arr1), begin(arr6), end(arr6), equal_to<EmptyBase*>{}));
        assert(!equal_fn(begin(arr1), end(arr1), begin(arr6), end(arr6), equal_to<void*>{}));

        assert(!equal_fn(begin(arr2), end(arr2), begin(arr5), end(arr5), equal_to<>{}));
        assert(!equal_fn(begin(arr2), end(arr2), begin(arr5), end(arr5), equal_to<EmptyBase*>{}));
        assert(!equal_fn(begin(arr2), end(arr2), begin(arr5), end(arr5), equal_to<void*>{}));

        assert(equal_fn(begin(arr2), end(arr2), begin(arr6), end(arr6), equal_to<>{}));
        assert(equal_fn(begin(arr2), end(arr2), begin(arr6), end(arr6), equal_to<EmptyBase*>{}));
        assert(equal_fn(begin(arr2), end(arr2), begin(arr6), end(arr6), equal_to<void*>{}));

        assert(equal_fn(begin(arr3), end(arr3), begin(arr5), end(arr5), equal_to<>{}));
        assert(equal_fn(begin(arr3), end(arr3), begin(arr5), end(arr5), equal_to<StatefulBase*>{}));
        assert(equal_fn(begin(arr3), end(arr3), begin(arr5), end(arr5), equal_to<void*>{}));

        assert(!equal_fn(begin(arr3), end(arr3), begin(arr6), end(arr6), equal_to<>{}));
        assert(!equal_fn(begin(arr3), end(arr3), begin(arr6), end(arr6), equal_to<StatefulBase*>{}));
        assert(!equal_fn(begin(arr3), end(arr3), begin(arr6), end(arr6), equal_to<void*>{}));

        assert(!equal_fn(begin(arr4), end(arr4), begin(arr5), end(arr5), equal_to<>{}));
        assert(!equal_fn(begin(arr4), end(arr4), begin(arr5), end(arr5), equal_to<StatefulBase*>{}));
        assert(!equal_fn(begin(arr4), end(arr4), begin(arr5), end(arr5), equal_to<void*>{}));

        assert(equal_fn(begin(arr4), end(arr4), begin(arr6), end(arr6), equal_to<>{}));
        assert(equal_fn(begin(arr4), end(arr4), begin(arr6), end(arr6), equal_to<StatefulBase*>{}));
        assert(equal_fn(begin(arr4), end(arr4), begin(arr6), end(arr6), equal_to<void*>{}));
    }

    { // Test function pointers
        using fn_ptr   = void (*)(int);
        fn_ptr lambda1 = [](int) { abort(); };
        fn_ptr lambda2 = [](int) { exit(0); };
        fn_ptr arr1[5] = {lambda1, lambda2, nullptr, lambda1, nullptr};
        fn_ptr arr2[5] = {lambda1, lambda2, nullptr, lambda1, nullptr};
        fn_ptr arr3[5] = {lambda1, lambda1, nullptr, lambda1, nullptr};

        assert(equal_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), equal_to<>{}));
        assert(!equal_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), equal_to<>{}));

#ifndef __clang__
        if constexpr (is_convertible_v<void (*)(int), void*>) { // Test void (*)(int) == void* (non-standard extension)
            void* arr4[5] = {lambda1, lambda2, nullptr, lambda1, nullptr};
            void* arr5[5] = {lambda1, lambda1, nullptr, lambda1, nullptr};

            assert(equal_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), equal_to<void*>{}));
            assert(!equal_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), equal_to<void*>{}));

            assert(equal_fn(begin(arr1), end(arr1), begin(arr4), end(arr4), equal_to<>{}));
            assert(equal_fn(begin(arr1), end(arr1), begin(arr4), end(arr4), equal_to<void*>{}));
            assert(!equal_fn(begin(arr1), end(arr1), begin(arr5), end(arr5), equal_to<>{}));
            assert(!equal_fn(begin(arr1), end(arr1), begin(arr5), end(arr5), equal_to<void*>{}));
            assert(equal_fn(begin(arr3), end(arr3), begin(arr5), end(arr5), equal_to<>{}));
            assert(equal_fn(begin(arr3), end(arr3), begin(arr5), end(arr5), equal_to<void*>{}));
        }
#endif // __clang__
    }

    { // Test member object pointers
        using m_ptr    = int StatefulBase::*;
        using m_ptr2   = int StatefulDerived::*;
        m_ptr arr1[5]  = {&StatefulBase::i, &StatefulBase::i, nullptr, &StatefulBase::i, nullptr};
        m_ptr arr2[5]  = {&StatefulBase::i, &StatefulBase::i, nullptr, &StatefulBase::i, nullptr};
        m_ptr arr3[5]  = {nullptr, &StatefulBase::i, nullptr, &StatefulBase::i, nullptr};
        m_ptr2 arr4[5] = {&StatefulDerived::i, &StatefulBase::i, nullptr, &StatefulDerived::i, nullptr};
        m_ptr2 arr5[5] = {nullptr, &StatefulBase::i, nullptr, &StatefulDerived::i, nullptr};

        assert(equal_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), equal_to<>{}));
        assert(!equal_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), equal_to<>{}));
        assert(equal_fn(begin(arr1), end(arr1), begin(arr4), end(arr4), equal_to<>{}));
        assert(!equal_fn(begin(arr1), end(arr1), begin(arr5), end(arr5), equal_to<>{}));
        assert(equal_fn(begin(arr3), end(arr3), begin(arr5), end(arr5), equal_to<>{}));
    }

#ifndef _M_CEE_PURE // TRANSITION, VSO-1409786
    { // Test member function pointers
        using mfn_ptr    = int (StatefulBase::*)();
        using mfn_ptr2   = int (StatefulDerived::*)();
        mfn_ptr arr1[5]  = {&StatefulBase::get_i, &StatefulBase::get_i2, nullptr, &StatefulBase::get_i2, nullptr};
        mfn_ptr arr2[5]  = {&StatefulBase::get_i, &StatefulBase::get_i2, nullptr, &StatefulBase::get_i2, nullptr};
        mfn_ptr arr3[5]  = {&StatefulBase::get_i, &StatefulBase::get_i, nullptr, &StatefulBase::get_i2, nullptr};
        mfn_ptr2 arr4[5] = {&StatefulDerived::get_i, &StatefulBase::get_i2, nullptr, &StatefulDerived::get_i2, nullptr};
        mfn_ptr2 arr5[5] = {&StatefulDerived::get_i, &StatefulBase::get_i2, nullptr, &StatefulDerived::get_i2, nullptr};
        mfn_ptr2 arr6[5] = {&StatefulDerived::get_i, &StatefulBase::get_i2, nullptr, &StatefulDerived::get_i3, nullptr};
        mfn_ptr2 arr7[5] = {&StatefulDerived::get_i, &StatefulBase::get_i, nullptr, &StatefulDerived::get_i3, nullptr};

#if _HAS_CXX20 && !defined(__clang__) && !defined(__EDG__) // TRANSITION, DevCom-1535084
#define assert2(x)
#else // ^^^ ICE ^^^ / vvv no ICE vvv
#define assert2 assert
#endif // ^^^ no ICE ^^^

        assert(equal_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), equal_to<>{}));
        assert(equal_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), equal_to<mfn_ptr>{}));
        assert2(equal_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), equal_to<mfn_ptr2>{}));
        assert(!equal_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), equal_to<>{}));
        assert(!equal_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), equal_to<mfn_ptr>{}));
        assert2(!equal_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), equal_to<mfn_ptr2>{}));

        assert(equal_fn(begin(arr1), end(arr1), begin(arr4), end(arr4), equal_to<>{}));
        assert2(equal_fn(begin(arr1), end(arr1), begin(arr4), end(arr4), equal_to<mfn_ptr2>{}));
        assert(!equal_fn(begin(arr1), end(arr1), begin(arr6), end(arr6), equal_to<>{}));
        assert2(!equal_fn(begin(arr1), end(arr1), begin(arr6), end(arr6), equal_to<mfn_ptr2>{}));
        assert(!equal_fn(begin(arr1), end(arr1), begin(arr7), end(arr7), equal_to<>{}));
        assert2(!equal_fn(begin(arr1), end(arr1), begin(arr7), end(arr7), equal_to<mfn_ptr2>{}));

        assert(!equal_fn(begin(arr3), end(arr3), begin(arr4), end(arr4), equal_to<>{}));
        assert2(!equal_fn(begin(arr3), end(arr3), begin(arr4), end(arr4), equal_to<mfn_ptr2>{}));
        assert(!equal_fn(begin(arr3), end(arr3), begin(arr6), end(arr6), equal_to<>{}));
        assert2(!equal_fn(begin(arr3), end(arr3), begin(arr6), end(arr6), equal_to<mfn_ptr2>{}));
        assert(!equal_fn(begin(arr3), end(arr3), begin(arr7), end(arr7), equal_to<>{}));
        assert2(!equal_fn(begin(arr3), end(arr3), begin(arr7), end(arr7), equal_to<mfn_ptr2>{}));

        assert(equal_fn(begin(arr4), end(arr4), begin(arr5), end(arr5), equal_to<>{}));
        assert(equal_fn(begin(arr4), end(arr4), begin(arr5), end(arr5), equal_to<mfn_ptr2>{}));
        assert(!equal_fn(begin(arr4), end(arr4), begin(arr6), end(arr6), equal_to<>{}));
        assert(!equal_fn(begin(arr4), end(arr4), begin(arr6), end(arr6), equal_to<mfn_ptr2>{}));
        assert(!equal_fn(begin(arr4), end(arr4), begin(arr7), end(arr7), equal_to<>{}));
        assert(!equal_fn(begin(arr4), end(arr4), begin(arr7), end(arr7), equal_to<mfn_ptr2>{}));

        assert(!equal_fn(begin(arr6), end(arr6), begin(arr7), end(arr7), equal_to<>{}));
        assert(!equal_fn(begin(arr6), end(arr6), begin(arr7), end(arr7), equal_to<mfn_ptr2>{}));
    }
#endif // _M_CEE_PURE

    { // Test vector
        vector<int> arr1 = {3, 6, 4, 7, 3};
        vector<int> arr2 = {3, 6, 4, 7, 3};
        vector<int> arr3 = {3, 6, 3, 7, 3};

        assert(equal_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), equal_to<>{}));
        assert(!equal_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), equal_to<>{}));
    }

    { // Test array
        array<int, 5> arr1 = {3, 6, 4, 7, 3};
        array<int, 5> arr2 = {3, 6, 4, 7, 3};
        array<int, 5> arr3 = {3, 6, 3, 7, 3};

        assert(equal_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), equal_to<>{}));
        assert(!equal_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), equal_to<>{}));
    }

    { // Test comparing array and vector
        vector<int> arr1   = {3, 6, 4, 7, 3};
        array<int, 5> arr2 = {3, 6, 4, 7, 3};
        array<int, 5> arr3 = {3, 6, 3, 7, 3};
        vector<int> arr4   = {3, 6, 3, 7, 3};

        assert(equal_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), equal_to<>{}));
        assert(equal_fn(begin(arr3), end(arr3), begin(arr4), end(arr4), equal_to<>{}));
        assert(!equal_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), equal_to<>{}));
        assert(!equal_fn(begin(arr2), end(arr2), begin(arr4), end(arr4), equal_to<>{}));
    }

    { // Test list
        list<int> arr1 = {3, 6, 4, 7, 3};
        list<int> arr2 = {3, 6, 4, 7, 3};
        list<int> arr3 = {3, 6, 3, 7, 3};

        assert(equal_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), equal_to<>{}));
        assert(!equal_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), equal_to<>{}));
    }

    { // Test comparing list and vector
        vector<int> arr1 = {3, 6, 4, 7, 3};
        list<int> arr2   = {3, 6, 4, 7, 3};
        list<int> arr3   = {3, 6, 3, 7, 3};
        vector<int> arr4 = {3, 6, 3, 7, 3};

        assert(equal_fn(begin(arr1), end(arr1), begin(arr2), end(arr2), equal_to<>{}));
        assert(equal_fn(begin(arr3), end(arr3), begin(arr4), end(arr4), equal_to<>{}));
        assert(!equal_fn(begin(arr1), end(arr1), begin(arr3), end(arr3), equal_to<>{}));
        assert(!equal_fn(begin(arr2), end(arr2), begin(arr4), end(arr4), equal_to<>{}));
    }
}

int main() {
    test_algorithms([](auto begin1, auto end1, auto begin2, auto end2, auto pred) {
        return equal(begin1, end1, begin2, end2, pred);
    });

#ifdef __cpp_lib_concepts
    test_algorithms([](auto begin1, auto end1, auto begin2, auto end2, auto pred) {
        return ranges::equal(begin1, end1, begin2, end2, pred);
    });
#endif // test_algorithms
}

#ifdef __cpp_lib_concepts
// Also test GH-1523, in which std::equal didn't properly convert non-pointer contiguous iterators to pointers.
struct gh1523_iter {
    // a contiguous_iterator that doesn't unwrap into a pointer
    using iterator_concept  = contiguous_iterator_tag;
    using iterator_category = random_access_iterator_tag;
    using value_type        = int;

    int* ptr = nullptr;

    int& operator*() const {
        return *ptr;
    }
    gh1523_iter& operator++() {
        return *this;
    }
    gh1523_iter operator++(int) {
        return {};
    }
    gh1523_iter& operator--() {
        return *this;
    }
    gh1523_iter operator--(int) {
        return {};
    }
    ptrdiff_t operator-(const gh1523_iter&) const {
        return 0;
    }
    auto operator<=>(const gh1523_iter&) const = default;
    gh1523_iter& operator-=(ptrdiff_t) {
        return *this;
    }
    gh1523_iter operator-(ptrdiff_t) const {
        return {};
    }
    gh1523_iter& operator+=(ptrdiff_t) {
        return *this;
    }
    gh1523_iter operator+(ptrdiff_t) const {
        return {};
    }
    friend gh1523_iter operator+(ptrdiff_t, const gh1523_iter&) {
        return {};
    }
    int& operator[](ptrdiff_t) const {
        return *ptr;
    }
};

template <>
struct pointer_traits<gh1523_iter> {
    using pointer         = gh1523_iter;
    using element_type    = int;
    using difference_type = ptrdiff_t;

    static int* to_address(const pointer&) noexcept {
        return nullptr;
    }
};
static_assert(contiguous_iterator<gh1523_iter>);

void test_gh_1523() { // COMPILE-ONLY
    // GH-1523 Some StdLib algorithms fail /std:c++latest compilation with custom contiguous iterators
    (void) equal(gh1523_iter{}, gh1523_iter{}, gh1523_iter{}, gh1523_iter{});
}
#endif // __cpp_lib_concepts
