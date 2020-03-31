//// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "header.h"
#include <type_traits>

static_assert(is_nothrow_move_constructible_v<source_location>, "source_location is not nothrow move constructible.");
static_assert(is_nothrow_move_assignable_v<source_location>, "source_location is not nothrow move assignable.");
static_assert(is_nothrow_swappable_v<source_location>, "source_location is not nothrow swappable.");

constexpr auto g = source_location::current();

struct s {
    constexpr s(source_location x = source_location::current()) : loc(x) {}
    constexpr s([[maybe_unused]] int i) {}
    source_location loc = source_location::current();
};

struct s2 {
    s x;
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
    assert(x.line() == 36);
    assert(x.column() == 41);
    assert(x.function_name() == "local_test"sv);
    assert(string_view{x.file_name()}.ends_with(R"(tests\std\tests\P1208R6_source_location\test.cpp)"sv));
}

constexpr void global_test() {
    assert(g.line() == 10);
    assert(g.column() == 37);
    assert(g.function_name() == ""sv);
    assert(string_view{g.file_name()}.ends_with(R"(tests\std\tests\P1208R6_source_location\test.cpp)"sv));
}

constexpr void calling_test(source_location x = source_location::current()) {
    assert(x.line() == 121);
    assert(x.column() == 5);
    assert(x.function_name() == "test"sv);
    assert(string_view{x.file_name()}.ends_with(R"(tests\std\tests\P1208R6_source_location\test.cpp)"sv));
}

constexpr void argument_test(source_location x = source_location::current()) {
    assert(x.line() == 122);
    assert(x.column() == 38);
    assert(x.function_name() == "test"sv);
    assert(string_view{x.file_name()}.ends_with(R"(tests\std\tests\P1208R6_source_location\test.cpp)"sv));
}

constexpr void sloc_constructor_test() {
    s x;
    assert(x.loc.line() == 65);
    assert(x.loc.column() == 7);
    assert(x.loc.function_name() == "sloc_constructor_test"sv);
    assert(string_view{x.loc.file_name()}.ends_with(R"(tests\std\tests\P1208R6_source_location\test.cpp)"sv));
}

constexpr void different_constructor_test() {
    s x{1};
    assert(x.loc.line() == 14);
    assert(x.loc.column() == 5);
    assert(x.loc.function_name() == "s"sv);
    assert(string_view{x.loc.file_name()}.ends_with(R"(tests\std\tests\P1208R6_source_location\test.cpp)"sv));
}

constexpr void sub_member_test() {
    s2 s;
    assert(s.x.loc.line() == 19);
    assert(s.x.loc.column() == 7);
    assert(s.x.loc.function_name() == "s2"sv);
    assert(string_view{s.x.loc.file_name()}.ends_with(R"(tests\std\tests\P1208R6_source_location\test.cpp)"sv));
}

constexpr void lambda_test() {
    auto l = [loc = source_location::current()]() { return loc; };
    auto x = l();
    assert(x.line() == 89);
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
    assert(x1.line() == 99);
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
    global_test();
    local_test();
    calling_test();
    auto loc = std::source_location::current();
    argument_test(loc);
    sloc_constructor_test();
    different_constructor_test();
    // sub_member_test();
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
