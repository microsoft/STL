// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <assert.h>
#include <format>
#include <string_view>
using namespace std;

struct custom_formattable_type {
    string_view string_content;
};

template <>
struct std::formatter<custom_formattable_type, char> {
    basic_format_parse_context<char>::iterator parse(basic_format_parse_context<char>& parse_ctx) {
        if (parse_ctx.begin() != parse_ctx.end()) {
            throw format_error{"only empty specs please"};
        }
        return parse_ctx.end();
    }
    format_context::iterator format(const custom_formattable_type& val, format_context& ctx) {
        ctx.advance_to(copy(val.string_content.begin(), val.string_content.end(), ctx.out()));
        return ctx.out();
    }
};

int main() {
    assert(format("{}", custom_formattable_type{"f"}) == "f"s);
    return 0;
}
