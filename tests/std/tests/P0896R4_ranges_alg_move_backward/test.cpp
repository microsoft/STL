// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <ranges>
#include <span>

#include <range_algorithm_support.hpp>

using namespace std;

struct int_wrapper {
    int val                 = 10;
    constexpr int_wrapper() = default;
    constexpr int_wrapper(int x) : val{x} {}
    constexpr int_wrapper(int_wrapper&& that) : val{exchange(that.val, -1)} {}
    constexpr int_wrapper& operator=(int_wrapper&& that) {
        val = exchange(that.val, -1);
        return *this;
    }
    auto operator<=>(const int_wrapper&) const = default;
};

// Validate that move_backward_result aliases in_out_result
STATIC_ASSERT(same_as<ranges::move_backward_result<int, double>, ranges::in_out_result<int, double>>);

// Validate dangling story
STATIC_ASSERT(same_as<decltype(ranges::move_backward(borrowed<false>{}, static_cast<int*>(nullptr))),
    ranges::move_backward_result<ranges::dangling, int*>>);
STATIC_ASSERT(same_as<decltype(ranges::move_backward(borrowed<true>{}, static_cast<int*>(nullptr))),
    ranges::move_backward_result<int*, int*>>);

static constexpr int expected_output[]      = {13, 42, 1729};
static constexpr int expected_input[]       = {-1, -1, -1};
static constexpr int expected_overlapping[] = {-1, 0, 1, 2};

struct instantiator {
    template <ranges::bidirectional_range R1, ranges::bidirectional_range R2>
    static constexpr void call() {
#if !defined(__clang__) && !defined(__EDG__) // TRANSITION, VSO-938163
#pragma warning(suppress : 4127) // conditional expression is constant
        if (!ranges::contiguous_range<R1> || !ranges::contiguous_range<R1> || !is_constant_evaluated())
#endif // TRANSITION, VSO-938163
        {
            // For the second range, we need an iterator to the end; it's expedient to simply ignore ranges with
            // differing iterator and sentinel types (i.e., ranges that don't satisfy common_range).
            if constexpr (ranges::common_range<R2>) {
                using ranges::move_backward, ranges::move_backward_result, ranges::equal, ranges::iterator_t;

                { // Validate range overload
                    int_wrapper input[]  = {13, 42, 1729};
                    int_wrapper output[] = {-2, -2, -2};
                    R1 wrapped_input{input};
                    R2 wrapped_output{output};
                    same_as<move_backward_result<iterator_t<R1>, iterator_t<R2>>> auto result =
                        move_backward(wrapped_input, wrapped_output.end());
                    assert(result.in == wrapped_input.end());
                    assert(result.out == wrapped_output.begin());
                    assert(equal(output, expected_output, ranges::equal_to{}, &int_wrapper::val));
                    assert(equal(input, expected_input, ranges::equal_to{}, &int_wrapper::val));
                }
                { // Validate iterator + sentinel overload
                    int_wrapper input[]  = {13, 42, 1729};
                    int_wrapper output[] = {-2, -2, -2};
                    R1 wrapped_input{input};
                    R2 wrapped_output{output};
                    same_as<move_backward_result<iterator_t<R1>, iterator_t<R2>>> auto result =
                        move_backward(wrapped_input.begin(), wrapped_input.end(), wrapped_output.end());
                    assert(result.in == wrapped_input.end());
                    assert(result.out == wrapped_output.begin());
                    assert(equal(output, expected_output, ranges::equal_to{}, &int_wrapper::val));
                    assert(equal(input, expected_input, ranges::equal_to{}, &int_wrapper::val));
                }
                { // Validate overlapping ranges
                    int_wrapper io[] = {0, 1, 2, 42};
                    R1 wrapped_input{span{io}.first<3>()};
                    R2 wrapped_output{span{io}.last<3>()};
                    same_as<move_backward_result<iterator_t<R1>, iterator_t<R2>>> auto result =
                        move_backward(wrapped_input, wrapped_output.end());
                    assert(result.in == wrapped_input.end());
                    assert(result.out == wrapped_output.begin());
                    assert(equal(io, expected_overlapping, ranges::equal_to{}, &int_wrapper::val));
                }
            }
        }
    }
};

int main() {
    STATIC_ASSERT((test_bidi_bidi<instantiator, int_wrapper, int_wrapper>(), true));
    test_bidi_bidi<instantiator, int_wrapper, int_wrapper>();
}
