// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <bitset>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <deque>
#include <functional>
#include <isa_availability.h>
#include <limits>
#include <list>
#include <random>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#if _HAS_CXX20
#include <compare>
#include <ranges>
#endif // _HAS_CXX20

#include "test_min_max_element_support.hpp"

using namespace std;

#pragma warning(disable : 4984) // 'if constexpr' is a C++17 language extension
#ifdef __clang__
#pragma clang diagnostic ignored "-Wc++17-extensions" // constexpr if is a C++17 extension
#endif // __clang__

void initialize_randomness(mt19937_64& gen) {
    constexpr size_t n = mt19937_64::state_size;
    constexpr size_t w = mt19937_64::word_size;
    static_assert(w % 32 == 0, "w should be evenly divisible by 32");
    constexpr size_t k = w / 32;

    vector<uint32_t> vec(n * k);

    random_device rd;
    generate(vec.begin(), vec.end(), ref(rd));

    printf("This is a randomized test.\n");
    printf("DO NOT IGNORE/RERUN ANY FAILURES.\n");
    printf("You must report them to the STL maintainers.\n\n");

    printf("Seed vector: ");
    for (const auto& e : vec) {
        printf("%u,", e);
    }
    printf("\n");

    seed_seq seq(vec.cbegin(), vec.cend());
    gen.seed(seq);
}

#if (defined(_M_IX86) || defined(_M_X64)) && !defined(_M_CEE_PURE)
extern "C" long __isa_enabled;

void disable_instructions(ISA_AVAILABILITY isa) {
    __isa_enabled &= ~(1UL << static_cast<unsigned long>(isa));
}
#endif // (defined(_M_IX86) || defined(_M_X64)) && !defined(_M_CEE_PURE)

constexpr size_t dataCount = 1024;

