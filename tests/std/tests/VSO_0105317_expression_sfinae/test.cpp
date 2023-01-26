// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <functional>
#include <memory>
#include <type_traits>

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

// Test:
// N3462 "std::result_of and SFINAE"
// VSO-105317 "<type_traits>: [expression sfinae] std::result_of used in template function declaration can cause C2893
// (probably ignores SFINAE rule)" VSO-120635 "<type_traits>: [expression sfinae] C++14 result_of must be
// SFINAE-friendly"

#if _HAS_CXX17
template <typename Void, typename Callable, typename... Args>
struct HasInvokeResultT : false_type {
    STATIC_ASSERT(!is_invocable_v<Callable, Args...>);
    STATIC_ASSERT(!is_nothrow_invocable_v<Callable, Args...>);
    STATIC_ASSERT(!is_invocable_r_v<void, Callable, Args...>);
    STATIC_ASSERT(!is_nothrow_invocable_r_v<void, Callable, Args...>);
};

template <typename Callable, typename... Args>
struct HasInvokeResultT<void_t<invoke_result_t<Callable, Args...>>, Callable, Args...> : true_type {
    STATIC_ASSERT(is_invocable_v<Callable, Args...>);
    STATIC_ASSERT(is_invocable_r_v<void, Callable, Args...>);
};

template <typename A>
struct HasResultOfT;

template <typename Callable, typename... Args>
struct HasResultOfT<Callable(Args...)> : HasInvokeResultT<void, Callable, Args...>::type {};
#else // _HAS_CXX17
template <typename A, typename = void>
struct HasResultOfT : false_type {};

template <typename A>
struct HasResultOfT<A, void_t<result_of_t<A>>> : true_type {};
#endif // _HAS_CXX17


// BAD: Integers aren't callable.
STATIC_ASSERT(!HasResultOfT<int(int, int)>::value);


// GOOD: Function pointers are callable.
using FP = int (*)(int);
STATIC_ASSERT(HasResultOfT<FP(int)>::value);

// BAD: Not enough arguments.
STATIC_ASSERT(!HasResultOfT<FP()>::value);

// BAD: Too many arguments.
STATIC_ASSERT(!HasResultOfT<FP(int, int)>::value);

// BAD: Can't convert argument.
STATIC_ASSERT(!HasResultOfT<FP(void*)>::value);

// GOOD: User-defined conversion.
struct Okay {
    operator int();
};
STATIC_ASSERT(HasResultOfT<FP(Okay)>::value);

// BAD: Multiple user-defined conversions.
struct MultipleUDCs {
    operator Okay();
};
STATIC_ASSERT(!HasResultOfT<FP(MultipleUDCs)>::value);

// BAD: Inaccessible conversion.
class Inaccessible {
private:
    operator int();
};
STATIC_ASSERT(!HasResultOfT<FP(Inaccessible)>::value);

// BAD: Ambiguous conversion.
struct Ambiguous {
    operator short();
    operator unsigned short();
};
STATIC_ASSERT(!HasResultOfT<FP(Ambiguous)>::value);

// BAD: Explicit conversion.
struct Explicit {
    explicit operator int();
};
STATIC_ASSERT(!HasResultOfT<FP(Explicit)>::value);

// GOOD: cv-qualifiers and ref-qualifiers.
struct Woof {
    operator int() &;
};
STATIC_ASSERT(HasResultOfT<FP(Woof&)>::value);

// BAD: cv-qualifiers and ref-qualifiers.
STATIC_ASSERT(!HasResultOfT<FP(const Woof&)>::value);
STATIC_ASSERT(!HasResultOfT<FP(Woof)>::value);

// GOOD: Templated UDC.
struct Template {
    template <typename T>
    operator T();
};
STATIC_ASSERT(HasResultOfT<FP(Template)>::value);

// BAD: Template argument deduction failure.
struct Deduction {
    template <typename T, typename U>
    operator T();
};
STATIC_ASSERT(!HasResultOfT<FP(Deduction)>::value);

// BAD: Substitution failure.
struct Substitution {
    template <typename T, typename = enable_if_t<!is_integral_v<T>>>
    operator T();
};
STATIC_ASSERT(!HasResultOfT<FP(Substitution)>::value);

// GOOD: Reference binding.
using FP2 = int (*)(int&);
STATIC_ASSERT(HasResultOfT<FP2(int&)>::value);

// BAD: Reference binding can't discard constness.
STATIC_ASSERT(!HasResultOfT<FP2(const int&)>::value);

// GOOD: Rvalue references bind to rvalues.
using FP3 = int (*)(int&&);
STATIC_ASSERT(HasResultOfT<FP3(int)>::value);

// BAD: Rvalue references can't bind to lvalues.
STATIC_ASSERT(!HasResultOfT<FP3(int&)>::value);


