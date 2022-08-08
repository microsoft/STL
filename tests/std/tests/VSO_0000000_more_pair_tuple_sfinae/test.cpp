// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <memory>
#ifndef _M_CEE
#include <mutex>
#endif // _M_CEE
#include <new>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

// LWG-2367 "pair and tuple are not correctly implemented for is_constructible with no args"

struct NoDefault {
    NoDefault(int, int) {}
};

STATIC_ASSERT(is_default_constructible_v<int>);
STATIC_ASSERT(!is_default_constructible_v<NoDefault>);

STATIC_ASSERT(is_default_constructible_v<pair<int, int>>);
STATIC_ASSERT(!is_default_constructible_v<pair<int, NoDefault>>);
STATIC_ASSERT(!is_default_constructible_v<pair<NoDefault, int>>);
STATIC_ASSERT(!is_default_constructible_v<pair<NoDefault, NoDefault>>);

STATIC_ASSERT(is_default_constructible_v<tuple<int, int>>);
STATIC_ASSERT(!is_default_constructible_v<tuple<int, NoDefault>>);
STATIC_ASSERT(!is_default_constructible_v<tuple<NoDefault, int>>);
STATIC_ASSERT(!is_default_constructible_v<tuple<NoDefault, NoDefault>>);

STATIC_ASSERT(is_constructible_v<tuple<int, int>, allocator_arg_t, allocator<int>>);
STATIC_ASSERT(!is_constructible_v<tuple<int, NoDefault>, allocator_arg_t, allocator<int>>);
STATIC_ASSERT(!is_constructible_v<tuple<NoDefault, int>, allocator_arg_t, allocator<int>>);
STATIC_ASSERT(!is_constructible_v<tuple<NoDefault, NoDefault>, allocator_arg_t, allocator<int>>);


// LWG-2510 "Tag types should not be DefaultConstructible"
template <typename T, typename = void>
struct IsImplicitlyDefaultConstructible : false_type {};

template <typename T>
void ImplicitlyDefaultConstruct(const T&);

template <typename T>
struct IsImplicitlyDefaultConstructible<T, void_t<decltype(ImplicitlyDefaultConstruct<T>({}))>> : true_type {};

struct ExplicitDefault {
    explicit ExplicitDefault() = default;
};

template <typename T>
constexpr bool OrdinaryDC = is_default_constructible_v<T>&& IsImplicitlyDefaultConstructible<T>::value;

template <typename T>
constexpr bool ExplicitDC = is_default_constructible_v<T> && !IsImplicitlyDefaultConstructible<T>::value;

template <typename T>
constexpr bool VerbotenDC = !is_default_constructible_v<T> && !IsImplicitlyDefaultConstructible<T>::value;

STATIC_ASSERT(OrdinaryDC<int>);
STATIC_ASSERT(VerbotenDC<NoDefault>);
STATIC_ASSERT(ExplicitDC<ExplicitDefault>);
STATIC_ASSERT(ExplicitDC<nothrow_t>);
STATIC_ASSERT(ExplicitDC<piecewise_construct_t>);
STATIC_ASSERT(ExplicitDC<allocator_arg_t>);
#ifndef _M_CEE
STATIC_ASSERT(ExplicitDC<defer_lock_t>);
STATIC_ASSERT(ExplicitDC<try_to_lock_t>);
STATIC_ASSERT(ExplicitDC<adopt_lock_t>);
#endif // _M_CEE

using Expl = ExplicitDefault;
using NOPE = NoDefault;

STATIC_ASSERT(OrdinaryDC<pair<long, long>>);
STATIC_ASSERT(ExplicitDC<pair<long, Expl>>);
STATIC_ASSERT(VerbotenDC<pair<long, NOPE>>);
STATIC_ASSERT(ExplicitDC<pair<Expl, long>>);
STATIC_ASSERT(ExplicitDC<pair<Expl, Expl>>);
STATIC_ASSERT(VerbotenDC<pair<Expl, NOPE>>);
STATIC_ASSERT(VerbotenDC<pair<NOPE, long>>);
STATIC_ASSERT(VerbotenDC<pair<NOPE, Expl>>);
STATIC_ASSERT(VerbotenDC<pair<NOPE, NOPE>>);

STATIC_ASSERT(OrdinaryDC<tuple<long, long>>);
STATIC_ASSERT(ExplicitDC<tuple<long, Expl>>);
STATIC_ASSERT(VerbotenDC<tuple<long, NOPE>>);
STATIC_ASSERT(ExplicitDC<tuple<Expl, long>>);
STATIC_ASSERT(ExplicitDC<tuple<Expl, Expl>>);
STATIC_ASSERT(VerbotenDC<tuple<Expl, NOPE>>);
STATIC_ASSERT(VerbotenDC<tuple<NOPE, long>>);
STATIC_ASSERT(VerbotenDC<tuple<NOPE, Expl>>);
STATIC_ASSERT(VerbotenDC<tuple<NOPE, NOPE>>);
STATIC_ASSERT(OrdinaryDC<tuple<long, long, long, long, long, long>>);
STATIC_ASSERT(ExplicitDC<tuple<long, long, long, long, Expl, long>>);
STATIC_ASSERT(VerbotenDC<tuple<long, long, long, long, NOPE, long>>);


// LWG-2729 "Missing SFINAE on std::pair::operator="
// LWG-2958 "Moves improperly defined as deleted"
template <typename T, typename = void>
struct IsBraceAssignable : false_type {};

template <typename T>
struct IsBraceAssignable<T, void_t<decltype(declval<T&>() = {})>> : true_type {};

template <typename T>
constexpr bool IsBraceAssignableV = IsBraceAssignable<T>::value;

