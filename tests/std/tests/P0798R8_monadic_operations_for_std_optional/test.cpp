// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <optional>
#include <type_traits>
#include <utility>

using namespace std;

struct Immovable {
    constexpr Immovable(int x) : value(x) {}
    Immovable(const Immovable&) = delete;
    Immovable(Immovable&&)      = delete;
    Immovable& operator=(const Immovable&) = delete;
    Immovable& operator=(Immovable&&) = delete;
    constexpr ~Immovable() {}

    int value;
};

template <class T>
struct fn {
    template <class U>
    constexpr auto operator()(U&&) & {
        static_assert(is_same_v<T, U&&>);
        return optional<int>{48};
    }

    constexpr auto operator()() && {
        return optional<long>{49};
    }

    template <class U>
    constexpr auto operator()(U&& u) && {
        static_assert(is_same_v<T, U&&>);
        return Immovable{u + 2};
    }
};

template <class Optional>
constexpr void test_impl(Optional&& nonempty, Optional&& empty) {
    using U = decltype(forward<Optional>(nonempty).value());
    {
        fn<U> f{};
        decltype(auto) result = forward<Optional>(nonempty).and_then(f);
        static_assert(is_same_v<decltype(result), optional<int>>);
        assert(result == 48);
    }
    {
        fn<U> f{};
        decltype(auto) result = forward<Optional>(empty).and_then(f);
        assert(!result);
    }
    {
        decltype(auto) result = forward<Optional>(nonempty).transform([](auto&&) { return 43; });
        static_assert(is_same_v<decltype(result), optional<int>>);
        assert(result.value() == 43);
    }
    {
        decltype(auto) result = forward<Optional>(nonempty).transform(fn<U>{});
        static_assert(is_same_v<decltype(result), optional<Immovable>>);
        assert(result.value().value == *nonempty + 2);
    }
    {
        decltype(auto) result = forward<Optional>(empty).transform(fn<U>{});
        assert(!result);
    }
#ifdef __cpp_lib_concepts
    {
        decltype(auto) result = forward<Optional>(nonempty).or_else(fn<U>{});
        static_assert(is_same_v<decltype(result), optional<long>>);
        assert(result == nonempty);
    }
    {
        decltype(auto) result = forward<Optional>(empty).or_else(fn<U>{});
        assert(result == 49);
    }
#endif
}

constexpr bool test() {
    {
        optional<long> nonempty = 42;
        optional<long> empty;
        test_impl(nonempty, empty);
    }
    {
        const optional<long> nonempty = 420;
        const optional<long> empty;
        test_impl(nonempty, empty);
    }
    {
        optional<long> nonempty = 4200;
        optional<long> empty;
        test_impl(move(nonempty), move(empty));
    }
    {
        const optional<long> nonempty = 42000;
        const optional<long> empty;
        test_impl(move(nonempty), move(empty));
    }
    return true;
}

int main() {
    test();
    static_assert(test());
}
