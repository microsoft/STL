// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cstddef>
#include <iterator>

using namespace std;

struct Test {};
enum Unscoped {};
enum class Scoped {};

// Tests for raw pointers
static_assert(contiguous_iterator<volatile int*>);
static_assert(contiguous_iterator<const volatile int*>);
static_assert(contiguous_iterator<volatile double*>);
static_assert(contiguous_iterator<const volatile double*>);
static_assert(contiguous_iterator<volatile nullptr_t*>);
static_assert(contiguous_iterator<const volatile nullptr_t*>);
static_assert(contiguous_iterator<volatile Unscoped*>);
static_assert(contiguous_iterator<const volatile Unscoped*>);
static_assert(contiguous_iterator<volatile Scoped*>);
static_assert(contiguous_iterator<const volatile Scoped*>);
static_assert(contiguous_iterator<Test* volatile*>);
static_assert(contiguous_iterator<Test* const volatile*>);
static_assert(contiguous_iterator<int Test::*volatile*>);
static_assert(contiguous_iterator<int Test::*const volatile*>);

// Tests for move_iterator specializations
static_assert(random_access_iterator<move_iterator<volatile int*>>);
static_assert(random_access_iterator<move_iterator<const volatile int*>>);
static_assert(random_access_iterator<move_iterator<volatile double*>>);
static_assert(random_access_iterator<move_iterator<const volatile double*>>);
static_assert(random_access_iterator<move_iterator<volatile nullptr_t*>>);
static_assert(random_access_iterator<move_iterator<const volatile nullptr_t*>>);
static_assert(random_access_iterator<move_iterator<volatile Unscoped*>>);
static_assert(random_access_iterator<move_iterator<const volatile Unscoped*>>);
static_assert(random_access_iterator<move_iterator<volatile Scoped*>>);
static_assert(random_access_iterator<move_iterator<const volatile Scoped*>>);
static_assert(random_access_iterator<move_iterator<Test* volatile*>>);
static_assert(random_access_iterator<move_iterator<Test* const volatile*>>);
static_assert(random_access_iterator<move_iterator<int Test::*volatile*>>);
static_assert(random_access_iterator<move_iterator<int Test::*const volatile*>>);
