// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// some portions of this file are derived from libc++'s test files:
// * std/utilities/function.objects/func.bind_front/bind_front.pass.cpp

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
              == is_invocable_v<const decltype(bind(WeirdIdentity{}, placeholders::_1)), int>);

static_assert(is_invocable_r_v<void, WeirdIdentity&, int>
              == is_invocable_v<decltype(bind<void>(WeirdIdentity{}, placeholders::_1))&, int>);
static_assert(is_invocable_r_v<void, WeirdIdentity&, int>
              == is_invocable_v<decltype(bind<void>(WeirdIdentity{}, placeholders::_1)), int>);
static_assert(is_invocable_r_v<void, const WeirdIdentity&, int>
              == is_invocable_v<const decltype(bind<void>(WeirdIdentity{}, placeholders::_1))&, int>);
static_assert(is_invocable_r_v<void, const WeirdIdentity&, int>
              == is_invocable_v<const decltype(bind<void>(WeirdIdentity{}, placeholders::_1)), int>);

static_assert(is_invocable_r_v<int, WeirdIdentity&, int>
              == is_invocable_v<decltype(bind<int>(WeirdIdentity{}, placeholders::_1))&, int>);
static_assert(is_invocable_r_v<int, WeirdIdentity&, int>
              == is_invocable_v<decltype(bind<int>(WeirdIdentity{}, placeholders::_1)), int>);
static_assert(is_invocable_r_v<int, const WeirdIdentity&, int>
              == is_invocable_v<const decltype(bind<int>(WeirdIdentity{}, placeholders::_1))&, int>);
static_assert(is_invocable_r_v<int, const WeirdIdentity&, int>
              == is_invocable_v<const decltype(bind<int>(WeirdIdentity{}, placeholders::_1)), int>);
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

// derived from libc++'s test file
void test_bind_front_deletion() {
    // Make sure the bind_front unspecified-type is NOT invocable when the call would select a differently-qualified
    // operator().
    //
    // For example, if the call to `operator()() &` is ill-formed, the call to the unspecified-type
    // should be ill-formed and not fall back to the `operator()() const&` overload.
    {
        // Make sure we delete the & overload when the underlying call isn't valid
        struct F {
            void operator()() & = delete;
            void operator()() const& {}
            void operator()() && {}
            void operator()() const&& {}
        };
        using X = decltype(bind_front(F{}));
        static_assert(!is_invocable_v<X&>);
        static_assert(is_invocable_v<X const&>);
        static_assert(is_invocable_v<X>);
        static_assert(is_invocable_v<X const>);
    }

    // There's no way to make sure we delete the const& overload when the underlying call isn't valid,
    // so we can't check this one.

    // Make sure we delete the && overload when the underlying call isn't valid
    {
        struct F {
            void operator()() & {}
            void operator()() const& {}
            void operator()() && = delete;
            void operator()() const&& {}
        };
        using X = decltype(bind_front(F{}));
        static_assert(is_invocable_v<X&>);
        static_assert(is_invocable_v<X const&>);
        static_assert(!is_invocable_v<X>);
        static_assert(is_invocable_v<X const>);
    }

    // Make sure we delete the const&& overload when the underlying call isn't valid
    {
        struct F {
            void operator()() & {}
            void operator()() const& {}
            void operator()() && {}
            void operator()() const&& = delete;
        };
        using X = decltype(bind_front(F{}));
        static_assert(is_invocable_v<X&>);
        static_assert(is_invocable_v<X const&>);
        static_assert(is_invocable_v<X>);
        static_assert(!is_invocable_v<X const>);
    }

    // Some examples by Tim Song
    {
        struct T {};
        struct F {
            void operator()(T&&) const& {}
            void operator()(T&&) && = delete;
        };
        using X = decltype(bind_front(F{}));
        static_assert(!is_invocable_v<X, T>);
    }

    {
        struct T {};
        struct F {
            void operator()(T const&) const {}
            void operator()(T&&) const = delete;
        };
        using X = decltype(bind_front(F{}, T{}));
        static_assert(!is_invocable_v<X>);
    }
}
#endif // _HAS_CXX20

#if _HAS_CXX23
static_assert(is_invocable_v<WeirdDual&, int, long> == is_invocable_v<decltype(bind_back(WeirdDual{}, 0L))&, int>);
static_assert(
    is_invocable_v<const WeirdDual&, int, long> == is_invocable_v<const decltype(bind_back(WeirdDual{}, 0L))&, int>);
static_assert(is_invocable_v<WeirdDual, int, long> == is_invocable_v<decltype(bind_back(WeirdDual{}, 0L)), int>);
static_assert(
    is_invocable_v<const WeirdDual, int, long> == is_invocable_v<const decltype(bind_back(WeirdDual{}, 0L)), int>);

// derived from libc++'s test file
void test_bind_back_deletion() {
    // Make sure the bind_back unspecified-type is NOT invocable when the call would select a differently-qualified
    // operator().
    //
    // For example, if the call to `operator()() &` is ill-formed, the call to the unspecified-type
    // should be ill-formed and not fall back to the `operator()() const&` overload.
    {
        // Make sure we delete the & overload when the underlying call isn't valid
        struct F {
            void operator()() & = delete;
            void operator()() const& {}
            void operator()() && {}
            void operator()() const&& {}
        };
        using X = decltype(bind_back(F{}));
        static_assert(!is_invocable_v<X&>);
        static_assert(is_invocable_v<X const&>);
        static_assert(is_invocable_v<X>);
        static_assert(is_invocable_v<X const>);
    }

    // There's no way to make sure we delete the const& overload when the underlying call isn't valid,
    // so we can't check this one.

    // Make sure we delete the && overload when the underlying call isn't valid
    {
        struct F {
            void operator()() & {}
            void operator()() const& {}
            void operator()() && = delete;
            void operator()() const&& {}
        };
        using X = decltype(bind_back(F{}));
        static_assert(is_invocable_v<X&>);
        static_assert(is_invocable_v<X const&>);
        static_assert(!is_invocable_v<X>);
        static_assert(is_invocable_v<X const>);
    }

    // Make sure we delete the const&& overload when the underlying call isn't valid
    {
        struct F {
            void operator()() & {}
            void operator()() const& {}
            void operator()() && {}
            void operator()() const&& = delete;
        };
        using X = decltype(bind_back(F{}));
        static_assert(is_invocable_v<X&>);
        static_assert(is_invocable_v<X const&>);
        static_assert(is_invocable_v<X>);
        static_assert(!is_invocable_v<X const>);
    }

    // Some examples by Tim Song
    {
        struct T {};
        struct F {
            void operator()(T&&) const& {}
            void operator()(T&&) && = delete;
        };
        using X = decltype(bind_back(F{}));
        static_assert(!is_invocable_v<X, T>);
    }

    {
        struct T {};
        struct F {
            void operator()(T const&) const {}
            void operator()(T&&) const = delete;
        };
        using X = decltype(bind_back(F{}, T{}));
        static_assert(!is_invocable_v<X>);
    }
}
#endif // _HAS_CXX23
