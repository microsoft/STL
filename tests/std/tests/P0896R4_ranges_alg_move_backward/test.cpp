// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <ranges>
#include <span>
#include <type_traits>
#include <utility>

#include <range_algorithm_support.hpp>

using namespace std;

struct int_wrapper {
    int val = 10;

    constexpr int_wrapper() = default;
    constexpr int_wrapper(int x) : val{x} {}
    constexpr int_wrapper(int_wrapper&& that) : val{exchange(that.val, -1)} {}
    constexpr int_wrapper& operator=(int_wrapper&& that) {
        val = exchange(that.val, -1);
        return *this;
    }
    auto operator<=>(const int_wrapper&) const = default;
};

#ifdef _M_CEE // TRANSITION, VSO-1664341
constexpr auto get_int_wrapper_val = [](const int_wrapper& w) { return w.val; };
#else // ^^^ workaround / no workaround vvv
constexpr auto get_int_wrapper_val = &int_wrapper::val;
#endif // ^^^ no workaround ^^^

// Validate that move_backward_result aliases in_out_result
STATIC_ASSERT(same_as<ranges::move_backward_result<int, double>, ranges::in_out_result<int, double>>);

// Validate dangling story
STATIC_ASSERT(same_as<decltype(ranges::move_backward(borrowed<false>{}, nullptr_to<int>)),
    ranges::move_backward_result<ranges::dangling, int*>>);
STATIC_ASSERT(same_as<decltype(ranges::move_backward(borrowed<true>{}, nullptr_to<int>)),
    ranges::move_backward_result<int*, int*>>);

struct instantiator {
    static constexpr int expected_output[]      = {13, 42, 1729};
    static constexpr int expected_input[]       = {-1, -1, -1};
    static constexpr int expected_overlapping[] = {-1, 0, 1, 2};

    template <ranges::bidirectional_range R1, ranges::bidirectional_range R2>
    static constexpr void call() {
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
                assert(equal(output, expected_output, ranges::equal_to{}, get_int_wrapper_val));
                assert(equal(input, expected_input, ranges::equal_to{}, get_int_wrapper_val));
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
                assert(equal(output, expected_output, ranges::equal_to{}, get_int_wrapper_val));
                assert(equal(input, expected_input, ranges::equal_to{}, get_int_wrapper_val));
            }
            { // Validate overlapping ranges
                int_wrapper io[] = {0, 1, 2, 42};
                R1 wrapped_input{span{io}.first<3>()};
                R2 wrapped_output{span{io}.last<3>()};
                same_as<move_backward_result<iterator_t<R1>, iterator_t<R2>>> auto result =
                    move_backward(wrapped_input, wrapped_output.end());
                assert(result.in == wrapped_input.end());
                assert(result.out == wrapped_output.begin());
                assert(equal(io, expected_overlapping, ranges::equal_to{}, get_int_wrapper_val));
            }
        }
    }
};

constexpr void test_memmove() {
    // Get some coverage for the memmove optimization, which we would not otherwise have since we do not currently
    // unwrap output iterators. TRANSITION, GH-893
    using ranges::move_backward, ranges::move_backward_result, ranges::begin, ranges::end, ranges::equal;

    struct S { // move-only and trivially copyable
        int val = 10;

        constexpr S() = default;
        constexpr S(int x) : val{x} {}
        constexpr S(S&&)                 = default;
        constexpr S& operator=(S&&)      = default;
        auto operator<=>(const S&) const = default;
    };

    { // Validate range overload
        S input[]                                               = {13, 42, 1729};
        S output[]                                              = {-2, -2, -2};
        const same_as<move_backward_result<S*, S*>> auto result = move_backward(input, end(output));
        assert(result.in == end(input));
        assert(result.out == begin(output));
        assert(equal(output, input));
    }
    { // Validate iterator + sentinel overload
        S input[]                                               = {13, 42, 1729};
        S output[]                                              = {-2, -2, -2};
        const same_as<move_backward_result<S*, S*>> auto result = move_backward(begin(input), end(input), end(output));
        assert(result.in == end(input));
        assert(result.out == begin(output));
        assert(equal(output, input));
    }
    { // Validate overlapping ranges
        S io[]                                                  = {0, 1, 2, 42};
        const same_as<move_backward_result<S*, S*>> auto result = move_backward(io + 0, io + 3, io + 4);
        assert(result.in == io + 3);
        assert(result.out == io + 1);
        constexpr int expected[] = {0, 0, 1, 2};
#ifdef _M_CEE // TRANSITION, VSO-1664341
        assert(equal(io, expected, ranges::equal_to{}, [](const S& s) { return s.val; }));
#else // ^^^ workaround / no workaround vvv
        assert(equal(io, expected, ranges::equal_to{}, &S::val));
#endif // ^^^ no workaround ^^^
    }
}

int main() {
    STATIC_ASSERT((test_bidi_bidi<instantiator, int_wrapper, int_wrapper>(), true));
    test_bidi_bidi<instantiator, int_wrapper, int_wrapper>();

    STATIC_ASSERT((test_memmove(), true));
    test_memmove();
}
