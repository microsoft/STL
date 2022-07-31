// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _CONTAINER_DEBUG_LEVEL 1

#include <cassert>
#include <concepts>
#include <exception>
#include <expected>
#include <initializer_list>
#include <type_traits>
#include <utility>

using namespace std;

enum class IsDefaultConstructible : bool { Not, Yes };
enum class IsTriviallyCopyConstructible : bool { Not, Yes };
enum class IsTriviallyMoveConstructible : bool { Not, Yes };
enum class IsTriviallyDestructible : bool { Not, Yes };

enum class IsNothrowConstructible : bool { Not, Yes };
enum class IsNothrowCopyConstructible : bool { Not, Yes };
enum class IsNothrowMoveConstructible : bool { Not, Yes };
enum class IsNothrowCopyAssignable : bool { Not, Yes };
enum class IsNothrowMoveAssignable : bool { Not, Yes };
enum class IsNothrowConvertible : bool { Not, Yes };
enum class IsNothrowComparable : bool { Not, Yes };
enum class IsNothrowSwappable : bool { Not, Yes };

enum class IsExplicitConstructible : bool { Not, Yes };

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

namespace test_unexpected {
    template <IsNothrowCopyConstructible nothrowCopyConstructible, IsNothrowMoveConstructible nothrowMoveConstructible,
        IsNothrowComparable nothrowComparable>
    constexpr void test() {
        constexpr bool copy_construction_is_noexcept = IsYes(nothrowCopyConstructible);
        constexpr bool move_construction_is_noexcept = IsYes(nothrowMoveConstructible);
        constexpr bool compare_is_noexcept           = IsYes(nothrowComparable);

        struct test_error {
            constexpr test_error(const int& val) noexcept(copy_construction_is_noexcept) : _val(val) {}
            constexpr test_error(int&& val) noexcept(move_construction_is_noexcept) : _val(val) {}

            constexpr test_error(initializer_list<int>, const int& val) noexcept(copy_construction_is_noexcept)
                : _val(val) {}
            constexpr test_error(initializer_list<int>, int&& val) noexcept(move_construction_is_noexcept)
                : _val(val) {}

            constexpr test_error(const convertible& other) noexcept(copy_construction_is_noexcept) : _val(other._val) {}
            constexpr test_error(convertible&& other) noexcept(move_construction_is_noexcept) : _val(other._val) {}

            [[nodiscard]] constexpr bool operator==(const test_error& right) const noexcept(compare_is_noexcept) {
                return _val == right._val;
            }
            [[nodiscard]] constexpr bool operator==(const convertible& right) const noexcept(compare_is_noexcept) {
                return _val == right._val;
            }

            int _val = 0;
        };
        using Unexpect = std::unexpected<test_error>;

        // [expected.un.ctor]
        const int& input = 1;
        Unexpect in_place_lvalue_constructed{in_place, input};
        static_assert(noexcept(Unexpect{in_place, input}) == copy_construction_is_noexcept);
        assert(in_place_lvalue_constructed == Unexpect{test_error{1}});

        Unexpect in_place_rvalue_constructed{in_place, 42};
        static_assert(noexcept(Unexpect{in_place, 42}) == move_construction_is_noexcept);
        assert(in_place_rvalue_constructed == Unexpect{test_error{42}});

        Unexpect in_place_ilist_lvalue_constructed{in_place, {2}, input};
        static_assert(noexcept(Unexpect{in_place, {2}, input}) == copy_construction_is_noexcept);
        assert(in_place_ilist_lvalue_constructed == Unexpect{test_error{1}});

        Unexpect in_place_ilist_rvalue_constructed{in_place, {2}, 1337};
        static_assert(noexcept(Unexpect{in_place, {2}, 1337}) == move_construction_is_noexcept);
        assert(in_place_ilist_rvalue_constructed == Unexpect{test_error{1337}});

        Unexpect base_error_constructed{test_error{3}};
        static_assert(noexcept(Unexpect{test_error{3}}) == move_construction_is_noexcept);
        assert(base_error_constructed.error()._val == 3);

        Unexpect conversion_error_constructed{convertible{4}};
        static_assert(noexcept(Unexpect{convertible{4}}) == move_construction_is_noexcept);
        assert(conversion_error_constructed.error()._val == 4);

        Unexpect brace_error_constructed{{5}};
        static_assert(noexcept(Unexpect{{5}}) == move_construction_is_noexcept);
        assert(brace_error_constructed.error()._val == 5);

        // [expected.un.eq]
        assert(in_place_lvalue_constructed == in_place_lvalue_constructed);
        assert(in_place_lvalue_constructed != in_place_rvalue_constructed);
        static_assert(noexcept(in_place_lvalue_constructed == in_place_lvalue_constructed) == compare_is_noexcept);
        static_assert(noexcept(in_place_lvalue_constructed != in_place_lvalue_constructed) == compare_is_noexcept);

        const auto converted = std::unexpected<convertible>{convertible{3}};
        assert(base_error_constructed == converted);
        assert(conversion_error_constructed != converted);
        static_assert(noexcept(base_error_constructed == converted) == compare_is_noexcept);
        static_assert(noexcept(conversion_error_constructed != converted) == compare_is_noexcept);

        // [expected.un.swap]
        in_place_lvalue_constructed.swap(in_place_rvalue_constructed);
        assert(in_place_lvalue_constructed == Unexpect{test_error{42}});
        assert(in_place_rvalue_constructed == Unexpect{test_error{1}});
        static_assert(noexcept(in_place_lvalue_constructed.swap(in_place_rvalue_constructed)));

        swap(base_error_constructed, conversion_error_constructed);
        assert(base_error_constructed == Unexpect{test_error{4}});
        assert(conversion_error_constructed == Unexpect{test_error{3}});
        static_assert(noexcept(swap(base_error_constructed, conversion_error_constructed)));

        // [expected.un.obs]
        auto&& lvalue_error = base_error_constructed.error();
        assert(lvalue_error == test_error{4});
        static_assert(is_same_v<decltype(lvalue_error), test_error&>);

        auto&& rvalue_error = move(conversion_error_constructed).error();
        assert(rvalue_error == test_error{3});
        static_assert(is_same_v<decltype(rvalue_error), test_error&&>);

        auto&& const_lvalue_error = as_const(in_place_lvalue_constructed).error();
        assert(const_lvalue_error == test_error{42});
        static_assert(is_same_v<decltype(const_lvalue_error), const test_error&>);

        auto&& const_rvalue_error = move(as_const(in_place_ilist_lvalue_constructed)).error();
        assert(const_rvalue_error == test_error{1});
        static_assert(is_same_v<decltype(const_rvalue_error), const test_error&&>);

        // deduction guide
        std::unexpected deduced(test_error{42});
        static_assert(same_as<decltype(deduced), Unexpect>);
    }

    constexpr bool test_all() {
        test<IsNothrowCopyConstructible::Not, IsNothrowMoveConstructible::Not, IsNothrowComparable::Not>();
        test<IsNothrowCopyConstructible::Not, IsNothrowMoveConstructible::Not, IsNothrowComparable::Yes>();
        test<IsNothrowCopyConstructible::Not, IsNothrowMoveConstructible::Yes, IsNothrowComparable::Not>();
        test<IsNothrowCopyConstructible::Not, IsNothrowMoveConstructible::Yes, IsNothrowComparable::Yes>();
        test<IsNothrowCopyConstructible::Yes, IsNothrowMoveConstructible::Not, IsNothrowComparable::Not>();
        test<IsNothrowCopyConstructible::Yes, IsNothrowMoveConstructible::Not, IsNothrowComparable::Yes>();
        test<IsNothrowCopyConstructible::Yes, IsNothrowMoveConstructible::Yes, IsNothrowComparable::Not>();
        test<IsNothrowCopyConstructible::Yes, IsNothrowMoveConstructible::Yes, IsNothrowComparable::Yes>();

        return true;
    }
} // namespace test_unexpected

namespace test_unexpect {
    auto copy = unexpect;
    static_assert(is_same_v<decltype(copy), unexpect_t>);
    static_assert(is_trivial_v<unexpect_t>);
    static_assert(is_empty_v<unexpect_t>);
} // namespace test_unexpect

namespace test_expected {
    constexpr void test_aliases() {
        struct value_tag {};
        struct error_tag {};

        {
            using Expected = expected<value_tag, error_tag>;
            static_assert(same_as<typename Expected::value_type, value_tag>);
            static_assert(same_as<typename Expected::error_type, error_tag>);
            static_assert(same_as<typename Expected::unexpected_type, std::unexpected<error_tag>>);

            static_assert(same_as<typename Expected::rebind<int>, expected<int, error_tag>>);
        }

        {
            using Expected = expected<void, error_tag>;
            static_assert(same_as<typename Expected::value_type, void>);
            static_assert(same_as<typename Expected::error_type, error_tag>);
            static_assert(same_as<typename Expected::unexpected_type, std::unexpected<error_tag>>);

            static_assert(same_as<typename Expected::rebind<int>, expected<int, error_tag>>);
        }
    }

    template <IsDefaultConstructible defaultConstructible>
    constexpr void test_default_constructors() {
        constexpr bool should_be_defaultable = IsYes(defaultConstructible);

        struct payload_default_constructor {
            constexpr payload_default_constructor() requires(should_be_defaultable) : _val(42) {}

            [[nodiscard]] constexpr bool operator==(const int val) const noexcept {
                return _val == val;
            }

            int _val = 0;
        };

        static_assert(is_default_constructible_v<expected<payload_default_constructor, int>> == should_be_defaultable);
        // we only care about payload type
        static_assert(is_default_constructible_v<expected<int, payload_default_constructor>>);
        static_assert(is_default_constructible_v<expected<void, payload_default_constructor>>);

        if constexpr (should_be_defaultable) {
            const expected<payload_default_constructor, int> defaulted;
            assert(defaulted);
            assert(defaulted.value() == 42);
        }
    }

