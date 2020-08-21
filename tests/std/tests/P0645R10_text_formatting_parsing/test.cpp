// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <assert.h>
#include <format>
#include <string_view>

using namespace std;
using namespace std::string_view_literals;

template <typename _CharT>
struct testing_callbacks {
    _Align expected_alignment;
    basic_string_view<_CharT> expected_fill;
    constexpr void _On_align(_Align _Aln) {
        assert(_Aln == expected_alignment);
    }
    constexpr void _On_fill(basic_string_view<_CharT> _Sv) {
        assert(_Sv == expected_fill);
    }
};
template <typename _CharT>
testing_callbacks(_Align, basic_string_view<_CharT>) -> testing_callbacks<_CharT>;

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

int main() {
    test_parse_align();
    static_assert(test_parse_align());
    return 0;
}
