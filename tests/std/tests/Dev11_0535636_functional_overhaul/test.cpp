// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _HAS_DEPRECATED_ADAPTOR_TYPEDEFS 1
#define _HAS_DEPRECATED_RESULT_OF        1
#define _SILENCE_CXX17_ADAPTOR_TYPEDEFS_DEPRECATION_WARNING
#define _SILENCE_CXX17_RESULT_OF_DEPRECATION_WARNING

#include <cassert>
#include <cstdlib>
#include <functional>
#include <memory>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <utility>

#if _HAS_CXX17
#include <string_view>
using string_or_view = std::string_view;
#else
using string_or_view = std::string;
#endif

using namespace std;
using namespace std::placeholders;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

// N3936 20.10.7.5 [meta.trans.ptr]:
// Template:
// template <class T> struct remove_pointer;

// Comments:
// If T has type "(possibly cv-qualified) pointer to T1"
// then the member typedef type shall name T1;
// otherwise, it shall name T.

// Test remove_pointer with objects and functions.
STATIC_ASSERT(is_same_v<remove_pointer_t<int>, int>);
STATIC_ASSERT(is_same_v<remove_pointer_t<const int>, const int>);
STATIC_ASSERT(is_same_v<remove_pointer_t<volatile int>, volatile int>);
STATIC_ASSERT(is_same_v<remove_pointer_t<const volatile int>, const volatile int>);
STATIC_ASSERT(is_same_v<remove_pointer_t<int*>, int>);
STATIC_ASSERT(is_same_v<remove_pointer_t<const int*>, const int>);
STATIC_ASSERT(is_same_v<remove_pointer_t<volatile int*>, volatile int>);
STATIC_ASSERT(is_same_v<remove_pointer_t<const volatile int*>, const volatile int>);
STATIC_ASSERT(is_same_v<remove_pointer_t<int* const>, int>);
STATIC_ASSERT(is_same_v<remove_pointer_t<const int* const>, const int>);
STATIC_ASSERT(is_same_v<remove_pointer_t<volatile int* const>, volatile int>);
STATIC_ASSERT(is_same_v<remove_pointer_t<const volatile int* const>, const volatile int>);
STATIC_ASSERT(is_same_v<remove_pointer_t<int* volatile>, int>);
STATIC_ASSERT(is_same_v<remove_pointer_t<const int* volatile>, const int>);
STATIC_ASSERT(is_same_v<remove_pointer_t<volatile int* volatile>, volatile int>);
STATIC_ASSERT(is_same_v<remove_pointer_t<const volatile int* volatile>, const volatile int>);
STATIC_ASSERT(is_same_v<remove_pointer_t<int* const volatile>, int>);
STATIC_ASSERT(is_same_v<remove_pointer_t<const int* const volatile>, const int>);
STATIC_ASSERT(is_same_v<remove_pointer_t<volatile int* const volatile>, volatile int>);
STATIC_ASSERT(is_same_v<remove_pointer_t<const volatile int* const volatile>, const volatile int>);
STATIC_ASSERT(is_same_v<remove_pointer_t<short(long)>, short(long)>);
STATIC_ASSERT(is_same_v<remove_pointer_t<short (*)(long)>, short(long)>);
STATIC_ASSERT(is_same_v<remove_pointer_t<short (*const)(long)>, short(long)>);
STATIC_ASSERT(is_same_v<remove_pointer_t<short (*volatile)(long)>, short(long)>);
STATIC_ASSERT(is_same_v<remove_pointer_t<short (*const volatile)(long)>, short(long)>);


struct X {};

// Test is_member_function_pointer with all combinations of:
// * 0, 1, and 2 parameters
// * cv-qualifiers
// * ref-qualifiers
// * non-variadic and variadic parameters
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)()>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(int)>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(int, int)>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)() const>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(int) const>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(int, int) const>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)() volatile>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(int) volatile>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(int, int) volatile>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)() const volatile>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(int) const volatile>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(int, int) const volatile>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)() &>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(int) &>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(int, int) &>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)() const&>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(int) const&>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(int, int) const&>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)() volatile&>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(int) volatile&>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(int, int) volatile&>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)() const volatile&>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(int) const volatile&>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(int, int) const volatile&>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)() &&>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(int) &&>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(int, int) &&>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)() const&&>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(int) const&&>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(int, int) const&&>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)() volatile&&>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(int) volatile&&>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(int, int) volatile&&>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)() const volatile&&>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(int) const volatile&&>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(int, int) const volatile&&>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(...)>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(int, ...)>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(int, int, ...)>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(...) const>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(int, ...) const>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(int, int, ...) const>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(...) volatile>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(int, ...) volatile>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(int, int, ...) volatile>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(...) const volatile>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(int, ...) const volatile>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(int, int, ...) const volatile>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(...) &>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(int, ...) &>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(int, int, ...) &>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(...) const&>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(int, ...) const&>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(int, int, ...) const&>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(...) volatile&>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(int, ...) volatile&>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(int, int, ...) volatile&>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(...) const volatile&>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(int, ...) const volatile&>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(int, int, ...) const volatile&>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(...) &&>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(int, ...) &&>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(int, int, ...) &&>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(...) const&&>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(int, ...) const&&>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(int, int, ...) const&&>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(...) volatile&&>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(int, ...) volatile&&>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(int, int, ...) volatile&&>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(...) const volatile&&>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(int, ...) const volatile&&>);
STATIC_ASSERT(is_member_function_pointer_v<int (X::*)(int, int, ...) const volatile&&>);

// Test is_member_object_pointer.
STATIC_ASSERT(is_member_object_pointer_v<int X::*>);

// Verify that PMFs aren't detected as PMDs and vice versa.
STATIC_ASSERT(!is_member_function_pointer_v<int X::*>);
STATIC_ASSERT(!is_member_object_pointer_v<int (X::*)(int, int)>);

// Verify that the machinery for LWG-3617 "function/packaged_task deduction guides and deducing this"
// doesn't cause such function pointers to be detected as PMFs.
STATIC_ASSERT(!is_member_function_pointer_v<int (*)(X, int)>);


// N4594 20.13.7.6 [meta.trans.other]:
// Template:
// template <class Fn, class... ArgTypes> struct result_of<Fn(ArgTypes...)>;

// Condition:
// Fn and all types in the parameter pack ArgTypes shall be complete types,
// (possibly cv-qualified) void, or arrays of unknown bound.

// Comments:
// If the expression INVOKE(declval<Fn>(), declval<ArgTypes>()...) is well formed
// when treated as an unevaluated operand (Clause 5), the member typedef type
// shall name the type decltype(INVOKE(declval<Fn>(), declval<ArgTypes>()...));
// otherwise, there shall be no member type. Access checking is performed as if in
// a context unrelated to Fn and ArgTypes. Only the validity of the immediate
// context of the expression is considered. [ Note: The compilation of the
// expression can result in side effects such as the instantiation of class
// template specializations and function template specializations, the generation
// of implicitly-defined functions, and so on. Such side effects are not in the
// "immediate context" and can result in the program being ill-formed. -end note ]

// N4594 20.12.2 [func.require]/1:
// Define INVOKE(f, t1, t2, ..., tN) as follows:
// - (t1.*f)(t2, ..., tN) when f is a pointer to a member function of a class T
//     and is_base_of<T, decay_t<decltype(t1)>>::value is true;
// - (t1.get().*f)(t2, ..., tN) when f is a pointer to a member function of a class T
//     and decay_t<decltype(t1)> is a specialization of reference_wrapper;
// - ((*t1).*f)(t2, ..., tN) when f is a pointer to a member function of a class T
//     and t1 does not satisfy the previous two items;
// - t1.*f when N == 1 and f is a pointer to data member of a class T
//     and is_base_of<T, decay_t<decltype(t1)>>::value is true;
// - t1.get().*f when N == 1 and f is a pointer to data member of a class T
//     and decay_t<decltype(t1)> is a specialization of reference_wrapper;
// - (*t1).*f when N == 1 and f is a pointer to data member of a class T
//     and t1 does not satisfy the previous two items;
// - f(t1, t2, ..., tN) in all other cases.

// Test modifiable/const prvalue/lvalue/xvalue return types.
using PmfModifiablePrvalue = string (X::*)();
using PmfConstPrvalue      = const string (X::*)();
using PmfModifiableLvalue  = string& (X::*) ();
using PmfConstLvalue       = const string& (X::*) ();
using PmfModifiableXvalue  = string && (X::*) ();
using PmfConstXvalue       = const string && (X::*) ();
STATIC_ASSERT(is_same_v<result_of_t<PmfModifiablePrvalue(X&)>, string>);
STATIC_ASSERT(is_same_v<result_of_t<PmfConstPrvalue(X&)>, const string>);
STATIC_ASSERT(is_same_v<result_of_t<PmfModifiableLvalue(X&)>, string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmfConstLvalue(X&)>, const string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmfModifiableXvalue(X&)>, string&&>);
STATIC_ASSERT(is_same_v<result_of_t<PmfConstXvalue(X&)>, const string&&>);

// Test void return types.
using PmfVoid = void (X::*)();
STATIC_ASSERT(is_same_v<result_of_t<PmfVoid(X&)>, void>);

// Test 0, 1, and 2 parameters, also with implicit conversions.
using PmfNullary = float (X::*)();
using PmfUnary   = float* (X::*) (int);
using PmfBinary  = float** (X::*) (int, int);
using PmfString  = double (X::*)(const string&);
STATIC_ASSERT(is_same_v<result_of_t<PmfNullary(X&)>, float>);
STATIC_ASSERT(is_same_v<result_of_t<PmfUnary(X&, int)>, float*>);
STATIC_ASSERT(is_same_v<result_of_t<PmfUnary(X&, short)>, float*>);
STATIC_ASSERT(is_same_v<result_of_t<PmfBinary(X&, int, int)>, float**>);
STATIC_ASSERT(is_same_v<result_of_t<PmfBinary(X&, int, short)>, float**>);
STATIC_ASSERT(is_same_v<result_of_t<PmfBinary(X&, short, int)>, float**>);
STATIC_ASSERT(is_same_v<result_of_t<PmfBinary(X&, short, short)>, float**>);
STATIC_ASSERT(is_same_v<result_of_t<PmfString(X&, string)>, double>);
STATIC_ASSERT(is_same_v<result_of_t<PmfString(X&, const string)>, double>);
STATIC_ASSERT(is_same_v<result_of_t<PmfString(X&, string&)>, double>);
STATIC_ASSERT(is_same_v<result_of_t<PmfString(X&, const string&)>, double>);
STATIC_ASSERT(is_same_v<result_of_t<PmfString(X&, string&&)>, double>);
STATIC_ASSERT(is_same_v<result_of_t<PmfString(X&, const string&&)>, double>);
STATIC_ASSERT(is_same_v<result_of_t<PmfString(X&, const char*)>, double>);

// Test cv-qualifiers.
using PmfPlain         = char (X::*)();
using PmfConst         = short (X::*)() const;
using PmfVolatile      = int (X::*)() volatile;
using PmfConstVolatile = long (X::*)() const volatile;
STATIC_ASSERT(is_same_v<result_of_t<PmfPlain(X&)>, char>);
STATIC_ASSERT(is_same_v<result_of_t<PmfConst(X&)>, short>);
STATIC_ASSERT(is_same_v<result_of_t<PmfConst(const X&)>, short>);
STATIC_ASSERT(is_same_v<result_of_t<PmfVolatile(X&)>, int>);
STATIC_ASSERT(is_same_v<result_of_t<PmfVolatile(volatile X&)>, int>);
STATIC_ASSERT(is_same_v<result_of_t<PmfConstVolatile(X&)>, long>);
STATIC_ASSERT(is_same_v<result_of_t<PmfConstVolatile(const X&)>, long>);
STATIC_ASSERT(is_same_v<result_of_t<PmfConstVolatile(volatile X&)>, long>);
STATIC_ASSERT(is_same_v<result_of_t<PmfConstVolatile(const volatile X&)>, long>);

