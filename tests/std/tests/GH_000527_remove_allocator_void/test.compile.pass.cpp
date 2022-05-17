// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING

#include <memory>
#include <type_traits>

#define STATIC_ASSERT(E) static_assert(E, #E)

using namespace std;

template <class T, class = void>
inline constexpr bool has_member_size_type = false;

template <class T>
inline constexpr bool has_member_size_type<T, void_t<typename T::size_type>> = true;

template <class T, class = void>
inline constexpr bool has_member_difference_type = false;

template <class T>
inline constexpr bool has_member_difference_type<T, void_t<typename T::difference_type>> = true;

template <class T, class = void>
inline constexpr bool has_member_pocma = false;

template <class T>
inline constexpr bool has_member_pocma<T, void_t<typename T::propagate_on_container_move_assignment>> = true;

template <class T, class = void>
inline constexpr bool has_member_is_always_equal = false;

template <class T>
inline constexpr bool has_member_is_always_equal<T, void_t<typename T::is_always_equal>> = true;

template <class T, class = void>
inline constexpr bool can_allocate = false;

template <class T>
inline constexpr bool can_allocate<T, void_t<decltype(declval<T&>().allocate(size_t{}))>> = true;

STATIC_ASSERT(has_member_size_type<allocator<int>>);
STATIC_ASSERT(has_member_difference_type<allocator<int>>);
STATIC_ASSERT(has_member_pocma<allocator<int>>);
STATIC_ASSERT(has_member_is_always_equal<allocator<int>>);
STATIC_ASSERT(can_allocate<allocator<int>>);
STATIC_ASSERT((is_convertible_v<allocator<void>, allocator<int>>));

#if _HAS_CXX20
STATIC_ASSERT(has_member_size_type<allocator<void>>);
STATIC_ASSERT(has_member_difference_type<allocator<void>>);
STATIC_ASSERT(has_member_pocma<allocator<void>>);
STATIC_ASSERT(has_member_is_always_equal<allocator<void>>);
STATIC_ASSERT(can_allocate<allocator<void>>);
STATIC_ASSERT((is_convertible_v<allocator<int>, allocator<void>>));
#else
STATIC_ASSERT(!has_member_size_type<allocator<void>>);
STATIC_ASSERT(!has_member_difference_type<allocator<void>>);
STATIC_ASSERT(!has_member_pocma<allocator<void>>);
STATIC_ASSERT(!has_member_is_always_equal<allocator<void>>);
STATIC_ASSERT(!can_allocate<allocator<void>>);
STATIC_ASSERT(!(is_convertible_v<allocator<int>, allocator<void>>));
#endif

int main(){} // COMPILE-ONLY