// Function references behave like function pointers.
using FR = int (&)(int);
STATIC_ASSERT(HasResultOfT<FR(int)>::value);
STATIC_ASSERT(!HasResultOfT<FR()>::value);
STATIC_ASSERT(!HasResultOfT<FR(int, int)>::value);
STATIC_ASSERT(!HasResultOfT<FR(void*)>::value);
STATIC_ASSERT(HasResultOfT<FR(Okay)>::value);
STATIC_ASSERT(!HasResultOfT<FR(MultipleUDCs)>::value);
STATIC_ASSERT(!HasResultOfT<FR(Inaccessible)>::value);
STATIC_ASSERT(!HasResultOfT<FR(Ambiguous)>::value);
STATIC_ASSERT(!HasResultOfT<FR(Explicit)>::value);
STATIC_ASSERT(HasResultOfT<FR(Woof&)>::value);
STATIC_ASSERT(!HasResultOfT<FR(const Woof&)>::value);
STATIC_ASSERT(!HasResultOfT<FR(Woof)>::value);
STATIC_ASSERT(HasResultOfT<FR(Template)>::value);
STATIC_ASSERT(!HasResultOfT<FR(Deduction)>::value);
STATIC_ASSERT(!HasResultOfT<FR(Substitution)>::value);
using FR2 = int (&)(int&);
STATIC_ASSERT(HasResultOfT<FR2(int&)>::value);
STATIC_ASSERT(!HasResultOfT<FR2(const int&)>::value);
using FR3 = int (&)(int&&);
STATIC_ASSERT(HasResultOfT<FR3(int)>::value);
STATIC_ASSERT(!HasResultOfT<FR3(int&)>::value);


// BAD: Totally empty classes aren't callable.
struct Empty {};
STATIC_ASSERT(!HasResultOfT<Empty(int)>::value);


// Classes with function call operators behave like function pointers.
struct Functor {
    int operator()(int);
};
STATIC_ASSERT(HasResultOfT<Functor(int)>::value);
STATIC_ASSERT(!HasResultOfT<Functor()>::value);
STATIC_ASSERT(!HasResultOfT<Functor(int, int)>::value);
STATIC_ASSERT(!HasResultOfT<Functor(void*)>::value);
STATIC_ASSERT(HasResultOfT<Functor(Okay)>::value);
STATIC_ASSERT(!HasResultOfT<Functor(MultipleUDCs)>::value);
STATIC_ASSERT(!HasResultOfT<Functor(Inaccessible)>::value);
STATIC_ASSERT(!HasResultOfT<Functor(Ambiguous)>::value);
STATIC_ASSERT(!HasResultOfT<Functor(Explicit)>::value);
STATIC_ASSERT(HasResultOfT<Functor(Woof&)>::value);
STATIC_ASSERT(!HasResultOfT<Functor(const Woof&)>::value);
STATIC_ASSERT(!HasResultOfT<Functor(Woof)>::value);
STATIC_ASSERT(HasResultOfT<Functor(Template)>::value);
STATIC_ASSERT(!HasResultOfT<Functor(Deduction)>::value);
STATIC_ASSERT(!HasResultOfT<Functor(Substitution)>::value);
struct Functor2 {
    int operator()(int&);
};
STATIC_ASSERT(HasResultOfT<Functor2(int&)>::value);
STATIC_ASSERT(!HasResultOfT<Functor2(const int&)>::value);
struct Functor3 {
    int operator()(int&&);
};
STATIC_ASSERT(HasResultOfT<Functor3(int)>::value);
STATIC_ASSERT(!HasResultOfT<Functor3(int&)>::value);

// More scenarios.
class InaccessibleFunctor {
private:
    int operator()(int);
};
STATIC_ASSERT(!HasResultOfT<InaccessibleFunctor(int)>::value);
struct AmbiguousFunctor {
    int operator()(long);
    int operator()(long long);
};
STATIC_ASSERT(!HasResultOfT<AmbiguousFunctor(int)>::value);
struct OinkFunctor {
    int operator()(int) &;
};
STATIC_ASSERT(HasResultOfT<OinkFunctor&(int)>::value);
STATIC_ASSERT(!HasResultOfT<const OinkFunctor&(int)>::value);
STATIC_ASSERT(!HasResultOfT<OinkFunctor(int)>::value);
struct TemplateFunctor {
    template <typename T>
    int operator()(T);
};
STATIC_ASSERT(HasResultOfT<TemplateFunctor(int)>::value);
struct DeductionFunctor {
    template <typename T, typename U>
    int operator()(T);
};
STATIC_ASSERT(!HasResultOfT<DeductionFunctor(int)>::value);
struct SubstitutionFunctor {
    template <typename T, typename = enable_if_t<!is_integral_v<T>>>
    int operator()(T);
};
STATIC_ASSERT(!HasResultOfT<SubstitutionFunctor(int)>::value);


