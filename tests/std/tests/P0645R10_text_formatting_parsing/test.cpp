// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <assert.h>
#include <format>
#include <stdio.h>
#include <string_view>

using namespace std;
using namespace std::string_view_literals;

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


constexpr bool test_parse_align() {
    auto s0 = ""sv;
    auto s2 = "*<"sv;
    auto s3 = "*>"sv;
    auto s4 = "*^"sv;
    _Parse_align(s0.data(), s0.data() + s0.size(), testing_callbacks{_Align::_None, ""sv});
    _Parse_align(s2.data(), s2.data() + s2.size(), testing_callbacks{_Align::_Left, "*"sv});
    _Parse_align(s3.data(), s3.data() + s3.size(), testing_callbacks{_Align::_Right, "*"sv});
    assert(_Parse_align(s4.data(), s4.data() + s4.size(), testing_callbacks{_Align::_Center, "*"sv})
           == s4.data() + s4.size());
    return true;
}

template <typename CharT>
void test_parse_width_helper(basic_string_view<CharT> view, bool err_expected = false, int width = -1,
    int dynamic_width_id = -1, bool dynamic_width_auto = false) {
    try {
        _Parse_width(view.data(), view.data() + view.size(),
            testing_callbacks<CharT>{.expected_width = width,
                .expected_dynamic_width              = dynamic_width_id,
                .expected_auto_dynamic_width         = dynamic_width_auto});
        assert(!err_expected);
    } catch (const format_error&) {
        assert(err_expected);
    }
}

bool test_parse_width() {
    auto s0 = "1"sv;
    auto s1 = "{1}"sv;
    auto s2 = "{0}"sv;
    auto i0 = "0"sv;
    auto i1 = "01"sv;
    test_parse_width_helper(s0, false, 1);
    test_parse_width_helper(s1, false, -1, 1);
    test_parse_width_helper(s2, false, -1, 0);
    test_parse_width_helper(i0, false);
    test_parse_width_helper(i1, false);
    return true;
}

bool test_parse_arg_id() {
    // note that parse arg id starts with the arg id itself, not the { beginning of the
    // format spec
    auto s0 = "}"sv;
    auto s1 = ":"sv;
    auto s2 = ":}"sv;
    auto s3 = "0:}"sv;
    auto s4 = "0:"sv;
    auto s5 = "1}"sv;
    auto i0 = "01}"sv;
    auto i1 = "0"sv;

    const char* end = nullptr;

    end = _Parse_arg_id(s0.data(), s0.data() + s0.size(), testing_arg_id_callbacks{});
    assert(end == &s0[0]);
    end = _Parse_arg_id(s1.data(), s1.data() + s1.size(), testing_arg_id_callbacks{});
    assert(end == &s1[0]);
    end = _Parse_arg_id(s2.data(), s2.data() + s2.size(), testing_arg_id_callbacks{});
    assert(end == &s2[0]);
    end = _Parse_arg_id(s3.data(), s3.data() + s3.size(), testing_arg_id_callbacks{});
    assert(end == &s3[1]);
    end = _Parse_arg_id(s4.data(), s4.data() + s4.size(), testing_arg_id_callbacks{});
    assert(end == &s4[1]);
    end = _Parse_arg_id(s5.data(), s5.data() + s5.size(), testing_arg_id_callbacks{});
    assert(end == &s5[1]);
    try {
        _Parse_arg_id(i0.data(), i0.data() + i0.size(), testing_arg_id_callbacks{});
        assert(false);
    } catch (const format_error&) {
        assert(true);
    }
    try {
        _Parse_arg_id(i1.data(), i1.data() + i1.size(), testing_arg_id_callbacks{});
        assert(false);
    } catch (const format_error&) {
        assert(true);
    }
    return true;
}

int main() {
    test_parse_align();
    static_assert(test_parse_align());
    test_parse_arg_id();
    test_parse_width();
    return 0;
}
