// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <expected>
#include <type_traits>
#include <utility>

using namespace std;

enum class IsNothrowConstructible : bool { Not, Yes };
enum class IsNothrowConvertible : bool { Not, Yes };

template <typename E>
[[nodiscard]] constexpr bool IsYes(const E e) noexcept {
    return e == E::Yes;
}

struct convertible {
    constexpr convertible() = default;
    constexpr convertible(const int val) noexcept : _val(val) {}

    [[nodiscard]] constexpr bool operator==(const int other) const noexcept {
        return other == _val;
    }

    int _val = 0;
};

struct Immovable {
    constexpr Immovable(int x) : v(x) {}
    Immovable(const Immovable&)            = delete;
    Immovable(Immovable&&)                 = delete;
    Immovable& operator=(const Immovable&) = delete;
    Immovable& operator=(Immovable&&)      = delete;
    constexpr ~Immovable() {}

    int v;
};

struct Thingy {
    expected<int, int> x;
    constexpr int member_func() const {
        return 66;
    }
};

template <class Expected>
constexpr void test_impl(Expected&& exp, Expected&& unexp) {
    assert(exp.has_value());
    assert(!unexp.has_value());
    using Val = typename remove_cvref_t<Expected>::value_type;

    auto succeed = [](auto...) { return expected<int, int>{33}; };
    auto fail    = [](auto...) { return expected<int, int>(unexpect, 44); };

    {
        decltype(auto) result = forward<Expected>(exp).and_then(succeed);
        static_assert(is_same_v<decltype(result), expected<int, int>>);
        assert(result == 33);
    }
    {
        decltype(auto) result = forward<Expected>(unexp).and_then(succeed);
        static_assert(is_same_v<decltype(result), expected<int, int>>);
        assert(!result);
        assert(result.error() == 22);
    }
    {
        decltype(auto) result = forward<Expected>(exp).and_then(fail);
        static_assert(is_same_v<decltype(result), expected<int, int>>);
        assert(!result);
        assert(result.error() == 44);
    }
    {
        decltype(auto) result = forward<Expected>(unexp).and_then(fail);
        static_assert(is_same_v<decltype(result), expected<int, int>>);
        assert(!result);
        assert(result.error() == 22);
    }
    if constexpr (!is_void_v<Val>) {
        {
            decltype(auto) result = forward<Expected>(exp).and_then(&Thingy::x);
            static_assert(is_same_v<decltype(result), expected<int, int>>);
            assert(result == 11);
        }
        {
            decltype(auto) result = forward<Expected>(unexp).and_then(&Thingy::x);
            static_assert(is_same_v<decltype(result), expected<int, int>>);
            assert(result.error() == 22);
        }
    }

    auto f       = [](auto...) { return 55; };
    auto immov   = [](auto...) { return Immovable{88}; };
    auto to_void = [](auto...) { return; };

    {
        decltype(auto) result = forward<Expected>(exp).transform(f);
        static_assert(is_same_v<decltype(result), expected<int, int>>);
        assert(result == 55);
    }
    {
        decltype(auto) result = forward<Expected>(unexp).transform(f);
        static_assert(is_same_v<decltype(result), expected<int, int>>);
        assert(!result);
        assert(result.error() == 22);
    }
    if constexpr (!is_void_v<Val>) {
        {
            decltype(auto) result = forward<Expected>(exp).transform(&Thingy::member_func);
            static_assert(is_same_v<decltype(result), expected<int, int>>);
            assert(result == 66);
        }
        {
            decltype(auto) result = forward<Expected>(unexp).transform(&Thingy::member_func);
            static_assert(is_same_v<decltype(result), expected<int, int>>);
            assert(result.error() == 22);
        }
    }
    {
        decltype(auto) result = forward<Expected>(exp).transform(immov);
        static_assert(is_same_v<decltype(result), expected<Immovable, int>>);
        assert(result->v == 88);
    }
    {
        decltype(auto) result = forward<Expected>(unexp).transform(immov);
        static_assert(is_same_v<decltype(result), expected<Immovable, int>>);
        assert(!result);
        assert(result.error() == 22);
    }
    {
        decltype(auto) result = forward<Expected>(exp).transform(to_void);
        static_assert(is_same_v<decltype(result), expected<void, int>>);
        assert(result);
    }
    {
        decltype(auto) result = forward<Expected>(unexp).transform(to_void);
        static_assert(is_same_v<decltype(result), expected<void, int>>);
        assert(!result);
        assert(result.error() == 22);
    }


    auto to_thingy = [](int i) { return Thingy{i}; };

    {
        decltype(auto) result = forward<Expected>(exp).transform_error(to_thingy);
        static_assert(is_same_v<decltype(result), expected<Val, Thingy>>);
        assert(result);
        if constexpr (!is_void_v<Val>) {
            assert(result->x == 11);
        }
    }
    {
        decltype(auto) result = forward<Expected>(unexp).transform_error(to_thingy);
        static_assert(is_same_v<decltype(result), expected<Val, Thingy>>);
        assert(!result);
        assert(result.error().x == 22);
    }
    {
        decltype(auto) result = forward<Expected>(exp).transform_error(to_thingy).transform_error(&Thingy::member_func);
        static_assert(is_same_v<decltype(result), expected<Val, int>>);
        if constexpr (!is_void_v<Val>) {
            assert(result->x == 11);
        }
    }
    {
        decltype(auto) result =
            forward<Expected>(unexp).transform_error(to_thingy).transform_error(&Thingy::member_func);
        static_assert(is_same_v<decltype(result), expected<Val, int>>);
        assert(!result);
        assert(result.error() == 66);
    }
    {
        decltype(auto) result = forward<Expected>(exp).transform_error(immov);
        static_assert(is_same_v<decltype(result), expected<Val, Immovable>>);
        if constexpr (!is_void_v<Val>) {
            assert(result->x == 11);
        }
    }
    {
        decltype(auto) result = forward<Expected>(unexp).transform_error(immov);
        static_assert(is_same_v<decltype(result), expected<Val, Immovable>>);
        assert(!result);
        assert(result.error().v == 88);
    }

    auto to_expected_thingy = [](auto...) {
        if constexpr (is_void_v<Val>) {
            return expected<void, int>{};
        } else {
            return expected<Val, int>{Thingy{77}};
        }
    };
    {
        decltype(auto) result = forward<Expected>(exp).or_else(to_expected_thingy);
        static_assert(is_same_v<decltype(result), expected<Val, int>>);
        assert(result);
        if constexpr (!is_void_v<Val>) {
            assert(result.value().x == 11);
        }
    }
    {
        decltype(auto) result = forward<Expected>(unexp).or_else(to_expected_thingy);
        static_assert(is_same_v<decltype(result), expected<Val, int>>);
        assert(result);
        if constexpr (!is_void_v<Val>) {
            assert(result.value().x == 77);
        }
    }
}