// Classes that are convertible to function pointers are also callable.
// See N4527 13.3.1.1.2 [over.call.object]/2.
struct Conv {
    operator FP();
};
STATIC_ASSERT(HasResultOfT<Conv(int)>::value);
STATIC_ASSERT(!HasResultOfT<Conv()>::value);
STATIC_ASSERT(!HasResultOfT<Conv(int, int)>::value);
STATIC_ASSERT(!HasResultOfT<Conv(void*)>::value);
STATIC_ASSERT(HasResultOfT<Conv(Okay)>::value);
STATIC_ASSERT(!HasResultOfT<Conv(MultipleUDCs)>::value);
STATIC_ASSERT(!HasResultOfT<Conv(Inaccessible)>::value);
STATIC_ASSERT(!HasResultOfT<Conv(Ambiguous)>::value);
STATIC_ASSERT(!HasResultOfT<Conv(Explicit)>::value);
STATIC_ASSERT(HasResultOfT<Conv(Woof&)>::value);
STATIC_ASSERT(!HasResultOfT<Conv(const Woof&)>::value);
STATIC_ASSERT(!HasResultOfT<Conv(Woof)>::value);
STATIC_ASSERT(HasResultOfT<Conv(Template)>::value);
STATIC_ASSERT(!HasResultOfT<Conv(Deduction)>::value);
STATIC_ASSERT(!HasResultOfT<Conv(Substitution)>::value);
struct Conv2 {
    operator FP2();
};
STATIC_ASSERT(HasResultOfT<Conv2(int&)>::value);
STATIC_ASSERT(!HasResultOfT<Conv2(const int&)>::value);
struct Conv3 {
    operator FP3();
};
STATIC_ASSERT(HasResultOfT<Conv3(int)>::value);
STATIC_ASSERT(!HasResultOfT<Conv3(int&)>::value);
class InaccessibleConv {
private:
    operator FP();
};
STATIC_ASSERT(!HasResultOfT<InaccessibleConv(int)>::value);
using FP4 = int (*)(long);
using FP5 = int (*)(long long);
struct AmbiguousConv {
    operator FP4();
    operator FP5();
};
STATIC_ASSERT(!HasResultOfT<AmbiguousConv(int)>::value);
struct OinkConv {
    operator FP() &;
};
STATIC_ASSERT(HasResultOfT<OinkConv&(int)>::value);
STATIC_ASSERT(!HasResultOfT<const OinkConv&(int)>::value);
STATIC_ASSERT(!HasResultOfT<OinkConv(int)>::value);

// BAD: Explicit conversions to function pointers aren't callable.
struct ExplicitConv {
    explicit operator FP();
};
STATIC_ASSERT(!HasResultOfT<ExplicitConv(int)>::value);


struct X {};

struct Unrelated {};

class InaccessibleDerived : private X {};

struct DerivedOne : X {};
struct DerivedTwo : X {};
struct AmbiguousDerived : DerivedOne, DerivedTwo {};

struct Pointer {
    X& operator*();
};

class InaccessiblePointer {
private:
    X& operator*();
};

struct AmbiguousPointer {
    template <typename = void>
    X& operator*();
    template <int = 0>
    X& operator*();
};

struct OinkPointer {
    X& operator*() &;
};

struct TemplatePointer {
    template <typename T = void>
    X& operator*();
};

struct DeductionPointer {
    template <typename T>
    X& operator*();
};

struct SubstitutionPointer {
    template <typename T = void, typename = enable_if_t<!is_void_v<T>>>
    X& operator*();
};


// GOOD: PMDs are callable.
using PMD = int X::*;
STATIC_ASSERT(HasResultOfT<PMD(X)>::value);

// BAD: Zero arguments (important).
STATIC_ASSERT(!HasResultOfT<PMD()>::value);

// BAD: Too many arguments.
STATIC_ASSERT(!HasResultOfT<PMD(X, int)>::value);

// BAD: int is bogus.
STATIC_ASSERT(!HasResultOfT<PMD(int)>::value);

// BAD: Also bogus.
STATIC_ASSERT(!HasResultOfT<PMD(Unrelated)>::value);

// BAD: Inaccessible inheritance.
STATIC_ASSERT(!HasResultOfT<PMD(InaccessibleDerived)>::value);

// BAD: Ambiguous inheritance.
STATIC_ASSERT(!HasResultOfT<PMD(AmbiguousDerived)>::value);

// PMDs are also callable on reference_wrappers.
STATIC_ASSERT(HasResultOfT<PMD(reference_wrapper<X>)>::value);
STATIC_ASSERT(!HasResultOfT<PMD(reference_wrapper<X>, int)>::value);
STATIC_ASSERT(!HasResultOfT<PMD(reference_wrapper<int>)>::value);
STATIC_ASSERT(!HasResultOfT<PMD(reference_wrapper<Unrelated>)>::value);
STATIC_ASSERT(!HasResultOfT<PMD(reference_wrapper<InaccessibleDerived>)>::value);
STATIC_ASSERT(!HasResultOfT<PMD(reference_wrapper<AmbiguousDerived>)>::value);

// PMDs are also callable on pointers.
STATIC_ASSERT(HasResultOfT<PMD(X*)>::value);
STATIC_ASSERT(!HasResultOfT<PMD(X*, int)>::value);
STATIC_ASSERT(!HasResultOfT<PMD(int*)>::value);
STATIC_ASSERT(!HasResultOfT<PMD(Unrelated*)>::value);
STATIC_ASSERT(!HasResultOfT<PMD(InaccessibleDerived*)>::value);
STATIC_ASSERT(!HasResultOfT<PMD(AmbiguousDerived*)>::value);

