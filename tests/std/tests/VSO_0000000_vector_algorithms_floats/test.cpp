// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cstddef>
#include <functional>
#include <limits>
#include <random>
#include <vector>

#include "test_is_sorted_until_support.hpp"
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
vector<T> test_floating_input(mt19937_64& gen) {
    normal_distribution<T> dis(0, 100000.0);

    constexpr auto input_of_input_size = dataCount / 2;
    vector<T> result(input_of_input_size);

    for (auto& element : result) {
        element = dis(gen);
    }

    result[0] = -0.0;
    result[1] = +0.0;
#ifndef _M_FP_FAST
    result[2] = -numeric_limits<T>::infinity();
    result[3] = +numeric_limits<T>::infinity();
#endif // !defined(_M_FP_FAST)
    return result;
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
    test_min_max_element_floating_with_values(gen, test_floating_input<T>(gen));
    test_min_max_element_floating_zero<T>(gen);
    test_min_max_element_floating_zero_predef<T>();
}

template <class T>
void test_is_sorted_until_floating_with_values(mt19937_64& gen, const vector<T>& input_of_input) {
    uniform_int_distribution<size_t> idx_dis(0, input_of_input.size() - 1);

    vector<T> original_input;
    vector<T> input;
    original_input.reserve(dataCount);
    input.reserve(dataCount);

    test_case_is_sorted_until(input, less<>{});
    test_case_is_sorted_until(input, greater<>{});

    for (size_t attempts = 0; attempts < dataCount; ++attempts) {
        original_input.push_back(input_of_input[idx_dis(gen)]);
        input = original_input;

        uniform_int_distribution<ptrdiff_t> pos_dis{0, static_cast<ptrdiff_t>(input.size() - 1)};
        auto it = input.begin() + pos_dis(gen);
        sort(input.begin(), it, less<>{});

        test_case_is_sorted_until(input, less<>{});
        reverse(input.begin(), it);
        test_case_is_sorted_until(input, greater<>{});
    }
}

void test_vector_algorithms(mt19937_64& gen) {
    test_min_max_element_floating<float>(gen);
    test_min_max_element_floating<double>(gen);

    test_is_sorted_until_floating_with_values(gen, test_floating_input<float>(gen));
    test_is_sorted_until_floating_with_values(gen, test_floating_input<double>(gen));
}

int main() {
    run_randomized_tests_with_different_isa_levels(test_vector_algorithms);
}