template <IsNothrowConstructible nothrowConstructible, IsNothrowConvertible nothrowConvertible>
constexpr void test_error_or() {
    constexpr bool construction_is_noexcept = IsYes(nothrowConstructible);
    constexpr bool conversion_is_noexcept   = IsYes(nothrowConvertible);
    constexpr bool should_be_noexcept       = construction_is_noexcept && conversion_is_noexcept;

    struct payload_error_or {
        constexpr payload_error_or(const int val) noexcept : _val(val) {}
        constexpr payload_error_or(const payload_error_or& other) noexcept(construction_is_noexcept)
            : _val(other._val + 2) {}
        constexpr payload_error_or(payload_error_or&& other) noexcept(construction_is_noexcept)
            : _val(other._val + 3) {}
        constexpr payload_error_or(const convertible& val) noexcept(conversion_is_noexcept) : _val(val._val + 4) {}
        constexpr payload_error_or(convertible&& val) noexcept(conversion_is_noexcept) : _val(val._val + 5) {}

        [[nodiscard]] constexpr bool operator==(const payload_error_or& right) const noexcept {
            return _val == right._val;
        }

        int _val = 0;
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

        const payload_error_or input{2};
        Expected with_value{in_place, 42};
        const Expected const_with_value{in_place, 1337};
        assert(with_value.error_or(payload_error_or{1}) == 1 + 3);
        assert(const_with_value.error_or(input) == 2 + 2);
        static_assert(noexcept(with_value.error_or(payload_error_or{1})) == construction_is_noexcept);
        static_assert(noexcept(const_with_value.error_or(input)) == construction_is_noexcept);

        assert(move(with_value).error_or(payload_error_or{1}) == 1 + 3);
        assert(move(const_with_value).error_or(input) == 2 + 2);
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

        const convertible input{2};
        Expected with_value{in_place, 42};
        const Expected const_with_value{in_place, 1337};
        assert(with_value.error_or(convertible{1}) == 1 + 5);
        assert(const_with_value.error_or(input) == 2 + 4);
        static_assert(noexcept(with_value.error_or(convertible{1})) == should_be_noexcept);
        static_assert(noexcept(const_with_value.error_or(input)) == should_be_noexcept);

        assert(move(with_value).error_or(convertible{1}) == 1 + 5);
        assert(move(const_with_value).error_or(input) == 2 + 4);
        static_assert(noexcept(move(with_value).error_or(convertible{1})) == should_be_noexcept);
        static_assert(noexcept(move(const_with_value).error_or(input)) == should_be_noexcept);
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
        expected<Thingy, int> exp{Thingy{11}};
        expected<Thingy, int> unexp{std::unexpect, 22};
        test_impl(exp, unexp);
        test_impl(as_const(exp), as_const(unexp));
        test_impl(move(exp), move(unexp));
        test_impl(move(as_const(exp)), move(as_const(unexp)));
    }

    {
        expected<void, int> exp{};
        expected<void, int> unexp{std::unexpect, 22};
        test_impl(exp, unexp);
        test_impl(as_const(exp), as_const(unexp));
        test_impl(move(exp), move(unexp));
        test_impl(move(as_const(exp)), move(as_const(unexp)));
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