struct Copyable {
    Copyable()                             = default;
    Copyable(const ::Copyable&)            = default;
    Copyable& operator=(const ::Copyable&) = default;
};

struct Movable {
    Movable()                       = default;
    Movable(::Movable&&)            = default;
    Movable& operator=(::Movable&&) = default;
};

struct NonCopyable {
    NonCopyable()                              = default;
    NonCopyable(const NonCopyable&)            = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;
};

struct Other {};

struct CopiesOther {
    CopiesOther& operator=(const Other&) {
        return *this;
    }
};

struct MovesOther {
    MovesOther& operator=(Other&&) {
        return *this;
    }
};

struct RejectsOther {
    RejectsOther& operator=(const Other&) = delete;
};

struct WeirdlyTakesOther {
    WeirdlyTakesOther& operator=(const Other&) && {
        return *this;
    }
};

STATIC_ASSERT(is_copy_assignable_v<::Copyable>);
STATIC_ASSERT(!is_copy_assignable_v<::Movable>);
STATIC_ASSERT(!is_copy_assignable_v<NonCopyable>);

STATIC_ASSERT(is_move_assignable_v<::Copyable>);
STATIC_ASSERT(is_move_assignable_v<::Movable>);
STATIC_ASSERT(!is_move_assignable_v<NonCopyable>);

STATIC_ASSERT(IsBraceAssignableV<::Copyable>);
STATIC_ASSERT(IsBraceAssignableV<::Movable>);
STATIC_ASSERT(!IsBraceAssignableV<NonCopyable>);

STATIC_ASSERT(is_assignable_v<CopiesOther&, const Other&>);
STATIC_ASSERT(!is_assignable_v<MovesOther&, const Other&>);
STATIC_ASSERT(!is_assignable_v<RejectsOther&, const Other&>);
STATIC_ASSERT(!is_assignable_v<WeirdlyTakesOther&, const Other&>);
STATIC_ASSERT(is_assignable_v<WeirdlyTakesOther, const Other&>);

STATIC_ASSERT(is_assignable_v<CopiesOther&, Other>);
STATIC_ASSERT(is_assignable_v<MovesOther&, Other>);
STATIC_ASSERT(!is_assignable_v<RejectsOther&, Other>);
STATIC_ASSERT(!is_assignable_v<WeirdlyTakesOther&, Other>);
STATIC_ASSERT(is_assignable_v<WeirdlyTakesOther, Other>);

STATIC_ASSERT(is_copy_assignable_v<pair<::Copyable, ::Copyable>>);
STATIC_ASSERT(!is_copy_assignable_v<pair<::Copyable, ::Movable>>);
STATIC_ASSERT(!is_copy_assignable_v<pair<::Copyable, NonCopyable>>);
STATIC_ASSERT(!is_copy_assignable_v<pair<::Movable, ::Copyable>>);
STATIC_ASSERT(!is_copy_assignable_v<pair<::Movable, ::Movable>>);
STATIC_ASSERT(!is_copy_assignable_v<pair<::Movable, NonCopyable>>);
STATIC_ASSERT(!is_copy_assignable_v<pair<NonCopyable, ::Copyable>>);
STATIC_ASSERT(!is_copy_assignable_v<pair<NonCopyable, ::Movable>>);
STATIC_ASSERT(!is_copy_assignable_v<pair<NonCopyable, NonCopyable>>);

STATIC_ASSERT(is_move_assignable_v<pair<::Copyable, ::Copyable>>);
STATIC_ASSERT(is_move_assignable_v<pair<::Copyable, ::Movable>>);
STATIC_ASSERT(!is_move_assignable_v<pair<::Copyable, NonCopyable>>);
STATIC_ASSERT(is_move_assignable_v<pair<::Movable, ::Copyable>>);
STATIC_ASSERT(is_move_assignable_v<pair<::Movable, ::Movable>>);
STATIC_ASSERT(!is_move_assignable_v<pair<::Movable, NonCopyable>>);
STATIC_ASSERT(!is_move_assignable_v<pair<NonCopyable, ::Copyable>>);
STATIC_ASSERT(!is_move_assignable_v<pair<NonCopyable, ::Movable>>);
STATIC_ASSERT(!is_move_assignable_v<pair<NonCopyable, NonCopyable>>);

STATIC_ASSERT(IsBraceAssignableV<pair<::Copyable, ::Copyable>>);
STATIC_ASSERT(IsBraceAssignableV<pair<::Copyable, ::Movable>>);
STATIC_ASSERT(!IsBraceAssignableV<pair<::Copyable, NonCopyable>>);
STATIC_ASSERT(IsBraceAssignableV<pair<::Movable, ::Copyable>>);
STATIC_ASSERT(IsBraceAssignableV<pair<::Movable, ::Movable>>);
STATIC_ASSERT(!IsBraceAssignableV<pair<::Movable, NonCopyable>>);
STATIC_ASSERT(!IsBraceAssignableV<pair<NonCopyable, ::Copyable>>);
STATIC_ASSERT(!IsBraceAssignableV<pair<NonCopyable, ::Movable>>);
STATIC_ASSERT(!IsBraceAssignableV<pair<NonCopyable, NonCopyable>>);

STATIC_ASSERT(is_assignable_v<pair<CopiesOther, int>&, const pair<Other, int>&>);
STATIC_ASSERT(!is_assignable_v<pair<MovesOther, int>&, const pair<Other, int>&>);
STATIC_ASSERT(!is_assignable_v<pair<RejectsOther, int>&, const pair<Other, int>&>);
STATIC_ASSERT(!is_assignable_v<pair<WeirdlyTakesOther, int>&, const pair<Other, int>&>);

