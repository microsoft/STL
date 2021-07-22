// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

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
    static_assert(_Is_any_of_v<CharT, unsigned char, wchar_t>);
    const CharT* it = begin(encoded);
    for (size_t i = 0; i < N_dec; ++i) {
        char32_t val = 0;
        it           = _Decode_utf(it, end(encoded), val);
        assert(val == decoded[i]);
    }
    assert(it == end(encoded));
}

constexpr bool test_utf8_decode() {
    test_utf_decode_helper<unsigned char>({0xC0, 0xAF, 0xE0, 0x80, 0xBF, 0xF0, 0x81, 0x82, 0x41},
        {0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0x41});
    test_utf_decode_helper<unsigned char>({0xED, 0xA0, 0x80, 0xED, 0xBF, 0xBF, 0xED, 0xAF, 0x41},
        {0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0x41});
    test_utf_decode_helper<unsigned char>({0xF4, 0x91, 0x92, 0x93, 0xFF, 0x41, 0x80, 0xBF, 0x42},
        {0xFFFd, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0x41, 0xFFFD, 0xFFFD, 0x42});
    test_utf_decode_helper<unsigned char>(
        {0xE1, 0x80, 0xE2, 0xF0, 0x91, 0x92, 0xF1, 0xBF, 0x41}, {0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0x41});
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
    test_unicode_properties();
    static_assert(test_unicode_properties());

    test_utf8_decode();
    static_assert(test_utf8_decode());

    test_utf16_decode();
    static_assert(test_utf16_decode());

    static_assert(forward_iterator<_Unicode_codepoint_iterator<char>>);
    // static_assert(_STD input_or_output_iterator<_Grapheme_break_property_iterator<wchar_t>>);
    return 0;
}
