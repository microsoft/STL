// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <random>
#include <type_traits>
#include <utility>
#include <vector>

#if _HAS_CXX20
#include <compare>
#include <ranges>
#endif // _HAS_CXX20

#include "test_vector_algorithms_support.hpp"

using namespace std;

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

void test_vector_algorithms(mt19937_64& gen) {
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
}

int main() {
    run_randomized_tests_with_different_isa_levels(test_vector_algorithms);
}
