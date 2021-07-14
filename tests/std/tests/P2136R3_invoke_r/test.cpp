// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <functional>
#include <type_traits>

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
    static_assert(is_same_v<decltype(v1), long>);

    auto v2 = invoke_r<double>([]() -> int { return 5; });
    assert(v2 == 5);
    static_assert(is_same_v<decltype(v2), double>);
    static_assert(is_void_v<decltype(invoke_r<void>(square, 1))>);

    // TRANSITION, DevCom-1457457
    static_assert(noexcept(invoke_r<int>(square, 3)) == is_permissive, "invoke_r<int>(square, 3) is noexcept");
    static_assert(noexcept(invoke(square, 3)) == is_permissive, "invoke(square, 3) is noexcept");

    constexpr bool has_noexcept_in_type =
#ifdef __cpp_noexcept_function_type
        true;
#else
        false;
#endif
    static_assert(
        noexcept(invoke(square_noexcept, 3)) == has_noexcept_in_type, "invoke(square_noexcept, 3) isn't noexcept");
    static_assert(noexcept(invoke_r<int>(square_noexcept, 3)) == has_noexcept_in_type,
        "invoke_r<int>(square_noexcept, 3) isn't noexcept");
    static_assert(noexcept(invoke(cstring)) == has_noexcept_in_type, "invoke(cstring) isn't noexcept");
    static_assert(!noexcept(invoke_r<string>(cstring)), "invoke_r<string>(cstring) is noexcept");

    Thing thing;
    invoke_r<void>(&Thing::n, thing); // no nodiscard warning
    static_assert(is_same_v<decltype(invoke(&Thing::moo, thing)), int&>);
    static_assert(is_same_v<decltype(invoke_r<int>(&Thing::moo, thing)), int>);

    int count = 0;
    assert(invoke_r<int>([&count] { return ++count; }) == 1);
    assert(count == 1);

    assert(invoke_r<int>(RefQualified{}, 0) == 1);
    assert(invoke_r<int>(RefQualified{}, count) == 2);
    RefQualified r;
    assert(invoke_r<int>(r, 0) == 3);
    assert(invoke_r<int>(r, count) == 4);

    return true;
}

int main() {
    test_invoke_r();
    static_assert(test_invoke_r());
}
