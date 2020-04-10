// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "header.h"
#include <type_traits>

static_assert(is_nothrow_move_constructible_v<source_location>, "source_location is not nothrow move constructible.");
static_assert(is_nothrow_move_assignable_v<source_location>, "source_location is not nothrow move assignable.");
static_assert(is_nothrow_swappable_v<source_location>, "source_location is not nothrow swappable.");

constexpr auto g = source_location::current();
static_assert(g.line() == __LINE__ - 1);
static_assert(g.column() == 37);
// static_assert(g.function_name() == ""sv);
// static_assert(string_view{g.file_name()}.ends_with(R"(tests\std\tests\P1208R6_source_location\test.cpp)"sv));

constexpr int s_int_line = __LINE__ + 3;
struct s {
    constexpr s(source_location x = source_location::current()) : loc(x) {}
    constexpr s([[maybe_unused]] int i) {}
    source_location loc = source_location::current();
};

constexpr int s2_int_line = __LINE__ + 3;
struct s2 {
    constexpr s2(source_location l = source_location::current()) : x{l} {}
    constexpr s2([[maybe_unused]] int i) {}
    s x = source_location::current();
};

constexpr void copy_test() {
    auto rhs = source_location::current();
    auto lhs = rhs;
    assert(lhs.line() == rhs.line());
    assert(lhs.column() == rhs.column());
    assert(string_view{lhs.function_name()} == string_view{rhs.function_name()});
    assert(string_view{lhs.file_name()} == string_view{rhs.file_name()});
}

// constexpr void tab_test() {
//    constexpr auto x = source_location::current();
// }

constexpr void local_test() {
    constexpr auto x = source_location::current();
    assert(x.line() == __LINE__ - 1);
    assert(x.column() == 41);
    assert(x.function_name() == "local_test"sv);
    assert(string_view{x.file_name()}.ends_with(R"(tests\std\tests\P1208R6_source_location\test.cpp)"sv));
}

constexpr void argument_test(unsigned line, unsigned column, source_location x = source_location::current()) {
    assert(x.line() == line);
    assert(x.column() == column);
    assert(x.function_name() == "test"sv);
    assert(string_view{x.file_name()}.ends_with(R"(tests\std\tests\P1208R6_source_location\test.cpp)"sv));
}

constexpr void sloc_constructor_test() {
    s x;
    assert(x.loc.line() == __LINE__ - 1);
    assert(x.loc.column() == 7);
    assert(x.loc.function_name() == "sloc_constructor_test"sv);
    assert(string_view{x.loc.file_name()}.ends_with(R"(tests\std\tests\P1208R6_source_location\test.cpp)"sv));
}

constexpr void different_constructor_test() {
    s x{1};
    assert(x.loc.line() == s_int_line);
    assert(x.loc.column() == 5);
    assert(x.loc.function_name() == "s"sv);
    assert(string_view{x.loc.file_name()}.ends_with(R"(tests\std\tests\P1208R6_source_location\test.cpp)"sv));
}

constexpr void sub_member_test() {
    s2 s;
    assert(s.x.loc.line() == __LINE__ - 1);
    assert(s.x.loc.column() == 8);
    assert(s.x.loc.function_name() == "sub_member_test"sv);
    assert(string_view{s.x.loc.file_name()}.ends_with(R"(tests\std\tests\P1208R6_source_location\test.cpp)"sv));

    s2 s_i{1};
    assert(s_i.x.loc.line() == s2_int_line);
    assert(s_i.x.loc.column() == 5);
    assert(s_i.x.loc.function_name() == "s2"sv);
    assert(string_view{s_i.x.loc.file_name()}.ends_with(R"(tests\std\tests\P1208R6_source_location\test.cpp)"sv));
}

constexpr void lambda_test() {
    auto l = [loc = source_location::current()]() { return loc; };
    auto x = l();
    assert(x.line() == __LINE__ - 2);
    assert(x.column() == 21);
    assert(x.function_name() == "lambda_test"sv);
    assert(string_view{x.file_name()}.ends_with(R"(tests\std\tests\P1208R6_source_location\test.cpp)"sv));
}

template <class T>
constexpr source_location function_template() {
    return source_location::current();
}

constexpr void function_template_test() {
    auto x1 = function_template<void>();
    assert(x1.line() == __LINE__ - 5);
    assert(x1.column() == 29);
    assert(x1.function_name() == "function_template"sv);
    assert(string_view{x1.file_name()}.ends_with(R"(tests\std\tests\P1208R6_source_location\test.cpp)"sv));

    auto x2 = function_template<int>();
    assert(x1.line() == x2.line());
    assert(x1.column() == x2.column());
    assert(string_view{x1.function_name()} == string_view{x2.function_name()});
    assert(string_view{x1.file_name()} == string_view{x2.file_name()});
}

constexpr bool test() {
    copy_test();
    // tab_test();
    local_test();
    argument_test(__LINE__, 5);
    auto loc = std::source_location::current();
    argument_test(__LINE__ - 1, 38, loc);
    sloc_constructor_test();
    different_constructor_test();
    sub_member_test();
    lambda_test();
    function_template_test();
    header_test();
    return true;
}

int main() {
    test();
    // static_assert(test());
    return 0;
}
