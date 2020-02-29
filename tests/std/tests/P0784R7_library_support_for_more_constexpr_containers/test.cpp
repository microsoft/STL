// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <assert.h>
#include <memory>
#include <stddef.h>
#include <string.h>
#include <string>
#include <type_traits>
#include <utility>

using namespace std;

template <class Void, class Ty, class... Types>
inline constexpr bool can_construct_at_impl = false;

template <class Ty, class... Types>
inline constexpr bool
    can_construct_at_impl<void_t<decltype(construct_at(declval<Ty*>(), declval<Types>()...))>, Ty, Types...> = true;

template <class Ty, class... Types>
inline constexpr bool can_construct_at = can_construct_at_impl<void, Ty, Types...>;

static_assert(can_construct_at<int>);
static_assert(can_construct_at<const int>);
static_assert(can_construct_at<volatile int>);
static_assert(can_construct_at<const volatile int>);
static_assert(can_construct_at<int, int>);
static_assert(can_construct_at<const int, int>);
static_assert(can_construct_at<volatile int, int>);
static_assert(can_construct_at<const volatile int, int>);
static_assert(can_construct_at<int, int&>);
static_assert(can_construct_at<const int, int&>);
static_assert(can_construct_at<volatile int, int&>);
static_assert(can_construct_at<const volatile int, int&>);

struct X {};

#ifndef __EDG__ // TRANSITION, VSO-1075296
static_assert(!can_construct_at<int, X>);
static_assert(!can_construct_at<X, int>);
#endif // __EDG__

// note that indestructible isn't constructible but is construct_at-ible:
struct indestructible {
    void destroy() {
        this->~indestructible();
    };

private:
    ~indestructible() = default;
};

static_assert(can_construct_at<indestructible>);
static_assert(can_construct_at<const indestructible>);
static_assert(can_construct_at<volatile indestructible>);
static_assert(can_construct_at<const volatile indestructible>);

static_assert(can_construct_at<X>);
static_assert(can_construct_at<X, X>);
static_assert(can_construct_at<X, const X>);
static_assert(can_construct_at<X, const X&>);
static_assert(can_construct_at<X, X&>);

static_assert(can_construct_at<string>);
static_assert(can_construct_at<string, size_t, char>);
static_assert(!can_construct_at<string, size_t, char, char>);
static_assert(!can_construct_at<string, X>);

// The following static_asserts test our strengthening of noexcept

#ifndef __EDG__ // TRANSITION, VSO-1075296
static_assert(noexcept(construct_at(declval<int*>(), 42)));
static_assert(noexcept(construct_at(declval<const int*>(), 42)));
static_assert(noexcept(construct_at(declval<volatile int*>(), 42)));
static_assert(noexcept(construct_at(declval<const volatile int*>(), 42)));
#endif // __EDG__

static_assert(!noexcept(construct_at(declval<string*>(), "hello")));
static_assert(!noexcept(construct_at(declval<const string*>(), "hello")));
static_assert(!noexcept(construct_at(declval<volatile string*>(), "hello")));
static_assert(!noexcept(construct_at(declval<const volatile string*>(), "hello")));

static_assert(noexcept(destroy_at(declval<int*>())));
static_assert(noexcept(destroy_at(declval<string*>())));
static_assert(noexcept(destroy_at(declval<const int*>())));
static_assert(noexcept(destroy_at(declval<const string*>())));
static_assert(noexcept(destroy_at(declval<volatile int*>())));
static_assert(noexcept(destroy_at(declval<volatile string*>())));
static_assert(noexcept(destroy_at(declval<const volatile int*>())));
static_assert(noexcept(destroy_at(declval<const volatile string*>())));

struct throwing_dtor {
    ~throwing_dtor() noexcept(false) {}
};

static_assert(noexcept(destroy_at(declval<throwing_dtor*>())));

template <class Ty>
void test_runtime(const Ty& val) {
    alignas(Ty) unsigned char storage[sizeof(Ty)];
    memset(storage, 42, sizeof(Ty));
    const auto asPtrTy = reinterpret_cast<Ty*>(&storage);
    assert(asPtrTy == construct_at(asPtrTy, val));
    assert(*asPtrTy == val);
    destroy_at(asPtrTy);

    // test ranges:
    assert(asPtrTy == ranges::construct_at(asPtrTy, val));
    assert(*asPtrTy == val);
    destroy_at(asPtrTy);

    // test voidify:
    const auto asCv = static_cast<const volatile Ty*>(asPtrTy);
    assert(asPtrTy == construct_at(asCv, val));
    assert(const_cast<const Ty&>(*asCv) == val);
    destroy_at(asCv);
}

int main() {
    test_runtime(1234);
    test_runtime(string("hello world"));
    test_runtime(string("hello to some really long world that certainly doesn't fit in SSO"));

    {
        alignas(indestructible) unsigned char storage[sizeof(indestructible)];
        const auto ptr = reinterpret_cast<indestructible*>(storage);
        construct_at(ptr);
        ptr->destroy();
    }
}
