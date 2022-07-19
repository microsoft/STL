// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <iterator>
#include <list>
#include <memory>
#include <type_traits>
#include <vector>

#if _HAS_CXX17
#include <string_view>
#endif // _HAS_CXX17

#if _HAS_CXX20
#include <span>
#endif // _HAS_CXX20

#pragma warning(disable : 4242) // conversion from 'X' to 'Y', possible loss of data
#pragma warning(disable : 4244) // conversion from 'X' to 'Y', possible loss of data (Yes, duplicated message.)
#pragma warning(disable : 4365) // conversion from 'X' to 'Y', signed/unsigned mismatch
#pragma warning(disable : 4984) // 'if constexpr' is a C++17 language extension

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

struct TrivialStruct {
    int i;

    bool operator==(const TrivialStruct& right) const {
        return i == right.i;
    }
};
STATIC_ASSERT(is_trivial_v<TrivialStruct>);

struct TriviallyCopyableStruct {
    int i;
    TriviallyCopyableStruct() {}
    TriviallyCopyableStruct(int j) : i(j) {}

    bool operator==(const TriviallyCopyableStruct& right) const {
        return i == right.i;
    }
};
STATIC_ASSERT(is_trivially_copyable_v<TriviallyCopyableStruct>);
STATIC_ASSERT(!is_trivial_v<TriviallyCopyableStruct>);

struct TriviallyMovableStruct {
    int i;
    TriviallyMovableStruct() {}
    TriviallyMovableStruct(int j) : i(j) {}
    TriviallyMovableStruct(const TriviallyMovableStruct&)            = delete;
    TriviallyMovableStruct(TriviallyMovableStruct&&)                 = default;
    TriviallyMovableStruct& operator=(const TriviallyMovableStruct&) = delete;
    TriviallyMovableStruct& operator=(TriviallyMovableStruct&&)      = default;

    bool operator==(const TriviallyMovableStruct& right) const {
        return i == right.i;
    }
};
STATIC_ASSERT(is_trivially_copyable_v<TriviallyMovableStruct>);
STATIC_ASSERT(!is_trivial_v<TriviallyMovableStruct>);

enum int_enum : int {};
enum char_enum : char {};
enum bool_enum : bool { false_e = false, true_e = true };

constexpr int_enum operator""_e(unsigned long long value) noexcept {
    return static_cast<int_enum>(value);
}

