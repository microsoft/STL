// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <assert.h>
#include <concepts>
#include <format>
#include <optional>
#include <stdio.h>
#include <string_view>

using namespace std;

// copied from the string_view tests
template <typename CharT>
struct choose_literal; // not defined

template <>
struct choose_literal<char> {
    static constexpr const char* choose(const char* s, const wchar_t*) {
        return s;
    }
};

template <>
struct choose_literal<wchar_t> {
    static constexpr const wchar_t* choose(const char*, const wchar_t* s) {
        return s;
    }
};

#define TYPED_LITERAL(CharT, Literal) (choose_literal<CharT>::choose(Literal, L##Literal))

template <typename CharT>
struct noop_testing_callbacks {
    constexpr void _On_align(_Align) {}
    constexpr void _On_fill(basic_string_view<CharT>) {}
    constexpr void _On_width(unsigned int) {}
    constexpr void _On_dynamic_width(size_t) {}
    constexpr void _On_dynamic_width(_Auto_id_tag) {}
    constexpr void _On_precision(unsigned int) {}
    constexpr void _On_dynamic_precision(size_t) {}
    constexpr void _On_dynamic_precision(_Auto_id_tag) {}
    constexpr void _On_sign(_Sign) {}
    constexpr void _On_hash() {}
    constexpr void _On_zero() {}
    constexpr void _On_localized() {}
    constexpr void _On_type(CharT) {}
};

template <typename CharT>
struct testing_callbacks {
    _Align expected_alignment = _Align::_None;
    _Sign expected_sign       = _Sign::_None;
    basic_string_view<CharT> expected_fill;
    int expected_width                   = -1;
    size_t expected_dynamic_width        = static_cast<size_t>(-1);
    bool expected_auto_dynamic_width     = false;
    int expected_precision               = -1;
    size_t expected_dynamic_precision    = static_cast<size_t>(-1);
    bool expected_auto_dynamic_precision = false;
    bool expected_hash                   = false;
    bool expected_zero                   = false;
    bool expected_localized              = false;
    CharT expected_type                  = '\0';
    constexpr void _On_align(_Align aln) {
        assert(aln == expected_alignment);
    }
    constexpr void _On_fill(basic_string_view<CharT> str_view) {
        assert(str_view == expected_fill);
    }
    constexpr void _On_width(int width) {
        assert(width == expected_width);
    }
    constexpr void _On_dynamic_width(size_t id) {
        assert(id == expected_dynamic_width);
    }
    constexpr void _On_dynamic_width(_Auto_id_tag) {
        assert(expected_auto_dynamic_width);
    }
    constexpr void _On_precision(int pre) {
        assert(pre == expected_precision);
    }
    constexpr void _On_dynamic_precision(size_t id) {
        assert(id == expected_dynamic_precision);
    }
    constexpr void _On_dynamic_precision(_Auto_id_tag) {
        assert(expected_auto_dynamic_precision);
    }
    constexpr void _On_sign(_Sign sgn) {
        assert(sgn == expected_sign);
    }
    constexpr void _On_hash() {
        assert(expected_hash);
    }
    constexpr void _On_zero() {
        assert(expected_zero);
    }
    constexpr void _On_localized() {
        assert(expected_localized);
    }
    constexpr void _On_type(CharT type) {
        assert(type == expected_type);
    }
};
template <typename CharT>
testing_callbacks(_Align, basic_string_view<CharT>) -> testing_callbacks<CharT>;

struct testing_arg_id_callbacks {
    constexpr void _On_auto_id() {}
    constexpr void _On_manual_id(size_t) {}
};

template <typename CharT, typename callback_type>
constexpr void test_parse_helper(const CharT* (*func)(const CharT*, const CharT*, callback_type&&),
    basic_string_view<CharT> view, bool err_expected = false,
    typename basic_string_view<CharT>::size_type expected_end_position = basic_string_view<CharT>::npos,
    callback_type&& callbacks                                          = {}) {
    try {
        auto end = func(view.data(), view.data() + view.size(), move(callbacks));
        if (expected_end_position != basic_string_view<CharT>::npos) {
            assert(end == view.data() + expected_end_position);
        }
        assert(!err_expected);
    } catch (const format_error&) {
        assert(err_expected);
    }
}

