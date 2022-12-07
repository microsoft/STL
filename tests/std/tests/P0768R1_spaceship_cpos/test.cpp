// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Test the customization point objects in N4861 [cmp.alg].

#include <bit>
#include <cassert>
#include <compare>
#include <cstddef>
#include <limits>
#include <type_traits>
#include <utility>

// Intentionally not `using namespace std;` because we need to test ADL.
using std::bit_cast, std::compare_partial_order_fallback, std::compare_strong_order_fallback,
    std::compare_weak_order_fallback, std::declval, std::extent_v, std::is_same_v, std::numeric_limits, std::pair,
    std::partial_ordering, std::size_t, std::strong_ordering, std::weak_ordering;

// Define test machinery.
struct IllFormed {};

template <const auto& CPO, typename E, typename F>
struct CpoResultImpl {
    using type = IllFormed;
};

template <const auto& CPO, typename E, typename F>
    requires requires { CPO(declval<E>(), declval<F>()); }
struct CpoResultImpl<CPO, E, F> {
    using type = decltype(CPO(declval<E>(), declval<F>()));
};

template <const auto& CPO, typename E, typename F = E>
using CpoResult = typename CpoResultImpl<CPO, E, F>::type;

template <const auto& CPO, typename E, typename F = E>
inline constexpr bool NoexceptCpo = noexcept(CPO(declval<E>(), declval<F>()));

// Test when the decayed types differ.
static_assert(is_same_v<CpoResult<std::strong_order, int, long>, IllFormed>); // [cmp.alg]/1.1
static_assert(is_same_v<CpoResult<std::weak_order, int, long>, IllFormed>); // [cmp.alg]/2.1
static_assert(is_same_v<CpoResult<std::partial_order, int, long>, IllFormed>); // [cmp.alg]/3.1
static_assert(is_same_v<CpoResult<compare_strong_order_fallback, int, long>, IllFormed>); // [cmp.alg]/4.1
static_assert(is_same_v<CpoResult<compare_weak_order_fallback, int, long>, IllFormed>); // [cmp.alg]/5.1
static_assert(is_same_v<CpoResult<compare_partial_order_fallback, int, long>, IllFormed>); // [cmp.alg]/6.1

// Also test when the decayed types differ, with floating-point.
namespace {
    using D = double;
    using L = long double;
    static_assert(is_same_v<CpoResult<std::strong_order, D, L>, IllFormed>); // [cmp.alg]/1.1
    static_assert(is_same_v<CpoResult<std::weak_order, D, L>, IllFormed>); // [cmp.alg]/2.1
    static_assert(is_same_v<CpoResult<std::partial_order, D, L>, IllFormed>); // [cmp.alg]/3.1
    static_assert(is_same_v<CpoResult<compare_strong_order_fallback, D, L>, IllFormed>); // [cmp.alg]/4.1
    static_assert(is_same_v<CpoResult<compare_weak_order_fallback, D, L>, IllFormed>); // [cmp.alg]/5.1
    static_assert(is_same_v<CpoResult<compare_partial_order_fallback, D, L>, IllFormed>); // [cmp.alg]/6.1
} // namespace

// Test when the type isn't comparable at all.
struct NotComparable {};
static_assert(is_same_v<CpoResult<std::strong_order, NotComparable>, IllFormed>); // [cmp.alg]/1.5
static_assert(is_same_v<CpoResult<std::weak_order, NotComparable>, IllFormed>); // [cmp.alg]/2.6
static_assert(is_same_v<CpoResult<std::partial_order, NotComparable>, IllFormed>); // [cmp.alg]/3.5
static_assert(is_same_v<CpoResult<compare_strong_order_fallback, NotComparable>, IllFormed>); // [cmp.alg]/4.4
static_assert(is_same_v<CpoResult<compare_weak_order_fallback, NotComparable>, IllFormed>); // [cmp.alg]/5.4
static_assert(is_same_v<CpoResult<compare_partial_order_fallback, NotComparable>, IllFormed>); // [cmp.alg]/6.4

// Define test machinery for ADL.
inline constexpr bool Throwing    = false;
inline constexpr bool NonThrowing = true;

namespace TestAdl {
    template <bool NE = NonThrowing>
    struct StrongType {
        int val;
    };

    template <bool NE>
    [[nodiscard]] constexpr strong_ordering strong_order(
        const StrongType<NE>& left, const StrongType<NE>& right) noexcept(NE) {
        return left.val <=> right.val;
    }

    template <bool NE = NonThrowing>
    struct WeakType {
        int val;
    };

    template <bool NE>
    [[nodiscard]] constexpr weak_ordering weak_order(const WeakType<NE>& left, const WeakType<NE>& right) noexcept(NE) {
        return left.val <=> right.val;
    }

    template <bool NE = NonThrowing>
    struct PartialType {
        int val;
    };

    template <bool NE>
    [[nodiscard]] constexpr partial_ordering partial_order(
        const PartialType<NE>& left, const PartialType<NE>& right) noexcept(NE) {
        return left.val <=> right.val;
    }
} // namespace TestAdl