    template <IsTriviallyCopyConstructible triviallyCopyConstructible,
        IsNothrowCopyConstructible nothrowCopyConstructible>
    constexpr void test_copy_constructors() {
        constexpr bool should_be_trivial  = IsYes(triviallyCopyConstructible);
        constexpr bool should_be_noexcept = should_be_trivial || IsYes(nothrowCopyConstructible);

        struct payload_copy_constructor {
            payload_copy_constructor()                                           = default;
            payload_copy_constructor& operator=(const payload_copy_constructor&) = delete;
            constexpr payload_copy_constructor(const payload_copy_constructor&) noexcept(should_be_noexcept) //
                requires(!should_be_trivial)
                : _val(42) {}
            constexpr payload_copy_constructor(const payload_copy_constructor&) = default;

            [[nodiscard]] constexpr bool operator==(const int val) const noexcept {
                return _val == val;
            }

            int _val = 0;
        };

        { // Check payload type
            using Expected = expected<payload_copy_constructor, int>;
#ifndef __clang__ // TRANSITION, LLVM-46269
            static_assert(is_trivially_copy_constructible_v<Expected> == should_be_trivial);
#endif // !__clang__
            static_assert(is_copy_constructible_v<Expected>);

            const Expected with_value{in_place};
            const Expected from_value{with_value};
            assert(from_value);
            assert(from_value.value() == (should_be_trivial ? 0 : 42));
            static_assert(noexcept(Expected{with_value}) == should_be_noexcept);

            const Expected with_error{unexpect};
            const Expected from_error{with_error};
            assert(!from_error);
            assert(from_error.error() == 0);
            static_assert(noexcept(Expected{with_error}) == should_be_noexcept);
        }

        { // Check error type
            using Expected = expected<int, payload_copy_constructor>;
#ifndef __clang__ // TRANSITION, LLVM-46269
            static_assert(is_trivially_copy_constructible_v<Expected> == should_be_trivial);
#endif // !__clang__
            static_assert(is_copy_constructible_v<Expected>);

            const Expected with_value{in_place};
            const Expected from_value{with_value};
            assert(from_value);
            assert(from_value.value() == 0);
            static_assert(noexcept(Expected{with_value}) == should_be_noexcept);

            const Expected with_error{unexpect};
            const Expected from_error{with_error};
            assert(!from_error);
            assert(from_error.error() == (should_be_trivial ? 0 : 42));
            static_assert(noexcept(Expected{with_error}) == should_be_noexcept);
        }

        { // Check void payload
            using Expected = expected<void, payload_copy_constructor>;
#ifndef __clang__ // TRANSITION, LLVM-46269
            static_assert(is_trivially_copy_constructible_v<Expected> == should_be_trivial);
#endif // !__clang__
            static_assert(is_copy_constructible_v<Expected>);

            const Expected with_value{in_place};
            const Expected from_value{with_value};
            assert(from_value);
            static_assert(noexcept(Expected{with_value}) == should_be_noexcept);

            const Expected with_error{unexpect};
            const Expected from_error{with_error};
            assert(!from_error);
            assert(from_error.error() == (should_be_trivial ? 0 : 42));
            static_assert(noexcept(Expected{with_error}) == should_be_noexcept);
        }

        { // ensure we are not copy constructible if either the payload or the error are not
            struct not_copy_constructible {
                not_copy_constructible(const not_copy_constructible&) = delete;
            };

            static_assert(!is_copy_constructible_v<expected<not_copy_constructible, int>>);
            static_assert(!is_copy_constructible_v<expected<int, not_copy_constructible>>);
            static_assert(!is_copy_constructible_v<expected<void, not_copy_constructible>>);
        }
    }

    template <IsTriviallyMoveConstructible triviallyMoveConstructible,
        IsNothrowMoveConstructible nothrowMoveConstructible>
    constexpr void test_move_constructors() {
        constexpr bool should_be_trivial  = IsYes(triviallyMoveConstructible);
        constexpr bool should_be_noexcept = should_be_trivial || IsYes(nothrowMoveConstructible);

        struct payload_move_constructor {
            payload_move_constructor()                                      = default;
            payload_move_constructor(const payload_move_constructor&)       = default;
            payload_move_constructor& operator=(payload_move_constructor&&) = delete;
            constexpr payload_move_constructor(payload_move_constructor&&) noexcept(should_be_noexcept) //
                requires(!should_be_trivial)
                : _val(42) {}
            constexpr payload_move_constructor(payload_move_constructor&&) = default;

            [[nodiscard]] constexpr bool operator==(const int val) const noexcept {
                return _val == val;
            }

            int _val = 0;
        };

        { // Check payload type
            using Expected = expected<payload_move_constructor, int>;
#ifndef __clang__ // TRANSITION, LLVM-46269
            static_assert(is_trivially_move_constructible_v<Expected> == should_be_trivial);
#endif // !__clang__
            static_assert(is_move_constructible_v<Expected>);

            Expected value_input{in_place};
            const Expected from_value{move(value_input)};
            assert(from_value);
            assert(from_value.value() == (should_be_trivial ? 0 : 42));
            static_assert(noexcept(Expected{move(value_input)}) == should_be_noexcept);

            Expected error_input{unexpect};
            const Expected from_error{move(error_input)};
            assert(!from_error);
            assert(from_error.error() == 0);
            static_assert(noexcept(Expected{move(error_input)}) == should_be_noexcept);
        }

        { // Check error type
            using Expected = expected<int, payload_move_constructor>;
#ifndef __clang__ // TRANSITION, LLVM-46269
            static_assert(is_trivially_move_constructible_v<Expected> == should_be_trivial);
#endif // !__clang__
            static_assert(is_move_constructible_v<Expected>);

            Expected value_input{in_place};
            const Expected from_value{move(value_input)};
            assert(from_value);
            assert(from_value.value() == 0);
            static_assert(noexcept(Expected{move(value_input)}) == should_be_noexcept);

            Expected error_input{unexpect};
            const Expected from_error{move(error_input)};
            assert(!from_error);
            assert(from_error.error() == (should_be_trivial ? 0 : 42));
            static_assert(noexcept(Expected{move(error_input)}) == should_be_noexcept);
        }

        { // Check void payload
            using Expected = expected<void, payload_move_constructor>;
#ifndef __clang__ // TRANSITION, LLVM-46269
            static_assert(is_trivially_move_constructible_v<Expected> == should_be_trivial);
#endif // !__clang__
            static_assert(is_move_constructible_v<Expected>);

            Expected value_input{in_place};
            const Expected from_value{move(value_input)};
            assert(from_value);
            static_assert(noexcept(Expected{move(value_input)}) == should_be_noexcept);

            Expected error_input{unexpect};
            const Expected from_error{move(error_input)};
            assert(!from_error);
            assert(from_error.error() == (should_be_trivial ? 0 : 42));
            static_assert(noexcept(Expected{move(error_input)}) == should_be_noexcept);
        }

        { // ensure we are not move constructible if either the payload or the error are not
            struct not_move_constructible {
                not_move_constructible(not_move_constructible&&) = delete;
            };

            static_assert(!is_move_constructible_v<expected<not_move_constructible, int>>);
            static_assert(!is_move_constructible_v<expected<int, not_move_constructible>>);
            static_assert(!is_move_constructible_v<expected<void, not_move_constructible>>);
        }
    }

    template <IsTriviallyDestructible triviallyDestructible>
    struct payload_destructor {
        constexpr payload_destructor(bool& destructor_called) : _destructor_called(destructor_called) {}
        bool& _destructor_called;
    };
    template <> // TRANSITION, LLVM-46269
    struct payload_destructor<IsTriviallyDestructible::Not> {
        constexpr payload_destructor(bool& destructor_called) : _destructor_called(destructor_called) {}
        payload_destructor(const payload_destructor&) = default;
        constexpr ~payload_destructor() {
            _destructor_called = true;
        };

        bool& _destructor_called;
    };
    template <IsTriviallyDestructible triviallyDestructible>
    constexpr void test_destructors() {
        constexpr bool is_trivial = IsYes(triviallyDestructible);
        bool destructor_called    = false;
        { // Check payload
            using Expected = expected<payload_destructor<triviallyDestructible>, int>;
#ifndef __clang__ // TRANSITION, LLVM-46269
            static_assert(is_trivially_destructible_v<Expected> == is_trivial);
#endif // !__clang__

            Expected val{in_place, destructor_called};
        }
        assert(destructor_called == !is_trivial);
        destructor_called = false;

        { // Check error
            using Expected = expected<int, payload_destructor<triviallyDestructible>>;
#ifndef __clang__ // TRANSITION, LLVM-46269
            static_assert(is_trivially_destructible_v<Expected> == is_trivial);
#endif // !__clang__

            Expected err{unexpect, destructor_called};
        }
        assert(destructor_called == !is_trivial);
        destructor_called = false;

        { // Check void error
            using Expected = expected<void, payload_destructor<triviallyDestructible>>;
#ifndef __clang__ // TRANSITION, LLVM-46269
            static_assert(is_trivially_destructible_v<Expected> == is_trivial);
#endif // !__clang__

            Expected err{unexpect, destructor_called};
        }
        assert(destructor_called == !is_trivial);
    }

    constexpr void test_special_members() {
        test_default_constructors<IsDefaultConstructible::Not>();
        test_default_constructors<IsDefaultConstructible::Yes>();

        test_copy_constructors<IsTriviallyCopyConstructible::Not, IsNothrowCopyConstructible::Not>();
        test_copy_constructors<IsTriviallyCopyConstructible::Not, IsNothrowCopyConstructible::Yes>();
        test_copy_constructors<IsTriviallyCopyConstructible::Yes, IsNothrowCopyConstructible::Not>();
        test_copy_constructors<IsTriviallyCopyConstructible::Yes, IsNothrowCopyConstructible::Yes>();

        test_move_constructors<IsTriviallyMoveConstructible::Not, IsNothrowMoveConstructible::Not>();
        test_move_constructors<IsTriviallyMoveConstructible::Not, IsNothrowMoveConstructible::Yes>();
        test_move_constructors<IsTriviallyMoveConstructible::Yes, IsNothrowMoveConstructible::Not>();
        test_move_constructors<IsTriviallyMoveConstructible::Yes, IsNothrowMoveConstructible::Yes>();

        test_destructors<IsTriviallyDestructible::Not>();
        test_destructors<IsTriviallyDestructible::Yes>();
    }

