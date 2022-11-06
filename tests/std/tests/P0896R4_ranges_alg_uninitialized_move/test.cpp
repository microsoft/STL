// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <cstdlib>
#include <memory>
#include <ranges>
#include <span>
#include <utility>

#include <range_algorithm_support.hpp>

using namespace std;

// Validate that uninitialized_move_result aliases in_out_result
STATIC_ASSERT(same_as<ranges::uninitialized_move_result<int, double>, ranges::in_out_result<int, double>>);

// Validate dangling story
STATIC_ASSERT(same_as<decltype(ranges::uninitialized_move(borrowed<false>{}, borrowed<false>{})),
    ranges::uninitialized_move_result<ranges::dangling, ranges::dangling>>);
STATIC_ASSERT(same_as<decltype(ranges::uninitialized_move(borrowed<false>{}, borrowed<true>{})),
    ranges::uninitialized_move_result<ranges::dangling, int*>>);
STATIC_ASSERT(same_as<decltype(ranges::uninitialized_move(borrowed<true>{}, borrowed<false>{})),
    ranges::uninitialized_move_result<int*, ranges::dangling>>);
STATIC_ASSERT(same_as<decltype(ranges::uninitialized_move(borrowed<true>{}, borrowed<true>{})),
    ranges::uninitialized_move_result<int*, int*>>);

struct int_wrapper {
    inline static int constructions = 0;
    inline static int destructions  = 0;

    static void clear_counts() {
        constructions = 0;
        destructions  = 0;
    }

    static constexpr int magic_throwing_val = 29;
    int val                                 = 10;

    int_wrapper() {
        ++constructions;
    }
    int_wrapper(int x) : val{x} {
        ++constructions;
    }

    int_wrapper(int_wrapper&& that) {
        if (that.val == magic_throwing_val) {
            throw magic_throwing_val;
        }

        val = exchange(that.val, -1);
        ++constructions;
    }

    ~int_wrapper() {
        ++destructions;
    }

    int_wrapper& operator=(int_wrapper&&) {
        abort();
    }

    auto operator<=>(const int_wrapper&) const = default;
};
STATIC_ASSERT(movable<int_wrapper> && !copyable<int_wrapper>);

#ifdef _M_CEE // TRANSITION, VSO-1664341
constexpr auto get_int_wrapper_val = [](const int_wrapper& w) { return w.val; };
#else // ^^^ workaround / no workaround vvv
constexpr auto get_int_wrapper_val = &int_wrapper::val;
#endif // ^^^ no workaround ^^^

struct instantiator {
    static constexpr int expected_output[]      = {13, 55, 12345};
    static constexpr int expected_output_long[] = {13, 55, 12345, -1};
    static constexpr int expected_input[]       = {-1, -1, -1};
    static constexpr int expected_input_long[]  = {-1, -1, -1, 42};

    template <ranges::input_range R, ranges::forward_range W>
    static void call() {
        using ranges::uninitialized_move, ranges::uninitialized_move_result, ranges::destroy, ranges::equal,
            ranges::equal_to, ranges::iterator_t;

        { // Validate range overload
            int_wrapper input[3] = {13, 55, 12345};
            R wrapped_input{input};
            holder<int_wrapper, 3> mem;
            W wrapped_output{mem.as_span()};

            int_wrapper::clear_counts();
            const same_as<uninitialized_move_result<iterator_t<R>, iterator_t<W>>> auto result =
                uninitialized_move(wrapped_input, wrapped_output);
            assert(int_wrapper::constructions == 3);
            assert(int_wrapper::destructions == 0);
            assert(result.in == wrapped_input.end());
            assert(result.out == wrapped_output.end());
            assert(equal(wrapped_output, expected_output, equal_to{}, get_int_wrapper_val));
            assert(equal(input, expected_input, equal_to{}, get_int_wrapper_val));
            destroy(wrapped_output);
            assert(int_wrapper::constructions == 3);
            assert(int_wrapper::destructions == 3);
        }

        { // Validate iterator overload
            int_wrapper input[3] = {13, 55, 12345};
            R wrapped_input{input};
            holder<int_wrapper, 3> mem;
            W wrapped_output{mem.as_span()};

            int_wrapper::clear_counts();
            const same_as<uninitialized_move_result<iterator_t<R>, iterator_t<W>>> auto result = uninitialized_move(
                wrapped_input.begin(), wrapped_input.end(), wrapped_output.begin(), wrapped_output.end());
            assert(int_wrapper::constructions == 3);
            assert(int_wrapper::destructions == 0);
            assert(result.in == wrapped_input.end());
            assert(result.out == wrapped_output.end());
            assert(equal(wrapped_output, expected_output, equal_to{}, get_int_wrapper_val));
            assert(equal(input, expected_input, equal_to{}, get_int_wrapper_val));
            destroy(wrapped_output);
            assert(int_wrapper::constructions == 3);
            assert(int_wrapper::destructions == 3);
        }

        { // Validate range overload shorter output
            int_wrapper input[4] = {13, 55, 12345, 42};
            R wrapped_input{input};
            holder<int_wrapper, 3> mem;
            W wrapped_output{mem.as_span()};

            int_wrapper::clear_counts();
            same_as<uninitialized_move_result<iterator_t<R>, iterator_t<W>>> auto result =
                uninitialized_move(wrapped_input, wrapped_output);
            assert(int_wrapper::constructions == 3);
            assert(int_wrapper::destructions == 0);
            assert(++result.in == wrapped_input.end());
            assert(result.out == wrapped_output.end());
            assert(equal(wrapped_output, expected_output, equal_to{}, get_int_wrapper_val));
            assert(equal(input, expected_input_long, equal_to{}, get_int_wrapper_val));
            destroy(wrapped_output);
            assert(int_wrapper::constructions == 3);
            assert(int_wrapper::destructions == 3);
        }

        { // Validate range overload shorter input
            int_wrapper input[3] = {13, 55, 12345};
            R wrapped_input{input};
            holder<int_wrapper, 4> mem;
            W wrapped_output{mem.as_span()};

            int_wrapper::clear_counts();
            same_as<uninitialized_move_result<iterator_t<R>, iterator_t<W>>> auto result =
                uninitialized_move(wrapped_input, wrapped_output);
            assert(int_wrapper::constructions == 3);
            assert(int_wrapper::destructions == 0);
            assert(result.in == wrapped_input.end());
            construct_at(addressof(*result.out), -1); // Need to construct non written element for comparison
            assert(++result.out == wrapped_output.end());
            assert(equal(wrapped_output, expected_output_long, equal_to{}, get_int_wrapper_val));
            assert(equal(input, expected_input, equal_to{}, get_int_wrapper_val));
            destroy(wrapped_output);
            assert(int_wrapper::constructions == 4);
            assert(int_wrapper::destructions == 4);
        }
    }
};