// Smart pointer scenarios.
STATIC_ASSERT(HasResultOfT<PMD(Pointer)>::value);
STATIC_ASSERT(!HasResultOfT<PMD(InaccessiblePointer)>::value);
STATIC_ASSERT(!HasResultOfT<PMD(AmbiguousPointer)>::value);
STATIC_ASSERT(HasResultOfT<PMD(OinkPointer&)>::value);
STATIC_ASSERT(!HasResultOfT<PMD(const OinkPointer&)>::value);
STATIC_ASSERT(!HasResultOfT<PMD(OinkPointer)>::value);
STATIC_ASSERT(HasResultOfT<PMD(TemplatePointer)>::value);
STATIC_ASSERT(!HasResultOfT<PMD(DeductionPointer)>::value);
STATIC_ASSERT(!HasResultOfT<PMD(SubstitutionPointer)>::value);


// PMFs have all of the issues that PMDs have. (There are a couple more cases for arity.)
using PMF = int (X::*)(int);
STATIC_ASSERT(HasResultOfT<PMF(X, int)>::value);
STATIC_ASSERT(!HasResultOfT<PMF(X)>::value);
STATIC_ASSERT(!HasResultOfT<PMF()>::value);
STATIC_ASSERT(!HasResultOfT<PMF(X, int, int)>::value);
STATIC_ASSERT(!HasResultOfT<PMF(int, int)>::value);
STATIC_ASSERT(!HasResultOfT<PMF(Unrelated, int)>::value);
STATIC_ASSERT(!HasResultOfT<PMF(InaccessibleDerived, int)>::value);
STATIC_ASSERT(!HasResultOfT<PMF(AmbiguousDerived, int)>::value);
STATIC_ASSERT(HasResultOfT<PMF(reference_wrapper<X>, int)>::value);
STATIC_ASSERT(!HasResultOfT<PMF(reference_wrapper<X>)>::value);
STATIC_ASSERT(!HasResultOfT<PMF(reference_wrapper<X>, int, int)>::value);
STATIC_ASSERT(!HasResultOfT<PMF(reference_wrapper<int>, int)>::value);
STATIC_ASSERT(!HasResultOfT<PMF(reference_wrapper<Unrelated>, int)>::value);
STATIC_ASSERT(!HasResultOfT<PMF(reference_wrapper<InaccessibleDerived>, int)>::value);
STATIC_ASSERT(!HasResultOfT<PMF(reference_wrapper<AmbiguousDerived>, int)>::value);
STATIC_ASSERT(HasResultOfT<PMF(X*, int)>::value);
STATIC_ASSERT(!HasResultOfT<PMF(X*)>::value);
STATIC_ASSERT(!HasResultOfT<PMF(X*, int, int)>::value);
STATIC_ASSERT(!HasResultOfT<PMF(int*, int)>::value);
STATIC_ASSERT(!HasResultOfT<PMF(Unrelated*, int)>::value);
STATIC_ASSERT(!HasResultOfT<PMF(InaccessibleDerived*, int)>::value);
STATIC_ASSERT(!HasResultOfT<PMF(AmbiguousDerived*, int)>::value);
STATIC_ASSERT(HasResultOfT<PMF(Pointer, int)>::value);
STATIC_ASSERT(!HasResultOfT<PMF(InaccessiblePointer, int)>::value);
STATIC_ASSERT(!HasResultOfT<PMF(AmbiguousPointer, int)>::value);
STATIC_ASSERT(HasResultOfT<PMF(OinkPointer&, int)>::value);
STATIC_ASSERT(!HasResultOfT<PMF(const OinkPointer&, int)>::value);
STATIC_ASSERT(!HasResultOfT<PMF(OinkPointer, int)>::value);
STATIC_ASSERT(HasResultOfT<PMF(TemplatePointer, int)>::value);
STATIC_ASSERT(!HasResultOfT<PMF(DeductionPointer, int)>::value);
STATIC_ASSERT(!HasResultOfT<PMF(SubstitutionPointer, int)>::value);


// PMFs also have all of the issues that function pointers have.
STATIC_ASSERT(!HasResultOfT<PMF(X, void*)>::value);
STATIC_ASSERT(HasResultOfT<PMF(X, Okay)>::value);
STATIC_ASSERT(!HasResultOfT<PMF(X, MultipleUDCs)>::value);
STATIC_ASSERT(!HasResultOfT<PMF(X, Inaccessible)>::value);
STATIC_ASSERT(!HasResultOfT<PMF(X, Ambiguous)>::value);
STATIC_ASSERT(!HasResultOfT<PMF(X, Explicit)>::value);
STATIC_ASSERT(HasResultOfT<PMF(X, Woof&)>::value);
STATIC_ASSERT(!HasResultOfT<PMF(X, const Woof&)>::value);
STATIC_ASSERT(!HasResultOfT<PMF(X, Woof)>::value);
STATIC_ASSERT(HasResultOfT<PMF(X, Template)>::value);
STATIC_ASSERT(!HasResultOfT<PMF(X, Deduction)>::value);
STATIC_ASSERT(!HasResultOfT<PMF(X, Substitution)>::value);
using PMF2 = int (X::*)(int&);
STATIC_ASSERT(HasResultOfT<PMF2(X, int&)>::value);
STATIC_ASSERT(!HasResultOfT<PMF2(X, const int&)>::value);
using PMF3 = int (X::*)(int&&);
STATIC_ASSERT(HasResultOfT<PMF3(X, int)>::value);
STATIC_ASSERT(!HasResultOfT<PMF3(X, int&)>::value);


