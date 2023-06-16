// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <functional>
#include <type_traits>

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

using namespace std;

void test() { // COMPILE-ONLY
    {
        auto lambda = [](int) {};
        auto f      = bind(lambda, placeholders::_1);
        STATIC_ASSERT(!is_convertible_v<decltype(f), function<void()>>);
        STATIC_ASSERT(!is_convertible_v<decltype(f), function<void*(int)>>);
        STATIC_ASSERT(is_convertible_v<decltype(f), function<void(int)>>);
        STATIC_ASSERT(!is_convertible_v<decltype(f), function<char(int)>>);
    }
    {
        auto lambda = [](int) { return 42; };
        auto f      = bind<void>(lambda, placeholders::_1);
        STATIC_ASSERT(!is_convertible_v<decltype(f), function<void()>>);
        STATIC_ASSERT(!is_convertible_v<decltype(f), function<void*(int)>>);
        STATIC_ASSERT(is_convertible_v<decltype(f), function<void(int)>>);
        STATIC_ASSERT(!is_convertible_v<decltype(f), function<char(int)>>);
    }
    {
        auto lambda = [](int) { return 42; };
        auto f      = bind<long>(lambda, placeholders::_1);
        STATIC_ASSERT(!is_convertible_v<decltype(f), function<void()>>);
        STATIC_ASSERT(!is_convertible_v<decltype(f), function<void*(int)>>);
        STATIC_ASSERT(is_convertible_v<decltype(f), function<void(int)>>);
        STATIC_ASSERT(is_convertible_v<decltype(f), function<char(int)>>);
    }
    {
        auto lambda0 = [](int, int) { return true; };
        auto lambda1 = [](const char*) { return 0; };
        auto f       = bind(lambda0, placeholders::_1, bind(lambda1, placeholders::_2));
        STATIC_ASSERT(!is_convertible_v<decltype(f), function<void()>>);
        STATIC_ASSERT(!is_convertible_v<decltype(f), function<void*(int, const char*)>>);
        STATIC_ASSERT(is_convertible_v<decltype(f), function<void(int, const char*)>>);
        STATIC_ASSERT(is_convertible_v<decltype(f), function<bool(int, const char*)>>);
    }
}

// Also test expression-equivalence for the invocation on the return types of bind, bind_front, and bind_back

#if _HAS_CXX17
struct WeirdIdentity {
    template <class T>
    void operator()(T&&) = delete;

    template <class T>
    constexpr T&& operator()(T&& t) const noexcept {
        return static_cast<T&&>(t);
    }
};

static_assert(
    is_invocable_v<WeirdIdentity&, int> == is_invocable_v<decltype(bind(WeirdIdentity{}, placeholders::_1))&, int>);
static_assert(
    is_invocable_v<WeirdIdentity&, int> == is_invocable_v<decltype(bind(WeirdIdentity{}, placeholders::_1)), int>);
static_assert(is_invocable_v<const WeirdIdentity&, int>
              == is_invocable_v<const decltype(bind(WeirdIdentity{}, placeholders::_1))&, int>);
static_assert(is_invocable_v<const WeirdIdentity&, int>
              == is_invocable_v<const decltype(bind(WeirdIdentity{}, placeholders::_1))&, int>);

static_assert(is_invocable_r_v<void, WeirdIdentity&, int>
              == is_invocable_v<decltype(bind<void>(WeirdIdentity{}, placeholders::_1))&, int>);
static_assert(is_invocable_r_v<void, WeirdIdentity&, int>
              == is_invocable_v<decltype(bind<void>(WeirdIdentity{}, placeholders::_1)), int>);
static_assert(is_invocable_r_v<void, const WeirdIdentity&, int>
              == is_invocable_v<const decltype(bind<void>(WeirdIdentity{}, placeholders::_1))&, int>);
static_assert(is_invocable_r_v<void, const WeirdIdentity&, int>
              == is_invocable_v<const decltype(bind<void>(WeirdIdentity{}, placeholders::_1))&, int>);

static_assert(is_invocable_r_v<int, WeirdIdentity&, int>
              == is_invocable_v<decltype(bind<int>(WeirdIdentity{}, placeholders::_1))&, int>);
static_assert(is_invocable_r_v<int, WeirdIdentity&, int>
              == is_invocable_v<decltype(bind<int>(WeirdIdentity{}, placeholders::_1)), int>);
static_assert(is_invocable_r_v<int, const WeirdIdentity&, int>
              == is_invocable_v<const decltype(bind<int>(WeirdIdentity{}, placeholders::_1))&, int>);
static_assert(is_invocable_r_v<int, const WeirdIdentity&, int>
              == is_invocable_v<const decltype(bind<int>(WeirdIdentity{}, placeholders::_1))&, int>);
#endif // _HAS_CXX17

#if _HAS_CXX20
struct WeirdDual {
    template <class T, class U>
    void operator()(T&&, U&&) & = delete;

    template <class T, class U>
    constexpr void operator()(T&&, U&&) const& noexcept {}

    template <class T, class U>
    constexpr void operator()(T&&, U&&) && noexcept {}

    template <class T, class U>
    constexpr void operator()(T&&, U&&) const&& = delete;
};

static_assert(is_invocable_v<WeirdDual&, int, long> == is_invocable_v<decltype(bind_front(WeirdDual{}, 0))&, long>);
static_assert(
    is_invocable_v<const WeirdDual&, int, long> == is_invocable_v<const decltype(bind_front(WeirdDual{}, 0))&, long>);
static_assert(is_invocable_v<WeirdDual, int, long> == is_invocable_v<decltype(bind_front(WeirdDual{}, 0)), long>);
static_assert(
    is_invocable_v<const WeirdDual, int, long> == is_invocable_v<const decltype(bind_front(WeirdDual{}, 0)), long>);

#if _HAS_CXX23
static_assert(is_invocable_v<WeirdDual&, int, long> == is_invocable_v<decltype(bind_back(WeirdDual{}, 0L))&, long>);
static_assert(
    is_invocable_v<const WeirdDual&, int, long> == is_invocable_v<const decltype(bind_front(WeirdDual{}, 0L))&, long>);
static_assert(is_invocable_v<WeirdDual, int, long> == is_invocable_v<decltype(bind_front(WeirdDual{}, 0L)), long>);
static_assert(
    is_invocable_v<const WeirdDual, int, long> == is_invocable_v<const decltype(bind_front(WeirdDual{}, 0L)), long>);
#endif // _HAS_CXX23
#endif // _HAS_CXX20
