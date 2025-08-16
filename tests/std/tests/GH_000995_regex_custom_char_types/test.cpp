// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cwchar>
#include <ios>
#include <iterator>
#include <regex>
#include <string>
#include <type_traits>

using namespace std;

namespace signed_wchar_ns {
    enum class signed_wchar_enum : short {};

    bool operator==(const signed_wchar_enum swe, const char ch) {
        return static_cast<unsigned short>(swe) == static_cast<unsigned char>(ch);
    }

#if !_HAS_CXX20
    bool operator!=(const signed_wchar_enum swe, const char ch) {
        return !(swe == ch);
    }

    bool operator==(const char ch, const signed_wchar_enum swe) {
        return swe == ch;
    }

    bool operator!=(const char ch, const signed_wchar_enum swe) {
        return !(swe == ch);
    }
#endif // !_HAS_CXX20
} // namespace signed_wchar_ns

using signed_wchar_ns::signed_wchar_enum;

namespace ullong_ns {
    enum class ullong_enum : unsigned long long {};

    bool operator==(const ullong_enum ull, const char ch) {
        return static_cast<unsigned long long>(ull) == static_cast<unsigned char>(ch);
    }

#if !_HAS_CXX20
    bool operator!=(const ullong_enum ull, const char ch) {
        return !(ull == ch);
    }

    bool operator==(const char ch, const ullong_enum ull) {
        return ull == ch;
    }

    bool operator!=(const char ch, const ullong_enum ull) {
        return !(ull == ch);
    }
#endif // !_HAS_CXX20
} // namespace ullong_ns

using ullong_ns::ullong_enum;

template <class T>
T convert_to(const signed_wchar_enum& char_enum) {
    return static_cast<T>(char_enum);
}

template <class T>
T convert_to(const ullong_enum& char_enum) {
    return static_cast<T>(char_enum);
}

template <class Elem>
class wrapped_character {
public:
    wrapped_character() = default;
    explicit wrapped_character(char ch) : character(static_cast<wchar_t>(ch)) {}
    explicit wrapped_character(unsigned char ch) : character(ch) {}
    explicit wrapped_character(Elem w) : character(w) {}
    template <class T = wchar_t, enable_if_t<!is_same_v<T, Elem>, int> = 0>
    explicit wrapped_character(wchar_t w) : character(w) {}
    explicit wrapped_character(int w) = delete;
    explicit wrapped_character(unsigned int w) : character(static_cast<wchar_t>(w)) {}

    operator unsigned char() const {
        return static_cast<unsigned char>(character);
    }

    operator unsigned int() const {
        return static_cast<unsigned int>(character);
    }

    operator char() const               = delete;
    operator wchar_t() const            = delete;
    operator int() const                = delete;
    operator unsigned long long() const = delete;

    friend bool operator==(const wrapped_character& lhs, const wrapped_character& rhs) {
        return lhs.character == rhs.character;
    }

    friend bool operator==(const wrapped_character& wc, const char ch) {
        return wc.character == static_cast<Elem>(ch);
    }

#if !_HAS_CXX20
    friend bool operator!=(const wrapped_character& lhs, const wrapped_character& rhs) {
        return !(lhs == rhs);
    }

    friend bool operator!=(const wrapped_character& wc, const char ch) {
        return !(wc == ch);
    }

    friend bool operator==(const char ch, const wrapped_character& wc) {
        return wc == ch;
    }

    friend bool operator!=(const char ch, const wrapped_character& wc) {
        return !(wc == ch);
    }
#endif // !_HAS_CXX20

    template <class T>
    friend T convert_to(const wrapped_character<Elem>& wrapped_char) {
        return static_cast<T>(wrapped_char.character);
    }

private:
    Elem character;
};

template <class T, class Elem>
T convert_to(const wrapped_character<Elem>& wrapped_char);

using wrapped_wchar  = wrapped_character<wchar_t>;
using wrapped_ullong = wrapped_character<unsigned long long>;