// Define test machinery for ADL, when the return type is a weird user-defined type
// with a conversion operator to a Standard comparison category type.
namespace TestUdt {
    template <typename Ordering, bool NE>
    struct WeirdOrdering {
        Ordering ordering;

        constexpr explicit operator Ordering() const noexcept(NE) {
            return ordering;
        }

        template <typename AnythingElse>
        constexpr explicit operator AnythingElse() const = delete;
    };

    template <bool NE = NonThrowing>
    struct StrongWeird {
        int val;
    };

    template <bool NE>
    [[nodiscard]] constexpr WeirdOrdering<strong_ordering, NE> strong_order(
        const StrongWeird<NE>& left, const StrongWeird<NE>& right) noexcept {
        return {left.val <=> right.val};
    }

    template <bool NE = NonThrowing>
    struct WeakWeird {
        int val;
    };

    template <bool NE>
    [[nodiscard]] constexpr WeirdOrdering<weak_ordering, NE> weak_order(
        const WeakWeird<NE>& left, const WeakWeird<NE>& right) noexcept {
        return {left.val <=> right.val};
    }

    template <bool NE = NonThrowing>
    struct PartialWeird {
        int val;
    };

    template <bool NE>
    [[nodiscard]] constexpr WeirdOrdering<partial_ordering, NE> partial_order(
        const PartialWeird<NE>& left, const PartialWeird<NE>& right) noexcept {
        return {left.val <=> right.val};
    }
} // namespace TestUdt

// Define test machinery for compare_three_way.
template <bool NE = NonThrowing>
struct SpaceshipType {
    int val;

    [[nodiscard]] constexpr auto operator<=>(const SpaceshipType&) const noexcept(NE) = default;
};

// Define test machinery for fallbacks.
template <int I = 0>
struct Fallback {
    static constexpr int UnorderedVal = -1000;

    int val{UnorderedVal};

    // I == 0: Normal.

    // I == 1: E == F can throw.
    // I == 2: E == F is ill-formed.
    // I == 3: E == F returns int.
    // I == 4: E == F returns void.
    // clang-format off
    constexpr auto operator==(const Fallback& other) const noexcept(I != 1) requires (I != 2) {
        // clang-format on
        const bool result{val == other.val && val != UnorderedVal};

        if constexpr (I == 3) {
            return static_cast<int>(result);
        } else if constexpr (I == 4) {
            return;
        } else {
            return result;
        }
    }

    // I == 5: E < F can throw.
    // I == 6: E < F is ill-formed.
    // I == 7: E < F returns int.
    // I == 8: E < F returns void.
    // clang-format off
    constexpr auto operator<(const Fallback& other) const noexcept(I != 5) requires (I != 6) {
        // clang-format on
        const bool result{val < other.val && val != UnorderedVal && other.val != UnorderedVal};

        if constexpr (I == 7) {
            return static_cast<int>(result);
        } else if constexpr (I == 8) {
            return;
        } else {
            return result;
        }
    }

    // I == 9: F < E is ill-formed.
    // clang-format off
    constexpr bool operator<(Fallback&) const noexcept requires (I == 9) = delete;
    // clang-format on

    // I == 10: F < E can throw.
    // I == 11: F < E returns int.
    // I == 12: F < E returns void.
    // I == 13: F < E returns bool.
    // clang-format off
    constexpr auto operator<(Fallback& other) const noexcept(I != 10) requires (I >= 10 && I <= 13) {
        // clang-format on
        const bool result{val < other.val && val != UnorderedVal && other.val != UnorderedVal};

        if constexpr (I == 11) {
            return static_cast<int>(result);
        } else if constexpr (I == 12) {
            return;
        } else {
            return result;
        }
    }
};

// Test when the type is comparable through ADL. Part A, return types.
static_assert(is_same_v<CpoResult<std::strong_order, TestAdl::StrongType<>>, strong_ordering>); // [cmp.alg]/1.2
static_assert(is_same_v<CpoResult<std::weak_order, TestAdl::WeakType<>>, weak_ordering>); // [cmp.alg]/2.2
static_assert(is_same_v<CpoResult<std::partial_order, TestAdl::PartialType<>>, partial_ordering>); // [cmp.alg]/3.2

// Test floating-point types. Part A, return types. Also test decay.
static_assert(is_same_v<CpoResult<std::strong_order, float>, strong_ordering>); // [cmp.alg]/1.3
static_assert(is_same_v<CpoResult<std::strong_order, double>, strong_ordering>); // [cmp.alg]/1.3
static_assert(is_same_v<CpoResult<std::strong_order, long double>, strong_ordering>); // [cmp.alg]/1.3

static_assert(is_same_v<CpoResult<std::strong_order, double, const double&>, strong_ordering>); // [cmp.alg]/1.3
static_assert(is_same_v<CpoResult<std::strong_order, const double&, double>, strong_ordering>); // [cmp.alg]/1.3
static_assert(is_same_v<CpoResult<std::strong_order, const double&>, strong_ordering>); // [cmp.alg]/1.3

