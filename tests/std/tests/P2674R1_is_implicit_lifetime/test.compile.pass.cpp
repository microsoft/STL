// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cstddef>
#include <type_traits>
using namespace std;

enum UnscopedEnum { Enumerator };
enum class ScopedEnum { Enumerator2 };
class IncompleteClass;
struct TrivialClass {};
struct UserProvidedDestructorClass {
    ~UserProvidedDestructorClass() {}
};

#ifdef __cpp_lib_is_implicit_lifetime
template <bool Val, typename T>
constexpr bool test_implicit_lifetime = is_implicit_lifetime_v<T> == Val && is_implicit_lifetime<T>::value == Val;

// Basics (arrays thereof included in Arrays section)
static_assert(test_implicit_lifetime<true, int>);
static_assert(test_implicit_lifetime<true, int*>);
static_assert(test_implicit_lifetime<true, nullptr_t>);
static_assert(test_implicit_lifetime<true, UnscopedEnum>);
static_assert(test_implicit_lifetime<true, ScopedEnum>);
static_assert(test_implicit_lifetime<true, void (*)()>);
static_assert(test_implicit_lifetime<true, TrivialClass>);
static_assert(test_implicit_lifetime<false, UserProvidedDestructorClass>);

// Arrays
static_assert(test_implicit_lifetime<true, int[]>);
static_assert(test_implicit_lifetime<true, int[2]>);
static_assert(test_implicit_lifetime<true, TrivialClass[10]>);
static_assert(test_implicit_lifetime<true, UserProvidedDestructorClass[12]>);
static_assert(test_implicit_lifetime<true, IncompleteClass[]>);
static_assert(test_implicit_lifetime<true, IncompleteClass[20]>);

static_assert(test_implicit_lifetime<false, void>);
static_assert(test_implicit_lifetime<false, const volatile void>);
static_assert(test_implicit_lifetime<false, long&>);
static_assert(test_implicit_lifetime<false, long&&>);
#endif // ^^^ defined(__cpp_lib_is_implicit_lifetime) ^^^