    template <IsNothrowConstructible nothrowConstructible, IsExplicitConstructible explicitConstructible>
    constexpr void test_constructors() noexcept {
        constexpr bool should_be_noexcept = IsYes(nothrowConstructible);
        constexpr bool should_be_explicit = IsYes(explicitConstructible);

        struct payload_constructors {
            payload_constructors() = default;
            // Note clang does not accept local variables in explicit
            constexpr explicit(IsYes(explicitConstructible))
                payload_constructors(const convertible&) noexcept(should_be_noexcept)
                : _val(3) {}
            constexpr explicit(IsYes(explicitConstructible))
                payload_constructors(convertible&&) noexcept(should_be_noexcept)
                : _val(42) {}
            constexpr explicit(IsYes(explicitConstructible))
                payload_constructors(initializer_list<int>&, convertible) noexcept(should_be_noexcept)
                : _val(1337) {}

            [[nodiscard]] constexpr bool operator==(const int val) const noexcept {
                return _val == val;
            }

            int _val = 0;
        };

        { // constructing from convertible payload
            using Input    = convertible;
            using Expected = expected<payload_constructors, payload_constructors>;
            static_assert(is_convertible_v<const Input&, Expected> != should_be_explicit);
            static_assert(is_convertible_v<Input, Expected> != should_be_explicit);

            const Input const_input_value{};
            const Expected copy_constructed_value{const_input_value};
            assert(copy_constructed_value);
            assert(copy_constructed_value.value() == 3);
            static_assert(noexcept(Expected{const_input_value}) == should_be_noexcept);

            const Expected move_constructed_value{Input{}};
            assert(move_constructed_value);
            assert(move_constructed_value.value() == 42);
            static_assert(noexcept(Expected{Input{}}) == should_be_noexcept);

            const Expected brace_constructed_value{{}};
            assert(brace_constructed_value);
            assert(brace_constructed_value.value() == 0);
            static_assert(noexcept(Expected{{}}));
        }

        { // converting from different expected
            using Input    = expected<convertible, convertible>;
            using Expected = expected<payload_constructors, payload_constructors>;
            static_assert(is_convertible_v<const Input&, Expected> != should_be_explicit);
            static_assert(is_convertible_v<Input, Expected> != should_be_explicit);

            const Input const_input_value{};
            const Expected copy_constructed_value{const_input_value};
            assert(copy_constructed_value);
            assert(copy_constructed_value.value() == 3);
            static_assert(noexcept(Expected{const_input_value}) == should_be_noexcept);

            const Expected move_constructed_value{Input{in_place}};
            assert(move_constructed_value);
            assert(move_constructed_value.value() == 42);
            static_assert(noexcept(Expected{Input{in_place}}) == should_be_noexcept);

            const Input const_input_error{unexpect};
            const Expected copy_constructed_error{const_input_error};
            assert(!copy_constructed_error);
            assert(copy_constructed_error.error() == 3);
            static_assert(noexcept(Expected{const_input_error}) == should_be_noexcept);

            const Expected move_constructed_error{Input{unexpect}};
            assert(!move_constructed_error);
            assert(move_constructed_error.error() == 42);
            static_assert(noexcept(Expected{Input{unexpect}}) == should_be_noexcept);
        }

        { // converting from unexpected
            using Input    = std::unexpected<convertible>;
            using Expected = expected<int, payload_constructors>;

            const Input const_input{in_place};
            const Expected copy_constructed{const_input};
            assert(!copy_constructed);
            assert(copy_constructed.error() == 3);
            static_assert(noexcept(Expected{const_input}) == should_be_noexcept);

            const Expected move_constructed{Input{in_place}};
            assert(!move_constructed);
            assert(move_constructed.error() == 42);
            static_assert(noexcept(Expected{Input{in_place}}) == should_be_noexcept);
        }

        { // in place payload
            using Expected = expected<payload_constructors, int>;
            const Expected default_constructed{in_place};
            assert(default_constructed);
            assert(default_constructed.value() == 0);
            static_assert(noexcept(Expected{in_place}));

            const Expected value_constructed{in_place, convertible{}};
            assert(value_constructed);
            assert(value_constructed.value() == 42);
            static_assert(noexcept(Expected{in_place, convertible{}}) == should_be_noexcept);

            const Expected ilist_value_constructed{in_place, {1}, convertible{}};
            assert(ilist_value_constructed);
            assert(ilist_value_constructed.value() == 1337);
            static_assert(noexcept(Expected{in_place, {1}, convertible{}}) == should_be_noexcept);
        }

        { // in place error
            using Expected = expected<int, payload_constructors>;
            const Expected default_constructed{unexpect};
            assert(!default_constructed);
            assert(default_constructed.error() == 0);
            static_assert(noexcept(Expected{unexpect}));

            const Expected value_constructed{unexpect, convertible{}};
            assert(!value_constructed);
            assert(value_constructed.error() == 42);
            static_assert(noexcept(Expected{unexpect, convertible{}}) == should_be_noexcept);

            const Expected ilist_value_constructed{unexpect, {1}, convertible{}};
            assert(!ilist_value_constructed);
            assert(ilist_value_constructed.error() == 1337);
            static_assert(noexcept(Expected{unexpect, {1}, convertible{}}) == should_be_noexcept);
        }

        { // expected<void, E>: converting from different expected
            using Input    = expected<void, convertible>;
            using Expected = expected<void, payload_constructors>;
            static_assert(is_convertible_v<const Input&, Expected> != should_be_explicit);
            static_assert(is_convertible_v<Input, Expected> != should_be_explicit);

            const Input const_input_value{};
            const Expected copy_constructed_value{const_input_value};
            assert(copy_constructed_value);
            copy_constructed_value.value();
            static_assert(noexcept(Expected{const_input_value}) == should_be_noexcept);

            const Expected move_constructed_value{Input{in_place}};
            assert(move_constructed_value);
            move_constructed_value.value();
            static_assert(noexcept(Expected{Input{in_place}}) == should_be_noexcept);

            const Input const_input_error{unexpect};
            const Expected copy_constructed_error{const_input_error};
            assert(!copy_constructed_error);
            assert(copy_constructed_error.error() == 3);
            static_assert(noexcept(Expected{const_input_error}) == should_be_noexcept);

            const Expected move_constructed_error{Input{unexpect}};
            assert(!move_constructed_error);
            assert(move_constructed_error.error() == 42);
            static_assert(noexcept(Expected{Input{unexpect}}) == should_be_noexcept);
        }

        { // expected<void, E>: converting from unexpected
            using Input    = std::unexpected<convertible>;
            using Expected = expected<void, payload_constructors>;

            const Input const_input{in_place};
            const Expected copy_constructed{const_input};
            assert(!copy_constructed);
            assert(copy_constructed.error() == 3);
            static_assert(noexcept(Expected{const_input}) == should_be_noexcept);

            const Expected move_constructed{Input{in_place}};
            assert(!move_constructed);
            assert(move_constructed.error() == 42);
            static_assert(noexcept(Expected{Input{in_place}}) == should_be_noexcept);
        }

        { // expected<void, E>: in place payload
            using Expected = expected<void, int>;
            const Expected default_constructed{in_place};
            assert(default_constructed);
            default_constructed.value();
            static_assert(noexcept(Expected{in_place}));
        }

        { // expected<void, E>: in place error
            using Expected = expected<void, payload_constructors>;
            const Expected default_constructed{unexpect};
            assert(!default_constructed);
            assert(default_constructed.error() == 0);
            static_assert(noexcept(Expected{unexpect}));

            const Expected value_constructed{unexpect, convertible{}};
            assert(!value_constructed);
            assert(value_constructed.error() == 42);
            static_assert(noexcept(Expected{unexpect, convertible{}}) == should_be_noexcept);

            const Expected ilist_value_constructed{unexpect, {1}, convertible{}};
            assert(!ilist_value_constructed);
            assert(ilist_value_constructed.error() == 1337);
            static_assert(noexcept(Expected{unexpect, {1}, convertible{}}) == should_be_noexcept);
        }
    }

    constexpr void test_constructors() noexcept {
        test_constructors<IsNothrowConstructible::Not, IsExplicitConstructible::Not>();
        test_constructors<IsNothrowConstructible::Not, IsExplicitConstructible::Yes>();
        test_constructors<IsNothrowConstructible::Yes, IsExplicitConstructible::Not>();
        test_constructors<IsNothrowConstructible::Yes, IsExplicitConstructible::Yes>();
    }