static_assert(is_same_v<CpoResult<std::weak_order, float>, weak_ordering>); // [cmp.alg]/2.3
static_assert(is_same_v<CpoResult<std::weak_order, double>, weak_ordering>); // [cmp.alg]/2.3
static_assert(is_same_v<CpoResult<std::weak_order, long double>, weak_ordering>); // [cmp.alg]/2.3

static_assert(is_same_v<CpoResult<std::weak_order, double, const double&>, weak_ordering>); // [cmp.alg]/2.3
static_assert(is_same_v<CpoResult<std::weak_order, const double&, double>, weak_ordering>); // [cmp.alg]/2.3
static_assert(is_same_v<CpoResult<std::weak_order, const double&>, weak_ordering>); // [cmp.alg]/2.3

// Test when the type is comparable through compare_three_way. Part A, return types.
static_assert(is_same_v<CpoResult<std::strong_order, int>, strong_ordering>); // [cmp.alg]/1.4
static_assert(is_same_v<CpoResult<std::weak_order, int>, weak_ordering>); // [cmp.alg]/2.4
static_assert(is_same_v<CpoResult<std::partial_order, int>, partial_ordering>); // [cmp.alg]/3.3

static_assert(is_same_v<CpoResult<std::strong_order, SpaceshipType<>>, strong_ordering>); // [cmp.alg]/1.4
static_assert(is_same_v<CpoResult<std::weak_order, SpaceshipType<>>, weak_ordering>); // [cmp.alg]/2.4
static_assert(is_same_v<CpoResult<std::partial_order, SpaceshipType<>>, partial_ordering>); // [cmp.alg]/3.3

// Test when the type is comparable through stronger ADL. Part A, return types.
static_assert(is_same_v<CpoResult<std::weak_order, TestAdl::StrongType<>>, weak_ordering>); // [cmp.alg]/2.5
static_assert(is_same_v<CpoResult<std::partial_order, TestAdl::StrongType<>>, partial_ordering>); // [cmp.alg]/3.4
static_assert(is_same_v<CpoResult<std::partial_order, TestAdl::WeakType<>>, partial_ordering>); // [cmp.alg]/3.4

// Test weird ADL cases. Part A, return types.
static_assert(is_same_v<CpoResult<std::strong_order, TestUdt::StrongWeird<>>, strong_ordering>); // [cmp.alg]/1.2
static_assert(is_same_v<CpoResult<std::weak_order, TestUdt::WeakWeird<>>, weak_ordering>); // [cmp.alg]/2.2
static_assert(is_same_v<CpoResult<std::partial_order, TestUdt::PartialWeird<>>, partial_ordering>); // [cmp.alg]/3.2

static_assert(is_same_v<CpoResult<std::weak_order, TestUdt::StrongWeird<>>, weak_ordering>); // [cmp.alg]/2.5
static_assert(is_same_v<CpoResult<std::partial_order, TestUdt::StrongWeird<>>, partial_ordering>); // [cmp.alg]/3.4
static_assert(is_same_v<CpoResult<std::partial_order, TestUdt::WeakWeird<>>, partial_ordering>); // [cmp.alg]/3.4

// Test non-fallback cases, just enough to verify CPO usage. Part A, return types. See [cmp.alg]/4.2, 5.2, and 6.2.
static_assert(is_same_v<CpoResult<compare_strong_order_fallback, TestAdl::StrongType<>>, strong_ordering>);
static_assert(is_same_v<CpoResult<compare_weak_order_fallback, TestAdl::WeakType<>>, weak_ordering>);
static_assert(is_same_v<CpoResult<compare_partial_order_fallback, TestAdl::PartialType<>>, partial_ordering>);

static_assert(is_same_v<CpoResult<compare_strong_order_fallback, SpaceshipType<>>, strong_ordering>);
static_assert(is_same_v<CpoResult<compare_weak_order_fallback, SpaceshipType<>>, weak_ordering>);
static_assert(is_same_v<CpoResult<compare_partial_order_fallback, SpaceshipType<>>, partial_ordering>);

// Test fallbacks. Part A, return types.
static_assert(is_same_v<CpoResult<compare_strong_order_fallback, Fallback<0>>, strong_ordering>); // [cmp.alg]/4.3
static_assert(is_same_v<CpoResult<compare_strong_order_fallback, Fallback<1>>, strong_ordering>); // [cmp.alg]/4.3
static_assert(is_same_v<CpoResult<compare_strong_order_fallback, Fallback<2>>, IllFormed>); // [cmp.alg]/4.3
static_assert(is_same_v<CpoResult<compare_strong_order_fallback, Fallback<3>>, strong_ordering>); // [cmp.alg]/4.3
static_assert(is_same_v<CpoResult<compare_strong_order_fallback, Fallback<4>>, IllFormed>); // [cmp.alg]/4.3
static_assert(is_same_v<CpoResult<compare_strong_order_fallback, Fallback<5>>, strong_ordering>); // [cmp.alg]/4.3
static_assert(is_same_v<CpoResult<compare_strong_order_fallback, Fallback<6>>, IllFormed>); // [cmp.alg]/4.3
static_assert(is_same_v<CpoResult<compare_strong_order_fallback, Fallback<7>>, strong_ordering>); // [cmp.alg]/4.3
static_assert(is_same_v<CpoResult<compare_strong_order_fallback, Fallback<8>>, IllFormed>); // [cmp.alg]/4.3