// Test ref-qualifiers.
using PmfNoRef = short (X::*)();
using PmfLRef  = int (X::*)() &;
using PmfRRef  = long (X::*)() &&;
STATIC_ASSERT(is_same_v<result_of_t<PmfNoRef(X&)>, short>);
STATIC_ASSERT(is_same_v<result_of_t<PmfNoRef(X&&)>, short>);
STATIC_ASSERT(is_same_v<result_of_t<PmfLRef(X&)>, int>);
STATIC_ASSERT(is_same_v<result_of_t<PmfRRef(X&&)>, long>);

// Test varargs.
using PmfEllipsis = double (X::*)(int, ...);
STATIC_ASSERT(is_same_v<result_of_t<PmfEllipsis(X&, int)>, double>);
STATIC_ASSERT(is_same_v<result_of_t<PmfEllipsis(X&, int, int)>, double>);
STATIC_ASSERT(is_same_v<result_of_t<PmfEllipsis(X&, int, int, int)>, double>);
STATIC_ASSERT(is_same_v<result_of_t<PmfEllipsis(X&, int, const char*)>, double>);
STATIC_ASSERT(is_same_v<result_of_t<PmfEllipsis(X&, int, const char*, const char*)>, double>);

// Test base/derived with references, reference_wrappers, raw pointers, and smart pointers.
struct Base {};
struct Derived : Base {};
using PmfBase = bool (Base::*)();
STATIC_ASSERT(is_same_v<result_of_t<PmfBase(Base&)>, bool>);
STATIC_ASSERT(is_same_v<result_of_t<PmfBase(Derived&)>, bool>);
STATIC_ASSERT(is_same_v<result_of_t<PmfBase(reference_wrapper<Base>)>, bool>);
STATIC_ASSERT(is_same_v<result_of_t<PmfBase(reference_wrapper<Derived>)>, bool>);
STATIC_ASSERT(is_same_v<result_of_t<PmfBase(Base*)>, bool>);
STATIC_ASSERT(is_same_v<result_of_t<PmfBase(Derived*)>, bool>);
STATIC_ASSERT(is_same_v<result_of_t<PmfBase(shared_ptr<Base>)>, bool>);
STATIC_ASSERT(is_same_v<result_of_t<PmfBase(shared_ptr<Derived>)>, bool>);

// Test lvalue/rvalue PMFs.
using PmfWhatever = int (X::*)();
STATIC_ASSERT(is_same_v<result_of_t<PmfWhatever(X&)>, int>);
STATIC_ASSERT(is_same_v<result_of_t<PmfWhatever&(X&)>, int>);
STATIC_ASSERT(is_same_v<result_of_t<PmfWhatever && (X&)>, int>);
// const PmfWhatever(X&) triggers Clang -Wignored-qualifiers, and is unnecessary to test.
STATIC_ASSERT(is_same_v<result_of_t<const PmfWhatever&(X&)>, int>);
STATIC_ASSERT(is_same_v<result_of_t<const PmfWhatever && (X&)>, int>);


// N3936 5.5 [expr.mptr.oper]/6:
// The result of a .* expression whose second operand is a pointer to a data member
// is an lvalue if the first operand is an lvalue and an xvalue otherwise.

// N3936 5.5 [expr.mptr.oper]/5:
// The restrictions on cv-qualification, and the manner in which the cv-qualifiers
// of the operands are combined to produce the cv-qualifiers of the result,
// are the same as the rules for E1.E2 given in 5.2.5.

// N3936 5.2.5 [expr.ref]/4:
// If E2 is a non-static data member and the type of E1 is "cq1 vq1 X", and the
// type of E2 is "cq2 vq2 T", the expression designates the named member of the
// object designated by the first expression. If E1 is an lvalue, then E1.E2 is
// an lvalue; otherwise E1.E2 is an xvalue. Let the notation vq12 stand for the
// "union" of vq1 and vq2; that is, if vq1 or vq2 is volatile, then vq12 is
// volatile. Similarly, let the notation cq12 stand for the "union" of cq1 and cq2;
// that is, if cq1 or cq2 is const, then cq12 is const. If E2 is declared to be
// a mutable member, then the type of E1.E2 is "vq12 T". If E2 is not declared to be
// a mutable member, then the type of E1.E2 is "cq12 vq12 T".

// Test PMDs with all combinations of:
// * cv-qualified members
// * lvalue references, rvalue references, reference_wrappers, raw pointers, and smart pointers to objects
// * cv-qualified objects
// * base/derived objects
using PmdPlain         = string Base::*;
using PmdConst         = const string Base::*;
using PmdVolatile      = volatile string Base::*;
using PmdConstVolatile = const volatile string Base::*;
STATIC_ASSERT(is_same_v<result_of_t<PmdPlain(Base&)>, string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdPlain(Base&&)>, string&&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdPlain(reference_wrapper<Base>)>, string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdPlain(Base*)>, string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdPlain(shared_ptr<Base>)>, string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdPlain(const Base&)>, const string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdPlain(const Base&&)>, const string&&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdPlain(reference_wrapper<const Base>)>, const string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdPlain(const Base*)>, const string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdPlain(shared_ptr<const Base>)>, const string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdPlain(volatile Base&)>, volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdPlain(volatile Base&&)>, volatile string&&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdPlain(reference_wrapper<volatile Base>)>, volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdPlain(volatile Base*)>, volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdPlain(shared_ptr<volatile Base>)>, volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdPlain(const volatile Base&)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdPlain(const volatile Base&&)>, const volatile string&&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdPlain(reference_wrapper<const volatile Base>)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdPlain(const volatile Base*)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdPlain(shared_ptr<const volatile Base>)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConst(Base&)>, const string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConst(Base&&)>, const string&&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConst(reference_wrapper<Base>)>, const string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConst(Base*)>, const string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConst(shared_ptr<Base>)>, const string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConst(const Base&)>, const string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConst(const Base&&)>, const string&&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConst(reference_wrapper<const Base>)>, const string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConst(const Base*)>, const string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConst(shared_ptr<const Base>)>, const string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConst(volatile Base&)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConst(volatile Base&&)>, const volatile string&&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConst(reference_wrapper<volatile Base>)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConst(volatile Base*)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConst(shared_ptr<volatile Base>)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConst(const volatile Base&)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConst(const volatile Base&&)>, const volatile string&&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConst(reference_wrapper<const volatile Base>)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConst(const volatile Base*)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConst(shared_ptr<const volatile Base>)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdVolatile(Base&)>, volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdVolatile(Base&&)>, volatile string&&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdVolatile(reference_wrapper<Base>)>, volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdVolatile(Base*)>, volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdVolatile(shared_ptr<Base>)>, volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdVolatile(const Base&)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdVolatile(const Base&&)>, const volatile string&&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdVolatile(reference_wrapper<const Base>)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdVolatile(const Base*)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdVolatile(shared_ptr<const Base>)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdVolatile(volatile Base&)>, volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdVolatile(volatile Base&&)>, volatile string&&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdVolatile(reference_wrapper<volatile Base>)>, volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdVolatile(volatile Base*)>, volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdVolatile(shared_ptr<volatile Base>)>, volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdVolatile(const volatile Base&)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdVolatile(const volatile Base&&)>, const volatile string&&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdVolatile(reference_wrapper<const volatile Base>)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdVolatile(const volatile Base*)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdVolatile(shared_ptr<const volatile Base>)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConstVolatile(Base&)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConstVolatile(Base&&)>, const volatile string&&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConstVolatile(reference_wrapper<Base>)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConstVolatile(Base*)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConstVolatile(shared_ptr<Base>)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConstVolatile(const Base&)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConstVolatile(const Base&&)>, const volatile string&&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConstVolatile(reference_wrapper<const Base>)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConstVolatile(const Base*)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConstVolatile(shared_ptr<const Base>)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConstVolatile(volatile Base&)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConstVolatile(volatile Base&&)>, const volatile string&&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConstVolatile(reference_wrapper<volatile Base>)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConstVolatile(volatile Base*)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConstVolatile(shared_ptr<volatile Base>)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConstVolatile(const volatile Base&)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConstVolatile(const volatile Base&&)>, const volatile string&&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConstVolatile(reference_wrapper<const volatile Base>)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConstVolatile(const volatile Base*)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConstVolatile(shared_ptr<const volatile Base>)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdPlain(Derived&)>, string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdPlain(Derived&&)>, string&&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdPlain(reference_wrapper<Derived>)>, string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdPlain(Derived*)>, string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdPlain(shared_ptr<Derived>)>, string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdPlain(const Derived&)>, const string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdPlain(const Derived&&)>, const string&&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdPlain(reference_wrapper<const Derived>)>, const string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdPlain(const Derived*)>, const string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdPlain(shared_ptr<const Derived>)>, const string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdPlain(volatile Derived&)>, volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdPlain(volatile Derived&&)>, volatile string&&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdPlain(reference_wrapper<volatile Derived>)>, volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdPlain(volatile Derived*)>, volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdPlain(shared_ptr<volatile Derived>)>, volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdPlain(const volatile Derived&)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdPlain(const volatile Derived&&)>, const volatile string&&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdPlain(reference_wrapper<const volatile Derived>)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdPlain(const volatile Derived*)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdPlain(shared_ptr<const volatile Derived>)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConst(Derived&)>, const string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConst(Derived&&)>, const string&&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConst(reference_wrapper<Derived>)>, const string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConst(Derived*)>, const string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConst(shared_ptr<Derived>)>, const string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConst(const Derived&)>, const string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConst(const Derived&&)>, const string&&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConst(reference_wrapper<const Derived>)>, const string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConst(const Derived*)>, const string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConst(shared_ptr<const Derived>)>, const string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConst(volatile Derived&)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConst(volatile Derived&&)>, const volatile string&&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConst(reference_wrapper<volatile Derived>)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConst(volatile Derived*)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConst(shared_ptr<volatile Derived>)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConst(const volatile Derived&)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConst(const volatile Derived&&)>, const volatile string&&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConst(reference_wrapper<const volatile Derived>)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConst(const volatile Derived*)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConst(shared_ptr<const volatile Derived>)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdVolatile(Derived&)>, volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdVolatile(Derived&&)>, volatile string&&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdVolatile(reference_wrapper<Derived>)>, volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdVolatile(Derived*)>, volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdVolatile(shared_ptr<Derived>)>, volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdVolatile(const Derived&)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdVolatile(const Derived&&)>, const volatile string&&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdVolatile(reference_wrapper<const Derived>)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdVolatile(const Derived*)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdVolatile(shared_ptr<const Derived>)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdVolatile(volatile Derived&)>, volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdVolatile(volatile Derived&&)>, volatile string&&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdVolatile(reference_wrapper<volatile Derived>)>, volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdVolatile(volatile Derived*)>, volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdVolatile(shared_ptr<volatile Derived>)>, volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdVolatile(const volatile Derived&)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdVolatile(const volatile Derived&&)>, const volatile string&&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdVolatile(reference_wrapper<const volatile Derived>)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdVolatile(const volatile Derived*)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdVolatile(shared_ptr<const volatile Derived>)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConstVolatile(Derived&)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConstVolatile(Derived&&)>, const volatile string&&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConstVolatile(reference_wrapper<Derived>)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConstVolatile(Derived*)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConstVolatile(shared_ptr<Derived>)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConstVolatile(const Derived&)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConstVolatile(const Derived&&)>, const volatile string&&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConstVolatile(reference_wrapper<const Derived>)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConstVolatile(const Derived*)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConstVolatile(shared_ptr<const Derived>)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConstVolatile(volatile Derived&)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConstVolatile(volatile Derived&&)>, const volatile string&&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConstVolatile(reference_wrapper<volatile Derived>)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConstVolatile(volatile Derived*)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConstVolatile(shared_ptr<volatile Derived>)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConstVolatile(const volatile Derived&)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConstVolatile(const volatile Derived&&)>, const volatile string&&>);
STATIC_ASSERT(
    is_same_v<result_of_t<PmdConstVolatile(reference_wrapper<const volatile Derived>)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConstVolatile(const volatile Derived*)>, const volatile string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdConstVolatile(shared_ptr<const volatile Derived>)>, const volatile string&>);

