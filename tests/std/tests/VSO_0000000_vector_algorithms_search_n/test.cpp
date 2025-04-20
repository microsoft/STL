// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <limits>
#include <random>
#include <type_traits>
#include <vector>

#include "test_vector_algorithms_support.hpp"

using namespace std;

template <class FwdIt, class T>
auto last_known_good_search_n(FwdIt first, const FwdIt last, const size_t count, const T val) {
    // Deliberately using simple approach, not smart bidi/random iterators "check from the other end" stuff
    if (count == 0) {
        return first;
    }

    for (; first != last; ++first) {
        if (*first == val) {
            FwdIt match       = first;
            size_t match_size = count;
            for (;;) {
                --match_size;
                if (match_size == 0) {
                    return match;
                }

                ++first;

                if (first == last) {
                    return last;
                }

                if (*first != val) {
                    break;
                }
            }
        }
    }
    return last;
}

template <class Container, class T>
void test_case_search_n(const Container& c, size_t count, T val) {
    auto expected = last_known_good_search_n(c.begin(), c.end(), count, val);
    auto actual   = search_n(c.begin(), c.end(), count, val);
    assert(expected == actual);

#if _HAS_CXX20
    auto ranges_actual = ranges::search_n(c, static_cast<ptrdiff_t>(count), val);
    assert(expected == begin(ranges_actual));
    if (expected == c.end()) {
        assert(end(ranges_actual) == c.end());
    } else {
        assert(distance(expected, end(ranges_actual)) == static_cast<ptrdiff_t>(count));
    }
#endif // _HAS_CXX20
}

template <class T>
void test_search_n(mt19937_64& gen) {
    constexpr size_t lengthCount  = 70;
    constexpr size_t patternCount = 5;
    using TD                      = conditional_t<sizeof(T) == 1, int, T>;
    uniform_int_distribution<TD> dis((numeric_limits<T>::min)(), (numeric_limits<T>::max)());
    vector<T> input_src;
    vector<T> input;
    input_src.reserve(dataCount);
    input.reserve(dataCount);

    for (;;) {
        for (size_t count = 0; count != lengthCount; ++count) {
            input = input_src;

            const T val = static_cast<T>(dis(gen));

            test_case_search_n(input, count, val);

            binomial_distribution<size_t> pattern_length_dis(count * 2, 0.5);
            uniform_int_distribution<size_t> pos_dis(0, input.size() - 1);

            for (size_t pattern = 0; pattern != patternCount; ++pattern) {
                const size_t pattern_length = pattern_length_dis(gen);
                const size_t pattern_pos    = pos_dis(gen);

                if (pattern_length + pattern_pos < input.size()) {
                    fill_n(input.begin() + static_cast<ptrdiff_t>(pattern_pos), pattern_length, val);

                    test_case_search_n(input, count, val);
                }
            }
        }

        if (input.size() == dataCount) {
            break;
        }

        input_src.push_back(static_cast<T>(dis(gen)));
    }
}

void test_vector_algorithms(mt19937_64& gen) {
    test_search_n<char>(gen);
    test_search_n<signed char>(gen);
    test_search_n<unsigned char>(gen);
    test_search_n<short>(gen);
    test_search_n<unsigned short>(gen);
    test_search_n<int>(gen);
    test_search_n<unsigned int>(gen);
    test_search_n<long long>(gen);
    test_search_n<unsigned long long>(gen);
}

int main() {
    run_randomized_tests_with_different_isa_levels(test_vector_algorithms);
}
