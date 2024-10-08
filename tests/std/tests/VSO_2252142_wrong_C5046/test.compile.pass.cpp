// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <deque>
#include <forward_list>
#include <list>
#include <type_traits>
#include <utility>
#include <vector>

// Test for DevCom-10745303 / VSO-2252142 "C5046 is wrongly triggered in unevaluated context"

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

using namespace std;

template <class T>
struct convertible_to_any {
    operator T() &&; // not defined, only used in unevaluated context
};

template <class Cont, class = void>
constexpr bool has_emplace_back = false;
template <class Cont>
constexpr bool has_emplace_back<Cont,
    void_t<decltype(declval<Cont&>().emplace_back(declval<convertible_to_any<typename Cont::value_type>>()))>> = true;

template <class Cont, class = void>
constexpr bool has_emplace_front = false;
template <class Cont>
constexpr bool has_emplace_front<Cont,
    void_t<decltype(declval<Cont&>().emplace_front(declval<convertible_to_any<typename Cont::value_type>>()))>> = true;

namespace {
    struct S2 {};
} // namespace

// Was emitting "warning C5046: Symbol involving type with internal linkage not defined"
// as a consequence of our use of return type deduction for the pertinent container functions.
STATIC_ASSERT(has_emplace_back<vector<S2>>);
STATIC_ASSERT(has_emplace_back<deque<S2>>);
STATIC_ASSERT(has_emplace_front<deque<S2>>);
STATIC_ASSERT(has_emplace_front<forward_list<S2>>);
STATIC_ASSERT(has_emplace_back<list<S2>>);
STATIC_ASSERT(has_emplace_front<list<S2>>);
STATIC_ASSERT(has_emplace_back<vector<bool>>); // Cannot trigger this bug, but for consistency

// N4988 [queue.defn] and [stack.defn] require the container adaptors to have `decltype(auto) emplace(Args&&... args)`,
// allowing them to adapt both C++14-era and C++17-era containers.
