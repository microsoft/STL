// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <ranges>
#include <span>

#include <range_algorithm_support.hpp>

using namespace std;

// Validate that copy_backward_result aliases in_out_result
STATIC_ASSERT(same_as<ranges::copy_backward_result<int, double>, ranges::in_out_result<int, double>>);

// Validate dangling story
STATIC_ASSERT(same_as<decltype(ranges::copy_backward(borrowed<false>{}, static_cast<int*>(nullptr))),
    ranges::copy_backward_result<ranges::dangling, int*>>);
STATIC_ASSERT(same_as<decltype(ranges::copy_backward(borrowed<true>{}, static_cast<int*>(nullptr))),
    ranges::copy_backward_result<int*, int*>>);

static constexpr int input[]                = {13, 42, 1729};
static constexpr int expected_overlapping[] = {0, 0, 1, 2};

struct instantiator {
    template <class Bidi1, class Bidi2>
    static constexpr void call() {
        // For the second range, we need an iterator to the end; it's expedient to simply ignore ranges with differing
        // iterator and sentinel types (i.e., ranges that don't satisfy common_range).
        if constexpr (ranges::common_range<Bidi2>) {
            using ranges::copy_backward, ranges::copy_backward_result, ranges::equal, ranges::iterator_t;
            const Bidi1 wrapped_input{input};

            { // Validate range overload
                int output[] = {-2, -2, -2};
                Bidi2 wrapped_output{output};
                auto result = copy_backward(wrapped_input, wrapped_output.end());
                STATIC_ASSERT(same_as<decltype(result), copy_backward_result<iterator_t<Bidi1>, iterator_t<Bidi2>>>);
                assert(result.in == wrapped_input.end());
                assert(result.out == wrapped_output.begin());
                assert(equal(output, input));
            }
            { // Validate iterator + sentinel overload
                int output[] = {-2, -2, -2};
                Bidi2 wrapped_output{output};
                auto result = copy_backward(wrapped_input.begin(), wrapped_input.end(), wrapped_output.end());
                STATIC_ASSERT(same_as<decltype(result), copy_backward_result<iterator_t<Bidi1>, iterator_t<Bidi2>>>);
                assert(result.in == wrapped_input.end());
                assert(result.out == wrapped_output.begin());
                assert(equal(output, input));
            }
            { // Validate overlapping ranges
                int io[] = {0, 1, 2, 42};
                Bidi1 in{span{io}.first<3>()};
                Bidi2 out{span{io}.last<3>()};
                auto result = copy_backward(in, out.end());
                STATIC_ASSERT(same_as<decltype(result), copy_backward_result<iterator_t<Bidi1>, iterator_t<Bidi2>>>);
                assert(result.in == in.end());
                assert(result.out == out.begin());
                assert(equal(io, expected_overlapping));
            }
        }
    }
};

constexpr void test_memmove() {
    // Get some coverage for the memmove optimization, which we would not otherwise have since we do not currently
    // unwrap output iterators. TRANSITION, GH-893
    using ranges::copy_backward, ranges::copy_backward_result, ranges::begin, ranges::end, ranges::equal;

    { // Validate range overload
        int output[] = {-2, -2, -2};
        auto result  = copy_backward(input, end(output));
        STATIC_ASSERT(same_as<decltype(result), copy_backward_result<const int*, int*>>);
        assert(result.in == end(input));
        assert(result.out == begin(output));
        assert(equal(output, input));
    }
    { // Validate iterator + sentinel overload
        int output[] = {-2, -2, -2};
        auto result  = copy_backward(begin(input), end(input), end(output));
        STATIC_ASSERT(same_as<decltype(result), copy_backward_result<const int*, int*>>);
        assert(result.in == end(input));
        assert(result.out == begin(output));
        assert(equal(output, input));
    }
    { // Validate overlapping ranges
        int io[]    = {0, 1, 2, 42};
        auto result = copy_backward(io + 0, io + 3, io + 4);
        STATIC_ASSERT(same_as<decltype(result), copy_backward_result<int*, int*>>);
        assert(result.in == io + 3);
        assert(result.out == io + 1);
        assert(equal(io, expected_overlapping));
    }
}

int main() {
    STATIC_ASSERT((test_bidi_bidi<instantiator, const int, int>(), true));
    test_bidi_bidi<instantiator, const int, int>();

    STATIC_ASSERT((test_memmove(), true));
    test_memmove();
}