STATIC_ASSERT(is_assignable_v<pair<int, CopiesOther>&, const pair<int, Other>&>);
STATIC_ASSERT(!is_assignable_v<pair<int, MovesOther>&, const pair<int, Other>&>);
STATIC_ASSERT(!is_assignable_v<pair<int, RejectsOther>&, const pair<int, Other>&>);
STATIC_ASSERT(!is_assignable_v<pair<int, WeirdlyTakesOther>&, const pair<int, Other>&>);

STATIC_ASSERT(is_assignable_v<pair<CopiesOther, int>&, pair<Other, int>>);
STATIC_ASSERT(is_assignable_v<pair<MovesOther, int>&, pair<Other, int>>);
STATIC_ASSERT(!is_assignable_v<pair<RejectsOther, int>&, pair<Other, int>>);
STATIC_ASSERT(!is_assignable_v<pair<WeirdlyTakesOther, int>&, pair<Other, int>>);

STATIC_ASSERT(is_assignable_v<pair<int, CopiesOther>&, pair<int, Other>>);
STATIC_ASSERT(is_assignable_v<pair<int, MovesOther>&, pair<int, Other>>);
STATIC_ASSERT(!is_assignable_v<pair<int, RejectsOther>&, pair<int, Other>>);
STATIC_ASSERT(!is_assignable_v<pair<int, WeirdlyTakesOther>&, pair<int, Other>>);

STATIC_ASSERT(is_copy_assignable_v<tuple<::Copyable, ::Copyable>>);
STATIC_ASSERT(!is_copy_assignable_v<tuple<::Copyable, ::Movable>>);
STATIC_ASSERT(!is_copy_assignable_v<tuple<::Copyable, NonCopyable>>);
STATIC_ASSERT(!is_copy_assignable_v<tuple<::Movable, ::Copyable>>);
STATIC_ASSERT(!is_copy_assignable_v<tuple<::Movable, ::Movable>>);
STATIC_ASSERT(!is_copy_assignable_v<tuple<::Movable, NonCopyable>>);
STATIC_ASSERT(!is_copy_assignable_v<tuple<NonCopyable, ::Copyable>>);
STATIC_ASSERT(!is_copy_assignable_v<tuple<NonCopyable, ::Movable>>);
STATIC_ASSERT(!is_copy_assignable_v<tuple<NonCopyable, NonCopyable>>);

STATIC_ASSERT(is_move_assignable_v<tuple<::Copyable, ::Copyable>>);
STATIC_ASSERT(is_move_assignable_v<tuple<::Copyable, ::Movable>>);
STATIC_ASSERT(!is_move_assignable_v<tuple<::Copyable, NonCopyable>>);
STATIC_ASSERT(is_move_assignable_v<tuple<::Movable, ::Copyable>>);
STATIC_ASSERT(is_move_assignable_v<tuple<::Movable, ::Movable>>);
STATIC_ASSERT(!is_move_assignable_v<tuple<::Movable, NonCopyable>>);
STATIC_ASSERT(!is_move_assignable_v<tuple<NonCopyable, ::Copyable>>);
STATIC_ASSERT(!is_move_assignable_v<tuple<NonCopyable, ::Movable>>);
STATIC_ASSERT(!is_move_assignable_v<tuple<NonCopyable, NonCopyable>>);

STATIC_ASSERT(IsBraceAssignableV<tuple<::Copyable, ::Copyable>>);
STATIC_ASSERT(IsBraceAssignableV<tuple<::Copyable, ::Movable>>);
STATIC_ASSERT(!IsBraceAssignableV<tuple<::Copyable, NonCopyable>>);
STATIC_ASSERT(IsBraceAssignableV<tuple<::Movable, ::Copyable>>);
STATIC_ASSERT(IsBraceAssignableV<tuple<::Movable, ::Movable>>);
STATIC_ASSERT(!IsBraceAssignableV<tuple<::Movable, NonCopyable>>);
STATIC_ASSERT(!IsBraceAssignableV<tuple<NonCopyable, ::Copyable>>);
STATIC_ASSERT(!IsBraceAssignableV<tuple<NonCopyable, ::Movable>>);
STATIC_ASSERT(!IsBraceAssignableV<tuple<NonCopyable, NonCopyable>>);

STATIC_ASSERT(is_assignable_v<tuple<CopiesOther, int>&, const tuple<Other, int>&>);
STATIC_ASSERT(!is_assignable_v<tuple<MovesOther, int>&, const tuple<Other, int>&>);
STATIC_ASSERT(!is_assignable_v<tuple<RejectsOther, int>&, const tuple<Other, int>&>);
STATIC_ASSERT(!is_assignable_v<tuple<WeirdlyTakesOther, int>&, const tuple<Other, int>&>);

STATIC_ASSERT(is_assignable_v<tuple<int, CopiesOther>&, const tuple<int, Other>&>);
STATIC_ASSERT(!is_assignable_v<tuple<int, MovesOther>&, const tuple<int, Other>&>);
STATIC_ASSERT(!is_assignable_v<tuple<int, RejectsOther>&, const tuple<int, Other>&>);
STATIC_ASSERT(!is_assignable_v<tuple<int, WeirdlyTakesOther>&, const tuple<int, Other>&>);

STATIC_ASSERT(is_assignable_v<tuple<CopiesOther, int>&, tuple<Other, int>>);
STATIC_ASSERT(is_assignable_v<tuple<MovesOther, int>&, tuple<Other, int>>);
STATIC_ASSERT(!is_assignable_v<tuple<RejectsOther, int>&, tuple<Other, int>>);
STATIC_ASSERT(!is_assignable_v<tuple<WeirdlyTakesOther, int>&, tuple<Other, int>>);

