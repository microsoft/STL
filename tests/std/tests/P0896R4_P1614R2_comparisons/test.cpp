// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Covers:
// * three_way_comparable and three_way_comparable_with
// * compare_three_way, compare_three_way_result, and compare_three_way_result_t
// * ranges::equal_to, ranges::not_equal_to, ranges::less, ranges::less_equal,
//   ranges::greater, and ranges::greater_equal

#ifdef __clang__
#pragma clang diagnostic ignored "-Wsign-compare"
#endif // __clang__

#include <cassert>
#include <compare>
#include <concepts>
#include <functional>
#include <ranges>
#include <type_traits>
#include <utility>

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

namespace ranges = std::ranges;

using std::common_comparison_category_t;
using std::compare_three_way, std::compare_three_way_result, std::compare_three_way_result_t;
using std::partial_ordering, std::weak_ordering, std::strong_ordering;
using std::same_as, std::convertible_to;
using std::three_way_comparable, std::three_way_comparable_with;

template <class T, class... Types>
constexpr bool is_one_of = (same_as<T, Types> || ...);

struct common_comparable {
    template <class T>
    common_comparable(T&&);

    bool operator==(common_comparable const&) const;
    strong_ordering operator<=>(common_comparable const&) const;
};

struct common_incomparable {
    template <class T>
    common_incomparable(T&&);
};

// Validate properties common to the concept-constrained comparison object types
template <class T>
constexpr bool is_trivially_constexpr() {
    STATIC_ASSERT(std::semiregular<T>);

    // Not required, but likely portable nonetheless:
    STATIC_ASSERT(std::is_empty_v<T>);
    STATIC_ASSERT(std::is_trivial_v<T>);
    STATIC_ASSERT(std::is_trivially_copy_constructible_v<T>);
    STATIC_ASSERT(std::is_trivially_move_constructible_v<T>);
    STATIC_ASSERT(std::is_trivially_copy_assignable_v<T>);
    STATIC_ASSERT(std::is_trivially_move_assignable_v<T>);

    // Not required to be constant expressions, but likely portable nonetheless:
    T value_initialized{};
    T copy_constructed = value_initialized;
    T move_constructed = std::move(copy_constructed);
    copy_constructed   = std::move(move_constructed);
    move_constructed   = copy_constructed;

    return true;
}

STATIC_ASSERT(is_trivially_constexpr<compare_three_way>());
STATIC_ASSERT(is_trivially_constexpr<ranges::equal_to>());
STATIC_ASSERT(is_trivially_constexpr<ranges::not_equal_to>());
STATIC_ASSERT(is_trivially_constexpr<ranges::less>());
STATIC_ASSERT(is_trivially_constexpr<ranges::less_equal>());
STATIC_ASSERT(is_trivially_constexpr<ranges::greater>());
STATIC_ASSERT(is_trivially_constexpr<ranges::greater_equal>());

// Validate three_way_comparable
template <int I, class Category>
struct three_way_archetype {
    three_way_archetype()                                      = delete;
    three_way_archetype(three_way_archetype const&)            = delete;
    three_way_archetype& operator=(three_way_archetype const&) = delete;
    ~three_way_archetype()                                     = delete;
    // clang-format off
    // 0: not equality_comparable
    bool operator==(three_way_archetype const&) const requires (I == 0) = delete;
    bool operator==(three_way_archetype const&) const requires (I != 0);
    // 1: not totally_ordered
    bool operator<(three_way_archetype const&) const requires (I == 1) = delete;
    bool operator<(three_way_archetype const&) const requires (I != 1);
    bool operator>(three_way_archetype const&) const requires (I != 1);
    bool operator<=(three_way_archetype const&) const requires (I != 1);
    bool operator>=(three_way_archetype const&) const requires (I != 1);
    // 2: <=> isn't defined
    Category operator<=>(three_way_archetype const&) const requires (I != 2 && I != 3);
    // 3: <=> doesn't return a comparison category type
    int operator<=>(three_way_archetype const&) const requires (I == 3);
    // clang-format on
};
constexpr int three_way_archetype_max = 4;

