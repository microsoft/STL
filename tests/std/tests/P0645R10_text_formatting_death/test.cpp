// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _CONTAINER_DEBUG_LEVEL 1

#include <cassert>
#include <format>
#include <string_view>

#include <test_death.hpp>
using namespace std;

void test_case_advance_no_range() {
    const auto format_string       = "First {} and second {} and third {}"sv;
    const auto other_format_string = "something"sv;
    basic_format_parse_context context{format_string};
    context.advance_to(other_format_string.begin());
}

void test_case_negative_dynamic_width() {
    (void) format("{:{}}", 42, -2);
}

int main(int argc, char* argv[]) {
    std_testing::death_test_executive exec;

#if _ITERATOR_DEBUG_LEVEL != 0
    exec.add_death_tests({
        test_case_advance_no_range,
        test_case_negative_dynamic_width,
    });
#endif // _ITERATOR_DEBUG_LEVEL != 0

    return exec.run(argc, argv);
}
