// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cmath>
#include <type_traits>

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

constexpr long double ld = 10.0l;
constexpr double d       = 10.0;
constexpr float f        = 10.0f;
constexpr long long ll   = 2;
constexpr int i          = 2;
constexpr short s        = 2;

STATIC_ASSERT(std::is_same_v<decltype(std::pow(ld, ld)), long double>);
STATIC_ASSERT(std::is_same_v<decltype(std::pow(ld, d)), long double>);
STATIC_ASSERT(std::is_same_v<decltype(std::pow(ld, f)), long double>);
STATIC_ASSERT(std::is_same_v<decltype(std::pow(ld, ll)), long double>);
STATIC_ASSERT(std::is_same_v<decltype(std::pow(ld, i)), long double>);
STATIC_ASSERT(std::is_same_v<decltype(std::pow(ld, s)), long double>);

STATIC_ASSERT(std::is_same_v<decltype(std::pow(d, ld)), long double>);
STATIC_ASSERT(std::is_same_v<decltype(std::pow(d, d)), double>);
STATIC_ASSERT(std::is_same_v<decltype(std::pow(d, f)), double>);
STATIC_ASSERT(std::is_same_v<decltype(std::pow(d, ll)), double>);
STATIC_ASSERT(std::is_same_v<decltype(std::pow(d, i)), double>);
STATIC_ASSERT(std::is_same_v<decltype(std::pow(d, s)), double>);

STATIC_ASSERT(std::is_same_v<decltype(std::pow(f, ld)), long double>);
STATIC_ASSERT(std::is_same_v<decltype(std::pow(f, d)), double>);
STATIC_ASSERT(std::is_same_v<decltype(std::pow(f, f)), float>);
STATIC_ASSERT(std::is_same_v<decltype(std::pow(f, ll)), double>);
STATIC_ASSERT(std::is_same_v<decltype(std::pow(f, i)), double>);
STATIC_ASSERT(std::is_same_v<decltype(std::pow(f, s)), double>);

STATIC_ASSERT(std::is_same_v<decltype(std::pow(ll, ld)), long double>);
STATIC_ASSERT(std::is_same_v<decltype(std::pow(ll, d)), double>);
STATIC_ASSERT(std::is_same_v<decltype(std::pow(ll, f)), double>);
STATIC_ASSERT(std::is_same_v<decltype(std::pow(ll, ll)), double>);
STATIC_ASSERT(std::is_same_v<decltype(std::pow(ll, i)), double>);
STATIC_ASSERT(std::is_same_v<decltype(std::pow(ll, s)), double>);

STATIC_ASSERT(std::is_same_v<decltype(std::pow(i, ld)), long double>);
STATIC_ASSERT(std::is_same_v<decltype(std::pow(i, d)), double>);
STATIC_ASSERT(std::is_same_v<decltype(std::pow(i, f)), double>);
STATIC_ASSERT(std::is_same_v<decltype(std::pow(i, ll)), double>);
STATIC_ASSERT(std::is_same_v<decltype(std::pow(i, i)), double>);
STATIC_ASSERT(std::is_same_v<decltype(std::pow(i, s)), double>);

STATIC_ASSERT(std::is_same_v<decltype(std::pow(s, ld)), long double>);
STATIC_ASSERT(std::is_same_v<decltype(std::pow(s, d)), double>);
STATIC_ASSERT(std::is_same_v<decltype(std::pow(s, f)), double>);
STATIC_ASSERT(std::is_same_v<decltype(std::pow(s, ll)), double>);
STATIC_ASSERT(std::is_same_v<decltype(std::pow(s, i)), double>);
STATIC_ASSERT(std::is_same_v<decltype(std::pow(s, s)), double>);