template <class UnderlyingChar, class FwdIt>
basic_string<UnderlyingChar> convert_to_underlying_string(FwdIt first, FwdIt last) {
    basic_string<UnderlyingChar> str;
    for (; first != last; ++first) {
        str.push_back(convert_to<UnderlyingChar>(*first));
    }
    return str;
}

template <class StringType, class UnderlyingChar>
StringType convert_from_underlying_string(const basic_string<UnderlyingChar>& str) {
    StringType result;
    for (const auto& ch : str) {
        result.push_back(static_cast<typename StringType::value_type>(ch));
    }
    return result;
}

template <class SourceChar, class UnderlyingChar>
class test_regex_traits {
private:
    using rx_traits = regex_traits<UnderlyingChar>;

public:
    using char_type       = SourceChar;
    using string_type     = basic_string<SourceChar>;
    using locale_type     = typename rx_traits::locale_type;
    using char_class_type = typename rx_traits::char_class_type;

    test_regex_traits() = default;

    static size_t length(const SourceChar* p) {
        return string_type::char_traits::length(p);
    }

    SourceChar translate(const SourceChar c) const {
        return c;
    }

    SourceChar translate_nocase(const SourceChar c) const {
        return static_cast<SourceChar>(inner.translate_nocase(convert_to<UnderlyingChar>(c)));
    }

    template <class FwdIt>
    string_type transform(FwdIt first, FwdIt last) const {
        auto str = convert_to_underlying_string<UnderlyingChar>(first, last);
        return convert_from_underlying_string<string_type>(inner.transform(str.begin(), str.end()));
    }

    template <class FwdIt>
    string_type transform_primary(FwdIt first, FwdIt last) const {
        if (distance(first, last) == 1) {
            return string_type{first, last};
        }
        return string_type{};
    }

    template <class FwdIt>
    string_type lookup_collatename(FwdIt first, FwdIt last) const {
        auto str = convert_to_underlying_string<UnderlyingChar>(first, last);
        return convert_from_underlying_string<string_type>(inner.lookup_collatename(str.begin(), str.end()));
    }

    template <class FwdIt>
    char_class_type lookup_classname(FwdIt first, FwdIt last, bool icase = false) const {
        auto str = convert_to_underlying_string<UnderlyingChar>(first, last);
        return inner.lookup_classname(str.begin(), str.end(), icase);
    }

    bool isctype(SourceChar c, char_class_type f) const {
        return inner.isctype(convert_to<UnderlyingChar>(c), f);
    }

    int value(SourceChar ch, int radix) const {
        return inner.value(convert_to<UnderlyingChar>(ch), radix);
    }

    locale_type imbue(locale_type l) {
        return inner.imbue(l);
    }

    locale_type getloc() const {
        return inner.getloc();
    }

private:
    rx_traits inner;
};

template <class Elem, class Uelem>
struct custom_char_traits {
    using char_type  = Elem;
    using int_type   = conditional_t<sizeof(Elem) <= 4, int, unsigned long long>;
    using pos_type   = streampos;
    using off_type   = streamoff;
    using state_type = mbstate_t;

    static Elem* copy(Elem* const first1, const Elem* const first2, const size_t count) noexcept {
        copy_n(first2, count, first1);
        return first1;
    }

    static Elem* move(Elem* const result, const Elem* const first2, const size_t count) noexcept /* strengthened */ {
        if (result == first2) {
            // nothing to do
        } else if (first2 <= result && result < first2 + count) {
            copy_backward(first2, first2 + count, result + count);
        } else {
            copy_n(first2, count, result);
        }

        return result;
    }

    static int compare(const Elem* first1, const Elem* first2, size_t count) noexcept {
        for (; 0 < count; --count, ++first1, ++first2) {
            if (*first1 != *first2) {
                return custom_char_traits::lt(*first1, *first2) ? -1 : +1;
            }
        }

        return 0;
    }