    template <IsNothrowCopyConstructible nothrowCopyConstructible, IsNothrowMoveConstructible nothrowMoveConstructible,
        IsNothrowCopyAssignable nothrowCopyAssignable, IsNothrowMoveAssignable nothrowMoveAssignable>
    constexpr void test_assignment() noexcept {
        constexpr bool nothrow_copy_constructible = IsYes(nothrowCopyConstructible);
        constexpr bool nothrow_move_constructible = IsYes(nothrowMoveConstructible);
        constexpr bool nothrow_copy_assignable    = IsYes(nothrowCopyAssignable);
        constexpr bool nothrow_move_assignable    = IsYes(nothrowMoveAssignable);

        struct payload_assign {
            payload_assign() = default;
            constexpr payload_assign(const int val) noexcept : _val(val) {}
            constexpr payload_assign(const payload_assign& other) noexcept(nothrow_copy_constructible)
                : _val(other._val) {}
            constexpr payload_assign(payload_assign&& other) noexcept(nothrow_move_constructible) : _val(other._val) {}
            constexpr payload_assign& operator=(const payload_assign& other) noexcept(nothrow_copy_assignable) {
                _val = other._val;
                return *this;
            }
            constexpr payload_assign& operator=(payload_assign&& other) noexcept(nothrow_move_assignable) {
                _val = other._val;
                return *this;
            }

            constexpr payload_assign(const convertible& other) noexcept(nothrow_copy_constructible)
                : _val(other._val) {}
            constexpr payload_assign(convertible&& other) noexcept(nothrow_move_constructible) : _val(other._val) {}
            constexpr payload_assign& operator=(const convertible& other) noexcept(nothrow_copy_assignable) {
                _val = other._val;
                return *this;
            }
            constexpr payload_assign& operator=(convertible&& other) noexcept(nothrow_move_assignable) {
                _val = other._val;
                return *this;
            }

            [[nodiscard]] constexpr bool operator==(const int other) const noexcept {
                return other == _val;
            }
            int _val = 0;
        };

        { // assign same expected as const ref check payload
            constexpr bool should_be_noexcept = nothrow_copy_constructible && nothrow_copy_assignable;
            using Expected                    = expected<payload_assign, int>;
            const Expected input_value{in_place, 42};
            const Expected input_error{unexpect, 1337};

            Expected assign_value_to_value{in_place, 1};
            assign_value_to_value = input_value;
            assert(assign_value_to_value);
            assert(assign_value_to_value.value() == 42);
            static_assert(noexcept(assign_value_to_value = input_value) == should_be_noexcept);

            Expected assign_error_to_value{in_place, 1};
            assign_error_to_value = input_error;
            assert(!assign_error_to_value);
            assert(assign_error_to_value.error() == 1337);
            static_assert(noexcept(assign_error_to_value = input_error) == should_be_noexcept);

            Expected assign_value_to_error{unexpect, 1};
            assign_value_to_error = input_value;
            assert(assign_value_to_error);
            assert(assign_value_to_error.value() == 42);
            static_assert(noexcept(assign_value_to_error = input_value) == should_be_noexcept);

            Expected assign_error_to_error{unexpect, 1};
            assign_error_to_error = input_error;
            assert(!assign_error_to_error);
            assert(assign_error_to_error.error() == 1337);
            static_assert(noexcept(assign_error_to_error = input_error) == should_be_noexcept);
        }

        { // assign same expected as const ref check error
            constexpr bool should_be_noexcept = nothrow_copy_constructible && nothrow_copy_assignable;
            using Expected                    = expected<int, payload_assign>;
            const Expected input_value{in_place, 42};
            const Expected input_error{unexpect, 1337};

            Expected assign_value_to_value{in_place, 1};
            assign_value_to_value = input_value;
            assert(assign_value_to_value);
            assert(assign_value_to_value.value() == 42);
            static_assert(noexcept(assign_value_to_value = input_value) == should_be_noexcept);

            Expected assign_error_to_value{in_place, 1};
            assign_error_to_value = input_error;
            assert(!assign_error_to_value);
            assert(assign_error_to_value.error() == 1337);
            static_assert(noexcept(assign_error_to_value = input_error) == should_be_noexcept);

            Expected assign_value_to_error{unexpect, 1};
            assign_value_to_error = input_value;
            assert(assign_value_to_error);
            assert(assign_value_to_error.value() == 42);
            static_assert(noexcept(assign_value_to_error = input_value) == should_be_noexcept);

            Expected assign_error_to_error{unexpect, 1};
            assign_error_to_error = input_error;
            assert(!assign_error_to_error);
            assert(assign_error_to_error.error() == 1337);
            static_assert(noexcept(assign_error_to_error = input_error) == should_be_noexcept);
        }

        { // assign same expected<void> as const ref check error
            constexpr bool should_be_noexcept = nothrow_copy_constructible && nothrow_copy_assignable;
            using Expected                    = expected<void, payload_assign>;
            const Expected input_value{in_place};
            const Expected input_error{unexpect, 1337};

            Expected assign_value_to_value{in_place};
            assign_value_to_value = input_value;
            assert(assign_value_to_value);
            static_assert(noexcept(assign_value_to_value = input_value) == should_be_noexcept);

            Expected assign_error_to_value{in_place};
            assign_error_to_value = input_error;
            assert(!assign_error_to_value);
            assert(assign_error_to_value.error() == 1337);
            static_assert(noexcept(assign_error_to_value = input_error) == should_be_noexcept);

            Expected assign_value_to_error{unexpect, 1};
            assign_value_to_error = input_value;
            assert(assign_value_to_error);
            static_assert(noexcept(assign_value_to_error = input_value) == should_be_noexcept);

            Expected assign_error_to_error{unexpect, 1};
            assign_error_to_error = input_error;
            assert(!assign_error_to_error);
            assert(assign_error_to_error.error() == 1337);
            static_assert(noexcept(assign_error_to_error = input_error) == should_be_noexcept);
        }

        { // assign same expected as rvalue check payload
            constexpr bool should_be_noexcept = nothrow_move_constructible && nothrow_move_assignable;
            using Expected                    = expected<payload_assign, int>;

            Expected assign_value_to_value{in_place, 1};
            assign_value_to_value = Expected{in_place, 42};
            assert(assign_value_to_value);
            assert(assign_value_to_value.value() == 42);
            static_assert(noexcept(assign_value_to_value = Expected{in_place, 42}) == should_be_noexcept);

            Expected assign_error_to_value{in_place, 1};
            assign_error_to_value = Expected{unexpect, 1337};
            assert(!assign_error_to_value);
            assert(assign_error_to_value.error() == 1337);
            static_assert(noexcept(assign_error_to_value = Expected{unexpect, 1337}) == should_be_noexcept);

            Expected assign_value_to_error{unexpect, 1};
            assign_value_to_error = Expected{in_place, 42};
            assert(assign_value_to_error);
            assert(assign_value_to_error.value() == 42);
            static_assert(noexcept(assign_value_to_error = Expected{in_place, 42}) == should_be_noexcept);

            Expected assign_error_to_error{unexpect, 1};
            assign_error_to_error = Expected{unexpect, 1337};
            assert(!assign_error_to_error);
            assert(assign_error_to_error.error() == 1337);
            static_assert(noexcept(assign_error_to_error = Expected{unexpect, 1337}) == should_be_noexcept);
        }

        { // assign same expected as rvalue check error
            constexpr bool should_be_noexcept = nothrow_move_constructible && nothrow_move_assignable;
            using Expected                    = expected<int, payload_assign>;

            Expected assign_value_to_value{in_place, 1};
            assign_value_to_value = Expected{in_place, 42};
            assert(assign_value_to_value);
            assert(assign_value_to_value.value() == 42);
            static_assert(noexcept(assign_value_to_value = Expected{in_place, 42}) == should_be_noexcept);

            Expected assign_error_to_value{in_place, 1};
            assign_error_to_value = Expected{unexpect, 1337};
            assert(!assign_error_to_value);
            assert(assign_error_to_value.error() == 1337);
            static_assert(noexcept(assign_error_to_value = Expected{unexpect, 1337}) == should_be_noexcept);

            Expected assign_value_to_error{unexpect, 1};
            assign_value_to_error = Expected{in_place, 42};
            assert(assign_value_to_error);
            assert(assign_value_to_error.value() == 42);
            static_assert(noexcept(assign_value_to_error = Expected{in_place, 42}) == should_be_noexcept);

            Expected assign_error_to_error{unexpect, 1};
            assign_error_to_error = Expected{unexpect, 1337};
            assert(!assign_error_to_error);
            assert(assign_error_to_error.error() == 1337);
            static_assert(noexcept(assign_error_to_error = Expected{unexpect, 1337}) == should_be_noexcept);
        }

        { // assign same expected<void> as rvalue check error
            constexpr bool should_be_noexcept = nothrow_move_constructible && nothrow_move_assignable;
            using Expected                    = expected<void, payload_assign>;

            Expected assign_value_to_value{in_place};
            assign_value_to_value = Expected{in_place};
            assert(assign_value_to_value);
            static_assert(noexcept(assign_value_to_value = Expected{in_place}) == should_be_noexcept);

            Expected assign_error_to_value{in_place};
            assign_error_to_value = Expected{unexpect, 1337};
            assert(!assign_error_to_value);
            assert(assign_error_to_value.error() == 1337);
            static_assert(noexcept(assign_error_to_value = Expected{unexpect, 1337}) == should_be_noexcept);

            Expected assign_value_to_error{unexpect, 1};
            assign_value_to_error = Expected{in_place};
            assert(assign_value_to_error);
            static_assert(noexcept(assign_value_to_error = Expected{in_place}) == should_be_noexcept);

            Expected assign_error_to_error{unexpect, 1};
            assign_error_to_error = Expected{unexpect, 1337};
            assert(!assign_error_to_error);
            assert(assign_error_to_error.error() == 1337);
            static_assert(noexcept(assign_error_to_error = Expected{unexpect, 1337}) == should_be_noexcept);
        }

        { // assign base type const ref
            constexpr bool should_be_noexcept = nothrow_copy_constructible && nothrow_copy_assignable;
            using Expected                    = expected<payload_assign, int>;
            const payload_assign input_value{42};

            Expected assign_value_to_value{in_place, 1};
            assign_value_to_value = input_value;
            assert(assign_value_to_value);
            assert(assign_value_to_value.value() == 42);
            static_assert(noexcept(assign_value_to_value = input_value) == should_be_noexcept);

            Expected assign_value_to_error{unexpect, 1};
            assign_value_to_error = input_value;
            assert(assign_value_to_error);
            assert(assign_value_to_error.value() == 42);
            static_assert(noexcept(assign_value_to_error = input_value) == should_be_noexcept);
        }

        { // assign base type rvalue
            constexpr bool should_be_noexcept = nothrow_move_constructible && nothrow_move_assignable;
            using Expected                    = expected<payload_assign, int>;

            Expected assign_value_to_value{in_place, 1};
            assign_value_to_value = payload_assign{42};
            assert(assign_value_to_value);
            assert(assign_value_to_value.value() == 42);
            static_assert(noexcept(assign_value_to_value = payload_assign{42}) == should_be_noexcept);

            Expected assign_value_to_error{unexpect, 1};
            assign_value_to_error = payload_assign{42};
            assert(assign_value_to_error);
            assert(assign_value_to_error.value() == 42);
            static_assert(noexcept(assign_value_to_error = payload_assign{42}) == should_be_noexcept);
        }

        { // assign base type braces
            constexpr bool should_be_noexcept = nothrow_move_constructible && nothrow_move_assignable;
            using Expected                    = expected<payload_assign, int>;

            Expected assign_value_to_value{in_place, 1};
            assign_value_to_value = {42};
            assert(assign_value_to_value);
            assert(assign_value_to_value.value() == 42);
            static_assert(noexcept(assign_value_to_value = {42}) == should_be_noexcept);

            Expected assign_value_to_error{unexpect, 1};
            assign_value_to_error = {42};
            assert(assign_value_to_error);
            assert(assign_value_to_error.value() == 42);
            static_assert(noexcept(assign_value_to_error = {42}) == should_be_noexcept);
        }

        { // assign convertible type const ref
            constexpr bool should_be_noexcept = nothrow_copy_constructible && nothrow_copy_assignable;
            using Expected                    = expected<payload_assign, int>;
            const convertible input_value{42};

            Expected assign_value_to_value{in_place, 1};
            assign_value_to_value = input_value;
            assert(assign_value_to_value);
            assert(assign_value_to_value.value() == 42);
            static_assert(noexcept(assign_value_to_value = input_value) == should_be_noexcept);

            Expected assign_value_to_error{unexpect, 1};
            assign_value_to_error = input_value;
            assert(assign_value_to_error);
            assert(assign_value_to_error.value() == 42);
            static_assert(noexcept(assign_value_to_error = input_value) == should_be_noexcept);
        }

        { // assign convertible type rvalue
            constexpr bool should_be_noexcept = nothrow_move_constructible && nothrow_move_assignable;
            using Expected                    = expected<payload_assign, int>;

            Expected assign_value_to_value{in_place, 1};
            assign_value_to_value = convertible{42};
            assert(assign_value_to_value);
            assert(assign_value_to_value.value() == 42);
            static_assert(noexcept(assign_value_to_value = convertible{42}) == should_be_noexcept);

            Expected assign_value_to_error{unexpect, 1};
            assign_value_to_error = convertible{42};
            assert(assign_value_to_error);
            assert(assign_value_to_error.value() == 42);
            static_assert(noexcept(assign_value_to_error = convertible{42}) == should_be_noexcept);
        }

        { // assign error type const ref
            constexpr bool should_be_noexcept = nothrow_copy_constructible && nothrow_copy_assignable;
            using Expected                    = expected<int, payload_assign>;
            using Unexpected                  = std::unexpected<payload_assign>;
            const Unexpected input_error{42};

            Expected assign_error_to_value{in_place, 1};
            assign_error_to_value = input_error;
            assert(!assign_error_to_value);
            assert(assign_error_to_value.error() == 42);
            static_assert(noexcept(assign_error_to_value = input_error) == should_be_noexcept);

            Expected assign_error_to_error{unexpect, 1};
            assign_error_to_error = input_error;
            assert(!assign_error_to_error);
            assert(assign_error_to_error.error() == 42);
            static_assert(noexcept(assign_error_to_error = input_error) == should_be_noexcept);
        }

        { // assign expected<void> error type const ref
            constexpr bool should_be_noexcept = nothrow_copy_constructible && nothrow_copy_assignable;
            using Expected                    = expected<void, payload_assign>;
            using Unexpected                  = std::unexpected<payload_assign>;
            const Unexpected input_error{42};

            Expected assign_error_to_value{in_place};
            assign_error_to_value = input_error;
            assert(!assign_error_to_value);
            assert(assign_error_to_value.error() == 42);
            static_assert(noexcept(assign_error_to_value = input_error) == should_be_noexcept);

            Expected assign_error_to_error{unexpect, 1};
            assign_error_to_error = input_error;
            assert(!assign_error_to_error);
            assert(assign_error_to_error.error() == 42);
            static_assert(noexcept(assign_error_to_error = input_error) == should_be_noexcept);
        }

        { // assign error type rvalue
            constexpr bool should_be_noexcept = nothrow_move_constructible && nothrow_move_assignable;
            using Expected                    = expected<int, payload_assign>;
            using Unexpected                  = std::unexpected<payload_assign>;

            Expected assign_error_to_value{in_place, 1};
            assign_error_to_value = Unexpected{42};
            assert(!assign_error_to_value);
            assert(assign_error_to_value.error() == 42);
            static_assert(noexcept(assign_error_to_value = Unexpected{42}) == should_be_noexcept);

            Expected assign_error_to_error{unexpect, 1};
            assign_error_to_error = Unexpected{42};
            assert(!assign_error_to_error);
            assert(assign_error_to_error.error() == 42);
            static_assert(noexcept(assign_error_to_error = Unexpected{42}) == should_be_noexcept);
        }

        { // assign expected<void> error type rvalue
            constexpr bool should_be_noexcept = nothrow_move_constructible && nothrow_move_assignable;
            using Expected                    = expected<void, payload_assign>;
            using Unexpected                  = std::unexpected<payload_assign>;

            Expected assign_error_to_value{in_place};
            assign_error_to_value = Unexpected{42};
            assert(!assign_error_to_value);
            assert(assign_error_to_value.error() == 42);
            static_assert(noexcept(assign_error_to_value = Unexpected{42}) == should_be_noexcept);

            Expected assign_error_to_error{unexpect, 1};
            assign_error_to_error = Unexpected{42};
            assert(!assign_error_to_error);
            assert(assign_error_to_error.error() == 42);
            static_assert(noexcept(assign_error_to_error = Unexpected{42}) == should_be_noexcept);
        }

        { // assign convertible error const ref
            constexpr bool should_be_noexcept = nothrow_copy_constructible && nothrow_copy_assignable;
            using Expected                    = expected<int, payload_assign>;
            using Unexpected                  = std::unexpected<convertible>;
            const Unexpected input_error{42};

            Expected assign_error_to_value{in_place, 1};
            assign_error_to_value = input_error;
            assert(!assign_error_to_value);
            assert(assign_error_to_value.error() == 42);
            static_assert(noexcept(assign_error_to_value = input_error) == should_be_noexcept);

            Expected assign_error_to_error{unexpect, 1};
            assign_error_to_error = input_error;
            assert(!assign_error_to_error);
            assert(assign_error_to_error.error() == 42);
            static_assert(noexcept(assign_error_to_error = input_error) == should_be_noexcept);
        }

        { // assign expected<void> convertible error const ref
            constexpr bool should_be_noexcept = nothrow_copy_constructible && nothrow_copy_assignable;
            using Expected                    = expected<void, payload_assign>;
            using Unexpected                  = std::unexpected<convertible>;
            const Unexpected input_error{42};

            Expected assign_error_to_value{in_place};
            assign_error_to_value = input_error;
            assert(!assign_error_to_value);
            assert(assign_error_to_value.error() == 42);
            static_assert(noexcept(assign_error_to_value = input_error) == should_be_noexcept);

            Expected assign_error_to_error{unexpect, 1};
            assign_error_to_error = input_error;
            assert(!assign_error_to_error);
            assert(assign_error_to_error.error() == 42);
            static_assert(noexcept(assign_error_to_error = input_error) == should_be_noexcept);
        }

        { // assign convertible error rvalue
            constexpr bool should_be_noexcept = nothrow_move_constructible && nothrow_move_assignable;
            using Expected                    = expected<int, payload_assign>;
            using Unexpected                  = std::unexpected<convertible>;

            Expected assign_error_to_value{in_place, 1};
            assign_error_to_value = Unexpected{42};
            assert(!assign_error_to_value);
            assert(assign_error_to_value.error() == 42);
            static_assert(noexcept(assign_error_to_value = Unexpected{42}) == should_be_noexcept);

            Expected assign_error_to_error{unexpect, 1};
            assign_error_to_error = Unexpected{42};
            assert(!assign_error_to_error);
            assert(assign_error_to_error.error() == 42);
            static_assert(noexcept(assign_error_to_error = Unexpected{42}) == should_be_noexcept);
        }

        { // assign expected<void> convertible error rvalue
            constexpr bool should_be_noexcept = nothrow_move_constructible && nothrow_move_assignable;
            using Expected                    = expected<void, payload_assign>;
            using Unexpected                  = std::unexpected<convertible>;

            Expected assign_error_to_value{in_place};
            assign_error_to_value = Unexpected{42};
            assert(!assign_error_to_value);
            assert(assign_error_to_value.error() == 42);
            static_assert(noexcept(assign_error_to_value = Unexpected{42}) == should_be_noexcept);

            Expected assign_error_to_error{unexpect, 1};
            assign_error_to_error = Unexpected{42};
            assert(!assign_error_to_error);
            assert(assign_error_to_error.error() == 42);
            static_assert(noexcept(assign_error_to_error = Unexpected{42}) == should_be_noexcept);
        }

        { // ensure we are not copy_assignable if either the payload or the error are not copy_assignable or the payload
          // is not copy_constructible
            struct not_copy_assignable {
                not_copy_assignable& operator=(not_copy_assignable&&) = delete;
            };

            static_assert(!is_copy_assignable_v<expected<not_copy_assignable, int>>);
            static_assert(!is_copy_assignable_v<expected<int, not_copy_assignable>>);
            static_assert(!is_copy_assignable_v<expected<void, not_copy_assignable>>);

            struct not_copy_constructible {
                not_copy_constructible(const not_copy_constructible&) = delete;
            };

            static_assert(!is_copy_assignable_v<expected<not_copy_constructible, int>>);
        }

        { // ensure we are not move_assignable if either the payload or the error are not move_assignable or
          // move_constructible
            struct not_move_assignable {
                not_move_assignable& operator=(not_move_assignable&&) = delete;
            };

            static_assert(!is_move_assignable_v<expected<not_move_assignable, int>>);
            static_assert(!is_move_assignable_v<expected<int, not_move_assignable>>);
            static_assert(!is_move_assignable_v<expected<void, not_move_assignable>>);

            struct not_move_constructible {
                not_move_constructible(not_move_constructible&&) = delete;
            };

            static_assert(!is_move_assignable_v<expected<not_move_constructible, int>>);
            static_assert(!is_move_assignable_v<expected<int, not_move_constructible>>);
            static_assert(!is_move_assignable_v<expected<void, not_move_constructible>>);
        }
    }

