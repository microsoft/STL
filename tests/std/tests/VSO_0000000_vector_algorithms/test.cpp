// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <assert.h>
#include <deque>
#include <isa_availability.h>
#include <list>
#include <random>
#include <vector>

using namespace std;

#if (defined(_M_IX86) || defined(_M_X64)) && !defined(_M_CEE_PURE)
extern "C" long __isa_enabled;

void disable_instructions(ISA_AVAILABILITY isa) {
    __isa_enabled &= ~(1UL << static_cast<unsigned long>(isa));
}
#endif // (defined(_M_IX86) || defined(_M_X64)) && !defined(_M_CEE_PURE)

constexpr size_t dataCount = 1024;

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

void test_vector_algorithms() {
    mt19937_64 gen(1729);
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
    test_vector_algorithms();
    test_various_containers();
#ifndef _M_CEE_PURE
#if defined(_M_IX86) || defined(_M_X64)
    disable_instructions(__ISA_AVAILABLE_AVX2);
    test_vector_algorithms();
    disable_instructions(__ISA_AVAILABLE_SSE42);
    test_vector_algorithms();
#endif // defined(_M_IX86) || defined(_M_X64)
#if defined(_M_IX86)
    disable_instructions(__ISA_AVAILABLE_SSE2);
    test_vector_algorithms();
#endif // defined(_M_IX86)
#endif // _M_CEE_PURE
}
