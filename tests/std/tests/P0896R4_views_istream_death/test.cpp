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

void test_predecrement_default_initialized() {
    decltype(std::declval<iview>().begin()) it;
    (void) ++it;
}

void test_postdecrement_default_initialized() {
    decltype(std::declval<iview>().begin()) it;
    (void) it++;
}

void test_dereference_default_initialized() {
    decltype(std::declval<iview>().begin()) it;
    (void) *it;
}

void test_predecrement_no_stream() {
    iview v;
    auto it = v.begin();
    (void) ++it;
}

void test_postdecrement_no_stream() {
    iview v;
    auto it = v.begin();
    (void) it++;
}

void test_dereference_no_stream() {
    iview v;
    auto it = v.begin();
    (void) *it;
}


void test_predecrement_end_of_stream() {
    istringstream stream;
    iview view{stream};
    auto it = view.begin();
    (void) ++it;
}

void test_postdecrement_end_of_stream() {
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
    std_testing::death_test_executive exec([] {});

#if _ITERATOR_DEBUG_LEVEL != 0
    exec.add_death_tests({
        test_predecrement_default_initialized,
        test_postdecrement_default_initialized,
        test_dereference_default_initialized,
        test_predecrement_no_stream,
        test_postdecrement_no_stream,
        test_dereference_no_stream,
        test_predecrement_end_of_stream,
        test_postdecrement_end_of_stream,
        test_dereference_end_of_stream,
    });
#endif // _ITERATOR_DEBUG_LEVEL != 0

    return exec.run(argc, argv);
}
