// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <assert.h>
#include <chrono>
#include <concepts>
#include <format>
#include <iostream>
#include <stdio.h>
#include <string_view>
#include <type_traits>
#include <utility>

using namespace std;
using namespace chrono;

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
struct testing_callbacks {
    _Align expected_alignment = _Align::_None;
    basic_string_view<CharT> expected_fill;
    int expected_width                   = -1;
    size_t expected_dynamic_width        = static_cast<size_t>(-1);
    bool expected_auto_dynamic_width     = false;
    int expected_precision               = -1;
    size_t expected_dynamic_precision    = static_cast<size_t>(-1);
    bool expected_auto_dynamic_precision = false;
    vector<_Chrono_specs<CharT>>& expected_chrono_specs;
    size_t curr_index = 0;

    void _On_align(_Align aln) {
        assert(aln == expected_alignment);
    }
    void _On_fill(basic_string_view<CharT> str_view) {
        assert(str_view == expected_fill);
    }
    void _On_width(int width) {
        assert(width == expected_width);
    }
    void _On_dynamic_width(size_t id) {
        assert(id == expected_dynamic_width);
    }
    void _On_dynamic_width(_Auto_id_tag) {
        assert(expected_auto_dynamic_width);
    }
    void _On_precision(int pre) {
        assert(pre == expected_precision);
    }
    void _On_dynamic_precision(size_t id) {
        assert(id == expected_dynamic_precision);
    }
    void _On_dynamic_precision(_Auto_id_tag) {
        assert(expected_auto_dynamic_precision);
    }
    void _On_conversion_spec(CharT mod, CharT type) {
        assert(static_cast<char>(mod) == expected_chrono_specs[curr_index]._Modifier);
        assert(static_cast<char>(type) == expected_chrono_specs[curr_index]._Type);
        assert(expected_chrono_specs[curr_index]._Lit_char == CharT{0}); // not set
        ++curr_index;
    }
    void _On_lit_char(CharT ch) {
        assert(ch == expected_chrono_specs[curr_index]._Lit_char);
        assert(expected_chrono_specs[curr_index]._Modifier == '\0'); // not set
        assert(expected_chrono_specs[curr_index]._Type == '\0'); // not set
        ++curr_index;
    }
};

