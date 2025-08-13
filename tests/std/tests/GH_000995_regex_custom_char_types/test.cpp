// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// TRANSITION, GH-5563
#pragma warning(push)
#pragma warning(disable : 6510)
#include <string>
#pragma warning(pop)

#include <algorithm>
#include <cstddef>
#include <ios>
#include <regex>

using namespace std;

template <class UnderlyingChar, class FwdIt>
basic_string<UnderlyingChar> convert_to_underlying_string(FwdIt first, FwdIt last) {
    basic_string<UnderlyingChar> str;
    for (; first != last; ++first) {
        str.push_back(static_cast<UnderlyingChar>(*first));
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
        return static_cast<SourceChar>(inner.translate_nocase(static_cast<UnderlyingChar>(c)));
    }

    template <class FwdIt>
    string_type transform(FwdIt first, FwdIt last) const {
        auto str = convert_to_underlying_string<UnderlyingChar>(first, last);
        return convert_from_underlying_string<string_type>(inner.transform(str.begin(), str.end()));
    }

    template <class FwdIt>
    string_type transform_primary(FwdIt first, FwdIt last) const {
        auto str = convert_to_underlying_string<UnderlyingChar>(first, last);
        return convert_from_underlying_string<string_type>(inner.transform_primary(str.begin(), str.end()));
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
        return inner.isctype(static_cast<UnderlyingChar>(c), f);
    }

    int value(SourceChar ch, int radix) const {
        return inner.value(static_cast<UnderlyingChar>(ch), radix);
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

class wrapped_wchar {
public:
    wrapped_wchar() = default;
    explicit wrapped_wchar(char ch) : character(static_cast<wchar_t>(ch)) {}
    explicit wrapped_wchar(unsigned char ch) : character(ch) {}
    explicit wrapped_wchar(wchar_t w) : character(w) {}
    explicit wrapped_wchar(int w) = delete;
    explicit wrapped_wchar(unsigned int w) : character(static_cast<wchar_t>(w)) {}

    operator unsigned char() const {
        return static_cast<unsigned char>(character);
    }

    operator unsigned int() const {
        return static_cast<unsigned int>(character);
    }

    // to support test_regex_traits
    operator wchar_t() const {
        return character;
    }

    operator char() const = delete;
    operator int() const  = delete;

    friend bool operator==(const wrapped_wchar& lhs, const wrapped_wchar& rhs) {
        return lhs.character == rhs.character;
    }

    friend bool operator==(const wrapped_wchar& lhs, const char rhs) {
        return lhs.character == static_cast<wchar_t>(rhs);
    }

#if !_HAS_CXX20
    friend bool operator!=(const wrapped_wchar& lhs, const wrapped_wchar& rhs) {
        return lhs.character != rhs.character;
    }

    friend bool operator!=(const wrapped_wchar& lhs, const char rhs) {
        return lhs.character != static_cast<wchar_t>(rhs);
    }

    friend bool operator==(const char lhs, const wrapped_wchar& rhs) {
        return static_cast<wchar_t>(lhs) == rhs.character;
    }

    friend bool operator!=(const char lhs, const wrapped_wchar& rhs) {
        return static_cast<wchar_t>(lhs) != rhs.character;
    }
#endif // !_HAS_CXX20

private:
    wchar_t character;
};

template <class Elem>
struct custom_char_traits {
    using char_type  = Elem;
    using int_type   = int;
    using pos_type   = streampos;
    using off_type   = streamoff;
    using state_type = char_traits<wchar_t>::state_type;

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
        return static_cast<wchar_t>(left) < static_cast<wchar_t>(right);
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
struct char_traits<signed_wchar_enum> : custom_char_traits<signed_wchar_enum> {};

template <>
struct char_traits<wrapped_wchar> : custom_char_traits<wrapped_wchar> {};

void test_gh_5592() {
    // GH-5592: Remove _Uelem from the parser
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

int main() {
    test_gh_5592();
    return 0;
}
