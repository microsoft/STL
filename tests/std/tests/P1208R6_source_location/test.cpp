// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef __EDG__ // TRANSITION, VSO-1285779
#include "header.h"
#include <cassert>
#include <functional>
#include <source_location>
#include <string_view>
#include <type_traits>
using namespace std;

static_assert(is_nothrow_default_constructible_v<source_location>);
static_assert(is_nothrow_move_constructible_v<source_location>);
static_assert(is_nothrow_move_assignable_v<source_location>);
static_assert(is_nothrow_swappable_v<source_location>);

constexpr auto test_cpp = "test.cpp"sv;

constexpr auto g = source_location::current();
static_assert(g.line() == __LINE__ - 1);
#ifdef __clang__
static_assert(g.column() == 20);
#else // ^^^ defined(__clang__) / !defined(__clang__) vvv
static_assert(g.column() == 37);
#endif // ^^^ !defined(__clang__) ^^^
static_assert(g.function_name() == ""sv);
static_assert(string_view{g.file_name()}.ends_with(test_cpp));

constexpr int s_int_line = __LINE__ + 3;
struct s {
    constexpr s(const source_location x = source_location::current()) : loc(x) {}
    constexpr s(int) {}
    source_location loc = source_location::current();
};

constexpr int s2_int_line = __LINE__ + 3;
struct s2 {
    constexpr s2(const source_location l = source_location::current()) : x{l} {}
    constexpr s2(int) {}
    s x = source_location::current();
};

constexpr void copy_test() {
    const auto rhs = source_location::current();
    const auto lhs = rhs;
    assert(lhs.line() == rhs.line());
    assert(lhs.column() == rhs.column());
    assert(string_view{lhs.function_name()} == string_view{rhs.function_name()});
    assert(string_view{lhs.file_name()} == string_view{rhs.file_name()});
}

constexpr void local_test() {
    const auto x = source_location::current();
    assert(x.line() == __LINE__ - 1);
#ifdef __clang__
    assert(x.column() == 20);
#else // ^^^ defined(__clang__) / !defined(__clang__) vvv
    assert(x.column() == 37);
#endif // ^^^ !defined(__clang__) ^^^
#if defined(__clang__) || defined(__EDG__) // TRANSITION, DevCom-10199227 and LLVM-58951
    assert(x.function_name() == "local_test"sv);
#else // ^^^ workaround / no workaround vvv
    assert(x.function_name() == "void __cdecl local_test(void)"sv);
#endif // TRANSITION, DevCom-10199227 and LLVM-58951
    assert(string_view{x.file_name()}.ends_with(test_cpp));
}

constexpr void argument_test(
    const unsigned int line, const unsigned int column, const source_location x = source_location::current()) {
    assert(x.line() == line);
    assert(x.column() == column);
#if defined(__clang__) || defined(__EDG__) // TRANSITION, DevCom-10199227 and LLVM-58951
    assert(x.function_name() == "test"sv);
#else // ^^^ workaround / no workaround vvv
    assert(x.function_name() == "bool __cdecl test(void)"sv);
#endif // TRANSITION, DevCom-10199227 and LLVM-58951
    assert(string_view{x.file_name()}.ends_with(test_cpp));
}

constexpr void sloc_constructor_test() {
    const s x;
    assert(x.loc.line() == __LINE__ - 1);
    assert(x.loc.column() == 13);
#if defined(__clang__) || defined(__EDG__) // TRANSITION, DevCom-10199227 and LLVM-58951
    assert(x.loc.function_name() == "sloc_constructor_test"sv);
#else // ^^^ workaround / no workaround vvv
    if (is_constant_evaluated()) {
        assert(x.loc.function_name() == "int __cdecl main(void)"sv); // TRANSITION, VSO-1285783
    } else {
        assert(x.loc.function_name() == "void __cdecl sloc_constructor_test(void)"sv);
    }
#endif // TRANSITION, DevCom-10199227 and LLVM-58951
    assert(string_view{x.loc.file_name()}.ends_with(test_cpp));
}

constexpr void different_constructor_test() {
    const s x{1};
    assert(x.loc.line() == s_int_line);
#ifdef __clang__
    assert(x.loc.column() == 15);
#else // ^^^ defined(__clang__) / !defined(__clang__) vvv
    assert(x.loc.column() == 5);
#endif // ^^^ !defined(__clang__) ^^^
#if defined(__clang__) || defined(__EDG__) // TRANSITION, DevCom-10199227 and LLVM-58951
    assert(x.loc.function_name() == "s"sv);
#elif defined(_M_IX86) // ^^^ workaround / no workaround vvv
    assert(x.loc.function_name() == "__thiscall s::s(int)"sv);
#else // ^^^ _M_IX86 / !_M_IX86 vvv
    assert(x.loc.function_name() == "__cdecl s::s(int)"sv);
#endif // TRANSITION, DevCom-10199227 and LLVM-58951
    assert(string_view{x.loc.file_name()}.ends_with(test_cpp));
}