template <class FwdIt, class T>
ptrdiff_t last_known_good_count(FwdIt first, FwdIt last, T v) {
    ptrdiff_t result = 0;
    for (; first != last; ++first) {
        result += (*first == v);
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

template <class FwdIt, class T>
auto last_known_good_find(FwdIt first, FwdIt last, T v) {
    for (; first != last; ++first) {
        if (*first == v) {
            break;
        }
    }
    return first;
}

template <class FwdIt, class T>
auto last_known_good_find_last(FwdIt first, FwdIt last, T v) {
    FwdIt last_save = last;
    for (;;) {
        if (last == first) {
            return last_save;
        }
        --last;
        if (*last == v) {
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
    const auto n_len = n_last - n_first;

    for (; h_last - h_first >= n_len; ++h_first) {
        if (equal(h_first, h_first + n_len, n_first, n_last)) {
            return h_first;
        }
    }

    return h_last;
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

template <class T>
void test_find_first_of(mt19937_64& gen) {
    constexpr size_t haystackDataCount = 200;
    constexpr size_t needleDataCount   = 35;
    using TD                           = conditional_t<sizeof(T) == 1, int, T>;
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
    auto expected =
        last_known_good_search(input_haystack.begin(), input_haystack.end(), input_needle.begin(), input_needle.end());
    auto actual = search(input_haystack.begin(), input_haystack.end(), input_needle.begin(), input_needle.end());
    assert(expected == actual);
#if _HAS_CXX17
    auto searcher_actual = search(
        input_haystack.begin(), input_haystack.end(), default_searcher{input_needle.begin(), input_needle.end()});
    assert(expected == searcher_actual);
#endif // _HAS_CXX17
#if _HAS_CXX20
    auto ranges_actual = ranges::search(input_haystack, input_needle);
    assert(expected == begin(ranges_actual));
    if (expected != input_haystack.end()) {
        assert(expected + static_cast<ptrdiff_t>(input_needle.size()) == end(ranges_actual));
    } else {
        assert(expected == end(ranges_actual));
    }
#endif // _HAS_CXX20
}

template <class T>
void test_search(mt19937_64& gen) {
    constexpr size_t haystackDataCount = 200;
    constexpr size_t needleDataCount   = 35;
    using TD                           = conditional_t<sizeof(T) == 1, int, T>;
    uniform_int_distribution<TD> dis('0', '9');
    vector<T> input_haystack;
    vector<T> input_needle;
    input_haystack.reserve(haystackDataCount);
    input_needle.reserve(needleDataCount);

    for (;;) {
        input_needle.clear();

        test_case_search(input_haystack, input_needle);
        for (size_t attempts = 0; attempts < needleDataCount; ++attempts) {
            input_needle.push_back(static_cast<T>(dis(gen)));
            test_case_search(input_haystack, input_needle);
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

template <class T>
void test_min_max_element_floating(mt19937_64& gen) {
    normal_distribution<T> dis(-100000.0, 100000.0);

    constexpr auto input_of_input_size = dataCount / 2;
    vector<T> input_of_input(input_of_input_size);
    input_of_input[0] = -numeric_limits<T>::infinity();
    input_of_input[1] = +numeric_limits<T>::infinity();
    input_of_input[2] = -0.0;
    input_of_input[3] = +0.0;
    for (size_t i = 4; i < input_of_input_size; ++i) {
        input_of_input[i] = dis(gen);
    }

    uniform_int_distribution<size_t> idx_dis(0, input_of_input_size - 1);

    vector<T> input;
    input.reserve(dataCount);
    test_case_min_max_element(input);
    for (size_t attempts = 0; attempts < dataCount; ++attempts) {
        input.push_back(input_of_input[idx_dis(gen)]);
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

template <class FwdIt>
auto last_known_good_mismatch(FwdIt first1, FwdIt last1, FwdIt first2, FwdIt last2) {
    for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
        if (*first1 != *first2) {
            break;
        }
    }

    return make_pair(first1, first2);
}

template <class FwdIt>
bool last_known_good_lex_compare(pair<FwdIt, FwdIt> expected_mismatch, FwdIt last1, FwdIt last2) {
    if (expected_mismatch.second == last2) {
        return false;
    } else if (expected_mismatch.first == last1) {
        return true;
    } else if (*expected_mismatch.first < *expected_mismatch.second) {
        return true;
    } else {
        assert(*expected_mismatch.second < *expected_mismatch.first);
        return false;
    }
}

#if _HAS_CXX20
template <class FwdIt>
auto last_known_good_lex_compare_3way(pair<FwdIt, FwdIt> expected_mismatch, FwdIt last1, FwdIt last2) {
    if (expected_mismatch.second == last2) {
        if (expected_mismatch.first == last1) {
            return strong_ordering::equal;
        } else {
            return strong_ordering::greater;
        }
    } else if (expected_mismatch.first == last1) {
        return strong_ordering::less;
    } else {
        auto order = *expected_mismatch.first <=> *expected_mismatch.second;
        assert(order != 0);
        return order;
    }
}
#endif // _HAS_CXX20

template <class T>
void test_case_mismatch_and_lex_compare_family(const vector<T>& a, const vector<T>& b) {
    auto expected_mismatch = last_known_good_mismatch(a.begin(), a.end(), b.begin(), b.end());
    auto actual_mismatch   = mismatch(a.begin(), a.end(), b.begin(), b.end());
    assert(expected_mismatch == actual_mismatch);

    auto expected_lex = last_known_good_lex_compare(expected_mismatch, a.end(), b.end());
    auto actual_lex   = lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
    assert(expected_lex == actual_lex);

#if _HAS_CXX20
    auto ranges_actual_mismatch = ranges::mismatch(a, b);
    assert(get<0>(expected_mismatch) == ranges_actual_mismatch.in1);
    assert(get<1>(expected_mismatch) == ranges_actual_mismatch.in2);

    auto ranges_actual_lex = ranges::lexicographical_compare(a, b);
    assert(expected_lex == ranges_actual_lex);

    auto expected_lex_3way = last_known_good_lex_compare_3way(expected_mismatch, a.end(), b.end());
    auto actual_lex_3way   = lexicographical_compare_three_way(a.begin(), a.end(), b.begin(), b.end());
    assert(expected_lex_3way == actual_lex_3way);
#endif // _HAS_CXX20
}

template <class T>
void test_mismatch_and_lex_compare_family(mt19937_64& gen) {
    constexpr size_t shrinkCount   = 4;
    constexpr size_t mismatchCount = 10;
    using TD                       = conditional_t<sizeof(T) == 1, int, T>;
    uniform_int_distribution<TD> dis('a', 'z');
    vector<T> input_a;
    vector<T> input_b;
    input_a.reserve(dataCount);
    input_b.reserve(dataCount);

    for (;;) {
        // equal
        test_case_mismatch_and_lex_compare_family(input_a, input_b);

        // different sizes
        for (size_t i = 0; i != shrinkCount && !input_b.empty(); ++i) {
            input_b.pop_back();
            test_case_mismatch_and_lex_compare_family(input_a, input_b);
            test_case_mismatch_and_lex_compare_family(input_b, input_a);
        }

        // actual mismatch (or maybe not, depending on random)
        if (!input_b.empty()) {
            uniform_int_distribution<size_t> mismatch_dis(0, input_a.size() - 1);

            for (size_t attempts = 0; attempts < mismatchCount; ++attempts) {
                const size_t possible_mismatch_pos = mismatch_dis(gen);
                input_a[possible_mismatch_pos]     = static_cast<T>(dis(gen));
                test_case_mismatch_and_lex_compare_family(input_a, input_b);
                test_case_mismatch_and_lex_compare_family(input_b, input_a);
            }
        }

        if (input_a.size() == dataCount) {
            break;
        }

        input_a.push_back(static_cast<T>(dis(gen)));
        input_b = input_a;
    }
}

template <class C1, class C2>
void test_mismatch_and_lex_compare_family_containers() {
    C1 a{'m', 'e', 'o', 'w', ' ', 'C', 'A', 'T', 'S'};
    C2 b{'m', 'e', 'o', 'w', ' ', 'K', 'I', 'T', 'T', 'E', 'N', 'S'};

    const auto result_mismatch_4 = mismatch(a.begin(), a.end(), b.begin(), b.end());
    const auto result_mismatch_3 = mismatch(a.begin(), a.end(), b.begin());
    assert(get<0>(result_mismatch_4) == a.begin() + 5);
    assert(get<1>(result_mismatch_4) == b.begin() + 5);
    assert(get<0>(result_mismatch_3) == a.begin() + 5);
    assert(get<1>(result_mismatch_3) == b.begin() + 5);

    const auto result_lex = lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
    assert(result_lex == true);

#if _HAS_CXX20
    const auto result_mismatch_r = ranges::mismatch(a, b);
    assert(result_mismatch_r.in1 == a.begin() + 5);
    assert(result_mismatch_r.in2 == b.begin() + 5);

    const auto result_lex_r = ranges::lexicographical_compare(a, b);
    assert(result_lex_r == true);

    const auto result_lex_3way = lexicographical_compare_three_way(a.begin(), a.end(), b.begin(), b.end());
    assert(result_lex_3way == strong_ordering::less);
#endif // _HAS_CXX20
}

namespace test_mismatch_sizes_and_alignments {
    constexpr size_t range     = 33;
    constexpr size_t alignment = 32;

#pragma pack(push, 1)
    template <class T, size_t Size, size_t PadSize>
    struct with_pad {
        char p[PadSize];
        T v[Size];
    };
#pragma pack(pop)

    template <class T, size_t Size, size_t PadSize>
    char stack_array_various_alignments_impl() {
        with_pad<T, Size + 1, PadSize + 1> a = {};
        with_pad<T, Size + 1, PadSize + 1> b = {};
        assert(mismatch(begin(a.v), end(a.v), begin(b.v), end(b.v)) == make_pair(end(a.v), end(b.v)));
        return 0;
    }

    template <class T, size_t Size, size_t... PadSizes>
    void stack_array_various_alignments(index_sequence<PadSizes...>) {
        char ignored[] = {stack_array_various_alignments_impl<T, Size, PadSizes>()...};
        (void) ignored;
    }

    template <class T, size_t Size>
    char stack_array_impl() {
        T a[Size + 1] = {};
        T b[Size + 1] = {};
        assert(mismatch(begin(a), end(a), begin(b), end(b)) == make_pair(end(a), end(b)));
        stack_array_various_alignments<T, Size>(make_index_sequence<alignment>{});
        return 0;
    }

    template <class T, size_t... Sizes>
    void stack_array(index_sequence<Sizes...>) {
        char ignored[] = {stack_array_impl<T, Sizes>()...};
        (void) ignored;
    }

    template <class T>
    void test() {
        // stack with different sizes and alignments. ASan would catch out-of-range reads
        stack_array<T>(make_index_sequence<range>{});

        // vector with different sizes. ASan vector annotations would catch out-of-range reads
        for (size_t i = 0; i != range; ++i) {
            vector<T> a(i, 0);
            vector<T> b(i, 0);
            assert(mismatch(begin(a), end(a), begin(b), end(b)) == make_pair(end(a), end(b)));
        }

        // heap with different sizes. ASan would catch out-of-range reads
        for (size_t i = 0; i != range; ++i) {
            T* a = static_cast<T*>(calloc(i, sizeof(T)));
            T* b = static_cast<T*>(calloc(i, sizeof(T)));
            assert(mismatch(a, a + i, b, b + i) == make_pair(a + i, b + i));
            free(a);
            free(b);
        }

        // subarray from stack array. We would have wrong results if we run out of the range.
        T a[range + 1] = {};
        T b[range + 1] = {};
        for (size_t i = 0; i != range; ++i) {
            a[i + 1] = 1;
            // whole range mismatch finds mismatch after past-the-end of the subarray
            assert(mismatch(a, a + range + 1, b, b + range + 1) == make_pair(a + i + 1, b + i + 1));
            // limited range mismatch gets to past-the-end of the subarray
            assert(mismatch(a, a + i, b, b + i) == make_pair(a + i, b + i));
            a[i + 1] = 0;
        }
    }
} // namespace test_mismatch_sizes_and_alignments

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

template <class FwdIt1, class FwdIt2>
FwdIt2 last_known_good_swap_ranges(FwdIt1 first1, const FwdIt1 last1, FwdIt2 dest) {
    for (; first1 != last1; ++first1, ++dest) {
        iter_swap(first1, dest);
    }

    return dest;
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

void test_vector_algorithms(mt19937_64& gen) {
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

    test_min_max_element_floating<float>(gen);
    test_min_max_element_floating<double>(gen);
    test_min_max_element_floating<long double>(gen);

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

    test_mismatch_and_lex_compare_family<char>(gen);
    test_mismatch_and_lex_compare_family<signed char>(gen);
    test_mismatch_and_lex_compare_family<unsigned char>(gen);
    test_mismatch_and_lex_compare_family<short>(gen);
    test_mismatch_and_lex_compare_family<unsigned short>(gen);
    test_mismatch_and_lex_compare_family<int>(gen);
    test_mismatch_and_lex_compare_family<unsigned int>(gen);
    test_mismatch_and_lex_compare_family<long long>(gen);
    test_mismatch_and_lex_compare_family<unsigned long long>(gen);

    test_mismatch_and_lex_compare_family_containers<vector<char>, vector<signed char>>();
    test_mismatch_and_lex_compare_family_containers<vector<char>, vector<unsigned char>>();
    test_mismatch_and_lex_compare_family_containers<vector<wchar_t>, vector<char>>();
    test_mismatch_and_lex_compare_family_containers<const vector<char>, const vector<char>>();
    test_mismatch_and_lex_compare_family_containers<vector<char>, const vector<char>>();
    test_mismatch_and_lex_compare_family_containers<const vector<wchar_t>, vector<wchar_t>>();
    test_mismatch_and_lex_compare_family_containers<vector<char>, vector<int>>();

    test_mismatch_sizes_and_alignments::test<char>();
    test_mismatch_sizes_and_alignments::test<short>();
    test_mismatch_sizes_and_alignments::test<int>();
    test_mismatch_sizes_and_alignments::test<long long>();

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

    test_swap_ranges<char>(gen);
    test_swap_ranges<short>(gen);
    test_swap_ranges<int>(gen);
    test_swap_ranges<unsigned int>(gen);
    test_swap_ranges<unsigned long long>(gen);
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

    test_randomized_bitset_base_count<512 - 5, 32 + 10>(gen);
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

    mt19937_64 gen;
    initialize_randomness(gen);

    test_vector_algorithms(gen);
    test_various_containers();
    test_bitset(gen);
#ifndef _M_CEE_PURE
#if defined(_M_IX86) || defined(_M_X64)
    disable_instructions(__ISA_AVAILABLE_AVX2);
    test_vector_algorithms(gen);
    test_various_containers();
    test_bitset(gen);

    disable_instructions(__ISA_AVAILABLE_SSE42);
    test_vector_algorithms(gen);
    test_various_containers();
    test_bitset(gen);
#endif // defined(_M_IX86) || defined(_M_X64)
#endif // _M_CEE_PURE
}
