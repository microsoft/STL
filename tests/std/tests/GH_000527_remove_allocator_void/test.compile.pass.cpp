// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _SILENCE_CXX20_IS_ALWAYS_EQUAL_DEPRECATION_WARNING

#include <memory>
#include <type_traits>
#include <utility>

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

using namespace std;

template <class T, class = void>
constexpr bool has_member_size_type = false;

template <class T>
constexpr bool has_member_size_type<T, void_t<typename T::size_type>> = true;

template <class T, class = void>
constexpr bool has_member_difference_type = false;

template <class T>
constexpr bool has_member_difference_type<T, void_t<typename T::difference_type>> = true;

template <class T, class = void>
constexpr bool has_member_pocma = false;

template <class T>
constexpr bool has_member_pocma<T, void_t<typename T::propagate_on_container_move_assignment>> = true;

template <class T, class = void>
constexpr bool has_member_is_always_equal = false;

template <class T>
constexpr bool has_member_is_always_equal<T, void_t<typename T::is_always_equal>> = true;

template <class T, class = void>
constexpr bool can_allocate = false;

template <class T>
constexpr bool can_allocate<T, void_t<decltype(declval<T&>().allocate(size_t{}))>> = true;

STATIC_ASSERT(has_member_size_type<allocator<int>>);
STATIC_ASSERT(has_member_difference_type<allocator<int>>);
STATIC_ASSERT(has_member_pocma<allocator<int>>);
STATIC_ASSERT(has_member_is_always_equal<allocator<int>>);
STATIC_ASSERT(can_allocate<allocator<int>>);
STATIC_ASSERT(is_convertible_v<allocator<void>, allocator<int>>);

#if _HAS_CXX20
constexpr bool has_cxx20 = true;
#else
constexpr bool has_cxx20 = false;
#endif

STATIC_ASSERT(has_cxx20 == has_member_size_type<allocator<void>>);
STATIC_ASSERT(has_cxx20 == has_member_difference_type<allocator<void>>);
STATIC_ASSERT(has_cxx20 == has_member_pocma<allocator<void>>);
STATIC_ASSERT(has_cxx20 == has_member_is_always_equal<allocator<void>>);
STATIC_ASSERT(has_cxx20 == can_allocate<allocator<void>>);
STATIC_ASSERT(has_cxx20 == is_convertible_v<allocator<int>, allocator<void>>);

int main() {} // COMPILE-ONLY