constexpr void sub_member_test() {
    const s2 s;
    assert(s.x.loc.line() == __LINE__ - 1);
    assert(s.x.loc.column() == 14);
#if defined(__clang__) || defined(__EDG__) // TRANSITION, DevCom-10199227 and LLVM-58951
    assert(s.x.loc.function_name() == "sub_member_test"sv);
#else // ^^^ workaround / no workaround vvv
    if (is_constant_evaluated()) {
        assert(s.x.loc.function_name() == "int __cdecl main(void)"sv); // TRANSITION, VSO-1285783
    } else {
        assert(s.x.loc.function_name() == "void __cdecl sub_member_test(void)"sv);
    }
#endif // TRANSITION, DevCom-10199227 and LLVM-58951
    assert(string_view{s.x.loc.file_name()}.ends_with(test_cpp));

    const s2 s_i{1};
    assert(s_i.x.loc.line() == s2_int_line);
#ifdef __clang__
    assert(s_i.x.loc.column() == 15);
#else // ^^^ defined(__clang__) / !defined(__clang__) vvv
    assert(s_i.x.loc.column() == 5);
#endif // ^^^ !defined(__clang__) ^^^
#if defined(__clang__) || defined(__EDG__) // TRANSITION, DevCom-10199227 and LLVM-58951
    assert(s_i.x.loc.function_name() == "s2"sv);
#elif defined(_M_IX86) // ^^^ workaround / no workaround vvv
    assert(s_i.x.loc.function_name() == "__thiscall s2::s2(int)"sv);
#else // ^^^ _M_IX86 / !_M_IX86 vvv
    assert(s_i.x.loc.function_name() == "__cdecl s2::s2(int)"sv);
#endif // TRANSITION, DevCom-10199227 and LLVM-58951
    assert(string_view{s_i.x.loc.file_name()}.ends_with(test_cpp));
}

constexpr void lambda_test() {
    const auto l1 = [loc = source_location::current()] { return loc; };
    const auto l2 = [] { return source_location::current(); };
    const auto x1 = l1();
    const auto x2 = l2();
    assert(x1.line() == __LINE__ - 4);
    assert(x2.line() == __LINE__ - 4);
#ifdef __clang__
    assert(x1.column() == 28);
    assert(x2.column() == 33);
#else // ^^^ defined(__clang__) / !defined(__clang__) vvv
    assert(x1.column() == 52);
    assert(x2.column() == 50);
#endif // ^^^ !defined(__clang__) ^^^
#if defined(__clang__) || defined(__EDG__) // TRANSITION, DevCom-10199227 and LLVM-58951
    assert(x1.function_name() == "lambda_test"sv);
    assert(x2.function_name() == "operator()"sv);
#elif defined(_M_IX86) // ^^^ workaround / no workaround vvv
    assert(x1.function_name() == "void __cdecl lambda_test(void)"sv);
    assert(
        string_view{x2.function_name()}.starts_with("struct std::source_location __thiscall lambda_test::<lambda_"sv));
    assert(string_view{x2.function_name()}.ends_with("::operator ()(void) const"sv));
#else // ^^^ _M_IX86 / !_M_IX86 vvv
    assert(x1.function_name() == "void __cdecl lambda_test(void)"sv);
    assert(string_view{x2.function_name()}.starts_with("struct std::source_location __cdecl lambda_test::<lambda_"sv));
    assert(string_view{x2.function_name()}.ends_with("::operator ()(void) const"sv));
#endif // TRANSITION, DevCom-10199227 and LLVM-58951
    assert(string_view{x1.file_name()}.ends_with(test_cpp));
    assert(string_view{x2.file_name()}.ends_with(test_cpp));
}

template <class T>
constexpr source_location function_template() {
    return source_location::current();
}

constexpr void function_template_test() {
    const auto x1 = function_template<void>();
    assert(x1.line() == __LINE__ - 5);
#ifdef __clang__
    assert(x1.column() == 12);
#else // ^^^ defined(__clang__) / !defined(__clang__) vvv
    assert(x1.column() == 29);
#endif // ^^^ !defined(__clang__) ^^^
#if defined(__clang__) || defined(__EDG__) // TRANSITION, DevCom-10199227 and LLVM-58951
    assert(x1.function_name() == "function_template"sv);
#else // ^^^ workaround / no workaround vvv
    assert(x1.function_name() == "struct std::source_location __cdecl function_template<void>(void)"sv);
#endif // TRANSITION, DevCom-10199227 and LLVM-58951
    assert(string_view{x1.file_name()}.ends_with(test_cpp));

    const auto x2 = function_template<int>();
    assert(x1.line() == x2.line());
    assert(x1.column() == x2.column());
#if defined(__clang__) || defined(__EDG__) // TRANSITION, DevCom-10199227 and LLVM-58951
    assert(x2.function_name() == "function_template"sv);
#else // ^^^ workaround / no workaround vvv
    assert(x2.function_name() == "struct std::source_location __cdecl function_template<int>(void)"sv);
#endif // TRANSITION, DevCom-10199227 and LLVM-58951
    assert(string_view{x1.file_name()} == string_view{x2.file_name()});
}

constexpr bool test() {
    copy_test();
    local_test();
    argument_test(__LINE__, 5);
#ifdef __clang__
    const auto loc = source_location::current();
    argument_test(__LINE__ - 1, 22, loc);
#else // ^^^ defined(__clang__) / !defined(__clang__) vvv
    const auto loc = source_location::current();
    argument_test(__LINE__ - 1, 39, loc);
#endif // ^^^ !defined(__clang__) ^^^
    sloc_constructor_test();
    different_constructor_test();
    sub_member_test();
    lambda_test();
    function_template_test();
    header_test();
    return true;
}

// Also test GH-2822 Failed to specialize std::invoke on operator() with default argument
// std::source_location::current()
void test_gh_2822() { // COMPILE-ONLY
    invoke([](source_location = source_location::current()) {});
}

int main() {
    test();
    static_assert(test());
    return 0;
}
#else // ^^^ !defined(__EDG__) / defined(__EDG__) vvv
int main() {}
#endif // ^^^ defined(__EDG__) ^^^
