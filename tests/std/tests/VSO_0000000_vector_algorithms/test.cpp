// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <deque>
#include <functional>
#include <limits>
#include <list>
#include <numeric>
#include <random>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#if _HAS_CXX20
#include <compare>
#include <ranges>
#endif // _HAS_CXX20

#include "test_is_sorted_until_support.hpp"
#include "test_min_max_element_support.hpp"
#include "test_vector_algorithms_support.hpp"

using namespace std;

#pragma warning(disable : 4984) // 'if constexpr' is a C++17 language extension
#ifdef __clang__
#pragma clang diagnostic ignored "-Wc++17-extensions" // constexpr if is a C++17 extension
#endif // __clang__

template <class InIt, class OutIt, class BinOp>
OutIt last_known_good_adj_diff(InIt first, InIt last, OutIt dest, BinOp binop) {
    if (first == last) {
        return dest;
    }

    auto val = *first;
    *dest    = val;

    for (++first, ++dest; first != last; ++first, ++dest) {
        auto tmp = *first;
        *dest    = binop(tmp, val);
        val      = tmp;
    }

    return dest;
}

template <class T>
void test_case_adj_diff(const vector<T>& input, vector<T>& output_expected, vector<T>& output_actual) {
    // Avoid truncation warnings:
    const auto subtract = [](const T& left, const T& right) { return static_cast<T>(left - right); };
    const auto expected = last_known_good_adj_diff(input.begin(), input.end(), output_expected.begin(), subtract);
    const auto actual   = adjacent_difference(input.begin(), input.end(), output_actual.begin(), subtract);

    assert(actual - output_actual.begin() == expected - output_expected.begin());
    assert(output_actual == output_expected);
}

template <class T>
void test_adjacent_difference(mt19937_64& gen) {
    using Limits = numeric_limits<T>;

    uniform_int_distribution<conditional_t<sizeof(T) == 1, int, T>> dis(
        is_signed_v<T> ? static_cast<T>(Limits::min() / 2) : Limits::min(),
        is_signed_v<T> ? static_cast<T>(Limits::max() / 2) : Limits::max());

    vector<T> input;
    vector<T> output_expected;
    vector<T> output_actual;

    for (const auto& v : {&input, &output_expected, &output_actual}) {
        v->reserve(dataCount);
    }

    test_case_adj_diff(input, output_expected, output_actual);
    for (size_t attempts = 0; attempts < dataCount; ++attempts) {
        input.push_back(static_cast<T>(dis(gen)));

        for (const auto& v : {&output_expected, &output_actual}) {
            v->assign(input.size(), 0);
        }

        test_case_adj_diff(input, output_expected, output_actual);
    }
}

void test_adjacent_difference_with_heterogeneous_types() {
    const vector<unsigned char> input = {10, 70, 20, 90};
    vector<int> output(4);

    const auto result = adjacent_difference(input.begin(), input.end(), output.begin());
    assert(result == output.end());

    const vector<int> expected = {10, 60, -50, 70};
    assert(output == expected);
}

template <class FwdIt>
FwdIt last_known_good_adj_find(FwdIt first, FwdIt last) {
    if (first == last) {
        return last;
    }

    auto next = first;
    for (++next; next != last; ++first, ++next) {
        if (*first == *next) {
            return first;
        }
    }

    return last;
}

template <class T>
void test_case_adj_find(const vector<T>& input) {
    const auto actual   = adjacent_find(input.begin(), input.end());
    const auto expected = last_known_good_adj_find(input.begin(), input.end());
    assert(actual == expected);

#if _HAS_CXX20
    const auto actual_r = ranges::adjacent_find(input);
    assert(actual_r == expected);
#endif // _HAS_CXX20
}

template <class T>
void test_adjacent_find(mt19937_64& gen) {
    constexpr size_t replicaCount = 4;

    using Limits = numeric_limits<T>;

    uniform_int_distribution<conditional_t<sizeof(T) == 1, int, T>> dis(Limits::min(), Limits::max());

    vector<T> original_input;
    vector<T> input;

    original_input.reserve(dataCount);
    input.reserve(dataCount);

    test_case_adj_find(input);
    for (size_t attempts = 0; attempts < dataCount; ++attempts) {
        original_input.push_back(static_cast<T>(dis(gen)));
        input = original_input;

        test_case_adj_find(input);

        if (original_input.size() > 2) {
            uniform_int_distribution<size_t> pos_dis(0, original_input.size() - 2);

            for (size_t replicas = 0; replicas < replicaCount; ++replicas) {
                const size_t replica_pos = pos_dis(gen);
                input[replica_pos]       = input[replica_pos + 1];
                test_case_adj_find(input);
            }
        }
    }
}

template <class FwdIt, class T>
ptrdiff_t last_known_good_count(FwdIt first, FwdIt last, T v) {
    ptrdiff_t result = 0;
    for (; first != last; ++first) {
        if (*first == v) {
            ++result;
        }
    }
    return result;
}


template <class T>
void test_case_count(const vector<T>& input, T v) {
    auto expected = last_known_good_count(input.begin(), input.end(), v);
    auto actual   = count(input.begin(), input.end(), v);
    assert(expected == actual);
#if _HAS_CXX20
    auto actual_r = ranges::count(input, v);
    assert(actual_r == expected);
#endif // _HAS_CXX20
}

template <class T>
void test_count_zero(const vector<T>& input, const ptrdiff_t n) {
    const auto first = input.begin();
    const auto last  = first + n;

    assert(count(first, last, T{0}) == n);
#if _HAS_CXX20
    assert(ranges::count(first, last, T{0}) == n);
#endif // _HAS_CXX20
}