// Finally, PMFs care about cv-qualifiers and ref-qualifiers.
using PMF4 = int (X::*)(int) &;
STATIC_ASSERT(HasResultOfT<PMF4(X&, int)>::value);
STATIC_ASSERT(!HasResultOfT<PMF4(const X&, int)>::value);
STATIC_ASSERT(!HasResultOfT<PMF4(X&&, int)>::value);
STATIC_ASSERT(!HasResultOfT<PMF4(const X&&, int)>::value);
using PMF5 = int (X::*)(int) &&;
STATIC_ASSERT(!HasResultOfT<PMF5(X&, int)>::value);
STATIC_ASSERT(!HasResultOfT<PMF5(const X&, int)>::value);
STATIC_ASSERT(HasResultOfT<PMF5(X&&, int)>::value);
STATIC_ASSERT(!HasResultOfT<PMF5(const X&&, int)>::value);


#if _HAS_CXX17
// Test:
// P0604R0 invoke_result, is_invocable, is_nothrow_invocable

// HasResultOfT also tests is_invocable.

// Test is_invocable_r. (Same scenarios as std::function SFINAE below.)
using Long_FP_Short            = long (*)(short);
using Int_FP_Int               = int (*)(int);
using Long_FP_DoubleStar       = long (*)(double*);
using DoubleStar_FP_Short      = double* (*) (short);
using DoubleStar_FP_DoubleStar = double* (*) (double*);
using Void_FP_Short            = void (*)(short);
using Void_FP_DoubleStar       = void (*)(double*);
using SharedPtrInt_FP_Nullary  = shared_ptr<int> (*)();
STATIC_ASSERT(is_invocable_r_v<long, Long_FP_Short, short>);
STATIC_ASSERT(is_invocable_r_v<long, Int_FP_Int, short>);
STATIC_ASSERT(!is_invocable_r_v<long, Long_FP_DoubleStar, short>);
STATIC_ASSERT(!is_invocable_r_v<long, DoubleStar_FP_Short, short>);
STATIC_ASSERT(!is_invocable_r_v<long, DoubleStar_FP_DoubleStar, short>);
STATIC_ASSERT(!is_invocable_r_v<long, Void_FP_Short, short>);
STATIC_ASSERT(!is_invocable_r_v<long, Void_FP_DoubleStar, short>);
STATIC_ASSERT(is_invocable_r_v<void, Void_FP_Short, short>);
STATIC_ASSERT(is_invocable_r_v<void, Int_FP_Int, short>);
STATIC_ASSERT(!is_invocable_r_v<void, Void_FP_DoubleStar, short>);
STATIC_ASSERT(is_invocable_r_v<shared_ptr<const int>, SharedPtrInt_FP_Nullary>);
STATIC_ASSERT(!is_invocable_r_v<bool, SharedPtrInt_FP_Nullary>);

// Finally, test is_nothrow_invocable and is_nothrow_invocable_r.

struct Kitty {
    int operator()(int) noexcept;

    int operator()(int, int);
};

// Not callable due to input.
STATIC_ASSERT(!is_invocable_v<Kitty, double*>);
STATIC_ASSERT(!is_nothrow_invocable_v<Kitty, double*>);

// Not callable due to output.
STATIC_ASSERT(!is_invocable_r_v<double*, Kitty, int>);
STATIC_ASSERT(!is_nothrow_invocable_r_v<double*, Kitty, int>);

// When the return type is void, does the invocation throw?
STATIC_ASSERT(is_invocable_v<Kitty, int>);
STATIC_ASSERT(is_nothrow_invocable_v<Kitty, int>);
STATIC_ASSERT(is_invocable_v<Kitty, int, int>);
STATIC_ASSERT(!is_nothrow_invocable_v<Kitty, int, int>);

// These PMD invocations don't throw.
STATIC_ASSERT(is_invocable_v<PMD, X>);
STATIC_ASSERT(is_nothrow_invocable_v<PMD, X>);
STATIC_ASSERT(is_invocable_v<PMD, reference_wrapper<X>>);
STATIC_ASSERT(is_nothrow_invocable_v<PMD, reference_wrapper<X>>);
STATIC_ASSERT(is_invocable_v<PMD, X*>);
STATIC_ASSERT(is_nothrow_invocable_v<PMD, X*>);

