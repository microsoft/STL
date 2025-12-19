// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <version>

void test_array();
void test_deque();
void test_flat_map();
void test_flat_set();
void test_forward_list();
#if 0 // TRANSITION, _HAS_CXX26
void test_hive();
void test_inplace_vector();
#endif // _HAS_CXX26
void test_list();
void test_map();
void test_optional();
void test_regex();
void test_set();
void test_span();
void test_stacktrace();
void test_string();
void test_string_view();
void test_unordered_map();
void test_unordered_set();
void test_valarray();
void test_vector();

int main() {
    test_array();
    test_deque();
    test_flat_map();
    test_flat_set();
    test_forward_list();
#if 0 // TRANSITION, _HAS_CXX26
    test_hive();
    test_inplace_vector();
#endif // _HAS_CXX26
    test_list();
    test_map();
    test_optional();
    test_regex();
    test_set();
    test_span();
    test_stacktrace();
    test_string();
    test_string_view();
    test_unordered_map();
    test_unordered_set();
    test_valarray();
    test_vector();
}
