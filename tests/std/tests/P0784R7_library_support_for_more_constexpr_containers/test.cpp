// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <assert.h>
#include <memory>
#include <string.h>
#include <string>
#include <type_traits>
#include <utility>

struct X {};

template <class _Void, class Ty, class... Types>
inline constexpr bool can_construct_at = false;

template <class Ty, class... Types>
inline constexpr bool can_construct_at<
    std::void_t<decltype(std::construct_at(std::declval<Ty*>(), std::declval<Types>()...))>, Ty, Types...> = true;

static_assert(can_construct_at<void, int>);
static_assert(can_construct_at<void, const int>);
static_assert(can_construct_at<void, volatile int>);
static_assert(can_construct_at<void, const volatile int>);
static_assert(can_construct_at<void, int, int>);
static_assert(can_construct_at<void, const int, int>);
static_assert(can_construct_at<void, volatile int, int>);
static_assert(can_construct_at<void, const volatile int, int>);
static_assert(can_construct_at<void, int, int&>);
static_assert(can_construct_at<void, const int, int&>);
static_assert(can_construct_at<void, volatile int, int&>);
static_assert(can_construct_at<void, const volatile int, int&>);

static_assert(!can_construct_at<void, int, X>);
static_assert(!can_construct_at<void, X, int>);

static_assert(!can_construct_at<void, int&>); // note that references can be constructed by not new'd
static_assert(!can_construct_at<void, const int&>);
static_assert(!can_construct_at<void, volatile int&>);
static_assert(!can_construct_at<void, const volatile int&>);
static_assert(!can_construct_at<void, int&, int>);
static_assert(!can_construct_at<void, const int&, int>);
static_assert(!can_construct_at<void, volatile int&, int>);
static_assert(!can_construct_at<void, const volatile int&, int>);
static_assert(!can_construct_at<void, int&, int&>);
static_assert(!can_construct_at<void, const int&, const int&>);
static_assert(!can_construct_at<void, volatile int&, volatile int&>);
static_assert(!can_construct_at<void, const volatile int&, const volatile int&>);

static_assert(can_construct_at<void, X>);
static_assert(can_construct_at<void, X, X>);
static_assert(can_construct_at<void, X, const X&>);
static_assert(can_construct_at<void, X, X&>);
static_assert(can_construct_at<void, X, X&&>);

static_assert(can_construct_at<void, std::string>);
static_assert(can_construct_at<void, std::string, std::size_t, char>);
static_assert(!can_construct_at<void, std::string, std::size_t, char, char>);
static_assert(!can_construct_at<void, std::string, X>);

static_assert(noexcept(std::construct_at(std::declval<int*>(), 42)));
static_assert(!noexcept(std::construct_at(std::declval<std::string*>(), "hello")));
static_assert(noexcept(std::construct_at(std::declval<const int*>(), 42)));
static_assert(!noexcept(std::construct_at(std::declval<const std::string*>(), "hello")));
static_assert(noexcept(std::construct_at(std::declval<volatile int*>(), 42)));
static_assert(!noexcept(std::construct_at(std::declval<volatile std::string*>(), "hello")));
static_assert(noexcept(std::construct_at(std::declval<const volatile int*>(), 42)));
static_assert(!noexcept(std::construct_at(std::declval<const volatile std::string*>(), "hello")));

static_assert(noexcept(std::destroy_at(std::declval<int*>())));
static_assert(noexcept(std::destroy_at(std::declval<std::string*>())));
static_assert(noexcept(std::destroy_at(std::declval<const int*>())));
static_assert(noexcept(std::destroy_at(std::declval<const std::string*>())));
static_assert(noexcept(std::destroy_at(std::declval<volatile int*>())));
static_assert(noexcept(std::destroy_at(std::declval<volatile std::string*>())));
static_assert(noexcept(std::destroy_at(std::declval<const volatile int*>())));
static_assert(noexcept(std::destroy_at(std::declval<const volatile std::string*>())));

struct throwing_dtor {
    ~throwing_dtor() noexcept(false) {}
};

static_assert(noexcept(std::destroy_at(std::declval<throwing_dtor*>())));

template <class Ty>
void test_runtime(const Ty& val) {
    alignas(Ty) unsigned char storage[sizeof(Ty)];
    memset(storage, 42, sizeof(Ty));
    const auto asPtrTy = reinterpret_cast<Ty*>(&storage);
    assert(asPtrTy == std::construct_at(asPtrTy, val));
    assert(*asPtrTy == val);
    std::destroy_at(asPtrTy);

    // test ranges:
    assert(asPtrTy == std::ranges::construct_at(asPtrTy, val));
    assert(*asPtrTy == val);
    std::destroy_at(asPtrTy);

    // test voidify:
    const auto asCv = static_cast<const volatile Ty*>(asPtrTy);
    assert(asPtrTy == std::construct_at(asCv, val));
    assert(const_cast<const Ty&>(*asCv) == val);
    std::destroy_at(asCv);
}

int main() {
    test_runtime(1234);
    test_runtime(std::string("hello world"));
    test_runtime(std::string("hello to some really long world that certainly doesn't fit in SSO"));
}
