// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <expected>
#include <type_traits>
#include <utility>

using namespace std;

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

enum class IsNothrowConstructible : bool { Not, Yes };
enum class IsNothrowConvertible : bool { Not, Yes };

template <class E>
[[nodiscard]] constexpr bool IsYes(const E e) noexcept {
    return e == E::Yes;
}

struct convertible {
    constexpr convertible(const int val) noexcept : _val(val) {}

    [[nodiscard]] constexpr bool operator==(const int other) const noexcept {
        return other == _val;
    }

    int _val = 0;
};

struct Immovable {
    constexpr Immovable(const int x) : v(x) {}
    Immovable(const Immovable&)            = delete;
    Immovable(Immovable&&)                 = delete;
    Immovable& operator=(const Immovable&) = delete;
    Immovable& operator=(Immovable&&)      = delete;
    constexpr ~Immovable()                 = default;

    int v;
};

struct Thingy {
    expected<int, int> x;
    constexpr int member_func() const {
        return 66;
    }
};

template <class Expected>
constexpr void test_impl(Expected&& engaged, Expected&& unengaged) {
    assert(engaged.has_value());
    assert(!unengaged.has_value());
    static_assert(is_same_v<typename remove_cvref_t<Expected>::error_type, int>);
    using Val = typename remove_cvref_t<Expected>::value_type;

    const auto succeed = [](auto...) { return expected<int, int>{33}; };
    const auto fail    = [](auto...) { return expected<int, int>{unexpect, 44}; };

    {
        decltype(auto) result = forward<Expected>(engaged).and_then(succeed);
        static_assert(is_same_v<decltype(result), expected<int, int>>);
        assert(result == 33);
    }
    {
        decltype(auto) result = forward<Expected>(unengaged).and_then(succeed);
        static_assert(is_same_v<decltype(result), expected<int, int>>);
        assert(!result);
        assert(result.error() == 22);
    }
    {
        decltype(auto) result = forward<Expected>(engaged).and_then(fail);
        static_assert(is_same_v<decltype(result), expected<int, int>>);
        assert(!result);
        assert(result.error() == 44);
    }
    {
        decltype(auto) result = forward<Expected>(unengaged).and_then(fail);
        static_assert(is_same_v<decltype(result), expected<int, int>>);
        assert(!result);
        assert(result.error() == 22);
    }

    if constexpr (!is_void_v<Val>) {
        {
            decltype(auto) result = forward<Expected>(engaged).and_then(&Thingy::x);
            static_assert(is_same_v<decltype(result), expected<int, int>>);
            assert(result == 11);
        }
        {
            decltype(auto) result = forward<Expected>(unengaged).and_then(&Thingy::x);
            static_assert(is_same_v<decltype(result), expected<int, int>>);
            assert(!result);
            assert(result.error() == 22);
        }
    }

    const auto f       = [](auto...) { return 55; };
    const auto immov   = [](auto...) { return Immovable{88}; };
    const auto to_void = [](auto...) { return; };

    {
        decltype(auto) result = forward<Expected>(engaged).transform(f);
        static_assert(is_same_v<decltype(result), expected<int, int>>);
        assert(result == 55);
    }
    {
        decltype(auto) result = forward<Expected>(unengaged).transform(f);
        static_assert(is_same_v<decltype(result), expected<int, int>>);
        assert(!result);
        assert(result.error() == 22);
    }

    if constexpr (!is_void_v<Val>) {
        {
            decltype(auto) result = forward<Expected>(engaged).transform(&Thingy::member_func);
            static_assert(is_same_v<decltype(result), expected<int, int>>);
            assert(result == 66);
        }
        {
            decltype(auto) result = forward<Expected>(unengaged).transform(&Thingy::member_func);
            static_assert(is_same_v<decltype(result), expected<int, int>>);
            assert(!result);
            assert(result.error() == 22);
        }
    }

    if constexpr (!is_permissive) { // TRANSITION, VSO-1734935
        {
            decltype(auto) result = forward<Expected>(engaged).transform(immov);
            static_assert(is_same_v<decltype(result), expected<Immovable, int>>);
            assert(result->v == 88);
        }
        {
            decltype(auto) result = forward<Expected>(unengaged).transform(immov);
            static_assert(is_same_v<decltype(result), expected<Immovable, int>>);
            assert(!result);
            assert(result.error() == 22);
        }
    }

    {
        decltype(auto) result = forward<Expected>(engaged).transform(to_void);
        static_assert(is_same_v<decltype(result), expected<void, int>>);
        assert(result);
    }
    {
        decltype(auto) result = forward<Expected>(unengaged).transform(to_void);
        static_assert(is_same_v<decltype(result), expected<void, int>>);
        assert(!result);
        assert(result.error() == 22);
    }

    const auto to_thingy = [](int i) { return Thingy{i}; };

    {
        decltype(auto) result = forward<Expected>(engaged).transform_error(to_thingy);
        static_assert(is_same_v<decltype(result), expected<Val, Thingy>>);
        assert(result);
        if constexpr (!is_void_v<Val>) {
            assert(result->x == 11);
        }
    }
    {
        decltype(auto) result = forward<Expected>(unengaged).transform_error(to_thingy);
        static_assert(is_same_v<decltype(result), expected<Val, Thingy>>);
        assert(!result);
        assert(result.error().x == 22);
    }
    {
        decltype(auto) result =
            forward<Expected>(engaged).transform_error(to_thingy).transform_error(&Thingy::member_func);
        static_assert(is_same_v<decltype(result), expected<Val, int>>);
        assert(result);
        if constexpr (!is_void_v<Val>) {
            assert(result->x == 11);
        }
    }
    {
        decltype(auto) result =
            forward<Expected>(unengaged).transform_error(to_thingy).transform_error(&Thingy::member_func);
        static_assert(is_same_v<decltype(result), expected<Val, int>>);
        assert(!result);
        assert(result.error() == 66);
    }

    if constexpr (!is_permissive) { // TRANSITION, VSO-1734935
        {
            decltype(auto) result = forward<Expected>(engaged).transform_error(immov);
            static_assert(is_same_v<decltype(result), expected<Val, Immovable>>);
            assert(result);
            if constexpr (!is_void_v<Val>) {
                assert(result->x == 11);
            }
        }
        {
            decltype(auto) result = forward<Expected>(unengaged).transform_error(immov);
            static_assert(is_same_v<decltype(result), expected<Val, Immovable>>);
            assert(!result);
            assert(result.error().v == 88);
        }
    }

    const auto to_expected_thingy = [](auto...) {
        if constexpr (is_void_v<Val>) {
            return expected<void, int>{};
        } else {
            return expected<Val, int>{Thingy{77}};
        }
    };
    {
        decltype(auto) result = forward<Expected>(engaged).or_else(to_expected_thingy);
        static_assert(is_same_v<decltype(result), expected<Val, int>>);
        assert(result);
        if constexpr (!is_void_v<Val>) {
            assert(result.value().x == 11);
        }
    }
    {
        decltype(auto) result = forward<Expected>(unengaged).or_else(to_expected_thingy);
        static_assert(is_same_v<decltype(result), expected<Val, int>>);
        assert(result);
        if constexpr (!is_void_v<Val>) {
            assert(result.value().x == 77);
        }
    }

    engaged.transform([](auto...) { return ""; });
}

