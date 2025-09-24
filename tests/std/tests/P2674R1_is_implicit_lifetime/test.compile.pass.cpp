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
template <bool Val, typename T>
constexpr bool test_implicit_lifetime_cv = test_implicit_lifetime<Val, T> //
                                        && test_implicit_lifetime<Val, const T> //
                                        && test_implicit_lifetime<Val, volatile T> //
                                        && test_implicit_lifetime<Val, const volatile T>;

// Basics (arrays thereof included in Arrays section)
static_assert(test_implicit_lifetime_cv<true, int>);
static_assert(test_implicit_lifetime_cv<true, int*>);
static_assert(test_implicit_lifetime_cv<true, nullptr_t>);
static_assert(test_implicit_lifetime_cv<true, UnscopedEnum>);
static_assert(test_implicit_lifetime_cv<true, ScopedEnum>);
static_assert(test_implicit_lifetime_cv<true, void (*)()>);
static_assert(test_implicit_lifetime_cv<true, TrivialClass>);
static_assert(test_implicit_lifetime_cv<false, UserProvidedDestructorClass>);

// Arrays
static_assert(test_implicit_lifetime_cv<true, int[]>);
static_assert(test_implicit_lifetime_cv<true, int[2]>);
static_assert(test_implicit_lifetime_cv<true, TrivialClass[10]>);
static_assert(test_implicit_lifetime_cv<true, UserProvidedDestructorClass[12]>);
static_assert(test_implicit_lifetime_cv<true, IncompleteClass[]>);
static_assert(test_implicit_lifetime_cv<true, IncompleteClass[20]>);

static_assert(test_implicit_lifetime_cv<false, void>);
static_assert(test_implicit_lifetime<false, long&>);
static_assert(test_implicit_lifetime<false, long&&>);
#endif // ^^^ defined(__cpp_lib_is_implicit_lifetime) ^^^