    static size_t length(const Elem* first) {
        size_t count = 0;
        while (*first != Elem{}) {
            ++count;
            ++first;
        }

        return count;
    }

    static const Elem* find(const Elem* first, size_t count, const Elem& ch) noexcept /* strengthened */ {
        // look for ch in [first, first + count)
        for (; 0 < count; --count, ++first) {
            if (*first == ch) {
                return first;
            }
        }

        return nullptr;
    }

    static Elem* assign(const Elem* const first, size_t count, const Elem ch) {
        for (Elem* next = first; count > 0; --count, ++next) {
            *next = ch;
        }

        return first;
    }

    static void assign(Elem& left, const Elem& right) noexcept {
        left = right;
    }

    static bool eq(const Elem left, const Elem right) noexcept {
        return left == right;
    }

    static bool lt(const Elem left, const Elem right) noexcept {
        return convert_to<Uelem>(left) < convert_to<Uelem>(right);
    }

    static Elem to_char_type(const int_type meta) noexcept {
        return static_cast<Elem>(meta);
    }

    static int_type to_int_type(const Elem ch) noexcept {
        return static_cast<int_type>(static_cast<wchar_t>(ch));
    }

    static bool eq_int_type(const int_type left, const int_type right) noexcept {
        return left == right;
    }

    static int_type not_eof(const int_type meta) noexcept {
        return meta != eof() ? meta : !eof();
    }

    static int_type eof() noexcept {
        return static_cast<int_type>(-1);
    }
};

template <>
struct char_traits<signed_wchar_enum> : custom_char_traits<signed_wchar_enum, wchar_t> {};

template <>
struct char_traits<ullong_enum> : custom_char_traits<ullong_enum, unsigned long long> {};

template <>
struct char_traits<wrapped_wchar> : custom_char_traits<wrapped_wchar, wchar_t> {};

template <>
struct char_traits<wrapped_ullong> : custom_char_traits<wrapped_ullong, unsigned long long> {};


void test_gh_5592() {
    // GH-5592: Remove non-standard _Uelem from parser
    // This test checks that the parser compiles and doesn't crash
    // when user-defined character types are used.

    // This regex is only intended to exert many different paths in the parser,
    // but it is not designed to be meaningful.
    wstring test_regex = L"^a*[\u00fe-\\u0101][\u0123-\u0146](?:abc|def){0,3}(=.gwer)"
                         LR"(\b.{6}\B(\.\d\f)\g\1\0\x34(?!gef)[-f[.a.][=b=]c-e-]$)";
    {
        auto str = convert_from_underlying_string<basic_string<signed_wchar_enum>>(test_regex);
        basic_regex<signed_wchar_enum, test_regex_traits<signed_wchar_enum, wchar_t>> re{str};
    }

    {
        auto str = convert_from_underlying_string<basic_string<wrapped_wchar>>(test_regex);
        basic_regex<wrapped_wchar, test_regex_traits<wrapped_wchar, wchar_t>> re{str};
    }
}

template <class Elem, class RxTraits>
void test_regex_for_custom_char(const wstring& pattern, const wstring& input, bool should_match) {
    auto converted_pattern = convert_from_underlying_string<basic_string<Elem>>(pattern);
    auto converted_input   = convert_from_underlying_string<basic_string<Elem>>(input);
    basic_regex<Elem, RxTraits> re{converted_pattern};
    assert(regex_match(converted_input, re) == should_match);
}

void test_regex_on_custom_wchars(const wstring& pattern, const wstring& input, bool should_match) {
    test_regex_for_custom_char<signed_wchar_enum, test_regex_traits<signed_wchar_enum, wchar_t>>(
        pattern, input, should_match);
    test_regex_for_custom_char<wrapped_wchar, test_regex_traits<wrapped_wchar, wchar_t>>(pattern, input, should_match);
}

