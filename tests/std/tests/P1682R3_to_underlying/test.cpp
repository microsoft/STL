// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <type_traits>
#include <utility>

using namespace std;

// TRANSITION, GH-602
// template <class T>
// constexpr bool can_underlying = requires(T e) { to_underlying(e); };

template <class, class = void>
constexpr bool can_underlying = false;
template <class T>
constexpr bool can_underlying<T, void_t<decltype(to_underlying(declval<T>()))>> = true;

enum enum1 : char { a = '1' };
enum class enum2 : int { b = 2 };
enum class enum3 : long { c = 3 };
enum class enum4 { d = 4 };

struct not_an_enum {};

static_assert(can_underlying<enum1>);
static_assert(can_underlying<enum2>);
static_assert(can_underlying<enum3>);
static_assert(can_underlying<enum4>);
static_assert(!can_underlying<not_an_enum>);
static_assert(!can_underlying<int>);

static_assert(is_same_v<decltype(to_underlying(enum1::a)), char>);
static_assert(is_same_v<decltype(to_underlying(enum2::b)), int>);
static_assert(is_same_v<decltype(to_underlying(enum3::c)), long>);
static_assert(is_same_v<decltype(to_underlying(enum4::d)), int>);

static_assert(noexcept(to_underlying(enum1::a)));

constexpr bool test() {
    assert(to_underlying(enum1::a) == '1');
    assert(to_underlying(enum2::b) == 2);
    assert(to_underlying(enum3::c) == 3);
    assert(to_underlying(enum4::d) == 4);
    return true;
}

int main() {
    test();
    static_assert(test());
}
