// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <assert.h>
#include <format>
#include <string_view>

using namespace std;


constexpr bool test_unicode_properties() {
    assert(_Grapheme_Break_Property_Data._Get_property_for_codepoint(0xB)
           == _Grapheme_Break_Property_Values::_Control_value);
    assert(_Grapheme_Break_Property_Data._Get_property_for_codepoint(0xC)
           == _Grapheme_Break_Property_Values::_Control_value);
    assert(
        _Grapheme_Break_Property_Data._Get_property_for_codepoint(0xA) == _Grapheme_Break_Property_Values::_LF_value);
    assert(
        _Grapheme_Break_Property_Data._Get_property_for_codepoint(0xD) == _Grapheme_Break_Property_Values::_CR_value);

    assert(_Grapheme_Break_Property_Data._Get_property_for_codepoint(0x7F)
           == _Grapheme_Break_Property_Values::_Control_value);
    assert(_Grapheme_Break_Property_Data._Get_property_for_codepoint(0x80)
           == _Grapheme_Break_Property_Values::_Control_value);
    assert(_Grapheme_Break_Property_Data._Get_property_for_codepoint(0x9F)
           == _Grapheme_Break_Property_Values::_Control_value);
    assert(
        _Grapheme_Break_Property_Data._Get_property_for_codepoint(0x7E) == _Grapheme_Break_Property_Values::_No_value);
    assert(
        _Grapheme_Break_Property_Data._Get_property_for_codepoint(0xA0) == _Grapheme_Break_Property_Values::_No_value);


    // emoji-data
    assert(_Extended_Pictographic_Property_Data._Get_property_for_codepoint(0x2194)
           == _Extended_Pictographic_Property_Values::_Extended_Pictographic_value);

    // emoji_component, not extended_pictographic
    assert(_Extended_Pictographic_Property_Data._Get_property_for_codepoint(0x23)
           == _Extended_Pictographic_Property_Values::_No_value);
    return true;
}

template <typename CharT, size_t N_enc, size_t N_dec>
constexpr void test_utf_decode_helper(const CharT (&encoded)[N_enc], const char32_t (&decoded)[N_dec]) {
    static_assert(_Is_any_of_v<CharT, char, wchar_t>);
    assert(ranges::equal(_Unicode_codepoint_iterator(encoded, encoded + N_enc), _Unicode_codepoint_end_iterator{},
        decoded, decoded + N_dec));
}

constexpr bool test_utf8_decode() {
    test_utf_decode_helper<char>("\xC0\xAF\xE0\x80\xBF\xF0\x81\x82\x41",
        {0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0x41, 0x0});
    test_utf_decode_helper<char>("\xED\xA0\x80\xED\xBF\xBF\xED\xAF\x41",
        {0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0x41, 0x0});
    test_utf_decode_helper<char>("\xF4\x91\x92\x93\xFF\x41\x80\xBF\x42",
        {0xFFFd, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0x41, 0xFFFD, 0xFFFD, 0x42, 0x0});
    test_utf_decode_helper<char>("\xE1\x80\xE2\xF0\x91\x92\xF1\xBF\x41", {0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0x41, 0x0});
    return true;
}

constexpr bool test_utf16_decode() {
    test_utf_decode_helper<wchar_t>({0xD7FF, 0xD800}, {0xD7FF, 0xFFFD});
    test_utf_decode_helper<wchar_t>({0xD800, 0xD7FF}, {0xFFFD, 0xD7FF});
    test_utf_decode_helper<wchar_t>({0xD7FF, 0xDF02}, {0xD7FF, 0xFFFD});
    test_utf_decode_helper<wchar_t>({0xDF02, 0xD7FF}, {0xFFFD, 0xD7FF});

    return true;
}

int main() {
    _set_error_mode(_OUT_TO_MSGBOX);
    test_unicode_properties();
    static_assert(test_unicode_properties());

    test_utf8_decode();
    static_assert(test_utf8_decode());

    test_utf16_decode();
    static_assert(test_utf16_decode());

    static_assert(forward_iterator<_Unicode_codepoint_iterator<char>>);
    static_assert(sentinel_for<_Unicode_codepoint_end_iterator, _Unicode_codepoint_iterator<char>>);
    // static_assert(_STD input_or_output_iterator<_Grapheme_break_property_iterator<wchar_t>>);
    return 0;
}
