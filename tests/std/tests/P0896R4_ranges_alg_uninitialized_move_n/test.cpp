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
#include <vector>

#include <range_algorithm_support.hpp>

using namespace std;

// Validate that uninitialized_move_n_result aliases in_out_result
STATIC_ASSERT(same_as<ranges::uninitialized_move_n_result<int, double>, ranges::in_out_result<int, double>>);

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

template <class T, size_t N>
struct holder {
    STATIC_ASSERT(N < ~size_t{0} / sizeof(T));
    alignas(T) unsigned char space[N * sizeof(T)];

    auto as_span() {
        return span<T, N>{reinterpret_cast<T*>(space + 0), N};
    }
};

struct instantiator {
    static constexpr int expected_output[]      = {13, 55, 12345};
    static constexpr int expected_output_long[] = {13, 55, 12345, -1};
    static constexpr int expected_input[]       = {-1, -1, -1};
    static constexpr int expected_input_long[]  = {-1, -1, -1, 42};

    template <ranges::input_range Read, ranges::forward_range Write>
    static void call() {
        using ranges::destroy, ranges::uninitialized_move_n, ranges::uninitialized_move_n_result, ranges::equal,
            ranges::equal_to, ranges::iterator_t;
        { // Validate matching ranges
            int_wrapper input[3] = {13, 55, 12345};
            Read wrapped_input{input};
            holder<int_wrapper, 3> mem;
            Write wrapped_output{mem.as_span()};

            int_wrapper::clear_counts();
            const same_as<uninitialized_move_n_result<iterator_t<Read>, iterator_t<Write>>> auto result =
                uninitialized_move_n(wrapped_input.begin(), 3, wrapped_output.begin(), wrapped_output.end());
            assert(int_wrapper::constructions == 3);
            assert(int_wrapper::destructions == 0);
            assert(result.in == wrapped_input.end());
            assert(result.out == wrapped_output.end());
            assert(equal(wrapped_output, expected_output, equal_to{}, &int_wrapper::val));
            assert(equal(input, expected_input, equal_to{}, &int_wrapper::val));
            destroy(wrapped_output);
            assert(int_wrapper::constructions == 3);
            assert(int_wrapper::destructions == 3);
        }

        { // Validate shorter output
            int_wrapper input[4] = {13, 55, 12345, 42};
            Read wrapped_input{input};
            holder<int_wrapper, 3> mem;
            Write wrapped_output{mem.as_span()};

            int_wrapper::clear_counts();
            same_as<uninitialized_move_n_result<iterator_t<Read>, iterator_t<Write>>> auto result =
                uninitialized_move_n(wrapped_input.begin(), 3, wrapped_output.begin(), wrapped_output.end());
            assert(int_wrapper::constructions == 3);
            assert(int_wrapper::destructions == 0);
            assert(++result.in == wrapped_input.end());
            assert(result.out == wrapped_output.end());
            assert(equal(wrapped_output, expected_output, equal_to{}, &int_wrapper::val));
            assert(equal(input, expected_input_long, equal_to{}, &int_wrapper::val));
            destroy(wrapped_output);
            assert(int_wrapper::constructions == 3);
            assert(int_wrapper::destructions == 3);
        }

        { // Validate shorter input
            int_wrapper input[3] = {13, 55, 12345};
            Read wrapped_input{input};
            holder<int_wrapper, 4> mem;
            Write wrapped_output{mem.as_span()};

            int_wrapper::clear_counts();
            same_as<uninitialized_move_n_result<iterator_t<Read>, iterator_t<Write>>> auto result =
                uninitialized_move_n(wrapped_input.begin(), 3, wrapped_output.begin(), wrapped_output.end());
            assert(int_wrapper::constructions == 3);
            assert(int_wrapper::destructions == 0);
            assert(result.in == wrapped_input.end());
            construct_at(addressof(*result.out), -1); // Need to construct non written element for comparison
            assert(++result.out == wrapped_output.end());
            assert(equal(wrapped_output, expected_output_long, equal_to{}, &int_wrapper::val));
            assert(equal(input, expected_input, equal_to{}, &int_wrapper::val));
            destroy(wrapped_output);
            assert(int_wrapper::constructions == 4);
            assert(int_wrapper::destructions == 4);
        }
    }
};