template <class T, class Cat>
constexpr bool test_three_way_comparable1() {
    STATIC_ASSERT(is_one_of<Cat, void, partial_ordering, weak_ordering, strong_ordering>);

    STATIC_ASSERT(three_way_comparable<T, partial_ordering> == convertible_to<Cat, partial_ordering>);
    STATIC_ASSERT(three_way_comparable<T, weak_ordering> == convertible_to<Cat, weak_ordering>);
    STATIC_ASSERT(three_way_comparable<T, strong_ordering> == same_as<Cat, strong_ordering>);

    STATIC_ASSERT(three_way_comparable_with<T, T, partial_ordering> == convertible_to<Cat, partial_ordering>);
    STATIC_ASSERT(three_way_comparable_with<T, T, weak_ordering> == convertible_to<Cat, weak_ordering>);
    STATIC_ASSERT(three_way_comparable_with<T, T, strong_ordering> == same_as<Cat, strong_ordering>);

    return true;
}

template <int... Is>
constexpr bool test_three_way_comparable(std::integer_sequence<int, Is...>) {
    (test_three_way_comparable1<three_way_archetype<Is, partial_ordering>, void>(), ...);
    (test_three_way_comparable1<three_way_archetype<Is, weak_ordering>, void>(), ...);
    (test_three_way_comparable1<three_way_archetype<Is, strong_ordering>, void>(), ...);
    STATIC_ASSERT(
        test_three_way_comparable1<three_way_archetype<three_way_archetype_max, partial_ordering>, partial_ordering>());
    STATIC_ASSERT(
        test_three_way_comparable1<three_way_archetype<three_way_archetype_max, weak_ordering>, weak_ordering>());
    STATIC_ASSERT(
        test_three_way_comparable1<three_way_archetype<three_way_archetype_max, strong_ordering>, strong_ordering>());

    return true;
}
STATIC_ASSERT(test_three_way_comparable(std::make_integer_sequence<int, three_way_archetype_max>{}));

// Validate three_way_comparable_with

// 4: not common_reference_with
// 5: common_reference_t is not three_way_comparable
template <int I1, class Cat1, int I2, class Cat2>
    requires ((I1 != I2 || !same_as<Cat1, Cat2>) && I1 != 4 && I2 != 4)
struct std::common_type<three_way_archetype<I1, Cat1>, three_way_archetype<I2, Cat2>> {
    using type = conditional_t<I1 == 5 || I2 == 5, ::common_incomparable, ::common_comparable>;
};

// clang-format off

// 6: not _Weakly_equality_comparable_with
template <int I1, class Cat1, int I2, class Cat2>
    requires ((I1 != I2 || !same_as<Cat1, Cat2>) && (I1 == 6 || I2 == 6))
bool operator==(three_way_archetype<I1, Cat1> const&, three_way_archetype<I2, Cat2> const&) = delete;
template <int I1, class Cat1, int I2, class Cat2>
    requires ((I1 != I2 || !same_as<Cat1, Cat2>) && I1 != 6 && I2 != 6)
bool operator==(three_way_archetype<I1, Cat1> const&, three_way_archetype<I2, Cat2> const&);

// 7: not _Partially_ordered_with
template <int I1, class Cat1, int I2, class Cat2>
    requires ((I1 != I2 || !same_as<Cat1, Cat2>) && (I1 == 7 || I2 == 7))
bool operator<(three_way_archetype<I1, Cat1> const&, three_way_archetype<I2, Cat2> const&) = delete;
template <int I1, class Cat1, int I2, class Cat2>
    requires ((I1 != I2 || !same_as<Cat1, Cat2>) && I1 != 7 && I2 != 7)
bool operator<(three_way_archetype<I1, Cat1> const&, three_way_archetype<I2, Cat2> const&);
template <int I1, class Cat1, int I2, class Cat2>
    requires (I1 != I2 || !same_as<Cat1, Cat2>)