// Test lvalue/rvalue PMDs.
using PmdWhatever = int X::*;
STATIC_ASSERT(is_same_v<result_of_t<PmdWhatever(X&)>, int&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdWhatever&(X&)>, int&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdWhatever && (X&)>, int&>);
// const PmdWhatever(X&) triggers Clang -Wignored-qualifiers, and is unnecessary to test.
STATIC_ASSERT(is_same_v<result_of_t<const PmdWhatever&(X&)>, int&>);
STATIC_ASSERT(is_same_v<result_of_t<const PmdWhatever && (X&)>, int&>);


// Also test that reference_wrappers are detected after decaying.
STATIC_ASSERT(is_same_v<result_of_t<PmfBase(const reference_wrapper<Base>&)>, bool>);
STATIC_ASSERT(is_same_v<result_of_t<PmfBase(const reference_wrapper<Derived>&)>, bool>);
STATIC_ASSERT(is_same_v<result_of_t<PmdPlain(const reference_wrapper<Base>&)>, string&>);
STATIC_ASSERT(is_same_v<result_of_t<PmdPlain(const reference_wrapper<Derived>&)>, string&>);


// Test different kinds of function objects:
// * Function pointers
// * Classes with function call operators:
//     ** Handwritten plain
//     ** Handwritten overloaded
//     ** Handwritten templated
//     ** Handwritten templated and overloaded
//     ** Stateless lambdas
//     ** Stateful lambdas
//     ** Generic lambdas
// * Classes with conversion operators to function pointers
using FpOrdinary = bool (*)(int, int);

struct ClassOrdinary {
    char operator()(int, int);
};

struct ClassOverloaded {
    float operator()(long);
    double operator()(void*);
};

struct ClassTemplated {
    template <typename T>
    T* operator()(T);
};

struct ClassTemplatedAndOverloaded {
    template <typename T>
    unique_ptr<T> operator()(T);
    template <typename T>
    shared_ptr<T> operator()(T*);
};

struct ClassConversion {
    using FxnPtr = unsigned long (*)(int, int);
    operator FxnPtr();
};

STATIC_ASSERT(is_same_v<result_of_t<FpOrdinary(int, int)>, bool>);
STATIC_ASSERT(is_same_v<result_of_t<ClassOrdinary(int, int)>, char>);
STATIC_ASSERT(is_same_v<result_of_t<ClassOverloaded(long)>, float>);
STATIC_ASSERT(is_same_v<result_of_t<ClassOverloaded(int)>, float>);
STATIC_ASSERT(is_same_v<result_of_t<ClassOverloaded(void*)>, double>);
STATIC_ASSERT(is_same_v<result_of_t<ClassOverloaded(int*)>, double>);
STATIC_ASSERT(is_same_v<result_of_t<ClassTemplated(int)>, int*>);
STATIC_ASSERT(is_same_v<result_of_t<ClassTemplated(long)>, long*>);
STATIC_ASSERT(is_same_v<result_of_t<ClassTemplatedAndOverloaded(char)>, unique_ptr<char>>);
STATIC_ASSERT(is_same_v<result_of_t<ClassTemplatedAndOverloaded(short)>, unique_ptr<short>>);
STATIC_ASSERT(is_same_v<result_of_t<ClassTemplatedAndOverloaded(int*)>, shared_ptr<int>>);
STATIC_ASSERT(is_same_v<result_of_t<ClassTemplatedAndOverloaded(long*)>, shared_ptr<long>>);
STATIC_ASSERT(is_same_v<result_of_t<ClassConversion(int, int)>, unsigned long>);

void test_lambdas() {
    int i = 1729;

    auto lambda_stateless = [](int x) { return x < 5; };
    using LambdaStateless = decltype(lambda_stateless);
    STATIC_ASSERT(is_same_v<result_of_t<LambdaStateless(int)>, bool>);

    auto lambda_ordinary = [&i](int x) { return i + x; };
    using LambdaOrdinary = decltype(lambda_ordinary);
    STATIC_ASSERT(is_same_v<result_of_t<LambdaOrdinary(int)>, int>);

    auto lambda_generic = [&i](auto t) { return i + t; };
    using LambdaGeneric = decltype(lambda_generic);
    STATIC_ASSERT(is_same_v<result_of_t<LambdaGeneric(short)>, int>);
    STATIC_ASSERT(is_same_v<result_of_t<LambdaGeneric(int)>, int>);
    STATIC_ASSERT(is_same_v<result_of_t<LambdaGeneric(long)>, long>);
    STATIC_ASSERT(is_same_v<result_of_t<LambdaGeneric(double)>, double>);
}

// Test modifiable/const prvalue/lvalue/xvalue return types.
using FpModifiablePrvalue = string (*)();
using FpConstPrvalue      = const string (*)();
using FpModifiableLvalue  = string& (*) ();
using FpConstLvalue       = const string& (*) ();
using FpModifiableXvalue  = string && (*) ();
using FpConstXvalue       = const string && (*) ();
STATIC_ASSERT(is_same_v<result_of_t<FpModifiablePrvalue()>, string>);
STATIC_ASSERT(is_same_v<result_of_t<FpConstPrvalue()>, const string>);
STATIC_ASSERT(is_same_v<result_of_t<FpModifiableLvalue()>, string&>);
STATIC_ASSERT(is_same_v<result_of_t<FpConstLvalue()>, const string&>);
STATIC_ASSERT(is_same_v<result_of_t<FpModifiableXvalue()>, string&&>);
STATIC_ASSERT(is_same_v<result_of_t<FpConstXvalue()>, const string&&>);

// Test void return types.
using FpVoid = void (*)();
STATIC_ASSERT(is_same_v<result_of_t<FpVoid()>, void>);

// Test 0, 1, and 2 parameters, also with implicit conversions.
using FpNullary = float (*)();
using FpUnary   = float* (*) (int);
using FpBinary  = float** (*) (int, int);
using FpString  = double (*)(const string&);
STATIC_ASSERT(is_same_v<result_of_t<FpNullary()>, float>);
STATIC_ASSERT(is_same_v<result_of_t<FpUnary(int)>, float*>);
STATIC_ASSERT(is_same_v<result_of_t<FpUnary(short)>, float*>);
STATIC_ASSERT(is_same_v<result_of_t<FpBinary(int, int)>, float**>);
STATIC_ASSERT(is_same_v<result_of_t<FpBinary(int, short)>, float**>);
STATIC_ASSERT(is_same_v<result_of_t<FpBinary(short, int)>, float**>);
STATIC_ASSERT(is_same_v<result_of_t<FpBinary(short, short)>, float**>);
STATIC_ASSERT(is_same_v<result_of_t<FpString(string)>, double>);
STATIC_ASSERT(is_same_v<result_of_t<FpString(const string)>, double>);
STATIC_ASSERT(is_same_v<result_of_t<FpString(string&)>, double>);
STATIC_ASSERT(is_same_v<result_of_t<FpString(const string&)>, double>);
STATIC_ASSERT(is_same_v<result_of_t<FpString(string&&)>, double>);
STATIC_ASSERT(is_same_v<result_of_t<FpString(const string&&)>, double>);
STATIC_ASSERT(is_same_v<result_of_t<FpString(const char*)>, double>);

// Test varargs.
using FpEllipsis = double (*)(int, ...);
STATIC_ASSERT(is_same_v<result_of_t<FpEllipsis(int)>, double>);
STATIC_ASSERT(is_same_v<result_of_t<FpEllipsis(int, int)>, double>);
STATIC_ASSERT(is_same_v<result_of_t<FpEllipsis(int, int, int)>, double>);
STATIC_ASSERT(is_same_v<result_of_t<FpEllipsis(int, const char*)>, double>);
STATIC_ASSERT(is_same_v<result_of_t<FpEllipsis(int, const char*, const char*)>, double>);

// Test cv-qualifiers.
struct MeowPlain {
    char operator()();
};

struct MeowConst {
    short operator()() const;
};

struct MeowVolatile {
    int operator()() volatile;
};

struct MeowConstVolatile {
    long operator()() const volatile;
};

struct MeowOverloaded {
    int operator()();
    int* operator()() const;
    int** operator()() volatile;
    int*** operator()() const volatile;
};

STATIC_ASSERT(is_same_v<result_of_t<MeowPlain&()>, char>);
STATIC_ASSERT(is_same_v<result_of_t<MeowConst&()>, short>);
STATIC_ASSERT(is_same_v<result_of_t<const MeowConst&()>, short>);
STATIC_ASSERT(is_same_v<result_of_t<MeowVolatile&()>, int>);
STATIC_ASSERT(is_same_v<result_of_t<volatile MeowVolatile&()>, int>);
STATIC_ASSERT(is_same_v<result_of_t<MeowConstVolatile&()>, long>);
STATIC_ASSERT(is_same_v<result_of_t<const MeowConstVolatile&()>, long>);
STATIC_ASSERT(is_same_v<result_of_t<volatile MeowConstVolatile&()>, long>);
STATIC_ASSERT(is_same_v<result_of_t<const volatile MeowConstVolatile&()>, long>);
STATIC_ASSERT(is_same_v<result_of_t<MeowOverloaded&()>, int>);
STATIC_ASSERT(is_same_v<result_of_t<const MeowOverloaded&()>, int*>);
STATIC_ASSERT(is_same_v<result_of_t<volatile MeowOverloaded&()>, int**>);
STATIC_ASSERT(is_same_v<result_of_t<const volatile MeowOverloaded&()>, int***>);

// Test ref-qualifiers.
struct KittyNoRef {
    short operator()();
};

struct KittyLRef {
    int operator()() &;
};

struct KittyRRef {
    long operator()() &&;
};

struct KittyOverloaded {
    float operator()() &;
    double operator()() &&;
};

STATIC_ASSERT(is_same_v<result_of_t<KittyNoRef&()>, short>);
STATIC_ASSERT(is_same_v<result_of_t<KittyNoRef && ()>, short>);
STATIC_ASSERT(is_same_v<result_of_t<KittyLRef&()>, int>);
STATIC_ASSERT(is_same_v<result_of_t<KittyRRef && ()>, long>);
STATIC_ASSERT(is_same_v<result_of_t<KittyOverloaded&()>, float>);
STATIC_ASSERT(is_same_v<result_of_t<KittyOverloaded && ()>, double>);

// Test overloaded cv-qualifiers/ref-qualifiers simultaneously.
struct Purr {
    char operator()() &;
    short operator()() const&;
    int operator()() &&;
    long operator()() const&&;
};

STATIC_ASSERT(is_same_v<result_of_t<Purr()>, int>);
STATIC_ASSERT(is_same_v<result_of_t<Purr&()>, char>);
STATIC_ASSERT(is_same_v<result_of_t<Purr && ()>, int>);
STATIC_ASSERT(is_same_v<result_of_t<const Purr()>, long>);
STATIC_ASSERT(is_same_v<result_of_t<const Purr&()>, short>);
STATIC_ASSERT(is_same_v<result_of_t<const Purr && ()>, long>);

// Also test references to functions, DDB-198033.
using FuncRef = int (&)(float, double);
STATIC_ASSERT(is_same_v<result_of_t<FuncRef(float, double)>, int>);


constexpr int triple(int n) {
    return n * 3;
}

constexpr int quadruple(int n) {
    return n * 4;
}

// separate constexpr and non-constexpr functions are TRANSITION, CWG-1129
int square(int n) {
    return n * n;
}

constexpr int square_constexpr(int n) {
    return n * n;
}

int square_noexcept(int n) noexcept {
    return n * n;
}

int cube(int n) {
    return n * n * n;
}

constexpr int cube_constexpr(int n) {
    return n * n * n;
}

int cube_noexcept(int n) noexcept {
    return n * n * n;
}


// Test DevDiv-391117 "<functional> reference_wrapper: reference_wrapper doesn't compile with pure virtual function call
// operators".
struct BaseMeow {
    BaseMeow() {}
    virtual ~BaseMeow() {}

    BaseMeow(const BaseMeow&)            = delete;
    BaseMeow& operator=(const BaseMeow&) = delete;

    virtual int operator()(int, int) = 0;
};

struct DerivedMeow : BaseMeow {
    virtual int operator()(int x, int y) override {
        return x * x * x + y * y * y;
    }
};