static_assert(is_same_v<CpoResult<compare_weak_order_fallback, Fallback<0>>, weak_ordering>); // [cmp.alg]/5.3
static_assert(is_same_v<CpoResult<compare_weak_order_fallback, Fallback<1>>, weak_ordering>); // [cmp.alg]/5.3
static_assert(is_same_v<CpoResult<compare_weak_order_fallback, Fallback<2>>, IllFormed>); // [cmp.alg]/5.3
static_assert(is_same_v<CpoResult<compare_weak_order_fallback, Fallback<3>>, weak_ordering>); // [cmp.alg]/5.3
static_assert(is_same_v<CpoResult<compare_weak_order_fallback, Fallback<4>>, IllFormed>); // [cmp.alg]/5.3
static_assert(is_same_v<CpoResult<compare_weak_order_fallback, Fallback<5>>, weak_ordering>); // [cmp.alg]/5.3
static_assert(is_same_v<CpoResult<compare_weak_order_fallback, Fallback<6>>, IllFormed>); // [cmp.alg]/5.3
static_assert(is_same_v<CpoResult<compare_weak_order_fallback, Fallback<7>>, weak_ordering>); // [cmp.alg]/5.3
static_assert(is_same_v<CpoResult<compare_weak_order_fallback, Fallback<8>>, IllFormed>); // [cmp.alg]/5.3

static_assert(is_same_v<CpoResult<compare_partial_order_fallback, Fallback<0>>, partial_ordering>); // [cmp.alg]/6.3
static_assert(is_same_v<CpoResult<compare_partial_order_fallback, Fallback<1>>, partial_ordering>); // [cmp.alg]/6.3
static_assert(is_same_v<CpoResult<compare_partial_order_fallback, Fallback<2>>, IllFormed>); // [cmp.alg]/6.3
static_assert(is_same_v<CpoResult<compare_partial_order_fallback, Fallback<3>>, partial_ordering>); // [cmp.alg]/6.3
static_assert(is_same_v<CpoResult<compare_partial_order_fallback, Fallback<4>>, IllFormed>); // [cmp.alg]/6.3
static_assert(is_same_v<CpoResult<compare_partial_order_fallback, Fallback<5>>, partial_ordering>); // [cmp.alg]/6.3
static_assert(is_same_v<CpoResult<compare_partial_order_fallback, Fallback<6>>, IllFormed>); // [cmp.alg]/6.3
static_assert(is_same_v<CpoResult<compare_partial_order_fallback, Fallback<7>>, partial_ordering>); // [cmp.alg]/6.3
static_assert(is_same_v<CpoResult<compare_partial_order_fallback, Fallback<8>>, IllFormed>); // [cmp.alg]/6.3

namespace {
    using Partial = partial_ordering;
    template <int I>
    using F = Fallback<I>;

    static_assert(is_same_v<CpoResult<compare_partial_order_fallback, F<9>, const F<9>>, IllFormed>); // [cmp.alg]/6.3
    static_assert(is_same_v<CpoResult<compare_partial_order_fallback, F<10>, const F<10>>, Partial>); // [cmp.alg]/6.3
    static_assert(is_same_v<CpoResult<compare_partial_order_fallback, F<11>, const F<11>>, Partial>); // [cmp.alg]/6.3
    static_assert(is_same_v<CpoResult<compare_partial_order_fallback, F<12>, const F<12>>, IllFormed>); // [cmp.alg]/6.3
    static_assert(is_same_v<CpoResult<compare_partial_order_fallback, F<13>, const F<13>>, Partial>); // [cmp.alg]/6.3
} // namespace

// Test strengthened requirements in P2167R3: compare_*_order_fallback CPOs require return types to be boolean-testable.
enum class ResultKind : bool {
    Bad,
    Good,
};

template <ResultKind K>
struct ComparisonResult {
    bool value;

    constexpr operator bool() const noexcept {
        return value;
    }

    constexpr auto operator!() const noexcept {
        if constexpr (K == ResultKind::Good) {
            return ComparisonResult{!value};
        }
    }
};

template <ResultKind EqKind, ResultKind LeKind>
struct BoolTestType {
    friend constexpr ComparisonResult<EqKind> operator==(BoolTestType, BoolTestType) noexcept {
        return ComparisonResult<EqKind>{true};
    }

    friend constexpr ComparisonResult<LeKind> operator<(BoolTestType, BoolTestType) noexcept {
        return ComparisonResult<LeKind>{false};
    }
};

static_assert(is_same_v<CpoResult<compare_strong_order_fallback, BoolTestType<ResultKind::Bad, ResultKind::Bad>>,
    IllFormed>); // [cmp.alg]/4.3
static_assert(is_same_v<CpoResult<compare_strong_order_fallback, BoolTestType<ResultKind::Bad, ResultKind::Good>>,
    IllFormed>); // [cmp.alg]/4.3
