// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <ranges>
#include <vector>

#include <test_death.hpp>
using namespace std;

template <class I>
void test_misordered_start_bound_int() {
    ranges::iota_view{I{42}, I{1}};
}

template <class UI>
void test_misordered_start_bound_uint_value_init() {
    ranges::iota_view<UI, UI>{UI{42}};
}

template <class T>
void test_misordered_start_bound_ptr() {
    T arr[1]{};
    ranges::iota_view{arr + 1, arr + 0};
}

template <class T>
void test_misordered_start_bound_vector_iter() {
    vector<T> vec(1);
    ranges::iota_view{vec.end(), vec.begin()};
}

int main(int argc, char* argv[]) {
    std_testing::death_test_executive exec;

#ifdef _DEBUG
    struct S {};

    exec.add_death_tests({
        test_misordered_start_bound_int<signed char>,
        test_misordered_start_bound_int<unsigned char>,
        test_misordered_start_bound_int<short>,
        test_misordered_start_bound_int<unsigned short>,
        test_misordered_start_bound_int<int>,
        test_misordered_start_bound_int<unsigned int>,
        test_misordered_start_bound_int<long>,
        test_misordered_start_bound_int<unsigned long>,
        test_misordered_start_bound_int<long long>,
        test_misordered_start_bound_int<unsigned long long>,

        test_misordered_start_bound_int<char>,
#ifdef __cpp_char8_t
        test_misordered_start_bound_int<char8_t>,
#endif // __cpp_char8_t
        test_misordered_start_bound_int<char16_t>,
        test_misordered_start_bound_int<char32_t>,
        test_misordered_start_bound_int<wchar_t>,

        test_misordered_start_bound_uint_value_init<unsigned char>,
        test_misordered_start_bound_uint_value_init<unsigned short>,
        test_misordered_start_bound_uint_value_init<unsigned int>,
        test_misordered_start_bound_uint_value_init<unsigned long>,
        test_misordered_start_bound_uint_value_init<unsigned long long>,
#ifdef __cpp_char8_t
        test_misordered_start_bound_uint_value_init<char8_t>,
#endif // __cpp_char8_t
        test_misordered_start_bound_uint_value_init<char16_t>,
        test_misordered_start_bound_uint_value_init<char32_t>,
        test_misordered_start_bound_uint_value_init<wchar_t>,

        test_misordered_start_bound_ptr<char>,
        test_misordered_start_bound_ptr<int>,
        test_misordered_start_bound_ptr<S>,

        test_misordered_start_bound_vector_iter<char>,
        test_misordered_start_bound_vector_iter<int>,
        test_misordered_start_bound_vector_iter<S>,
    });
#endif // _DEBUG

    return exec.run(argc, argv);
}