void test_dev11_391117() {
    unique_ptr<BaseMeow> upb(new DerivedMeow);

    reference_wrapper<BaseMeow> rwb(*upb);

    assert(rwb(9, 10) == 1729);
}


// Test DevDiv-535636 "<functional> reference_wrapper: reference_wrapper<int (int)>::get() doesn't compile".
void test_dev11_535636() {
    reference_wrapper<int(int)> rw(triple);

    assert(rw(123) == 369);

    int (&rf)(int) = rw.get();
    assert(rf(1729) == 5187);
    assert(&rf == &triple);

    int (&rf2)(int) = rw;
    assert(rf2(64) == 192);
    assert(&rf2 == &triple);
}


// Test DevDiv-794227 "<functional> reference_wrapper: ambiguous access of result_type - functional, xrefwrap".
template <typename Arg, typename Result>
struct UnaryFunction {
    typedef Arg argument_type;
    typedef Result result_type;
};

template <typename Arg1, typename Arg2, typename Result>
struct BinaryFunction {
    typedef Arg1 first_argument_type;
    typedef Arg2 second_argument_type;
    typedef Result result_type;
};

void test_dev11_794227() {
    struct Functor : BinaryFunction<const string&, size_t, bool> {
        result_type operator()(first_argument_type s, second_argument_type n) const {
            return s.size() == n;
        }
    };

#if !_HAS_CXX20
    STATIC_ASSERT(is_same_v<reference_wrapper<Functor>::first_argument_type, const string&>);
    STATIC_ASSERT(is_same_v<reference_wrapper<Functor>::second_argument_type, size_t>);
    STATIC_ASSERT(is_same_v<reference_wrapper<Functor>::result_type, bool>);
#endif // !_HAS_CXX20

    Functor f;
    reference_wrapper<Functor> rw(f);
    const string s("cat");

    assert(rw(s, static_cast<size_t>(3)));
    assert(!rw(s, static_cast<size_t>(4)));

    assert(rw("kitty", static_cast<size_t>(5)));
    assert(!rw("kitty", static_cast<size_t>(6)));
}


// Test DevDiv-868374 "<functional> reference_wrapper: Cannot assign a std::reference_wrapper object to another
// std::reference_wrapper object [libcxx]".
void test_dev11_868374() {
    reference_wrapper<int(int)> rw(triple);
    assert(&rw.get() == &triple);
    assert(rw(11) == 33);

    reference_wrapper<int(int)> other(quadruple);
    rw = other;
    assert(&rw.get() == &quadruple);
    assert(rw(111) == 444);

    rw = ref(square);
    assert(&rw.get() == &square);
    assert(rw(16) == 256);

    rw = cube;
    assert(&rw.get() == &cube);
    assert(rw(7) == 343);

    reference_wrapper<int(int)> rw2(rw);
    assert(&rw2.get() == &cube);
    assert(rw2(9) == 729);

    auto rw3 = ref(rw2);
    STATIC_ASSERT(is_same_v<decltype(rw3), reference_wrapper<int(int)>>);
    assert(&rw3.get() == &cube);
    assert(rw3(12) == 1728);

    // N4140 8.3.5 [dcl.fct]/6:
    // The effect of a cv-qualifier-seq in a function declarator is not the same as adding
    // cv-qualification on top of the function type. In the latter case, the cv-qualifiers are ignored.
    auto rw4 = cref(quadruple);
    STATIC_ASSERT(is_same_v<decltype(rw4), reference_wrapper<int(int)>>);
    assert(&rw4.get() == &quadruple);
    assert(rw4(5) == 20);

    auto rw5 = cref(rw4);
    STATIC_ASSERT(is_same_v<decltype(rw5), reference_wrapper<int(int)>>);
    assert(&rw5.get() == &quadruple);
    assert(rw5(6) == 24);
}


// More reference_wrapper tests.
_CONSTEXPR20 bool test_more_reference_wrapper() {
    STATIC_ASSERT(is_trivially_copyable_v<reference_wrapper<int>>);
    STATIC_ASSERT(is_trivially_copyable_v<reference_wrapper<string>>);
    STATIC_ASSERT(is_trivially_copyable_v<reference_wrapper<int(int, int)>>);
    STATIC_ASSERT(is_trivially_copyable_v<reference_wrapper<string(string, string)>>);

    STATIC_ASSERT(is_same_v<reference_wrapper<int>::type, int>);
    STATIC_ASSERT(is_same_v<reference_wrapper<int(int, int)>::type, int(int, int)>);

    int x = 100;
    reference_wrapper<int> rw(x);
    assert(&rw.get() == &x);
    int& r = rw;
    assert(&r == &x);
    reference_wrapper<int> rw2(rw);
    assert(&rw2.get() == &x);
    int y = 999;
    rw    = ref(y);
    assert(&rw.get() == &y);
    auto rw3 = ref(rw);
    STATIC_ASSERT(is_same_v<decltype(rw3), reference_wrapper<int>>);
    assert(&rw3.get() == &y);
    auto crw = cref(x);
    STATIC_ASSERT(is_same_v<decltype(crw), reference_wrapper<const int>>);
    assert(&crw.get() == &x);
    auto crw2 = cref(rw);
    STATIC_ASSERT(is_same_v<decltype(crw2), reference_wrapper<const int>>);
    assert(&crw2.get() == &y);
    auto crw3 = cref(crw);
    STATIC_ASSERT(is_same_v<decltype(crw3), reference_wrapper<const int>>);
    assert(&crw3.get() == &x);

    struct Cat {};
    struct Dog {
        Cat cat;
        constexpr operator Cat&() {
            return cat;
        }
    };

    Dog dog;
    reference_wrapper<Cat> catref(dog);
    assert(&catref.get() == &dog.cat);

    return true;
}


// Test reference_wrapper's function call operator.
shared_ptr<int> sharer(unique_ptr<int>&& up) {
    return shared_ptr<int>(move(up));
}

unique_ptr<int> uniquer(int a, int b) {
    return make_unique<int>(a * 10 + b);
}

struct Thing {
    int m_x = 1000;
    int m_y = 20;

    constexpr int sum(int z) const {
        return m_x + m_y + z;
    }

    constexpr int sum_noexcept(int z) const noexcept {
        return m_x + m_y + z;
    }

    constexpr int product(int z) const {
        return m_x * m_y * z;
    }
};

class UnaryBinary {
public:
    // Originally for testing Dev10-539137
    // "reference_wrapper: Doesn't handle classes that derive from both unary_function and binary_function".
    // The typedefs are tested elsewhere here (see SameResults and DifferentResults).

    constexpr UnaryBinary() : m_i(0) {}

    constexpr int val() const {
        return m_i;
    }

    constexpr int operator()(const int n) {
        m_i += 7;
        return n * 10;
    }

    constexpr int operator()(const int x, const int y) {
        m_i += 40;
        return x + y;
    }

private:
    int m_i;
};

void test_reference_wrapper_invocation() {
    int i = 10;

    auto lambda = [i](int& r) mutable {
        ++i;
        ++r;
        return i * r;
    };

    int x = 5;

    assert(lambda(x) == 66 && x == 6);
    assert(lambda(x) == 84 && x == 7);
    assert(lambda(x) == 104 && x == 8);

    auto rw_lambda = ref(lambda);

    STATIC_ASSERT(!noexcept(rw_lambda(x)));

    assert(rw_lambda(x) == 126 && x == 9);
    assert(rw_lambda(x) == 150 && x == 10);
    assert(rw_lambda(x) == 176 && x == 11);

    assert(lambda(x) == 204 && x == 12);
    assert(lambda(x) == 234 && x == 13);
    assert(lambda(x) == 266 && x == 14);

    assert(i == 10);

    const auto noexcept_lambda     = []() noexcept {};
    const auto noexcept_lambda_ref = ref(noexcept_lambda);
    STATIC_ASSERT(noexcept(noexcept_lambda_ref())); // strengthened

    reference_wrapper<int(int)> rw_fxn(quadruple);
    assert(rw_fxn(9) == 36);


    int (*fp)(int) = &triple;
    reference_wrapper<int (*)(int)> rw_fp(fp);
    assert(rw_fp(10) == 30);
    fp = &square;
    assert(rw_fp(10) == 100);


    const reference_wrapper<int(int)> rw_fxn2(triple);
    assert(rw_fxn2(5) == 15);
    const reference_wrapper<int (*)(int)> rw_fp2(fp);
    assert(rw_fp2(5) == 25);
    fp = &cube;
    assert(rw_fp2(5) == 125);


    auto sp = make_shared<Thing>();

    int (Thing::*pmf)(int) const = &Thing::sum;
    reference_wrapper<int (Thing::*)(int) const> rw_pmf(pmf);
    assert(rw_pmf(*sp, 709) == 1729);
    assert(rw_pmf(sp.get(), 304) == 1324);
    assert(rw_pmf(sp, 506) == 1526);
    pmf = &Thing::product;
    assert(rw_pmf(sp, 3) == 60000);

    int Thing::*pmd = &Thing::m_x;
    reference_wrapper<int Thing::*> rw_pmd(pmd);
    assert(rw_pmd(*sp) == 1000);
    assert(rw_pmd(sp.get()) == 1000);
    assert(rw_pmd(sp) == 1000);
    rw_pmd(sp) += 5;
    assert(sp->m_x == 1005);
    pmd = &Thing::m_y;
    assert(rw_pmd(sp) == 20);


    auto rw_sharer = ref(sharer);
    auto up5       = make_unique<int>(5);
    auto sp5       = rw_sharer(move(up5));
    assert(!up5);
    STATIC_ASSERT(is_same_v<decltype(sp5), shared_ptr<int>>);
    assert(sp5 && *sp5 == 5);

    auto rw_uniquer = ref(uniquer);
    auto up47       = rw_uniquer(4, 7);
    STATIC_ASSERT(is_same_v<decltype(up47), unique_ptr<int>>);
    assert(up47 && *up47 == 47);


    UnaryBinary ub;
    reference_wrapper<UnaryBinary> rw_ub(ub);
    assert(rw_ub(1729) == 17290);
    assert(rw_ub(3, 5) == 8);
    assert(ub.val() == 47);
}


// Test C++17 invoke().
#if _HAS_CXX17
constexpr bool test_invoke_constexpr() {
    // MSVC implements LWG-2894 in C++17 and later
    Thing thing;
    auto p = &thing;

    assert(&invoke(&Thing::m_x, *p) == &p->m_x);
#if _HAS_CXX20
#if defined(__clang__) || defined(__EDG__) // TRANSITION, DevCom-1419425 "constexpr PMD emits bogus error C2131"
    assert(&invoke(&Thing::m_x, ref(*p)) == &p->m_x);
#endif // ^^^ no workaround ^^^
#endif // _HAS_CXX20
    assert(&invoke(&Thing::m_x, p) == &p->m_x);

#ifndef _M_CEE // TRANSITION, DevCom-939490
    assert(invoke(&Thing::sum, *p, 3) == 1023);
#if _HAS_CXX20
    assert(invoke(&Thing::sum, ref(*p), 4) == 1024);
#endif // _HAS_CXX20
    assert(invoke(&Thing::sum, p, 5) == 1025);
#endif // _M_CEE

    assert(invoke(square_constexpr, 6) == 36);
    assert(invoke(&cube_constexpr, 7) == 343);
    return true;
}
#endif // _HAS_CXX17

