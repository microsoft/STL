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

struct Thing {
    int n = 0;
    constexpr int& foo() {
        return n;
    }
};

constexpr bool is_rvalue(const int&) {
    return false;
}
constexpr bool is_rvalue(int&&) {
    return true;
}

template <class>
constexpr void noexcept_test() {
    static_assert(!noexcept(invoke_r<int>(square, 3)), "invoke_r<int>(square, 3) is noexcept");
    static_assert(!noexcept(invoke(square, 3)), "invoke(square, 3) is noexcept");
}

constexpr bool test_invoke_r() {
    auto v1 = invoke_r<long int>(square, 3);
    assert(v1 == 9L);
    assert((is_same_v<decltype(v1), long int>) );

    auto v2 = invoke_r<double>([]() -> int { return 5; });
    assert(v2 == 5);
    assert((is_same_v<decltype(v2), double>) );

    static_assert(is_void_v<decltype(invoke_r<void>(square, 1))>);
    Thing thing;
    invoke_r<void>(&Thing::n, thing); // no nodiscard warning

    // TRANSITION, DevCom-1457457
    if constexpr (!is_permissive) {
        noexcept_test<void>();
    }
#ifdef __cpp_noexcept_function_type
    static_assert(noexcept(invoke_r<int>(square_noexcept, 3)), "invoke_r<int>(square_noexcept, 3) isn't noexcept");
#endif

    assert(!is_rvalue(invoke(&Thing::foo, thing)));
    assert(is_rvalue(invoke_r<int>(&Thing::foo, thing)));

    return true;
}

int main() {
    test_invoke_r();
    static_assert(test_invoke_r());
}
