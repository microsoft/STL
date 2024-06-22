// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <isa_availability.h>
#include <limits>
#include <random>
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

#if (defined(_M_IX86) || defined(_M_X64)) && !defined(_M_CEE_PURE)
extern "C" long __isa_enabled;

void disable_instructions(ISA_AVAILABILITY isa) {
    __isa_enabled &= ~(1UL << static_cast<unsigned long>(isa));
}
#endif // (defined(_M_IX86) || defined(_M_X64)) && !defined(_M_CEE_PURE)

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

constexpr size_t dataCount = 1024;

template <class T>
void test_min_max_element_floating_with_values(mt19937_64& gen, const std::vector<T>& input_of_input) {
    uniform_int_distribution<size_t> idx_dis(0, input_of_input.size() - 1);

    vector<T> input;
    input.reserve(dataCount);
    test_case_min_max_element(input);
    for (size_t attempts = 0; attempts < dataCount; ++attempts) {
        input.push_back(input_of_input[idx_dis(gen)]);
        test_case_min_max_element(input);
    }
}

template <class T>
void test_min_max_element_floating_any(mt19937_64& gen) {
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

    test_min_max_element_floating_with_values(gen, input_of_input);
}

template <class T>
void test_min_max_element_floating_zero(mt19937_64& gen) {
    test_min_max_element_floating_with_values<T>(gen, {-0, +0});
    test_min_max_element_floating_with_values<T>(gen, {-0, +0, +1});
    test_min_max_element_floating_with_values<T>(gen, {-0, +0, -1});
}

template <class T>
void test_min_max_element_floating_zero_predef() {
    for (size_t len = 2; len != 16; ++len) {
        for (size_t pos = 0; pos != len; ++pos) {
            vector<T> v(len, +0.0);
            v[pos] = -0.0;
            test_case_min_max_element(v);

            for (size_t i = 0; i != pos; ++i) {
                v[i] = +1.0;
            }

            test_case_min_max_element(v);

            for (size_t i = 0; i != pos; ++i) {
                v[i] = -1, 0;
            }

            test_case_min_max_element(v);

            for (size_t i = 0; i != pos; ++i) {
                v[i] = +0.0;
            }

            for (size_t i = pos + 1; i != len; ++i) {
                v[i] = +1.0;
            }

            test_case_min_max_element(v);

            for (size_t i = pos + 1; i != len; ++i) {
                v[i] = -1.0;
            }
        }
    }
}

template <class T>
void test_min_max_element_floating(mt19937_64& gen) {
    test_min_max_element_floating_any<T>(gen);
    test_min_max_element_floating_zero<T>(gen);
    test_min_max_element_floating_zero_predef<T>();
}

void test_vector_algorithms(mt19937_64& gen) {
    test_min_max_element_floating<float>(gen);
    test_min_max_element_floating<double>(gen);
}

int main() {
    mt19937_64 gen;
    initialize_randomness(gen);

    test_vector_algorithms(gen);
#ifndef _M_CEE_PURE
#if defined(_M_IX86) || defined(_M_X64)
    disable_instructions(__ISA_AVAILABLE_AVX2);
    test_vector_algorithms(gen);

    disable_instructions(__ISA_AVAILABLE_SSE42);
    test_vector_algorithms(gen);
#endif // defined(_M_IX86) || defined(_M_X64)
#endif // _M_CEE_PURE
}