STATIC_ASSERT(is_assignable_v<tuple<int, CopiesOther>&, tuple<int, Other>>);
STATIC_ASSERT(is_assignable_v<tuple<int, MovesOther>&, tuple<int, Other>>);
STATIC_ASSERT(!is_assignable_v<tuple<int, RejectsOther>&, tuple<int, Other>>);
STATIC_ASSERT(!is_assignable_v<tuple<int, WeirdlyTakesOther>&, tuple<int, Other>>);

STATIC_ASSERT(is_assignable_v<tuple<CopiesOther, int>&, const pair<Other, int>&>);
STATIC_ASSERT(!is_assignable_v<tuple<MovesOther, int>&, const pair<Other, int>&>);
STATIC_ASSERT(!is_assignable_v<tuple<RejectsOther, int>&, const pair<Other, int>&>);
STATIC_ASSERT(!is_assignable_v<tuple<WeirdlyTakesOther, int>&, const pair<Other, int>&>);

STATIC_ASSERT(is_assignable_v<tuple<int, CopiesOther>&, const pair<int, Other>&>);
STATIC_ASSERT(!is_assignable_v<tuple<int, MovesOther>&, const pair<int, Other>&>);
STATIC_ASSERT(!is_assignable_v<tuple<int, RejectsOther>&, const pair<int, Other>&>);
STATIC_ASSERT(!is_assignable_v<tuple<int, WeirdlyTakesOther>&, const pair<int, Other>&>);

STATIC_ASSERT(is_assignable_v<tuple<CopiesOther, int>&, pair<Other, int>>);
STATIC_ASSERT(is_assignable_v<tuple<MovesOther, int>&, pair<Other, int>>);
STATIC_ASSERT(!is_assignable_v<tuple<RejectsOther, int>&, pair<Other, int>>);
STATIC_ASSERT(!is_assignable_v<tuple<WeirdlyTakesOther, int>&, pair<Other, int>>);

STATIC_ASSERT(is_assignable_v<tuple<int, CopiesOther>&, pair<int, Other>>);
STATIC_ASSERT(is_assignable_v<tuple<int, MovesOther>&, pair<int, Other>>);
STATIC_ASSERT(!is_assignable_v<tuple<int, RejectsOther>&, pair<int, Other>>);
STATIC_ASSERT(!is_assignable_v<tuple<int, WeirdlyTakesOther>&, pair<int, Other>>);


// Verify workaround for LWG-2729 being unimplementable with tuples of
// references to incomplete types. Without the workaround, MSVC diagnoses
// a violation of the is_copy_assignable and is_move_assignable preconditions
// when determining the deleted-ness of the class WebKit's special member
// functions.
struct Incomplete;
struct WebKit {
    tuple<Incomplete&> x;
};


// LWG-2796 "tuple should be a literal type"
STATIC_ASSERT(is_trivially_destructible_v<pair<int, int>>);
STATIC_ASSERT(!is_trivially_destructible_v<pair<int, string>>);
STATIC_ASSERT(!is_trivially_destructible_v<pair<string, int>>);
STATIC_ASSERT(!is_trivially_destructible_v<pair<string, string>>);

STATIC_ASSERT(is_trivially_destructible_v<tuple<>>);

STATIC_ASSERT(is_trivially_destructible_v<tuple<int>>);
STATIC_ASSERT(!is_trivially_destructible_v<tuple<string>>);

STATIC_ASSERT(is_trivially_destructible_v<tuple<int, int>>);
STATIC_ASSERT(!is_trivially_destructible_v<tuple<int, string>>);
STATIC_ASSERT(!is_trivially_destructible_v<tuple<string, int>>);
STATIC_ASSERT(!is_trivially_destructible_v<tuple<string, string>>);


// N4387 "Improving pair and tuple"

struct No {};
struct Im {};
struct Ex {};

struct A {
    A(Im) {}
    explicit A(Ex) {}
};

template <typename T, typename Arg>
using NotConstructible = bool_constant<!is_constructible_v<T, Arg> && !is_convertible_v<Arg, T>>;

template <typename T, typename Arg>
using ImplicitlyConstructible = bool_constant<is_constructible_v<T, Arg> && is_convertible_v<Arg, T>>;

template <typename T, typename Arg>
using ExplicitlyConstructible = bool_constant<is_constructible_v<T, Arg> && !is_convertible_v<Arg, T>>;

STATIC_ASSERT(NotConstructible<A, No>::value);
STATIC_ASSERT(ImplicitlyConstructible<A, Im>::value);
STATIC_ASSERT(ExplicitlyConstructible<A, Ex>::value);

using UP = unique_ptr<int>;

STATIC_ASSERT(is_constructible_v<pair<int, int>, const int&, const int&>);
STATIC_ASSERT(!is_constructible_v<pair<int, UP>, const int&, const UP&>);
STATIC_ASSERT(!is_constructible_v<pair<UP, int>, const UP&, const int&>);
STATIC_ASSERT(!is_constructible_v<pair<UP, UP>, const UP&, const UP&>);

STATIC_ASSERT(is_constructible_v<tuple<int, int>, const int&, const int&>);
STATIC_ASSERT(!is_constructible_v<tuple<int, UP>, const int&, const UP&>);
STATIC_ASSERT(!is_constructible_v<tuple<UP, int>, const UP&, const int&>);
STATIC_ASSERT(!is_constructible_v<tuple<UP, UP>, const UP&, const UP&>);