// Test noexcept function types.
#ifdef __cpp_noexcept_function_type
using FP_noexcept                                 = int (*)(int) noexcept;
using FR_noexcept                                 = int (&)(int) noexcept;
using PMF_noexcept                                = int (X::*)(int) noexcept;
inline constexpr bool noexcept_in_the_type_system = true;
#else // ^^^ __cpp_noexcept_function_type / !__cpp_noexcept_function_type vvv
using FP_noexcept                                 = int (*)(int);
using FR_noexcept                                 = int (&)(int);
using PMF_noexcept                                = int (X::*)(int);
inline constexpr bool noexcept_in_the_type_system = false;
#endif // __cpp_noexcept_function_type

STATIC_ASSERT(is_invocable_v<FP, int>);
STATIC_ASSERT(!is_nothrow_invocable_v<FP, int>);
STATIC_ASSERT(is_invocable_v<FP_noexcept, int>);
STATIC_ASSERT(is_nothrow_invocable_v<FP_noexcept, int> == noexcept_in_the_type_system);

STATIC_ASSERT(is_invocable_v<FR, int>);
STATIC_ASSERT(!is_nothrow_invocable_v<FR, int>);
STATIC_ASSERT(is_invocable_v<FR_noexcept, int>);
STATIC_ASSERT(is_nothrow_invocable_v<FR_noexcept, int> == noexcept_in_the_type_system);

STATIC_ASSERT(is_invocable_v<PMF, X, int>);
STATIC_ASSERT(!is_nothrow_invocable_v<PMF, X, int>);
STATIC_ASSERT(is_invocable_v<PMF, reference_wrapper<X>, int>);
STATIC_ASSERT(!is_nothrow_invocable_v<PMF, reference_wrapper<X>, int>);
STATIC_ASSERT(is_invocable_v<PMF, X*, int>);
STATIC_ASSERT(!is_nothrow_invocable_v<PMF, X*, int>);
STATIC_ASSERT(is_invocable_v<PMF_noexcept, X, int>);
STATIC_ASSERT(is_nothrow_invocable_v<PMF_noexcept, X, int> == noexcept_in_the_type_system);
STATIC_ASSERT(is_invocable_v<PMF_noexcept, reference_wrapper<X>, int>);
STATIC_ASSERT(is_nothrow_invocable_v<PMF_noexcept, reference_wrapper<X>, int> == noexcept_in_the_type_system);
STATIC_ASSERT(is_invocable_v<PMF_noexcept, X*, int>);
STATIC_ASSERT(is_nothrow_invocable_v<PMF_noexcept, X*, int> == noexcept_in_the_type_system);

struct Conv_noexcept1 {
    operator FP() noexcept;
};

struct Conv_noexcept2 {
    operator FP_noexcept();
};

struct Conv_noexcept3 {
    operator FP_noexcept() noexcept;
};

STATIC_ASSERT(is_invocable_v<Conv, int>);
STATIC_ASSERT(!is_nothrow_invocable_v<Conv, int>);
STATIC_ASSERT(is_invocable_v<Conv_noexcept1, int>);
STATIC_ASSERT(!is_nothrow_invocable_v<Conv_noexcept1, int>);
STATIC_ASSERT(is_invocable_v<Conv_noexcept2, int>);
STATIC_ASSERT(!is_nothrow_invocable_v<Conv_noexcept2, int>);
STATIC_ASSERT(is_invocable_v<Conv_noexcept3, int>);
STATIC_ASSERT(is_nothrow_invocable_v<Conv_noexcept3, int> == noexcept_in_the_type_system);

// Test argument conversions.
struct Okay_noexcept {
    operator int() noexcept;
};

STATIC_ASSERT(is_invocable_v<FP_noexcept, Okay>);
STATIC_ASSERT(!is_nothrow_invocable_v<FP_noexcept, Okay>);
STATIC_ASSERT(is_invocable_v<FP_noexcept, Okay_noexcept>);
STATIC_ASSERT(is_nothrow_invocable_v<FP_noexcept, Okay_noexcept> == noexcept_in_the_type_system);

// When the return type is non-void, does the invocation throw?
STATIC_ASSERT(is_invocable_r_v<long, Kitty, int>);
STATIC_ASSERT(is_nothrow_invocable_r_v<long, Kitty, int>);
STATIC_ASSERT(is_invocable_r_v<long, Kitty, int, int>);
STATIC_ASSERT(!is_nothrow_invocable_r_v<long, Kitty, int, int>);

// When the return type is void, does the invocation throw?
STATIC_ASSERT(is_invocable_r_v<void, Kitty, int>);
STATIC_ASSERT(is_nothrow_invocable_r_v<void, Kitty, int>);
STATIC_ASSERT(is_invocable_r_v<void, Kitty, int, int>);
STATIC_ASSERT(!is_nothrow_invocable_r_v<void, Kitty, int, int>);

struct Puppy {
    explicit Puppy(int);
    Puppy(long) noexcept;
};

struct Zebra {
    explicit Zebra(int) noexcept;
    Zebra(long);
};

// When direct-inited from int, Puppy throws and Zebra doesn't.
STATIC_ASSERT(!is_nothrow_constructible_v<Puppy, int>);
STATIC_ASSERT(is_nothrow_constructible_v<Zebra, int>);

