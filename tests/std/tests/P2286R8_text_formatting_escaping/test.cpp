// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <format>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

using namespace std;

template <typename CharT>
[[nodiscard]] constexpr const CharT* choose_literal(const char* const str, const wchar_t* const wstr) noexcept {
    if constexpr (is_same_v<CharT, char>) {
        return str;
    } else {
        return wstr;
    }
}

#define STR(Literal) (choose_literal<CharT>(Literal, L##Literal))

template <class charT, class... Args>
auto make_testing_format_args(Args&&... vals) {
    if constexpr (is_same_v<charT, wchar_t>) {
        return make_wformat_args(forward<Args>(vals)...);
    } else {
        return make_format_args(forward<Args>(vals)...);
    }
}

template <class CharT, class T>
void test_escaped_character() {
    assert(format(STR("{:?}"), T('\t')) == STR(R"('\t')"));
    assert(format(STR("{:?}"), T('\n')) == STR(R"('\n')"));
    assert(format(STR("{:?}"), T('\r')) == STR(R"('\r')"));
    assert(format(STR("{:?}"), T('\"')) == STR(R"('"')"));
    assert(format(STR("{:?}"), T('\'')) == STR(R"('\'')"));
    assert(format(STR("{:?}"), T('\\')) == STR(R"('\\')"));

    assert(format(STR("{:?}"), T('\0')) == STR(R"('\u{0}')"));
    assert(format(STR("{:?}"), T('\v')) == STR(R"('\u{b}')"));
    assert(format(STR("{:?}"), T('\f')) == STR(R"('\u{c}')"));
    assert(format(STR("{:?}"), T('\x7F')) == STR(R"('\u{7f}')"));

    assert(format(STR("{:?}"), T(' ')) == STR("' '"));
    assert(format(STR("{:?}"), T('~')) == STR("'~'"));

    if constexpr (is_same_v<CharT, wchar_t> && is_same_v<T, wchar_t>) {
        assert(format(L"{:?}", L'\xA0') == LR"('\u{a0}')"); // U+00A0 NO-BREAK SPACE
        assert(format(L"{:?}", L'\x300') == LR"('\u{300}')"); // U+0300 COMBINING GRAVE ACCENT

        assert(format(L"{:?}", L'\xA1') == L"'\xA1'"); // U+00A1 INVERTED EXCLAMATION MARK

        assert(format(L"{:?}", L'\xD800') == LR"('\x{d800}')");
        assert(format(L"{:?}", L'\xDFFF') == LR"('\x{dfff}')");
    }
}

template <class CharT>
void test_escaped_string() {
    assert(format(STR("{:?}"), STR("\t")) == STR(R"("\t")"));
    assert(format(STR("{:?}"), STR("\n")) == STR(R"("\n")"));
    assert(format(STR("{:?}"), STR("\r")) == STR(R"("\r")"));
    assert(format(STR("{:?}"), STR("\"")) == STR("\"\\\"\""));
    assert(format(STR("{:?}"), STR("\'")) == STR(R"("'")"));
    assert(format(STR("{:?}"), STR("\\")) == STR(R"("\\")"));

    assert(format(STR("{:?}"), STR("\v")) == STR(R"("\u{b}")"));
    assert(format(STR("{:?}"), STR("\f")) == STR(R"("\u{c}")"));
    assert(format(STR("{:?}"), STR("\x7F")) == STR(R"("\u{7f}")"));

    assert(format(STR("{:?}"), STR(" ")) == STR("\" \""));
    assert(format(STR("{:?}"), STR("~")) == STR("\"~\""));

    assert(format(STR("[{:?}]"), basic_string{STR("\0 \n \t \x02 \x1b"), 9}) == STR(R"(["\u{0} \n \t \u{2} \u{1b}"])"));
    assert(format(STR("[{:?}]"), basic_string_view{STR("\0 \n \t \x02 \x1b"), 9})
           == STR(R"(["\u{0} \n \t \u{2} \u{1b}"])"));

    if constexpr (is_same_v<CharT, wchar_t>) {
        assert(format(L"{:?}", L"\xA0") == LR"("\u{a0}")"); // U+00A0 NO-BREAK SPACE
        assert(format(L"{:?}", L"\U0010FFFF") == LR"("\u{10ffff}")"); // noncharacter
        assert(format(L"{:?}", L"\x300") == LR"("\u{300}")"); // U+0300 COMBINING GRAVE ACCENT

        assert(format(L"{:?}", L"\xA1") == L"\"\xA1\""); // U+00A1 INVERTED EXCLAMATION MARK
        assert(format(L"{:?}", L"\U00010000") == L"\"\U00010000\""); // U+10000 LINEAR B SYLLABLE B008 A

        assert(format(L"{:?}", L"\xD800") == L"\"\\x{d800}\"");
        assert(format(L"{:?}", L"\xDFFF") == L"\"\\x{dfff}\"");
        assert(format(L"{:?}", L"\xDFFF\xD800") == L"\"\\x{dfff}\\x{d800}\"");

        assert(format(L"{:?}", L"\xA0\x300") == L"\"\\u{a0}\\u{300}\"");
        assert(format(L"{:?}", L" \x300") == L"\" \x300\"");
        assert(format(L"{:?}", L"~\x300") == L"\"~\x300\"");
    }
}

template <class CharT, class T>
void test_format_specs() {
    assert(format(STR("{:5?}"), T('\n')) == STR(R"('\n' )"));
    try {
        (void) vformat(STR("{:.3?}"), make_testing_format_args<CharT>(T('\n')));
        assert(false);
    } catch (const format_error&) {
    }

    assert(format(STR("{:5?}"), STR("\n")) == STR(R"("\n" )"));
    assert(format(STR("{:.3?}"), STR("\n")) == STR(R"("\n)"));
    assert(format(STR("{:5.3?}"), STR("\n")) == STR(R"("\n  )"));
    assert(format(STR("{:>5.3?}"), STR("\n")) == STR(R"(  "\n)"));
}

int main() {
    test_escaped_character<char, char>();
    test_escaped_character<wchar_t, char>();
    test_escaped_character<wchar_t, wchar_t>();

    test_escaped_string<char>();
    test_escaped_string<wchar_t>();

    test_format_specs<char, char>();
    test_format_specs<wchar_t, char>();
    test_format_specs<wchar_t, wchar_t>();
}
