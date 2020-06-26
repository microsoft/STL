// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <assert.h>
#include <format>
#include <string_view>

// smoke tests
/*
void fmt_parse_ctx_2() {
    const std::string_view test_str_view("this is a test");
    std::basic_format_parse_context test_parse_ctx(test_str_view, 4);
    assert(test_parse_ctx.begin() == test_str_view.begin());
    assert(test_parse_ctx.end() == test_str_view.end());
    assert(test_parse_ctx.next_arg_id() == 0);
}

void fmt_parse_ctx_3() {}
*/
void fmt_args() {
    using namespace std;
    using test_no_named_args_type = _Format_arg_store<void, int, char, int, char*>;
    assert(test_no_named_args_type::_Num_args == 4);
    assert(test_no_named_args_type::_Num_named_args == 0);
    using test_named_args_type = _Format_arg_store<void, int, _Named_arg<char, int>, char*>;
    assert(test_named_args_type::_Num_args == 3);
    assert(test_named_args_type::_Num_named_args == 1);
}

int main() {
    fmt_args();
    return 0;
}
