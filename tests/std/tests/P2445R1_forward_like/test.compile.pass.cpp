// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <type_traits>
#include <utility>

using namespace std;

struct U {}; // class type so const-qualification is not stripped from a prvalue
using CU = const U;
using T  = int;
using CT = const T;

U u{};
const U& cu = u;

static_assert(is_same_v<decltype(forward_like<T>(U{})), U&&>);
static_assert(is_same_v<decltype(forward_like<T>(CU{})), CU&&>);
static_assert(is_same_v<decltype(forward_like<T>(u)), U&&>);
static_assert(is_same_v<decltype(forward_like<T>(cu)), CU&&>);
static_assert(is_same_v<decltype(forward_like<T>(move(u))), U&&>);
static_assert(is_same_v<decltype(forward_like<T>(move(cu))), CU&&>);

static_assert(is_same_v<decltype(forward_like<CT>(U{})), CU&&>);
static_assert(is_same_v<decltype(forward_like<CT>(CU{})), CU&&>);
static_assert(is_same_v<decltype(forward_like<CT>(u)), CU&&>);
static_assert(is_same_v<decltype(forward_like<CT>(cu)), CU&&>);
static_assert(is_same_v<decltype(forward_like<CT>(move(u))), CU&&>);
static_assert(is_same_v<decltype(forward_like<CT>(move(cu))), CU&&>);

static_assert(is_same_v<decltype(forward_like<T&>(U{})), U&>);
static_assert(is_same_v<decltype(forward_like<T&>(CU{})), CU&>);
static_assert(is_same_v<decltype(forward_like<T&>(u)), U&>);
static_assert(is_same_v<decltype(forward_like<T&>(cu)), CU&>);
static_assert(is_same_v<decltype(forward_like<T&>(move(u))), U&>);
static_assert(is_same_v<decltype(forward_like<T&>(move(cu))), CU&>);

static_assert(is_same_v<decltype(forward_like<CT&>(U{})), CU&>);
static_assert(is_same_v<decltype(forward_like<CT&>(CU{})), CU&>);
static_assert(is_same_v<decltype(forward_like<CT&>(u)), CU&>);
static_assert(is_same_v<decltype(forward_like<CT&>(cu)), CU&>);
static_assert(is_same_v<decltype(forward_like<CT&>(move(u))), CU&>);
static_assert(is_same_v<decltype(forward_like<CT&>(move(cu))), CU&>);

static_assert(is_same_v<decltype(forward_like<T&&>(U{})), U&&>);
static_assert(is_same_v<decltype(forward_like<T&&>(CU{})), CU&&>);
static_assert(is_same_v<decltype(forward_like<T&&>(u)), U&&>);
static_assert(is_same_v<decltype(forward_like<T&&>(cu)), CU&&>);
static_assert(is_same_v<decltype(forward_like<T&&>(move(u))), U&&>);
static_assert(is_same_v<decltype(forward_like<T&&>(move(cu))), CU&&>);

static_assert(is_same_v<decltype(forward_like<CT&&>(U{})), CU&&>);
static_assert(is_same_v<decltype(forward_like<CT&&>(CU{})), CU&&>);
static_assert(is_same_v<decltype(forward_like<CT&&>(u)), CU&&>);
static_assert(is_same_v<decltype(forward_like<CT&&>(cu)), CU&&>);
static_assert(is_same_v<decltype(forward_like<CT&&>(move(u))), CU&&>);
static_assert(is_same_v<decltype(forward_like<CT&&>(move(cu))), CU&&>);

static_assert(noexcept(forward_like<T>(u)));

constexpr bool test() {
    int val       = 1729;
    auto&& result = forward_like<const double&>(val);
    static_assert(is_same_v<decltype(result), const int&>);
    assert(&result == &val);
    return true;
}

static_assert(test());

int main() {} // COMPILE-ONLY
