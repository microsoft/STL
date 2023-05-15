// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <concepts>
#include <cstdio>
#include <exception>
#include <format>
#include <optional>
#include <string_view>
#include <type_traits>

#include "test_format_support.hpp"

using namespace std;

template <typename CharT>
bool test_parse_align() {
    auto parse_align_fn = _Parse_align<CharT, testing_callbacks<CharT>>;
    using view_typ      = basic_string_view<CharT>;

    view_typ s1(TYPED_LITERAL(CharT, "*<"));
    view_typ s2(TYPED_LITERAL(CharT, "*>"));
    view_typ s3(TYPED_LITERAL(CharT, "*^"));

    test_parse_helper(parse_align_fn, s1, false, view_typ::npos,
        {.expected_alignment = _Fmt_align::_Left, .expected_fill = view_typ(TYPED_LITERAL(CharT, "*"))});
    test_parse_helper(parse_align_fn, s2, false, view_typ::npos,
        {.expected_alignment = _Fmt_align::_Right, .expected_fill = view_typ(TYPED_LITERAL(CharT, "*"))});
    test_parse_helper(parse_align_fn, s3, false, view_typ::npos,
        {.expected_alignment = _Fmt_align::_Center, .expected_fill = view_typ(TYPED_LITERAL(CharT, "*"))});

    if constexpr (same_as<CharT, wchar_t>) {
        // This is a CJK character where the least significant byte is the same as ASCII '>',
        // libfmt and initial drafts of <format> narrowed characters when parsing alignments, causing
        // \x343E (which is from CJK unified ideographs extension A) and similar characters to parse as
        // an alignment specifier.
        auto s4 = L"*\x343E"sv;
        test_parse_helper(parse_align_fn, s4, false, 0, {.expected_fill = L"*"sv});

        // test multi-code-unit fill characters
        {
            test_parse_helper(parse_align_fn, L"\U0001F3C8<X"sv, false, 3,
                {.expected_alignment = _Fmt_align::_Left, .expected_fill = L"\U0001F3C8"sv});
            test_parse_helper(parse_align_fn, L"\U0001F3C8>X"sv, false, 3,
                {.expected_alignment = _Fmt_align::_Right, .expected_fill = L"\U0001F3C8"sv});
            test_parse_helper(parse_align_fn, L"\U0001F3C8^X"sv, false, 3,
                {.expected_alignment = _Fmt_align::_Center, .expected_fill = L"\U0001F3C8"sv});
        }

        // test invalid fill characters
        {
            test_parse_helper(parse_align_fn, L"\xD800<X"sv, true);
            test_parse_helper(parse_align_fn, L"\xDC00<X"sv, true);
        }
    }

    return true;
}

template <typename CharT>
bool test_parse_width() {
    auto parse_width_fn = _Parse_width<CharT, testing_callbacks<CharT>>;
    using view_typ      = basic_string_view<CharT>;

    view_typ s0(TYPED_LITERAL(CharT, "1"));
    view_typ s1(TYPED_LITERAL(CharT, "{1}"));
    view_typ s2(TYPED_LITERAL(CharT, "{0}"));
    view_typ s3(TYPED_LITERAL(CharT, "{}"));
    view_typ i0(TYPED_LITERAL(CharT, "0"));
    view_typ i1(TYPED_LITERAL(CharT, "01"));

    test_parse_helper(parse_width_fn, s0, false, view_typ::npos, {.expected_width = 1});
    test_parse_helper(parse_width_fn, s1, false, view_typ::npos, {.expected_dynamic_width = 1});
    test_parse_helper(parse_width_fn, s2, false, view_typ::npos, {.expected_dynamic_width = 0});
    test_parse_helper(parse_width_fn, s3, false, view_typ::npos, {.expected_auto_dynamic_width = true});
    test_parse_helper(parse_width_fn, i0, false, false);
    test_parse_helper(parse_width_fn, i1, false, false);
    return true;
}

template <typename CharT>
bool test_parse_arg_id() {
    auto parse_arg_id_fn = _Parse_arg_id<CharT, testing_arg_id_callbacks>;
    using view_typ       = basic_string_view<CharT>;
    // note that parse arg id starts with the arg id itself, not the { beginning of the
    // format spec
    view_typ s0(TYPED_LITERAL(CharT, "}"));
    view_typ s1(TYPED_LITERAL(CharT, ":"));
    view_typ s2(TYPED_LITERAL(CharT, ":}"));
    view_typ s3(TYPED_LITERAL(CharT, "0:}"));
    view_typ s4(TYPED_LITERAL(CharT, "0:"));
    view_typ s5(TYPED_LITERAL(CharT, "1}"));
    view_typ i0(TYPED_LITERAL(CharT, "01}"));
    view_typ i1(TYPED_LITERAL(CharT, "0"));

    test_parse_helper(parse_arg_id_fn, s0, false, 0);
    test_parse_helper(parse_arg_id_fn, s1, false, 0);
    test_parse_helper(parse_arg_id_fn, s2, false, 0);
    test_parse_helper(parse_arg_id_fn, s3, false, 1);
    test_parse_helper(parse_arg_id_fn, s4, false, 1);
    test_parse_helper(parse_arg_id_fn, s5, false, 1);

    // can't test the expected exceptions in a constexpr
    // context
    if (!is_constant_evaluated()) {
        test_parse_helper(parse_arg_id_fn, i0, true);
        test_parse_helper(parse_arg_id_fn, i1, true);
    }

    return true;
}

