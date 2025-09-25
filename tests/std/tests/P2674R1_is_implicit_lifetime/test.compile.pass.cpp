// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cstddef>
#include <string>
#include <type_traits>
using namespace std;

enum UnscopedEnum { Enumerator };
enum class ScopedEnum { Enumerator2 };
class IncompleteClass;
struct TrivialClass {};
struct DefaultedDestructorClass {
    ~DefaultedDestructorClass() = default;
};
struct DeletedDestructorClass {
    ~DeletedDestructorClass() = delete;
};
struct UserProvidedDestructorClass {
    ~UserProvidedDestructorClass() {}
};
struct StringAggregateWithImplicitlyDeclaredDestructor {
    string juliette_andromeda_meow;
    string clarissa_melpomene_meow;
};
struct StringAggregateWithUserProvidedDestructor {
    string chaos_engine_one;
    string chaos_engine_two;
    ~StringAggregateWithUserProvidedDestructor() {}
};

class NonAggregateWithTrivialCtorAndTrivialDtor {
public:
    NonAggregateWithTrivialCtorAndTrivialDtor()  = default;
    ~NonAggregateWithTrivialCtorAndTrivialDtor() = default;

    NonAggregateWithTrivialCtorAndTrivialDtor(const NonAggregateWithTrivialCtorAndTrivialDtor&)            = delete;
    NonAggregateWithTrivialCtorAndTrivialDtor& operator=(const NonAggregateWithTrivialCtorAndTrivialDtor&) = delete;

    void set_num(int x) {
        num = x;
    }
    int get_num() const {
        return num;
    }

private:
    int num;
};

class NonAggregateWithNonTrivialCtor {
public:
    NonAggregateWithNonTrivialCtor()  = default;
    ~NonAggregateWithNonTrivialCtor() = default;

    NonAggregateWithNonTrivialCtor(const NonAggregateWithNonTrivialCtor&)            = delete;
    NonAggregateWithNonTrivialCtor& operator=(const NonAggregateWithNonTrivialCtor&) = delete;

    void set_num(int x) {
        num = x;
    }
    int get_num() const {
        return num;
    }

private:
    int num{0}; // default member initializer => non-trivial default constructor, N5014 [class.default.ctor]/3.2
};

class NonAggregateWithUserProvidedCtor {
public:
    NonAggregateWithUserProvidedCtor() {}
    ~NonAggregateWithUserProvidedCtor() = default;

    NonAggregateWithUserProvidedCtor(const NonAggregateWithUserProvidedCtor&)            = delete;
    NonAggregateWithUserProvidedCtor& operator=(const NonAggregateWithUserProvidedCtor&) = delete;

    void set_num(int x) {
        num = x;
    }
    int get_num() const {
        return num;
    }

private:
    int num;
};

class NonAggregateWithDeletedDtor {
public:
    NonAggregateWithDeletedDtor()  = default;
    ~NonAggregateWithDeletedDtor() = delete;

    NonAggregateWithDeletedDtor(const NonAggregateWithDeletedDtor&)            = delete;
    NonAggregateWithDeletedDtor& operator=(const NonAggregateWithDeletedDtor&) = delete;

    void set_num(int x) {
        num = x;
    }
    int get_num() const {
        return num;
    }

private:
    int num;
};

class NonAggregateWithUserProvidedDtor {
public:
    NonAggregateWithUserProvidedDtor() = default;
    ~NonAggregateWithUserProvidedDtor() {}

    NonAggregateWithUserProvidedDtor(const NonAggregateWithUserProvidedDtor&)            = delete;
    NonAggregateWithUserProvidedDtor& operator=(const NonAggregateWithUserProvidedDtor&) = delete;

    void set_num(int x) {
        num = x;
    }
    int get_num() const {
        return num;
    }

private:
    int num;
};

#ifdef __cpp_lib_is_implicit_lifetime
template <bool Val, typename T>
constexpr bool test_implicit_lifetime = is_implicit_lifetime_v<T> == Val && is_implicit_lifetime<T>::value == Val;
template <bool Val, typename T>
constexpr bool test_implicit_lifetime_cv = test_implicit_lifetime<Val, T> //
                                        && test_implicit_lifetime<Val, const T> //
                                        && test_implicit_lifetime<Val, volatile T> //
                                        && test_implicit_lifetime<Val, const volatile T>;

// Scalar types, N5014 [basic.types.general]/9
static_assert(test_implicit_lifetime_cv<true, int>);
static_assert(test_implicit_lifetime_cv<true, UnscopedEnum>);
static_assert(test_implicit_lifetime_cv<true, ScopedEnum>);
static_assert(test_implicit_lifetime_cv<true, void*>);
static_assert(test_implicit_lifetime_cv<true, int*>);
static_assert(test_implicit_lifetime_cv<true, void (*)()>);
static_assert(test_implicit_lifetime_cv<true, int TrivialClass::*>);
static_assert(test_implicit_lifetime_cv<true, int (TrivialClass::*)(int)>);
static_assert(test_implicit_lifetime_cv<true, nullptr_t>);

// Implicit-lifetime class types, N5014 [class.prop]/16:
// "A class S is an implicit-lifetime class if
// - it is an aggregate whose destructor is not user-provided or
// - it has at least one trivial eligible constructor and a trivial, non-deleted destructor."
static_assert(test_implicit_lifetime_cv<true, TrivialClass>);
static_assert(test_implicit_lifetime_cv<true, DefaultedDestructorClass>);
static_assert(test_implicit_lifetime_cv<true, DeletedDestructorClass>);
static_assert(test_implicit_lifetime_cv<false, UserProvidedDestructorClass>);
static_assert(test_implicit_lifetime_cv<true, StringAggregateWithImplicitlyDeclaredDestructor>);
static_assert(test_implicit_lifetime_cv<false, StringAggregateWithUserProvidedDestructor>);
static_assert(test_implicit_lifetime_cv<true, NonAggregateWithTrivialCtorAndTrivialDtor>);
#ifndef __clang__ // TRANSITION, LLVM-160610
static_assert(test_implicit_lifetime_cv<false, NonAggregateWithNonTrivialCtor>);
static_assert(test_implicit_lifetime_cv<false, NonAggregateWithUserProvidedCtor>);
#endif // ^^^ no workaround ^^^
static_assert(test_implicit_lifetime_cv<false, NonAggregateWithDeletedDtor>);
static_assert(test_implicit_lifetime_cv<false, NonAggregateWithUserProvidedDtor>);

// Arrays
static_assert(test_implicit_lifetime_cv<true, int[]>);
static_assert(test_implicit_lifetime_cv<true, int[2]>);
static_assert(test_implicit_lifetime_cv<true, TrivialClass[]>);
static_assert(test_implicit_lifetime_cv<true, TrivialClass[10]>);
static_assert(test_implicit_lifetime_cv<true, UserProvidedDestructorClass[]>);
static_assert(test_implicit_lifetime_cv<true, UserProvidedDestructorClass[12]>);
static_assert(test_implicit_lifetime_cv<true, IncompleteClass[]>);
static_assert(test_implicit_lifetime_cv<true, IncompleteClass[20]>);

static_assert(test_implicit_lifetime_cv<false, void>);
static_assert(test_implicit_lifetime<false, long&>);
static_assert(test_implicit_lifetime<false, long&&>);
static_assert(test_implicit_lifetime<false, const long&>);
static_assert(test_implicit_lifetime<false, const long&&>);
#endif // ^^^ defined(__cpp_lib_is_implicit_lifetime) ^^^