STATIC_ASSERT(is_constructible_v<tuple<int, int>, allocator_arg_t, allocator<int>, const int&, const int&>);
STATIC_ASSERT(!is_constructible_v<tuple<int, UP>, allocator_arg_t, allocator<int>, const int&, const UP&>);
STATIC_ASSERT(!is_constructible_v<tuple<UP, int>, allocator_arg_t, allocator<int>, const UP&, const int&>);
STATIC_ASSERT(!is_constructible_v<tuple<UP, UP>, allocator_arg_t, allocator<int>, const UP&, const UP&>);


STATIC_ASSERT(is_constructible_v<pair<A, A>, Im, Im>);
STATIC_ASSERT(!is_constructible_v<pair<A, A>, Im, No>);
STATIC_ASSERT(!is_constructible_v<pair<A, A>, No, Im>);
STATIC_ASSERT(!is_constructible_v<pair<A, A>, No, No>);

STATIC_ASSERT(is_constructible_v<tuple<A, A>, Im, Im>);
STATIC_ASSERT(!is_constructible_v<tuple<A, A>, Im, No>);
STATIC_ASSERT(!is_constructible_v<tuple<A, A>, No, Im>);
STATIC_ASSERT(!is_constructible_v<tuple<A, A>, No, No>);

STATIC_ASSERT(is_constructible_v<tuple<A, A>, allocator_arg_t, allocator<int>, Im, Im>);
STATIC_ASSERT(!is_constructible_v<tuple<A, A>, allocator_arg_t, allocator<int>, Im, No>);
STATIC_ASSERT(!is_constructible_v<tuple<A, A>, allocator_arg_t, allocator<int>, No, Im>);
STATIC_ASSERT(!is_constructible_v<tuple<A, A>, allocator_arg_t, allocator<int>, No, No>);

STATIC_ASSERT(NotConstructible<tuple<A>, No>::value);
STATIC_ASSERT(ImplicitlyConstructible<tuple<A>, Im>::value);
STATIC_ASSERT(ExplicitlyConstructible<tuple<A>, Ex>::value);


STATIC_ASSERT(NotConstructible<pair<A, A>, const pair<No, No>&>::value);
STATIC_ASSERT(NotConstructible<pair<A, A>, const pair<No, Im>&>::value);
STATIC_ASSERT(NotConstructible<pair<A, A>, const pair<Im, No>&>::value);
STATIC_ASSERT(ImplicitlyConstructible<pair<A, A>, const pair<Im, Im>&>::value);
STATIC_ASSERT(ExplicitlyConstructible<pair<A, A>, const pair<Im, Ex>&>::value);
STATIC_ASSERT(ExplicitlyConstructible<pair<A, A>, const pair<Ex, Im>&>::value);
STATIC_ASSERT(ExplicitlyConstructible<pair<A, A>, const pair<Ex, Ex>&>::value);

STATIC_ASSERT(NotConstructible<tuple<A, A>, const tuple<No, No>&>::value);
STATIC_ASSERT(NotConstructible<tuple<A, A>, const tuple<No, Im>&>::value);
STATIC_ASSERT(NotConstructible<tuple<A, A>, const tuple<Im, No>&>::value);
STATIC_ASSERT(ImplicitlyConstructible<tuple<A, A>, const tuple<Im, Im>&>::value);
STATIC_ASSERT(ExplicitlyConstructible<tuple<A, A>, const tuple<Im, Ex>&>::value);
STATIC_ASSERT(ExplicitlyConstructible<tuple<A, A>, const tuple<Ex, Im>&>::value);
STATIC_ASSERT(ExplicitlyConstructible<tuple<A, A>, const tuple<Ex, Ex>&>::value);

STATIC_ASSERT(!is_constructible_v<tuple<A, A>, allocator_arg_t, allocator<int>, const tuple<No, No>&>);
STATIC_ASSERT(!is_constructible_v<tuple<A, A>, allocator_arg_t, allocator<int>, const tuple<No, Im>&>);
STATIC_ASSERT(!is_constructible_v<tuple<A, A>, allocator_arg_t, allocator<int>, const tuple<Im, No>&>);
STATIC_ASSERT(is_constructible_v<tuple<A, A>, allocator_arg_t, allocator<int>, const tuple<Im, Im>&>);
STATIC_ASSERT(is_constructible_v<tuple<A, A>, allocator_arg_t, allocator<int>, const tuple<Im, Ex>&>);
STATIC_ASSERT(is_constructible_v<tuple<A, A>, allocator_arg_t, allocator<int>, const tuple<Ex, Im>&>);
STATIC_ASSERT(is_constructible_v<tuple<A, A>, allocator_arg_t, allocator<int>, const tuple<Ex, Ex>&>);


STATIC_ASSERT(NotConstructible<tuple<A, A>, const pair<No, No>&>::value);
STATIC_ASSERT(NotConstructible<tuple<A, A>, const pair<No, Im>&>::value);
STATIC_ASSERT(NotConstructible<tuple<A, A>, const pair<Im, No>&>::value);
STATIC_ASSERT(ImplicitlyConstructible<tuple<A, A>, const pair<Im, Im>&>::value);
STATIC_ASSERT(ExplicitlyConstructible<tuple<A, A>, const pair<Im, Ex>&>::value);
STATIC_ASSERT(ExplicitlyConstructible<tuple<A, A>, const pair<Ex, Im>&>::value);
STATIC_ASSERT(ExplicitlyConstructible<tuple<A, A>, const pair<Ex, Ex>&>::value);