// When the return type is non-void, does the conversion throw?
// When copy-inited from int, Puppy doesn't throw and Zebra does.
STATIC_ASSERT(is_invocable_r_v<Puppy, Kitty, int>);
STATIC_ASSERT(is_nothrow_invocable_r_v<Puppy, Kitty, int>);
STATIC_ASSERT(is_invocable_r_v<Zebra, Kitty, int>);
STATIC_ASSERT(!is_nothrow_invocable_r_v<Zebra, Kitty, int>);

#if defined(__clang__) || defined(__EDG__) // TRANSITION, VSO-1026729
// Defend against regression of VSO-963790, in which is_invocable_r mishandles non-movable return types
struct NonMovable {
    NonMovable(NonMovable&&)      = delete;
    NonMovable(const NonMovable&) = delete;
};

template <bool Nothrow>
NonMovable getNonMovable() noexcept(Nothrow);

STATIC_ASSERT(is_invocable_r_v<NonMovable, decltype(&getNonMovable<false>)>);
STATIC_ASSERT(is_invocable_r_v<NonMovable, decltype(&getNonMovable<true>)>);
STATIC_ASSERT(!is_nothrow_invocable_r_v<NonMovable, decltype(&getNonMovable<false>)>);
STATIC_ASSERT(is_nothrow_invocable_r_v<NonMovable, decltype(&getNonMovable<true>)>);

template <bool Nothrow>
struct ConvertsToNonMovable {
    operator NonMovable() const noexcept(Nothrow);
};

template <bool Nothrow, bool NothrowReturn>
ConvertsToNonMovable<NothrowReturn> getConvertsToNonMovable() noexcept(Nothrow);

STATIC_ASSERT(is_invocable_r_v<NonMovable, decltype(&getConvertsToNonMovable<false, false>)>);
STATIC_ASSERT(is_invocable_r_v<NonMovable, decltype(&getConvertsToNonMovable<false, true>)>);
STATIC_ASSERT(is_invocable_r_v<NonMovable, decltype(&getConvertsToNonMovable<true, false>)>);
STATIC_ASSERT(is_invocable_r_v<NonMovable, decltype(&getConvertsToNonMovable<true, true>)>);
STATIC_ASSERT(!is_nothrow_invocable_r_v<NonMovable, decltype(&getConvertsToNonMovable<false, false>)>);
STATIC_ASSERT(!is_nothrow_invocable_r_v<NonMovable, decltype(&getConvertsToNonMovable<false, true>)>);
STATIC_ASSERT(!is_nothrow_invocable_r_v<NonMovable, decltype(&getConvertsToNonMovable<true, false>)>);
STATIC_ASSERT(is_nothrow_invocable_r_v<NonMovable, decltype(&getConvertsToNonMovable<true, true>)>);
#endif // TRANSITION, VSO-1026729
#endif // _HAS_CXX17


// Test:
// LWG-2132 "std::function ambiguity"
// VSO-95177 "<functional>: [expression sfinae] std::function implicit conversions, seems like the compiler stops after
// one level" VSO-108113 "<functional>: [expression sfinae] C++11 compiler unable to distinguish different lambda
// templates in overloaded parameter list"

// GOOD: Same input, same output.
STATIC_ASSERT(is_constructible_v<function<long(short)>, long (*)(short)>);
STATIC_ASSERT(is_assignable_v<function<long(short)>&, long (*)(short)>);

// GOOD: Input conversion, output conversion.
STATIC_ASSERT(is_constructible_v<function<long(short)>, int (*)(int)>);
STATIC_ASSERT(is_assignable_v<function<long(short)>&, int (*)(int)>);

// BAD: Input not convertible.
STATIC_ASSERT(!is_constructible_v<function<long(short)>, long (*)(double*)>);
STATIC_ASSERT(!is_assignable_v<function<long(short)>&, long (*)(double*)>);

// BAD: Output not convertible.
STATIC_ASSERT(!is_constructible_v<function<long(short)>, double* (*) (short)>);
STATIC_ASSERT(!is_assignable_v<function<long(short)>&, double* (*) (short)>);

// SUPER BAD.
STATIC_ASSERT(!is_constructible_v<function<long(short)>, double* (*) (double*)>);
STATIC_ASSERT(!is_assignable_v<function<long(short)>&, double* (*) (double*)>);

// ALSO BAD: void output not convertible to non-void.
STATIC_ASSERT(!is_constructible_v<function<long(short)>, void (*)(short)>);
STATIC_ASSERT(!is_assignable_v<function<long(short)>&, void (*)(short)>);

// ULTRA BAD.
STATIC_ASSERT(!is_constructible_v<function<long(short)>, void (*)(double*)>);
STATIC_ASSERT(!is_assignable_v<function<long(short)>&, void (*)(double*)>);


// GOOD: Same input, same output.
STATIC_ASSERT(is_constructible_v<function<void(short)>, void (*)(short)>);
STATIC_ASSERT(is_assignable_v<function<void(short)>&, void (*)(short)>);

// GOOD: Input conversion, output conversion.
STATIC_ASSERT(is_constructible_v<function<void(short)>, int (*)(int)>);
STATIC_ASSERT(is_assignable_v<function<void(short)>&, int (*)(int)>);

