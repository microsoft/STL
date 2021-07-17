// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <assert.h>
#include <format>

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

constexpr bool test_utf8_decode() {
    const uint8_t table_3_8_overlong[] = {0xC0, 0xAF, 0xE0, 0x80, 0xBF, 0xF0, 0x81, 0x82, 0xF4};
    const uint8_t* it                  = table_3_8_overlong;
    while (it != end(table_3_8_overlong) - 1) {
        uint32_t val       = 0;
        const uint8_t* nxt = _Decode_utf8(it, end(table_3_8_overlong), val);
        assert(val == 0xFFFD && nxt == it + 1);
        it = nxt;
    }
    uint32_t val       = 0;
    const uint8_t* nxt = _Decode_utf8(it, end(table_3_8_overlong), val);
    assert(val == 0x41);
    assert(nxt == end(table_3_8_overlong));
    return true;
}

int main() {
    test_unicode_properties();
    static_assert(test_unicode_properties());

    test_utf8_decode();
    // static_assert(test_utf8_decode());
    // static_assert(_STD input_or_output_iterator<_Grapheme_break_property_iterator<wchar_t>>);
    return 0;
}