STATIC_ASSERT(!is_constructible_v<tuple<A, A>, allocator_arg_t, allocator<int>, const pair<No, No>&>);
STATIC_ASSERT(!is_constructible_v<tuple<A, A>, allocator_arg_t, allocator<int>, const pair<No, Im>&>);
STATIC_ASSERT(!is_constructible_v<tuple<A, A>, allocator_arg_t, allocator<int>, const pair<Im, No>&>);
STATIC_ASSERT(is_constructible_v<tuple<A, A>, allocator_arg_t, allocator<int>, const pair<Im, Im>&>);
STATIC_ASSERT(is_constructible_v<tuple<A, A>, allocator_arg_t, allocator<int>, const pair<Im, Ex>&>);
STATIC_ASSERT(is_constructible_v<tuple<A, A>, allocator_arg_t, allocator<int>, const pair<Ex, Im>&>);
STATIC_ASSERT(is_constructible_v<tuple<A, A>, allocator_arg_t, allocator<int>, const pair<Ex, Ex>&>);


STATIC_ASSERT(NotConstructible<pair<A, A>, pair<No, No>>::value);
STATIC_ASSERT(NotConstructible<pair<A, A>, pair<No, Im>>::value);
STATIC_ASSERT(NotConstructible<pair<A, A>, pair<Im, No>>::value);
STATIC_ASSERT(ImplicitlyConstructible<pair<A, A>, pair<Im, Im>>::value);
STATIC_ASSERT(ExplicitlyConstructible<pair<A, A>, pair<Im, Ex>>::value);
STATIC_ASSERT(ExplicitlyConstructible<pair<A, A>, pair<Ex, Im>>::value);
STATIC_ASSERT(ExplicitlyConstructible<pair<A, A>, pair<Ex, Ex>>::value);

STATIC_ASSERT(NotConstructible<tuple<A, A>, tuple<No, No>>::value);
STATIC_ASSERT(NotConstructible<tuple<A, A>, tuple<No, Im>>::value);
STATIC_ASSERT(NotConstructible<tuple<A, A>, tuple<Im, No>>::value);
STATIC_ASSERT(ImplicitlyConstructible<tuple<A, A>, tuple<Im, Im>>::value);
STATIC_ASSERT(ExplicitlyConstructible<tuple<A, A>, tuple<Im, Ex>>::value);
STATIC_ASSERT(ExplicitlyConstructible<tuple<A, A>, tuple<Ex, Im>>::value);
STATIC_ASSERT(ExplicitlyConstructible<tuple<A, A>, tuple<Ex, Ex>>::value);

STATIC_ASSERT(!is_constructible_v<tuple<A, A>, allocator_arg_t, allocator<int>, tuple<No, No>>);
STATIC_ASSERT(!is_constructible_v<tuple<A, A>, allocator_arg_t, allocator<int>, tuple<No, Im>>);
STATIC_ASSERT(!is_constructible_v<tuple<A, A>, allocator_arg_t, allocator<int>, tuple<Im, No>>);
STATIC_ASSERT(is_constructible_v<tuple<A, A>, allocator_arg_t, allocator<int>, tuple<Im, Im>>);
STATIC_ASSERT(is_constructible_v<tuple<A, A>, allocator_arg_t, allocator<int>, tuple<Im, Ex>>);
STATIC_ASSERT(is_constructible_v<tuple<A, A>, allocator_arg_t, allocator<int>, tuple<Ex, Im>>);
STATIC_ASSERT(is_constructible_v<tuple<A, A>, allocator_arg_t, allocator<int>, tuple<Ex, Ex>>);


STATIC_ASSERT(NotConstructible<tuple<A, A>, pair<No, No>>::value);
STATIC_ASSERT(NotConstructible<tuple<A, A>, pair<No, Im>>::value);
STATIC_ASSERT(NotConstructible<tuple<A, A>, pair<Im, No>>::value);
STATIC_ASSERT(ImplicitlyConstructible<tuple<A, A>, pair<Im, Im>>::value);
STATIC_ASSERT(ExplicitlyConstructible<tuple<A, A>, pair<Im, Ex>>::value);
STATIC_ASSERT(ExplicitlyConstructible<tuple<A, A>, pair<Ex, Im>>::value);
STATIC_ASSERT(ExplicitlyConstructible<tuple<A, A>, pair<Ex, Ex>>::value);

STATIC_ASSERT(!is_constructible_v<tuple<A, A>, allocator_arg_t, allocator<int>, pair<No, No>>);
STATIC_ASSERT(!is_constructible_v<tuple<A, A>, allocator_arg_t, allocator<int>, pair<No, Im>>);
STATIC_ASSERT(!is_constructible_v<tuple<A, A>, allocator_arg_t, allocator<int>, pair<Im, No>>);
STATIC_ASSERT(is_constructible_v<tuple<A, A>, allocator_arg_t, allocator<int>, pair<Im, Im>>);
STATIC_ASSERT(is_constructible_v<tuple<A, A>, allocator_arg_t, allocator<int>, pair<Im, Ex>>);
STATIC_ASSERT(is_constructible_v<tuple<A, A>, allocator_arg_t, allocator<int>, pair<Ex, Im>>);
STATIC_ASSERT(is_constructible_v<tuple<A, A>, allocator_arg_t, allocator<int>, pair<Ex, Ex>>);


STATIC_ASSERT(!is_constructible_v<tuple<int, int, int>, const int&, const int&>);
STATIC_ASSERT(is_constructible_v<tuple<int, int, int>, const int&, const int&, const int&>);
STATIC_ASSERT(!is_constructible_v<tuple<int, int, int>, const int&, const int&, const int&, const int&>);

STATIC_ASSERT(!is_constructible_v<tuple<int, int, int>, short, short>);
STATIC_ASSERT(is_constructible_v<tuple<int, int, int>, short, short, short>);
STATIC_ASSERT(!is_constructible_v<tuple<int, int, int>, short, short, short, short>);