static_assert(is_same_v<CpoResult<compare_strong_order_fallback, BoolTestType<ResultKind::Good, ResultKind::Bad>>,
    IllFormed>); // [cmp.alg]/4.3
static_assert(is_same_v<CpoResult<compare_strong_order_fallback, BoolTestType<ResultKind::Good, ResultKind::Good>>,
    strong_ordering>); // [cmp.alg]/4.3

static_assert(is_same_v<CpoResult<compare_weak_order_fallback, BoolTestType<ResultKind::Bad, ResultKind::Bad>>,
    IllFormed>); // [cmp.alg]/5.3
static_assert(is_same_v<CpoResult<compare_weak_order_fallback, BoolTestType<ResultKind::Bad, ResultKind::Good>>,
    IllFormed>); // [cmp.alg]/5.3
static_assert(is_same_v<CpoResult<compare_weak_order_fallback, BoolTestType<ResultKind::Good, ResultKind::Bad>>,
    IllFormed>); // [cmp.alg]/5.3
static_assert(is_same_v<CpoResult<compare_weak_order_fallback, BoolTestType<ResultKind::Good, ResultKind::Good>>,
    weak_ordering>); // [cmp.alg]/5.3

static_assert(is_same_v<CpoResult<compare_partial_order_fallback, BoolTestType<ResultKind::Bad, ResultKind::Bad>>,
    IllFormed>); // [cmp.alg]/6.3
static_assert(is_same_v<CpoResult<compare_partial_order_fallback, BoolTestType<ResultKind::Bad, ResultKind::Good>>,
    IllFormed>); // [cmp.alg]/6.3
static_assert(is_same_v<CpoResult<compare_partial_order_fallback, BoolTestType<ResultKind::Good, ResultKind::Bad>>,
    IllFormed>); // [cmp.alg]/6.3
static_assert(is_same_v<CpoResult<compare_partial_order_fallback, BoolTestType<ResultKind::Good, ResultKind::Good>>,
    partial_ordering>); // [cmp.alg]/6.3

// Test when the type is comparable through ADL. Part B, exception specifications.
static_assert(!NoexceptCpo<std::strong_order, TestAdl::StrongType<Throwing>>); // [cmp.alg]/1.2
static_assert(!NoexceptCpo<std::weak_order, TestAdl::WeakType<Throwing>>); // [cmp.alg]/2.2
static_assert(!NoexceptCpo<std::partial_order, TestAdl::PartialType<Throwing>>); // [cmp.alg]/3.2

static_assert(NoexceptCpo<std::strong_order, TestAdl::StrongType<NonThrowing>>); // [cmp.alg]/1.2
static_assert(NoexceptCpo<std::weak_order, TestAdl::WeakType<NonThrowing>>); // [cmp.alg]/2.2
static_assert(NoexceptCpo<std::partial_order, TestAdl::PartialType<NonThrowing>>); // [cmp.alg]/3.2

// Test floating-point types. Part B, exception specifications.
static_assert(NoexceptCpo<std::strong_order, float>); // [cmp.alg]/1.3
static_assert(NoexceptCpo<std::strong_order, double>); // [cmp.alg]/1.3
static_assert(NoexceptCpo<std::strong_order, long double>); // [cmp.alg]/1.3

static_assert(NoexceptCpo<std::weak_order, float>); // [cmp.alg]/2.3
static_assert(NoexceptCpo<std::weak_order, double>); // [cmp.alg]/2.3
static_assert(NoexceptCpo<std::weak_order, long double>); // [cmp.alg]/2.3

// Test when the type is comparable through compare_three_way. Part B, exception specifications.
static_assert(NoexceptCpo<std::strong_order, int>); // [cmp.alg]/1.4
static_assert(NoexceptCpo<std::weak_order, int>); // [cmp.alg]/2.4
static_assert(NoexceptCpo<std::partial_order, int>); // [cmp.alg]/3.3

static_assert(!NoexceptCpo<std::strong_order, SpaceshipType<Throwing>>); // [cmp.alg]/1.4
static_assert(!NoexceptCpo<std::weak_order, SpaceshipType<Throwing>>); // [cmp.alg]/2.4
static_assert(!NoexceptCpo<std::partial_order, SpaceshipType<Throwing>>); // [cmp.alg]/3.3

static_assert(NoexceptCpo<std::strong_order, SpaceshipType<NonThrowing>>); // [cmp.alg]/1.4
static_assert(NoexceptCpo<std::weak_order, SpaceshipType<NonThrowing>>); // [cmp.alg]/2.4
static_assert(NoexceptCpo<std::partial_order, SpaceshipType<NonThrowing>>); // [cmp.alg]/3.3

// Test when the type is comparable through stronger ADL. Part B, exception specifications.
static_assert(!NoexceptCpo<std::weak_order, TestAdl::StrongType<Throwing>>); // [cmp.alg]/2.5
static_assert(!NoexceptCpo<std::partial_order, TestAdl::StrongType<Throwing>>); // [cmp.alg]/3.4
static_assert(!NoexceptCpo<std::partial_order, TestAdl::WeakType<Throwing>>); // [cmp.alg]/3.4