bool operator>(three_way_archetype<I1, Cat1> const&, three_way_archetype<I2, Cat2> const&);
template <int I1, class Cat1, int I2, class Cat2>
    requires (I1 != I2 || !same_as<Cat1, Cat2>)
bool operator<=(three_way_archetype<I1, Cat1> const&, three_way_archetype<I2, Cat2> const&);
template <int I1, class Cat1, int I2, class Cat2>
    requires (I1 != I2 || !same_as<Cat1, Cat2>)
bool operator>=(three_way_archetype<I1, Cat1> const&, three_way_archetype<I2, Cat2> const&);

// clang-format on

// 8: <=> isn't defined
template <int I1, class Cat1, int I2, class Cat2>
    requires ((I1 != I2 || !same_as<Cat1, Cat2>) && I1 != 8 && I1 != 9 && I2 != 8 && I2 != 9)
common_comparison_category_t<Cat1, Cat2> operator<=>(
    three_way_archetype<I1, Cat1> const&, three_way_archetype<I2, Cat2> const&);

// 9: <=> returns a non-comparison category type
template <int I1, class Cat1, int I2, class Cat2>
    requires ((I1 != I2 || !same_as<Cat1, Cat2>) && I1 == 9 && I2 == 9)
int operator<=>(three_way_archetype<I1, Cat1> const&, three_way_archetype<I2, Cat2> const&);

constexpr int three_way_with_max = 10;

template <class T, class Cat>
constexpr bool test_three_way_comparable_with1() {
    STATIC_ASSERT(is_one_of<Cat, void, partial_ordering, weak_ordering, strong_ordering>);

    // All specializations of three_way_archetype<I, T> for which I >= three_way_with_max are "good"; we need such a
    // specialization that is different from three_way_archetype<three_way_with_max, T> to ensure we're fully testing
    // the cross-type three_way_comparable_with concept. Why not three_way_with_max + 1?
    constexpr int three_way_known_good = three_way_with_max + 1;

    using P = three_way_archetype<three_way_known_good, partial_ordering>;
    using W = three_way_archetype<three_way_known_good, weak_ordering>;
    using S = three_way_archetype<three_way_known_good, strong_ordering>;

    STATIC_ASSERT(three_way_comparable_with<T, P, partial_ordering> == convertible_to<Cat, partial_ordering>);
    STATIC_ASSERT(three_way_comparable_with<T, W, weak_ordering> == convertible_to<Cat, weak_ordering>);
    STATIC_ASSERT(three_way_comparable_with<T, S, strong_ordering> == same_as<Cat, strong_ordering>);

    return true;
}

template <int... Is>
constexpr bool test_three_way_comparable_with(std::integer_sequence<int, Is...>) {
    (test_three_way_comparable_with1<three_way_archetype<Is, partial_ordering>, void>(), ...);
    (test_three_way_comparable_with1<three_way_archetype<Is, weak_ordering>, void>(), ...);
    (test_three_way_comparable_with1<three_way_archetype<Is, strong_ordering>, void>(), ...);
    STATIC_ASSERT(
        test_three_way_comparable_with1<three_way_archetype<three_way_with_max, partial_ordering>, partial_ordering>());
    STATIC_ASSERT(
        test_three_way_comparable_with1<three_way_archetype<three_way_with_max, weak_ordering>, weak_ordering>());
    STATIC_ASSERT(
        test_three_way_comparable_with1<three_way_archetype<three_way_with_max, strong_ordering>, strong_ordering>());

    return true;
}
STATIC_ASSERT(test_three_way_comparable_with(std::make_integer_sequence<int, three_way_with_max>{}));

// Validate static properties of compare_three_way, compare_three_way_result, and compare_three_way_result_t
template <class T>
concept is_trait = requires { typename T::type; };

template <class T, class U>
concept can_three_way = requires(T const& t, U const& u) { t <=> u; };

