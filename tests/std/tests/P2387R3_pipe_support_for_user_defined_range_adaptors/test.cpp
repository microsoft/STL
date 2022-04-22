// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <concepts>
#include <ranges>

using namespace std;

template <class T>
concept CanInstantiateRangeAdaptorClosure = requires {
    typename std::ranges::range_adaptor_closure<T>;
};

class EmptyTestType {};
class IncompleteTestType;

static_assert(CanInstantiateRangeAdaptorClosure<EmptyTestType>);
static_assert(!CanInstantiateRangeAdaptorClosure<const EmptyTestType>);
static_assert(!CanInstantiateRangeAdaptorClosure<volatile EmptyTestType>);
static_assert(!CanInstantiateRangeAdaptorClosure<EmptyTestType&>);
static_assert(!CanInstantiateRangeAdaptorClosure<int>);
static_assert(CanInstantiateRangeAdaptorClosure<IncompleteTestType>);


template <class LHS, class RHS>
concept CanPipe = requires(LHS lhs, RHS rhs) {
    std::forward<LHS>(lhs) | std::forward<RHS>(rhs);
};

template <class LHS, class RHS, class Ret>
concept CanPipe_R = requires(LHS lhs, RHS rhs) {
    { std::forward<LHS>(lhs) | std::forward<RHS>(rhs) } -> std::same_as<Ret>;
};

using TestRange = std::array<int, 1>;

template <class T>
constexpr bool does_not_satisfy_range_adaptor_closure() {
    return !CanPipe<TestRange,
               T&> && !CanPipe<TestRange, const T&> && !CanPipe<TestRange, T&&> && !CanPipe<TestRange, const T&&>;
}

struct IdentityRangeAdaptorClosure : std::ranges::range_adaptor_closure<IdentityRangeAdaptorClosure> {
    template <class T>
    constexpr decltype(auto) operator()(T&& range) const {
        return std::forward<T>(range);
    }
};

// Is not a range adaptor closure, because it is not a function object.
struct NotCallable : std::ranges::range_adaptor_closure<NotCallable> {};
static_assert(does_not_satisfy_range_adaptor_closure<NotCallable>());

// Is not a range adaptor closure, because it does not accept a range as argument.
struct NotCallableWithRange : std::ranges::range_adaptor_closure<NotCallableWithRange> {
    void operator()() {}
};
static_assert(does_not_satisfy_range_adaptor_closure<NotCallableWithRange>());

// Is not a range adaptor closure, because it doesn't derive from range_adaptor_closure.
struct NotDerivedFrom {
    void operator()(const TestRange&) {}
};
static_assert(does_not_satisfy_range_adaptor_closure<NotDerivedFrom>());

// Is not a range adaptor closure, because it inherits privately from range_adaptor_closure.
struct DerivedPrivately : private std::ranges::range_adaptor_closure<DerivedPrivately> {
    void operator()(const TestRange&) {}
};
static_assert(does_not_satisfy_range_adaptor_closure<DerivedPrivately>());

// Is not a range adaptor closure, because it inherits from the wrong specialization of range_adaptor_closure.
struct DerivedFromWrongSpecialization : std::ranges::range_adaptor_closure<IdentityRangeAdaptorClosure> {
    void operator()(const TestRange&) {}
};
static_assert(does_not_satisfy_range_adaptor_closure<DerivedFromWrongSpecialization>());

// Is not a range adaptor closure, because it has two base classes which are specializations of
// range_adaptor_closure.
struct DerivedFromTwoSpecializations : std::ranges::range_adaptor_closure<DerivedFromTwoSpecializations>,
                                       std::ranges::range_adaptor_closure<IdentityRangeAdaptorClosure> {
    void operator()(const TestRange&) {}
};
static_assert(does_not_satisfy_range_adaptor_closure<DerivedFromTwoSpecializations>());

// Is not a range adaptor closure, because it models std::ranges::range.
struct ModelsRange : std::ranges::range_adaptor_closure<ModelsRange> {
    void operator()(const TestRange&) {}