static_assert(NoexceptCpo<std::weak_order, TestAdl::StrongType<NonThrowing>>); // [cmp.alg]/2.5
static_assert(NoexceptCpo<std::partial_order, TestAdl::StrongType<NonThrowing>>); // [cmp.alg]/3.4
static_assert(NoexceptCpo<std::partial_order, TestAdl::WeakType<NonThrowing>>); // [cmp.alg]/3.4

// Test weird ADL cases. Part B, exception specifications.
static_assert(!NoexceptCpo<std::strong_order, TestUdt::StrongWeird<Throwing>>); // [cmp.alg]/1.2
static_assert(!NoexceptCpo<std::weak_order, TestUdt::WeakWeird<Throwing>>); // [cmp.alg]/2.2
static_assert(!NoexceptCpo<std::partial_order, TestUdt::PartialWeird<Throwing>>); // [cmp.alg]/3.2

static_assert(NoexceptCpo<std::strong_order, TestUdt::StrongWeird<NonThrowing>>); // [cmp.alg]/1.2
static_assert(NoexceptCpo<std::weak_order, TestUdt::WeakWeird<NonThrowing>>); // [cmp.alg]/2.2
static_assert(NoexceptCpo<std::partial_order, TestUdt::PartialWeird<NonThrowing>>); // [cmp.alg]/3.2

static_assert(!NoexceptCpo<std::weak_order, TestUdt::StrongWeird<Throwing>>); // [cmp.alg]/2.5
static_assert(!NoexceptCpo<std::partial_order, TestUdt::StrongWeird<Throwing>>); // [cmp.alg]/3.4
static_assert(!NoexceptCpo<std::partial_order, TestUdt::WeakWeird<Throwing>>); // [cmp.alg]/3.4

static_assert(NoexceptCpo<std::weak_order, TestUdt::StrongWeird<NonThrowing>>); // [cmp.alg]/2.5
static_assert(NoexceptCpo<std::partial_order, TestUdt::StrongWeird<NonThrowing>>); // [cmp.alg]/3.4
static_assert(NoexceptCpo<std::partial_order, TestUdt::WeakWeird<NonThrowing>>); // [cmp.alg]/3.4

// Test non-fallback cases. Part B, exception specifications.
static_assert(!NoexceptCpo<compare_strong_order_fallback, SpaceshipType<Throwing>>); // [cmp.alg]/4.2
static_assert(!NoexceptCpo<compare_weak_order_fallback, SpaceshipType<Throwing>>); // [cmp.alg]/5.2
static_assert(!NoexceptCpo<compare_partial_order_fallback, SpaceshipType<Throwing>>); // [cmp.alg]/6.2

static_assert(NoexceptCpo<compare_strong_order_fallback, SpaceshipType<NonThrowing>>); // [cmp.alg]/4.2
static_assert(NoexceptCpo<compare_weak_order_fallback, SpaceshipType<NonThrowing>>); // [cmp.alg]/5.2
static_assert(NoexceptCpo<compare_partial_order_fallback, SpaceshipType<NonThrowing>>); // [cmp.alg]/6.2

// Test fallbacks. Part B, exception specifications.
static_assert(NoexceptCpo<compare_strong_order_fallback, Fallback<0>>); // [cmp.alg]/4.3
static_assert(!NoexceptCpo<compare_strong_order_fallback, Fallback<1>>); // [cmp.alg]/4.3
static_assert(!NoexceptCpo<compare_strong_order_fallback, Fallback<5>>); // [cmp.alg]/4.3

static_assert(NoexceptCpo<compare_weak_order_fallback, Fallback<0>>); // [cmp.alg]/5.3
static_assert(!NoexceptCpo<compare_weak_order_fallback, Fallback<1>>); // [cmp.alg]/5.3
static_assert(!NoexceptCpo<compare_weak_order_fallback, Fallback<5>>); // [cmp.alg]/5.3

static_assert(NoexceptCpo<compare_partial_order_fallback, Fallback<0>>); // [cmp.alg]/6.3
static_assert(!NoexceptCpo<compare_partial_order_fallback, Fallback<1>>); // [cmp.alg]/6.3
static_assert(!NoexceptCpo<compare_partial_order_fallback, Fallback<5>>); // [cmp.alg]/6.3

static_assert(!NoexceptCpo<compare_partial_order_fallback, Fallback<10>, const Fallback<10>>); // [cmp.alg]/6.3

// Define test machinery for compile-time and run-time behavior.
template <const auto& CPO, typename T>
constexpr void test_behavior() {
    const T one{1};
    const T two{2};

    using Ordering = CpoResult<CPO, const T&>;

    assert(CPO(one, two) == Ordering::less);
    assert(CPO(two, two) == Ordering::equivalent);
    assert(CPO(two, one) == Ordering::greater);
}

