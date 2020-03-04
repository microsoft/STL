// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <compare>
#include <concepts>
#include <ranges>
#include <utility>
//
#include <range_algorithm_support.hpp>

template <class T, class U>
struct not_pair {
    T first;
    U second;

    auto operator<=>(const not_pair&) const = default;
};

constexpr void smoke_test() {
    using ranges::copy_if, ranges::copy_if_result, ranges::iterator_t;
    using std::same_as;
    using P = not_pair<int, int>;

    constexpr auto is_odd = [](int x) { return x % 2 != 0; };

    // Validate that copy_if_result aliases in_out_result
    STATIC_ASSERT(same_as<copy_if_result<int, double>, ranges::in_out_result<int, double>>);

    // Validate dangling story
    STATIC_ASSERT(same_as<decltype(copy_if(borrowed<false>{}, static_cast<int*>(nullptr), is_odd)),
        copy_if_result<ranges::dangling, int*>>);
    STATIC_ASSERT(
        same_as<decltype(copy_if(borrowed<true>{}, static_cast<int*>(nullptr), is_odd)), copy_if_result<int*, int*>>);

    std::array<P, 3> const input    = {{{1, 99}, {4, 98}, {5, 97}}};
    std::array<P, 2> const expected = {{{1, 99}, {5, 97}}};
    using I                         = iterator_t<move_only_range<P const>>;
    using O                         = iterator_t<move_only_range<P>>;
    { // Validate range overload
        std::array<P, 2> output = {};
        auto result             = copy_if(move_only_range{input}, move_only_range{output}.begin(), is_odd, get_first);
        STATIC_ASSERT(same_as<decltype(result), copy_if_result<I, O>>);
        assert(result.in == move_only_range{input}.end());
        assert(result.out == move_only_range{output}.end());
        assert(ranges::equal(output, expected));
    }
    { // Validate iterator + sentinel overload
        std::array<P, 2> output = {};
        move_only_range wrapped_input{input};
        auto result =
            copy_if(wrapped_input.begin(), wrapped_input.end(), move_only_range{output}.begin(), is_odd, get_first);
        STATIC_ASSERT(same_as<decltype(result), copy_if_result<I, O>>);
        assert(result.in == wrapped_input.end());
        assert(result.out == move_only_range{output}.end());
        assert(ranges::equal(output, expected));
    }
}

int main() {
    STATIC_ASSERT((smoke_test(), true));
    smoke_test();
}

struct instantiator {
    template <class In, class Out>
    static void call(In&& in = {}, Out out = {}) {
        using ranges::begin, ranges::copy_if, ranges::end, ranges::iterator_t;

        constexpr UnaryPredicateFor<iterator_t<In>> pred{};
        constexpr ProjectionFor<iterator_t<In>> proj{};

        copy_if(in, std::move(out), pred);
        copy_if(begin(in), end(in), std::move(out), pred);
        copy_if(in, std::move(out), ProjectedUnaryPredicate<>{}, proj);
        copy_if(begin(in), end(in), std::move(out), ProjectedUnaryPredicate<>{}, proj);
    }
};

template void test_in_out<instantiator>();