template <IsNothrowConstructible nothrowConstructible, IsNothrowConvertible nothrowConvertible>
constexpr void test_error_or() {
    constexpr bool construction_is_noexcept = IsYes(nothrowConstructible);
    constexpr bool conversion_is_noexcept   = IsYes(nothrowConvertible);
    constexpr bool should_be_noexcept       = construction_is_noexcept && conversion_is_noexcept;

    struct payload_error_or {
        constexpr payload_error_or() noexcept : _val(55) {}
        constexpr payload_error_or(const int val) noexcept : _val(val) {}
        constexpr payload_error_or(const payload_error_or& other) noexcept(construction_is_noexcept)
            : _val(other._val + 2) {}
        constexpr payload_error_or(payload_error_or&& other) noexcept(construction_is_noexcept)
            : _val(other._val + 3) {}
        constexpr payload_error_or(const convertible& val) noexcept(conversion_is_noexcept) : _val(val._val + 4) {}
        constexpr payload_error_or(convertible&& val) noexcept(conversion_is_noexcept) : _val(val._val + 5) {}

        [[nodiscard]] constexpr bool operator==(const payload_error_or&) const noexcept = default;

        int _val;
    };

    { // with payload argument
        using Expected = expected<int, payload_error_or>;

        Expected with_error{unexpect, 42};
        const Expected const_with_error{unexpect, 1337};
        assert(with_error.error_or(payload_error_or{1}) == 42 + 2);
        assert(const_with_error.error_or(payload_error_or{1}) == 1337 + 2);
        static_assert(noexcept(with_error.error_or(payload_error_or{1})) == construction_is_noexcept);
        static_assert(noexcept(const_with_error.error_or(payload_error_or{1})) == construction_is_noexcept);

        assert(move(with_error).error_or(payload_error_or{1}) == 42 + 3);
        assert(move(const_with_error).error_or(payload_error_or{1}) == 1337 + 2);
        static_assert(noexcept(move(with_error).error_or(payload_error_or{1})) == construction_is_noexcept);
        static_assert(noexcept(move(const_with_error).error_or(payload_error_or{1})) == construction_is_noexcept);

        const payload_error_or input{2000};
        Expected with_value{in_place, 42};
        const Expected const_with_value{in_place, 1337};
        assert(with_value.error_or(payload_error_or{1}) == 1 + 3);
        assert(const_with_value.error_or(input) == 2000 + 2);
        static_assert(noexcept(with_value.error_or(payload_error_or{1})) == construction_is_noexcept);
        static_assert(noexcept(const_with_value.error_or(input)) == construction_is_noexcept);

        assert(move(with_value).error_or(payload_error_or{1}) == 1 + 3);
        assert(move(const_with_value).error_or(input) == 2000 + 2);
        static_assert(noexcept(move(with_value).error_or(payload_error_or{1})) == construction_is_noexcept);
        static_assert(noexcept(move(const_with_value).error_or(input)) == construction_is_noexcept);
    }

    { // with convertible argument
        using Expected = expected<int, payload_error_or>;

        Expected with_error{unexpect, 42};
        const Expected const_with_error{unexpect, 1337};
        assert(with_error.error_or(convertible{1}) == 42 + 2);
        assert(const_with_error.error_or(convertible{1}) == 1337 + 2);
        static_assert(noexcept(with_error.error_or(convertible{1})) == should_be_noexcept);
        static_assert(noexcept(const_with_error.error_or(convertible{1})) == should_be_noexcept);

        assert(move(with_error).error_or(convertible{1}) == 42 + 3);
        assert(move(const_with_error).error_or(convertible{1}) == 1337 + 2);
        static_assert(noexcept(move(with_error).error_or(convertible{1})) == should_be_noexcept);
        static_assert(noexcept(move(const_with_error).error_or(convertible{1})) == should_be_noexcept);

        const convertible input{2000};
        Expected with_value{in_place, 42};
        const Expected const_with_value{in_place, 1337};
        assert(with_value.error_or(convertible{1}) == 1 + 5);
        assert(const_with_value.error_or(input) == 2000 + 4);
        static_assert(noexcept(with_value.error_or(convertible{1})) == should_be_noexcept);
        static_assert(noexcept(const_with_value.error_or(input)) == should_be_noexcept);

        assert(move(with_value).error_or(convertible{1}) == 1 + 5);
        assert(move(const_with_value).error_or(input) == 2000 + 4);
        static_assert(noexcept(move(with_value).error_or(convertible{1})) == should_be_noexcept);
        static_assert(noexcept(move(const_with_value).error_or(input)) == should_be_noexcept);
    }

    { // test error_or({})
        using Expected = expected<int, payload_error_or>;

        Expected with_error{unexpect, 42};
        const Expected const_with_error{unexpect, 1337};
        Expected with_value{in_place, 42};
        const Expected const_with_value{in_place, 1337};

        assert(with_error.error_or({}) == 42 + 2);
        assert(const_with_error.error_or({}) == 1337 + 2);
        assert(with_value.error_or({}) == 55 + 3);
        assert(const_with_value.error_or({}) == 55 + 3);
    }
}

