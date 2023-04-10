// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <memory>
#include <type_traits>
#include <utility>

#if _HAS_CXX17
#include <any>
#include <initializer_list>
#include <optional>
#endif // _HAS_CXX17

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

using namespace std;

// tests for shared_ptr<T>::operator=
template <class T, class U, class = void>
constexpr bool can_shared_ptr_assign = false;

template <class T, class U>
constexpr bool can_shared_ptr_assign<T, U, void_t<decltype(declval<shared_ptr<T>&>() = declval<U>())>> = true;

STATIC_ASSERT(can_shared_ptr_assign<int, shared_ptr<int>>);
STATIC_ASSERT(!can_shared_ptr_assign<int, shared_ptr<long>>);
STATIC_ASSERT(!can_shared_ptr_assign<int, const shared_ptr<long>&>);
STATIC_ASSERT(!can_shared_ptr_assign<int, unique_ptr<long>>);
#if _HAS_AUTO_PTR_ETC
STATIC_ASSERT(!can_shared_ptr_assign<int, auto_ptr<long>>);
#endif

// tests for shared_ptr<T>::reset
template <class Void, class T, class... Us>
constexpr bool can_shared_ptr_reset_impl = false;

template <class T, class... Us>
constexpr bool
    can_shared_ptr_reset_impl<void_t<decltype(declval<shared_ptr<T>&>().reset(declval<Us>()...))>, T, Us...> = true;

template <class T, class... Us>
constexpr bool can_shared_ptr_reset = can_shared_ptr_reset_impl<void, T, Us...>;

STATIC_ASSERT(can_shared_ptr_reset<int, int*>);
STATIC_ASSERT(!can_shared_ptr_reset<int, long*>);
STATIC_ASSERT(!can_shared_ptr_reset<int, long*, default_delete<long>>);
STATIC_ASSERT(!can_shared_ptr_reset<int, long*, default_delete<long>, allocator<long>>);

// tests for weak_ptr<T>::operator=
template <class T, class U, class = void>
constexpr bool can_weak_ptr_assign = false;

template <class T, class U>
constexpr bool can_weak_ptr_assign<T, U, void_t<decltype(declval<weak_ptr<T>&>() = declval<U>())>> = true;

STATIC_ASSERT(can_weak_ptr_assign<int, weak_ptr<int>>);
STATIC_ASSERT(!can_weak_ptr_assign<int, weak_ptr<long>>);
STATIC_ASSERT(!can_weak_ptr_assign<int, const weak_ptr<long>&>);
STATIC_ASSERT(!can_weak_ptr_assign<int, const shared_ptr<long>&>);

#if _HAS_CXX17
// tests for make_optional
template <class T, class = void>
constexpr bool can_make_optional_decay = false;

template <class T>
constexpr bool can_make_optional_decay<T, void_t<decltype(make_optional(declval<T>()))>> = true;

template <class Void, class T, class... Us>
constexpr bool can_make_optional_impl = false;

template <class T, class... Us>
constexpr bool can_make_optional_impl<void_t<decltype(make_optional<T>(declval<Us>()...))>, T, Us...> = true;

template <class T, class... Us>
constexpr bool can_make_optional_usual = can_make_optional_impl<void, T, Us...>;

STATIC_ASSERT(can_make_optional_decay<unique_ptr<int>>);
STATIC_ASSERT(!can_make_optional_decay<const unique_ptr<int>&>); // LWG-3627
STATIC_ASSERT(can_make_optional_usual<int, int>);
STATIC_ASSERT(!can_make_optional_usual<int, int, int>);
STATIC_ASSERT(!can_make_optional_usual<int, initializer_list<int>&>);

// tests for make_any
template <class Void, class T, class... Us>
constexpr bool can_make_any_impl = false;

template <class T, class... Us>
constexpr bool can_make_any_impl<void_t<decltype(make_any<T>(declval<Us>()...))>, T, Us...> = true;

template <class T, class... Us>
constexpr bool can_make_any = can_make_any_impl<void, T, Us...>;

STATIC_ASSERT(can_make_any<int, int>);
STATIC_ASSERT(!can_make_any<unique_ptr<int>, const unique_ptr<int>&>);
STATIC_ASSERT(!can_make_any<int, int, int>);
STATIC_ASSERT(!can_make_any<int, initializer_list<int>&>);
#endif // _HAS_CXX17
