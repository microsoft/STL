// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <compare>
#include <type_traits>

enum class comp { equal, nonequal, less, greater, unordered };

template <comp Z, class T>
constexpr bool test_ord(T val) {
    assert((val == 0) == (Z == comp::equal));
    assert((0 == val) == (Z == comp::equal));
    assert((val != 0) == (Z != comp::equal));
    assert((0 != val) == (Z != comp::equal));
#ifdef __cpp_impl_three_way_comparison
    assert(((val <=> 0) == 0) == (Z == comp::equal));
    assert(((0 <=> val) == 0) == (Z == comp::equal));

#if __cpp_impl_three_way_comparison >= 201907L
    assert(val == val);
    assert(!(val != val));
#endif // __cpp_impl_three_way_comparison >= 201907L
#endif // __cpp_impl_three_way_comparison
    assert(std::is_eq(val) == (Z == comp::equal));
    assert(std::is_neq(val) == (Z != comp::equal));

    assert((val < 0) == (Z == comp::less));
    assert((0 > val) == (Z == comp::less));
    assert((val > 0) == (Z == comp::greater));
    assert((0 < val) == (Z == comp::greater));
    assert((val <= 0) == (Z != comp::greater && Z != comp::unordered));
    assert((0 >= val) == (Z != comp::greater && Z != comp::unordered));
    assert((val >= 0) == (Z != comp::less && Z != comp::unordered));
    assert((0 <= val) == (Z != comp::less && Z != comp::unordered));
#ifdef __cpp_impl_three_way_comparison
    assert(((val <=> 0) < 0) == (Z == comp::less));
    assert(((0 <=> val) < 0) == (Z == comp::greater));

#if __cpp_impl_three_way_comparison >= 201907L
    assert(val == val);
    assert(!(val != val));
#endif // __cpp_impl_three_way_comparison >= 201907L
#endif // __cpp_impl_three_way_comparison
    assert(std::is_lt(val) == (Z == comp::less));
    assert(std::is_lteq(val) == (Z != comp::greater && Z != comp::unordered));
    assert(std::is_gt(val) == (Z == comp::greater));
    assert(std::is_gteq(val) == (Z != comp::less && Z != comp::unordered));

    return true;
}

static_assert(test_ord<comp::equal>(std::partial_ordering::equivalent));
static_assert(test_ord<comp::less>(std::partial_ordering::less));
static_assert(test_ord<comp::greater>(std::partial_ordering::greater));
static_assert(test_ord<comp::unordered>(std::partial_ordering::unordered));

static_assert(test_ord<comp::equal>(std::weak_ordering::equivalent));
static_assert(test_ord<comp::less>(std::weak_ordering::less));
static_assert(test_ord<comp::greater>(std::weak_ordering::greater));

static_assert(test_ord<comp::equal>(std::strong_ordering::equal));
static_assert(test_ord<comp::equal>(std::strong_ordering::equivalent));
static_assert(test_ord<comp::less>(std::strong_ordering::less));
static_assert(test_ord<comp::greater>(std::strong_ordering::greater));

template <class Expected, class... Categories>
constexpr bool test_common_cc = std::is_same_v<std::common_comparison_category_t<Categories...>, Expected>;

// The following references are to N4842 [class.spaceship].

// (4.1) If any T_i is not a comparison category type, U is void.
static_assert(test_common_cc<void, int>);
static_assert(test_common_cc<void, int, std::strong_ordering>);
static_assert(test_common_cc<void, std::strong_ordering, int>);

// (4.2) Otherwise, if at least one T_i is std::partial_ordering, U is std::partial_ordering.
static_assert(test_common_cc<std::partial_ordering, std::partial_ordering>);
static_assert(test_common_cc<std::partial_ordering, std::partial_ordering, std::strong_ordering, std::weak_ordering>);
static_assert(test_common_cc<std::partial_ordering, std::weak_ordering, std::partial_ordering, std::strong_ordering>);
static_assert(test_common_cc<std::partial_ordering, std::strong_ordering, std::weak_ordering, std::partial_ordering>);

// (4.3) Otherwise, if at least one T_i is std::weak_ordering, U is std::weak_ordering.
static_assert(test_common_cc<std::weak_ordering, std::weak_ordering>);
static_assert(test_common_cc<std::weak_ordering, std::weak_ordering, std::strong_ordering, std::strong_ordering>);
static_assert(test_common_cc<std::weak_ordering, std::strong_ordering, std::weak_ordering, std::strong_ordering>);
static_assert(test_common_cc<std::weak_ordering, std::strong_ordering, std::strong_ordering, std::weak_ordering>);

// (4.4) Otherwise, U is std::strong_ordering.
static_assert(test_common_cc<std::strong_ordering, std::strong_ordering>);
static_assert(test_common_cc<std::strong_ordering, std::strong_ordering, std::strong_ordering, std::strong_ordering>);
// [Note: In particular, this is the result when n is 0.-end note]
static_assert(test_common_cc<std::strong_ordering>);