// This takes an array of pair<int, Floating>, where the ints are arbitrary "ranks" for the std::weak_order equivalence
// classes. It tests std::strong_order, std::weak_order, and std::partial_order with every permutation of values.
template <typename PairArray>
constexpr void test_ranked_values(const PairArray& rank_value_pairs) {
    constexpr size_t N = extent_v<PairArray>;

    for (size_t l_idx = 0; l_idx < N; ++l_idx) {
        const auto& [l_rank, left] = rank_value_pairs[l_idx];
        for (size_t r_idx = 0; r_idx < N; ++r_idx) {
            const auto& [r_rank, right] = rank_value_pairs[r_idx];

            assert(std::strong_order(left, right) == l_idx <=> r_idx); // [cmp.alg]/1.3
            assert(std::weak_order(left, right) == l_rank <=> r_rank); // [cmp.alg]/2.3
            assert(std::partial_order(left, right) == left <=> right); // [cmp.alg]/3.3 (uses compare_three_way)
        }
    }
}

template <typename Floating>
constexpr void test_floating() {
    if constexpr (is_same_v<Floating, float>) {
        const pair<int, float> rank_value_pairs[]{
#ifndef _M_CEE // TRANSITION, VSO-1666161
            {10, bit_cast<float>(0xFFFFFFFFu)}, // negative quiet NaN, all payload bits set
            {10, bit_cast<float>(0xFFC01234u)}, // negative quiet NaN, some payload bits set
            {10, bit_cast<float>(0xFFC00000u)}, // negative quiet NaN, no payload bits set
#endif // _M_CEE
#ifdef __clang__ // TRANSITION, MSVC "quiets" signaling NaNs into quiet NaNs when constant evaluated
            {10, bit_cast<float>(0xFFBFFFFFu)}, // negative signaling NaN, all payload bits set
            {10, bit_cast<float>(0xFF801234u)}, // negative signaling NaN, some payload bits set
            {10, bit_cast<float>(0xFF800001u)}, // negative signaling NaN, minimum payload bits set
#endif // defined(__clang__)
            {20, -numeric_limits<float>::infinity()}, // negative infinity
            {30, -0x1.fffffep+127f}, // negative max normal
            {31, -0x1.000000p-126f}, // negative min normal
            {40, -0x0.fffffep-126f}, // negative max subnormal
            {41, -0x0.000002p-126f}, // negative min subnormal
            {50, -0.0f}, // negative zero
            {50, 0.0f}, // zero
            {60, 0x0.000002p-126f}, // min subnormal
            {61, 0x0.fffffep-126f}, // max subnormal
            {70, 0x1.000000p-126f}, // min normal
            {71, 0x1.fffffep+127f}, // max normal
            {80, numeric_limits<float>::infinity()}, // infinity
#ifdef __clang__ // TRANSITION, MSVC "quiets" signaling NaNs into quiet NaNs when constant evaluated
            {90, bit_cast<float>(0x7F800001u)}, // signaling NaN, minimum payload bits set
            {90, bit_cast<float>(0x7F801234u)}, // signaling NaN, some payload bits set
            {90, bit_cast<float>(0x7FBFFFFFu)}, // signaling NaN, all payload bits set
#endif // defined(__clang__)
#ifndef _M_CEE // TRANSITION, VSO-1666161
            {90, bit_cast<float>(0x7FC00000u)}, // quiet NaN, no payload bits set
            {90, bit_cast<float>(0x7FC01234u)}, // quiet NaN, some payload bits set
            {90, bit_cast<float>(0x7FFFFFFFu)}, // quiet NaN, all payload bits set
#endif // _M_CEE
        };

        test_ranked_values(rank_value_pairs);
    } else {
        const pair<int, Floating> rank_value_pairs[]{
            {10, bit_cast<Floating>(0xFFFFFFFFFFFFFFFFull)}, // negative quiet NaN, all payload bits set
            {10, bit_cast<Floating>(0xFFF8000000001234ull)}, // negative quiet NaN, some payload bits set
            {10, bit_cast<Floating>(0xFFF8000000000000ull)}, // negative quiet NaN, no payload bits set
#ifdef __clang__ // TRANSITION, MSVC "quiets" signaling NaNs into quiet NaNs when constant evaluated
            {10, bit_cast<Floating>(0xFFF7FFFFFFFFFFFFull)}, // negative signaling NaN, all payload bits set
            {10, bit_cast<Floating>(0xFFF0000000001234ull)}, // negative signaling NaN, some payload bits set
            {10, bit_cast<Floating>(0xFFF0000000000001ull)}, // negative signaling NaN, minimum payload bits set
#endif // defined(__clang__)
            {20, -numeric_limits<Floating>::infinity()}, // negative infinity
            {30, -0x1.fffffffffffffp+1023}, // negative max normal
            {31, -0x1.0000000000000p-1022}, // negative min normal
            {40, -0x0.fffffffffffffp-1022}, // negative max subnormal
            {41, -0x0.0000000000001p-1022}, // negative min subnormal
            {50, -0.0}, // negative zero
            {50, 0.0}, // zero
            {60, 0x0.0000000000001p-1022}, // min subnormal
            {61, 0x0.fffffffffffffp-1022}, // max subnormal
            {70, 0x1.0000000000000p-1022}, // min normal
            {71, 0x1.fffffffffffffp+1023}, // max normal
            {80, numeric_limits<Floating>::infinity()}, // infinity
#ifdef __clang__ // TRANSITION, MSVC "quiets" signaling NaNs into quiet NaNs when constant evaluated
            {90, bit_cast<Floating>(0x7FF0000000000001ull)}, // signaling NaN, minimum payload bits set
            {90, bit_cast<Floating>(0x7FF0000000001234ull)}, // signaling NaN, some payload bits set
            {90, bit_cast<Floating>(0x7FF7FFFFFFFFFFFFull)}, // signaling NaN, all payload bits set
#endif // defined(__clang__)
            {90, bit_cast<Floating>(0x7FF8000000000000ull)}, // quiet NaN, no payload bits set
            {90, bit_cast<Floating>(0x7FF8000000001234ull)}, // quiet NaN, some payload bits set
            {90, bit_cast<Floating>(0x7FFFFFFFFFFFFFFFull)}, // quiet NaN, all payload bits set
        };

        test_ranked_values(rank_value_pairs);
    }
}