    int* begin() {
        return nullptr;
    }
    int* begin() const {
        return nullptr;
    }
    int* end() {
        return nullptr;
    }
    int* end() const {
        return nullptr;
    }
};
static_assert(does_not_satisfy_range_adaptor_closure<ModelsRange>());

struct RangeAdaptorClosureMemberRefQualTest : std::ranges::range_adaptor_closure<RangeAdaptorClosureMemberRefQualTest> {
    constexpr std::ranges::empty_view<char> operator()(const auto&) & {
        return std::views::empty<char>;
    }

    constexpr std::ranges::empty_view<short> operator()(const auto&) && {
        return std::views::empty<short>;
    }

    constexpr std::ranges::empty_view<float> operator()(const auto&) const& {
        return std::views::empty<float>;
    }

    constexpr std::ranges::empty_view<double> operator()(const auto&) const&& {
        return std::views::empty<double>;
    }
};
static_assert(CanPipe_R<TestRange, RangeAdaptorClosureMemberRefQualTest&, std::ranges::empty_view<char>>);
static_assert(CanPipe_R<TestRange, RangeAdaptorClosureMemberRefQualTest&&, std::ranges::empty_view<short>>);
static_assert(CanPipe_R<TestRange, RangeAdaptorClosureMemberRefQualTest const&, std::ranges::empty_view<float>>);
static_assert(CanPipe_R<TestRange, RangeAdaptorClosureMemberRefQualTest const&&, std::ranges::empty_view<double>>);

using FirstIdentityThenMemberRefQualTest = decltype(
    IdentityRangeAdaptorClosure{} | RangeAdaptorClosureMemberRefQualTest{});
static_assert(CanPipe_R<TestRange, FirstIdentityThenMemberRefQualTest&, std::ranges::empty_view<char>>);
static_assert(CanPipe_R<TestRange, FirstIdentityThenMemberRefQualTest&&, std::ranges::empty_view<short>>);
static_assert(CanPipe_R<TestRange, FirstIdentityThenMemberRefQualTest const&, std::ranges::empty_view<float>>);
static_assert(CanPipe_R<TestRange, FirstIdentityThenMemberRefQualTest const&&, std::ranges::empty_view<double>>);

using FirstTransformThenMemberRefQualTest = decltype(
    std::views::transform([](auto x) { return x; }) | RangeAdaptorClosureMemberRefQualTest{});
static_assert(CanPipe_R<TestRange, FirstTransformThenMemberRefQualTest&, std::ranges::empty_view<char>>);
static_assert(CanPipe_R<TestRange, FirstTransformThenMemberRefQualTest&&, std::ranges::empty_view<short>>);
static_assert(CanPipe_R<TestRange, FirstTransformThenMemberRefQualTest const&, std::ranges::empty_view<float>>);
static_assert(CanPipe_R<TestRange, FirstTransformThenMemberRefQualTest const&&, std::ranges::empty_view<double>>);

using FirstMemberRefQualTestThenAll = decltype(RangeAdaptorClosureMemberRefQualTest{} | std::views::all);
static_assert(CanPipe_R<TestRange, FirstMemberRefQualTestThenAll&, std::ranges::empty_view<char>>);
static_assert(CanPipe_R<TestRange, FirstMemberRefQualTestThenAll&&, std::ranges::empty_view<short>>);
static_assert(CanPipe_R<TestRange, FirstMemberRefQualTestThenAll const&, std::ranges::empty_view<float>>);
static_assert(CanPipe_R<TestRange, FirstMemberRefQualTestThenAll const&&, std::ranges::empty_view<double>>);

struct RangeAdaptorClosureParameterRefQualTest
    : std::ranges::range_adaptor_closure<RangeAdaptorClosureParameterRefQualTest> {
    constexpr char operator()(TestRange&) {
        return {};
    }

    constexpr short operator()(TestRange&&) {
        return {};
    }

    constexpr float operator()(const TestRange&) {
        return {};
    }

    constexpr double operator()(const TestRange&&) {
        return {};
    }
};
static_assert(CanPipe_R<TestRange&, RangeAdaptorClosureParameterRefQualTest, char>);
static_assert(CanPipe_R<TestRange&&, RangeAdaptorClosureParameterRefQualTest, short>);
static_assert(CanPipe_R<const TestRange&, RangeAdaptorClosureParameterRefQualTest, float>);
static_assert(CanPipe_R<const TestRange&&, RangeAdaptorClosureParameterRefQualTest, double>);

