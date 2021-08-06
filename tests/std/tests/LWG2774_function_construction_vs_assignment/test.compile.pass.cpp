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

STATIC_ASSERT(!is_constructible_v<function<void()>, const Fn&>);
STATIC_ASSERT(!is_constructible_v<function<void()>, Fn&&>);

int main() {} // COMPILE-ONLY