    constexpr void test_assignment() noexcept {
        using NCC = IsNothrowCopyConstructible;
        using NMC = IsNothrowMoveConstructible;
        using NCA = IsNothrowCopyAssignable;
        using NMA = IsNothrowMoveAssignable;

        test_assignment<NCC::Not, NMC::Not, NCA::Not, NMA::Not>();
        test_assignment<NCC::Not, NMC::Not, NCA::Not, NMA::Yes>();
        test_assignment<NCC::Not, NMC::Not, NCA::Yes, NMA::Not>();
        test_assignment<NCC::Not, NMC::Not, NCA::Yes, NMA::Yes>();
        test_assignment<NCC::Not, NMC::Yes, NCA::Not, NMA::Not>();
        test_assignment<NCC::Not, NMC::Yes, NCA::Not, NMA::Yes>();
        test_assignment<NCC::Not, NMC::Yes, NCA::Yes, NMA::Not>();
        test_assignment<NCC::Not, NMC::Yes, NCA::Yes, NMA::Yes>();
        test_assignment<NCC::Yes, NMC::Not, NCA::Not, NMA::Not>();
        test_assignment<NCC::Yes, NMC::Not, NCA::Not, NMA::Yes>();
        test_assignment<NCC::Yes, NMC::Not, NCA::Yes, NMA::Not>();
        test_assignment<NCC::Yes, NMC::Not, NCA::Yes, NMA::Yes>();
        test_assignment<NCC::Yes, NMC::Yes, NCA::Not, NMA::Not>();
        test_assignment<NCC::Yes, NMC::Yes, NCA::Not, NMA::Yes>();
        test_assignment<NCC::Yes, NMC::Yes, NCA::Yes, NMA::Not>();
        test_assignment<NCC::Yes, NMC::Yes, NCA::Yes, NMA::Yes>();
    }