template <class T, class U, class Cat>
constexpr bool test_compare_three_way() {
    STATIC_ASSERT(same_as<T, std::remove_cvref_t<T>>);
    STATIC_ASSERT(same_as<U, std::remove_cvref_t<U>>);

    STATIC_ASSERT(can_three_way<T, U> == !std::is_void_v<Cat>);
    STATIC_ASSERT(can_three_way<U, T> == !std::is_void_v<Cat>);
    if constexpr (can_three_way<T, U>) {
        STATIC_ASSERT(same_as<decltype(std::declval<T const&>() <=> std::declval<U const&>()), Cat>);
        STATIC_ASSERT(same_as<decltype(std::declval<U const&>() <=> std::declval<T const&>()), Cat>);
        STATIC_ASSERT(same_as<compare_three_way_result_t<T, U>, Cat>);
        STATIC_ASSERT(same_as<compare_three_way_result_t<U, T>, Cat>);
        STATIC_ASSERT(same_as<typename compare_three_way_result<T, U>::type, Cat>);
        STATIC_ASSERT(same_as<typename compare_three_way_result<U, T>::type, Cat>);
        STATIC_ASSERT(same_as<decltype(compare_three_way{}(std::declval<T const&>(), std::declval<U const&>())), Cat>);
        STATIC_ASSERT(same_as<decltype(compare_three_way{}(std::declval<U const&>(), std::declval<T const&>())), Cat>);
    } else {
        STATIC_ASSERT(!is_trait<compare_three_way_result<T, U>>);
        STATIC_ASSERT(!is_trait<compare_three_way_result<U, T>>);
    }

    return true;
}

enum class some_enum { value };

STATIC_ASSERT(test_compare_three_way<int, int, strong_ordering>());
STATIC_ASSERT(test_compare_three_way<int, long, strong_ordering>());
STATIC_ASSERT(test_compare_three_way<float, float, partial_ordering>());
STATIC_ASSERT(test_compare_three_way<float, double, partial_ordering>());
STATIC_ASSERT(test_compare_three_way<long, double, partial_ordering>());
STATIC_ASSERT(test_compare_three_way<bool, int, void>());

STATIC_ASSERT(test_compare_three_way<some_enum, some_enum, strong_ordering>());
STATIC_ASSERT(test_compare_three_way<some_enum, int, void>());

STATIC_ASSERT(test_compare_three_way<int*, int*, strong_ordering>());
STATIC_ASSERT(test_compare_three_way<int*, void*, strong_ordering>());

STATIC_ASSERT(test_compare_three_way<int (*)(), int (*)(), void>());
STATIC_ASSERT(test_compare_three_way<int (*)(), void (*)(), void>());

template <class Cat>
struct compares_as {};
template <class Cat1, class Cat2>
bool operator==(compares_as<Cat1> const&, compares_as<Cat2> const&);
template <class Cat1, class Cat2>
common_comparison_category_t<Cat1, Cat2> operator<=>(compares_as<Cat1> const&, compares_as<Cat2> const&);

template <class Cat1, class Cat2>
struct std::common_type<compares_as<Cat1>, compares_as<Cat2>> {
    using type = common_comparable;
};

STATIC_ASSERT(test_compare_three_way<compares_as<partial_ordering>, compares_as<partial_ordering>, partial_ordering>());
STATIC_ASSERT(test_compare_three_way<compares_as<partial_ordering>, compares_as<weak_ordering>, partial_ordering>());
STATIC_ASSERT(test_compare_three_way<compares_as<partial_ordering>, compares_as<strong_ordering>, partial_ordering>());
STATIC_ASSERT(test_compare_three_way<compares_as<weak_ordering>, compares_as<weak_ordering>, weak_ordering>());
STATIC_ASSERT(test_compare_three_way<compares_as<weak_ordering>, compares_as<strong_ordering>, weak_ordering>());
STATIC_ASSERT(test_compare_three_way<compares_as<strong_ordering>, compares_as<strong_ordering>, strong_ordering>());

// Validate dynamic properties of compare_three_way, ranges::equal_to, ranges::not_equal_to, ranges::less,
// ranges::less_equal, ranges::greater, ranges::greater_equal
#define assert_three_way(t, u, result) assert(compare_three_way{}((t), (u)) == (result))

