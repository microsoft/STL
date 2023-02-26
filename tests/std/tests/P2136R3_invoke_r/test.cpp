// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <functional>
#include <string>
#include <type_traits>

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

using namespace std;

// TRANSITION, DevCom-1457457
namespace detail {
    static constexpr bool permissive() {
        return false;
    }

    template <class>
    struct DependentBase {
        static constexpr bool permissive() {
            return true;
        }
    };

    template <class T>
    struct Derived : DependentBase<T> {
        static constexpr bool test() {
            return permissive();
        }
    };
} // namespace detail
constexpr bool is_permissive = detail::Derived<int>::test();

constexpr int square(int n) {
    return n * n;
}
constexpr int square_noexcept(int n) noexcept {
    return n * n;
}
constexpr const char* cstring() noexcept;

struct Thing {
    int n = 0;
    constexpr int& moo() {
        return n;
    }
};

struct RefQualified {
    constexpr int operator()(int&&) && {
        return 1;
    }
    constexpr int operator()(const int&) && {
        return 2;
    }
    constexpr int operator()(int&&) & {
        return 3;
    }
    constexpr int operator()(const int&) & {
        return 4;
    }
};

constexpr bool test_invoke_r() {
    auto v1 = invoke_r<long>(square, 3);
    assert(v1 == 9L);
    STATIC_ASSERT(is_same_v<decltype(v1), long>);

    auto v2 = invoke_r<double>([]() -> int { return 5; });
    assert(v2 == 5);
    STATIC_ASSERT(is_same_v<decltype(v2), double>);
    STATIC_ASSERT(is_void_v<decltype(invoke_r<void>(square, 1))>);

    // TRANSITION, DevCom-1457457
    STATIC_ASSERT(noexcept(invoke_r<int>(square, 3)) == is_permissive);
    STATIC_ASSERT(noexcept(invoke(square, 3)) == is_permissive);

    constexpr bool has_noexcept_in_type =
#ifdef __cpp_noexcept_function_type
        true;
#else
        false;
#endif
    STATIC_ASSERT(noexcept(invoke(square_noexcept, 3)) == has_noexcept_in_type);
    STATIC_ASSERT(noexcept(invoke_r<int>(square_noexcept, 3)) == has_noexcept_in_type);
    STATIC_ASSERT(noexcept(invoke(cstring)) == has_noexcept_in_type);
    STATIC_ASSERT(!noexcept(invoke_r<string>(cstring)));

    Thing thing;
    invoke_r<void>(&Thing::n, thing); // no nodiscard warning
    STATIC_ASSERT(is_same_v<decltype(invoke(&Thing::moo, thing)), int&>);
    STATIC_ASSERT(is_same_v<decltype(invoke_r<int>(&Thing::moo, thing)), int>);

    auto lambda = [counter = 0]() mutable { return ++counter; };
    assert(lambda() == 1);
    assert(lambda() == 2);
    assert(invoke_r<int>(lambda) == 3);
    assert(invoke_r<int>(lambda) == 4);
    assert(lambda() == 5);

    int lvalue = 0;
    assert(invoke_r<int>(RefQualified{}, 0) == 1);
    assert(invoke_r<int>(RefQualified{}, lvalue) == 2);
    RefQualified r;
    assert(invoke_r<int>(r, 0) == 3);
    assert(invoke_r<int>(r, lvalue) == 4);

    return true;
}

// LWG-3655: The INVOKE operation and union types
union Union {
    int x;
};
static_assert(is_invocable_v<int Union::*, Union>);
static_assert(is_invocable_v<int Union::*, Union&>);
static_assert(is_invocable_v<int Union::*, const Union>);
static_assert(is_invocable_v<int Union::*, const Union&>);

static_assert(is_invocable_v<void (Union::*)(), Union>);
static_assert(is_invocable_v<void (Union::*)(), Union&>);
static_assert(!is_invocable_v<void (Union::*)(), const Union>);
static_assert(!is_invocable_v<void (Union::*)(), const Union&>);

static_assert(is_invocable_v<void (Union::*)() const, Union>);
static_assert(is_invocable_v<void (Union::*)() const, Union&>);
static_assert(is_invocable_v<void (Union::*)() const, const Union>);
static_assert(is_invocable_v<void (Union::*)() const, const Union&>);

int main() {
    test_invoke_r();
    STATIC_ASSERT(test_invoke_r());
}