void test_invoke() {
#if _HAS_CXX17
    assert(test_invoke_constexpr());
    STATIC_ASSERT(test_invoke_constexpr());
#endif // _HAS_CXX17

    auto sp = make_shared<Thing>();

    assert(&invoke(&Thing::m_x, *sp) == &sp->m_x);
    STATIC_ASSERT(noexcept(&invoke(&Thing::m_x, *sp) == &sp->m_x));
    assert(&invoke(&Thing::m_x, ref(*sp)) == &sp->m_x);
    STATIC_ASSERT(noexcept(&invoke(&Thing::m_x, ref(*sp)) == &sp->m_x));
    assert(&invoke(&Thing::m_x, sp.get()) == &sp->m_x);
    STATIC_ASSERT(noexcept(&invoke(&Thing::m_x, sp.get()) == &sp->m_x));
    assert(&invoke(&Thing::m_x, sp) == &sp->m_x);
    STATIC_ASSERT(noexcept(&invoke(&Thing::m_x, sp) == &sp->m_x));

    assert(invoke(&Thing::sum, *sp, 3) == 1023);
    STATIC_ASSERT(!noexcept(invoke(&Thing::sum, *sp, 3) == 1023));
    assert(invoke(&Thing::sum, ref(*sp), 4) == 1024);
    STATIC_ASSERT(!noexcept(invoke(&Thing::sum, ref(*sp), 4) == 1024));
    assert(invoke(&Thing::sum, sp.get(), 5) == 1025);
    STATIC_ASSERT(!noexcept(invoke(&Thing::sum, sp.get(), 5) == 1025));
    assert(invoke(&Thing::sum, sp, 6) == 1026);
    STATIC_ASSERT(!noexcept(invoke(&Thing::sum, sp, 6) == 1026));

    constexpr bool noexcept_is_in_the_type_system =
#ifdef __cpp_noexcept_function_type
        true
#else
        false
#endif // __cpp_noexcept_function_type
        ;

    assert(invoke(&Thing::sum_noexcept, *sp, 3) == 1023);
    STATIC_ASSERT(noexcept(invoke(&Thing::sum_noexcept, *sp, 3) == 1023) == noexcept_is_in_the_type_system);
    assert(invoke(&Thing::sum_noexcept, ref(*sp), 4) == 1024);
    STATIC_ASSERT(noexcept(invoke(&Thing::sum_noexcept, ref(*sp), 4) == 1024) == noexcept_is_in_the_type_system);
    assert(invoke(&Thing::sum_noexcept, sp.get(), 5) == 1025);
    STATIC_ASSERT(noexcept(invoke(&Thing::sum_noexcept, sp.get(), 5) == 1025) == noexcept_is_in_the_type_system);
    assert(invoke(&Thing::sum_noexcept, sp, 6) == 1026);
    STATIC_ASSERT(noexcept(invoke(&Thing::sum_noexcept, sp, 6) == 1026) == noexcept_is_in_the_type_system);

    assert(invoke(square, 6) == 36);
    STATIC_ASSERT(!noexcept(invoke(square, 6) == 36));
    assert(invoke(&cube, 7) == 343);
    STATIC_ASSERT(!noexcept(invoke(&cube, 7) == 343));

    assert(invoke(square_noexcept, 6) == 36);
    STATIC_ASSERT(noexcept(invoke(square_noexcept, 6) == 36) == noexcept_is_in_the_type_system);
    assert(invoke(&cube_noexcept, 7) == 343);
    STATIC_ASSERT(noexcept(invoke(&cube_noexcept, 7) == 343) == noexcept_is_in_the_type_system);
}


// More tests for reference_wrapper's nested types.
struct None {};

template <typename T, typename = void>
struct GetResultType {
    typedef None type;
};

template <typename T>
struct GetResultType<T, void_t<typename T::result_type>> {
    typedef typename T::result_type type;
};

template <typename T, typename = void>
struct GetArgType {
    typedef None type;
};

template <typename T>
struct GetArgType<T, void_t<typename T::argument_type>> {
    typedef typename T::argument_type type;
};

template <typename T, typename = void>
struct GetFirstType {
    typedef None type;
};

template <typename T>
struct GetFirstType<T, void_t<typename T::first_argument_type>> {
    typedef typename T::first_argument_type type;
};

template <typename T, typename = void>
struct GetSecondType {
    typedef None type;
};

template <typename T>
struct GetSecondType<T, void_t<typename T::second_argument_type>> {
    typedef typename T::second_argument_type type;
};

template <typename T, typename Res, typename Arg, typename First, typename Second>
struct TestTypes
    : conjunction<is_same<typename GetResultType<T>::type, Res>, is_same<typename GetArgType<T>::type, Arg>,
          is_same<typename GetFirstType<T>::type, First>, is_same<typename GetSecondType<T>::type, Second>>::type {};

template <typename T, typename Res, typename Arg, typename First, typename Second>
struct TestRWTypes : conjunction<TestTypes<reference_wrapper<T>, Res, Arg, First, Second>,
                         TestTypes<reference_wrapper<const T>, Res, Arg, First, Second>>::type {};

using Fxn0 = char();
using Fxn1 = short(short*);
using Fxn2 = int(int*, int**);
using Fxn3 = long(long*, long**, long***);

// Avoid triggering a warning when adding constness to function types.
using Pmf0 = float (X::*)();
using Pmf1 = double (X::*)(double*);
using Pmf2 = long double (X::*)(long double*, long double**);

using Pmf0c = unsigned char (X::*)() const;
using Pmf1c = unsigned short (X::*)(unsigned short*) const;
using Pmf2c = unsigned int (X::*)(unsigned int*, unsigned int**) const;

#if _HAS_CXX20
STATIC_ASSERT(TestTypes<reference_wrapper<Fxn0>, None, None, None, None>::value);
STATIC_ASSERT(TestTypes<reference_wrapper<Fxn1>, None, None, None, None>::value);
STATIC_ASSERT(TestTypes<reference_wrapper<Fxn2>, None, None, None, None>::value);
STATIC_ASSERT(TestTypes<reference_wrapper<Fxn3>, None, None, None, None>::value);

STATIC_ASSERT(TestRWTypes<Fxn0*, None, None, None, None>::value);
STATIC_ASSERT(TestRWTypes<Fxn1*, None, None, None, None>::value);
STATIC_ASSERT(TestRWTypes<Fxn2*, None, None, None, None>::value);
STATIC_ASSERT(TestRWTypes<Fxn3*, None, None, None, None>::value);

STATIC_ASSERT(TestRWTypes<Pmf0, None, None, None, None>::value);
STATIC_ASSERT(TestRWTypes<Pmf1, None, None, None, None>::value);
STATIC_ASSERT(TestRWTypes<Pmf2, None, None, None, None>::value);

STATIC_ASSERT(TestRWTypes<Pmf0c, None, None, None, None>::value);
STATIC_ASSERT(TestRWTypes<Pmf1c, None, None, None, None>::value);
STATIC_ASSERT(TestRWTypes<Pmf2c, None, None, None, None>::value);
#else // ^^^ _HAS_CXX20 // !_HAS_CXX20 vvv
STATIC_ASSERT(TestTypes<reference_wrapper<Fxn0>, char, None, None, None>::value);
STATIC_ASSERT(TestTypes<reference_wrapper<Fxn1>, short, short*, None, None>::value);
STATIC_ASSERT(TestTypes<reference_wrapper<Fxn2>, int, None, int*, int**>::value);
STATIC_ASSERT(TestTypes<reference_wrapper<Fxn3>, long, None, None, None>::value);

STATIC_ASSERT(TestRWTypes<Fxn0*, char, None, None, None>::value);
STATIC_ASSERT(TestRWTypes<Fxn1*, short, short*, None, None>::value);
STATIC_ASSERT(TestRWTypes<Fxn2*, int, None, int*, int**>::value);
STATIC_ASSERT(TestRWTypes<Fxn3*, long, None, None, None>::value);

STATIC_ASSERT(TestRWTypes<Pmf0, float, X*, None, None>::value);
STATIC_ASSERT(TestRWTypes<Pmf1, double, None, X*, double*>::value);
STATIC_ASSERT(TestRWTypes<Pmf2, long double, None, None, None>::value);

STATIC_ASSERT(TestRWTypes<Pmf0c, unsigned char, const X*, None, None>::value);
STATIC_ASSERT(TestRWTypes<Pmf1c, unsigned short, None, const X*, unsigned short*>::value);
STATIC_ASSERT(TestRWTypes<Pmf2c, unsigned int, None, None, None>::value);
#endif // _HAS_CXX20

struct OnlyRes {
    typedef bool result_type;
};

struct OnlyArg {
    typedef bool* argument_type;
};

struct OnlyFirst {
    typedef bool** first_argument_type;
};

struct OnlySecond {
    typedef bool*** second_argument_type;
};

struct BothFirstSecond {
    typedef short first_argument_type;
    typedef long second_argument_type;
};

struct NormalOne {
    typedef void* result_type;
    typedef void** argument_type;
};

struct NormalTwo {
    typedef double result_type;
    typedef double* first_argument_type;
    typedef double** second_argument_type;

    result_type operator()(first_argument_type, second_argument_type) const {
        return 3.14;
    }
};

#if _HAS_CXX20
STATIC_ASSERT(TestRWTypes<OnlyRes, None, None, None, None>::value);
STATIC_ASSERT(TestRWTypes<OnlyArg, None, None, None, None>::value);
STATIC_ASSERT(TestRWTypes<OnlyFirst, None, None, None, None>::value);
STATIC_ASSERT(TestRWTypes<OnlySecond, None, None, None, None>::value);
STATIC_ASSERT(TestRWTypes<BothFirstSecond, None, None, None, None>::value);
STATIC_ASSERT(TestRWTypes<NormalOne, None, None, None, None>::value);
STATIC_ASSERT(TestRWTypes<NormalTwo, None, None, None, None>::value);
#else // ^^^ _HAS_CXX20 // !_HAS_CXX20 vvv
STATIC_ASSERT(TestRWTypes<OnlyRes, bool, None, None, None>::value);
STATIC_ASSERT(TestRWTypes<OnlyArg, None, bool*, None, None>::value);
STATIC_ASSERT(TestRWTypes<OnlyFirst, None, None, None, None>::value);
STATIC_ASSERT(TestRWTypes<OnlySecond, None, None, None, None>::value);
STATIC_ASSERT(TestRWTypes<BothFirstSecond, None, None, short, long>::value);
STATIC_ASSERT(TestRWTypes<NormalOne, void*, void**, None, None>::value);
STATIC_ASSERT(TestRWTypes<NormalTwo, double, None, double*, double**>::value);
#endif // _HAS_CXX20

struct Empty {};

STATIC_ASSERT(TestRWTypes<Empty, None, None, None, None>::value);
STATIC_ASSERT(TestRWTypes<int, None, None, None, None>::value);
STATIC_ASSERT(TestRWTypes<int X::*, None, None, None, None>::value);

// Test DevDiv-864867 "<functional> reference_wrapper: reference_wrapper should handle functors that are both unary and
// binary [libs-conformance]".
struct SameResults : UnaryFunction<int, bool>, BinaryFunction<short, long, bool> {

    typedef bool result_type;
};

#if _HAS_CXX20
STATIC_ASSERT(TestRWTypes<SameResults, None, None, None, None>::value);
#else // ^^^ _HAS_CXX20 // !_HAS_CXX20 vvv
STATIC_ASSERT(TestRWTypes<SameResults, bool, int, short, long>::value);
#endif // _HAS_CXX20

struct DifferentResults : UnaryFunction<unsigned int, float>, BinaryFunction<unsigned short, unsigned long, double> {};

#if _HAS_CXX20
STATIC_ASSERT(TestRWTypes<DifferentResults, None, None, None, None>::value);
#else // ^^^ _HAS_CXX20 // !_HAS_CXX20 vvv
STATIC_ASSERT(TestRWTypes<DifferentResults, None, unsigned int, unsigned short, unsigned long>::value);
#endif // _HAS_CXX20