struct throwing_test {
    static constexpr int expected_input[] = {-1, -1, int_wrapper::magic_throwing_val, 12345};

    template <ranges::input_range R, ranges::forward_range W>
    static void call() {
        // Validate only range overload (one is plenty since they both use the same backend)
        int_wrapper input[] = {13, 55, int_wrapper::magic_throwing_val, 12345};
        R wrapped_input{input};
        holder<int_wrapper, 4> mem;
        W wrapped_output{mem.as_span()};

        int_wrapper::clear_counts();
        try {
            (void) ranges::uninitialized_move(wrapped_input, wrapped_output);
            assert(false);
        } catch (int i) {
            assert(i == int_wrapper::magic_throwing_val);
        } catch (...) {
            assert(false);
        }
        assert(int_wrapper::constructions == 2);
        assert(int_wrapper::destructions == 2);
        assert(ranges::equal(input, expected_input, ranges::equal_to{}, get_int_wrapper_val));
    }
};

struct memcpy_test {
    static constexpr int expected_output[]      = {13, 55, 12345};
    static constexpr int expected_output_long[] = {13, 55, 12345, -1};
    static constexpr int expected_input[]       = {13, 55, 12345};
    static constexpr int expected_input_long[]  = {13, 55, 12345, 42};

    static void call() {
        { // Validate matching ranges
            int input[]  = {13, 55, 12345};
            int output[] = {-1, -1, -1};

            const auto result = ranges::uninitialized_move(input, output);
            assert(result.in == end(input));
            assert(result.out == end(output));
            assert(ranges::equal(input, expected_input));
            assert(ranges::equal(output, expected_output));
        }

        { // Validate input shorter
            int input[]  = {13, 55, 12345};
            int output[] = {-1, -1, -1, -1};

            auto result = ranges::uninitialized_move(input, output);
            assert(result.in == end(input));
            assert(++result.out == end(output));
            assert(ranges::equal(input, expected_input));
            assert(ranges::equal(output, expected_output_long));
        }

        { // Validate output shorter
            int input[]  = {13, 55, 12345, 42};
            int output[] = {-1, -1, -1};

            auto result = ranges::uninitialized_move(input, output);
            assert(++result.in == end(input));
            assert(result.out == end(output));
            assert(ranges::equal(input, expected_input_long));
            assert(ranges::equal(output, expected_output));
        }

        { // Validate non-common input range
            int input[]  = {13, 55, 12345};
            int output[] = {-1, -1, -1};

            const auto result =
                ranges::uninitialized_move(begin(input), unreachable_sentinel, begin(output), end(output));
            assert(result.in == end(input));
            assert(result.out == end(output));
            assert(ranges::equal(input, expected_input));
            assert(ranges::equal(output, expected_output));
        }

        { // Validate non-common output range
            int input[]  = {13, 55, 12345};
            int output[] = {-1, -1, -1};

            const auto result =
                ranges::uninitialized_move(begin(input), end(input), begin(output), unreachable_sentinel);
            assert(result.in == end(input));
            assert(result.out == end(output));
            assert(ranges::equal(input, expected_input));
            assert(ranges::equal(output, expected_output));
        }
    }
};

template <test::ProxyRef IsProxy>
using test_input  = test::range<test::input, int_wrapper, test::Sized::no, test::CanDifference::no, test::Common::no,
    test::CanCompare::no, IsProxy>;
using test_output = test::range<test::fwd, int_wrapper, test::Sized::no, test::CanDifference::no, test::Common::no,
    test::CanCompare::yes, test::ProxyRef::no>;

int main() {
    // The algorithm is oblivious to non-required category, size, difference, and "proxyness" of the input range. It
    // _is_ sensitive to proxyness in that it requires non-proxy references for the output range.

    instantiator::call<test_input<test::ProxyRef::no>, test_output>();
    instantiator::call<test_input<test::ProxyRef::yes>, test_output>();
    throwing_test::call<test_input<test::ProxyRef::no>, test_output>();
    throwing_test::call<test_input<test::ProxyRef::yes>, test_output>();
    memcpy_test::call();
}
