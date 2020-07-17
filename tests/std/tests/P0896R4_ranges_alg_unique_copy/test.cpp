// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <ranges>
#include <span>
#include <utility>

#include <range_algorithm_support.hpp>
using namespace std;

struct instrumentedPair {
    pair<int, int> _val = {0, 0};
    int _numProjections = 0;

    constexpr instrumentedPair() = default;
    constexpr instrumentedPair(const instrumentedPair& other) : _val{other._val} {}
    constexpr instrumentedPair(instrumentedPair&& other) : _val{exchange(other._val, {-1, -1})} {}
    constexpr instrumentedPair& operator=(const instrumentedPair& other) {
        _val            = other._val;
        _numProjections = 0;
        return *this;
    }
    constexpr instrumentedPair& operator=(instrumentedPair&& other) {
        _val            = exchange(other._val, {-1, -1});
        _numProjections = 0;
        return *this;
    }

    constexpr instrumentedPair(const int a, const int b) : _val{a, b} {};
    constexpr instrumentedPair(const int a, const int b, const int c) : _val{a, b}, _numProjections{c} {};

    constexpr bool operator==(const instrumentedPair&) const = default;
};

constexpr auto countedProjection = [](instrumentedPair& value) {
    ++value._numProjections;
    return value._val.second;
};

// Validate that unique_copy_result aliases in_out_result
STATIC_ASSERT(same_as<ranges::unique_copy_result<int, double>, ranges::in_out_result<int, double>>);

// Validate dangling story
STATIC_ASSERT(same_as<decltype(ranges::unique_copy(borrowed<false>{}, nullptr_to<int>)),
    ranges::unique_copy_result<ranges::dangling, int*>>);
STATIC_ASSERT(
    same_as<decltype(ranges::unique_copy(borrowed<true>{}, nullptr_to<int>)), ranges::unique_copy_result<int*, int*>>);

struct instantiator {
    static constexpr instrumentedPair expectedOutput[4] = {
        instrumentedPair{0, 99, 0}, //
        instrumentedPair{1, 47, 0}, //
        instrumentedPair{3, 99, 0}, //
        instrumentedPair{4, 47, 0} //
    };
    static constexpr instrumentedPair expectedOutputRead[4] = {
        instrumentedPair{0, 99, 1}, //
        instrumentedPair{1, 47, 2}, //
        instrumentedPair{3, 99, 1}, //
        instrumentedPair{4, 47, 1} //
    };
    static constexpr instrumentedPair expectedInput[6] = {
        instrumentedPair{0, 99, 0}, //
        instrumentedPair{1, 47, 1}, //
        instrumentedPair{2, 47, 1}, //
        instrumentedPair{3, 99, 1}, //
        instrumentedPair{4, 47, 1}, //
        instrumentedPair{5, 47, 1} //
    };
    static constexpr instrumentedPair expectedInputRead[6] = {
        instrumentedPair{0, 99, 1}, //
        instrumentedPair{1, 47, 3}, //
        instrumentedPair{2, 47, 1}, //
        instrumentedPair{3, 99, 2}, //
        instrumentedPair{4, 47, 2}, //
        instrumentedPair{5, 47, 1} //
    };

