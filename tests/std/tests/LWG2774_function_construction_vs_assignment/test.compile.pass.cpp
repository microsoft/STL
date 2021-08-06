// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <functional>
#include <type_traits>

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

struct Fn {
    Fn(const Fn&) = delete;
    Fn(Fn&&)      = delete;

    void operator()() const {}
};

template <typename, typename...>
struct detector : false_type {};

template <typename T, typename... Args>
struct detector<T, void_t<decltype(T(declval<Args>()...))>, Args...> : true_type {};

template <typename Arg>
using can_instantiate = detector<function<void()>, Arg>;

STATIC_ASSERT(!can_instantiate<const Fn&>::value);
STATIC_ASSERT(!can_instantiate<Fn&&>::value);

int main() {} // COMPILE-ONLY