template <typename CharT>
constexpr bool test_parse_align() {
    auto parse_align_fn = _Parse_align<CharT, testing_callbacks<CharT>>;
    using view_typ      = basic_string_view<CharT>;

    view_typ s1(TYPED_LITERAL(CharT, "*<"));
    view_typ s2(TYPED_LITERAL(CharT, "*>"));
    view_typ s3(TYPED_LITERAL(CharT, "*^"));

    test_parse_helper(parse_align_fn, s1, false, view_typ::npos,
        {.expected_alignment = _Align::_Left, .expected_fill = view_typ(TYPED_LITERAL(CharT, "*"))});
    test_parse_helper(parse_align_fn, s2, false, view_typ::npos,
        {.expected_alignment = _Align::_Right, .expected_fill = view_typ(TYPED_LITERAL(CharT, "*"))});
    test_parse_helper(parse_align_fn, s3, false, view_typ::npos,
        {.expected_alignment = _Align::_Center, .expected_fill = view_typ(TYPED_LITERAL(CharT, "*"))});
    if constexpr (same_as<CharT, wchar_t>) {
        // This is a CJK character where the least significant byte is the same as ascii '>',
        // libfmt and initial drafts of <format> narrowed characters when parsing alignments, causing
        // \x343E (which is from CJK unified ideographs extension A) and similar characters to parse as
        // an alignment specifier.
        auto s4 = L"*\x343E"sv;
        test_parse_helper(parse_align_fn, s4, false, view_typ::npos, {.expected_fill = L"*"sv});
    }

    return true;
}

template <typename CharT>
constexpr bool test_parse_width() {
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
constexpr bool test_parse_arg_id() {
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
constexpr bool test_parse_precision() {
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
constexpr bool test_parse_format_specs() {
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
        {.expected_alignment = _Align::_Left,
            .expected_fill   = view_typ(TYPED_LITERAL(CharT, "*")),
            .expected_width  = 6});
    test_parse_helper(parse_format_specs_fn, s2, false, s2.size() - 1,
        {.expected_alignment = _Align::_Right,
            .expected_fill   = view_typ(TYPED_LITERAL(CharT, "*")),
            .expected_width  = 6});
    test_parse_helper(parse_format_specs_fn, s3, false, s3.size() - 1,
        {.expected_alignment = _Align::_Center,
            .expected_fill   = view_typ(TYPED_LITERAL(CharT, "*")),
            .expected_width  = 6});
    test_parse_helper(parse_format_specs_fn, s4, false, s4.size() - 1, {.expected_width = 6, .expected_type = 'd'});
    test_parse_helper(parse_format_specs_fn, s5, false, s5.size() - 1,
        {.expected_alignment    = _Align::_Center,
            .expected_sign      = _Sign::_Plus,
            .expected_fill      = view_typ(TYPED_LITERAL(CharT, "*")),
            .expected_width     = 4,
            .expected_precision = 4,
            .expected_type      = 'a'});
    test_parse_helper(parse_format_specs_fn, s6, false, s6.size() - 1,
        {.expected_alignment    = _Align::_Center,
            .expected_sign      = _Sign::_Plus,
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

int main() {
    test_parse_align<char>();
    test_parse_align<wchar_t>();
    static_assert(test_parse_align<char>());
    static_assert(test_parse_align<wchar_t>());

    test_parse_arg_id<char>();
    test_parse_arg_id<wchar_t>();
    static_assert(test_parse_arg_id<char>());
    static_assert(test_parse_arg_id<wchar_t>());

    test_parse_width<char>();
    test_parse_width<wchar_t>();
    static_assert(test_parse_width<char>());
    static_assert(test_parse_width<wchar_t>());

    test_parse_precision<char>();
    test_parse_precision<wchar_t>();
    static_assert(test_parse_precision<char>());
    static_assert(test_parse_precision<wchar_t>());

    test_parse_format_specs<char>();
    test_parse_format_specs<wchar_t>();
    static_assert(test_parse_format_specs<char>());
    static_assert(test_parse_format_specs<wchar_t>());

    test_specs_setter<char>();
    test_specs_setter<wchar_t>();
    static_assert(test_specs_setter<char>());
    static_assert(test_specs_setter<wchar_t>());

    test_specs_checker<char>();
    test_specs_checker<wchar_t>();
    static_assert(test_specs_checker<char>());
    static_assert(test_specs_checker<wchar_t>());

    return 0;
}