    constexpr void test_emplace() noexcept {
        struct payload_emplace {
            constexpr payload_emplace(bool& destructor_called) noexcept : _destructor_called(destructor_called) {}
            constexpr payload_emplace(bool& destructor_called, const convertible&) noexcept
                : _destructor_called(destructor_called), _val(3) {}
            constexpr payload_emplace(bool& destructor_called, convertible&&) noexcept
                : _destructor_called(destructor_called), _val(42) {}
            constexpr payload_emplace(initializer_list<int>&, bool& destructor_called, convertible) noexcept
                : _destructor_called(destructor_called), _val(1337) {}
            constexpr ~payload_emplace() {
                _destructor_called = true;
            }

            [[nodiscard]] constexpr bool operator==(const int val) const noexcept {
                return _val == val;
            }

            bool& _destructor_called;
            int _val = 0;
        };
        using Expected = expected<payload_emplace, int>;

        bool destructor_called = false;
        {
            const convertible input;
            Expected emplaced_lvalue(destructor_called);
            emplaced_lvalue.emplace(destructor_called, input);
            assert(destructor_called);
            assert(emplaced_lvalue);
            assert(emplaced_lvalue.value() == 3);
        }

        destructor_called = false;
        {
            const convertible input;
            Expected emplaced_lvalue(unexpect);
            emplaced_lvalue.emplace(destructor_called, input);
            assert(!destructor_called);
            assert(emplaced_lvalue);
            assert(emplaced_lvalue.value() == 3);
        }

        destructor_called = false;
        {
            Expected emplaced_rvalue(destructor_called);
            emplaced_rvalue.emplace(destructor_called, convertible{});
            assert(destructor_called);
            assert(emplaced_rvalue);
            assert(emplaced_rvalue.value() == 42);
        }

        destructor_called = false;
        {
            Expected emplaced_rvalue(unexpect);
            emplaced_rvalue.emplace(destructor_called, convertible{});
            assert(!destructor_called);
            assert(emplaced_rvalue);
            assert(emplaced_rvalue.value() == 42);
        }

        destructor_called = false;
        {
            Expected emplaced_ilist(destructor_called);
            emplaced_ilist.emplace({1}, destructor_called, convertible{});
            assert(destructor_called);
            assert(emplaced_ilist);
            assert(emplaced_ilist.value() == 1337);
        }

        destructor_called = false;
        {
            Expected emplaced_ilist(unexpect);
            emplaced_ilist.emplace({1}, destructor_called, convertible{});
            assert(!destructor_called);
            assert(emplaced_ilist);
            assert(emplaced_ilist.value() == 1337);
        }

        {
            using ExpectedVoid = expected<void, int>;
            ExpectedVoid with_value{in_place};
            with_value.emplace();
            assert(with_value);

            ExpectedVoid with_error{unexpect, 42};
            with_error.emplace();
            assert(with_error);
        }
    }

    template <IsNothrowMoveConstructible nothrowMoveConstructible, IsNothrowSwappable nothrowSwappable>
    struct payload_swap {
        constexpr payload_swap(const int val) noexcept : _val(val) {}
        constexpr payload_swap(const payload_swap&) noexcept = default;
        constexpr payload_swap(payload_swap&& other) noexcept(IsYes(nothrowMoveConstructible))
            : _val(other._val + 42) {}
        // Note: cannot declare friends of function local structs
        constexpr friend void swap(payload_swap& left, payload_swap& right) noexcept(IsYes(nothrowSwappable)) {
            left._val = exchange(right._val, left._val);
        }

        [[nodiscard]] constexpr bool operator==(const int val) const noexcept {
            return _val == val;
        }

        int _val = 0;
    };

    template <IsNothrowMoveConstructible nothrowMoveConstructible, IsNothrowSwappable nothrowSwappable>
    constexpr void test_swap() noexcept {
        constexpr bool nothrow_move_constructible = IsYes(nothrowMoveConstructible);
        constexpr bool should_be_noexcept         = nothrow_move_constructible && IsYes(nothrowSwappable);

        { // Check payload member
            using Expected = expected<payload_swap<nothrowMoveConstructible, nothrowSwappable>, int>;
            Expected first_value{1};
            Expected second_value{1337};
            Expected first_error{unexpect, 3};
            Expected second_error{unexpect, 5};

            first_value.swap(second_value);
            assert(first_value && second_value);
            assert(first_value.value() == 1337);
            assert(second_value.value() == 1);
            static_assert(noexcept(first_value.swap(second_value)) == should_be_noexcept);

            first_error.swap(second_error);
            assert(!first_error && !second_error);
            assert(first_error.error() == 5);
            assert(second_error.error() == 3);
            static_assert(noexcept(first_error.swap(second_error)) == should_be_noexcept);

            first_value.swap(first_error);
            assert(first_error && !first_value);
            assert(first_value.error() == 5);
            assert(first_error.value() == 1337 + 42);
            static_assert(noexcept(first_value.swap(first_error)) == should_be_noexcept);

            second_error.swap(second_value);
            assert(second_error && !second_value);
            assert(second_value.error() == 3);
            assert(second_error.value() == 1 + 42);
            static_assert(noexcept(second_error.swap(second_value)) == should_be_noexcept);
        }

        { // Check error member
            using Expected = expected<int, payload_swap<nothrowMoveConstructible, nothrowSwappable>>;
            Expected first_value{1};
            Expected second_value{1337};
            Expected first_error{unexpect, 3};
            Expected second_error{unexpect, 5};

            first_value.swap(second_value);
            assert(first_value && second_value);
            assert(first_value.value() == 1337);
            assert(second_value.value() == 1);
            static_assert(noexcept(first_value.swap(second_value)) == should_be_noexcept);

            first_error.swap(second_error);
            assert(!first_error && !second_error);
            assert(first_error.error() == 5);
            assert(second_error.error() == 3);
            static_assert(noexcept(first_error.swap(second_error)) == should_be_noexcept);

            first_value.swap(first_error);
            assert(first_error && !first_value);
            if constexpr (nothrow_move_constructible) {
                assert(first_value.error() == 5 + 42 + 42);
            } else {
                // Here we are storing _Ty as a temporary so we only move once
                assert(first_value.error() == 5 + 42);
            }
            assert(first_error.value() == 1337);
            static_assert(noexcept(first_value.swap(first_error)) == should_be_noexcept);

            second_error.swap(second_value);
            assert(second_error && !second_value);
            if constexpr (nothrow_move_constructible) {
                assert(second_value.error() == 3 + 42 + 42);
            } else {
                // Here we are storing _Ty as a temporary so we only move once
                assert(second_value.error() == 3 + 42);
            }
            assert(second_error.value() == 1);
            static_assert(noexcept(second_error.swap(second_value)) == should_be_noexcept);
        }

        { // Check expected<void> error member
            using Expected = expected<void, payload_swap<nothrowMoveConstructible, nothrowSwappable>>;
            Expected first_value{in_place};
            Expected second_value{in_place};
            Expected first_error{unexpect, 3};
            Expected second_error{unexpect, 5};

            first_value.swap(second_value);
            assert(first_value && second_value);
            static_assert(noexcept(first_value.swap(second_value)) == should_be_noexcept);

            first_error.swap(second_error);
            assert(!first_error && !second_error);
            assert(first_error.error() == 5);
            assert(second_error.error() == 3);
            static_assert(noexcept(first_error.swap(second_error)) == should_be_noexcept);

            first_value.swap(first_error);
            assert(first_error && !first_value);
            assert(first_value.error() == 5 + 42);
            static_assert(noexcept(first_value.swap(first_error)) == should_be_noexcept);

            second_error.swap(second_value);
            assert(second_error && !second_value);
            assert(second_value.error() == 3 + 42);
            static_assert(noexcept(second_error.swap(second_value)) == should_be_noexcept);
        }

        { // Check payload friend
            using Expected = expected<payload_swap<nothrowMoveConstructible, nothrowSwappable>, int>;
            Expected first_value{1};
            Expected second_value{1337};
            Expected first_error{unexpect, 3};
            Expected second_error{unexpect, 5};

            swap(first_value, second_value);
            assert(first_value && second_value);
            assert(first_value.value() == 1337);
            assert(second_value.value() == 1);
            static_assert(noexcept(swap(first_value, second_value)) == should_be_noexcept);

            swap(first_error, second_error);
            assert(!first_error && !second_error);
            assert(first_error.error() == 5);
            assert(second_error.error() == 3);
            static_assert(noexcept(swap(first_error, second_error)) == should_be_noexcept);

            swap(first_value, first_error);
            assert(first_error && !first_value);
            assert(first_value.error() == 5);
            assert(first_error.value() == 1337 + 42);
            static_assert(noexcept(swap(first_value, first_error)) == should_be_noexcept);

            swap(second_error, second_value);
            assert(second_error && !second_value);
            assert(second_value.error() == 3);
            assert(second_error.value() == 1 + 42);
            static_assert(noexcept(swap(second_error, second_value)) == should_be_noexcept);
        }

        { // Check error friend
            using Expected = expected<int, payload_swap<nothrowMoveConstructible, nothrowSwappable>>;
            Expected first_value{1};
            Expected second_value{1337};
            Expected first_error{unexpect, 3};
            Expected second_error{unexpect, 5};

            swap(first_value, second_value);
            assert(first_value && second_value);
            assert(first_value.value() == 1337);
            assert(second_value.value() == 1);
            static_assert(noexcept(swap(first_value, second_value)) == should_be_noexcept);

            swap(first_error, second_error);
            assert(!first_error && !second_error);
            assert(first_error.error() == 5);
            assert(second_error.error() == 3);
            static_assert(noexcept(swap(first_error, second_error)) == should_be_noexcept);

            swap(first_value, first_error);
            assert(first_error && !first_value);
            if constexpr (nothrow_move_constructible) {
                assert(first_value.error() == 5 + 42 + 42);
            } else {
                // Here we are storing _Ty as a temporary so we only move once
                assert(first_value.error() == 5 + 42);
            }
            assert(first_error.value() == 1337);
            static_assert(noexcept(swap(first_value, first_error)) == should_be_noexcept);

            swap(second_error, second_value);
            assert(second_error && !second_value);
            if constexpr (nothrow_move_constructible) {
                assert(second_value.error() == 3 + 42 + 42);
            } else {
                // Here we are storing _Ty as a temporary so we only move once
                assert(second_value.error() == 3 + 42);
            }
            assert(second_error.value() == 1);
            static_assert(noexcept(swap(second_error, second_value)) == should_be_noexcept);
        }

        { // Check expected<void> error friend
            using Expected = expected<void, payload_swap<nothrowMoveConstructible, nothrowSwappable>>;
            Expected first_value{in_place};
            Expected second_value{in_place};
            Expected first_error{unexpect, 3};
            Expected second_error{unexpect, 5};

            swap(first_value, second_value);
            assert(first_value && second_value);
            static_assert(noexcept(swap(first_value, second_value)) == should_be_noexcept);

            swap(first_error, second_error);
            assert(!first_error && !second_error);
            assert(first_error.error() == 5);
            assert(second_error.error() == 3);
            static_assert(noexcept(swap(first_error, second_error)) == should_be_noexcept);

            swap(first_value, first_error);
            assert(first_error && !first_value);
            assert(first_value.error() == 5 + 42);
            static_assert(noexcept(swap(first_value, first_error)) == should_be_noexcept);

            swap(second_error, second_value);
            assert(second_error && !second_value);
            assert(second_value.error() == 3 + 42);
            static_assert(noexcept(swap(second_error, second_value)) == should_be_noexcept);
        }
    }

    constexpr void test_swap() noexcept {
        test_swap<IsNothrowMoveConstructible::Not, IsNothrowSwappable::Not>();
        test_swap<IsNothrowMoveConstructible::Not, IsNothrowSwappable::Yes>();
        test_swap<IsNothrowMoveConstructible::Yes, IsNothrowSwappable::Not>();
        test_swap<IsNothrowMoveConstructible::Yes, IsNothrowSwappable::Yes>();
    }