STATIC_ASSERT(NotConstructible<tuple<A, A, A>, const tuple<Im, Im>&>::value);
STATIC_ASSERT(ImplicitlyConstructible<tuple<A, A, A>, const tuple<Im, Im, Im>&>::value);
STATIC_ASSERT(NotConstructible<tuple<A, A, A>, const tuple<Im, Im, Im, Im>&>::value);

STATIC_ASSERT(NotConstructible<tuple<A, A, A>, const tuple<Ex, Ex>&>::value);
STATIC_ASSERT(ExplicitlyConstructible<tuple<A, A, A>, const tuple<Ex, Ex, Ex>&>::value);
STATIC_ASSERT(NotConstructible<tuple<A, A, A>, const tuple<Ex, Ex, Ex, Ex>&>::value);

STATIC_ASSERT(NotConstructible<tuple<A, A, A>, tuple<Im, Im>>::value);
STATIC_ASSERT(ImplicitlyConstructible<tuple<A, A, A>, tuple<Im, Im, Im>>::value);
STATIC_ASSERT(NotConstructible<tuple<A, A, A>, tuple<Im, Im, Im, Im>>::value);

STATIC_ASSERT(NotConstructible<tuple<A, A, A>, tuple<Ex, Ex>>::value);
STATIC_ASSERT(ExplicitlyConstructible<tuple<A, A, A>, tuple<Ex, Ex, Ex>>::value);
STATIC_ASSERT(NotConstructible<tuple<A, A, A>, tuple<Ex, Ex, Ex, Ex>>::value);

STATIC_ASSERT(NotConstructible<tuple<A>, const pair<Im, Im>&>::value);
STATIC_ASSERT(ImplicitlyConstructible<tuple<A, A>, const pair<Im, Im>&>::value);
STATIC_ASSERT(NotConstructible<tuple<A, A, A>, const pair<Im, Im>&>::value);

STATIC_ASSERT(NotConstructible<tuple<A>, const pair<Ex, Ex>&>::value);
STATIC_ASSERT(ExplicitlyConstructible<tuple<A, A>, const pair<Ex, Ex>&>::value);
STATIC_ASSERT(NotConstructible<tuple<A, A, A>, const pair<Ex, Ex>&>::value);

STATIC_ASSERT(NotConstructible<tuple<A>, pair<Im, Im>>::value);
STATIC_ASSERT(ImplicitlyConstructible<tuple<A, A>, pair<Im, Im>>::value);
STATIC_ASSERT(NotConstructible<tuple<A, A, A>, pair<Im, Im>>::value);

STATIC_ASSERT(NotConstructible<tuple<A>, pair<Ex, Ex>>::value);
STATIC_ASSERT(ExplicitlyConstructible<tuple<A, A>, pair<Ex, Ex>>::value);
STATIC_ASSERT(NotConstructible<tuple<A, A, A>, pair<Ex, Ex>>::value);


STATIC_ASSERT(!is_constructible_v<tuple<int, int, int>, allocator_arg_t, allocator<int>, const int&, const int&>);
STATIC_ASSERT(
    is_constructible_v<tuple<int, int, int>, allocator_arg_t, allocator<int>, const int&, const int&, const int&>);
STATIC_ASSERT(!is_constructible_v<tuple<int, int, int>, allocator_arg_t, allocator<int>, const int&, const int&,
              const int&, const int&>);

STATIC_ASSERT(!is_constructible_v<tuple<int, int, int>, allocator_arg_t, allocator<int>, short, short>);
STATIC_ASSERT(is_constructible_v<tuple<int, int, int>, allocator_arg_t, allocator<int>, short, short, short>);
STATIC_ASSERT(!is_constructible_v<tuple<int, int, int>, allocator_arg_t, allocator<int>, short, short, short, short>);

STATIC_ASSERT(!is_constructible_v<tuple<A, A, A>, allocator_arg_t, allocator<int>, const tuple<Im, Im>&>);
STATIC_ASSERT(is_constructible_v<tuple<A, A, A>, allocator_arg_t, allocator<int>, const tuple<Im, Im, Im>&>);
STATIC_ASSERT(!is_constructible_v<tuple<A, A, A>, allocator_arg_t, allocator<int>, const tuple<Im, Im, Im, Im>&>);

STATIC_ASSERT(!is_constructible_v<tuple<A, A, A>, allocator_arg_t, allocator<int>, const tuple<Ex, Ex>&>);
STATIC_ASSERT(is_constructible_v<tuple<A, A, A>, allocator_arg_t, allocator<int>, const tuple<Ex, Ex, Ex>&>);
STATIC_ASSERT(!is_constructible_v<tuple<A, A, A>, allocator_arg_t, allocator<int>, const tuple<Ex, Ex, Ex, Ex>&>);

STATIC_ASSERT(!is_constructible_v<tuple<A, A, A>, allocator_arg_t, allocator<int>, tuple<Im, Im>>);
STATIC_ASSERT(is_constructible_v<tuple<A, A, A>, allocator_arg_t, allocator<int>, tuple<Im, Im, Im>>);
STATIC_ASSERT(!is_constructible_v<tuple<A, A, A>, allocator_arg_t, allocator<int>, tuple<Im, Im, Im, Im>>);

STATIC_ASSERT(!is_constructible_v<tuple<A, A, A>, allocator_arg_t, allocator<int>, tuple<Ex, Ex>>);
STATIC_ASSERT(is_constructible_v<tuple<A, A, A>, allocator_arg_t, allocator<int>, tuple<Ex, Ex, Ex>>);
STATIC_ASSERT(!is_constructible_v<tuple<A, A, A>, allocator_arg_t, allocator<int>, tuple<Ex, Ex, Ex, Ex>>);

