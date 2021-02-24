// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <assert.h>
#include <format>
#include <iterator>
#include <locale>
#include <string>
#include <string_view>

using namespace std;
// TODO: fill in tests
template back_insert_iterator<string> std::vformat_to(
    back_insert_iterator<string>, const locale&, string_view, format_args_t<back_insert_iterator<string>, char>);


int main() {
    string output_string = "";

    vformat_to(back_insert_iterator(output_string), locale::classic(), "f", make_format_args());
    assert(output_string == "f");

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), "format", make_format_args());
    assert(output_string == "format");

    // test escaped opening curls
    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), "{{", make_format_args());
    assert(output_string == "{");

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), "{{{{", make_format_args());
    assert(output_string == "{{");

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), "x{{", make_format_args());
    assert(output_string == "x{");

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), "{{ {{", make_format_args());
    assert(output_string == "{ {");

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), "x{{x", make_format_args());
    assert(output_string == "x{x");

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), "{{x", make_format_args());
    assert(output_string == "{x");

    // tests escaped closing curls
    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), "}}", make_format_args());
    assert(output_string == "}");

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), "}}}}", make_format_args());
    assert(output_string == "}}");

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), "x}}", make_format_args());
    assert(output_string == "x}");

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), "}} }}", make_format_args());
    assert(output_string == "} }");

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), "x}}x", make_format_args());
    assert(output_string == "x}x");

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), "}}x", make_format_args());
    assert(output_string == "}x");

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), "{}", make_format_args((const char*) "f"));
    assert(output_string == "f");

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), "{0} {0}", make_format_args((const char*) "f"));
    assert(output_string == "f f");

    // Test string_view
    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), "{}", make_format_args("f"sv));
    assert(output_string == "f");

    // Test bool
    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), "{}", make_format_args(true));
    assert(output_string == "true");

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), "{}", make_format_args(false));
    assert(output_string == "false");

    // Test char
    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), "{}", make_format_args('a'));
    assert(output_string == "a");

    // Test void*
    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), "{}", make_format_args((const void*) 0));
    assert(output_string == "0x0");

    /* TODO: Doesn't properly overload on void* and nullptr
    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), "{}", make_format_args(nullptr));
    assert(output_string == "0x0");

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), "{}", make_format_args((void*)0));
    assert(output_string == "0x0");
    */

    // Test signed integers
    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), "{}", make_format_args(1234));
    assert(output_string == "1234");

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), "{}", make_format_args(1234ll));
    assert(output_string == "1234");

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), "{}", make_format_args(INT_MIN));
    assert(output_string == "-2147483648");

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), "{}", make_format_args(INT_MAX));
    assert(output_string == "2147483647");

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), "{}", make_format_args(LLONG_MAX));
    assert(output_string == "9223372036854775807");

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), "{}", make_format_args(LLONG_MIN));
    assert(output_string == "-9223372036854775808");

    // Test unsigned integers
    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), "{}", make_format_args(1234u));
    assert(output_string == "1234");

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), "{}", make_format_args(1234ull));
    assert(output_string == "1234");

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), "{}", make_format_args(UINT_MAX));
    assert(output_string == "4294967295");

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), "{}", make_format_args(ULLONG_MAX));
    assert(output_string == "18446744073709551615");

    // Test float
    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), "{}", make_format_args(12.34f));
    assert(output_string == "12.34");

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), "{}", make_format_args(FLT_MAX));
    assert(output_string == "3.4028235e+38");

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), "{}", make_format_args(-FLT_MAX));
    assert(output_string == "-3.4028235e+38");

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), "{}", make_format_args(FLT_MIN));
    assert(output_string == "1.1754944e-38");

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), "{}", make_format_args(FLT_EPSILON));
    assert(output_string == "1.1920929e-07");

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), "{}", make_format_args(FLT_TRUE_MIN));
    assert(output_string == "1e-45");

    // Test double
    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), "{}", make_format_args(12.34));
    assert(output_string == "12.34");

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), "{}", make_format_args(DBL_MAX));
    assert(output_string == "1.7976931348623157e+308");

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), "{}", make_format_args(-DBL_MAX));
    assert(output_string == "-1.7976931348623157e+308");

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), "{}", make_format_args(DBL_MIN));
    assert(output_string == "2.2250738585072014e-308");

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), "{}", make_format_args(DBL_EPSILON));
    assert(output_string == "2.220446049250313e-16");

    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), "{}", make_format_args(DBL_TRUE_MIN));
    assert(output_string == "5e-324");

    return 0;
}