    constexpr void test_access() noexcept {
        struct payload_access {
            payload_access* operator&()             = delete;
            const payload_access* operator&() const = delete;

            int x = 17;
            int y = 29;
        };

        { // operator->()
            using Expected = expected<payload_access, int>;
            Expected val;
            assert(val->x == 17);

            const Expected const_val;
            assert(const_val->y == 29);
        }

        { // operator*()
            using Expected = expected<convertible, int>;
            Expected lvalue{1};
            auto&& from_lvalue = *lvalue;
            assert(from_lvalue == 1);
            static_assert(is_same_v<decltype(from_lvalue), convertible&>);

            Expected rvalue{42};
            auto&& from_rvalue = *move(rvalue);
            assert(from_rvalue == 42);
            static_assert(is_same_v<decltype(from_rvalue), convertible&&>);

            const Expected const_lvalue{1337};
            auto&& from_const_lvalue = *const_lvalue;
            assert(from_const_lvalue == 1337);
            static_assert(is_same_v<decltype(from_const_lvalue), const convertible&>);

            const Expected const_rvalue{-42};
            auto&& from_const_rvalue = *move(const_rvalue);
            assert(from_const_rvalue == -42);
            static_assert(is_same_v<decltype(from_const_rvalue), const convertible&&>);
        }

        { // expected<void> operator*()
            using Expected = expected<void, int>;
            Expected lvalue{in_place};
            static_assert(is_same_v<decltype(*lvalue), void>);

            Expected rvalue{in_place};
            static_assert(is_same_v<decltype(*move(rvalue)), void>);

            const Expected const_lvalue{in_place};
            static_assert(is_same_v<decltype(*const_lvalue), void>);

            const Expected const_rvalue{in_place};
            static_assert(is_same_v<decltype(*move(const_rvalue)), void>);
        }

        { // operator bool()
            using Expected = expected<int, int>;
            const Expected defaulted;
            assert(defaulted);
            assert(defaulted.has_value());

            const Expected with_value{in_place, 5};
            assert(with_value);
            assert(with_value.has_value());

            const Expected with_error{unexpect, 5};
            assert(!with_error);
            assert(!with_error.has_value());
        }

        { // expected<void> operator bool()
            using Expected = expected<void, int>;
            const Expected defaulted;
            assert(defaulted);
            assert(defaulted.has_value());

            const Expected with_value{in_place};
            assert(with_value);
            assert(with_value.has_value());

            const Expected with_error{unexpect};
            assert(!with_error);
            assert(!with_error.has_value());
        }

        { // value()
            using Expected = expected<convertible, int>;
            Expected lvalue{1};
            auto&& from_lvalue = lvalue.value();
            assert(from_lvalue == 1);
            static_assert(is_same_v<decltype(from_lvalue), convertible&>);

            Expected rvalue{42};
            auto&& from_rvalue = move(rvalue).value();
            assert(from_rvalue == 42);
            static_assert(is_same_v<decltype(from_rvalue), convertible&&>);

            const Expected const_lvalue{1337};
            auto&& from_const_lvalue = const_lvalue.value();
            assert(from_const_lvalue == 1337);
            static_assert(is_same_v<decltype(from_const_lvalue), const convertible&>);

            const Expected const_rvalue{-42};
            auto&& from_const_rvalue = move(const_rvalue).value();
            assert(from_const_rvalue == -42);
            static_assert(is_same_v<decltype(from_const_rvalue), const convertible&&>);
        }

        { // expected<void> value()
            using Expected = expected<void, int>;
            Expected lvalue{in_place};
            static_assert(is_same_v<decltype(lvalue.value()), void>);

            Expected rvalue{in_place};
            static_assert(is_same_v<decltype(move(rvalue).value()), void>);

            const Expected const_lvalue{in_place};
            static_assert(is_same_v<decltype(const_lvalue.value()), void>);

            const Expected const_rvalue{in_place};
            static_assert(is_same_v<decltype(move(const_rvalue).value()), void>);
        }

        if (!is_constant_evaluated()) { // invalid value()
            using Expected = expected<int, convertible>;
            try {
                Expected lvalue{unexpect, 1};
                [[maybe_unused]] auto&& from_lvalue = lvalue.value();
                assert(false);
            } catch (bad_expected_access<convertible>& with_error) {
                assert(with_error.error() == 1);
                static_assert(is_same_v<decltype(with_error.error()), convertible&>);
            }

            try {
                Expected rvalue{unexpect, 42};
                [[maybe_unused]] auto&& from_rvalue = move(rvalue).value();
                assert(false);
            } catch (const bad_expected_access<convertible>& with_error) {
                assert(with_error.error() == 42);
                static_assert(is_same_v<decltype(with_error.error()), const convertible&>);
            }

            try {
                const Expected const_lvalue{unexpect, 1337};
                [[maybe_unused]] auto&& from_const_lvalue = const_lvalue.value();
                assert(false);
            } catch (bad_expected_access<convertible>& with_error) {
                assert(move(with_error).error() == 1337);
                static_assert(is_same_v<decltype(move(with_error).error()), convertible&&>);
            }

            try {
                const Expected const_rvalue{unexpect, -42};
                [[maybe_unused]] auto&& from_const_rvalue = move(const_rvalue).value();
                assert(false);
            } catch (const bad_expected_access<convertible>& with_error) {
                assert(move(with_error).error() == -42);
                static_assert(is_same_v<decltype(move(with_error).error()), const convertible&&>);
            }
        }

        if (!is_constant_evaluated()) { // expected<void> invalid value()
            using Expected = expected<void, convertible>;
            try {
                Expected lvalue{unexpect, 1};
                lvalue.value();
                assert(false);
            } catch (const bad_expected_access<convertible>& with_error) {
                assert(with_error.error() == 1);
            }

            try {
                Expected rvalue{unexpect, 42};
                move(rvalue).value();
                assert(false);
            } catch (const bad_expected_access<convertible>& with_error) {
                assert(with_error.error() == 42);
            }

            try {
                const Expected const_lvalue{unexpect, 1337};
                const_lvalue.value();
                assert(false);
            } catch (const bad_expected_access<convertible>& with_error) {
                assert(with_error.error() == 1337);
            }

            try {
                const Expected const_rvalue{unexpect, -42};
                move(const_rvalue).value();
                assert(false);
            } catch (const bad_expected_access<convertible>& with_error) {
                assert(with_error.error() == -42);
            }
        }

        { // error()
            using Expected = expected<int, convertible>;
            Expected lvalue{unexpect, 1};
            auto&& from_lvalue = lvalue.error();
            assert(from_lvalue == 1);
            static_assert(is_same_v<decltype(from_lvalue), convertible&>);

            Expected rvalue{unexpect, 42};
            auto&& from_rvalue = move(rvalue).error();
            assert(from_rvalue == 42);
            static_assert(is_same_v<decltype(from_rvalue), convertible&&>);

            const Expected const_lvalue{unexpect, 1337};
            auto&& from_const_lvalue = const_lvalue.error();
            assert(from_const_lvalue == 1337);
            static_assert(is_same_v<decltype(from_const_lvalue), const convertible&>);

            const Expected const_rvalue{unexpect, -42};
            auto&& from_const_rvalue = move(const_rvalue).error();
            assert(from_const_rvalue == -42);
            static_assert(is_same_v<decltype(from_const_rvalue), const convertible&&>);
        }

        { // expected<void> error()
            using Expected = expected<void, convertible>;
            Expected lvalue{unexpect, 1};
            auto&& from_lvalue = lvalue.error();
            assert(from_lvalue == 1);
            static_assert(is_same_v<decltype(from_lvalue), convertible&>);

            Expected rvalue{unexpect, 42};
            auto&& from_rvalue = move(rvalue).error();
            assert(from_rvalue == 42);
            static_assert(is_same_v<decltype(from_rvalue), convertible&&>);

            const Expected const_lvalue{unexpect, 1337};
            auto&& from_const_lvalue = const_lvalue.error();
            assert(from_const_lvalue == 1337);
            static_assert(is_same_v<decltype(from_const_lvalue), const convertible&>);

            const Expected const_rvalue{unexpect, -42};
            auto&& from_const_rvalue = move(const_rvalue).error();
            assert(from_const_rvalue == -42);
            static_assert(is_same_v<decltype(from_const_rvalue), const convertible&&>);
        }
    }

