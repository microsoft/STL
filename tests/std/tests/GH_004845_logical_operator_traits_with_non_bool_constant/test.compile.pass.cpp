// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// GH-4845 <type_traits>: Logical operator traits with non-bool_constant arguments emit truncation warnings

#include <type_traits>
using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

template <class Base, class Derived>
void test_base_derived() {
    STATIC_ASSERT(is_base_of_v<Base, Derived>);
    using ValueType = typename Base::value_type;
    STATIC_ASSERT(is_same_v<const ValueType, decltype(Derived::type::value)>);
    STATIC_ASSERT(Base::value == Derived::type::value);
    STATIC_ASSERT(is_same_v<const ValueType, decltype(Derived::value)>);
    STATIC_ASSERT(Base::value == Derived::value);
}

template <short N>
using SC = integral_constant<short, N>;
template <long N>
using LC = integral_constant<long, N>;

// This test is verifying the absence of truncation warnings, so it uses integral values of 2 or greater for "true"
// values, skipping 1. It also tests these values in both the "first" and "next" positions, as they were both affected.

void test_conjunction() {
    // N4986 [meta.logical]/5:
    // The specialization conjunction<B1, ..., BN> has a public and unambiguous base that is either
    // - the first type Bi in the list true_type, B1, ..., BN for which bool(Bi::value) is false, or
    // - if there is no such Bi, the last type in the list.
    test_base_derived<LC<3>, conjunction<SC<2>, LC<3>>>();
    test_base_derived<LC<0>, conjunction<SC<4>, LC<0>>>();
    test_base_derived<SC<0>, conjunction<SC<0>, LC<5>>>();
    test_base_derived<SC<0>, conjunction<SC<0>, LC<0>>>();
}

void test_disjunction() {
    // N4986 [meta.logical]/10:
    // The specialization disjunction<B1, ..., BN> has a public and unambiguous base that is either
    // - the first type Bi in the list false_type, B1, ..., BN for which bool(Bi::value) is true, or
    // - if there is no such Bi, the last type in the list.
    test_base_derived<SC<6>, disjunction<SC<6>, LC<7>>>();
    test_base_derived<SC<8>, disjunction<SC<8>, LC<0>>>();
    test_base_derived<LC<9>, disjunction<SC<0>, LC<9>>>();
    test_base_derived<LC<0>, disjunction<SC<0>, LC<0>>>();
}

void test_negation() {
    // N4986 [meta.logical]/12:
    // The class template negation forms the logical negation of its template type argument.
    // The type negation<B> is a Cpp17UnaryTypeTrait with a base characteristic of bool_constant<!bool(B::value)>.
    test_base_derived<false_type, negation<SC<1729>>>();
    test_base_derived<true_type, negation<SC<0>>>();
}
