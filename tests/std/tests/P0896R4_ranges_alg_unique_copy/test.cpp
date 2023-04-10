// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <ranges>
#include <sstream>
#include <utility>

#include <range_algorithm_support.hpp>
using namespace std;
using P = pair<int, int>;

// Validate that unique_copy_result aliases in_out_result
STATIC_ASSERT(same_as<ranges::unique_copy_result<int, double>, ranges::in_out_result<int, double>>);

// Validate dangling story
STATIC_ASSERT(same_as<decltype(ranges::unique_copy(borrowed<false>{}, nullptr_to<int>)),
    ranges::unique_copy_result<ranges::dangling, int*>>);
STATIC_ASSERT(
    same_as<decltype(ranges::unique_copy(borrowed<true>{}, nullptr_to<int>)), ranges::unique_copy_result<int*, int*>>);

constexpr P expectedInputRead[6]  = {{0, 99}, {1, 47}, {1, 47}, {1, 99}, {1, 47}, {1, 47}};
constexpr P expectedOutputRead[4] = {{1, 99}, {3, 47}, {2, 99}, {2, 47}};
constexpr P expectedInput[6]      = {{1, 99}, {3, 47}, {1, 47}, {2, 99}, {2, 47}, {1, 47}};
constexpr P expectedOutput[4]     = {{0, 99}, {1, 47}, {1, 99}, {1, 47}};
constexpr auto countedProjection  = [](P& value) {
    ++value.first;
    return value.second;
};

struct test_iterator_overload {
    template <ranges::input_range Read, weakly_incrementable Write>
    static constexpr void call() {
        using ranges::unique_copy, ranges::unique_copy_result, ranges::equal, ranges::equal_to, ranges::iterator_t,
            ranges::size;
        { // Validate iterator + sentinel overload
            P input[6]  = {{0, 99}, {0, 47}, {0, 47}, {0, 99}, {0, 47}, {0, 47}};
            P output[4] = {{-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}};
            Read wrapped_input{input};

            const same_as<unique_copy_result<iterator_t<Read>, Write>> auto result =
                unique_copy(wrapped_input.begin(), wrapped_input.end(), Write{output}, equal_to{}, countedProjection);
            assert(result.in == wrapped_input.end());
            assert(result.out.peek() == end(output));
            if constexpr (input_iterator<Write> || !ranges::forward_range<Read>) {
                assert(equal(output, expectedOutputRead));
                assert(equal(input, expectedInputRead));
            } else {
                assert(equal(output, expectedOutput));
                assert(equal(input, expectedInput));
            }
        }

        { // Validate already unique range
            P input[4]  = {{0, 99}, {0, 47}, {0, 99}, {0, 47}};
            P output[4] = {{-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}};
            Read wrapped_input{input};

            const same_as<unique_copy_result<iterator_t<Read>, Write>> auto result =
                unique_copy(wrapped_input.begin(), wrapped_input.end(), Write{output}, equal_to{});
            assert(result.in == wrapped_input.end());
            assert(result.out.peek() == end(output));
            assert(equal(output, input));
        }
    }
};

struct test_range_overload {
    template <ranges::input_range Read, weakly_incrementable Write>
    static constexpr void call() {
        using ranges::unique_copy, ranges::unique_copy_result, ranges::equal, ranges::equal_to, ranges::iterator_t,
            ranges::size;
        { // Validate range overload
            P input[6]  = {{0, 99}, {0, 47}, {0, 47}, {0, 99}, {0, 47}, {0, 47}};
            P output[4] = {{-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}};
            Read wrapped_input{input};

            const same_as<unique_copy_result<iterator_t<Read>, Write>> auto result =
                unique_copy(wrapped_input, Write{output}, equal_to{}, countedProjection);
            assert(result.in == wrapped_input.end());
            assert(result.out.peek() == end(output));
            if constexpr (input_iterator<Write> || !ranges::forward_range<Read>) {
                assert(equal(output, expectedOutputRead));
                assert(equal(input, expectedInputRead));
            } else {
                assert(equal(output, expectedOutput));
                assert(equal(input, expectedInput));
            }
        }

        { // Validate already unique range
            P input[4]  = {{0, 99}, {0, 47}, {0, 99}, {0, 47}};
            P output[4] = {{-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}};
            Read wrapped_input{input};

            const same_as<unique_copy_result<iterator_t<Read>, Write>> auto result =
                unique_copy(wrapped_input, Write{output}, equal_to{});
            assert(result.in == wrapped_input.end());
            assert(result.out.peek() == end(output));
            assert(equal(output, input));
        }
    }
};

#ifdef TEST_EVERYTHING
int main() {
#ifndef _PREFAST_ // TRANSITION, GH-1030
    STATIC_ASSERT((test_in_write<test_iterator_overload, P, P>(), true));
    STATIC_ASSERT((test_in_write<test_range_overload, P, P>(), true));
#endif // TRANSITION, GH-1030
    test_in_write<test_iterator_overload, P, P>();
    test_in_write<test_range_overload, P, P>();
}
#else // ^^^ test all range combinations / test only interesting range combos vvv
constexpr bool run_tests() {
    // We need to test the three different implementations, so we need input_range/forward_range as input and
    // output_iterator/input_iterator as output.
    using namespace test;
    using test::iterator, test::range;

    using in_test_range =
        range<input_iterator_tag, P, Sized::no, CanDifference::no, Common::no, CanCompare::no, ProxyRef::yes>;
    using fwd_test_range =
        range<input_iterator_tag, P, Sized::no, CanDifference::no, Common::no, CanCompare::no, ProxyRef::yes>;

    using out_test_iterator = iterator<output_iterator_tag, P, CanDifference::no, CanCompare::no, ProxyRef::yes>;
    using in_test_iterator  = iterator<input_iterator_tag, P, CanDifference::no, CanCompare::no, ProxyRef::yes>;

    // Reread output implementation
    test_iterator_overload::call<in_test_range, in_test_iterator>();
    test_range_overload::call<in_test_range, in_test_iterator>();

    // Reread input implementation
    test_iterator_overload::call<fwd_test_range, out_test_iterator>();
    test_range_overload::call<fwd_test_range, out_test_iterator>();

    // Store implementation
    test_iterator_overload::call<in_test_range, out_test_iterator>();
    test_range_overload::call<in_test_range, out_test_iterator>();

    return true;
}

void test_gh_1932() {
    // Defend against regression of GH-1932, in which ranges::unique_copy instantiated
    // iter_value_t<I> for a non-input iterator I.

    istringstream str("42 42 42");
    ostringstream result;
    ranges::unique_copy(istream_iterator<int>{str}, istream_iterator<int>{}, ostream_iterator<int>{result, " "});
    assert(result.str() == "42 ");
}

int main() {
    STATIC_ASSERT(run_tests());
    run_tests();

    test_gh_1932();
}
#endif // TEST_EVERYTHING
