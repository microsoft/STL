// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <concepts>
#include <ranges>
#include <utility>
//
#include <range_algorithm_support.hpp>

constexpr void smoke_test() {
    using ranges::dangling, ranges::equal_to, ranges::iterator_t, ranges::mismatch, ranges::mismatch_result;
    using std::same_as;
    using P = std::pair<int, int>;
    using R = std::array<P, 3>;

    R const x                                    = {{{0, 42}, {2, 42}, {4, 42}}};
    std::array<std::pair<long, long>, 3> const y = {{{13, 0}, {13, 2}, {13, 5}}};

    // Validate that mismatch_result aliases in_in_result
    STATIC_ASSERT(same_as<mismatch_result<int, double>, ranges::in_in_result<int, double>>);

    // Validate dangling story
    STATIC_ASSERT(
        same_as<decltype(mismatch(borrowed<false>{}, borrowed<false>{})), mismatch_result<dangling, dangling>>);
    STATIC_ASSERT(same_as<decltype(mismatch(borrowed<false>{}, borrowed<true>{})), mismatch_result<dangling, int*>>);
    STATIC_ASSERT(same_as<decltype(mismatch(borrowed<true>{}, borrowed<false>{})), mismatch_result<int*, dangling>>);
    STATIC_ASSERT(same_as<decltype(mismatch(borrowed<true>{}, borrowed<true>{})), mismatch_result<int*, int*>>);

    {
        // Validate sized ranges
        auto result = mismatch(x, y, equal_to{}, get_first, get_second);
        using I1    = iterator_t<R const>;
        using I2    = std::array<std::pair<long, long>, 3>::const_iterator;
        STATIC_ASSERT(same_as<decltype(result), mismatch_result<I1, I2>>);
        assert((*result.in1 == P{4, 42}));
        assert((*result.in2 == std::pair<long, long>{13, 5}));
    }
    {
        // Validate non-sized ranges
        auto result = mismatch(move_only_range{x}, move_only_range{y}, equal_to{}, get_first, get_second);
        using I1    = iterator_t<move_only_range<P const>>;
        using I2    = iterator_t<move_only_range<std::pair<long, long> const>>;
        STATIC_ASSERT(same_as<decltype(result), mismatch_result<I1, I2>>);
        assert((*result.in1 == P{4, 42}));
        assert((*result.in2 == std::pair<long, long>{13, 5}));
    }
    {
        // Validate sized iterator + sentinel pairs
        auto result = mismatch(x.begin(), x.end(), y.begin(), y.end(), equal_to{}, get_first, get_second);
        using I1    = iterator_t<R const>;
        using I2    = std::array<std::pair<long, long>, 3>::const_iterator;
        STATIC_ASSERT(same_as<decltype(result), mismatch_result<I1, I2>>);
        assert((*result.in1 == P{4, 42}));
        assert((*result.in2 == std::pair<long, long>{13, 5}));
    }
    {
        // Validate non-sized iterator + sentinel pairs
        move_only_range wrapped_x{x};
        move_only_range wrapped_y{y};
        auto result = mismatch(
            wrapped_x.begin(), wrapped_x.end(), wrapped_y.begin(), wrapped_y.end(), equal_to{}, get_first, get_second);
        using I1 = iterator_t<decltype(wrapped_x)>;
        using I2 = iterator_t<decltype(wrapped_y)>;
        STATIC_ASSERT(same_as<decltype(result), mismatch_result<I1, I2>>);
        assert((*result.in1 == P{4, 42}));
        assert((*result.in2 == std::pair<long, long>{13, 5}));
    }
}

int main() {
    STATIC_ASSERT((smoke_test(), true));
    smoke_test();
}

struct instantiator {
    template <class In1, class In2>
    static void call(In1&& in1 = {}, In2&& in2 = {}) {
        using ranges::begin, ranges::end, ranges::mismatch, ranges::iterator_t;

        if constexpr (!is_permissive) {
            (void) mismatch(in1, in2);
            (void) mismatch(begin(in1), end(in1), begin(in2), end(in2));
        }

        BinaryPredicateFor<iterator_t<In1>, iterator_t<In2>> pred{};
        (void) mismatch(in1, in2, pred);
        (void) mismatch(begin(in1), end(in1), begin(in2), end(in2), pred);

        HalfProjectedBinaryPredicateFor<iterator_t<In2>> halfpred{};
        ProjectionFor<iterator_t<In1>> halfproj{};
        (void) mismatch(in1, in2, halfpred, halfproj);
        (void) mismatch(begin(in1), end(in1), begin(in2), end(in2), halfpred, halfproj);

        ProjectedBinaryPredicate<0, 1> projpred{};
        ProjectionFor<iterator_t<In1>, 0> proj1{};
        ProjectionFor<iterator_t<In2>, 1> proj2{};
        (void) mismatch(in1, in2, projpred, proj1, proj2);
        (void) mismatch(begin(in1), end(in1), begin(in2), end(in2), projpred, proj1, proj2);
    }
};

template void test_in_in<instantiator>();