constexpr char_enum operator""_ce(unsigned long long value) noexcept {
    return static_cast<char_enum>(value);
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


template <class CopyFn>
void test_algorithms(CopyFn copy_fn) {

    { // Test ints
        int src[10]      = {5, 7, 3, 4, 6, 4, 7, 1, 9, 5};
        int expected[10] = {5, 7, 3, 4, 6, 4, 7, 1, 9, 5};
        int dst[10]{};

        copy_fn(begin(src), end(src), begin(dst));

        assert(equal(begin(expected), end(expected), begin(dst), end(dst)));
    }

    { // Test int => long
        int src[10]       = {5, 7, 3, 4, 6, 4, 7, 1, 9, 5};
        long expected[10] = {5, 7, 3, 4, 6, 4, 7, 1, 9, 5};
        long dst[10]{};

        copy_fn(begin(src), end(src), begin(dst));

        assert(equal(begin(expected), end(expected), begin(dst), end(dst)));
    }

    { // Test long => unsigned int
        long src[10]              = {5, 7, 3, 4, 6, 4, 7, 1, 9, 5};
        unsigned int expected[10] = {5, 7, 3, 4, 6, 4, 7, 1, 9, 5};
        unsigned int dst[10]{};

        copy_fn(begin(src), end(src), begin(dst));

        assert(equal(begin(expected), end(expected), begin(dst), end(dst)));
    }

    { // Test bool => char
        bool src[7]      = {true, true, true, false, true, false, false};
        char expected[7] = {1, 1, 1, 0, 1, 0, 0};
        char dst[7]{};

        copy_fn(begin(src), end(src), begin(dst));

        assert(equal(begin(expected), end(expected), begin(dst), end(dst)));
    }

    { // Test char => bool
        char src[7]      = {5, 2, 1, 0, static_cast<char>(-1), 0, 0};
        bool expected[7] = {true, true, true, false, true, false, false};
        bool dst[7]{};

        copy_fn(begin(src), end(src), begin(dst));

        assert(equal(begin(expected), end(expected), begin(dst), end(dst)));
    }

    { // Test int => bool
        int src[7]       = {5, 2, 1, 0, -1, 0, 0};
        bool expected[7] = {true, true, true, false, true, false, false};
        bool dst[7]{};

        copy_fn(begin(src), end(src), begin(dst));

        assert(equal(begin(expected), end(expected), begin(dst), end(dst)));
    }

    { // Test int => short
        int src[7]        = {5, 2, 1, 0, -1, 0, 0};
        short expected[7] = {5, 2, 1, 0, -1, 0, 0};
        short dst[7]{};

        copy_fn(begin(src), end(src), begin(dst));

        assert(equal(begin(expected), end(expected), begin(dst), end(dst)));
    }

    { // Test int => unsigned int
        int src[7]               = {5, 2, 1, 0, -1, 0, 0};
        unsigned int expected[7] = {5, 2, 1, 0, static_cast<unsigned int>(-1), 0, 0};
        unsigned int dst[7]{};

        copy_fn(begin(src), end(src), begin(dst));

        assert(equal(begin(expected), end(expected), begin(dst), end(dst)));
    }

    { // Test int_enum => int
        int_enum src[7] = {5_e, 2_e, 1_e, 0_e, 7_e, 0_e, 0_e};
        int expected[7] = {5, 2, 1, 0, 7, 0, 0};
        int dst[7]{};

        copy_fn(begin(src), end(src), begin(dst));

        assert(equal(begin(expected), end(expected), begin(dst), end(dst)));
    }

    { // Test int_enum => unsigned long
        int_enum src[7]           = {5_e, 2_e, 1_e, 0_e, 7_e, 0_e, 0_e};
        unsigned long expected[7] = {5, 2, 1, 0, 7, 0, 0};
        unsigned long dst[7]{};

        copy_fn(begin(src), end(src), begin(dst));

        assert(equal(begin(expected), end(expected), begin(dst), end(dst)));
    }

    { // Test bool_enum => bool
        bool_enum src[7] = {true_e, true_e, true_e, false_e, true_e, false_e, false_e};
        bool expected[7] = {true, true, true, false, true, false, false};
        bool dst[7]{};

        copy_fn(begin(src), end(src), begin(dst));

        assert(equal(begin(expected), end(expected), begin(dst), end(dst)));
    }

    { // Test bool_enum => char
        bool_enum src[7] = {true_e, true_e, true_e, false_e, true_e, false_e, false_e};
        char expected[7] = {1, 1, 1, 0, 1, 0, 0};
        char dst[7]{};

        copy_fn(begin(src), end(src), begin(dst));

        assert(equal(begin(expected), end(expected), begin(dst), end(dst)));
    }

    { // Test char_enum => bool
        char_enum src[7] = {5_ce, 2_ce, 1_ce, 0_ce, 255_ce, 0_ce, 0_ce};
        bool expected[7] = {true, true, true, false, true, false, false};
        bool dst[7]{};

        copy_fn(begin(src), end(src), begin(dst));

        assert(equal(begin(expected), end(expected), begin(dst), end(dst)));
    }

    { // Test floats
        float src[7]      = {5.0f, 2.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f};
        float expected[7] = {5.0f, 2.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f};
        float dst[7]{};

        copy_fn(begin(src), end(src), begin(dst));

        assert(equal(begin(expected), end(expected), begin(dst), end(dst)));
    }

    { // Test float => int
        float src[7]    = {5.0f, 2.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f};
        int expected[7] = {5, 2, 1, 0, -1, 0, 0};
        int dst[7]{};

        copy_fn(begin(src), end(src), begin(dst));

        assert(equal(begin(expected), end(expected), begin(dst), end(dst)));
    }

    { // Test int => float
        int src[7]        = {5, 2, 1, 0, -1, 0, 0};
        float expected[7] = {5.0f, 2.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f};
        float dst[7]{};

        copy_fn(begin(src), end(src), begin(dst));

        assert(equal(begin(expected), end(expected), begin(dst), end(dst)));
    }

    { // Test double => long double
        double src[7]           = {5.0, 2.0, 1.0, 0.0, -1.0, 0.0, 0.0};
        long double expected[7] = {5.0L, 2.0L, 1.0L, 0.0L, -1.0L, 0.0L, 0.0L};
        long double dst[7]{};

        copy_fn(begin(src), end(src), begin(dst));

        assert(equal(begin(expected), end(expected), begin(dst), end(dst)));
    }

    { // Test long double => double
        long double src[7] = {5.0L, 2.0L, 1.0L, 0.0L, -1.0L, 0.0L, 0.0L};
        double expected[7] = {5.0, 2.0, 1.0, 0.0, -1.0, 0.0, 0.0};
        double dst[7]{};

        copy_fn(begin(src), end(src), begin(dst));

        assert(equal(begin(expected), end(expected), begin(dst), end(dst)));
    }

    { // Test int*
        int arr[5]{};
        int* src[5]      = {&arr[0], &arr[1], nullptr, &arr[3], &arr[4]};
        int* expected[5] = {&arr[0], &arr[1], nullptr, &arr[3], &arr[4]};
        int* dst[5]{};

        copy_fn(begin(src), end(src), begin(dst));

        assert(equal(begin(expected), end(expected), begin(dst), end(dst)));
    }

    { // Test void*
        int arr[5]{};
        void* src[5]      = {&arr[0], &arr[1], nullptr, &arr[3], &arr[4]};
        void* expected[5] = {&arr[0], &arr[1], nullptr, &arr[3], &arr[4]};
        void* dst[5]{};

        copy_fn(begin(src), end(src), begin(dst));

        assert(equal(begin(expected), end(expected), begin(dst), end(dst)));
    }

    { // Test int* => void*
        int arr[5]{};
        int* src[5]       = {&arr[0], &arr[1], nullptr, &arr[3], &arr[4]};
        void* expected[5] = {&arr[0], &arr[1], nullptr, &arr[3], &arr[4]};
        void* dst[5]{};

        copy_fn(begin(src), end(src), begin(dst));

        assert(equal(begin(expected), end(expected), begin(dst), end(dst)));
    }

    { // Test int* => const int*
        int arr[5]{};
        int* src[5]            = {&arr[0], &arr[1], nullptr, &arr[3], &arr[4]};
        const int* expected[5] = {&arr[0], &arr[1], nullptr, &arr[3], &arr[4]};
        const int* dst[5]{};

        copy_fn(begin(src), end(src), begin(dst));

        assert(equal(begin(expected), end(expected), begin(dst), end(dst)));
    }

    { // Test EmptyBase*
        EmptyBase arr[5]{};
        EmptyBase* src[5]      = {&arr[0], &arr[1], nullptr, &arr[3], &arr[4]};
        EmptyBase* expected[5] = {&arr[0], &arr[1], nullptr, &arr[3], &arr[4]};
        EmptyBase* dst[5]{};

        copy_fn(begin(src), end(src), begin(dst));

        assert(equal(begin(expected), end(expected), begin(dst), end(dst)));
    }

    { // Test EmptyDerived*
        EmptyDerived arr[5]{};
        EmptyDerived* src[5]      = {&arr[0], &arr[1], nullptr, &arr[3], &arr[4]};
        EmptyDerived* expected[5] = {&arr[0], &arr[1], nullptr, &arr[3], &arr[4]};
        EmptyDerived* dst[5]{};

        copy_fn(begin(src), end(src), begin(dst));

        assert(equal(begin(expected), end(expected), begin(dst), end(dst)));
    }

    { // Test EmptyDerived* => EmptyBase*
        EmptyDerived arr[5]{};
        EmptyDerived* src[5]   = {&arr[0], &arr[1], nullptr, &arr[3], &arr[4]};
        EmptyBase* expected[5] = {&arr[0], &arr[1], nullptr, &arr[3], &arr[4]};
        EmptyBase* dst[5]{};

        copy_fn(begin(src), end(src), begin(dst));

        assert(equal(begin(expected), end(expected), begin(dst), end(dst)));
    }

    { // Test StatefulDerived* => StatefulBase*
        StatefulDerived arr[5]{};
        StatefulDerived* src[5]   = {&arr[0], &arr[1], nullptr, &arr[3], &arr[4]};
        StatefulBase* expected[5] = {&arr[0], &arr[1], nullptr, &arr[3], &arr[4]};
        StatefulBase* dst[5]{};

        copy_fn(begin(src), end(src), begin(dst));

        assert(equal(begin(expected), end(expected), begin(dst), end(dst)));
    }

    { // Test StatefulDerived* => EmptyBase*
        StatefulDerived arr[5]{};
        StatefulDerived* src[5] = {&arr[0], &arr[1], nullptr, &arr[3], &arr[4]};
        EmptyBase* expected[5]  = {&arr[0], &arr[1], nullptr, &arr[3], &arr[4]};
        EmptyBase* dst[5]{};

        copy_fn(begin(src), end(src), begin(dst));

        assert(equal(begin(expected), end(expected), begin(dst), end(dst)));
    }

    { // Test StatefulDerived2* => StatefulBase*
        StatefulDerived2 arr[5]{};
        StatefulDerived2* src[5]  = {&arr[0], &arr[1], nullptr, &arr[3], &arr[4]};
        StatefulBase* expected[5] = {&arr[0], &arr[1], nullptr, &arr[3], &arr[4]};
        StatefulBase* dst[5]{};

        copy_fn(begin(src), end(src), begin(dst));

        assert(equal(begin(expected), end(expected), begin(dst), end(dst)));
    }

    { // Test StatefulDerived2* => EmptyBase*
        StatefulDerived2 arr[5]{};
        StatefulDerived2* src[5] = {&arr[0], &arr[1], nullptr, &arr[3], &arr[4]};
        EmptyBase* expected[5]   = {&arr[0], &arr[1], nullptr, &arr[3], &arr[4]};
        EmptyBase* dst[5]{};

        copy_fn(begin(src), end(src), begin(dst));

        assert(equal(begin(expected), end(expected), begin(dst), end(dst)));
    }

    { // Test void (*)(int)
        using fn_ptr       = void (*)(int);
        auto lambda1       = [](int) { abort(); };
        auto lambda2       = [](int) { exit(0); };
        fn_ptr src[5]      = {lambda1, lambda2, nullptr, lambda1, nullptr};
        fn_ptr expected[5] = {lambda1, lambda2, nullptr, lambda1, nullptr};
        fn_ptr dst[5]{};

        copy_fn(begin(src), end(src), begin(dst));

        assert(equal(begin(expected), end(expected), begin(dst), end(dst)));
    }

#ifndef __clang__
    if constexpr (is_convertible_v<void (*)(int), void*>) { // Test void (*)(int) => void* (non-standard extension)
        using fn_ptr      = void (*)(int);
        fn_ptr lambda1    = [](int) { abort(); };
        fn_ptr lambda2    = [](int) { exit(0); };
        fn_ptr src[5]     = {lambda1, lambda2, nullptr, lambda1, nullptr};
        void* expected[5] = {lambda1, lambda2, nullptr, lambda1, nullptr};
        void* dst[5]{};

        copy_fn(begin(src), end(src), begin(dst));

        assert(equal(begin(expected), end(expected), begin(dst), end(dst)));
    }
#endif // __clang__

    { // Test int StatefulBase::*
        using m_ptr       = int StatefulBase::*;
        m_ptr src[5]      = {&StatefulBase::i, &StatefulBase::i, nullptr, &StatefulBase::i, nullptr};
        m_ptr expected[5] = {&StatefulBase::i, &StatefulBase::i, nullptr, &StatefulBase::i, nullptr};
        m_ptr dst[5]{};

        copy_fn(begin(src), end(src), begin(dst));

        assert(equal(begin(expected), end(expected), begin(dst), end(dst)));
    }

    { // Test int StatefulDerived::*
        using m_ptr       = int StatefulDerived::*;
        m_ptr src[5]      = {&StatefulDerived::i, &StatefulBase::i, nullptr, &StatefulDerived::i, nullptr};
        m_ptr expected[5] = {&StatefulDerived::i, &StatefulBase::i, nullptr, &StatefulDerived::i, nullptr};
        m_ptr dst[5]{};

        copy_fn(begin(src), end(src), begin(dst));

        assert(equal(begin(expected), end(expected), begin(dst), end(dst)));
    }

    { // Test int (StatefulBase::*)()
        using mfn_ptr       = int (StatefulBase::*)();
        mfn_ptr src[5]      = {&StatefulBase::get_i, &StatefulBase::get_i2, nullptr, &StatefulBase::get_i, nullptr};
        mfn_ptr expected[5] = {&StatefulBase::get_i, &StatefulBase::get_i2, nullptr, &StatefulBase::get_i, nullptr};
        mfn_ptr dst[5]{};

        copy_fn(begin(src), end(src), begin(dst));

        assert(equal(begin(expected), end(expected), begin(dst), end(dst)));
    }

    { // Test int (StatefulDerived::*)()
        using mfn_ptr  = int (StatefulDerived::*)();
        mfn_ptr src[5] = {&StatefulDerived::get_i, &StatefulBase::get_i2, nullptr, &StatefulDerived::get_i3, nullptr};
        mfn_ptr expected[5] = {
            &StatefulDerived::get_i, &StatefulBase::get_i2, nullptr, &StatefulDerived::get_i3, nullptr};
        mfn_ptr dst[5]{};

        copy_fn(begin(src), end(src), begin(dst));

        assert(equal(begin(expected), end(expected), begin(dst), end(dst)));
    }

#if _HAS_CXX17
    { // Test string_view
        string_view src[5]      = {"meow"sv, "purr"sv, ""sv, {}, "peppermint"sv};
        string_view expected[5] = {"meow"sv, "purr"sv, ""sv, {}, "peppermint"sv};
        string_view dst[5]{};

        copy_fn(begin(src), end(src), begin(dst));

        assert(equal(begin(expected), end(expected), begin(dst), end(dst)));
    }
#endif // _HAS_CXX17

#if _HAS_CXX20
    { // Test span<int>
        int arr1[3]           = {1, 2, 3};
        int arr2[5]           = {4, 5, 6, 7, 8};
        int arr3[1]           = {9};
        span<int> src[5]      = {arr1, arr3, arr2, {}, arr1};
        span<int> expected[5] = {arr1, arr3, arr2, {}, arr1};
        span<int> dst[5]{};

        copy_fn(begin(src), end(src), begin(dst));

        assert(equal(begin(expected), end(expected), begin(dst), end(dst), [](const auto& left, const auto& right) {
            return left.data() == right.data() && left.size() == right.size();
        }));
    }
#endif // _HAS_CXX20

    { // Test TrivialStruct
        TrivialStruct src[5]      = {{8}, {3}, {6}, {2}, {1}};
        TrivialStruct expected[5] = {{8}, {3}, {6}, {2}, {1}};
        TrivialStruct dst[5]{};

        copy_fn(begin(src), end(src), begin(dst));

        assert(equal(begin(expected), end(expected), begin(dst), end(dst)));
    }

    { // Test TriviallyCopyableStruct
        TriviallyCopyableStruct src[5]      = {{8}, {3}, {6}, {2}, {1}};
        TriviallyCopyableStruct expected[5] = {{8}, {3}, {6}, {2}, {1}};
        TriviallyCopyableStruct dst[5]{};

        copy_fn(begin(src), end(src), begin(dst));

        assert(equal(begin(expected), end(expected), begin(dst), end(dst)));
    }

    { // Test TriviallyMovableStruct
        TriviallyMovableStruct src[5]      = {{8}, {3}, {6}, {2}, {1}};
        TriviallyMovableStruct expected[5] = {{8}, {3}, {6}, {2}, {1}};
        TriviallyMovableStruct dst[5]{};

        copy_fn(make_move_iterator(begin(src)), make_move_iterator(end(src)), begin(dst));

        assert(equal(begin(expected), end(expected), begin(dst), end(dst)));
    }

    { // Test vector
        vector<int> src      = {3, 6, 4, 7, 3};
        vector<int> expected = {3, 6, 4, 7, 3};
        vector<int> dst      = {0, 0, 0, 0, 0};

        copy_fn(begin(src), end(src), begin(dst));

        assert(equal(begin(expected), end(expected), begin(dst), end(dst)));
    }

    { // Test array
        array<int, 5> src      = {3, 6, 4, 7, 3};
        array<int, 5> expected = {3, 6, 4, 7, 3};
        array<int, 5> dst      = {0, 0, 0, 0, 0};

        copy_fn(begin(src), end(src), begin(dst));

        assert(equal(begin(expected), end(expected), begin(dst), end(dst)));
    }

    { // Test copying from vector to array
        vector<int> src        = {3, 6, 4, 7, 3};
        array<int, 5> expected = {3, 6, 4, 7, 3};
        array<int, 5> dst      = {0, 0, 0, 0, 0};

        copy_fn(begin(src), end(src), begin(dst));

        assert(equal(begin(expected), end(expected), begin(dst), end(dst)));
    }

    { // Test copying from array to vector
        array<int, 5> src    = {3, 6, 4, 7, 3};
        vector<int> expected = {3, 6, 4, 7, 3};
        vector<int> dst      = {0, 0, 0, 0, 0};

        copy_fn(begin(src), end(src), begin(dst));

        assert(equal(begin(expected), end(expected), begin(dst), end(dst)));
    }

    { // Test list
        list<int> src      = {3, 6, 4, 7, 3};
        list<int> expected = {3, 6, 4, 7, 3};
        list<int> dst      = {0, 0, 0, 0, 0};

        copy_fn(begin(src), end(src), begin(dst));

        assert(equal(begin(expected), end(expected), begin(dst), end(dst)));
    }

    { // Test copying from vector to list
        vector<int> src    = {3, 6, 4, 7, 3};
        list<int> expected = {3, 6, 4, 7, 3};
        list<int> dst      = {0, 0, 0, 0, 0};

        copy_fn(begin(src), end(src), begin(dst));

        assert(equal(begin(expected), end(expected), begin(dst), end(dst)));
    }

    { // Test copying from list to vector
        list<int> src        = {3, 6, 4, 7, 3};
        vector<int> expected = {3, 6, 4, 7, 3};
        vector<int> dst      = {0, 0, 0, 0, 0};

        copy_fn(begin(src), end(src), begin(dst));

        assert(equal(begin(expected), end(expected), begin(dst), end(dst)));
    }

    { // Test move_iterator
        int src[5]      = {3, 6, 4, 7, 3};
        int expected[5] = {3, 6, 4, 7, 3};
        int dst[5]{};

        copy_fn(make_move_iterator(begin(src)), make_move_iterator(end(src)), begin(dst));

        assert(equal(begin(expected), end(expected), begin(dst), end(dst)));
    }

    { // Test double move_iterator
        int src[5]      = {3, 6, 4, 7, 3};
        int expected[5] = {3, 6, 4, 7, 3};
        int dst[5]{};

        copy_fn(make_move_iterator(make_move_iterator(begin(src))), make_move_iterator(make_move_iterator(end(src))),
            begin(dst));

        assert(equal(begin(expected), end(expected), begin(dst), end(dst)));
    }
}

template <class CopyFn>
void test_uninitialized_algorithms(CopyFn copy_fn) {
    test_algorithms(copy_fn);

    { // Test const destination
        int src[10]      = {5, 7, 3, 4, 6, 4, 7, 1, 9, 5};
        int expected[10] = {5, 7, 3, 4, 6, 4, 7, 1, 9, 5};
        int dst[10]{};

        copy_fn(begin(src), end(src), cbegin(dst));

        assert(equal(begin(expected), end(expected), begin(dst), end(dst)));
    }

    { // Test move_iterator with const destination
        int src[10]      = {5, 7, 3, 4, 6, 4, 7, 1, 9, 5};
        int expected[10] = {5, 7, 3, 4, 6, 4, 7, 1, 9, 5};
        int dst[10]{};

        copy_fn(make_move_iterator(begin(src)), make_move_iterator(end(src)), cbegin(dst));

        assert(equal(begin(expected), end(expected), begin(dst), end(dst)));
    }

    { // Test vector with const destination
        vector<int> src      = {3, 6, 4, 7, 3};
        vector<int> expected = {3, 6, 4, 7, 3};
        vector<int> dst      = {0, 0, 0, 0, 0};

        copy_fn(begin(src), end(src), cbegin(dst));

        assert(equal(begin(expected), end(expected), begin(dst), end(dst)));
    }
}

int main() {
    test_algorithms([](auto begin, auto end, auto out) { copy(begin, end, out); });
    test_algorithms([](auto begin, auto end, auto out) { copy_n(begin, distance(begin, end), out); });
    test_algorithms([](auto begin, auto end, auto out) { copy_backward(begin, end, next(out, distance(begin, end))); });

    test_algorithms([](auto begin, auto end, auto out) { move(begin, end, out); });
    test_algorithms([](auto begin, auto end, auto out) { move_backward(begin, end, next(out, distance(begin, end))); });

    test_uninitialized_algorithms([](auto begin, auto end, auto out) { uninitialized_copy(begin, end, out); });
    test_uninitialized_algorithms(
        [](auto begin, auto end, auto out) { uninitialized_copy_n(begin, distance(begin, end), out); });

#if _HAS_CXX17
    test_uninitialized_algorithms([](auto begin, auto end, auto out) { uninitialized_move(begin, end, out); });
    test_uninitialized_algorithms(
        [](auto begin, auto end, auto out) { uninitialized_move_n(begin, distance(begin, end), out); });
#endif // _HAS_CXX17

#ifdef __cpp_lib_concepts
    test_algorithms([](auto begin, auto end, auto out) { ranges::copy(begin, end, out); });
    test_algorithms([](auto begin, auto end, auto out) { ranges::copy_n(begin, distance(begin, end), out); });
    test_algorithms([](auto begin, auto end, auto out) {
        // Unfortunately move_iterator is not a bidirectional_iterator, so fallback to unconstrained version
        if constexpr (bidirectional_iterator<decltype(begin)>) {
            ranges::copy_backward(begin, end, next(out, distance(begin, end)));
        } else {
            copy_backward(begin, end, next(out, distance(begin, end)));
        }
    });

    test_algorithms([](auto begin, auto end, auto out) { ranges::move(begin, end, out); });
    test_algorithms([](auto begin, auto end, auto out) {
        // Unfortunately move_iterator is not a bidirectional_iterator, so fallback to unconstrained version
        if constexpr (bidirectional_iterator<decltype(begin)>) {
            ranges::move_backward(begin, end, next(out, distance(begin, end)));
        } else {
            move_backward(begin, end, next(out, distance(begin, end)));
        }
    });

    test_uninitialized_algorithms([](auto begin, auto end, auto out) {
        ranges::uninitialized_copy(begin, end, out, next(out, distance(begin, end)));
    });
    test_uninitialized_algorithms(
        [](auto begin, auto end, auto out) { ranges::uninitialized_copy(begin, end, out, unreachable_sentinel); });
    test_uninitialized_algorithms([](auto begin, auto end, auto out) {
        ranges::uninitialized_copy(begin, unreachable_sentinel, out, next(out, distance(begin, end)));
    });

    test_uninitialized_algorithms([](auto begin, auto end, auto out) {
        ranges::uninitialized_copy_n(begin, distance(begin, end), out, next(out, distance(begin, end)));
    });
    test_uninitialized_algorithms([](auto begin, auto end, auto out) {
        ranges::uninitialized_copy_n(begin, distance(begin, end), out, unreachable_sentinel);
    });

    test_uninitialized_algorithms([](auto begin, auto end, auto out) {
        ranges::uninitialized_move(begin, end, out, next(out, distance(begin, end)));
    });
    test_uninitialized_algorithms(
        [](auto begin, auto end, auto out) { ranges::uninitialized_move(begin, end, out, unreachable_sentinel); });
    test_uninitialized_algorithms([](auto begin, auto end, auto out) {
        ranges::uninitialized_move(begin, unreachable_sentinel, out, next(out, distance(begin, end)));
    });

    test_uninitialized_algorithms([](auto begin, auto end, auto out) {
        ranges::uninitialized_move_n(begin, distance(begin, end), out, next(out, distance(begin, end)));
    });
    test_uninitialized_algorithms([](auto begin, auto end, auto out) {
        ranges::uninitialized_move_n(begin, distance(begin, end), out, unreachable_sentinel);
    });
#endif // __cpp_lib_concepts
}