[[nodiscard]] constexpr bool test() {
    // Test when the type is comparable through ADL. Part C, compile-time and run-time behavior.
    test_behavior<std::strong_order, TestAdl::StrongType<>>(); // [cmp.alg]/1.2
    test_behavior<std::weak_order, TestAdl::WeakType<>>(); // [cmp.alg]/2.2
    test_behavior<std::partial_order, TestAdl::PartialType<>>(); // [cmp.alg]/3.2

    // Test floating-point types. Part C, compile-time and run-time behavior.
    test_behavior<std::strong_order, float>(); // [cmp.alg]/1.3
    test_behavior<std::strong_order, double>(); // [cmp.alg]/1.3
    test_behavior<std::strong_order, long double>(); // [cmp.alg]/1.3

    test_behavior<std::weak_order, float>(); // [cmp.alg]/2.3
    test_behavior<std::weak_order, double>(); // [cmp.alg]/2.3
    test_behavior<std::weak_order, long double>(); // [cmp.alg]/2.3

    test_behavior<std::partial_order, float>(); // [cmp.alg]/3.3 (uses compare_three_way)
    test_behavior<std::partial_order, double>(); // [cmp.alg]/3.3 (uses compare_three_way)
    test_behavior<std::partial_order, long double>(); // [cmp.alg]/3.3 (uses compare_three_way)

    test_floating<float>();
    test_floating<double>();
    test_floating<long double>();

    // Test when the type is comparable through compare_three_way. Part C, compile-time and run-time behavior.
    test_behavior<std::strong_order, int>(); // [cmp.alg]/1.4
    test_behavior<std::weak_order, int>(); // [cmp.alg]/2.4
    test_behavior<std::partial_order, int>(); // [cmp.alg]/3.3

    test_behavior<std::strong_order, SpaceshipType<>>(); // [cmp.alg]/1.4
    test_behavior<std::weak_order, SpaceshipType<>>(); // [cmp.alg]/2.4
    test_behavior<std::partial_order, SpaceshipType<>>(); // [cmp.alg]/3.3

    // Test when the type is comparable through stronger ADL. Part C, compile-time and run-time behavior.
    test_behavior<std::weak_order, TestAdl::StrongType<>>(); // [cmp.alg]/2.5
    test_behavior<std::partial_order, TestAdl::StrongType<>>(); // [cmp.alg]/3.4
    test_behavior<std::partial_order, TestAdl::WeakType<>>(); // [cmp.alg]/3.4

    // Test weird ADL cases. Part C, compile-time and run-time behavior.
    test_behavior<std::strong_order, TestUdt::StrongWeird<>>(); // [cmp.alg]/1.2
    test_behavior<std::weak_order, TestUdt::WeakWeird<>>(); // [cmp.alg]/2.2
    test_behavior<std::partial_order, TestUdt::PartialWeird<>>(); // [cmp.alg]/3.2

    test_behavior<std::weak_order, TestUdt::StrongWeird<>>(); // [cmp.alg]/2.5
    test_behavior<std::partial_order, TestUdt::StrongWeird<>>(); // [cmp.alg]/3.4
    test_behavior<std::partial_order, TestUdt::WeakWeird<>>(); // [cmp.alg]/3.4

    // Test non-fallback cases. Part C, compile-time and run-time behavior.
    test_behavior<compare_strong_order_fallback, SpaceshipType<>>(); // [cmp.alg]/4.2
    test_behavior<compare_weak_order_fallback, SpaceshipType<>>(); // [cmp.alg]/5.2
    test_behavior<compare_partial_order_fallback, SpaceshipType<>>(); // [cmp.alg]/6.2

    // Test fallbacks. Part C, compile-time and run-time behavior.
    test_behavior<compare_strong_order_fallback, Fallback<>>(); // [cmp.alg]/4.3
    test_behavior<compare_weak_order_fallback, Fallback<>>(); // [cmp.alg]/5.3
    test_behavior<compare_partial_order_fallback, Fallback<>>(); // [cmp.alg]/6.3

    assert(compare_partial_order_fallback(Fallback{}, Fallback{}) == partial_ordering::unordered); // [cmp.alg]/6.3

    return true;
}

int main() {
    assert(test());
    static_assert(test());
}
