// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <assert.h>
#include <format>
#include <string>
#include <string_view>
#include <type_traits>
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

constexpr void test_disabled_formatter_is_disabled() {
    using F = formatter<void, void>;
    static_assert(!is_default_constructible_v<F>);
    static_assert(!is_copy_constructible_v<F>);
    static_assert(!is_move_constructible_v<F>);
    static_assert(!is_copy_assignable_v<F>);
    static_assert(!is_move_assignable_v<F>);
}

int main() {
    assert(format("{}", custom_formattable_type{"f"}) == "f"s);
    return 0;
}
