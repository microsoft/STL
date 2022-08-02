// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <type_traits>
#include <utility>

using namespace std;

struct T {}; // class type so const-qualification is not stripped from a prvalue
using CT = const T;
using U  = int;
using CU = const U;

T t{};
const T& ct = t;

static_assert(is_same_v<decltype(forward_like<U>(T{})), T&&>);
static_assert(is_same_v<decltype(forward_like<U>(CT{})), CT&&>);
static_assert(is_same_v<decltype(forward_like<U>(t)), T&&>);
static_assert(is_same_v<decltype(forward_like<U>(ct)), CT&&>);
static_assert(is_same_v<decltype(forward_like<U>(move(t))), T&&>);
static_assert(is_same_v<decltype(forward_like<U>(move(ct))), CT&&>);

static_assert(is_same_v<decltype(forward_like<CU>(T{})), CT&&>);
static_assert(is_same_v<decltype(forward_like<CU>(CT{})), CT&&>);
static_assert(is_same_v<decltype(forward_like<CU>(t)), CT&&>);
static_assert(is_same_v<decltype(forward_like<CU>(ct)), CT&&>);
static_assert(is_same_v<decltype(forward_like<CU>(move(t))), CT&&>);
static_assert(is_same_v<decltype(forward_like<CU>(move(ct))), CT&&>);

static_assert(is_same_v<decltype(forward_like<U&>(T{})), T&>);
static_assert(is_same_v<decltype(forward_like<U&>(CT{})), CT&>);
static_assert(is_same_v<decltype(forward_like<U&>(t)), T&>);
static_assert(is_same_v<decltype(forward_like<U&>(ct)), CT&>);
static_assert(is_same_v<decltype(forward_like<U&>(move(t))), T&>);
static_assert(is_same_v<decltype(forward_like<U&>(move(ct))), CT&>);

static_assert(is_same_v<decltype(forward_like<CU&>(T{})), CT&>);
static_assert(is_same_v<decltype(forward_like<CU&>(CT{})), CT&>);
static_assert(is_same_v<decltype(forward_like<CU&>(t)), CT&>);
static_assert(is_same_v<decltype(forward_like<CU&>(ct)), CT&>);
static_assert(is_same_v<decltype(forward_like<CU&>(move(t))), CT&>);
static_assert(is_same_v<decltype(forward_like<CU&>(move(ct))), CT&>);

static_assert(is_same_v<decltype(forward_like<U&&>(T{})), T&&>);
static_assert(is_same_v<decltype(forward_like<U&&>(CT{})), CT&&>);
static_assert(is_same_v<decltype(forward_like<U&&>(t)), T&&>);
static_assert(is_same_v<decltype(forward_like<U&&>(ct)), CT&&>);
static_assert(is_same_v<decltype(forward_like<U&&>(move(t))), T&&>);
static_assert(is_same_v<decltype(forward_like<U&&>(move(ct))), CT&&>);

static_assert(is_same_v<decltype(forward_like<CU&&>(T{})), CT&&>);
static_assert(is_same_v<decltype(forward_like<CU&&>(CT{})), CT&&>);
static_assert(is_same_v<decltype(forward_like<CU&&>(t)), CT&&>);
static_assert(is_same_v<decltype(forward_like<CU&&>(ct)), CT&&>);
static_assert(is_same_v<decltype(forward_like<CU&&>(move(t))), CT&&>);
static_assert(is_same_v<decltype(forward_like<CU&&>(move(ct))), CT&&>);

int main() {} // COMPILE-ONLY
