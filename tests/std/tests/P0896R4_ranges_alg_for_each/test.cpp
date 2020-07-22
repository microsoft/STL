// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <concepts>
#include <functional>
#include <ranges>
#include <utility>

#include <range_algorithm_support.hpp>

constexpr void smoke_test() {
    using ranges::for_each, ranges::for_each_result, ranges::in_fun_result, ranges::iterator_t;
    using std::identity, std::same_as;
    using P = std::pair<int, int>;
    using R = std::array<P, 3>;

    // Validate that for_each_result aliases in_fun_result
    STATIC_ASSERT(same_as<for_each_result<int, double>, in_fun_result<int, double>>);

    // Validate dangling story
    STATIC_ASSERT(
        same_as<decltype(for_each(borrowed<false>{}, identity{})), for_each_result<ranges::dangling, identity>>);
    STATIC_ASSERT(same_as<decltype(for_each(borrowed<true>{}, identity{})), for_each_result<int*, identity>>);

    R const values = {{{0, 42}, {2, 42}, {4, 42}}};
    auto incr      = [](auto& y) { ++y; };

    {
        auto pairs  = values;
        auto result = for_each(basic_borrowed_range{pairs}, incr, get_first);
        STATIC_ASSERT(same_as<decltype(result), for_each_result<iterator_t<basic_borrowed_range<P>>, decltype(incr)>>);
        assert(result.in == basic_borrowed_range{pairs}.end());
        int some_value = 1729;
        result.fun(some_value);
        assert(some_value == 1730);
        R const expected = {{{1, 42}, {3, 42}, {5, 42}}};
        assert(ranges::equal(pairs, expected));
    }
    {
        auto pairs = values;
        basic_borrowed_range wrapped_pairs{pairs};
        auto result = for_each(wrapped_pairs.begin(), wrapped_pairs.end(), incr, get_second);
        STATIC_ASSERT(same_as<decltype(result), for_each_result<iterator_t<basic_borrowed_range<P>>, decltype(incr)>>);
        assert(result.in == wrapped_pairs.end());
        int some_value = 1729;
        result.fun(some_value);
        assert(some_value == 1730);
        R const expected = {{{0, 43}, {2, 43}, {4, 43}}};
        assert(ranges::equal(pairs, expected));
    }
}

int main() {
    STATIC_ASSERT((smoke_test(), true));
    smoke_test();
}

struct instantiator {
    template <class In>
    static void call(In&& in = {}) {
        using I   = ranges::iterator_t<In>;
        using Fun = void (*)(std::iter_common_reference_t<I>);
        ranges::for_each(in, Fun{});
        ranges::for_each(ranges::begin(in), ranges::end(in), Fun{});

        using ProjFun = void (*)(unique_tag<0>);
        ranges::for_each(in, ProjFun{}, ProjectionFor<I>{});
        ranges::for_each(ranges::begin(in), ranges::end(in), ProjFun{}, ProjectionFor<I>{});
    }
};

template void test_in<instantiator, const int>();