template <typename CharT, typename callback_type>
void test_parse_helper(const CharT* (*func)(const CharT*, const CharT*, callback_type&&), basic_string_view<CharT> view,
    bool err_expected                                                  = false,
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
bool test_parse_conversion_spec() {
    auto parse_conv_spec_fn = _Parse_conversion_specs<CharT, testing_callbacks<CharT>>;
    using view_typ          = basic_string_view<CharT>;
    using chrono_spec       = _Chrono_specs<CharT>;

    view_typ s0(TYPED_LITERAL(CharT, "B"));
    view_typ s1(TYPED_LITERAL(CharT, "Ec"));
    view_typ s2(TYPED_LITERAL(CharT, "Od"));
    view_typ s3(TYPED_LITERAL(CharT, "E"));
    view_typ s4(TYPED_LITERAL(CharT, ""));
    view_typ s5(TYPED_LITERAL(CharT, "}"));
    view_typ s6(TYPED_LITERAL(CharT, "E}"));

    vector<chrono_spec> v0{{._Type = 'B'}};
    test_parse_helper(parse_conv_spec_fn, s0, false, view_typ::npos, {.expected_chrono_specs = v0});

    vector<chrono_spec> v1{{._Modifier = 'E', ._Type = 'c'}};
    test_parse_helper(parse_conv_spec_fn, s1, false, view_typ::npos, {.expected_chrono_specs = v1});

    vector<chrono_spec> v2{{._Modifier = 'O', ._Type = 'd'}};
    test_parse_helper(parse_conv_spec_fn, s2, false, view_typ::npos, {.expected_chrono_specs = v2});

    vector<chrono_spec> v{};
    test_parse_helper(parse_conv_spec_fn, s3, true, view_typ::npos, {.expected_chrono_specs = v});
    test_parse_helper(parse_conv_spec_fn, s4, true, view_typ::npos, {.expected_chrono_specs = v});
    test_parse_helper(parse_conv_spec_fn, s5, true, view_typ::npos, {.expected_chrono_specs = v});
    test_parse_helper(parse_conv_spec_fn, s6, true, view_typ::npos, {.expected_chrono_specs = v});

    return true;
}

template <typename CharT>
bool test_parse_chrono_format_specs() {
    auto parse_chrono_format_specs_fn = _Parse_chrono_format_specs<CharT, testing_callbacks<CharT>>;
    using view_typ                    = basic_string_view<CharT>;
    using chrono_spec                 = _Chrono_specs<CharT>;

    view_typ s0(TYPED_LITERAL(CharT, "%Oe"));
    view_typ s1(TYPED_LITERAL(CharT, "lit"));
    view_typ s2(TYPED_LITERAL(CharT, "%H:%M}"));
    view_typ s3(TYPED_LITERAL(CharT, "6%H}"));
    view_typ s4(TYPED_LITERAL(CharT, "*<6hi"));
    view_typ s5(TYPED_LITERAL(CharT, "*^4.4%ymm"));
    view_typ s6(TYPED_LITERAL(CharT, "%H%"));
    view_typ s7(TYPED_LITERAL(CharT, "%H%}"));
    view_typ s8(TYPED_LITERAL(CharT, "A%nB%tC%%D"));

    vector<chrono_spec> v0{{._Modifier = 'O', ._Type = 'e'}};
    test_parse_helper(parse_chrono_format_specs_fn, s0, false, s0.size(), {.expected_chrono_specs = v0});

    vector<chrono_spec> v1{{._Lit_char = 'l'}, {._Lit_char = 'i'}, {._Lit_char = 't'}};
    test_parse_helper(parse_chrono_format_specs_fn, s1, false, s1.size(), {.expected_chrono_specs = v1});

    vector<chrono_spec> v2{{._Type = 'H'}, {._Lit_char = ':'}, {._Type = 'M'}};
    test_parse_helper(parse_chrono_format_specs_fn, s2, false, s2.size() - 1, {.expected_chrono_specs = v2});

    vector<chrono_spec> v3{{._Type = 'H'}};
    test_parse_helper(
        parse_chrono_format_specs_fn, s3, false, s3.size() - 1, {.expected_width = 6, .expected_chrono_specs = v3});

    vector<chrono_spec> v8{{._Lit_char = 'A'}, {._Lit_char = '\n'}, {._Lit_char = 'B'}, {._Lit_char = '\t'},
        {._Lit_char = 'C'}, {._Lit_char = '%'}, {._Lit_char = 'D'}};
    test_parse_helper(parse_chrono_format_specs_fn, s8, false, s8.size(), {.expected_chrono_specs = v8});

    vector<chrono_spec> v4{{._Lit_char = 'h'}, {._Lit_char = 'i'}};
    test_parse_helper(parse_chrono_format_specs_fn, s4, false, s4.size(),
        {.expected_alignment       = _Align::_Left,
            .expected_fill         = view_typ(TYPED_LITERAL(CharT, "*")),
            .expected_width        = 6,
            .expected_chrono_specs = v4});

    vector<chrono_spec> v5{{._Type = 'y'}, {._Lit_char = 'm'}, {._Lit_char = 'm'}};
    test_parse_helper(parse_chrono_format_specs_fn, s5, false, s5.size(),
        {.expected_alignment       = _Align::_Center,
            .expected_fill         = view_typ(TYPED_LITERAL(CharT, "*")),
            .expected_width        = 4,
            .expected_precision    = 4,
            .expected_chrono_specs = v5});

    vector<chrono_spec> v{{._Type = 'H'}}; // we don't throw a format_error until we parse the %H
    test_parse_helper(parse_chrono_format_specs_fn, s6, true, view_typ::npos, {.expected_chrono_specs = v});
    test_parse_helper(parse_chrono_format_specs_fn, s7, true, view_typ::npos, {.expected_chrono_specs = v});

    return true;
}

#ifndef __clang__ // TRANSITION, LLVM-48606
template <typename CharT>
bool test_day_formatter() {
    using view_typ = basic_string_view<CharT>;
    using str_typ  = basic_string<CharT>;

    view_typ s0(TYPED_LITERAL(CharT, "%d"));
    str_typ a0(TYPED_LITERAL(CharT, "27"));

    day d{27};
    auto res = format(s0, d);
    assert(res == a0);

    return true;
}
#endif // __clang__

int main() {
    test_parse_conversion_spec<char>();
    test_parse_conversion_spec<wchar_t>();

    test_parse_chrono_format_specs<char>();
    test_parse_chrono_format_specs<wchar_t>();

#ifndef __clang__ // TRANSITION, LLVM-48606
    test_day_formatter<char>();
    test_day_formatter<wchar_t>();
#endif // __clang__
}
