// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <assert.h>
#include <format>
#include <iterator>
#include <locale>
#include <string>
#include <string_view>

using namespace std;
using namespace std::string_view_literals;
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

    // TODO: enable these in _Write function PR for sv and bool
    /*
    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), "{}", make_format_args("f"sv));
    assert(output_string == "f");
    */

    /*
    output_string.clear();
    vformat_to(back_insert_iterator(output_string), locale::classic(), "{}", make_format_args(true));
    assert(output_string == "true");
    */

    return 0;
}
