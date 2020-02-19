// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <assert.h>
#include <isa_availability.h>
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

    test_swap_ranges<char>(gen);
    test_swap_ranges<short>(gen);
    test_swap_ranges<int>(gen);
    test_swap_ranges<unsigned int>(gen);
    test_swap_ranges<unsigned long long>(gen);
}

int main() {
    test_vector_algorithms();
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