template <class Elem, class RxTraits, class UnderlyingChar>
void test_regex_for_custom_char(const UnderlyingChar* pat_first, const UnderlyingChar* pat_last,
    const UnderlyingChar* input_first, const UnderlyingChar* input_last, bool should_match) {
    basic_string<Elem> converted_pattern;
    basic_string<Elem> converted_input;
    transform(pat_first, pat_last, back_inserter(converted_pattern),
        [](UnderlyingChar val) { return static_cast<Elem>(val); });
    transform(input_first, input_last, back_inserter(converted_input),
        [](UnderlyingChar val) { return static_cast<Elem>(val); });
    basic_regex<Elem, RxTraits> re{converted_pattern};
    assert(regex_match(converted_input, re) == should_match);
}

void test_regex_on_custom_ullongs(const unsigned long long* pat_first, const unsigned long long* pat_last,
    const unsigned long long* input_first, const unsigned long long* input_last, bool should_match) {
    test_regex_for_custom_char<ullong_enum, test_regex_traits<ullong_enum, wchar_t>>(
        pat_first, pat_last, input_first, input_last, should_match);
    test_regex_for_custom_char<wrapped_ullong, test_regex_traits<wrapped_ullong, wchar_t>>(
        pat_first, pat_last, input_first, input_last, should_match);
}

void test_gh_5671_single_character_patterns() {
    // test patterns matching single characters
    // simple smoke test
    test_regex_on_custom_wchars(L"a", L"a", true);
    test_regex_on_custom_wchars(L"a", L"b", false);

    // no truncation issues at 0x100
    test_regex_on_custom_wchars(L"\u0100", L"\u0100", true); // U+0100 LATIN CAPITAL LETTER A WITH MACRON
    test_regex_on_custom_wchars(wstring(1, L'\0'), L"\u0100", false); // U+0100 LATIN CAPITAL LETTER A WITH MACRON
    test_regex_on_custom_wchars(L"\u0100", wstring(1, L'\0'), false); // U+0100 LATIN CAPITAL LETTER A WITH MACRON

    // no issues with signed character values
    test_regex_on_custom_wchars(
        L"\uf000", L"\uf000", true); // U+F000 in private use area (signed for signed_wchar_enum)

    // matching for very large values
    {
        const unsigned long long pattern = 0xabababababULL;

        test_regex_on_custom_ullongs(&pattern, &pattern + 1, &pattern, &pattern + 1, true);

        for (const unsigned long long unmatched : {0xabULL, 0xababULL, 0xababababULL, 0xababababaaULL, 0xababababacULL,
                 0x9bababababULL, 0xbbababababULL, 0xffffffababababULL}) {
            test_regex_on_custom_ullongs(&pattern, &pattern + 1, &unmatched, &unmatched + 1, false);
        }
    }
}

void test_gh_5671_line_terminators() {
    // test line terminator handling for dot
    test_regex_on_custom_wchars(L".", L"a", true);
    test_regex_on_custom_wchars(L".", L"\r", false);
    test_regex_on_custom_wchars(L".", L"\n", false);
    test_regex_on_custom_wchars(L".", L"\u2028", false); // U+2028 LINE SEPARATOR
    test_regex_on_custom_wchars(L".", L"\u2029", false); // U+2029 PARAGRAPH SEPARATOR
    test_regex_on_custom_wchars(L".", L"\u2c60", true); // U+2C60 LATIN CAPITAL LETTER L WITH DOUBLE BAR

    {
        const unsigned long long dot = L'.';

        for (const unsigned long long line_terminator : {L'\r', L'\n', L'\u2028', L'\u2029'}) {
            test_regex_on_custom_ullongs(&dot, &dot + 1, &line_terminator, &line_terminator + 1, false);

            const unsigned long long shifted_line_terminator = 0x100000000ULL + line_terminator;
            test_regex_on_custom_ullongs(&dot, &dot + 1, &shifted_line_terminator, &shifted_line_terminator + 1, true);
        }
    }
}