// Test mem_fn().
_CONSTEXPR20 bool test_mem_fn() {
    struct Widget {
        int m_i = 100;

        constexpr int nullary() {
            return ++m_i;
        }
        constexpr int unary(int x) {
            return m_i += x;
        }
        constexpr int binary(int x, int y) {
            return m_i += x * y;
        }
        constexpr int nullary_c() const {
            return m_i * 2;
        }
        constexpr int unary_c(int x) const {
            return m_i * x;
        }
        constexpr int unary_lv(int x) & {
            return m_i += x * x;
        }
        constexpr int unary_rv(int x) && {
            return m_i += x * x * x;
        }
    };

    typedef Widget* Star;
    typedef const Widget* CStar;
    STATIC_ASSERT(TestTypes<decltype(mem_fn(&Widget::m_i)), None, None, None, None>::value);
    STATIC_ASSERT(TestTypes<decltype(mem_fn(&Widget::nullary)), int, Star, None, None>::value);
    STATIC_ASSERT(TestTypes<decltype(mem_fn(&Widget::unary)), int, None, Star, int>::value);
    STATIC_ASSERT(TestTypes<decltype(mem_fn(&Widget::binary)), int, None, None, None>::value);
    STATIC_ASSERT(TestTypes<decltype(mem_fn(&Widget::nullary_c)), int, CStar, None, None>::value);
    STATIC_ASSERT(TestTypes<decltype(mem_fn(&Widget::unary_c)), int, None, CStar, int>::value);
    STATIC_ASSERT(TestTypes<decltype(mem_fn(&Widget::unary_lv)), int, None, Star, int>::value);
    STATIC_ASSERT(TestTypes<decltype(mem_fn(&Widget::unary_rv)), int, None, Star, int>::value);

    Widget w;
    const Widget cw{};

    // Verify that the function call operator is const.
    const auto mf_pmd = mem_fn(&Widget::m_i);

    mf_pmd(w) += 20;
    mf_pmd(&w) += 3;
    assert(w.m_i == 123);

    assert(&mf_pmd(cw) == &cw.m_i);
    assert(&mf_pmd(&cw) == &cw.m_i);

    int&& r1 = mf_pmd(move(w));
    assert(&r1 == &w.m_i);

    const int&& r2 = mf_pmd(move(cw));
    assert(&r2 == &cw.m_i);


    w.m_i = 1000;

    assert(mem_fn(&Widget::nullary)(w) == 1001);
    assert(mem_fn(&Widget::nullary)(&w) == 1002);

    assert(mem_fn(&Widget::unary)(w, 10) == 1012);
    assert(mem_fn(&Widget::unary)(&w, 20) == 1032);

    assert(mem_fn(&Widget::binary)(w, 100, 4) == 1432);
    assert(mem_fn(&Widget::binary)(&w, 100, 5) == 1932);

    w.m_i = 1000;

    assert(mem_fn(&Widget::nullary_c)(w) == 2000);
    assert(mem_fn(&Widget::nullary_c)(&w) == 2000);

    assert(mem_fn(&Widget::unary_c)(w, 3) == 3000);
    assert(mem_fn(&Widget::unary_c)(&w, 4) == 4000);

    assert(mem_fn(&Widget::unary_lv)(w, 5) == 1025);
    assert(mem_fn(&Widget::unary_lv)(&w, 6) == 1061);

    assert(mem_fn(&Widget::unary_rv)(move(w), 7) == 1404);

    return true;
}


// Test std::function.
template <int N, bool NothrowCopyMove = true>
struct CopyMoveCounter {
    int m_copies;
    int m_moves;
    int m_arr[N];

    CopyMoveCounter() noexcept : m_copies(0), m_moves(0) {}

    CopyMoveCounter(const CopyMoveCounter& other) noexcept(NothrowCopyMove)
        : m_copies(other.m_copies + 1), m_moves(other.m_moves) {}

    CopyMoveCounter(CopyMoveCounter&& other) noexcept(NothrowCopyMove)
        : m_copies(other.m_copies), m_moves(other.m_moves + 1) {}

    CopyMoveCounter& operator=(const CopyMoveCounter&) = delete;

    pair<int, int> operator()() const noexcept {
        return make_pair(m_copies, m_moves);
    }
};

short short_long(long n) {
    return n < 100 ? 17 : 29;
}

void test_function() {
    STATIC_ASSERT(TestTypes<function<bool()>, bool, None, None, None>::value);
    STATIC_ASSERT(TestTypes<function<char(short)>, char, short, None, None>::value);
    STATIC_ASSERT(TestTypes<function<long(float, double)>, long, None, float, double>::value);
    STATIC_ASSERT(TestTypes<function<void(int, int, int)>, void, None, None, None>::value);


    {
        function<int(int)> f1;
        function<int(int)> f2 = nullptr;
        function<int(int)> f5 = [](int x) { return x; };

        assert(!f1);
        assert(!f2);
        assert(!!f5);

#if _HAS_FUNCTION_ALLOCATOR_SUPPORT
        function<int(int)> f3(allocator_arg, allocator<short>());
        function<int(int)> f4(allocator_arg, allocator<short>(), nullptr);

        assert(!f3);
        assert(!f4);
#endif // _HAS_FUNCTION_ALLOCATOR_SUPPORT

        assert(f1 == nullptr);
        assert(nullptr == f1);
        assert(!(f1 != nullptr));
        assert(!(nullptr != f1));

        assert(!(f5 == nullptr));
        assert(!(nullptr == f5));
        assert(f5 != nullptr);
        assert(nullptr != f5);

        f1 = nullptr;
        f5 = nullptr;

        assert(!f1);
        assert(!f5);
    }


    // Dev11_617384_empty_std_function covers testing for null function pointers, null member pointers, and empty
    // std::functions.


    // Test DevDiv-759096 "<functional> function: std::function construction copies its target instead of moving".
    {
        CopyMoveCounter<1> cmc0;
        CopyMoveCounter<1> cmc1(cmc0);
        CopyMoveCounter<1> cmc2(cmc1);
        CopyMoveCounter<1> cmc3(move(cmc2));
        CopyMoveCounter<1> cmc4(move(cmc3));
        assert(cmc0() == make_pair(0, 0));
        assert(cmc1() == make_pair(1, 0));
        assert(cmc2() == make_pair(2, 0));
        assert(cmc3() == make_pair(2, 1));
        assert(cmc4() == make_pair(2, 2));

        // The exact number of moves isn't technically guaranteed,
        // but we can rely on copy/move elision here, even without optimizations.
        function<pair<int, int>()> f1 = CopyMoveCounter<1>{};
        function<pair<int, int>()> f2 = CopyMoveCounter<100>{};
        assert(f1() == make_pair(0, 1));
        assert(f2() == make_pair(0, 1));

#if _HAS_FUNCTION_ALLOCATOR_SUPPORT
        function<pair<int, int>()> f3(allocator_arg, allocator<short>(), CopyMoveCounter<1>{});
        function<pair<int, int>()> f4(allocator_arg, allocator<short>(), CopyMoveCounter<100>{});
        assert(f3() == make_pair(0, 1));
        assert(f4() == make_pair(0, 1));
#endif // _HAS_FUNCTION_ALLOCATOR_SUPPORT

        // The following tests are sensitive to our Small Functor Optimization and swap() implementation.
        function<pair<int, int>()> x1 = CopyMoveCounter<1>{};
        function<pair<int, int>()> y1 = CopyMoveCounter<1>{};
        assert(x1() == make_pair(0, 1));
        assert(y1() == make_pair(0, 1));
        x1.swap(y1); // Small functors are swapped with three moves.
        assert(x1() == make_pair(0, 2));
        assert(y1() == make_pair(0, 3));

        function<pair<int, int>()> x2 = CopyMoveCounter<100>{};
        function<pair<int, int>()> y2 = CopyMoveCounter<100>{};
        assert(x2() == make_pair(0, 1));
        assert(y2() == make_pair(0, 1));
        x2.swap(y2); // Large functors are stored remotely, so they're swapped without moves.
        assert(x2() == make_pair(0, 1));
        assert(y2() == make_pair(0, 1));

        function<pair<int, int>()> x3 = CopyMoveCounter<1, false>{};
        function<pair<int, int>()> y3 = CopyMoveCounter<1, false>{};
        assert(x3() == make_pair(0, 1));
        assert(y3() == make_pair(0, 1));
        x3.swap(y3); // Functors with throwing moves are stored remotely, so they're swapped without moves.
        assert(x3() == make_pair(0, 1));
        assert(y3() == make_pair(0, 1));


        function<pair<int, int>()> a1 = CopyMoveCounter<1>{};
        function<pair<int, int>()> b1 = CopyMoveCounter<1>{};
        assert(a1() == make_pair(0, 1));
        assert(b1() == make_pair(0, 1));

        function<pair<int, int>()> c1 = a1;
        function<pair<int, int>()> m1 = move(a1);
        assert(c1() == make_pair(1, 1));
        assert(m1() == make_pair(0, 2));

#if _HAS_FUNCTION_ALLOCATOR_SUPPORT
        function<pair<int, int>()> d1(allocator_arg, allocator<short>(), b1);
        function<pair<int, int>()> n1(allocator_arg, allocator<short>(), move(b1));
        assert(d1() == make_pair(1, 1));
        assert(n1() == make_pair(0, 2));
#endif // _HAS_FUNCTION_ALLOCATOR_SUPPORT

        function<pair<int, int>()> a2 = CopyMoveCounter<100>{};
        function<pair<int, int>()> b2 = CopyMoveCounter<100>{};
        assert(a2() == make_pair(0, 1));
        assert(b2() == make_pair(0, 1));

        function<pair<int, int>()> c2 = a2;
        function<pair<int, int>()> m2 = move(a2);
        assert(c2() == make_pair(1, 1));
        assert(m2() == make_pair(0, 1));

#if _HAS_FUNCTION_ALLOCATOR_SUPPORT
        function<pair<int, int>()> d2(allocator_arg, allocator<short>(), b2);
        function<pair<int, int>()> n2(allocator_arg, allocator<short>(), move(b2));
        assert(d2() == make_pair(1, 1));
        assert(n2() == make_pair(0, 1));
#endif // _HAS_FUNCTION_ALLOCATOR_SUPPORT
    }


    {
        function<int(int)> f = [](int x) { return x * 2; };
        function<int(int)> g = [](int x) { return x * 3; };
        function<int(int)> h = [](int x) { return x * 4; };

        assert(f(11) == 22);

        f = g;
        assert(f(11) == 33);

        f = move(h);
        assert(f(11) == 44);

        f = [](int x) { return x * 5; };
        assert(f(11) == 55);

        auto lambda = [](int x) { return x * 6; };
        auto r      = ref(lambda);
        f           = r;
        assert(f(11) == 66);

        assert(f(100) == 600);
        assert(g(100) == 300);
        f.swap(g);
        assert(f(100) == 300);
        assert(g(100) == 600);
        swap(f, g);
        assert(f(100) == 600);
        assert(g(100) == 300);

#if _HAS_FUNCTION_ALLOCATOR_SUPPORT
        f.assign([](int x) { return x * 7; }, allocator<short>());
        assert(f(11) == 77);
#endif // _HAS_FUNCTION_ALLOCATOR_SUPPORT
    }


    {
        function<int(int)> f;
        const auto& c = f;
        assert(!f);
        assert(f.target_type() == typeid(void));
        assert(f.target<int (*)(int)>() == nullptr);
        assert(c.target<int (*)(int)>() == nullptr);
        assert(f.target<short (*)(long)>() == nullptr);
        assert(c.target<short (*)(long)>() == nullptr);

        f = triple;
        assert(f(1000) == 3000);
        assert(f.target_type() == typeid(int (*)(int)));
        assert(*f.target<int (*)(int)>() == &triple);
        assert(*c.target<int (*)(int)>() == &triple);
        assert(f.target<short (*)(long)>() == nullptr);
        assert(c.target<short (*)(long)>() == nullptr);

        f = short_long;
        assert(f(1000) == 29);
        assert(f.target_type() == typeid(short (*)(long)));
        assert(f.target<int (*)(int)>() == nullptr);
        assert(c.target<int (*)(int)>() == nullptr);
        assert(*f.target<short (*)(long)>() == &short_long);
        assert(*c.target<short (*)(long)>() == &short_long);
    }


    {
        function<void()> f;

        try {
            f();
            abort();
        } catch (const bad_function_call&) {
        }

        f = [] { throw string("kitty"); };

        try {
            f();
            abort();
        } catch (const string& s) {
            assert(s == "kitty");
        }
    }


    {
        function<void(string&, unique_ptr<string>&&, unique_ptr<int>)> f = [](string& lv, unique_ptr<string>&& xv,
                                                                               unique_ptr<int> prv) {
            lv += *xv + to_string(*prv - 1);
            xv = make_unique<string>("woof");
        };
        string s("Cute");
        auto up = make_unique<string>("Kittens");
        f(s, move(up), make_unique<int>(1730));
        assert(s == "CuteKittens1729");
        assert(*up == "woof");

        function<string&()> lv_ret = [&]() -> string& { return s; };
        assert(&lv_ret() == &s);

        function<string && ()> xv_ret = [&]() -> string&& { return move(s); };
        string&& r                    = xv_ret();
        assert(&r == &s);

        function<string()> prv_ret = [&] { return s + "Meow"; };
        assert(prv_ret() == "CuteKittens1729Meow");
    }


    // Test DevDiv-1010027 "<functional> function: std::function with return type void does not ignore return type on
    // assignment".
    {
        string s("ChooseAMovieTitle");

        auto lambda = [&](int x) {
            s += to_string(x);
            return s.c_str();
        };

        function<const char*(int)> f1 = lambda;
        const char* const p           = f1(2013);
        assert(p == s.c_str());
        assert(s == "ChooseAMovieTitle2013");

        s                        = "Orwell";
        function<string(int)> f2 = lambda;
        assert(f2(1984).size() == 10);
        assert(s == "Orwell1984");

        s                      = "AScienceFictionFilm";
        function<void(int)> f3 = lambda;
        f3(2015);
        assert(s == "AScienceFictionFilm2015");

        function<void(int)> f4 = [&](int x) { s = to_string(x * x); };
        f4(16);
        assert(s == "256");

#if 0 // TRANSITION, VSO-243564 "Kitty<const void (int)> emits bogus error LNK1179"
      // Also test LWG-2420 "function<void(ArgTypes...)> does not discard the return value of the target object".
        s = "Pentagon";
        function<const void (int)> f5 = lambda;
        f5(-5);
        assert(s == "Pentagon-5");

        s = "Hexagon";
        function<volatile void (int)> f6 = lambda;
        f6(-6);
        assert(s == "Hexagon-6");

        s = "Septagon";
        function<const volatile void (int)> f7 = lambda;
        f7(-7);
        assert(s == "Septagon-7");
#endif
    }


    // Test DevDiv-294051 "<functional> function: std::function has lost the ability to invoke PMFs/PMDs on various
    // things". Test DevDiv-789899 "<functional> function: std::function does not work for member functions".
    {
        struct Y {
            int m_n;

            int func1(int x) {
                return x + m_n + 1;
            }

            int func2(int x) const {
                return x + m_n + 2;
            }
        };

        Y y{30};
        const Y c{40};

        function<int(Y&, int)> f       = &Y::func1;
        function<int(const Y&, int)> g = &Y::func2;
        function<int(Y&, int)> h       = &Y::func2;

        assert(f(y, 500) == 531);

        assert(g(y, 600) == 632);
        assert(g(c, 700) == 742);

        assert(h(y, 800) == 832);
    }


    {
        // The function call operator must be const.
        const function<int(int)> cf = quadruple;
        assert(cf(10) == 40);

        // swap() must be noexcept.
        struct Puppy {
            int m_i;
            const bool* m_p;

            Puppy(int i, const bool* p) noexcept : m_i(i), m_p(p) {}

            Puppy(const Puppy& other) noexcept(false) : m_i(other.m_i), m_p(other.m_p) {
                if (*m_p) {
                    throw string("BARK");
                }
            }

            Puppy& operator=(const Puppy&) = delete;

            int operator()() const noexcept {
                return m_i;
            }
        };

        bool bark          = false;
        function<int()> f1 = Puppy(300, &bark);
        function<int()> f2 = Puppy(9999, &bark);
        bark               = true;
        assert(f1() == 300);
        assert(f2() == 9999);
        f1.swap(f2);
        assert(f1() == 9999);
        assert(f2() == 300);
        swap(f1, f2);
        assert(f1() == 300);
        assert(f2() == 9999);

        // Self-copy-assign must be a no-op.
        struct SmallFunctor {
            int m_n;

            int operator()() const {
                return m_n * m_n;
            }
        };

        struct LargeFunctor {
            int m_n;
            int m_arr[100];

            explicit LargeFunctor(int n) : m_n(n), m_arr() {}

            int operator()() const {
                return m_n * m_n * m_n;
            }
        };

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
#endif // __clang__

        function<int()> s2 = SmallFunctor{2};
        assert(s2() == 4);
        s2 = s2;
        assert(s2() == 4);

        function<int()> l3 = LargeFunctor{3};
        assert(l3() == 27);
        l3 = l3;
        assert(l3() == 27);

#ifdef __clang__
#pragma clang diagnostic pop
#endif // __clang__

        // Self-swap must be a no-op.
        function<int()> s4 = SmallFunctor{4};
        assert(s4() == 16);
        s4.swap(s4);
        assert(s4() == 16);

        function<int()> l5 = LargeFunctor{5};
        assert(l5() == 125);
        l5.swap(l5);
        assert(l5() == 125);

        // Test small-small, small-large, large-small, and large-large swaps.
        function<int()> s6 = SmallFunctor{6};
        function<int()> l7 = LargeFunctor{7};

        assert(s4() == 16);
        assert(s6() == 36);
        s4.swap(s6);
        assert(s4() == 36);
        assert(s6() == 16);

        assert(s4() == 36);
        assert(l5() == 125);
        s4.swap(l5);
        assert(s4() == 125);
        assert(l5() == 36);
        l5.swap(s4);
        assert(s4() == 36);
        assert(l5() == 125);

        assert(l5() == 125);
        assert(l7() == 343);
        l5.swap(l7);
        assert(l5() == 343);
        assert(l7() == 125);
    }
}