template <class T, class U>
constexpr void test_equality_comparable(T const& t, U const& u, strong_ordering const o) {
    assert(ranges::equal_to{}(t, t));
    assert(ranges::equal_to{}(u, u));
    assert(ranges::equal_to{}(t, u) == (o == strong_ordering::equal));
    assert(ranges::equal_to{}(u, t) == (o == strong_ordering::equal));

    assert(!ranges::not_equal_to{}(t, t));
    assert(!ranges::not_equal_to{}(u, u));
    assert(ranges::not_equal_to{}(t, u) == !(o == strong_ordering::equal));
    assert(ranges::not_equal_to{}(u, t) == !(o == strong_ordering::equal));
}

template <class T, class U>
constexpr void test_totally_ordered(T const& t, U const& u, strong_ordering const o) {
    test_equality_comparable(t, u, o);

    assert(!ranges::less{}(t, t));
    assert(!ranges::less{}(u, u));
    assert(ranges::less{}(t, u) == (o == strong_ordering::less));
    assert(ranges::less{}(u, t) == (o == strong_ordering::greater));

    assert(!ranges::greater{}(t, t));
    assert(!ranges::greater{}(u, u));
    assert(ranges::greater{}(t, u) == (o == strong_ordering::greater));
    assert(ranges::greater{}(u, t) == (o == strong_ordering::less));

    assert(ranges::less_equal{}(t, t));
    assert(ranges::less_equal{}(u, u));
    assert(ranges::less_equal{}(t, u) == !(o == strong_ordering::greater));
    assert(ranges::less_equal{}(u, t) == !(o == strong_ordering::less));

    assert(ranges::greater_equal{}(t, t));
    assert(ranges::greater_equal{}(u, u));
    assert(ranges::greater_equal{}(t, u) == !(o == strong_ordering::less));
    assert(ranges::greater_equal{}(u, t) == !(o == strong_ordering::greater));
}

template <class T, class U>
constexpr void test_strongly_ordered(T const& t, U const& u) {
    assert(t < u);
    test_totally_ordered(t, u, strong_ordering::less);

    assert_three_way(t, t, strong_ordering::equal);
    assert_three_way(u, u, strong_ordering::equal);
    assert_three_way(t, u, strong_ordering::less);
    assert_three_way(u, t, strong_ordering::greater);
}

template <class T, class U>
constexpr void test_weakly_ordered(T const& t, U const& u, weak_ordering const o) {
    assert_three_way(t, u, o);

    strong_ordering test_as = strong_ordering::equal;

    if (o == weak_ordering::equivalent) {
        assert_three_way(u, t, weak_ordering::equivalent);
        test_as = strong_ordering::equal;
    } else if (o == weak_ordering::less) {
        assert_three_way(u, t, weak_ordering::greater);
        test_as = strong_ordering::less;
    } else {
        assert(o == weak_ordering::greater);
        assert_three_way(u, t, weak_ordering::less);
        test_as = strong_ordering::greater;
    }

    test_totally_ordered(t, u, test_as);
}

template <class T, class U>
constexpr void test_partially_ordered(T const& t, U const& u, partial_ordering const o) {
    assert_three_way(t, u, o);

    strong_ordering test_as = strong_ordering::equal;

    if (o == partial_ordering::equivalent) {
        assert_three_way(u, t, partial_ordering::equivalent);
        test_as = strong_ordering::equal;
    } else if (o == partial_ordering::less) {
        assert_three_way(u, t, partial_ordering::greater);
        test_as = strong_ordering::less;
    } else if (o == partial_ordering::greater) {
        assert_three_way(u, t, partial_ordering::less);
        test_as = strong_ordering::greater;
    } else {
        assert(o == partial_ordering::unordered);
        assert_three_way(u, t, partial_ordering::unordered);
        return;
    }

    test_totally_ordered(t, u, test_as);
}

void f1() {}
void f2() {}

struct base {};
struct derived : base {};

enum unscoped : int {};
enum class scoped {};

