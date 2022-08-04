// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <type_traits>
#include <utility>

using namespace std;

struct U {}; // class type so const-qualification is not stripped from a prvalue
using CU = const U;
using T  = int;
using CT = const T;

U t{};
const U& ct = t;

static_assert(is_same_v<decltype(forward_like<T>(U{})), U&&>);
static_assert(is_same_v<decltype(forward_like<T>(CU{})), CU&&>);
static_assert(is_same_v<decltype(forward_like<T>(t)), U&&>);
static_assert(is_same_v<decltype(forward_like<T>(ct)), CU&&>);
static_assert(is_same_v<decltype(forward_like<T>(move(t))), U&&>);
static_assert(is_same_v<decltype(forward_like<T>(move(ct))), CU&&>);

static_assert(is_same_v<decltype(forward_like<CT>(U{})), CU&&>);
static_assert(is_same_v<decltype(forward_like<CT>(CU{})), CU&&>);
static_assert(is_same_v<decltype(forward_like<CT>(t)), CU&&>);
static_assert(is_same_v<decltype(forward_like<CT>(ct)), CU&&>);
static_assert(is_same_v<decltype(forward_like<CT>(move(t))), CU&&>);
static_assert(is_same_v<decltype(forward_like<CT>(move(ct))), CU&&>);

static_assert(is_same_v<decltype(forward_like<T&>(U{})), U&>);
static_assert(is_same_v<decltype(forward_like<T&>(CU{})), CU&>);
static_assert(is_same_v<decltype(forward_like<T&>(t)), U&>);
static_assert(is_same_v<decltype(forward_like<T&>(ct)), CU&>);
static_assert(is_same_v<decltype(forward_like<T&>(move(t))), U&>);
static_assert(is_same_v<decltype(forward_like<T&>(move(ct))), CU&>);

static_assert(is_same_v<decltype(forward_like<CT&>(U{})), CU&>);
static_assert(is_same_v<decltype(forward_like<CT&>(CU{})), CU&>);
static_assert(is_same_v<decltype(forward_like<CT&>(t)), CU&>);
static_assert(is_same_v<decltype(forward_like<CT&>(ct)), CU&>);
static_assert(is_same_v<decltype(forward_like<CT&>(move(t))), CU&>);
static_assert(is_same_v<decltype(forward_like<CT&>(move(ct))), CU&>);

static_assert(is_same_v<decltype(forward_like<T&&>(U{})), U&&>);
static_assert(is_same_v<decltype(forward_like<T&&>(CU{})), CU&&>);
static_assert(is_same_v<decltype(forward_like<T&&>(t)), U&&>);
static_assert(is_same_v<decltype(forward_like<T&&>(ct)), CU&&>);
static_assert(is_same_v<decltype(forward_like<T&&>(move(t))), U&&>);
static_assert(is_same_v<decltype(forward_like<T&&>(move(ct))), CU&&>);

static_assert(is_same_v<decltype(forward_like<CT&&>(U{})), CU&&>);
static_assert(is_same_v<decltype(forward_like<CT&&>(CU{})), CU&&>);
static_assert(is_same_v<decltype(forward_like<CT&&>(t)), CU&&>);
static_assert(is_same_v<decltype(forward_like<CT&&>(ct)), CU&&>);
static_assert(is_same_v<decltype(forward_like<CT&&>(move(t))), CU&&>);
static_assert(is_same_v<decltype(forward_like<CT&&>(move(ct))), CU&&>);

int main() {} // COMPILE-ONLY
