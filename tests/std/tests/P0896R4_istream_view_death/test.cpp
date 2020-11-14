// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _CONTAINER_DEBUG_LEVEL 1

#include <cassert>
#include <cstddef>
#include <ranges>
#include <sstream>

#include <test_death.hpp>
using namespace std;

using iview = ranges::basic_istream_view<int, char>;

void test_preincrement_default_initialized() {
    ranges::iterator_t<iview> it;
    (void) ++it;
}

void test_postincrement_default_initialized() {
    ranges::iterator_t<iview> it;
    (void) it++;
}

void test_dereference_default_initialized() {
    ranges::iterator_t<iview> it;
    (void) *it;
}

void test_preincrement_no_stream() {
    iview v;
    auto it = v.begin();
    (void) ++it;
}

void test_postincrement_no_stream() {
    iview v;
    auto it = v.begin();
    (void) it++;
}

void test_dereference_no_stream() {
    iview v;
    auto it = v.begin();
    (void) *it;
}

void test_compare_no_stream() {
    iview v;
    auto it = v.begin();
    auto se = v.end();
    (void) (it == se);
}

void test_preincrement_end_of_stream() {
    istringstream stream;
    iview view{stream};
    auto it = view.begin();
    (void) ++it;
}

void test_postincrement_end_of_stream() {
    istringstream stream;
    iview view{stream};
    auto it = view.begin();
    (void) it++;
}

void test_dereference_end_of_stream() {
    istringstream stream;
    iview view{stream};
    auto it = view.begin();
    (void) *it;
}

int main(int argc, char* argv[]) {
    std_testing::death_test_executive exec;

#if _ITERATOR_DEBUG_LEVEL != 0
    exec.add_death_tests({
        test_preincrement_default_initialized,
        test_postincrement_default_initialized,
        test_dereference_default_initialized,
        test_preincrement_no_stream,
        test_postincrement_no_stream,
        test_dereference_no_stream,
        test_compare_no_stream,
        test_preincrement_end_of_stream,
        test_postincrement_end_of_stream,
        test_dereference_end_of_stream,
    });
#endif // _ITERATOR_DEBUG_LEVEL != 0

    return exec.run(argc, argv);
}