void test_gh_5671_word_boundaries() {
    // test word boundaries
    test_regex_on_custom_wchars(LR"(a\b.)", L"a\u0141", true); // U+0141 LATIN CAPITAL LETTER L WITH STROKE
    test_regex_on_custom_wchars(LR"(a\B.)", L"a\u0141", false); // U+0141 LATIN CAPITAL LETTER L WITH STROKE
}

void test_gh_5671_character_ranges() {
    // test simple ranges
    test_regex_on_custom_wchars(LR"([b-\u0141])", L"a", false); // U+0141 LATIN CAPITAL LETTER L WITH STROKE
    test_regex_on_custom_wchars(LR"([b-\u0141])", L"b", true);
    test_regex_on_custom_wchars(LR"([b-\u0141])", L"\u0141", true);
    test_regex_on_custom_wchars(LR"([b-\u0141])", L"\u0142", false); // U+0142 LATIN SMALL LETTER L WITH STROKE

    // test that GH-5437 fix for small ranges near U+0100 remains in place for custom types
    test_regex_on_custom_wchars(LR"([\u00ff-\u0100])", L"\u00FE", false); // U+00FE LATIN SMALL LETTER THORN
    test_regex_on_custom_wchars(LR"([\u00ff-\u0100])", L"\u00FF", true); // U+00FF LATIN SMALL LETTER Y WITH DIAERESIS
    test_regex_on_custom_wchars(LR"([\u00ff-\u0100])", L"\u0100", true); // U+0100 LATIN CAPITAL LETTER A WITH MACRON
    test_regex_on_custom_wchars(LR"([\u00ff-\u0100])", L"\u0101", false); // U+0101 LATIN SMALL LETTER A WITH MACRON

    // test ranges with negative upper boundary when signed
    test_regex_on_custom_wchars(
        LR"([\u7fed-\u8123])", L"\u7fed", true); // U+7FED CJK UNIFIED IDEOGRAPH-7FED, U+8123 CJK UNIFIED IDEOGRAPH-8123
    test_regex_on_custom_wchars(LR"([\u7fed-\u8123])", L"\u8123", true);
    test_regex_on_custom_wchars(LR"([\u7fed-\u8123])", L"\u8001", true); // U+8001 CJK UNIFIED IDEOGRAPH-8001
    test_regex_on_custom_wchars(LR"([\u7fed-\u8123])", L"\u7fec", false); // U+7FEC CJK UNIFIED IDEOGRAPH-7FEC
    test_regex_on_custom_wchars(LR"([\u7fed-\u8123])", L"\u8124", false); // U+8124 CJK UNIFIED IDEOGRAPH-8124

    // test ranges with boundaries exceeding UINT_MAX for small and large ranges
    for (unsigned long long upper_bound : {0x100000006ULL, 0x100000036ULL}) {
        const unsigned long long pattern[] = {L'[', 0x100000004ULL, L'-', upper_bound, L']'};
        for (unsigned long long matched = 0x100000004ULL; matched <= upper_bound; ++matched) {
            test_regex_on_custom_ullongs(begin(pattern), end(pattern), &matched, &matched + 1, true);
        }

        for (const unsigned long long unmatched :
            {0x00000004ULL, upper_bound & 0xffffffffULL, 0x100000003ULL, upper_bound + 1ULL}) {
            test_regex_on_custom_ullongs(begin(pattern), end(pattern), &unmatched, &unmatched + 1, false);
        }
    }
}

void test_gh_5671() {
    // GH-5671: Remove non-standard _Uelem from matcher
    //
    // The following tests check that internal narrowing conversions to integers in parser and matcher
    // as well as signedness of the character type are handled appropriately
    // even when using (weird) custom character types.
    test_gh_5671_single_character_patterns();
    test_gh_5671_line_terminators();
    test_gh_5671_word_boundaries();
    test_gh_5671_character_ranges();
}

int main() {
    test_gh_5592();
    test_gh_5671();
    return 0;
}
