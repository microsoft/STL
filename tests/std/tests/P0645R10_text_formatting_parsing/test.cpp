// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <assert.h>
#include <concepts>
#include <format>
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

template <typename _CharT>
struct testing_callbacks {
    _Align expected_alignment = _Align::_None;
    basic_string_view<_CharT> expected_fill;
    int expected_width               = -1;
    int expected_dynamic_width       = -1;
    bool expected_auto_dynamic_width = false;
    constexpr void _On_align(_Align _Aln) {
        assert(_Aln == expected_alignment);
    }
    constexpr void _On_fill(basic_string_view<_CharT> _Sv) {
        assert(_Sv == expected_fill);
    }
    constexpr void _On_width(int width) {
        assert(width == expected_width);
    }
    constexpr void _On_dynamic_width(int id) {
        assert(id == expected_dynamic_width);
    }
    constexpr void _On_dynamic_width(_Auto_id_tag) {
        assert(expected_auto_dynamic_width);
    }
};
template <typename _CharT>
testing_callbacks(_Align, basic_string_view<_CharT>) -> testing_callbacks<_CharT>;

struct testing_arg_id_callbacks {
    constexpr void _On_auto_id() {}
    constexpr void _On_manual_id(int) {}
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

    auto s0 = view_typ(TYPED_LITERAL(CharT, ""));
    auto s2 = view_typ(TYPED_LITERAL(CharT, "*<"));
    auto s3 = view_typ(TYPED_LITERAL(CharT, "*>"));
    auto s4 = view_typ(TYPED_LITERAL(CharT, "*^"));

    test_parse_helper(parse_align_fn, s0, false, view_typ::npos, {_Align::_None, view_typ(TYPED_LITERAL(CharT, ""))});
    test_parse_helper(parse_align_fn, s2, false, view_typ::npos, {_Align::_Left, view_typ(TYPED_LITERAL(CharT, "*"))});
    test_parse_helper(parse_align_fn, s3, false, view_typ::npos, {_Align::_Right, view_typ(TYPED_LITERAL(CharT, "*"))});
    test_parse_helper(
        parse_align_fn, s4, false, view_typ::npos, {_Align::_Center, view_typ(TYPED_LITERAL(CharT, "*"))});
    return true;
}

template <typename CharT>
constexpr bool test_parse_width() {
    auto parse_width_fn = _Parse_width<CharT, testing_callbacks<CharT>>;
    using view_typ      = basic_string_view<CharT>;

    auto s0 = view_typ(TYPED_LITERAL(CharT, "1"));
    auto s1 = view_typ(TYPED_LITERAL(CharT, "{1}"));
    auto s2 = view_typ(TYPED_LITERAL(CharT, "{0}"));
    auto s3 = view_typ(TYPED_LITERAL(CharT, "{}"));
    auto i0 = view_typ(TYPED_LITERAL(CharT, "0"));
    auto i1 = view_typ(TYPED_LITERAL(CharT, "01"));

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
    auto s0 = view_typ(TYPED_LITERAL(CharT, "}"));
    auto s1 = view_typ(TYPED_LITERAL(CharT, ":"));
    auto s2 = view_typ(TYPED_LITERAL(CharT, ":}"));
    auto s3 = view_typ(TYPED_LITERAL(CharT, "0:}"));
    auto s4 = view_typ(TYPED_LITERAL(CharT, "0:"));
    auto s5 = view_typ(TYPED_LITERAL(CharT, "1}"));
    auto i0 = view_typ(TYPED_LITERAL(CharT, "01}"));
    auto i1 = view_typ(TYPED_LITERAL(CharT, "0"));

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
    return 0;
}
