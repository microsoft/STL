// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <ranges>
#include <utility>

#include <range_algorithm_support.hpp>

constexpr void smoke_test() {
    using ranges::copy, ranges::copy_result, ranges::iterator_t;
    using std::same_as;

    // Validate that copy_result aliases in_out_result
    STATIC_ASSERT(same_as<copy_result<int, double>, ranges::in_out_result<int, double>>);

    // Validate dangling story
    STATIC_ASSERT(
        same_as<decltype(copy(borrowed<false>{}, static_cast<int*>(nullptr))), copy_result<ranges::dangling, int*>>);
    STATIC_ASSERT(same_as<decltype(copy(borrowed<true>{}, static_cast<int*>(nullptr))), copy_result<int*, int*>>);

    int const input[] = {13, 42, 1729};
    { // Validate range overload
        int output[] = {-1, -1, -1};
        auto result  = copy(move_only_range{input}, move_only_range{output}.begin());
        STATIC_ASSERT(same_as<decltype(result),
            copy_result<iterator_t<move_only_range<int const>>, iterator_t<move_only_range<int>>>>);
        assert(result.in == move_only_range{input}.end());
        assert(result.out == move_only_range{output}.end());
        assert(ranges::equal(output, input));
    }
    { // Validate iterator + sentinel overload
        int output[] = {-1, -1, -1};
        move_only_range wrapped_input{input};
        auto result = copy(wrapped_input.begin(), wrapped_input.end(), move_only_range{output}.begin());
        STATIC_ASSERT(same_as<decltype(result),
            copy_result<iterator_t<move_only_range<int const>>, iterator_t<move_only_range<int>>>>);
        assert(result.in == wrapped_input.end());
        assert(result.out == move_only_range{output}.end());
        assert(ranges::equal(output, input));
    }
}

int main() {
    STATIC_ASSERT((smoke_test(), true));
    smoke_test();
}

struct instantiator {
    template <class In, class Out>
    static void call(In&& in = {}, Out out = {}) {
        (void) ranges::copy(in, std::move(out));
        (void) ranges::copy(ranges::begin(in), ranges::end(in), std::move(out));
    }
};

template void test_in_write<instantiator>();
