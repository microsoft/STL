// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <deque>
#include <functional>
#include <isa_availability.h>
#include <limits>
#include <list>
#include <random>
#include <type_traits>
#include <vector>

#ifdef __cpp_lib_concepts
#include <ranges>
#endif

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
inline ptrdiff_t last_known_good_count(FwdIt first, FwdIt last, T v) {
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
}

template <class FwdIt, class T>
inline auto last_known_good_find(FwdIt first, FwdIt last, T v) {
    for (; first != last; ++first) {
        if (*first == v) {
            break;
        }
    }
    return first;
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

template <class BidIt>
inline void last_known_good_reverse(BidIt first, BidIt last) {
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
inline FwdIt2 last_known_good_swap_ranges(FwdIt1 first1, const FwdIt1 last1, FwdIt2 dest) {
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

void test_various_containers() {
    test_one_container<vector<int>>(); // contiguous, vectorizable
    test_one_container<deque<int>>(); // random-access, not vectorizable
    test_one_container<list<int>>(); // bidi, not vectorizable
}

int main() {
    mt19937_64 gen;
    initialize_randomness(gen);

    test_vector_algorithms(gen);
    test_various_containers();
#ifndef _M_CEE_PURE
#if defined(_M_IX86) || defined(_M_X64)
    disable_instructions(__ISA_AVAILABLE_AVX2);
    test_vector_algorithms(gen);
    test_various_containers();

    disable_instructions(__ISA_AVAILABLE_SSE42);
    test_vector_algorithms(gen);
    test_various_containers();
#endif // defined(_M_IX86) || defined(_M_X64)
#if defined(_M_IX86)
    disable_instructions(__ISA_AVAILABLE_SSE2);
    test_vector_algorithms(gen);
    test_various_containers();
#endif // defined(_M_IX86)
#endif // _M_CEE_PURE
}
