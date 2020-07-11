// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <ranges>
#include <utility>

#include <range_algorithm_support.hpp>

struct int_wrapper {
    int val                 = 10;
    constexpr int_wrapper() = default;
    constexpr int_wrapper(int x) : val{x} {}
    constexpr int_wrapper(int_wrapper&& that) : val{std::exchange(that.val, -1)} {}
    constexpr int_wrapper& operator=(int_wrapper&& that) {
        val = std::exchange(that.val, -1);
        return *this;
    }
};

constexpr void smoke_test() {
    using ranges::move, ranges::move_result, ranges::iterator_t;
    using std::same_as;

    // Validate that move_result aliases in_out_result
    STATIC_ASSERT(same_as<move_result<int, double>, ranges::in_out_result<int, double>>);

    // Validate dangling story
    STATIC_ASSERT(
        same_as<decltype(move(borrowed<false>{}, static_cast<int*>(nullptr))), move_result<ranges::dangling, int*>>);
    STATIC_ASSERT(same_as<decltype(move(borrowed<true>{}, static_cast<int*>(nullptr))), move_result<int*, int*>>);

    int const input[] = {13, 53, 12435};
    {
        int output[] = {-2, -2, -2};
        auto result  = move(basic_borrowed_range{input}, basic_borrowed_range{output}.begin());
        STATIC_ASSERT(same_as<decltype(result),
            move_result<iterator_t<basic_borrowed_range<int const>>, iterator_t<basic_borrowed_range<int>>>>);
        assert(result.in == basic_borrowed_range{input}.end());
        assert(result.out == basic_borrowed_range{output}.end());
        assert(ranges::equal(output, input));
    }
    {
        int output[] = {-2, -2, -2};
        basic_borrowed_range wrapped_input{input};
        auto result = move(wrapped_input.begin(), wrapped_input.end(), basic_borrowed_range{output}.begin());
        STATIC_ASSERT(same_as<decltype(result),
            move_result<iterator_t<basic_borrowed_range<int const>>, iterator_t<basic_borrowed_range<int>>>>);
        assert(result.in == wrapped_input.end());
        assert(result.out == basic_borrowed_range{output}.end());
        assert(ranges::equal(output, input));
    }
    {
        int_wrapper input1[3]        = {13, 55, 1234};
        int const expected_output[3] = {13, 55, 1234};
        int_wrapper actual_output[3] = {-2, -2, -2};
        basic_borrowed_range wrapped_input{input1};
        auto result = move(wrapped_input.begin(), wrapped_input.end(), basic_borrowed_range{actual_output}.begin());
        assert(result.in == wrapped_input.end());
        assert(result.out == basic_borrowed_range{actual_output}.end());
        for (int i = 0; i < 3; ++i) {
            assert(input1[i].val == -1);
            assert(actual_output[i].val == expected_output[i]);
        }
    }
}

int main() {
    STATIC_ASSERT((smoke_test(), true));
    smoke_test();
}

struct instantiator {
    template <class In, class Out>
    static void call(In&& in = {}, Out out = {}) {
        (void) ranges::move(in, std::move(out));
        (void) ranges::move(ranges::begin(in), ranges::end(in), std::move(out));
    }
};

template void test_in_write<instantiator, const int, int>();
