// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cstddef>
#include <memory>
#include <type_traits>

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__);

template <class, class = void>
constexpr bool has_memtype_element_type = false;

template <class T>
constexpr bool has_memtype_element_type<T, void_t<typename T::element_type>> = true;

template <class, class = void>
constexpr bool has_memtype_pointer = false;

template <class T>
constexpr bool has_memtype_pointer<T, void_t<typename T::pointer>> = true;

template <class, class = void>
constexpr bool has_memtype_difference_type = false;

template <class T>
constexpr bool has_memtype_difference_type<T, void_t<typename T::difference_type>> = true;

STATIC_ASSERT(!has_memtype_element_type<pointer_traits<int>>);
STATIC_ASSERT(!has_memtype_pointer<pointer_traits<int>>);
STATIC_ASSERT(!has_memtype_difference_type<pointer_traits<int>>);

struct LackElementType {
    using pointer         = int;
    using difference_type = int;
    template <class>
    using rebind = int;
};

STATIC_ASSERT(!has_memtype_element_type<pointer_traits<LackElementType>>);
STATIC_ASSERT(!has_memtype_pointer<pointer_traits<LackElementType>>);
STATIC_ASSERT(!has_memtype_difference_type<pointer_traits<LackElementType>>);

struct OnlyElementType {
    using element_type = void;
};

STATIC_ASSERT(has_memtype_element_type<pointer_traits<OnlyElementType>>);
STATIC_ASSERT(has_memtype_pointer<pointer_traits<OnlyElementType>>);
STATIC_ASSERT(has_memtype_difference_type<pointer_traits<OnlyElementType>>);

STATIC_ASSERT(is_same_v<pointer_traits<OnlyElementType>::element_type, void>);
STATIC_ASSERT(is_same_v<pointer_traits<OnlyElementType>::pointer, OnlyElementType>);
STATIC_ASSERT(is_same_v<pointer_traits<OnlyElementType>::difference_type, ptrdiff_t>);

template <class I>
struct Templated {
    using difference_type = I;
};

STATIC_ASSERT(has_memtype_element_type<pointer_traits<Templated<char>>>);
STATIC_ASSERT(has_memtype_pointer<pointer_traits<Templated<char>>>);
STATIC_ASSERT(has_memtype_difference_type<pointer_traits<Templated<char>>>);

STATIC_ASSERT(is_same_v<pointer_traits<Templated<char>>::element_type, char>);
STATIC_ASSERT(is_same_v<pointer_traits<Templated<char>>::pointer, Templated<char>>);
STATIC_ASSERT(is_same_v<pointer_traits<Templated<char>>::difference_type, char>);

template <class I, size_t N = sizeof(I)>
struct BadTemplated {
    using difference_type = I;
};

STATIC_ASSERT(!has_memtype_element_type<pointer_traits<BadTemplated<int>>>);
STATIC_ASSERT(!has_memtype_pointer<pointer_traits<BadTemplated<int>>>);
STATIC_ASSERT(!has_memtype_difference_type<pointer_traits<BadTemplated<int>>>);

template <class T>
struct CheckPriority {
    using element_type = T[42];
};

STATIC_ASSERT(has_memtype_element_type<pointer_traits<CheckPriority<char>>>);
STATIC_ASSERT(has_memtype_pointer<pointer_traits<CheckPriority<char>>>);
STATIC_ASSERT(has_memtype_difference_type<pointer_traits<CheckPriority<char>>>);

STATIC_ASSERT(is_same_v<pointer_traits<CheckPriority<char>>::element_type, char[42]>);
STATIC_ASSERT(is_same_v<pointer_traits<CheckPriority<char>>::pointer, CheckPriority<char>>);
STATIC_ASSERT(is_same_v<pointer_traits<CheckPriority<char>>::difference_type, ptrdiff_t>);