    template <IsNothrowConstructible nothrowConstructible, IsNothrowConvertible nothrowConvertible>
    constexpr void test_monadic() {
        constexpr bool construction_is_noexcept = IsYes(nothrowConstructible);
        constexpr bool conversion_is_noexcept   = IsYes(nothrowConvertible);
        constexpr bool should_be_noexcept       = construction_is_noexcept && conversion_is_noexcept;

        struct payload_monadic {
            constexpr payload_monadic(const int val) noexcept : _val(val) {}
            constexpr payload_monadic(const payload_monadic& other) noexcept(construction_is_noexcept)
                : _val(other._val + 2) {}
            constexpr payload_monadic(payload_monadic&& other) noexcept(construction_is_noexcept)
                : _val(other._val + 3) {}
            constexpr payload_monadic(const convertible& val) noexcept(conversion_is_noexcept) : _val(val._val + 4) {}
            constexpr payload_monadic(convertible&& val) noexcept(conversion_is_noexcept) : _val(val._val + 5) {}

            [[nodiscard]] constexpr bool operator==(const payload_monadic& right) const noexcept {
                return _val == right._val;
            }

            int _val = 0;
        };

        { // with payload argument
            using Expected = expected<payload_monadic, int>;

            Expected with_value{in_place, 42};
            const Expected const_with_value{in_place, 1337};
            assert(with_value.value_or(payload_monadic{1}) == 42 + 2);
            assert(const_with_value.value_or(payload_monadic{1}) == 1337 + 2);
            static_assert(noexcept(with_value.value_or(payload_monadic{1})) == construction_is_noexcept);
            static_assert(noexcept(const_with_value.value_or(payload_monadic{1})) == construction_is_noexcept);

            assert(move(with_value).value_or(payload_monadic{1}) == 42 + 3);
            assert(move(const_with_value).value_or(payload_monadic{1}) == 1337 + 2);
            static_assert(noexcept(move(with_value).value_or(payload_monadic{1})) == construction_is_noexcept);
            static_assert(noexcept(move(const_with_value).value_or(payload_monadic{1})) == construction_is_noexcept);

            const payload_monadic input{2};
            Expected with_error{unexpect, 42};
            const Expected const_with_error{unexpect, 1337};
            assert(with_error.value_or(payload_monadic{1}) == 1 + 3);
            assert(const_with_error.value_or(input) == 2 + 2);
            static_assert(noexcept(with_error.value_or(payload_monadic{1})) == construction_is_noexcept);
            static_assert(noexcept(const_with_error.value_or(input)) == construction_is_noexcept);

            assert(move(with_error).value_or(payload_monadic{1}) == 1 + 3);
            assert(move(const_with_error).value_or(input) == 2 + 2);
            static_assert(noexcept(move(with_error).value_or(payload_monadic{1})) == construction_is_noexcept);
            static_assert(noexcept(move(const_with_error).value_or(input)) == construction_is_noexcept);
        }

        { // with convertible argument
            using Expected = expected<payload_monadic, int>;

            Expected with_value{in_place, 42};
            const Expected const_with_value{in_place, 1337};
            assert(with_value.value_or(convertible{1}) == 42 + 2);
            assert(const_with_value.value_or(convertible{1}) == 1337 + 2);
            static_assert(noexcept(with_value.value_or(convertible{1})) == should_be_noexcept);
            static_assert(noexcept(const_with_value.value_or(convertible{1})) == should_be_noexcept);

            assert(move(with_value).value_or(convertible{1}) == 42 + 3);
            assert(move(const_with_value).value_or(convertible{1}) == 1337 + 2);
            static_assert(noexcept(move(with_value).value_or(convertible{1})) == should_be_noexcept);
            static_assert(noexcept(move(const_with_value).value_or(convertible{1})) == should_be_noexcept);

            const convertible input{2};
            Expected with_error{unexpect, 42};
            const Expected const_with_error{unexpect, 1337};
            assert(with_error.value_or(convertible{1}) == 1 + 5);
            assert(const_with_error.value_or(input) == 2 + 4);
            static_assert(noexcept(with_error.value_or(convertible{1})) == should_be_noexcept);
            static_assert(noexcept(const_with_error.value_or(input)) == should_be_noexcept);

            assert(move(with_error).value_or(convertible{1}) == 1 + 5);
            assert(move(const_with_error).value_or(input) == 2 + 4);
            static_assert(noexcept(move(with_error).value_or(convertible{1})) == should_be_noexcept);
            static_assert(noexcept(move(const_with_error).value_or(input)) == should_be_noexcept);
        }
    }

    constexpr void test_monadic() noexcept {
        test_monadic<IsNothrowConstructible::Not, IsNothrowConvertible::Not>();
        test_monadic<IsNothrowConstructible::Not, IsNothrowConvertible::Yes>();
        test_monadic<IsNothrowConstructible::Yes, IsNothrowConvertible::Not>();
        test_monadic<IsNothrowConstructible::Yes, IsNothrowConvertible::Yes>();
    }

    template <IsNothrowComparable nothrowComparable>
    constexpr void test_equality() noexcept {
        constexpr bool should_be_noexcept = IsYes(nothrowComparable);

        struct payload_equality {
            constexpr payload_equality(const int val) noexcept : _val(val) {}

            [[nodiscard]] constexpr bool operator==(const payload_equality& right) const noexcept(should_be_noexcept) {
                return _val == right._val;
            }

            int _val = 0;
        };

        { // compare against same expected
            using Expected = expected<payload_equality, int>;

            const Expected with_value1{in_place, 42};
            const Expected with_value2{in_place, 1337};
            assert(with_value1 == with_value1);
            assert(with_value1 != with_value2);
            static_assert(noexcept(with_value1 == with_value1) == should_be_noexcept);
            static_assert(noexcept(with_value1 != with_value2) == should_be_noexcept);

            const Expected error1{unexpect, 42};
            const Expected error2{unexpect, 1337};
            assert(error1 == error1);
            assert(error1 != error2);
            static_assert(noexcept(error1 == error1) == should_be_noexcept);
            static_assert(noexcept(error1 != error2) == should_be_noexcept);

            assert(with_value1 != error1);
            static_assert(noexcept(with_value1 != error1) == should_be_noexcept);
        }

        { // expected<void> compare against same expected
            using Expected = expected<void, payload_equality>;

            const Expected with_value{in_place};
            assert(with_value == with_value);
            assert(!(with_value != with_value));
            static_assert(noexcept(with_value == with_value) == should_be_noexcept);
            static_assert(noexcept(with_value != with_value) == should_be_noexcept);

            const Expected error1{unexpect, 42};
            const Expected error2{unexpect, 1337};
            assert(error1 == error1);
            assert(error1 != error2);
            static_assert(noexcept(error1 == error1) == should_be_noexcept);
            static_assert(noexcept(error1 != error2) == should_be_noexcept);

            assert(with_value != error1);
            static_assert(noexcept(with_value != error1) == should_be_noexcept);
        }

        { // compare against different expected
            using Expected      = expected<payload_equality, int>;
            using OtherExpected = expected<int, payload_equality>;

            const Expected with_value1{in_place, 42};
            const OtherExpected with_value2{in_place, 1337};
            assert(with_value1 == with_value1);
            assert(with_value1 != with_value2);
            static_assert(noexcept(with_value1 == with_value1) == should_be_noexcept);
            static_assert(noexcept(with_value1 != with_value2) == should_be_noexcept);

            const Expected error1{unexpect, 42};
            const OtherExpected error2{unexpect, 1337};
            assert(error1 == error1);
            assert(error1 != error2);
            static_assert(noexcept(error1 == error1) == should_be_noexcept);
            static_assert(noexcept(error1 != error2) == should_be_noexcept);

            assert(with_value1 != error1);
            static_assert(noexcept(with_value1 != error1) == should_be_noexcept);
        }

        { // compare against base type
            using Base     = payload_equality;
            using Expected = expected<Base, int>;

            const Expected with_value{in_place, 42};
            const Expected with_error{unexpect, 1337};
            assert(with_value == Base{42});
            assert(with_value != Base{1337});
            static_assert(noexcept(with_value == Base{42}) == should_be_noexcept);
            static_assert(noexcept(with_value != Base{1337}) == should_be_noexcept);

            assert(with_error != 1337);
            static_assert(noexcept(with_error != 1337) == should_be_noexcept);

            assert(with_error != Base{1337});
            static_assert(noexcept(with_error != Base{1337}) == should_be_noexcept);
        }

        { // compare against unexpected with same base
            using Base       = payload_equality;
            using Unexpected = std::unexpected<Base>;
            using Expected   = expected<int, Base>;

            const Expected with_value{in_place, 42};
            const Expected with_error{unexpect, 1337};
            assert(with_value != Unexpected{1337});
            static_assert(noexcept(with_value != Unexpected{1337}) == should_be_noexcept);

            assert(with_error == Unexpected{1337});
            assert(with_error != Unexpected{42});
            static_assert(noexcept(with_error == Unexpected{1337}) == should_be_noexcept);
            static_assert(noexcept(with_error != Unexpected{42}) == should_be_noexcept);

            assert(with_error != Base{1337});
            static_assert(noexcept(with_error != Base{1337}) == should_be_noexcept);
        }

        { // expected<void> compare against unexpected with same base
            using Base       = payload_equality;
            using Unexpected = std::unexpected<Base>;
            using Expected   = expected<void, Base>;

            const Expected with_value{in_place};
            const Expected with_error{unexpect, 1337};
            assert(with_value != Unexpected{1337});
            static_assert(noexcept(with_value != Unexpected{1337}) == should_be_noexcept);

            assert(with_error == Unexpected{1337});
            assert(with_error != Unexpected{42});
            static_assert(noexcept(with_error == Unexpected{1337}) == should_be_noexcept);
            static_assert(noexcept(with_error != Unexpected{42}) == should_be_noexcept);
        }

        { // compare against unexpected with different base
            using Base       = payload_equality;
            using Unexpected = std::unexpected<int>;
            using Expected   = expected<int, Base>;

            const Expected with_value{in_place, 42};
            const Expected with_error{unexpect, 1337};
            assert(with_value != Unexpected{1337});
            static_assert(noexcept(with_value != Unexpected{1337}) == should_be_noexcept);

            assert(with_error == Unexpected{1337});
            assert(with_error != Unexpected{42});
            static_assert(noexcept(with_error == Unexpected{1337}) == should_be_noexcept);
            static_assert(noexcept(with_error != Unexpected{42}) == should_be_noexcept);

            assert(with_error != Base{1337});
            static_assert(noexcept(with_error != Base{1337}) == should_be_noexcept);
        }

        { // expected<void> compare against unexpected with different base
            using Base       = payload_equality;
            using Unexpected = std::unexpected<int>;
            using Expected   = expected<void, Base>;

            const Expected with_value{in_place};
            const Expected with_error{unexpect, 1337};
            assert(with_value != Unexpected{1337});
            static_assert(noexcept(with_value != Unexpected{1337}) == should_be_noexcept);

            assert(with_error == Unexpected{1337});
            assert(with_error != Unexpected{42});
            static_assert(noexcept(with_error == Unexpected{1337}) == should_be_noexcept);
            static_assert(noexcept(with_error != Unexpected{42}) == should_be_noexcept);
        }
    }

    constexpr void test_equality() noexcept {
        test_equality<IsNothrowComparable::Not>();
        test_equality<IsNothrowComparable::Yes>();
    }

    constexpr bool test_all() noexcept {
        test_aliases();
        test_special_members();
        test_constructors();
        test_assignment();
        test_emplace();
        test_swap();
        test_access();
        test_monadic();
        test_equality();

        return true;
    }
} // namespace test_expected

void test_reinit_regression() {
    // _Reinit_expected had a bug in its conditional noexcept that would terminate the program
    // when switching from error state to value state when the value type is nothrow-movable
    // but the conversion throws.

    constexpr int magic = 1729;

    struct throwing_int_conversion {
        [[noreturn]] operator int() const {
            throw magic;
        }
    };

    expected<int, bool> e{unexpect, false};

    try {
        e = throwing_int_conversion{};
        assert(false);
    } catch (const int& i) {
        assert(i == magic);
    }
}

int main() {
    test_unexpected::test_all();
    static_assert(test_unexpected::test_all());
    test_expected::test_all();
    static_assert(test_expected::test_all());

    static_assert(is_base_of_v<bad_expected_access<void>, bad_expected_access<int>>);
    static_assert(is_base_of_v<exception, bad_expected_access<void>>);
    static_assert(is_base_of_v<exception, bad_expected_access<int>>);

    static_assert(is_convertible_v<bad_expected_access<int>*, bad_expected_access<void>*>);
    static_assert(is_convertible_v<bad_expected_access<void>*, exception*>);
    static_assert(is_convertible_v<bad_expected_access<int>*, exception*>);

    test_reinit_regression();
}
