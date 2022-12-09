// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <ranges>
#include <utility>

#include <range_algorithm_support.hpp>

using namespace std;
using P = pair<int, int>;

// Validate that binary_transform_result aliases in_in_out_result
STATIC_ASSERT(same_as<ranges::binary_transform_result<int, P, double>, ranges::in_in_out_result<int, P, double>>);

// Validate dangling story
STATIC_ASSERT(same_as<decltype(ranges::transform(borrowed<false>{}, borrowed<false>{}, nullptr_to<int>, plus{})),
    ranges::binary_transform_result<ranges::dangling, ranges::dangling, int*>>);
STATIC_ASSERT(same_as<decltype(ranges::transform(borrowed<false>{}, borrowed<true>{}, nullptr_to<int>, plus{})),
    ranges::binary_transform_result<ranges::dangling, int*, int*>>);
STATIC_ASSERT(same_as<decltype(ranges::transform(borrowed<true>{}, borrowed<false>{}, nullptr_to<int>, plus{})),
    ranges::binary_transform_result<int*, ranges::dangling, int*>>);
STATIC_ASSERT(same_as<decltype(ranges::transform(borrowed<true>{}, borrowed<true>{}, nullptr_to<int>, plus{})),
    ranges::binary_transform_result<int*, int*, int*>>);

struct instantiator {
    static constexpr P input1[3]     = {{1, 99}, {4, 98}, {5, 97}};
    static constexpr P input2[3]     = {{99, 6}, {98, 7}, {97, 8}};
    static constexpr int expected[3] = {7, 11, 13};

    static constexpr P shortInput1[2]     = {{1, 99}, {4, 98}};
    static constexpr P shortInput2[2]     = {{99, 6}, {98, 7}};
    static constexpr int shortExpected[2] = {7, 11};

    template <ranges::input_range Read1, ranges::input_range Read2, weakly_incrementable Write>
    static constexpr void call() {
        using ranges::transform, ranges::binary_transform_result, ranges::iterator_t;
        { // Validate iterator + sentinel overload, first range shorter
            int output[2] = {-1, -1};
            Read1 wrapped_in1{shortInput1};
            Read2 wrapped_in2{input2};

            auto result = transform(wrapped_in1.begin(), wrapped_in1.end(), wrapped_in2.begin(), wrapped_in2.end(),
                Write{output}, plus{}, get_first, get_second);
            STATIC_ASSERT(
                same_as<decltype(result), binary_transform_result<iterator_t<Read1>, iterator_t<Read2>, Write>>);
            assert(result.in1 == wrapped_in1.end());
            assert(next(result.in2) == wrapped_in2.end());
            assert(result.out.peek() == output + 2);
            assert(ranges::equal(output, shortExpected));
        }
        { // Validate iterator + sentinel overload, second range shorter
            int output[2] = {-1, -1};
            Read1 wrapped_in1{input1};
            Read2 wrapped_in2{shortInput2};

            auto result = transform(wrapped_in1.begin(), wrapped_in1.end(), wrapped_in2.begin(), wrapped_in2.end(),
                Write{output}, plus{}, get_first, get_second);
            STATIC_ASSERT(
                same_as<decltype(result), binary_transform_result<iterator_t<Read1>, iterator_t<Read2>, Write>>);
            assert(next(result.in1) == wrapped_in1.end());
            assert(result.in2 == wrapped_in2.end());
            assert(result.out.peek() == output + 2);
            assert(ranges::equal(output, shortExpected));
        }
        { // Validate range overload, first range shorter
            int output[2] = {-1, -1};
            Read1 wrapped_in1{shortInput1};
            Read2 wrapped_in2{input2};

            auto result = transform(wrapped_in1, wrapped_in2, Write{output}, plus{}, get_first, get_second);
            STATIC_ASSERT(
                same_as<decltype(result), binary_transform_result<iterator_t<Read1>, iterator_t<Read2>, Write>>);
            assert(result.in1 == wrapped_in1.end());
            assert(next(result.in2) == wrapped_in2.end());
            assert(result.out.peek() == output + 2);
            assert(ranges::equal(output, shortExpected));
        }
        { // Validate range overload, second range shorter
            int output[2] = {-1, -1};
            Read1 wrapped_in1{input1};
            Read2 wrapped_in2{shortInput2};

            auto result = transform(wrapped_in1, wrapped_in2, Write{output}, plus{}, get_first, get_second);
            STATIC_ASSERT(
                same_as<decltype(result), binary_transform_result<iterator_t<Read1>, iterator_t<Read2>, Write>>);
            assert(next(result.in1) == wrapped_in1.end());
            assert(result.in2 == wrapped_in2.end());
            assert(result.out.peek() == output + 2);
            assert(ranges::equal(output, shortExpected));
        }
    }
};

using Elem1 = const P;
using Elem2 = const P;
using Elem3 = int;

#ifdef TEST_EVERYTHING
int main() {
    // No constexpr test here; the test_in_in_write call exceeds the maximum number of steps in a constexpr computation.
    test_in_in_write<instantiator, Elem1, Elem2, Elem3>();
}
#else // ^^^ test all range combinations / test only interesting range combos vvv
template <class Elem, test::Sized IsSized>
using in_test_range = test::range<input_iterator_tag, Elem, IsSized, test::CanDifference::no, test::Common::no,
    test::CanCompare::yes, test::ProxyRef::yes>;
template <class Elem>
using out_test_iterator =
    test::iterator<output_iterator_tag, Elem, test::CanDifference::no, test::CanCompare::yes, test::ProxyRef::yes>;

constexpr bool run_tests() {
    // All (except contiguous) proxy reference types, since the algorithm doesn't really care.
    using test::Common, test::Sized;

    // both input, non-common, and sized or unsized
    instantiator::call<in_test_range<Elem1, Sized::no>, in_test_range<Elem2, Sized::no>, out_test_iterator<Elem3>>();
    instantiator::call<in_test_range<Elem1, Sized::yes>, in_test_range<Elem2, Sized::yes>, out_test_iterator<Elem3>>();
    return true;
}

int main() {
    STATIC_ASSERT(run_tests());
    run_tests();
}
#endif // TEST_EVERYTHING
