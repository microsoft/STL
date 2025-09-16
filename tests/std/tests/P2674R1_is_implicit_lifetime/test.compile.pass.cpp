// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <type_traits>

enum UnscopedEnum { Enumerator };
enum struct ScopedEnum { Enumerator };
class IncompleteClass;
struct TrivialClass {};
struct AggregateClassWithUserProvidedDestructor {
    ~AggregateClassWithUserProvidedDestructor() {}
};

using namespace std;

#ifdef __cpp_lib_is_implicit_lifetime
// this is a test to test whether
// is_implicit_lifetime_v produces desired results
// compiles under std namespace

template <typename T>
constexpr bool test_implicit_lifetime = is_implicit_lifetime_v<T> && is_implicit_lifetime<T>::value;

// Basics (arrays thereof included in Arrays section)
static_assert(test_implicit_lifetime<int>);
static_assert(test_implicit_lifetime<int*>);
static_assert(test_implicit_lifetime<nullptr_t>);
static_assert(test_implicit_lifetime<UnscopedEnum>);
static_assert(test_implicit_lifetime<ScopedEnum>);
static_assert(!test_implicit_lifetime<void (*)()>);
static_assert(test_implicit_lifetime<TrivialClass>);
static_assert(!test_implicit_lifetime<UserProvidedDestructorClass>);

// Arrays
static_assert(test_implicit_lifetime<int[]>);
static_assert(test_implicit_lifetime<int[2]>);
static_assert(test_implicit_lifetime<TrivialClass[10]>);
static_assert(test_implicit_lifetime<UserProvidedDestructorClass[12]>);
static_assert(test_implicit_lifetime<IncompleteClass[]>);
static_assert(test_implicit_lifetime<IncompleteClass[20]>);

static_assert(!test_implicit_lifetime<void>);
static_assert(!test_implicit_lifetime<const volatile void>);
static_assert(!test_implicit_lifetime<long&>);
static_assert(!test_implicit_lifetime<long&&>);
#endif