constexpr void test_error_or() noexcept {
    test_error_or<IsNothrowConstructible::Not, IsNothrowConvertible::Not>();
    test_error_or<IsNothrowConstructible::Not, IsNothrowConvertible::Yes>();
    test_error_or<IsNothrowConstructible::Yes, IsNothrowConvertible::Not>();
    test_error_or<IsNothrowConstructible::Yes, IsNothrowConvertible::Yes>();
}

constexpr void test_monadic() {
    {
        expected<Thingy, int> engaged{Thingy{11}};
        expected<Thingy, int> unengaged{unexpect, 22};
        test_impl(engaged, unengaged);
        test_impl(as_const(engaged), as_const(unengaged));
        test_impl(move(engaged), move(unengaged));
        test_impl(move(as_const(engaged)), move(as_const(unengaged)));
    }

    {
        expected<void, int> engaged{};
        expected<void, int> unengaged{unexpect, 22};
        test_impl(engaged, unengaged);
        test_impl(as_const(engaged), as_const(unengaged));
        test_impl(move(engaged), move(unengaged));
        test_impl(move(as_const(engaged)), move(as_const(unengaged)));
    }
}

constexpr bool test() {
    test_error_or();
    test_monadic();

    return true;
}

int main() {
    test();
    static_assert(test());
}
