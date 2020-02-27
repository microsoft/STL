// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <assert.h>
#include <memory>
#include <string.h>
#include <string>
#include <utility>

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

template <class Ty>
void test_runtime(const Ty& _Val) {
    alignas(Ty) unsigned char storage[sizeof(Ty)];
    memset(storage, 42, sizeof(Ty));
    const auto asPtrTy = reinterpret_cast<Ty*>(&storage);
    std::construct_at(asPtrTy, _Val);
    assert(*asPtrTy == _Val);
    std::destroy_at(asPtrTy);

    // test voidify:
    const auto asCv = static_cast<const volatile Ty*>(asPtrTy);
    std::construct_at(asCv, _Val);
    assert(const_cast<const Ty&>(*asCv) == _Val);
    std::destroy_at(asCv);
}

int main() {
    test_runtime(1234);
    test_runtime(std::string("hello world"));
    test_runtime(std::string("hello to some really long world that certainly doesn't fit in SSO"));
}
