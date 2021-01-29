// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <assert.h>
#include <concepts>
#include <format>
#include <string_view>

using namespace std;
using namespace literals;

template <class CharType>
constexpr auto get_input() {
    if constexpr (same_as<CharType, char>) {
        return "First {} and second {} and third {}"sv;
    } else {
        return L"First {} and second {} and third {}"sv;
    }
}

template <class CharType>
bool ensure_not_constant_expression() {
    basic_format_parse_context<CharType> context{get_input<CharType>()};
    context.check_arg_id(1);
    if (is_constant_evaluated()) {
        return false;
    } else {
        return true;
    }
}

const bool check_arg_id_not_constexpr_char  = ensure_not_constant_expression<char>();
const bool check_arg_id_not_constexpr_wchar = ensure_not_constant_expression<wchar_t>();

template <class CharType>
constexpr bool test_basic_format_parse_context() {
    static_assert(!is_copy_constructible_v<basic_format_parse_context<CharType>>);
    static_assert(!is_copy_assignable_v<basic_format_parse_context<CharType>>);

    const auto format_string = get_input<CharType>();
    { // iterator interface
        basic_format_parse_context<CharType> context{format_string};
        const same_as<typename basic_string_view<CharType>::const_iterator> auto b = context.begin();
        assert(b == format_string.begin());
        static_assert(noexcept(context.begin()));

        const same_as<typename basic_string_view<CharType>::const_iterator> auto e = context.end();
        assert(e == format_string.end());
        static_assert(noexcept(context.end()));

        const auto new_position = b + 5;
        context.advance_to(new_position);
        assert(to_address(context.begin()) == to_address(new_position));
        assert(to_address(context.end()) == to_address(e));
    }

    { // arg_id
        basic_format_parse_context<CharType> context{format_string};
        const same_as<size_t> auto first_arg_id = context.next_arg_id();
        assert(first_arg_id == 0);

        const same_as<size_t> auto second_arg_id = context.next_arg_id();
        assert(second_arg_id == 1);

        if (!is_constant_evaluated()) {
            try {
                context.check_arg_id(0);
            } catch (format_error e) {
                assert(e.what() == "Can not switch from automatic to manual indexing"sv);
            }
        }
    }

    { // check_arg_id
        basic_format_parse_context<CharType> context{format_string, 3};
        context.check_arg_id(1);
        context.check_arg_id(1);

        if (!is_constant_evaluated()) {
            try {
                context.next_arg_id();
            } catch (format_error e) {
                assert(e.what() == "Can not switch from manual to automatic indexing"sv);
            }
        }
    }

    return true;
}

int main() {
    test_basic_format_parse_context<char>();
    test_basic_format_parse_context<wchar_t>();
    static_assert(test_basic_format_parse_context<char>());
    static_assert(test_basic_format_parse_context<wchar_t>());

    assert(check_arg_id_not_constexpr_char);
    assert(check_arg_id_not_constexpr_wchar);
}