// Test bind(), user-reported bugs.
void test_bind() {
    // Test DDB-176058 "TR1: result_of doesn't accept result_type typedefs for references" (title is now bogus).
    {
        struct PassThru {
            int& operator()(int& obj) const {
                return obj;
            }
        };

        STATIC_ASSERT(is_same_v<result_of_t<PassThru(int&)>, int&>);

        int x = 5;

        bind(PassThru(), ref(x))() += 1;

        assert(x == 6);
    }


    // Test DevDiv-343411 "<functional> bind: bind() and std::function don't work with rvalue references".
    // Test DevDiv-410033 "<functional>: bind() doesn't work with rvalue reference signatures".
    // Test DevDiv-862588 "<functional> bind: std::bind doesn't forward unbound arguments".
    {
#ifndef _M_CEE_PURE

        {auto lambda = [](int x, int y, int&& z) { return x * 100 + y * 10 + z; };

    auto b = bind(lambda, 7, _1, _2);

    function<int(int, int&&)> f(b);

    assert(f(8, 9) == 789);
}

{
    struct Thingy {
        int mf(int&& n) {
            return n * 5;
        }
    };

    Thingy t;

    auto b = bind(&Thingy::mf, t, _1);

    assert(b(7) == 35);
}

{
    auto consume_up = [](unique_ptr<int>&& up, int n) { return *up / n; };

    auto b = bind(consume_up, _1, 3);

    assert(b(make_unique<int>(1000)) == 333);
}

#endif // _M_CEE_PURE
}


// Test DevDiv-487679 "<functional> bind: MSVS 2012 C++ std::bind illegal indirection compiler error".
// Test DevDiv-617421 "<functional> bind: Bind failing to compile with a vector of functions".
{
    struct BaseFunctor {
        int operator()(int n) const {
            return n + 5;
        }
    };

    struct DerivedFunctor : BaseFunctor {};

    auto b = bind(&DerivedFunctor::operator(), _1, 200);

    DerivedFunctor df;

    assert(b(df) == 205);
}


// Test DevDiv-505570 "<functional> bind: Can't bind a pointer to a data member using a pointer, smart pointer or
// iterator to the object".
{
    struct Object {
        int member = 1000;
    };

    auto pmd = &Object::member;
    auto sp  = make_shared<Object>();

    auto b1 = bind(pmd, ref(*sp));
    auto b2 = bind(pmd, sp.get());
    auto b3 = bind(pmd, sp);

    assert(sp->member == 1000);
    ++b1();
    assert(sp->member == 1001);
    ++b2();
    assert(sp->member == 1002);
    ++b3();
    assert(sp->member == 1003);
}


// Test DevDiv-535246 "<functional> bind: Cannot call const forwarding call wrapper result of std::bind".
{
    const auto cb = bind(&quadruple, 11);

    assert(cb() == 44);
}
}


// Unit tests for bind().
struct UserTwo {};

namespace std {
    template <>
    struct is_placeholder<UserTwo> : integral_constant<int, 2> {};
} // namespace std

struct UserBind {
    template <typename T>
    constexpr T operator()(const T& a, const T& b) const {
        return a + b + b;
    }
};

namespace std {
    template <>
    struct is_bind_expression<UserBind> : true_type {};
} // namespace std

template <typename... Types>
struct Pack {};

struct Typewriter {
    template <typename... Types>
    constexpr Pack<Typewriter&, Types&&...> operator()(Types&&...) {
        return Pack<Typewriter&, Types&&...>();
    }

    template <typename... Types>
    constexpr Pack<const Typewriter&, Types&&...> operator()(Types&&...) const {
        return Pack<const Typewriter&, Types&&...>();
    }
};