struct MoveOnlyRangeAdaptorClosure : std::ranges::range_adaptor_closure<MoveOnlyRangeAdaptorClosure> {
    MoveOnlyRangeAdaptorClosure()                                   = default;
    MoveOnlyRangeAdaptorClosure(const MoveOnlyRangeAdaptorClosure&) = delete;
    MoveOnlyRangeAdaptorClosure(MoveOnlyRangeAdaptorClosure&&)      = default;

    void operator()(const TestRange&) {}
};
static_assert(CanPipe<TestRange, MoveOnlyRangeAdaptorClosure>);
static_assert(CanPipe<TestRange, MoveOnlyRangeAdaptorClosure&>);
static_assert(CanPipe<MoveOnlyRangeAdaptorClosure, IdentityRangeAdaptorClosure>);
static_assert(CanPipe<IdentityRangeAdaptorClosure, MoveOnlyRangeAdaptorClosure>);
static_assert(!CanPipe<MoveOnlyRangeAdaptorClosure&, IdentityRangeAdaptorClosure>);
static_assert(!CanPipe<IdentityRangeAdaptorClosure, MoveOnlyRangeAdaptorClosure&>);

class TimesTwoAdaptor : public std::ranges::range_adaptor_closure<TimesTwoAdaptor> {
public:
    template <std::ranges::range R>
    constexpr auto operator()(R&& range) const {
        return std::forward<R>(range) | std::views::transform([](auto x) { return x * 2; });
    }
};

class DividedByTwoAdaptor : public std::ranges::range_adaptor_closure<DividedByTwoAdaptor> {
public:
    template <std::ranges::range R>
    constexpr auto operator()(R&& range) const {
        return std::forward<R>(range) | std::views::transform([](auto x) { return x / 2; });
    }
};

constexpr bool test_user_defined_adaptors() {
    const std::array<int, 3> numbers{1, 2, 3};
    const std::array<int, 3> numbers_times_two{2, 4, 6};
    assert(std::ranges::equal(numbers_times_two, numbers | TimesTwoAdaptor{}));
    assert(std::ranges::equal(numbers, (numbers | TimesTwoAdaptor{}) | DividedByTwoAdaptor{}));
    assert(std::ranges::equal(numbers, numbers | (TimesTwoAdaptor{} | DividedByTwoAdaptor{})));
    assert(std::ranges::equal(numbers_times_two,
        (numbers | TimesTwoAdaptor{})
            | (TimesTwoAdaptor{} | TimesTwoAdaptor{} | DividedByTwoAdaptor{} | DividedByTwoAdaptor{})));

    return true;
}

constexpr bool test_mixing_of_range_adaptors() {
    const std::array<int, 3> numbers{1, 2, 3};
    assert(std::ranges::equal(
        numbers, numbers | (TimesTwoAdaptor{} | std::views::transform([](int x) { return x / 2; }))));

    const auto mixed_pipeline = TimesTwoAdaptor{} | std::views::reverse | DividedByTwoAdaptor{} | std::views::reverse;
    assert(std::ranges::equal(mixed_pipeline(numbers), numbers));
    assert(std::ranges::equal(numbers | mixed_pipeline, numbers));

    auto factory_pipeline = std::views::iota(1) | TimesTwoAdaptor{} | std::views::take(3);
    assert(std::ranges::equal(factory_pipeline, std::array{2, 4, 6}));

    return true;
}

int main() {
    assert(test_user_defined_adaptors());
    static_assert(test_user_defined_adaptors());

    assert(test_mixing_of_range_adaptors());
    static_assert(test_mixing_of_range_adaptors());
}
