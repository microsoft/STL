// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <assert.h>
#include <format>
#include <iterator>
#include <locale>
#include <string>
#include <string_view>

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


using namespace std;
// TODO: fill in tests
template back_insert_iterator<string> std::vformat_to(
    back_insert_iterator<string>, const locale&, string_view, format_args_t<back_insert_iterator<string>, char>);

template <class charT, class... Args>
auto make_testing_format_args(Args&&... vals) {
    if constexpr (is_same_v<charT, wchar_t>) {
        return make_wformat_args(forward<Args>(vals)...);
    } else {
        return make_format_args(forward<Args>(vals)...);
    }
}


// tests for format with no format args or replacement fields
template <class charT>
void test_simple_formatting() {
    basic_string<charT> output_string;

    vformat_to(back_insert_iterator(output_string), locale::classic(), TYPED_LITERAL(charT, "f"),
        make_testing_format_args<charT>());
    assert(output_string == TYPED_LITERAL(charT, "f"));

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), TYPED_LITERAL(charT, "format"),
        make_testing_format_args<charT>());
    assert(output_string == TYPED_LITERAL(charT, "format"));
}

template <class charT>
void test_escaped_curls() {
    basic_string<charT> output_string;

    // test escaped opening curls
    vformat_to(back_insert_iterator(output_string), locale::classic(), TYPED_LITERAL(charT, "{{"),
        make_testing_format_args<charT>());
    assert(output_string == TYPED_LITERAL(charT, "{"));

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), TYPED_LITERAL(charT, "{{{{"),
        make_testing_format_args<charT>());
    assert(output_string == TYPED_LITERAL(charT, "{{"));

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), TYPED_LITERAL(charT, "x{{"),
        make_testing_format_args<charT>());
    assert(output_string == TYPED_LITERAL(charT, "x{"));

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), TYPED_LITERAL(charT, "{{ {{"),
        make_testing_format_args<charT>());
    assert(output_string == TYPED_LITERAL(charT, "{ {"));

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), TYPED_LITERAL(charT, "x{{x"),
        make_testing_format_args<charT>());
    assert(output_string == TYPED_LITERAL(charT, "x{x"));

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), TYPED_LITERAL(charT, "{{x"),
        make_testing_format_args<charT>());
    assert(output_string == TYPED_LITERAL(charT, "{x"));

    // tests escaped closing curls
    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), TYPED_LITERAL(charT, "}}"),
        make_testing_format_args<charT>());
    assert(output_string == TYPED_LITERAL(charT, "}"));

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), TYPED_LITERAL(charT, "}}}}"),
        make_testing_format_args<charT>());
    assert(output_string == TYPED_LITERAL(charT, "}}"));

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), TYPED_LITERAL(charT, "x}}"),
        make_testing_format_args<charT>());
    assert(output_string == TYPED_LITERAL(charT, "x}"));

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), TYPED_LITERAL(charT, "}} }}"),
        make_testing_format_args<charT>());
    assert(output_string == TYPED_LITERAL(charT, "} }"));

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), TYPED_LITERAL(charT, "x}}x"),
        make_testing_format_args<charT>());
    assert(output_string == TYPED_LITERAL(charT, "x}x"));

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), TYPED_LITERAL(charT, "}}x"),
        make_testing_format_args<charT>());
    assert(output_string == TYPED_LITERAL(charT, "}x"));
}