_CONSTEXPR20 bool test_more_bind() {
    STATIC_ASSERT(is_placeholder_v<int> == 0);

    STATIC_ASSERT(is_placeholder_v<decltype(_1)> == 1);
    STATIC_ASSERT(is_placeholder_v<decltype(_2)> == 2);
    STATIC_ASSERT(is_placeholder_v<decltype(_3)> == 3);
    STATIC_ASSERT(is_placeholder_v<decltype(_4)> == 4);
    STATIC_ASSERT(is_placeholder_v<decltype(_5)> == 5);
    STATIC_ASSERT(is_placeholder_v<decltype(_6)> == 6);
    STATIC_ASSERT(is_placeholder_v<decltype(_7)> == 7);
    STATIC_ASSERT(is_placeholder_v<decltype(_8)> == 8);
    STATIC_ASSERT(is_placeholder_v<decltype(_9)> == 9);
    STATIC_ASSERT(is_placeholder_v<decltype(_10)> == 10);
    STATIC_ASSERT(is_placeholder_v<decltype(_11)> == 11);
    STATIC_ASSERT(is_placeholder_v<decltype(_12)> == 12);
    STATIC_ASSERT(is_placeholder_v<decltype(_13)> == 13);
    STATIC_ASSERT(is_placeholder_v<decltype(_14)> == 14);
    STATIC_ASSERT(is_placeholder_v<decltype(_15)> == 15);
    STATIC_ASSERT(is_placeholder_v<decltype(_16)> == 16);
    STATIC_ASSERT(is_placeholder_v<decltype(_17)> == 17);
    STATIC_ASSERT(is_placeholder_v<decltype(_18)> == 18);
    STATIC_ASSERT(is_placeholder_v<decltype(_19)> == 19);
    STATIC_ASSERT(is_placeholder_v<decltype(_20)> == 20);

    STATIC_ASSERT(is_placeholder_v<UserTwo> == 2);

    STATIC_ASSERT(is_nothrow_default_constructible_v<remove_const_t<decltype(_1)>>);
    STATIC_ASSERT(is_nothrow_copy_constructible_v<remove_const_t<decltype(_1)>>);
    STATIC_ASSERT(is_nothrow_move_constructible_v<remove_const_t<decltype(_1)>>);


    STATIC_ASSERT(!is_bind_expression_v<int>);
    STATIC_ASSERT(is_bind_expression_v<decltype(bind(&triple, 11))>);
    STATIC_ASSERT(is_bind_expression_v<UserBind>);


    STATIC_ASSERT(TestTypes<decltype(bind(square, 5)), int, None, None, None>::value);
    STATIC_ASSERT(TestTypes<decltype(bind(&square, 6)), int, None, None, None>::value);
    STATIC_ASSERT(TestTypes<decltype(bind(declval<PmfString>(), _1, "meow")), double, None, None, None>::value);
    STATIC_ASSERT(TestTypes<decltype(bind(declval<NormalTwo>(), _1, _2)), double, None, None, None>::value);
    STATIC_ASSERT(TestTypes<decltype(bind(declval<ClassOrdinary>(), 10, 20)), None, None, None, None>::value);

    STATIC_ASSERT(TestTypes<decltype(bind<void>(square, 5)), void, None, None, None>::value);
    STATIC_ASSERT(TestTypes<decltype(bind<void>(&square, 6)), void, None, None, None>::value);
    STATIC_ASSERT(TestTypes<decltype(bind<void>(declval<PmfString>(), _1, "meow")), void, None, None, None>::value);
    STATIC_ASSERT(TestTypes<decltype(bind<void>(declval<NormalTwo>(), _1, _2)), void, None, None, None>::value);
    STATIC_ASSERT(TestTypes<decltype(bind<void>(declval<ClassOrdinary>(), 10, 20)), void, None, None, None>::value);


    // Verify that bound arguments are passed as lvalues, so bound functors can be repeatedly invoked.
    auto b1 = bind([](int& r) { return ++r; }, 100);
    assert(b1() == 101);
    assert(b1() == 102);
    assert(b1() == 103);


    // Test placeholders.
    auto lambda = [](int a, int b, int c, int d) { return a * 1000 + b * 100 + c * 10 + d; };

    assert(bind(lambda, _1, _2, _3, _4)(5, 6, 7, 8) == 5678); // All placeholders.
    assert(bind(lambda, _1, _2, 5, 6)(7, 8) == 7856); // Some placeholders at beginning.
    assert(bind(lambda, 5, 6, _1, _2)(7, 8) == 5678); // Some placeholders at end.
    assert(bind(lambda, 6, 7, 8, 9)() == 6789); // No placeholders.
    assert(bind(lambda, _4, _3, _2, _1)(5, 6, 7, 8) == 8765); // Reorder arguments.
    assert(bind(lambda, _1, _2, _2, _1)(5, 6) == 5665); // Replicate arguments.
    assert(bind(lambda, 9, _2, _3, _4)(5, 6, 7, 8) == 9678); // Drop arguments at beginning.
    assert(bind(lambda, _1, 9, _3, _4)(5, 6, 7, 8) == 5978); // Drop arguments at middle.
    assert(bind(lambda, _1, _2, _3, 9)(5, 6, 7, 8) == 5679); // Drop arguments at end.
    assert(bind(lambda, UserTwo(), _1, _2, _3)(5, 6, 7) == 6567); // User placeholder.


    // Test reference_wrapper.
    int n         = 7;
    auto b2       = bind([](int& r) { r *= 2; }, ref(n));
    const auto b3 = b2;

    b2();
    assert(n == 14);
    b3();
    assert(n == 28);


    // Test nested bind.
    auto b4 = bind(plus<>(), bind(plus<>(), bind(multiplies<>(), _1, 100), bind(multiplies<>(), _2, 10)), _3);
    assert(b4(4, 5, 6) == 456);

    auto b5 = bind(multiplies<>(), 10, UserBind());
    assert(b5(30, 4) == 380);


    // Verify that bind()'s function call operator is const-overloaded.
    struct ConstOverloaded {
        constexpr int operator()(int x, int y) {
            return x * x * y;
        }

        constexpr int operator()(int x, int y) const {
            return x * x * x * y;
        }
    };

    auto b6        = bind(ConstOverloaded(), _1, 10);
    const auto& b7 = b6;

    assert(b6(4) == 160);
    assert(b7(4) == 640);


    // Test bind<R>().
    int calls = 0;

    auto lambda2 = [&calls](const char* p, int i) {
        ++calls;
        return p + i;
    };

    const string_or_view s("cute fluffy kittens");

    assert(bind<const char*>(lambda2, s.data(), _1)(2) == s.data() + 2);
    assert(calls == 1);

    assert(bind<string_or_view>(lambda2, s.data(), _1)(3).size() == 16);
    assert(calls == 2);

    bind<void>(lambda2, s.data(), _1)(4);
    assert(calls == 3);


    // Test types carefully.
    struct Z {};
    Z z;
    const Z cz{};

    auto b8 = bind(Typewriter(), X(), ref(n), _1, _2, _3, _4);
    auto p8 = b8(z, cz, move(z), move(cz));
    STATIC_ASSERT(is_same_v<decltype(p8), Pack<Typewriter&, X&, int&, Z&, const Z&, Z&&, const Z&&>>);

    const auto b9 = b8;
    auto p9       = b9(z, cz, move(z), move(cz));
    STATIC_ASSERT(is_same_v<decltype(p9), Pack<const Typewriter&, const X&, int&, Z&, const Z&, Z&&, const Z&&>>);

    auto b10 = bind(Typewriter(), true, bind(Typewriter(), _1, _2, _3, _4), _1, 3.14);
    auto p10 = b10(z, cz, move(z), move(cz));
    STATIC_ASSERT(is_same_v<decltype(p10),
        Pack<Typewriter&, bool&, Pack<Typewriter&, Z&, const Z&, Z&&, const Z&&>&&, Z&, double&>>);

    const auto b11 = b10;
    auto p11       = b11(z, cz, move(z), move(cz));
    STATIC_ASSERT(
        is_same_v<decltype(p11), Pack<const Typewriter&, const bool&,
                                     Pack<const Typewriter&, Z&, const Z&, Z&&, const Z&&>&&, Z&, const double&>>);

#if _HAS_CXX17
    struct NothrowInvocable {
        int operator()(int i) noexcept {
            return i;
        }
    };
    struct NotNothrowInvocable {
        int operator()(int i) {
            return i;
        }
    };
    struct PossiblyThrowingInt {
        int i;
        PossiblyThrowingInt() = default;
        constexpr PossiblyThrowingInt(int j) : i(j) {}
        operator int() {
            return i;
        }
    };
    PossiblyThrowingInt possibly_throwing_int{1729};

    static_assert(is_nothrow_invocable_v<decltype(bind(NothrowInvocable{}, 0))>);
    static_assert(is_nothrow_invocable_v<decltype(bind(NothrowInvocable{}, ref(n)))>);
    static_assert(is_nothrow_invocable_v<decltype(bind(NothrowInvocable{}, _1)), int>);
    static_assert(is_nothrow_invocable_v<decltype(bind<long>(NothrowInvocable{}, 0))>);
    static_assert(is_nothrow_invocable_v<decltype(bind<long>(NothrowInvocable{}, ref(n)))>);
    static_assert(is_nothrow_invocable_v<decltype(bind<long>(NothrowInvocable{}, _1)), int>);
    static_assert(!is_nothrow_invocable_v<decltype(bind<PossiblyThrowingInt>(NothrowInvocable{}, 0))>);
    static_assert(!is_nothrow_invocable_v<decltype(bind<PossiblyThrowingInt>(NothrowInvocable{}, ref(n)))>);
    static_assert(!is_nothrow_invocable_v<decltype(bind<PossiblyThrowingInt>(NothrowInvocable{}, _1)), int>);
    static_assert(!is_nothrow_invocable_v<decltype(bind(NothrowInvocable{}, PossiblyThrowingInt{}))>);
    static_assert(!is_nothrow_invocable_v<decltype(bind(NothrowInvocable{}, ref(possibly_throwing_int)))>);
    static_assert(!is_nothrow_invocable_v<decltype(bind(NothrowInvocable{}, _1)), PossiblyThrowingInt>);
    static_assert(!is_nothrow_invocable_v<decltype(bind<long>(NothrowInvocable{}, PossiblyThrowingInt{}))>);
    static_assert(!is_nothrow_invocable_v<decltype(bind<long>(NothrowInvocable{}, ref(possibly_throwing_int)))>);
    static_assert(!is_nothrow_invocable_v<decltype(bind<long>(NothrowInvocable{}, _1)), PossiblyThrowingInt>);
    static_assert(!is_nothrow_invocable_v<decltype(bind(NotNothrowInvocable{}, 0))>);
    static_assert(!is_nothrow_invocable_v<decltype(bind(NotNothrowInvocable{}, ref(n)))>);
    static_assert(!is_nothrow_invocable_v<decltype(bind(NotNothrowInvocable{}, _1)), int>);
    static_assert(!is_nothrow_invocable_v<decltype(bind<long>(NotNothrowInvocable{}, 0))>);
    static_assert(!is_nothrow_invocable_v<decltype(bind<long>(NotNothrowInvocable{}, ref(n)))>);
    static_assert(!is_nothrow_invocable_v<decltype(bind<long>(NotNothrowInvocable{}, _1)), int>);

    static_assert(is_nothrow_invocable_v<decltype(bind(NothrowInvocable{}, bind(NothrowInvocable{}, _1))), int>);
    static_assert(!is_nothrow_invocable_v<decltype(bind(NothrowInvocable{}, bind(NotNothrowInvocable{}, _1))), int>);
    static_assert(!is_nothrow_invocable_v<decltype(bind(NotNothrowInvocable{}, bind(NothrowInvocable{}, _1))), int>);
    static_assert(!is_nothrow_invocable_v<decltype(bind(NotNothrowInvocable{}, bind(NotNothrowInvocable{}, _1))), int>);
#endif // _HAS_CXX17

    return true;
}


// Test DevDiv-1160769 "<functional>: bind()'s cv-overloaded function call operators are triggering Expression SFINAE
// problems".
struct Test1160769 {
    void method(const int&) {}
};

void test_dev11_1160769() {
    Test1160769 t;
    bind(mem_fn(&Test1160769::method), _1, 1729)(t);
    bind(mem_fn(&Test1160769::method), t, 1729)();
    bind(&Test1160769::method, _1, 1729)(t);
    bind(&Test1160769::method, t, 1729)();
}


// P0005R4 not_fn()
struct BoolWrapper {
    bool wrapped;
};

struct TestNotFn {
    int m_x;

    constexpr explicit TestNotFn(const int x) : m_x(x) {}

    TestNotFn(const TestNotFn&)            = delete;
    TestNotFn(TestNotFn&&)                 = default;
    TestNotFn& operator=(const TestNotFn&) = delete;
    TestNotFn& operator=(TestNotFn&&)      = delete;

    constexpr bool operator()(const int i) & {
        return i < m_x + 100;
    }

    constexpr bool operator()(const int i) const& {
        return i < m_x + 200;
    }

    constexpr bool operator()(const int i) && {
        return i < m_x + 300;
    }

    constexpr bool operator()(const int i) const&& {
        return i < m_x + 400;
    }
};

struct EmptyTestNotFn {
    constexpr explicit EmptyTestNotFn(int) {}

    EmptyTestNotFn(const EmptyTestNotFn&)            = delete;
    constexpr EmptyTestNotFn(EmptyTestNotFn&&)       = default;
    EmptyTestNotFn& operator=(const EmptyTestNotFn&) = delete;
    EmptyTestNotFn& operator=(EmptyTestNotFn&&)      = delete;

    constexpr bool operator()(const int i) & {
        return i < 1500;
    }

    constexpr bool operator()(const int i) const& {
        return i < 2500;
    }

    constexpr bool operator()(const int i) && {
        return i < 3500;
    }

    constexpr bool operator()(const int i) const&& {
        return i < 4500;
    }
};

_CONSTEXPR20 bool test_not_fn() {
#if _HAS_CXX17
    {
        BoolWrapper bw_true{true};
        BoolWrapper bw_false{false};

        auto nf = not_fn(&BoolWrapper::wrapped);

        assert(!nf(bw_true));
        assert(nf(bw_false));
    }

    {
        auto f         = not_fn(TestNotFn{60});
        const auto& cf = f;

        assert(!f(155));
        assert(f(177));
        assert(!cf(255));
        assert(cf(277));
        assert(!move(f)(355));
        assert(move(f)(377));
        assert(!move(cf)(455));
        assert(move(cf)(477));
    }

    {
        auto g         = not_fn(EmptyTestNotFn{-1});
        const auto& cg = g;

        assert(!g(1400));
        assert(g(1600));
        assert(!cg(2400));
        assert(cg(2600));
        assert(!move(g)(3400));
        assert(move(g)(3600));
        assert(!move(cg)(4400));
        assert(move(cg)(4600));
    }
#endif // _HAS_CXX17
    return true;
}

int main() {
    // Test addressof() with functions.
    assert(addressof(triple) == &triple);

    test_dev11_391117();
    test_dev11_535636();
    test_dev11_794227();
    test_dev11_868374();
    test_more_reference_wrapper();
    test_reference_wrapper_invocation();
    test_invoke();
    test_mem_fn();
    test_function();
    test_bind();
    test_more_bind();
    test_dev11_1160769();
    test_not_fn();

#if _HAS_CXX20
    static_assert(test_more_reference_wrapper());
    static_assert(test_mem_fn());
    static_assert(test_more_bind());
    static_assert(test_not_fn());
#endif // _HAS_CXX20
}