struct throwing_test {
    static constexpr int expected_input[] = {-1, -1, int_wrapper::magic_throwing_val, 12345};

    template <ranges::input_range Read, ranges::forward_range Write>
    static void call() {
        int_wrapper input[] = {13, 55, int_wrapper::magic_throwing_val, 12345};
        Read wrapped_input{input};
        holder<int_wrapper, 4> mem;
        Write wrapped_output{mem.as_span()};

        int_wrapper::clear_counts();
        try {
            (void) ranges::uninitialized_move_n(wrapped_input.begin(), 4, wrapped_output.begin(), wrapped_output.end());
            assert(false);
        } catch (int i) {
            assert(i == int_wrapper::magic_throwing_val);
        } catch (...) {
            assert(false);
        }
        assert(int_wrapper::constructions == 2);
        assert(int_wrapper::destructions == 2);
        assert(ranges::equal(input, expected_input, ranges::equal_to{}, &int_wrapper::val));
    }
};

struct memcpy_test {
    static constexpr int expected_output[]      = {13, 55, 12345, -1};
    static constexpr int expected_output_long[] = {13, 55, -1, -1};
    static constexpr int expected_input[]       = {13, 55, 12345, 42};
    static constexpr int expected_input_short[] = {13, 55};
    static constexpr int expected_input_long[]  = {13, 55, 12345, 42};

    static void call() {
        using ranges::uninitialized_move_n, ranges::uninitialized_move_n_result, ranges::equal, ranges::iterator_t;
        { // Validate range overload
            vector<int> input  = {13, 55, 12345, 42};
            vector<int> output = {-1, -1, -1, -1};

            const same_as<uninitialized_move_n_result<iterator_t<vector<int>>, iterator_t<vector<int>>>> auto result =
                uninitialized_move_n(input.begin(), 3, output.begin(), output.end());
            assert(next(result.in) == input.end());
            assert(next(result.out) == output.end());
            assert(equal(input, expected_input));
            assert(equal(output, expected_output));
        }

        { // Validate shorter input
            vector<int> input  = {13, 55};
            vector<int> output = {-1, -1, -1, -1};

            const same_as<uninitialized_move_n_result<iterator_t<vector<int>>, iterator_t<vector<int>>>> auto result =
                uninitialized_move_n(input.begin(), 2, output.begin(), output.end());
            assert(result.in == input.end());
            assert(next(result.out, 2) == output.end());
            assert(equal(input, expected_input_short));
            assert(equal(output, expected_output_long));
        }

        { // Validate shorter output
            vector<int> input  = {13, 55, 12345, 42};
            vector<int> output = {-1, -1};

            const same_as<uninitialized_move_n_result<iterator_t<vector<int>>, iterator_t<vector<int>>>> auto result =
                uninitialized_move_n(input.begin(), 2, output.begin(), output.end());
            assert(next(result.in, 2) == input.end());
            assert(result.out == output.end());
            assert(equal(input, expected_input));
            assert(equal(output, expected_input_short));
        }

        { // Validate unreachable_sentinel
            vector<int> input  = {13, 55, 12345, 42};
            vector<int> output = {-1, -1, -1, -1};

            const same_as<uninitialized_move_n_result<iterator_t<vector<int>>, iterator_t<vector<int>>>> auto result =
                uninitialized_move_n(input.begin(), 3, output.begin(), unreachable_sentinel);
            assert(next(result.in) == input.end());
            assert(next(result.out) == output.end());
            assert(equal(input, expected_input));
            assert(equal(output, expected_output));
        }

        { // Validate non-common range
            vector<int> input  = {13, 55, 12345, 42};
            vector<int> output = {-1, -1, -1, -1};

            auto wrapped_output = output | views::take_while([](const auto&) { return true; });
            const same_as<uninitialized_move_n_result<iterator_t<vector<int>>, iterator_t<vector<int>>>> auto result =
                uninitialized_move_n(input.begin(), 3, wrapped_output.begin(), wrapped_output.end());
            assert(next(result.in) == input.end());
            assert(next(result.out) == output.end());
            assert(equal(input, expected_input));
            assert(equal(output, expected_output));
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