template <class charT>
void test_simple_replacement_field() {
    basic_string<charT> output_string;

    vformat_to(back_insert_iterator(output_string), locale::classic(), TYPED_LITERAL(charT, "{}"),
        make_testing_format_args<charT>(TYPED_LITERAL(charT, "f")));
    assert(output_string == TYPED_LITERAL(charT, "f"));

    // Test string_view
    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), TYPED_LITERAL(charT, "{}"),
        make_testing_format_args<charT>(basic_string_view<charT>(TYPED_LITERAL(charT, "f"))));
    assert(output_string == TYPED_LITERAL(charT, "f"));

    // Test bool
    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), TYPED_LITERAL(charT, "{}"),
        make_testing_format_args<charT>(true));
    assert(output_string == TYPED_LITERAL(charT, "true"));

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), TYPED_LITERAL(charT, "{}"),
        make_testing_format_args<charT>(false));
    assert(output_string == TYPED_LITERAL(charT, "false"));

    // Test char
    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), TYPED_LITERAL(charT, "{}"),
        make_testing_format_args<charT>('a'));
    assert(output_string == TYPED_LITERAL(charT, "a"));

    // Test const void*
    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), TYPED_LITERAL(charT, "{}"),
        make_testing_format_args<charT>(static_cast<const void*>(nullptr)));
    assert(output_string == TYPED_LITERAL(charT, "0x0"));

    // Test void*
    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), TYPED_LITERAL(charT, "{}"),
        make_testing_format_args<charT>(static_cast<void*>(nullptr)));
    assert(output_string == TYPED_LITERAL(charT, "0x0"));

    // Test nullptr
    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), TYPED_LITERAL(charT, "{}"),
        make_testing_format_args<charT>(nullptr));
    assert(output_string == TYPED_LITERAL(charT, "0x0"));

    // Test signed integers
    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), TYPED_LITERAL(charT, "{}"),
        make_testing_format_args<charT>(1234));
    assert(output_string == TYPED_LITERAL(charT, "1234"));

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), TYPED_LITERAL(charT, "{}"),
        make_testing_format_args<charT>(1234ll));
    assert(output_string == TYPED_LITERAL(charT, "1234"));

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), TYPED_LITERAL(charT, "{}"),
        make_testing_format_args<charT>(INT_MIN));
    assert(output_string == TYPED_LITERAL(charT, "-2147483648"));

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), TYPED_LITERAL(charT, "{}"),
        make_testing_format_args<charT>(INT_MAX));
    assert(output_string == TYPED_LITERAL(charT, "2147483647"));

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), TYPED_LITERAL(charT, "{}"),
        make_testing_format_args<charT>(LLONG_MAX));
    assert(output_string == TYPED_LITERAL(charT, "9223372036854775807"));

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), TYPED_LITERAL(charT, "{}"),
        make_testing_format_args<charT>(LLONG_MIN));
    assert(output_string == TYPED_LITERAL(charT, "-9223372036854775808"));

    // Test unsigned integers
    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), TYPED_LITERAL(charT, "{}"),
        make_testing_format_args<charT>(1234u));
    assert(output_string == TYPED_LITERAL(charT, "1234"));

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), TYPED_LITERAL(charT, "{}"),
        make_testing_format_args<charT>(1234ull));
    assert(output_string == TYPED_LITERAL(charT, "1234"));

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), TYPED_LITERAL(charT, "{}"),
        make_testing_format_args<charT>(UINT_MAX));
    assert(output_string == TYPED_LITERAL(charT, "4294967295"));

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), TYPED_LITERAL(charT, "{}"),
        make_testing_format_args<charT>(ULLONG_MAX));
    assert(output_string == TYPED_LITERAL(charT, "18446744073709551615"));

    // Test float
    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), TYPED_LITERAL(charT, "{}"),
        make_testing_format_args<charT>(12.34f));
    assert(output_string == TYPED_LITERAL(charT, "12.34"));

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), TYPED_LITERAL(charT, "{}"),
        make_testing_format_args<charT>(FLT_MAX));
    assert(output_string == TYPED_LITERAL(charT, "3.4028235e+38"));

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), TYPED_LITERAL(charT, "{}"),
        make_testing_format_args<charT>(-FLT_MAX));
    assert(output_string == TYPED_LITERAL(charT, "-3.4028235e+38"));

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), TYPED_LITERAL(charT, "{}"),
        make_testing_format_args<charT>(FLT_MIN));
    assert(output_string == TYPED_LITERAL(charT, "1.1754944e-38"));

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), TYPED_LITERAL(charT, "{}"),
        make_testing_format_args<charT>(FLT_EPSILON));
    assert(output_string == TYPED_LITERAL(charT, "1.1920929e-07"));

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), TYPED_LITERAL(charT, "{}"),
        make_testing_format_args<charT>(FLT_TRUE_MIN));
    assert(output_string == TYPED_LITERAL(charT, "1e-45"));

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), TYPED_LITERAL(charT, "{}"),
        make_testing_format_args<charT>(numeric_limits<float>::infinity()));
    assert(output_string == TYPED_LITERAL(charT, "inf"));

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), TYPED_LITERAL(charT, "{}"),
        make_testing_format_args<charT>(-numeric_limits<float>::infinity()));
    assert(output_string == TYPED_LITERAL(charT, "-inf"));

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), TYPED_LITERAL(charT, "{}"),
        make_testing_format_args<charT>(numeric_limits<float>::quiet_NaN()));
    assert(output_string == TYPED_LITERAL(charT, "nan"));

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), TYPED_LITERAL(charT, "{}"),
        make_testing_format_args<charT>(0.f));
    assert(output_string == TYPED_LITERAL(charT, "0"));

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), TYPED_LITERAL(charT, "{}"),
        make_testing_format_args<charT>(-0.f));
    assert(output_string == TYPED_LITERAL(charT, "-0"));

    // Test double
    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), TYPED_LITERAL(charT, "{}"),
        make_testing_format_args<charT>(12.34));
    assert(output_string == TYPED_LITERAL(charT, "12.34"));

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), TYPED_LITERAL(charT, "{}"),
        make_testing_format_args<charT>(DBL_MAX));
    assert(output_string == TYPED_LITERAL(charT, "1.7976931348623157e+308"));

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), TYPED_LITERAL(charT, "{}"),
        make_testing_format_args<charT>(-DBL_MAX));
    assert(output_string == TYPED_LITERAL(charT, "-1.7976931348623157e+308"));

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), TYPED_LITERAL(charT, "{}"),
        make_testing_format_args<charT>(DBL_MIN));
    assert(output_string == TYPED_LITERAL(charT, "2.2250738585072014e-308"));

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), TYPED_LITERAL(charT, "{}"),
        make_testing_format_args<charT>(DBL_EPSILON));
    assert(output_string == TYPED_LITERAL(charT, "2.220446049250313e-16"));

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), TYPED_LITERAL(charT, "{}"),
        make_testing_format_args<charT>(DBL_TRUE_MIN));
    assert(output_string == TYPED_LITERAL(charT, "5e-324"));

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), TYPED_LITERAL(charT, "{}"),
        make_testing_format_args<charT>(numeric_limits<double>::infinity()));
    assert(output_string == TYPED_LITERAL(charT, "inf"));

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), TYPED_LITERAL(charT, "{}"),
        make_testing_format_args<charT>(-numeric_limits<double>::infinity()));
    assert(output_string == TYPED_LITERAL(charT, "-inf"));

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), TYPED_LITERAL(charT, "{}"),
        make_testing_format_args<charT>(numeric_limits<double>::quiet_NaN()));
    assert(output_string == TYPED_LITERAL(charT, "nan"));

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), TYPED_LITERAL(charT, "{}"),
        make_testing_format_args<charT>(0.0));
    assert(output_string == TYPED_LITERAL(charT, "0"));

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), TYPED_LITERAL(charT, "{}"),
        make_testing_format_args<charT>(-0.0));
    assert(output_string == TYPED_LITERAL(charT, "-0"));
}

template <class charT>
void test_multiple_replacement_fields() {
    basic_string<charT> output_string;

    vformat_to(back_insert_iterator(output_string), locale::classic(), TYPED_LITERAL(charT, "{0} {0}"),
        make_testing_format_args<charT>(TYPED_LITERAL(charT, "f")));
    assert(output_string == TYPED_LITERAL(charT, "f f"));
}

int main() {

    test_simple_formatting<char>();
    test_simple_formatting<wchar_t>();

    test_escaped_curls<char>();
    test_escaped_curls<wchar_t>();

    test_simple_replacement_field<char>();
    test_simple_replacement_field<wchar_t>();

    test_multiple_replacement_fields<char>();
    test_multiple_replacement_fields<wchar_t>();

    return 0;
}