// BAD: Input not convertible.
STATIC_ASSERT(!is_constructible_v<function<void(short)>, void (*)(double*)>);
STATIC_ASSERT(!is_assignable_v<function<void(short)>&, void (*)(double*)>);


// GOOD: Output implicitly convertible.
STATIC_ASSERT(is_constructible_v<function<shared_ptr<const int>()>, shared_ptr<int> (*)()>);
STATIC_ASSERT(is_assignable_v<function<shared_ptr<const int>()>&, shared_ptr<int> (*)()>);

// BAD: Output explicitly convertible, not implicitly.
STATIC_ASSERT(!is_constructible_v<function<bool()>, shared_ptr<int> (*)()>);
STATIC_ASSERT(!is_assignable_v<function<bool()>&, shared_ptr<int> (*)()>);


// std::function invokes a decayed lvalue.
STATIC_ASSERT(is_constructible_v<function<int(int)>, OinkFunctor&>);
STATIC_ASSERT(is_assignable_v<function<int(int)>&, OinkFunctor&>);

STATIC_ASSERT(is_constructible_v<function<int(int)>, const OinkFunctor&>);
STATIC_ASSERT(is_assignable_v<function<int(int)>&, const OinkFunctor&>);

STATIC_ASSERT(is_constructible_v<function<int(int)>, OinkFunctor>);
STATIC_ASSERT(is_assignable_v<function<int(int)>&, OinkFunctor>);


#if _HAS_FUNCTION_ALLOCATOR_SUPPORT
using Tag = allocator_arg_t;
using Al  = allocator<int>;
STATIC_ASSERT(is_constructible_v<function<long(short)>, Tag, Al, long (*)(short)>);
STATIC_ASSERT(is_constructible_v<function<long(short)>, Tag, Al, int (*)(int)>);
STATIC_ASSERT(!is_constructible_v<function<long(short)>, Tag, Al, long (*)(double*)>);
STATIC_ASSERT(!is_constructible_v<function<long(short)>, Tag, Al, double* (*) (short)>);
STATIC_ASSERT(!is_constructible_v<function<long(short)>, Tag, Al, double* (*) (double*)>);
STATIC_ASSERT(!is_constructible_v<function<long(short)>, Tag, Al, void (*)(short)>);
STATIC_ASSERT(!is_constructible_v<function<long(short)>, Tag, Al, void (*)(double*)>);
STATIC_ASSERT(is_constructible_v<function<void(short)>, Tag, Al, void (*)(short)>);
STATIC_ASSERT(is_constructible_v<function<void(short)>, Tag, Al, int (*)(int)>);
STATIC_ASSERT(!is_constructible_v<function<void(short)>, Tag, Al, void (*)(double*)>);
STATIC_ASSERT(is_constructible_v<function<shared_ptr<const int>()>, Tag, Al, shared_ptr<int> (*)()>);
STATIC_ASSERT(!is_constructible_v<function<bool()>, Tag, Al, shared_ptr<int> (*)()>);
STATIC_ASSERT(is_constructible_v<function<int(int)>, Tag, Al, OinkFunctor&>);
STATIC_ASSERT(is_constructible_v<function<int(int)>, Tag, Al, const OinkFunctor&>);
STATIC_ASSERT(is_constructible_v<function<int(int)>, Tag, Al, OinkFunctor>);
#endif // _HAS_FUNCTION_ALLOCATOR_SUPPORT


// Finally, test overload resolution.
int meow(const function<int(int)>& f1) {
    return f1(10);
}

int meow(const function<int(int, int)>& f2) {
    return f2(200, 20);
}

int meow(const function<int(int, int, int)>& f3) {
    return f3(3000, 300, 30);
}

// LWG-2993 "reference_wrapper<T> conversion from T&&"
struct convertible_from_int {
    convertible_from_int(int) {}
};

int lwg_2993(reference_wrapper<int>) {
    return 1;
}

int lwg_2993(convertible_from_int) {
    return 2;
}

int main() {
    assert(meow([](int a) { return a + 5; }) == 15);
    assert(meow([](int i, int j) { return i + j; }) == 220);
    assert(meow([](int x, int y, int z) { return x + y - z; }) == 3270);

    int eleven = 11;
    assert(lwg_2993(ref(eleven)) == 1);
    assert(lwg_2993(0) == 2);
    STATIC_ASSERT(!is_constructible_v<reference_wrapper<double>, double>);
    STATIC_ASSERT(is_constructible_v<reference_wrapper<double>, double&>);
    STATIC_ASSERT(!is_constructible_v<reference_wrapper<double>, int>);
    STATIC_ASSERT(!is_constructible_v<reference_wrapper<double>, int&>);
    STATIC_ASSERT(!is_convertible_v<float, reference_wrapper<float>>);
    STATIC_ASSERT(is_convertible_v<float&, reference_wrapper<float>>);
    STATIC_ASSERT(!is_convertible_v<int, reference_wrapper<float>>);
    STATIC_ASSERT(!is_convertible_v<int&, reference_wrapper<float>>);
}