template <typename CharT>
bool test_parse_precision() {
    auto parse_pre_fn = _Parse_precision<CharT, testing_callbacks<CharT>>;
    using view_typ    = basic_string_view<CharT>;

    view_typ s0(TYPED_LITERAL(CharT, ".0"));
    view_typ s1(TYPED_LITERAL(CharT, ".1"));
    view_typ s2(TYPED_LITERAL(CharT, ".12"));
    view_typ s3(TYPED_LITERAL(CharT, ".{1}"));
    view_typ s4(TYPED_LITERAL(CharT, ".{}"));

    view_typ i0(TYPED_LITERAL(CharT, ".{ }"));
    view_typ i1(TYPED_LITERAL(CharT, "."));
    view_typ i2(TYPED_LITERAL(CharT, ".{    |"));
    view_typ i3(TYPED_LITERAL(CharT, ".{"));

    test_parse_helper(parse_pre_fn, s0, false, view_typ::npos, {.expected_precision = 0});
    test_parse_helper(parse_pre_fn, s1, false, view_typ::npos, {.expected_precision = 1});
    test_parse_helper(parse_pre_fn, s2, false, view_typ::npos, {.expected_precision = 12});
    test_parse_helper(parse_pre_fn, s3, false, view_typ::npos, {.expected_dynamic_precision = 1});
    test_parse_helper(parse_pre_fn, s4, false, view_typ::npos, {.expected_auto_dynamic_precision = true});

    if (!is_constant_evaluated()) {
        test_parse_helper(parse_pre_fn, i0, true);
        test_parse_helper(parse_pre_fn, i1, true);
        test_parse_helper(parse_pre_fn, i2, true);
        test_parse_helper(parse_pre_fn, i3, true);
    }

    return true;
}

template <typename CharT>
bool test_parse_format_specs() {
    auto parse_format_specs_fn = _Parse_format_specs<CharT, testing_callbacks<CharT>>;
    using view_typ             = basic_string_view<CharT>;

    view_typ s0(TYPED_LITERAL(CharT, "6}"));
    view_typ s1(TYPED_LITERAL(CharT, "*<6"));
    view_typ s2(TYPED_LITERAL(CharT, "*>6}"));
    view_typ s3(TYPED_LITERAL(CharT, "*^6}"));
    view_typ s4(TYPED_LITERAL(CharT, "6d}"));
    view_typ s5(TYPED_LITERAL(CharT, "*^+4.4a}"));
    view_typ s6(TYPED_LITERAL(CharT, "*^+#04.4La}"));
    test_parse_helper(parse_format_specs_fn, s0, false, s0.size() - 1, {.expected_width = 6});
    test_parse_helper(parse_format_specs_fn, s1, false, s1.size(),
        {.expected_alignment = _Fmt_align::_Left,
            .expected_fill   = view_typ(TYPED_LITERAL(CharT, "*")),
            .expected_width  = 6});
    test_parse_helper(parse_format_specs_fn, s2, false, s2.size() - 1,
        {.expected_alignment = _Fmt_align::_Right,
            .expected_fill   = view_typ(TYPED_LITERAL(CharT, "*")),
            .expected_width  = 6});
    test_parse_helper(parse_format_specs_fn, s3, false, s3.size() - 1,
        {.expected_alignment = _Fmt_align::_Center,
            .expected_fill   = view_typ(TYPED_LITERAL(CharT, "*")),
            .expected_width  = 6});
    test_parse_helper(parse_format_specs_fn, s4, false, s4.size() - 1, {.expected_width = 6, .expected_type = 'd'});
    test_parse_helper(parse_format_specs_fn, s5, false, s5.size() - 1,
        {.expected_alignment    = _Fmt_align::_Center,
            .expected_sign      = _Fmt_sign::_Plus,
            .expected_fill      = view_typ(TYPED_LITERAL(CharT, "*")),
            .expected_width     = 4,
            .expected_precision = 4,
            .expected_type      = 'a'});
    test_parse_helper(parse_format_specs_fn, s6, false, s6.size() - 1,
        {.expected_alignment    = _Fmt_align::_Center,
            .expected_sign      = _Fmt_sign::_Plus,
            .expected_fill      = view_typ(TYPED_LITERAL(CharT, "*")),
            .expected_width     = 4,
            .expected_precision = 4,
            .expected_hash      = true,
            .expected_zero      = true,
            .expected_localized = true,
            .expected_type      = 'a'});
    return true;
}

template <class CharT>
constexpr bool test_specs_setter() {
    // just instantiate for now.
    _Basic_format_specs<CharT> specs = {};
    _Specs_setter<CharT> setter(specs);

    (void) setter;
    return true;
}

template <class CharT>
constexpr bool test_specs_checker() {
    _Specs_checker<noop_testing_callbacks<CharT>> checker(
        noop_testing_callbacks<CharT>{}, _Basic_format_arg_type::_Float_type);
    (void) checker;
    return true;
}

void test() {
    test_parse_align<char>();
    test_parse_align<wchar_t>();

    test_parse_arg_id<char>();
    test_parse_arg_id<wchar_t>();

    test_parse_width<char>();
    test_parse_width<wchar_t>();

    test_parse_precision<char>();
    test_parse_precision<wchar_t>();

    test_parse_format_specs<char>();
    test_parse_format_specs<wchar_t>();

    test_specs_setter<char>();
    test_specs_setter<wchar_t>();
    static_assert(test_specs_setter<char>());
    static_assert(test_specs_setter<wchar_t>());

    test_specs_checker<char>();
    test_specs_checker<wchar_t>();
    static_assert(test_specs_checker<char>());
    static_assert(test_specs_checker<wchar_t>());
}

int main() {
    try {
        test();
    } catch (const format_error& e) {
        printf("format_error: %s\n", e.what());
        assert(false);
    } catch (const exception& e) {
        printf("exception: %s\n", e.what());
        assert(false);
    }
}
