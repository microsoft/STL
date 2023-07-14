// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <ranges>
#include <utility>

using namespace std;

template <class T>
concept CanInstantiateRangeAdaptorClosure = requires { typename ranges::range_adaptor_closure<T>; };

class EmptyTestType {};
class IncompleteTestType;

static_assert(CanInstantiateRangeAdaptorClosure<EmptyTestType>);
static_assert(!CanInstantiateRangeAdaptorClosure<const EmptyTestType>);
static_assert(!CanInstantiateRangeAdaptorClosure<volatile EmptyTestType>);
static_assert(!CanInstantiateRangeAdaptorClosure<EmptyTestType&>);
static_assert(!CanInstantiateRangeAdaptorClosure<int>);
static_assert(CanInstantiateRangeAdaptorClosure<IncompleteTestType>);

template <class LHS, class RHS>
concept CanPipe = requires(LHS lhs, RHS rhs) { forward<LHS>(lhs) | forward<RHS>(rhs); };

template <class LHS, class RHS, class Ret>
concept CanPipe_R = requires(LHS lhs, RHS rhs) {
    { forward<LHS>(lhs) | forward<RHS>(rhs) } -> same_as<Ret>;
};

using TestRange = array<int, 1>;

template <class T>
constexpr bool is_range_adaptor_closure() {
    return CanPipe<TestRange, T&> || CanPipe<TestRange, const T&> || CanPipe<TestRange, T&&>
        || CanPipe<TestRange, const T&&>;
}

struct IdentityRangeAdaptorClosure : ranges::range_adaptor_closure<IdentityRangeAdaptorClosure> {
    template <class T>
    constexpr decltype(auto) operator()(T&& range) const {
        return forward<T>(range);
    }
};

// Is not a range adaptor closure, because it is not a function object.
struct NotCallable : ranges::range_adaptor_closure<NotCallable> {};
static_assert(!is_range_adaptor_closure<NotCallable>());

// Is not a range adaptor closure, because it does not accept a range as argument.
struct NotCallableWithRange : ranges::range_adaptor_closure<NotCallableWithRange> {
    void operator()() {}
};
static_assert(!is_range_adaptor_closure<NotCallableWithRange>());

// Is not a range adaptor closure, because it doesn't derive from range_adaptor_closure.
struct NotDerivedFrom {
    void operator()(const TestRange&) {}
};
static_assert(!is_range_adaptor_closure<NotDerivedFrom>());

// Is not a range adaptor closure, because it inherits privately from range_adaptor_closure.
struct DerivedPrivately : private ranges::range_adaptor_closure<DerivedPrivately> {
    void operator()(const TestRange&) {}
};
static_assert(!is_range_adaptor_closure<DerivedPrivately>());

// Is not a range adaptor closure, because it inherits from the wrong specialization of range_adaptor_closure.
struct DerivedFromWrongSpecialization : ranges::range_adaptor_closure<IdentityRangeAdaptorClosure> {
    void operator()(const TestRange&) {}
};
static_assert(!is_range_adaptor_closure<DerivedFromWrongSpecialization>());

// Is not a range adaptor closure, because it has two base classes which are specializations of
// range_adaptor_closure.
struct DerivedFromTwoSpecializations : ranges::range_adaptor_closure<DerivedFromTwoSpecializations>,
                                       ranges::range_adaptor_closure<IdentityRangeAdaptorClosure> {
    void operator()(const TestRange&) {}
};
static_assert(!is_range_adaptor_closure<DerivedFromTwoSpecializations>());

// Is not a range adaptor closure, because it models ranges::range.
struct ModelsRange : ranges::range_adaptor_closure<ModelsRange> {
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
static_assert(ranges::range<ModelsRange>);
static_assert(!is_range_adaptor_closure<ModelsRange>());

struct RangeAdaptorClosureMemberRefQualTest : ranges::range_adaptor_closure<RangeAdaptorClosureMemberRefQualTest> {
    constexpr ranges::empty_view<char> operator()(const auto&) & {
        return views::empty<char>;
    }

    constexpr ranges::empty_view<short> operator()(const auto&) && {
        return views::empty<short>;
    }

    constexpr ranges::empty_view<float> operator()(const auto&) const& {
        return views::empty<float>;
    }