template <class T>
void test_count(mt19937_64& gen) {
    using TD = conditional_t<sizeof(T) == 1, int, T>;
    binomial_distribution<TD> dis(10);
    vector<T> input;
    input.reserve(dataCount);
    test_case_count(input, static_cast<T>(dis(gen)));
    for (size_t attempts = 0; attempts < dataCount; ++attempts) {
        input.push_back(static_cast<T>(dis(gen)));
        test_case_count(input, static_cast<T>(dis(gen)));
    }

    {
        input.assign(1'000'000, T{0});

        // test that counters don't overflow
        test_count_zero(input, 1'000'000);

        // Test the AVX2 maximum portion followed by all possible tail lengths, for 1-byte and 2-byte elements.
        // It's okay to test these lengths for other elements, or other instruction sets.
        for (ptrdiff_t i = 8'160; i < 8'192; ++i) {
            test_count_zero(input, i);
        }

        for (ptrdiff_t i = 524'272; i < 524'288; ++i) {
            test_count_zero(input, i);
        }

        // Test a random length.
        uniform_int_distribution<ptrdiff_t> len(0, 999'999);
        test_count_zero(input, len(gen));
    }
}

template <class FwdIt, class T, class Pred = equal_to<>>
auto last_known_good_find(FwdIt first, FwdIt last, T v, Pred pred = {}) {
    for (; first != last; ++first) {
        if (pred(*first, v)) {
            break;
        }
    }
    return first;
}

template <class FwdIt, class T, class Pred = equal_to<>>
auto last_known_good_find_last(FwdIt first, FwdIt last, T v, Pred pred = {}) {
    FwdIt last_save = last;
    for (;;) {
        if (last == first) {
            return last_save;
        }
        --last;
        if (pred(*last, v)) {
            return last;
        }
    }
}

template <class FwdItH, class FwdItN>
auto last_known_good_find_first_of(FwdItH h_first, FwdItH h_last, FwdItN n_first, FwdItN n_last) {
    for (; h_first != h_last; ++h_first) {
        for (FwdItN n = n_first; n != n_last; ++n) {
            if (*h_first == *n) {
                return h_first;
            }
        }
    }
    return h_first;
}

template <class RanItH, class RanItN>
auto last_known_good_search(RanItH h_first, RanItH h_last, RanItN n_first, RanItN n_last) {
    const ptrdiff_t n_len = n_last - n_first;

    for (; h_last - h_first >= n_len; ++h_first) {
        bool is_equal = true;

        for (ptrdiff_t i = 0; i != n_len; ++i) {
            if (*(h_first + i) != *(n_first + i)) {
                is_equal = false;
                break;
            }
        }

        if (is_equal) {
            return h_first;
        }
    }

    return h_last;
}

template <class RanItH, class RanItN>
auto last_known_good_find_end(RanItH h_first, RanItH h_last, RanItN n_first, RanItN n_last) {
    const ptrdiff_t n_len = n_last - n_first;

    if (n_len > h_last - h_first) {
        return h_last;
    }

    auto h_mid = h_last - n_len;

    for (;;) {
        bool is_equal = true;

        for (ptrdiff_t i = 0; i != n_len; ++i) {
            if (*(h_mid + i) != *(n_first + i)) {
                is_equal = false;
                break;
            }
        }

        if (is_equal) {
            return h_mid;
        }

        if (h_mid == h_first) {
            return h_last;
        }

        --h_mid;
    }
}

template <class T>
void test_case_find(const vector<T>& input, T v) {
    auto expected = last_known_good_find(input.begin(), input.end(), v);
    auto actual   = find(input.begin(), input.end(), v);
    assert(expected == actual);
}

template <class T>
void test_find(mt19937_64& gen) {
    using TD = conditional_t<sizeof(T) == 1, int, T>;
    binomial_distribution<TD> dis(10);
    vector<T> input;
    input.reserve(dataCount);
    test_case_find(input, static_cast<T>(dis(gen)));
    for (size_t attempts = 0; attempts < dataCount; ++attempts) {
        input.push_back(static_cast<T>(dis(gen)));
        test_case_find(input, static_cast<T>(dis(gen)));
    }
}

#if _HAS_CXX20
template <class T, size_t N>
struct NormalArrayWrapper {
    T m_arr[N];
};

// Also test GH-4454 "vector_algorithms.cpp: __std_find_trivial_unsized_impl assumes N-byte elements are N-aligned"
#pragma pack(push, 1)
template <class T, size_t N>
struct PackedArrayWrapper {
    uint8_t m_ignored; // to misalign the following array
    T m_arr[N];
};
#pragma pack(pop)

// GH-4449 <xutility>: ranges::find with unreachable_sentinel / __std_find_trivial_unsized_1 gives wrong result
template <class T, template <class, size_t> class ArrayWrapper>
void test_gh_4449_impl() {
    constexpr T desired_val{11};
    constexpr T unwanted_val{22};

    ArrayWrapper<T, 256> wrapper;
    auto& arr = wrapper.m_arr;

    constexpr int mid1 = 64;
    constexpr int mid2 = 192;

    ranges::fill(arr, arr + mid1, desired_val);
    ranges::fill(arr + mid1, arr + mid2, unwanted_val);
    ranges::fill(arr + mid2, end(arr), desired_val);

    for (int idx = mid1; idx <= mid2; ++idx) { // when idx == mid2, the value is immediately found
        const auto where = ranges::find(arr + idx, unreachable_sentinel, desired_val);

        assert(where == arr + mid2);

        arr[idx] = desired_val; // get ready for the next iteration
    }
}

template <class T>
void test_gh_4449() {
    test_gh_4449_impl<T, NormalArrayWrapper>();
    test_gh_4449_impl<T, PackedArrayWrapper>();
}
#endif // _HAS_CXX20

#if _HAS_CXX23
template <class T>
void test_case_find_last(const vector<T>& input, T v) {
    auto expected = last_known_good_find_last(input.begin(), input.end(), v);
    auto range    = ranges::find_last(input.begin(), input.end(), v);
    auto actual   = range.begin();
    assert(expected == actual);
    assert(range.end() == input.end());
}

template <class T>
void test_find_last(mt19937_64& gen) {
    using TD = conditional_t<sizeof(T) == 1, int, T>;
    binomial_distribution<TD> dis(10);
    vector<T> input;
    input.reserve(dataCount);
    test_case_find_last(input, static_cast<T>(dis(gen)));
    for (size_t attempts = 0; attempts < dataCount; ++attempts) {
        input.push_back(static_cast<T>(dis(gen)));
        test_case_find_last(input, static_cast<T>(dis(gen)));
    }
}
#endif // _HAS_CXX23

template <class T>
void test_case_find_first_of(const vector<T>& input_haystack, const vector<T>& input_needle) {
    auto expected = last_known_good_find_first_of(
        input_haystack.begin(), input_haystack.end(), input_needle.begin(), input_needle.end());
    auto actual = find_first_of(input_haystack.begin(), input_haystack.end(), input_needle.begin(), input_needle.end());
    assert(expected == actual);
#if _HAS_CXX20
    auto ranges_actual = ranges::find_first_of(input_haystack, input_needle);
    assert(expected == ranges_actual);
#endif // _HAS_CXX20
}

constexpr size_t haystackDataCount = 200;
constexpr size_t needleDataCount   = 35;

template <class T>
void test_find_first_of(mt19937_64& gen) {
    using TD = conditional_t<sizeof(T) == 1, int, T>;
    uniform_int_distribution<TD> dis('a', 'z');
    vector<T> input_haystack;
    vector<T> input_needle;
    input_haystack.reserve(haystackDataCount);
    input_needle.reserve(needleDataCount);

    for (;;) {
        input_needle.clear();

        test_case_find_first_of(input_haystack, input_needle);
        for (size_t attempts = 0; attempts < needleDataCount; ++attempts) {
            input_needle.push_back(static_cast<T>(dis(gen)));
            test_case_find_first_of(input_haystack, input_needle);
        }

        if (input_haystack.size() == haystackDataCount) {
            break;
        }

        input_haystack.push_back(static_cast<T>(dis(gen)));
    }
}

template <class C1, class C2>
void test_find_first_of_containers() {
    C1 haystack{'m', 'e', 'o', 'w', 'C', 'A', 'T', 'S'};
    C2 needle{'R', 'S', 'T'};
    const auto result = find_first_of(haystack.begin(), haystack.end(), needle.begin(), needle.end());
    assert(result == haystack.begin() + 6);
#if _HAS_CXX20
    const auto ranges_result = ranges::find_first_of(haystack, needle);
    assert(ranges_result == haystack.begin() + 6);
#endif // _HAS_CXX20
}

template <class T>
void test_case_search(const vector<T>& input_haystack, const vector<T>& input_needle) {
    auto expected_search =
        last_known_good_search(input_haystack.begin(), input_haystack.end(), input_needle.begin(), input_needle.end());
    auto actual_search = search(input_haystack.begin(), input_haystack.end(), input_needle.begin(), input_needle.end());
    assert(expected_search == actual_search);

    auto expected_find_end = last_known_good_find_end(
        input_haystack.begin(), input_haystack.end(), input_needle.begin(), input_needle.end());
    auto actual_find_end =
        find_end(input_haystack.begin(), input_haystack.end(), input_needle.begin(), input_needle.end());
    assert(expected_find_end == actual_find_end);
#if _HAS_CXX17
    auto searcher_actual = search(
        input_haystack.begin(), input_haystack.end(), default_searcher{input_needle.begin(), input_needle.end()});
    assert(expected_search == searcher_actual);
#endif // _HAS_CXX17
#if _HAS_CXX20
    auto ranges_actual_search = ranges::search(input_haystack, input_needle);
    assert(expected_search == begin(ranges_actual_search));
    if (expected_search != input_haystack.end()) {
        assert(expected_search + static_cast<ptrdiff_t>(input_needle.size()) == end(ranges_actual_search));
    } else {
        assert(expected_search == end(ranges_actual_search));
    }

    auto ranges_actual_find_end = ranges::find_end(input_haystack, input_needle);
    assert(expected_find_end == begin(ranges_actual_find_end));
    if (expected_find_end != input_haystack.end()) {
        assert(expected_find_end + static_cast<ptrdiff_t>(input_needle.size()) == end(ranges_actual_find_end));
    } else {
        assert(expected_find_end == end(ranges_actual_find_end));
    }
#endif // _HAS_CXX20
}

template <class T>
void test_search(mt19937_64& gen) {
    using TD = conditional_t<sizeof(T) == 1, int, T>;
    uniform_int_distribution<TD> dis('0', '9');
    vector<T> input_haystack;
    vector<T> input_needle;
    vector<T> temp;
    input_haystack.reserve(haystackDataCount);
    input_needle.reserve(needleDataCount);
    temp.reserve(needleDataCount);

    for (;;) {
        input_needle.clear();

        test_case_search(input_haystack, input_needle);
        for (size_t attempts = 0; attempts < needleDataCount; ++attempts) {
            input_needle.push_back(static_cast<T>(dis(gen)));
            test_case_search(input_haystack, input_needle);

            // For large needles the chance of a match is low, so test a guaranteed match
            if (input_haystack.size() > input_needle.size() * 2) {
                uniform_int_distribution<size_t> pos_dis(0, input_haystack.size() - input_needle.size());
                const size_t pos             = pos_dis(gen);
                const auto overwritten_first = input_haystack.begin() + static_cast<ptrdiff_t>(pos);
                temp.assign(overwritten_first, overwritten_first + static_cast<ptrdiff_t>(input_needle.size()));
                copy(input_needle.begin(), input_needle.end(), overwritten_first);
                test_case_search(input_haystack, input_needle);
                copy(temp.begin(), temp.end(), overwritten_first);
            }
        }

        if (input_haystack.size() == haystackDataCount) {
            break;
        }

        input_haystack.push_back(static_cast<T>(dis(gen)));
    }
}

template <class T>
void test_min_max_element(mt19937_64& gen) {
    using Limits = numeric_limits<T>;

    uniform_int_distribution<conditional_t<sizeof(T) == 1, int, T>> dis(Limits::min(), Limits::max());

    vector<T> input;
    input.reserve(dataCount);
    test_case_min_max_element(input);
    for (size_t attempts = 0; attempts < dataCount; ++attempts) {
        input.push_back(static_cast<T>(dis(gen)));
        test_case_min_max_element(input);
    }
}

void test_min_max_element_pointers(mt19937_64& gen) {
    const short arr[20]{};

    uniform_int_distribution<size_t> dis(0, size(arr) - 1);

    vector<const short*> input;
    input.reserve(dataCount);
    test_case_min_max_element(input);
    for (size_t attempts = 0; attempts < dataCount; ++attempts) {
        input.push_back(arr + dis(gen));
        test_case_min_max_element(input);
    }
}

template <class ElementType, size_t VectorSize>
void test_min_max_element_special_cases() {
    constexpr size_t block_size_in_vectors  = 1 << (sizeof(ElementType) * CHAR_BIT);
    constexpr size_t block_size_in_elements = block_size_in_vectors * VectorSize;
    constexpr size_t num_blocks             = 4;
    constexpr size_t tail_size              = 13;
    constexpr size_t array_size             = num_blocks * block_size_in_elements + tail_size;
    constexpr size_t last_block_first_elem  = (num_blocks - 1) * block_size_in_elements;
    constexpr size_t last_vector_first_elem = (block_size_in_vectors - 1) * VectorSize;

    vector<ElementType> v(array_size); // not array to avoid large data on stack

    // all equal
    fill(v.begin(), v.end(), ElementType{1});
    assert(min_element(v.begin(), v.end()) == v.begin());
    assert(max_element(v.begin(), v.end()) == v.begin());
    assert(minmax_element(v.begin(), v.end()).first == v.begin());
    assert(minmax_element(v.begin(), v.end()).second == v.end() - 1);

    // same position in different blocks
    fill(v.begin(), v.end(), ElementType{1});
    for (size_t block_pos = 0; block_pos != num_blocks; ++block_pos) {
        v[block_pos * block_size_in_elements + 20 * VectorSize + 2] = 0;
        v[block_pos * block_size_in_elements + 20 * VectorSize + 5] = 0;
        v[block_pos * block_size_in_elements + 25 * VectorSize + 6] = 2;
        v[block_pos * block_size_in_elements + 25 * VectorSize + 9] = 2;
    }
    assert(min_element(v.begin(), v.end()) == v.begin() + 20 * VectorSize + 2);
    assert(max_element(v.begin(), v.end()) == v.begin() + 25 * VectorSize + 6);
    assert(minmax_element(v.begin(), v.end()).first == v.begin() + 20 * VectorSize + 2);
    assert(minmax_element(v.begin(), v.end()).second == v.begin() + last_block_first_elem + 25 * VectorSize + 9);


    // same block in different vectors
    fill(v.begin(), v.end(), ElementType{1});
    for (size_t vector_pos = 0; vector_pos != block_size_in_vectors; ++vector_pos) {
        v[2 * block_size_in_elements + vector_pos * VectorSize + 2] = 0;
        v[2 * block_size_in_elements + vector_pos * VectorSize + 5] = 0;
        v[2 * block_size_in_elements + vector_pos * VectorSize + 6] = 2;
        v[2 * block_size_in_elements + vector_pos * VectorSize + 9] = 2;
    }
    assert(min_element(v.begin(), v.end()) == v.begin() + 2 * block_size_in_elements + 2);
    assert(max_element(v.begin(), v.end()) == v.begin() + 2 * block_size_in_elements + 6);
    assert(minmax_element(v.begin(), v.end()).first == v.begin() + 2 * block_size_in_elements + 2);
    assert(minmax_element(v.begin(), v.end()).second
           == v.begin() + 2 * block_size_in_elements + last_vector_first_elem + 9);
}

template <class T>
void test_is_sorted_until(mt19937_64& gen) {
    using Limits = numeric_limits<T>;

    uniform_int_distribution<conditional_t<sizeof(T) == 1, int, T>> dis(Limits::min(), Limits::max());

    vector<T> original_input;
    vector<T> input;
    original_input.reserve(dataCount);
    input.reserve(dataCount);

    test_case_is_sorted_until(input, less<>{});
    test_case_is_sorted_until(input, greater<>{});

    for (size_t attempts = 0; attempts < dataCount; ++attempts) {
        original_input.push_back(static_cast<T>(dis(gen)));
        input = original_input;

        uniform_int_distribution<ptrdiff_t> pos_dis{0, static_cast<ptrdiff_t>(input.size() - 1)};
        auto it = input.begin() + pos_dis(gen);

        sort(input.begin(), it, less<>{});
        test_case_is_sorted_until(input, less<>{});

        reverse(input.begin(), it);
        test_case_is_sorted_until(input, greater<>{});
    }
}

template <class FwdIt, class T>
void last_known_good_replace(FwdIt first, FwdIt last, const T old_val, const T new_val) {
    for (; first != last; ++first) {
        if (*first == old_val) {
            *first = new_val;
        }
    }
}

template <class T>
void test_case_replace(const vector<T>& input, T old_val, T new_val) {
    vector<T> replaced_actual(input);
    vector<T> replaced_expected(input);
    replace(replaced_actual.begin(), replaced_actual.end(), old_val, new_val);
    last_known_good_replace(replaced_expected.begin(), replaced_expected.end(), old_val, new_val);
    assert(replaced_expected == replaced_actual);

#if _HAS_CXX20
    vector<T> replaced_actual_r(input);
    ranges::replace(replaced_actual_r, old_val, new_val);
    assert(replaced_expected == replaced_actual_r);
#endif // _HAS_CXX20
}

template <class T>
void test_replace(mt19937_64& gen) {
    using TD = conditional_t<sizeof(T) == 1, int, T>;
    uniform_int_distribution<TD> dis(0, 9);
    vector<T> input;

    input.reserve(dataCount);

    {
        const T old_val = static_cast<T>(dis(gen));
        const T new_val = static_cast<T>(dis(gen));
        test_case_replace(input, old_val, new_val);
    }

    for (size_t i = 0; i != dataCount; ++i) {
        input.push_back(static_cast<T>(dis(gen)));
        const T old_val = static_cast<T>(dis(gen));
        const T new_val = static_cast<T>(dis(gen));
        test_case_replace(input, old_val, new_val);
    }
}

template <class BidIt>
void last_known_good_reverse(BidIt first, BidIt last) {
    for (; first != last && first != --last; ++first) {
        iter_swap(first, last);
    }
}

template <class T>
void test_case_reverse(vector<T>& actual, vector<T>& expected) {
    expected = actual;
    last_known_good_reverse(expected.begin(), expected.end());
    reverse(actual.begin(), actual.end());
    assert(expected == actual);
}

template <class T>
void test_reverse(mt19937_64& gen) {
    vector<T> actual;
    vector<T> expected;
    actual.reserve(dataCount);
    expected.reserve(dataCount);
    test_case_reverse(actual, expected);
    for (size_t attempts = 0; attempts < dataCount; ++attempts) {
        actual.push_back(static_cast<T>(gen())); // intentionally narrows
        test_case_reverse(actual, expected);
    }
}

template <class T>
void test_case_reverse_copy(vector<T>& input) {
    auto expected = input;
    last_known_good_reverse(expected.begin(), expected.end());
    vector<T> output(input.size(), T{});
    assert(reverse_copy(input.begin(), input.end(), output.begin()) == output.end());
    assert(expected == output);
}

template <class T>
void test_reverse_copy(mt19937_64& gen) {
    vector<T> input;
    input.reserve(dataCount);
    test_case_reverse_copy(input);
    for (size_t attempts = 0; attempts < dataCount; ++attempts) {
        input.push_back(static_cast<T>(gen())); // intentionally narrows
        test_case_reverse_copy(input);
    }
}

template <class RanIt>
void last_known_good_rotate(
    RanIt first, RanIt mid, RanIt last, vector<typename iterator_traits<RanIt>::value_type>& tmp) {
    const auto size_left  = mid - first;
    const auto size_right = last - mid;
    if (size_left <= size_right) {
        tmp.assign(first, mid);
        move_backward(mid, last, last - size_left);
        move(tmp.begin(), tmp.end(), last - size_left);
    } else {
        tmp.assign(mid, last);
        move(first, mid, first + size_right);
        move(tmp.begin(), tmp.end(), first);
    }
}

template <class T>
void test_case_rotate(
    vector<T>& actual, vector<T>& actual_r, vector<T>& expected, const ptrdiff_t pos, vector<T>& tmp) {
    const ptrdiff_t shift = static_cast<ptrdiff_t>(expected.size()) - pos;
    last_known_good_rotate(expected.begin(), expected.begin() + pos, expected.end(), tmp);
    const auto it = rotate(actual.begin(), actual.begin() + pos, actual.end());
    assert(expected == actual);
    assert(it == actual.begin() + shift);
#if _HAS_CXX20
    const auto rng = ranges::rotate(actual_r.begin(), actual_r.begin() + pos, actual_r.end());
    assert(expected == actual_r);
    assert(begin(rng) == actual_r.begin() + shift);
    assert(end(rng) == actual_r.end());
#else // ^^^ _HAS_CXX20 / !_HAS_CXX20 vvv
    (void) actual_r;
#endif // ^^^ !_HAS_CXX20 ^^^
}

template <class T>
void test_rotate(mt19937_64& gen, const size_t data_count = dataCount) {
    vector<T> actual;
    vector<T> actual_r;
    vector<T> expected;
    vector<T> tmp;
    actual.reserve(data_count);
    actual_r.reserve(data_count);
    expected.reserve(data_count);
    tmp.reserve(data_count);
    test_case_rotate(actual, actual_r, expected, 0, tmp);
    for (size_t attempts = 0; attempts < data_count; ++attempts) {
        const T val = static_cast<T>(gen()); // intentionally narrows
        actual.push_back(val);
        actual_r.push_back(val);
        expected.push_back(val);

        uniform_int_distribution<ptrdiff_t> dis_pos(0, static_cast<ptrdiff_t>(attempts) + 1);

        for (size_t pos_count = 0; pos_count != 5; ++pos_count) {
            test_case_rotate(actual, actual_r, expected, dis_pos(gen), tmp);
        }
    }
}

template <class FwdIt1, class FwdIt2>
FwdIt2 last_known_good_swap_ranges(FwdIt1 first1, const FwdIt1 last1, FwdIt2 dest) {
    for (; first1 != last1; ++first1, ++dest) {
        iter_swap(first1, dest);
    }

    return dest;
}

template <class FwdIt, class T>
FwdIt last_known_good_remove(FwdIt first, FwdIt last, T val) {
    FwdIt dest = first;

    while (first != last) {
        if (*first != val) {
            *dest = *first;
            ++dest;
        }

        ++first;
    }

    return dest;
}

template <class InIt, class OutIt, class T>
OutIt last_known_good_remove_copy(InIt first, InIt last, OutIt dest, T val) {
    while (first != last) {
        if (*first != val) {
            *dest = *first;
            ++dest;
        }

        ++first;
    }

    return dest;
}

template <class T>
void test_case_remove(vector<T>& in_out_expected, vector<T>& in_out_actual, vector<T>& in_out_actual_r, const T val) {
    auto rem_expected = last_known_good_remove(in_out_expected.begin(), in_out_expected.end(), val);
    auto rem_actual   = remove(in_out_actual.begin(), in_out_actual.end(), val);
    assert(equal(in_out_expected.begin(), rem_expected, in_out_actual.begin(), rem_actual));

#if _HAS_CXX20
    auto rem_actual_r = ranges::remove(in_out_actual_r, val);
    assert(equal(in_out_expected.begin(), rem_expected, begin(in_out_actual_r), begin(rem_actual_r)));
    assert(end(rem_actual_r) == in_out_actual_r.end());
#else // ^^^ _HAS_CXX20 / !_HAS_CXX20 vvv
    (void) in_out_actual_r;
#endif // ^^^ !_HAS_CXX20 ^^^
}

template <class T>
void test_case_remove_copy(
    const vector<T>& source, vector<T>& out_expected, vector<T>& out_actual, vector<T>& out_actual_r, const T val) {
    auto rem_expected = last_known_good_remove_copy(source.begin(), source.end(), out_expected.begin(), val);
    auto rem_actual   = remove_copy(source.begin(), source.end(), out_actual.begin(), val);
    assert(equal(out_expected.begin(), rem_expected, out_actual.begin(), rem_actual));
    assert(equal(rem_expected, out_expected.end(), rem_actual, out_actual.end()));

#if _HAS_CXX20
    auto rem_actual_r = ranges::remove_copy(source, out_actual_r.begin(), val);
    assert(equal(out_expected.begin(), rem_expected, out_actual_r.begin(), rem_actual_r.out));
    assert(equal(rem_expected, out_expected.end(), rem_actual_r.out, out_actual_r.end()));
    assert(rem_actual_r.in == source.end());
#else // ^^^ _HAS_CXX20 / !_HAS_CXX20 vvv
    (void) out_actual_r;
#endif // ^^^ !_HAS_CXX20 ^^^
}

template <class T>
void test_remove(mt19937_64& gen) {
    using TD = conditional_t<sizeof(T) == 1, int, T>;
    binomial_distribution<TD> dis(10);

    vector<T> source;
    vector<T> out_expected;
    vector<T> out_actual;
    vector<T> out_actual_r;
    vector<T> in_out_expected;
    vector<T> in_out_actual;
    vector<T> in_out_actual_r;

    for (const auto& v :
        {&source, &in_out_expected, &in_out_actual, &in_out_actual_r, &out_expected, &out_actual, &out_actual_r}) {
        v->reserve(dataCount);
    }

    test_case_remove(in_out_expected, in_out_actual, in_out_actual_r, static_cast<T>(dis(gen)));
    test_case_remove_copy(source, out_expected, out_actual, out_actual_r, static_cast<T>(dis(gen)));

    for (size_t attempts = 0; attempts < dataCount; ++attempts) {
        source.push_back(static_cast<T>(dis(gen)));

        for (const auto& v : {&in_out_expected, &in_out_actual, &in_out_actual_r}) {
            *v = source;
        }

        for (const auto& v : {&out_expected, &out_actual, &out_actual_r}) {
            v->assign(source.size(), T{0});
        }

        test_case_remove(in_out_expected, in_out_actual, in_out_actual_r, static_cast<T>(dis(gen)));
        test_case_remove_copy(source, out_expected, out_actual, out_actual_r, static_cast<T>(dis(gen)));
    }
}

template <class FwdIt>
FwdIt last_known_good_unique(FwdIt first, FwdIt last) {
    if (first == last) {
        return first;
    }

    FwdIt dest = first;
    ++first;

    while (first != last) {
        if (*first != *dest) {
            ++dest;
            *dest = *first;
        }

        ++first;
    }

    ++dest;
    return dest;
}

template <class FwdItIn, class FwdItOut>
FwdItOut last_known_good_unique_copy(FwdItIn first, FwdItIn last, FwdItOut dest) {
    if (first == last) {
        return dest;
    }

    *dest = *first;
    ++first;

    while (first != last) {
        if (*first != *dest) {
            ++dest;
            *dest = *first;
        }

        ++first;
    }

    ++dest;
    return dest;
}

template <class T>
void test_case_unique(vector<T>& in_out_expected, vector<T>& in_out_actual, vector<T>& in_out_actual_r) {
    auto un_expected = last_known_good_unique(in_out_expected.begin(), in_out_expected.end());
    auto un_actual   = unique(in_out_actual.begin(), in_out_actual.end());
    assert(equal(in_out_expected.begin(), un_expected, in_out_actual.begin(), un_actual));

#if _HAS_CXX20
    auto un_actual_r = ranges::unique(in_out_actual_r);
    assert(equal(in_out_expected.begin(), un_expected, begin(in_out_actual_r), begin(un_actual_r)));
    assert(end(un_actual_r) == in_out_actual_r.end());
#else // ^^^ _HAS_CXX20 / !_HAS_CXX20 vvv
    (void) in_out_actual_r;
#endif // ^^^ !_HAS_CXX20 ^^^
}

template <class T>
void test_case_unique_copy(
    const vector<T>& source, vector<T>& out_expected, vector<T>& out_actual, vector<T>& out_actual_r) {
    auto un_expected = last_known_good_unique_copy(source.begin(), source.end(), out_expected.begin());
    auto un_actual   = unique_copy(source.begin(), source.end(), out_actual.begin());
    assert(equal(out_expected.begin(), un_expected, out_actual.begin(), un_actual));
    assert(equal(un_expected, out_expected.end(), un_actual, out_actual.end()));

#if _HAS_CXX20
    auto un_actual_r = ranges::unique_copy(source, out_actual_r.begin());
    assert(equal(out_expected.begin(), un_expected, out_actual_r.begin(), un_actual_r.out));
    assert(equal(un_expected, out_expected.end(), un_actual_r.out, out_actual_r.end()));
    assert(un_actual_r.in == source.end());
#else // ^^^ _HAS_CXX20 / !_HAS_CXX20 vvv
    (void) out_actual_r;
#endif // ^^^ !_HAS_CXX20 ^^^
}

template <class T>
void test_unique(mt19937_64& gen) {
    constexpr int number_of_values = 5;

    struct unused_t {};

    conditional_t<is_pointer_v<T>, array<remove_pointer_t<T>, number_of_values>, unused_t> ptr_val_array{};

    using TD = conditional_t<sizeof(T) == 1 || is_pointer_v<T>, int, T>;
    binomial_distribution<TD> dis(number_of_values);

    vector<T> source;
    vector<T> out_expected;
    vector<T> out_actual;
    vector<T> out_actual_r;
    vector<T> in_out_expected;
    vector<T> in_out_actual;
    vector<T> in_out_actual_r;

    for (const auto& v :
        {&source, &in_out_expected, &in_out_actual, &in_out_actual_r, &out_expected, &out_actual, &out_actual_r}) {
        v->reserve(dataCount);
    }

    test_case_unique(in_out_expected, in_out_actual, in_out_actual_r);
    test_case_unique_copy(source, out_expected, out_actual, out_actual_r);
    for (size_t attempts = 0; attempts < dataCount; ++attempts) {
        if constexpr (is_pointer_v<T>) {
            source.push_back(ptr_val_array.data() + dis(gen));
        } else {
            source.push_back(static_cast<T>(dis(gen)));
        }

        for (const auto& v : {&in_out_expected, &in_out_actual, &in_out_actual_r}) {
            *v = source;
        }

        for (const auto& v : {&out_expected, &out_actual, &out_actual_r}) {
            v->assign(source.size(), T{0});
        }

        test_case_unique(in_out_expected, in_out_actual, in_out_actual_r);
        test_case_unique_copy(source, out_expected, out_actual, out_actual_r);
    }
}

template <class T>
void test_swap_ranges(mt19937_64& gen) {
    const auto fn = [&]() { return static_cast<T>(gen()); };
    vector<T> left(dataCount);
    vector<T> right(dataCount);
    generate(left.begin(), left.end(), fn);
    generate(right.begin(), right.end(), fn);

    auto leftCopy  = left;
    auto rightCopy = right;

    for (ptrdiff_t attempts = 0; attempts < static_cast<ptrdiff_t>(dataCount); ++attempts) {
        assert(right.begin() + attempts == swap_ranges(left.begin(), left.begin() + attempts, right.begin()));
        last_known_good_swap_ranges(leftCopy.begin(), leftCopy.begin() + attempts, rightCopy.begin());
        assert(left == leftCopy);
        assert(right == rightCopy);

        // also test unaligned input
        const auto endOffset = min(static_cast<ptrdiff_t>(dataCount), attempts + 1);
        assert(
            right.begin() + (endOffset - 1) == swap_ranges(left.begin() + 1, left.begin() + endOffset, right.begin()));
        last_known_good_swap_ranges(leftCopy.begin() + 1, leftCopy.begin() + endOffset, rightCopy.begin());
        assert(left == leftCopy);
        assert(right == rightCopy);
    }
}

// GH-2683 "std::swap of arrays, why is there no specialization for trivial types"
template <class T, size_t N>
void test_swap_arrays(mt19937_64& gen) {
    const auto fn = [&]() { return static_cast<T>(gen()); };
    T left[N];
    T right[N];
    generate(begin(left), end(left), fn);
    generate(begin(right), end(right), fn);

    const vector<T> origLeft(begin(left), end(left));
    const vector<T> origRight(begin(right), end(right));

    swap(left, right);

    assert(equal(begin(left), end(left), origRight.begin(), origRight.end()));
    assert(equal(begin(right), end(right), origLeft.begin(), origLeft.end()));

#if _HAS_CXX20
    ranges::swap(left, right);

    assert(equal(begin(left), end(left), origLeft.begin(), origLeft.end()));
    assert(equal(begin(right), end(right), origRight.begin(), origRight.end()));
#endif // _HAS_CXX20
}

void test_vector_algorithms(mt19937_64& gen) {
    test_adjacent_difference<char>(gen);
    test_adjacent_difference<signed char>(gen);
    test_adjacent_difference<unsigned char>(gen);
    test_adjacent_difference<short>(gen);
    test_adjacent_difference<unsigned short>(gen);
    test_adjacent_difference<int>(gen);
    test_adjacent_difference<unsigned int>(gen);
    test_adjacent_difference<long long>(gen);
    test_adjacent_difference<unsigned long long>(gen);

    test_adjacent_difference_with_heterogeneous_types();

    test_adjacent_find<char>(gen);
    test_adjacent_find<signed char>(gen);
    test_adjacent_find<unsigned char>(gen);
    test_adjacent_find<short>(gen);
    test_adjacent_find<unsigned short>(gen);
    test_adjacent_find<int>(gen);
    test_adjacent_find<unsigned int>(gen);
    test_adjacent_find<long long>(gen);
    test_adjacent_find<unsigned long long>(gen);

    test_count<char>(gen);
    test_count<signed char>(gen);
    test_count<unsigned char>(gen);
    test_count<short>(gen);
    test_count<unsigned short>(gen);
    test_count<int>(gen);
    test_count<unsigned int>(gen);
    test_count<long long>(gen);
    test_count<unsigned long long>(gen);

    test_find<char>(gen);
    test_find<signed char>(gen);
    test_find<unsigned char>(gen);
    test_find<short>(gen);
    test_find<unsigned short>(gen);
    test_find<int>(gen);
    test_find<unsigned int>(gen);
    test_find<long long>(gen);
    test_find<unsigned long long>(gen);

#if _HAS_CXX20
    test_gh_4449<uint8_t>();
    test_gh_4449<uint16_t>();
    test_gh_4449<uint32_t>();
    test_gh_4449<uint64_t>();
#endif // _HAS_CXX20

#if _HAS_CXX23
    test_find_last<char>(gen);
    test_find_last<signed char>(gen);
    test_find_last<unsigned char>(gen);
    test_find_last<short>(gen);
    test_find_last<unsigned short>(gen);
    test_find_last<int>(gen);
    test_find_last<unsigned int>(gen);
    test_find_last<long long>(gen);
    test_find_last<unsigned long long>(gen);
#endif // _HAS_CXX23

    test_find_first_of<char>(gen);
    test_find_first_of<signed char>(gen);
    test_find_first_of<unsigned char>(gen);
    test_find_first_of<short>(gen);
    test_find_first_of<unsigned short>(gen);
    test_find_first_of<int>(gen);
    test_find_first_of<unsigned int>(gen);
    test_find_first_of<long long>(gen);
    test_find_first_of<unsigned long long>(gen);

    test_find_first_of_containers<vector<char>, vector<signed char>>();
    test_find_first_of_containers<vector<char>, vector<unsigned char>>();
    test_find_first_of_containers<vector<wchar_t>, vector<char>>();
    test_find_first_of_containers<const vector<char>, const vector<char>>();
    test_find_first_of_containers<vector<char>, const vector<char>>();
    test_find_first_of_containers<const vector<wchar_t>, vector<wchar_t>>();
    test_find_first_of_containers<vector<char>, vector<int>>();

    test_search<char>(gen);
    test_search<signed char>(gen);
    test_search<unsigned char>(gen);
    test_search<short>(gen);
    test_search<unsigned short>(gen);
    test_search<int>(gen);
    test_search<unsigned int>(gen);
    test_search<long long>(gen);
    test_search<unsigned long long>(gen);

    test_min_max_element<char>(gen);
    test_min_max_element<signed char>(gen);
    test_min_max_element<unsigned char>(gen);
    test_min_max_element<short>(gen);
    test_min_max_element<unsigned short>(gen);
    test_min_max_element<int>(gen);
    test_min_max_element<unsigned int>(gen);
    test_min_max_element<long long>(gen);
    test_min_max_element<unsigned long long>(gen);

    test_min_max_element_pointers(gen);

    test_min_max_element_special_cases<int8_t, 16>(); // SSE2 vectors
    test_min_max_element_special_cases<int8_t, 32>(); // AVX2 vectors
    test_min_max_element_special_cases<int8_t, 64>(); // AVX512 vectors

    // Test VSO-1558536, a regression caused by GH-2447 that was specific to 64-bit types on x86.
    test_case_min_max_element(vector<uint64_t>{10, 0x8000'0000ULL, 20, 30});
    test_case_min_max_element(vector<uint64_t>{10, 20, 0xD000'0000'B000'0000ULL, 30, 0xC000'0000'A000'0000ULL});
    test_case_min_max_element(vector<int64_t>{10, 0x8000'0000LL, 20, 30});
    test_case_min_max_element(
        vector<int64_t>{-6604286336755016904, -4365366089374418225, 6104371530830675888, -8582621853879131834});

    test_is_sorted_until<char>(gen);
    test_is_sorted_until<signed char>(gen);
    test_is_sorted_until<unsigned char>(gen);
    test_is_sorted_until<short>(gen);
    test_is_sorted_until<unsigned short>(gen);
    test_is_sorted_until<int>(gen);
    test_is_sorted_until<unsigned int>(gen);
    test_is_sorted_until<long long>(gen);
    test_is_sorted_until<unsigned long long>(gen);

    // replace() is vectorized for 4 and 8 bytes only.
    test_replace<int>(gen);
    test_replace<unsigned int>(gen);
    test_replace<long long>(gen);
    test_replace<unsigned long long>(gen);

    test_reverse<char>(gen);
    test_reverse<signed char>(gen);
    test_reverse<unsigned char>(gen);
    test_reverse<short>(gen);
    test_reverse<unsigned short>(gen);
    test_reverse<int>(gen);
    test_reverse<unsigned int>(gen);
    test_reverse<long long>(gen);
    test_reverse<unsigned long long>(gen);
    test_reverse<float>(gen);
    test_reverse<double>(gen);
    test_reverse<long double>(gen);

    test_reverse_copy<char>(gen);
    test_reverse_copy<signed char>(gen);
    test_reverse_copy<unsigned char>(gen);
    test_reverse_copy<short>(gen);
    test_reverse_copy<unsigned short>(gen);
    test_reverse_copy<int>(gen);
    test_reverse_copy<unsigned int>(gen);
    test_reverse_copy<long long>(gen);
    test_reverse_copy<unsigned long long>(gen);
    test_reverse_copy<float>(gen);
    test_reverse_copy<double>(gen);
    test_reverse_copy<long double>(gen);

    test_rotate<char>(gen, 20000); // one real long rotate run, as for smaller arrays some strategies aren't executed
    test_rotate<signed char>(gen);
    test_rotate<unsigned char>(gen);
    test_rotate<short>(gen);
    test_rotate<unsigned short>(gen);
    test_rotate<int>(gen);
    test_rotate<unsigned int>(gen);
    test_rotate<long long>(gen);
    test_rotate<unsigned long long>(gen);
    test_rotate<float>(gen);
    test_rotate<double>(gen);
    test_rotate<long double>(gen);

    test_remove<char>(gen);
    test_remove<signed char>(gen);
    test_remove<unsigned char>(gen);
    test_remove<short>(gen);
    test_remove<unsigned short>(gen);
    test_remove<int>(gen);
    test_remove<unsigned int>(gen);
    test_remove<long long>(gen);
    test_remove<unsigned long long>(gen);

    test_unique<char>(gen);
    test_unique<signed char>(gen);
    test_unique<unsigned char>(gen);
    test_unique<short>(gen);
    test_unique<unsigned short>(gen);
    test_unique<int>(gen);
    test_unique<unsigned int>(gen);
    test_unique<long long>(gen);
    test_unique<unsigned long long>(gen);

    test_unique<long*>(gen);

    test_swap_ranges<char>(gen);
    test_swap_ranges<short>(gen);
    test_swap_ranges<int>(gen);
    test_swap_ranges<unsigned int>(gen);
    test_swap_ranges<unsigned long long>(gen);

    test_swap_arrays<uint8_t, 1>(gen);
    test_swap_arrays<uint16_t, 1>(gen);
    test_swap_arrays<uint32_t, 1>(gen);
    test_swap_arrays<uint64_t, 1>(gen);

    test_swap_arrays<uint8_t, 47>(gen);
    test_swap_arrays<uint16_t, 47>(gen);
    test_swap_arrays<uint32_t, 47>(gen);
    test_swap_arrays<uint64_t, 47>(gen);

    test_swap_arrays<uint8_t, 512>(gen);
    test_swap_arrays<uint16_t, 512>(gen);
    test_swap_arrays<uint32_t, 512>(gen);
    test_swap_arrays<uint64_t, 512>(gen);
}

template <typename Container1, typename Container2>
void test_two_containers() {
    Container1 one                  = {10, 20, 30, 40, 50};
    Container2 two                  = {-1, -1, -1, -1, -1};
    static constexpr int reversed[] = {50, 40, 30, 20, 10};

    assert(reverse_copy(one.begin(), one.end(), two.begin()) == two.end());
    assert(equal(two.begin(), two.end(), begin(reversed), end(reversed)));

    static constexpr int squares[] = {1, 4, 9, 16, 25};
    static constexpr int cubes[]   = {1, 8, 27, 64, 125};
    one.assign(begin(squares), end(squares));
    two.assign(begin(cubes), end(cubes));

    assert(swap_ranges(one.begin(), one.end(), two.begin()) == two.end());
    assert(equal(one.begin(), one.end(), begin(cubes), end(cubes)));
    assert(equal(two.begin(), two.end(), begin(squares), end(squares)));
}

template <typename Container>
void test_one_container() {
    Container x                     = {10, 20, 30, 40, 50};
    static constexpr int reversed[] = {50, 40, 30, 20, 10};

    reverse(x.begin(), x.end());
    assert(equal(x.begin(), x.end(), begin(reversed), end(reversed)));

    test_two_containers<Container, vector<int>>();
    test_two_containers<Container, deque<int>>();
    test_two_containers<Container, list<int>>();
}

template <size_t N>
bool test_randomized_bitset(mt19937_64& gen) {
    string str;
    wstring wstr;
    str.reserve(N);
    wstr.reserve(N);

    while (str.size() != N) {
        uint64_t random_value = gen();

        for (int bits = 0; bits < 64 && str.size() != N; ++bits) {
            const auto character = '0' + (random_value & 1);
            str.push_back(static_cast<char>(character));
            wstr.push_back(static_cast<wchar_t>(character));
            random_value >>= 1;
        }
    }

    const bitset<N> b(str);

    assert(b.to_string() == str);
    assert(b.template to_string<wchar_t>() == wstr);

    return true;
}

template <size_t Base, size_t... Vals>
void test_randomized_bitset_base(index_sequence<Vals...>, mt19937_64& gen) {
    bool ignored[] = {test_randomized_bitset<Base + Vals>(gen)...};
    (void) ignored;
}

template <size_t Base, size_t Count>
void test_randomized_bitset_base_count(mt19937_64& gen) {
    test_randomized_bitset_base<Base>(make_index_sequence<Count>{}, gen);
}

template <class F>
void assert_throws_inv(F f) {
    try {
        f();
        assert(false);
    } catch (const invalid_argument&) {
    }
}

void test_bitset(mt19937_64& gen) {
    assert(bitset<0>(0x0ULL).to_string() == "");
    assert(bitset<0>(0xFEDCBA9876543210ULL).to_string() == "");
    assert(bitset<15>(0x6789ULL).to_string() == "110011110001001");
    assert(bitset<15>(0xFEDCBA9876543210ULL).to_string() == "011001000010000");
    assert(bitset<32>(0xABCD1234ULL).to_string() == "10101011110011010001001000110100");
    assert(bitset<32>(0xFEDCBA9876543210ULL).to_string() == "01110110010101000011001000010000");
    assert(bitset<45>(0x1701D1729FFFULL).to_string() == "101110000000111010001011100101001111111111111");
    assert(bitset<45>(0xFEDCBA9876543210ULL).to_string() == "110101001100001110110010101000011001000010000");
    assert(bitset<64>(0xFEDCBA9876543210ULL).to_string()
           == "1111111011011100101110101001100001110110010101000011001000010000");
    assert(bitset<75>(0xFEDCBA9876543210ULL).to_string()
           == "000000000001111111011011100101110101001100001110110010101000011001000010000");

    assert(bitset<0>(0x0ULL).to_string<wchar_t>() == L"");
    assert(bitset<0>(0xFEDCBA9876543210ULL).to_string<wchar_t>() == L"");
    assert(bitset<15>(0x6789ULL).to_string<wchar_t>() == L"110011110001001");
    assert(bitset<15>(0xFEDCBA9876543210ULL).to_string<wchar_t>() == L"011001000010000");
    assert(bitset<32>(0xABCD1234ULL).to_string<wchar_t>() == L"10101011110011010001001000110100");
    assert(bitset<32>(0xFEDCBA9876543210ULL).to_string<wchar_t>() == L"01110110010101000011001000010000");
    assert(bitset<45>(0x1701D1729FFFULL).to_string<wchar_t>() == L"101110000000111010001011100101001111111111111");
    assert(bitset<45>(0xFEDCBA9876543210ULL).to_string<wchar_t>() == L"110101001100001110110010101000011001000010000");
    assert(bitset<64>(0xFEDCBA9876543210ULL).to_string<wchar_t>()
           == L"1111111011011100101110101001100001110110010101000011001000010000");
    assert(bitset<75>(0xFEDCBA9876543210ULL).to_string<wchar_t>()
           == L"000000000001111111011011100101110101001100001110110010101000011001000010000");

    assert(bitset<64>(0xFEDCBA9876543210ULL).to_string('o', 'x')
           == "xxxxxxxoxxoxxxooxoxxxoxoxooxxooooxxxoxxooxoxoxooooxxooxooooxoooo");
    assert(bitset<64>(0xFEDCBA9876543210ULL).to_string<wchar_t>(L'o', L'x')
           == L"xxxxxxxoxxoxxxooxoxxxoxoxooxxooooxxxoxxooxoxoxooooxxooxooooxoooo");

#ifdef __cpp_lib_char8_t
    assert(bitset<75>(0xFEDCBA9876543210ULL).to_string<char8_t>()
           == u8"000000000001111111011011100101110101001100001110110010101000011001000010000");
#endif // __cpp_lib_char8_t
    assert(bitset<75>(0xFEDCBA9876543210ULL).to_string<char16_t>()
           == u"000000000001111111011011100101110101001100001110110010101000011001000010000");
    assert(bitset<75>(0xFEDCBA9876543210ULL).to_string<char32_t>()
           == U"000000000001111111011011100101110101001100001110110010101000011001000010000"); // not vectorized

    assert(bitset<0>("").to_ullong() == 0);
    assert(bitset<0>("1").to_ullong() == 0);
    assert_throws_inv([] { (void) bitset<0>("x"); });

    assert(bitset<45>("101110000000111010001011100101001111111111111").to_ullong() == 0x1701D1729FFFULL);
    assert(bitset<45>("110101001100001110110010101000011001000010000").to_ullong() == 0x1A9876543210ULL);
    assert(bitset<45>("111").to_ullong() == 0x7);
    assert_throws_inv([] { (void) bitset<45>("11x11"); });
    assert_throws_inv([] { (void) bitset<45>("111111111111111111111111111111111111111111111x"); });
    assert_throws_inv([] { (void) bitset<45>("x111111111111111111111111111111111111111111111"); });

    assert(bitset<64>("xxxxxxxoxxoxxxooxoxxxoxoxooxxooooxxxoxxooxoxoxooooxxooxooooxoooo", string::npos, 'o', 'x')
               .to_ullong()
           == 0xFEDCBA9876543210ULL);
    assert(bitset<64>(L"xxxxxxxoxxoxxxooxoxxxoxoxooxxooooxxxoxxooxoxoxooooxxooxooooxoooo", wstring::npos, L'o', L'x')
               .to_ullong()
           == 0xFEDCBA9876543210ULL);

#ifdef __cpp_lib_char8_t
    assert(bitset<75>(u8"000000000001111111011011100101110101001100001110110010101000011001000010000").to_ullong()
           == 0xFEDCBA9876543210ULL);
#endif // __cpp_lib_char8_t
    assert(bitset<75>(u"000000000001111111011011100101110101001100001110110010101000011001000010000").to_ullong()
           == 0xFEDCBA9876543210ULL);
    assert(bitset<75>(U"000000000001111111011011100101110101001100001110110010101000011001000010000").to_ullong()
           == 0xFEDCBA9876543210ULL); // not vectorized

    test_randomized_bitset_base_count<512 - 5, 32 + 10>(gen);
}

template <class T>
size_t last_known_good_find_first_of(const basic_string<T>& h, const basic_string<T>& n) {
    for (size_t pos = 0, pos_max = h.size(); pos != pos_max; ++pos) {
        if (n.find(h[pos]) != basic_string<T>::npos) {
            return pos;
        }
    }

    return basic_string<T>::npos;
}

template <class T>
size_t last_known_good_find_first_not_of(const basic_string<T>& h, const basic_string<T>& n) {
    for (size_t pos = 0, pos_max = h.size(); pos != pos_max; ++pos) {
        if (n.find(h[pos]) == basic_string<T>::npos) {
            return pos;
        }
    }

    return basic_string<T>::npos;
}

template <class T>
size_t last_known_good_find_last_of(const basic_string<T>& h, const basic_string<T>& n) {
    size_t pos = h.size();
    while (pos != 0) {
        --pos;
        if (n.find(h[pos]) != basic_string<T>::npos) {
            return pos;
        }
    }

    return basic_string<T>::npos;
}

template <class T>
size_t last_known_good_find_last_not_of(const basic_string<T>& h, const basic_string<T>& n) {
    size_t pos = h.size();
    while (pos != 0) {
        --pos;
        if (n.find(h[pos]) == basic_string<T>::npos) {
            return pos;
        }
    }

    return basic_string<T>::npos;
}

template <class T>
void test_case_string_find_first_of(const basic_string<T>& input_haystack, const basic_string<T>& input_needle) {
    size_t expected = last_known_good_find_first_of(input_haystack, input_needle);
    size_t actual   = input_haystack.find_first_of(input_needle);
    assert(expected == actual);
}

template <class T>
void test_case_string_find_first_not_of(const basic_string<T>& input_haystack, const basic_string<T>& input_needle) {
    size_t expected = last_known_good_find_first_not_of(input_haystack, input_needle);
    size_t actual   = input_haystack.find_first_not_of(input_needle);
    assert(expected == actual);
}

template <class T>
void test_case_string_find_last_of(const basic_string<T>& input_haystack, const basic_string<T>& input_needle) {
    size_t expected = last_known_good_find_last_of(input_haystack, input_needle);
    size_t actual   = input_haystack.find_last_of(input_needle);
    assert(expected == actual);
}

template <class T>
void test_case_string_find_last_not_of(const basic_string<T>& input_haystack, const basic_string<T>& input_needle) {
    size_t expected = last_known_good_find_last_not_of(input_haystack, input_needle);
    size_t actual   = input_haystack.find_last_not_of(input_needle);
    assert(expected == actual);
}

template <class T>
void test_case_string_find_ch(const basic_string<T>& input_haystack, const T value) {
    ptrdiff_t expected;

    const auto expected_iter = last_known_good_find(input_haystack.begin(), input_haystack.end(), value);

    if (expected_iter != input_haystack.end()) {
        expected = expected_iter - input_haystack.begin();
    } else {
        expected = -1;
    }

    const auto actual = static_cast<ptrdiff_t>(input_haystack.find(value));
    assert(expected == actual);
}

template <class T>
void test_case_string_rfind_ch(const basic_string<T>& input_haystack, const T value) {
    ptrdiff_t expected;

    const auto expected_iter = last_known_good_find_last(input_haystack.begin(), input_haystack.end(), value);

    if (expected_iter != input_haystack.end()) {
        expected = expected_iter - input_haystack.begin();
    } else {
        expected = -1;
    }

    const auto actual = static_cast<ptrdiff_t>(input_haystack.rfind(value));
    assert(expected == actual);
}

template <class T>
void test_case_string_find_str(const basic_string<T>& input_haystack, const basic_string<T>& input_needle) {
    ptrdiff_t expected;
    if (input_needle.empty()) {
        expected = 0;
    } else {
        const auto expected_iter = last_known_good_search(
            input_haystack.begin(), input_haystack.end(), input_needle.begin(), input_needle.end());

        if (expected_iter != input_haystack.end()) {
            expected = expected_iter - input_haystack.begin();
        } else {
            expected = -1;
        }
    }
    const auto actual = static_cast<ptrdiff_t>(input_haystack.find(input_needle));
    assert(expected == actual);
}

template <class T>
void test_case_string_rfind_str(const basic_string<T>& input_haystack, const basic_string<T>& input_needle) {
    ptrdiff_t expected;
    if (input_needle.empty()) {
        expected = static_cast<ptrdiff_t>(input_haystack.size());
    } else {
        const auto expected_iter = last_known_good_find_end(
            input_haystack.begin(), input_haystack.end(), input_needle.begin(), input_needle.end());

        if (expected_iter != input_haystack.end()) {
            expected = expected_iter - input_haystack.begin();
        } else {
            expected = -1;
        }
    }
    const auto actual = static_cast<ptrdiff_t>(input_haystack.rfind(input_needle));
    assert(expected == actual);
}

template <class T>
void test_case_string_find_not_ch(const basic_string<T>& input_haystack, const T value) {
    ptrdiff_t expected;

    const auto expected_iter =
        last_known_good_find(input_haystack.begin(), input_haystack.end(), value, not_equal_to<>{});

    if (expected_iter != input_haystack.end()) {
        expected = expected_iter - input_haystack.begin();
    } else {
        expected = -1;
    }

    const auto actual = static_cast<ptrdiff_t>(input_haystack.find_first_not_of(value));
    assert(expected == actual);
}

template <class T>
void test_case_string_rfind_not_ch(const basic_string<T>& input_haystack, const T value) {
    ptrdiff_t expected;

    const auto expected_iter =
        last_known_good_find_last(input_haystack.begin(), input_haystack.end(), value, not_equal_to<>{});

    if (expected_iter != input_haystack.end()) {
        expected = expected_iter - input_haystack.begin();
    } else {
        expected = -1;
    }

    const auto actual = static_cast<ptrdiff_t>(input_haystack.find_last_not_of(value));
    assert(expected == actual);
}

template <class T, class D>
void test_basic_string_dis(mt19937_64& gen, D& dis) {
    basic_string<T> input_haystack;
    basic_string<T> input_haystack_not;
    basic_string<T> input_needle;
    basic_string<T> temp;
    input_haystack.reserve(haystackDataCount);
    input_haystack_not.reserve(haystackDataCount);
    input_needle.reserve(needleDataCount);
    temp.reserve(needleDataCount);

    for (;;) {
        const auto input_element = static_cast<T>(dis(gen));
        test_case_string_find_ch(input_haystack, input_element);
        test_case_string_rfind_ch(input_haystack, input_element);

        input_needle.clear();

        test_case_string_find_first_of(input_haystack, input_needle);
        test_case_string_find_last_of(input_haystack, input_needle);
        test_case_string_find_first_not_of(input_haystack, input_needle);
        test_case_string_find_last_not_of(input_haystack, input_needle);
        test_case_string_find_str(input_haystack, input_needle);
        test_case_string_rfind_str(input_haystack, input_needle);

        for (size_t attempts = 0; attempts < needleDataCount; ++attempts) {
            input_needle.push_back(static_cast<T>(dis(gen)));
            test_case_string_find_first_of(input_haystack, input_needle);
            test_case_string_find_last_of(input_haystack, input_needle);
            test_case_string_find_first_not_of(input_haystack, input_needle);
            test_case_string_find_last_not_of(input_haystack, input_needle);
            test_case_string_find_str(input_haystack, input_needle);
            test_case_string_rfind_str(input_haystack, input_needle);

            // For large needles the chance of a match is low, so test a guaranteed match
            if (input_haystack.size() > input_needle.size() * 2) {
                uniform_int_distribution<size_t> pos_dis(0, input_haystack.size() - input_needle.size());
                const size_t pos             = pos_dis(gen);
                const auto overwritten_first = input_haystack.begin() + static_cast<ptrdiff_t>(pos);
                temp.assign(overwritten_first, overwritten_first + static_cast<ptrdiff_t>(input_needle.size()));
                copy(input_needle.begin(), input_needle.end(), overwritten_first);
                test_case_string_find_str(input_haystack, input_needle);
                test_case_string_rfind_str(input_haystack, input_needle);
                copy(temp.begin(), temp.end(), overwritten_first);
            }
        }

        const auto input_not_ch = static_cast<T>(dis(gen));
        input_haystack_not.assign(input_haystack.size(), input_not_ch);

        test_case_string_find_not_ch(input_haystack_not, input_not_ch);
        test_case_string_rfind_not_ch(input_haystack_not, input_not_ch);
        if (!input_haystack_not.empty()) {
            uniform_int_distribution<size_t> not_pos_dis(0, input_haystack_not.size() - 1);

            for (size_t attempts = 0; attempts < needleDataCount; ++attempts) {
                const size_t pos        = not_pos_dis(gen);
                input_haystack_not[pos] = input_haystack[pos];
                test_case_string_find_not_ch(input_haystack_not, input_not_ch);
                test_case_string_rfind_not_ch(input_haystack_not, input_not_ch);
            }
        }

        if (input_haystack.size() == haystackDataCount) {
            break;
        }

        input_haystack.push_back(static_cast<T>(dis(gen)));
    }
}

template <class T>
void test_basic_string(mt19937_64& gen) {
    using dis_int_type = conditional_t<is_signed_v<T>, int32_t, uint32_t>;

    uniform_int_distribution<dis_int_type> dis_latin('a', 'z');
    test_basic_string_dis<T>(gen, dis_latin);
    if constexpr (sizeof(T) >= 2) {
        uniform_int_distribution<dis_int_type> dis_greek(0x391, 0x3C9);
        test_basic_string_dis<T>(gen, dis_greek);
    }
}

void test_string(mt19937_64& gen) {
    test_basic_string<char>(gen);
    test_basic_string<wchar_t>(gen);
#ifdef __cpp_lib_char8_t
    test_basic_string<char8_t>(gen);
#endif // __cpp_lib_char8_t
    test_basic_string<char16_t>(gen);
    test_basic_string<char32_t>(gen);
    test_basic_string<unsigned long long>(gen);
}

void test_various_containers() {
    test_one_container<vector<int>>(); // contiguous, vectorizable
    test_one_container<deque<int>>(); // random-access, not vectorizable
    test_one_container<list<int>>(); // bidi, not vectorizable
}

#if _HAS_CXX20
constexpr bool test_constexpr() {
    const int a[] = {20, 10, 30, 30, 30, 30, 40, 60, 50};

    assert(count(begin(a), end(a), 30) == 4);
    assert(ranges::count(a, 30) == 4);

    assert(find(begin(a), end(a), 30) == begin(a) + 2);
    assert(ranges::find(a, 30) == begin(a) + 2);

#if _HAS_CXX23
    assert(begin(ranges::find_last(a, 30)) == begin(a) + 5);
    assert(end(ranges::find_last(a, 30)) == end(a));
#endif // _HAS_CXX23

    assert(min_element(begin(a), end(a)) == begin(a) + 1);
    assert(max_element(begin(a), end(a)) == end(a) - 2);
    assert(get<0>(minmax_element(begin(a), end(a))) == begin(a) + 1);
    assert(get<1>(minmax_element(begin(a), end(a))) == end(a) - 2);

    assert(ranges::min_element(a) == begin(a) + 1);
    assert(ranges::max_element(a) == end(a) - 2);
    assert(ranges::minmax_element(a).min == begin(a) + 1);
    assert(ranges::minmax_element(a).max == end(a) - 2);

    assert(ranges::min(a) == 10);
    assert(ranges::max(a) == 60);
    assert(ranges::minmax(a).min == 10);
    assert(ranges::minmax(a).max == 60);

    int b[size(a)];
    reverse_copy(begin(a), end(a), begin(b));
    assert(equal(rbegin(a), rend(a), begin(b), end(b)));

    int c[size(a)];
    ranges::reverse_copy(a, c);
    assert(equal(rbegin(a), rend(a), begin(c), end(c)));

    reverse(begin(b), end(b));
    assert(equal(begin(a), end(a), begin(b), end(b)));

    swap_ranges(begin(b), end(b), begin(c));
    assert(equal(rbegin(a), rend(a), begin(b), end(b)));
    assert(equal(begin(a), end(a), begin(c), end(c)));

    ranges::swap_ranges(b, c);
    assert(equal(begin(a), end(a), begin(b), end(b)));
    assert(equal(rbegin(a), rend(a), begin(c), end(c)));

    ranges::reverse(c);
    assert(equal(begin(a), end(a), begin(c), end(c)));

    return true;
}

static_assert(test_constexpr());
#endif // _HAS_CXX20

int main() {
#if _HAS_CXX20
    assert(test_constexpr());
#endif // _HAS_CXX20
    run_randomized_tests_with_different_isa_levels([](mt19937_64& gen) {
        test_vector_algorithms(gen);
        test_various_containers();
        test_bitset(gen);
        test_string(gen);
    });
}