    template <ranges::input_range Read, weakly_incrementable Write>
    static constexpr void call() {
        using ranges::unique_copy, ranges::unique_copy_result, ranges::equal, ranges::equal_to, ranges::size,
            ranges::iterator_t;
#if !defined(__clang__) && !defined(__EDG__) // TRANSITION, VSO-938163
        if constexpr (!ranges::contiguous_range<Read>)
#endif // TRANSITION, VSO-938163
        {
            { // Validate iterator + sentinel overload
                instrumentedPair input[6] = {
                    instrumentedPair{0, 99}, //
                    instrumentedPair{1, 47}, //
                    instrumentedPair{2, 47}, //
                    instrumentedPair{3, 99}, //
                    instrumentedPair{4, 47}, //
                    instrumentedPair{5, 47} //
                };
                instrumentedPair output[4] = {
                    instrumentedPair{-1, -1}, //
                    instrumentedPair{-1, -1}, //
                    instrumentedPair{-1, -1}, //
                    instrumentedPair{-1, -1} //
                };
                Read wrapped_input{input};

                auto result = unique_copy(
                    wrapped_input.begin(), wrapped_input.end(), Write{output}, equal_to{}, countedProjection);
                STATIC_ASSERT(same_as<decltype(result), unique_copy_result<iterator_t<Read>, Write>>);
                assert(result.in == wrapped_input.end());
                assert(result.out.peek() == end(output));
                if constexpr (input_iterator<Write>) {
                    assert(equal(expectedOutputRead, output));
                    assert(equal(expectedInput, input));
                } else if constexpr (ranges::forward_range<Read>) {
                    assert(equal(expectedOutput, output));
                    assert(equal(expectedInputRead, input));
                } else {
                    assert(equal(expectedOutput, output));
                    assert(equal(expectedInput, input));
                }
            }
            { // Validate range overload
                instrumentedPair input[6] = {
                    instrumentedPair{0, 99}, //
                    instrumentedPair{1, 47}, //
                    instrumentedPair{2, 47}, //
                    instrumentedPair{3, 99}, //
                    instrumentedPair{4, 47}, //
                    instrumentedPair{5, 47} //
                };
                instrumentedPair output[4] = {
                    instrumentedPair{-1, -1}, //
                    instrumentedPair{-1, -1}, //
                    instrumentedPair{-1, -1}, //
                    instrumentedPair{-1, -1} //
                };
                Read wrapped_input{input};

                auto result = unique_copy(wrapped_input, Write{output}, equal_to{}, countedProjection);
                STATIC_ASSERT(same_as<decltype(result), unique_copy_result<iterator_t<Read>, Write>>);
                assert(result.in == wrapped_input.end());
                assert(result.out.peek() == end(output));
                if constexpr (input_iterator<Write>) {
                    assert(equal(expectedOutputRead, output));
                    assert(equal(expectedInput, input));
                } else if constexpr (ranges::forward_range<Read>) {
                    assert(equal(expectedOutput, output));
                    assert(equal(expectedInputRead, input));
                } else {
                    assert(equal(expectedOutput, output));
                    assert(equal(expectedInput, input));
                }
            }
        }
    }
};

#ifdef TEST_EVERYTHING
int main() {
#ifndef _PREFAST_ // TRANSITION, GH-1030
    STATIC_ASSERT((test_in_write<instantiator, instrumentedPair, instrumentedPair>(), true));
#endif // TRANSITION, GH-1030
    test_in_write<instantiator, instrumentedPair, instrumentedPair>();
}
#else // ^^^ test all range combinations // test only interesting range combos vvv

// We need to test the three different implementations, so we need input_range/forward_range as input and
// output_iterator/input_iterator as output.
using in_test_range = test::range<input_iterator_tag, instrumentedPair, test::Sized::no, test::CanDifference::no,
    test::Common::no, test::CanCompare::no, test::ProxyRef::yes>;

using fwd_test_range = test::range<input_iterator_tag, instrumentedPair, test::Sized::no, test::CanDifference::no,
    test::Common::no, test::CanCompare::no, test::ProxyRef::yes>;

using out_test_iterator = test::iterator<output_iterator_tag, instrumentedPair, test::CanDifference::no,
    test::CanCompare::no, test::ProxyRef::yes>;

using in_test_iterator = test::iterator<input_iterator_tag, instrumentedPair, test::CanDifference::no,
    test::CanCompare::no, test::ProxyRef::yes>;

constexpr bool run_tests() {
    // Reread output implementation
    instantiator::call<in_test_range, in_test_iterator>();

    // Reread input implementation
    instantiator::call<fwd_test_range, out_test_iterator>();

    // Store implementation
    instantiator::call<in_test_range, out_test_iterator>();
    return true;
}

int main() {
    STATIC_ASSERT(run_tests());
    run_tests();
}
#endif // TEST_EVERYTHING