    constexpr ranges::empty_view<double> operator()(const auto&) const&& {
        return views::empty<double>;
    }
};
static_assert(CanPipe_R<TestRange, RangeAdaptorClosureMemberRefQualTest&, ranges::empty_view<char>>);
static_assert(CanPipe_R<TestRange, RangeAdaptorClosureMemberRefQualTest&&, ranges::empty_view<short>>);
static_assert(CanPipe_R<TestRange, RangeAdaptorClosureMemberRefQualTest const&, ranges::empty_view<float>>);
static_assert(CanPipe_R<TestRange, RangeAdaptorClosureMemberRefQualTest const&&, ranges::empty_view<double>>);

using FirstIdentityThenMemberRefQualTest =
    decltype(IdentityRangeAdaptorClosure{} | RangeAdaptorClosureMemberRefQualTest{});
static_assert(CanPipe_R<TestRange, FirstIdentityThenMemberRefQualTest&, ranges::empty_view<char>>);
static_assert(CanPipe_R<TestRange, FirstIdentityThenMemberRefQualTest&&, ranges::empty_view<short>>);
static_assert(CanPipe_R<TestRange, FirstIdentityThenMemberRefQualTest const&, ranges::empty_view<float>>);
static_assert(CanPipe_R<TestRange, FirstIdentityThenMemberRefQualTest const&&, ranges::empty_view<double>>);

using FirstTransformThenMemberRefQualTest =
    decltype(views::transform([](auto x) { return x; }) | RangeAdaptorClosureMemberRefQualTest{});
static_assert(CanPipe_R<TestRange, FirstTransformThenMemberRefQualTest&, ranges::empty_view<char>>);
static_assert(CanPipe_R<TestRange, FirstTransformThenMemberRefQualTest&&, ranges::empty_view<short>>);
static_assert(CanPipe_R<TestRange, FirstTransformThenMemberRefQualTest const&, ranges::empty_view<float>>);
static_assert(CanPipe_R<TestRange, FirstTransformThenMemberRefQualTest const&&, ranges::empty_view<double>>);

using FirstMemberRefQualTestThenAll = decltype(RangeAdaptorClosureMemberRefQualTest{} | views::all);
static_assert(CanPipe_R<TestRange, FirstMemberRefQualTestThenAll&, ranges::empty_view<char>>);
static_assert(CanPipe_R<TestRange, FirstMemberRefQualTestThenAll&&, ranges::empty_view<short>>);
static_assert(CanPipe_R<TestRange, FirstMemberRefQualTestThenAll const&, ranges::empty_view<float>>);
static_assert(CanPipe_R<TestRange, FirstMemberRefQualTestThenAll const&&, ranges::empty_view<double>>);

struct RangeAdaptorClosureParameterRefQualTest
    : ranges::range_adaptor_closure<RangeAdaptorClosureParameterRefQualTest> {
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

struct MoveOnlyRangeAdaptorClosure : ranges::range_adaptor_closure<MoveOnlyRangeAdaptorClosure> {
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

class TimesTwoAdaptor : public ranges::range_adaptor_closure<TimesTwoAdaptor> {
public:
    template <ranges::range R>
    constexpr auto operator()(R&& range) const {
        return forward<R>(range) | views::transform([](auto x) { return x * 2; });
    }
};

class DividedByTwoAdaptor : public ranges::range_adaptor_closure<DividedByTwoAdaptor> {
public:
    template <ranges::range R>
    constexpr auto operator()(R&& range) const {
        return forward<R>(range) | views::transform([](auto x) { return x / 2; });
    }
};

constexpr bool test_user_defined_adaptors() {
    const array<int, 3> numbers{1, 2, 3};
    const array<int, 3> numbers_times_two{2, 4, 6};
    assert(ranges::equal(numbers_times_two, numbers | TimesTwoAdaptor{}));
    assert(ranges::equal(numbers, (numbers | TimesTwoAdaptor{}) | DividedByTwoAdaptor{}));
    assert(ranges::equal(numbers, numbers | (TimesTwoAdaptor{} | DividedByTwoAdaptor{})));
    assert(ranges::equal(numbers_times_two,
        (numbers | TimesTwoAdaptor{})
            | (TimesTwoAdaptor{} | TimesTwoAdaptor{} | DividedByTwoAdaptor{} | DividedByTwoAdaptor{})));

    return true;
}

constexpr bool test_mixing_of_range_adaptors() {
    const array<int, 3> numbers{1, 2, 3};
    assert(ranges::equal(numbers, numbers | (TimesTwoAdaptor{} | views::transform([](int x) { return x / 2; }))));

    const auto mixed_pipeline = TimesTwoAdaptor{} | views::reverse | DividedByTwoAdaptor{} | views::reverse;
    assert(ranges::equal(mixed_pipeline(numbers), numbers));
    assert(ranges::equal(numbers | mixed_pipeline, numbers));

    auto factory_pipeline = views::iota(1) | TimesTwoAdaptor{} | views::take(3);
    assert(ranges::equal(factory_pipeline, array{2, 4, 6}));

    return true;
}

int main() {
    assert(test_user_defined_adaptors());
    static_assert(test_user_defined_adaptors());

    assert(test_mixing_of_range_adaptors());
    static_assert(test_mixing_of_range_adaptors());
}