STATIC_ASSERT(!is_constructible_v<tuple<A>, allocator_arg_t, allocator<int>, const pair<Im, Im>&>);
STATIC_ASSERT(is_constructible_v<tuple<A, A>, allocator_arg_t, allocator<int>, const pair<Im, Im>&>);
STATIC_ASSERT(!is_constructible_v<tuple<A, A, A>, allocator_arg_t, allocator<int>, const pair<Im, Im>&>);

STATIC_ASSERT(!is_constructible_v<tuple<A>, allocator_arg_t, allocator<int>, const pair<Ex, Ex>&>);
STATIC_ASSERT(is_constructible_v<tuple<A, A>, allocator_arg_t, allocator<int>, const pair<Ex, Ex>&>);
STATIC_ASSERT(!is_constructible_v<tuple<A, A, A>, allocator_arg_t, allocator<int>, const pair<Ex, Ex>&>);

STATIC_ASSERT(!is_constructible_v<tuple<A>, allocator_arg_t, allocator<int>, pair<Im, Im>>);
STATIC_ASSERT(is_constructible_v<tuple<A, A>, allocator_arg_t, allocator<int>, pair<Im, Im>>);
STATIC_ASSERT(!is_constructible_v<tuple<A, A, A>, allocator_arg_t, allocator<int>, pair<Im, Im>>);

STATIC_ASSERT(!is_constructible_v<tuple<A>, allocator_arg_t, allocator<int>, pair<Ex, Ex>>);
STATIC_ASSERT(is_constructible_v<tuple<A, A>, allocator_arg_t, allocator<int>, pair<Ex, Ex>>);
STATIC_ASSERT(!is_constructible_v<tuple<A, A, A>, allocator_arg_t, allocator<int>, pair<Ex, Ex>>);


pair<int, int> func1() {
    const int x = 17;
    const int y = 29;
    return {x, y};
}

pair<string, string> func2() {
    return {"darth", "vader"};
}

tuple<int, int, int> func3() {
    const int x = 11;
    const int y = 22;
    const int z = 33;
    return {x, y, z};
}

tuple<string, string, string> func4() {
    return {"red", "green", "blue"};
}

void test_VSO_191303();

void test_copy_move();

void test_VSO_684710();

int main() {
    assert(func1() == make_pair(17, 29));
    assert(func2() == make_pair("darth"s, "vader"s));
    assert(func3() == make_tuple(11, 22, 33));
    assert(func4() == make_tuple("red"s, "green"s, "blue"s));


    // LWG-2549 "Tuple EXPLICIT constructor templates that take tuple parameters
    // end up taking references to temporaries and will create dangling references"
    {
        tuple<int> t1(1729);
        tuple<tuple<int>&&> t2(move(t1));

        assert(&get<0>(t2) == &t1);
        assert(&get<0>(get<0>(t2)) == &get<0>(t1));

        const tuple<int> t3(2015);
        tuple<const tuple<int>&> t4(t3);

        assert(&get<0>(t4) == &t3);
        assert(&get<0>(get<0>(t4)) == &get<0>(t3));
    }


    test_VSO_191303();

    test_copy_move();

    test_VSO_684710();
}


// Test VSO-191303 "<tuple>: VC14.2 CTP1: std::tuple broken"

struct None {};

template <typename T>
struct Optional {
    T thing;

    Optional(None) : thing() {}

    template <typename Expr, typename = enable_if_t<!is_same_v<decay_t<Expr>, None>>>
    Optional(Expr&& expr) : thing(forward<Expr>(expr)) {}
};

void test_VSO_191303() {
    tuple<int, None> one{};

    tuple<int, Optional<double>> two(one);
}


// Test:
// VSO-215996 "<tuple>: Wrong tuple constructor overload resolution causes stack overflow"
// VSO-216014 "<tuple>: Tuple constructor overload resolution error"

template <typename X>
struct AlwaysFalse : false_type {};

// AbsorbingRef and AbsorbingVal need to appear to be omni-constructible,
// so they use static_assert instead of =delete.

struct AbsorbingRef {
    AbsorbingRef() {}

    AbsorbingRef(const AbsorbingRef&) {}

    template <typename T>
    AbsorbingRef(const T&) {
        STATIC_ASSERT(AlwaysFalse<T>::value);
    }
};

struct AbsorbingVal {
    AbsorbingVal() {}

    AbsorbingVal(const AbsorbingVal&) {}

    template <typename U>
    AbsorbingVal(U) {
        STATIC_ASSERT(AlwaysFalse<U>::value);
    }
};

void test_copy_move() {
    tuple<AbsorbingRef> r1;
    tuple<AbsorbingRef> r2(r1);
    tuple<AbsorbingRef> r3 = r2;
    tuple<AbsorbingRef> r4(move(r3));
    tuple<AbsorbingRef> r5 = move(r4);

    tuple<AbsorbingVal> v1;
    tuple<AbsorbingVal> v2(v1);
    tuple<AbsorbingVal> v3 = v2;
    tuple<AbsorbingVal> v4(move(v3));
    tuple<AbsorbingVal> v5 = move(v4);
}

// Test:
// VSO-684710: "[Feedback] <pair>: assignments from types convertible-to-pair do not compile"

void test_VSO_684710() {
    using P = pair<int, int>;
    P target{};

    struct S1 {
        operator P() const {
            return P{};
        }
    };

    target = S1{};
    S1 s1{};
    target = s1;

    struct S2 {
        P p;

        operator P&&() {
            return move(p);
        }
    };
    target = S2{};
    S2 s2{};
    target = s2;

    struct S3 {
        P p;

        operator const P&() const {
            return p;
        }
    };
    target = S3{};
    S3 s3{};
    target = s3;
}