// Per P1614R2, common_type_t<T, U> must be the same type as common_comparison_category_t<T, U> when T and U are both
// comparison category types.
template <class T, class U>
constexpr bool test_common_type() {
    using CommonType = std::common_type_t<T, U>;
    static_assert(std::is_same_v<std::common_type_t<U, T>, CommonType>, "common_type should be symmetric");

    using CommonComparisonType = std::common_comparison_category_t<T, U>;
    static_assert(std::is_same_v<std::common_comparison_category_t<U, T>, CommonComparisonType>,
        "common_comparison_category should be symmetric");

    static_assert(std::is_same_v<CommonType, CommonComparisonType>,
        "A pair of comparison category types should have the same common type and common comparison type.");

    return true;
}

static_assert(test_common_type<std::partial_ordering, std::partial_ordering>());
static_assert(test_common_type<std::partial_ordering, std::weak_ordering>());
static_assert(test_common_type<std::partial_ordering, std::strong_ordering>());
static_assert(test_common_type<std::weak_ordering, std::partial_ordering>());
static_assert(test_common_type<std::weak_ordering, std::weak_ordering>());
static_assert(test_common_type<std::weak_ordering, std::strong_ordering>());
static_assert(test_common_type<std::strong_ordering, std::partial_ordering>());
static_assert(test_common_type<std::strong_ordering, std::weak_ordering>());
static_assert(test_common_type<std::strong_ordering, std::strong_ordering>());

#if defined(__cpp_impl_three_way_comparison) && defined(__cpp_lib_concepts)
constexpr auto my_cmp_three_way = [](const auto& left, const auto& right) { return left <=> right; };

template <class Left, class Right>
constexpr auto compare(const Left& left, const Right& right) {
    using std::begin, std::end;
    auto ret = std::lexicographical_compare_three_way(begin(left), end(left), begin(right), end(right));
    auto ret2 =
        std::lexicographical_compare_three_way(begin(left), end(left), begin(right), end(right), my_cmp_three_way);
    assert(ret == ret2);
    return ret;
}

template <class Ty1, class Ty2 = Ty1>
constexpr bool test_algorithm2() {
    std::array<Ty1, 4> original{{0, 1, 2, 3}};

    std::array<Ty2, 4> same{{0, 1, 2, 3}};
    std::array<Ty2, 4> sameSizeAndLesser{{0, 1, 1, 3}};
    std::array<Ty2, 4> sameSizeAndGreater{{0, 1, 3, 3}};

    std::array<Ty2, 3> shorter{{0, 1, 2}};
    std::array<Ty2, 3> shorterAndGreater{{0, 2, 3}};

    std::array<Ty2, 5> longer{{0, 1, 2, 3, 4}};
    std::array<Ty2, 5> longerAndLesser{{0, 1, 2, 2, 4}};

    assert(compare(original, original) == 0);

    assert(compare(original, same) == 0);
    assert(compare(same, original) == 0);

    assert(compare(original, sameSizeAndLesser) > 0);
    assert(compare(sameSizeAndLesser, original) < 0);

    assert(compare(original, sameSizeAndGreater) < 0);
    assert(compare(sameSizeAndGreater, original) > 0);

    assert(compare(original, shorter) > 0);
    assert(compare(shorter, original) < 0);

    assert(compare(original, shorterAndGreater) < 0);
    assert(compare(shorterAndGreater, original) > 0);

    assert(compare(original, longer) < 0);
    assert(compare(longer, original) > 0);

    assert(compare(original, longerAndLesser) > 0);
    assert(compare(longerAndLesser, original) < 0);

    return true;
}

template <class Ty1>
void test_algorithm() {
    static_assert(test_algorithm2<Ty1>());
    assert(test_algorithm2<Ty1>());
}

template <class Ty1, class Ty2>
void test_algorithm() {
    static_assert(test_algorithm2<Ty1, Ty2>());
    static_assert(test_algorithm2<Ty2, Ty1>());
    assert((test_algorithm2<Ty1, Ty2>()));
    assert((test_algorithm2<Ty2, Ty1>()));
}
#endif // defined(__cpp_impl_three_way_comparison) && defined(__cpp_lib_concepts)

int main() {
    test_ord<comp::equal>(std::partial_ordering::equivalent);
    test_ord<comp::less>(std::partial_ordering::less);
    test_ord<comp::greater>(std::partial_ordering::greater);
    test_ord<comp::unordered>(std::partial_ordering::unordered);

    test_ord<comp::equal>(std::weak_ordering::equivalent);
    test_ord<comp::less>(std::weak_ordering::less);
    test_ord<comp::greater>(std::weak_ordering::greater);

    test_ord<comp::equal>(std::strong_ordering::equal);
    test_ord<comp::equal>(std::strong_ordering::equivalent);
    test_ord<comp::less>(std::strong_ordering::less);
    test_ord<comp::greater>(std::strong_ordering::greater);

#if defined(__cpp_impl_three_way_comparison) && defined(__cpp_lib_concepts)
    test_algorithm<int>();
    test_algorithm<char>();
    test_algorithm<unsigned char>();
    test_algorithm<int, char>();
    test_algorithm<int, unsigned char>();
    test_algorithm<char, unsigned char>();
#endif // defined(__cpp_impl_three_way_comparison) && defined(__cpp_lib_concepts)
}
