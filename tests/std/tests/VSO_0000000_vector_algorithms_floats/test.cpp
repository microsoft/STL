// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cstddef>
#include <limits>
#include <random>
#include <vector>

#include "test_min_max_element_support.hpp"
#include "test_vector_algorithms_support.hpp"

using namespace std;

template <class T>
void test_min_max_element_floating_with_values(mt19937_64& gen, const vector<T>& input_of_input) {
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

    for (auto& element : input_of_input) {
        element = dis(gen);
    }

    input_of_input[0] = -0.0;
    input_of_input[1] = +0.0;
#ifndef _M_FP_FAST
    input_of_input[2] = -numeric_limits<T>::infinity();
    input_of_input[3] = +numeric_limits<T>::infinity();
#endif // !defined(_M_FP_FAST)

    test_min_max_element_floating_with_values(gen, input_of_input);
}

template <class T>
void test_min_max_element_floating_zero(mt19937_64& gen) {
    test_min_max_element_floating_with_values<T>(gen, {-0.0, +0.0});
    test_min_max_element_floating_with_values<T>(gen, {-0.0, +0.0, +1.0});
    test_min_max_element_floating_with_values<T>(gen, {-0.0, +0.0, -1.0});
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
                v[i] = -1.0;
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
    run_randomized_tests_with_different_isa_levels(test_vector_algorithms);
}