// TRANSITION, VSO-980378 (use numeric_limits::quiet_NaN)
constexpr auto NaN  = __builtin_nan("0");
constexpr auto NaNf = __builtin_nanf("0");

constexpr void ordering_test_cases() {
    // Validate types strongly ordered by builtin <=> operators
    test_strongly_ordered(false, true); // bool (but not with other integral types)

    test_strongly_ordered(13, 42); // integral types (but not mixed-sign)
    test_strongly_ordered(13, 42L);
    test_strongly_ordered(13L, 42);
    test_strongly_ordered(13U, 42U);
    test_strongly_ordered(13U, 42UL);
    test_strongly_ordered(13UL, 42U);
    test_strongly_ordered(13U, L'x');
#ifdef __cpp_char8_t
    test_strongly_ordered(13U, u8'x');
#endif // __cpp_char8_t
    test_strongly_ordered(13U, u'x');
    test_strongly_ordered(13U, U'x');

    test_strongly_ordered(scoped{13}, scoped{42});
    test_strongly_ordered(unscoped{13}, unscoped{42});

    int const some_ints[] = {13, 42};
    test_strongly_ordered(&some_ints[0], &some_ints[1]);
    std::pair<int, int> const int_pair{13, 42};
    test_strongly_ordered(&int_pair.first, &int_pair.second);

    derived const some_deriveds[2] = {};
    test_strongly_ordered(&some_deriveds[0], &some_deriveds[1]);
#if !defined(__clang__) && !defined(__EDG__) // TRANSITION, VSO-1168721
    if (!std::is_constant_evaluated())
#endif // TRANSITION, VSO-1168721
    {
        test_strongly_ordered(static_cast<base const*>(&some_deriveds[0]), &some_deriveds[1]);
        test_strongly_ordered(&some_deriveds[0], static_cast<base const*>(&some_deriveds[1]));
    }

    if (!std::is_constant_evaluated()) {
        test_strongly_ordered(&some_ints[0], static_cast<void const*>(&some_ints[1]));
        test_strongly_ordered(static_cast<void const*>(&some_ints[0]), &some_ints[1]);

        std::pair<int, long> const int_long_pair{13, 42L};
        test_strongly_ordered(static_cast<void const*>(&int_long_pair.first), &int_long_pair.second);
        test_strongly_ordered(&int_long_pair.first, static_cast<void const*>(&int_long_pair.second));
    }

    // Validate types partially ordered by builtin <=> operators
    test_partially_ordered(1.414, 3.14, partial_ordering::less);
    test_partially_ordered(1.414f, 3.14, partial_ordering::less);
    test_partially_ordered(1.414, 3.14f, partial_ordering::less);
    test_partially_ordered(31.625f, 31.625, partial_ordering::equivalent);
#if !defined(__clang__) && !defined(__EDG__) // TRANSITION, VSO-1062601
    if (!std::is_constant_evaluated())
#endif // TRANSITION, VSO-1062601
    {
        test_partially_ordered(3.14, NaN, partial_ordering::unordered);
        test_partially_ordered(3.14f, NaN, partial_ordering::unordered);
        test_partially_ordered(3.14, NaNf, partial_ordering::unordered);
    }

    // Validate types with no builtin <=> operators that are nonetheless totally_ordered (within a
    // limited domain) or equality_comparable
#ifndef __clang__
#pragma warning(push)
#pragma warning(disable : 4018) // '%s': signed/unsigned mismatch
#pragma warning(disable : 4389) // '%s': signed/unsigned mismatch
#endif // !__clang__
    test_totally_ordered(13, 42u, strong_ordering::less);
    test_totally_ordered(13u, 42, strong_ordering::less);
#ifndef __clang__
#pragma warning(pop)
#endif // !__clang__

    test_totally_ordered(3.14, 42, strong_ordering::less);
    test_totally_ordered(1, 3.14f, strong_ordering::less);

    test_equality_comparable(&f1, &f2, strong_ordering::less); // This means "not equal"

    struct has_members {
        int x;
        const int y;

        int f() {
            return 13;
        }
        int g() noexcept {
            return 42;
        }
    };
    test_equality_comparable(&has_members::x, &has_members::y, strong_ordering::less); // Ditto "not equal"
    test_equality_comparable(&has_members::f, &has_members::g, strong_ordering::less); // Ditto "not equal"

    test_equality_comparable(nullptr, nullptr, strong_ordering::equal);

    // Validate class types
    struct partially_ordered_class {
        int i;

        constexpr explicit partially_ordered_class(int x) noexcept : i{x} {}

        partially_ordered_class(partially_ordered_class const&)            = delete;
        partially_ordered_class& operator=(partially_ordered_class const&) = delete;

        constexpr bool operator==(partially_ordered_class const& that) const {
            if (i == 42 || that.i == 42) {
                return false;
            } else {
                return i == that.i;
            }
        }
        constexpr partial_ordering operator<=>(partially_ordered_class const& that) const {
            if (i == 42 || that.i == 42) {
                return partial_ordering::unordered;
            } else {
                return i <=> that.i;
            }
        }
    };
    struct weakly_ordered_class {
        int i;

        constexpr explicit weakly_ordered_class(int x) noexcept : i{x} {}

        weakly_ordered_class(weakly_ordered_class const&)            = delete;
        weakly_ordered_class& operator=(weakly_ordered_class const&) = delete;

        constexpr bool operator==(weakly_ordered_class const& that) const {
            return i / 2 == that.i / 2;
        }
        constexpr weak_ordering operator<=>(weakly_ordered_class const& that) const {
            return i / 2 <=> that.i / 2;
        }
    };
    struct strongly_ordered_class {
        int i;

        constexpr explicit strongly_ordered_class(int x) noexcept : i{x} {}

        strongly_ordered_class(strongly_ordered_class const&)            = delete;
        strongly_ordered_class& operator=(strongly_ordered_class const&) = delete;

        auto operator<=>(strongly_ordered_class const&) const = default;
    };

    struct equality_comparable_class {
        int i;

        struct boolish {
            bool b;
            constexpr operator bool() const {
                return b;
            }
        };

        constexpr explicit equality_comparable_class(int x) noexcept : i{x} {}

        equality_comparable_class(equality_comparable_class const&)            = delete;
        equality_comparable_class& operator=(equality_comparable_class const&) = delete;

        constexpr boolish operator==(equality_comparable_class const& that) const {
            return {i == that.i};
        }
        constexpr boolish operator!=(equality_comparable_class const& that) const {
            return {i != that.i};
        }
    };

    struct totally_ordered_class : equality_comparable_class {
        using equality_comparable_class::equality_comparable_class;

        constexpr boolish operator<(totally_ordered_class const& that) const {
            return {i < that.i};
        }
        constexpr boolish operator>(totally_ordered_class const& that) const {
            return {i > that.i};
        }
        constexpr boolish operator<=(totally_ordered_class const& that) const {
            return {i <= that.i};
        }
        constexpr boolish operator>=(totally_ordered_class const& that) const {
            return {i >= that.i};
        }
    };

    test_partially_ordered(partially_ordered_class{13}, partially_ordered_class{42}, partial_ordering::unordered);
    test_partially_ordered(partially_ordered_class{13}, partially_ordered_class{29}, partial_ordering::less);
    test_weakly_ordered(weakly_ordered_class{13}, weakly_ordered_class{42}, weak_ordering::less);
    test_weakly_ordered(weakly_ordered_class{13}, weakly_ordered_class{12}, weak_ordering::equivalent);
    test_strongly_ordered(strongly_ordered_class{13}, strongly_ordered_class{42});
    test_equality_comparable(equality_comparable_class{13}, equality_comparable_class{13}, strong_ordering::equal);
    test_equality_comparable(equality_comparable_class{13}, equality_comparable_class{42}, strong_ordering::less);
    test_totally_ordered(totally_ordered_class{13}, totally_ordered_class{42}, strong_ordering::less);
}

int main() {
    STATIC_ASSERT((ordering_test_cases(), true));
    ordering_test_cases();
}
