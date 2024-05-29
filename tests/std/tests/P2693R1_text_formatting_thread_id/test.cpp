// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <execution>
#include <ios>
#include <locale>
#include <ranges>
#include <sstream>
#include <string>
#include <string_view>
#include <thread>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>

#include "test_format_support.hpp"

#define STR(Str) TYPED_LITERAL(CharT, Str)

using namespace std;

template <class CharT, template <class> class Fmt>
void check_formatting_of_default_constructed_thread_id() {
    Fmt<CharT> fmt;
    thread::id id;

    // empty format-spec
    assert(fmt(STR("{}"), id) == STR("0"));
    assert(fmt(STR("{:}"), id) == STR("0"));
    assert(fmt(STR("{0:}"), id) == STR("0"));

    // align only
    assert(fmt(STR("{:<}"), id) == STR("0"));
    assert(fmt(STR("{:^}"), id) == STR("0"));
    assert(fmt(STR("{:>}"), id) == STR("0"));

    // fill-and-align only
    assert(fmt(STR("{::<}"), id) == STR("0"));
    assert(fmt(STR("{:*^}"), id) == STR("0"));
    assert(fmt(STR("{:=>}"), id) == STR("0"));

    // width only
    assert(fmt(STR("{:5}"), id) == STR("    0"));

    // width only (replacement field)
    assert(fmt(STR("{:{}}"), id, 7) == STR("      0"));

    // fill-and-align with width
    assert(fmt(STR("{:=<5}"), id) == STR("0===="));
    assert(fmt(STR("{::^6}"), id) == STR("::0:::"));
    assert(fmt(STR("{:*>7}"), id) == STR("******0"));

    // fill-and-align with width (replacement field)
    assert(fmt(STR("{:/<{}}"), id, 7) == STR("0//////"));
    assert(fmt(STR("{::^{}}"), id, 6) == STR("::0:::"));
    assert(fmt(STR("{0:_>{1}}"), id, 5) == STR("____0"));
}

template <class CharT, template <class> class Fmt>
void check_formatting_of_this_thread_id() {
    Fmt<CharT> fmt;
    const thread::id id = this_thread::get_id();
    const auto id_str   = [id] {
        // '_Get_underlying_id' is MSVC STL specific
        if constexpr (same_as<CharT, wchar_t>) {
            return to_wstring(id._Get_underlying_id());
        } else {
            return to_string(id._Get_underlying_id());
        }
    }();

    // empty format-spec
    assert(fmt(STR("{}"), id) == id_str);
    assert(fmt(STR("{:}"), id) == id_str);
    assert(fmt(STR("{0:}"), id) == id_str);

    // fill-and-align only
    assert(fmt(STR("{::<}"), id) == id_str);
    assert(fmt(STR("{:*^}"), id) == id_str);
    assert(fmt(STR("{:=>}"), id) == id_str);

    { // width only
        constexpr int width  = 13;
        const int fill_width = static_cast<int>(width - id_str.size());

        const auto s  = fmt(STR("{:13}"), id);
        const auto it = ranges::mismatch(s, views::repeat(STR(' '), fill_width)).in1;
        assert(it == s.begin() + fill_width);
        assert(ranges::equal(ranges::subrange{it, s.end()}, id_str));
    }

    { // width only (replacement field)
        constexpr int width  = 15;
        const int fill_width = static_cast<int>(width - id_str.size());

        const auto s  = fmt(STR("{:{}}"), id, width);
        const auto it = ranges::mismatch(s, views::repeat(STR(' '), fill_width)).in1;
        assert(it == s.begin() + fill_width);
        assert(ranges::equal(ranges::subrange{it, s.end()}, id_str));
    }

    { // fill-and-align with width
        constexpr int width  = 21;
        const int fill_width = static_cast<int>(width - id_str.size());

        {
            const auto s  = fmt(STR("{:=<21}"), id);
            const auto it = ranges::mismatch(views::reverse(s), views::repeat(STR('='), fill_width)).in1;
            assert(it.base() == s.end() - fill_width);
            assert(ranges::equal(ranges::subrange{s.begin(), it.base()}, id_str));
        }

        {
            const auto s              = fmt(STR("{::^21}"), id);
            const int left_fill_width = fill_width / 2;
            const auto it1            = ranges::mismatch(s, views::repeat(STR(':'), left_fill_width)).in1;
            assert(it1 == s.begin() + left_fill_width);

            const int right_fill_width = fill_width - left_fill_width;
            const auto it2 = ranges::mismatch(views::reverse(s), views::repeat(STR(':'), right_fill_width)).in1;
            assert(it2.base() == s.end() - right_fill_width);

            assert(ranges::equal(ranges::subrange{it1, it2.base()}, id_str));
        }

        {
            const auto s  = fmt(STR("{:*>21}"), id);
            const auto it = ranges::mismatch(s, views::repeat(STR('*'), fill_width)).in1;
            assert(it == s.begin() + fill_width);
            assert(ranges::equal(ranges::subrange{it, s.end()}, id_str));
        }
    }

    { // fill-and-align with width (replacement field)
        constexpr int width  = 27;
        const int fill_width = static_cast<int>(width - id_str.size());

        {
            const auto s  = fmt(STR("{:/<{}}"), id, width);
            const auto it = ranges::mismatch(views::reverse(s), views::repeat(STR('/'), fill_width)).in1;
            assert(it.base() == s.end() - fill_width);
            assert(ranges::equal(ranges::subrange{s.begin(), it.base()}, id_str));
        }

        {
            const auto s              = fmt(STR("{::^{}}"), id, width);
            const int left_fill_width = fill_width / 2;
            const auto it1            = ranges::mismatch(s, views::repeat(STR(':'), left_fill_width)).in1;
            assert(it1 == s.begin() + left_fill_width);

            const int right_fill_width = fill_width - left_fill_width;
            const auto it2 = ranges::mismatch(views::reverse(s), views::repeat(STR(':'), right_fill_width)).in1;
            assert(it2.base() == s.end() - right_fill_width);

            assert(ranges::equal(ranges::subrange{it1, it2.base()}, id_str));
        }

        {
            const auto s  = fmt(STR("{0:_>{1}}"), id, width);
            const auto it = ranges::mismatch(s, views::repeat(STR('_'), fill_width)).in1;
            assert(it == s.begin() + fill_width);
            assert(ranges::equal(ranges::subrange{it, s.end()}, id_str));
        }
    }
}

template <class CharT, template <class> class FormatFn>
void check_format_versus_ostream() {
    FormatFn<CharT> fmt;
    const thread::id id = this_thread::get_id();

    { // empty format-spec
        basic_ostringstream<CharT> ss;
        ss << id;
        assert(fmt(STR("{}"), id) == ss.view());
    }

    { // fill-and-align only
        basic_ostringstream<CharT> ss;
        ss.fill('=');
        ss.setf(ss.flags() | ios_base::left, ios_base::adjustfield);
        ss << id;
        assert(fmt(STR("{:=<}"), id) == ss.view());
    }

    { // width only
        constexpr int w = 20;
        basic_ostringstream<CharT> ss;
        ss.width(w);
        ss << id;
        assert(fmt(STR("{:20}"), id) == ss.view());
        assert(fmt(STR("{:{}}"), id, w) == ss.view());
    }

    { // fill-and-align with width
        constexpr int w = 30;
        basic_ostringstream<CharT> ss;
        ss.fill('*');
        ss.setf(ss.flags() | ios_base::left, ios_base::adjustfield);
        ss.width(w);
        ss << id;
        assert(fmt(STR("{:*<30}"), id) == ss.view());
        assert(fmt(STR("{:*<{}}"), id, w) == ss.view());
    }

    { // check what happens if we change stream's locale, precision, and fmtflags other than fill and alignment
        constexpr int w = 25;
        basic_ostringstream<CharT> ss;
        ss.imbue(locale{"en-US"});
        ss.fill('x');
        ss.setf(ss.flags() | ios_base::left | ios_base::hex | ios_base::uppercase | ios_base::showbase,
            ios_base::adjustfield | ios_base::basefield);
        ss.width(w);
        ss.precision(99);
        ss << id;
        assert(fmt(STR("{:x<25}"), id) == ss.view());
        assert(fmt(STR("{:x<{}}"), id, w) == ss.view());
    }
}

template <class CharT>
void check_invalid_specs() {
    ExpectFormatError<CharT> fmt;
    const thread::id id = this_thread::get_id();

    { // damaged fields
        fmt(STR("{"), id);
        fmt(STR("{:"), id);
        fmt(STR("{}}"), id);
        fmt(STR("}"), id);
    }

    { // fill-and-align should not contain '{' or '}' characters
        fmt(STR("{:{^}"), id);
        fmt(STR("{:{<}"), id);
    }

    { // sign, #, 0, precision, L, or type options are not allowed in format-specs
        fmt(STR("{:+}"), id);
        fmt(STR("{:#}"), id);
        fmt(STR("{:0}"), id);
        fmt(STR("{:.5}"), id);
        fmt(STR("{:.{}}"), id, 5);
        fmt(STR("{:L}"), id);
        fmt(STR("{:d}"), id);
    }

    { // mixed invalid format-specs
        fmt(STR("{:+#0}"), id);
        fmt(STR("{:=^5.5}"), id);
        fmt(STR("{0:{1}.{1}}"), id, 5);
        fmt(STR("{:Lx}"), id);
    }
}

// Also test GH-4651 "<format>: Underlying formatters of pair-or-tuple formatter cannot access format args"

template <class CharT>
using default_format_parse_context = conditional_t<is_same_v<CharT, char>, format_parse_context,
    conditional_t<is_same_v<CharT, wchar_t>, wformat_parse_context, void>>;

template <size_t I>
struct substitute_arg {};

template <size_t I, class CharT>
struct std::formatter<substitute_arg<I>, CharT> {
    template <class ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        auto it = ctx.begin();
        if (it != ctx.end() && *it != '}') {
            throw format_error{"Expected empty spec"};
        }

        ctx.check_arg_id(I);
        return it;
    }

    template <class FormatContext>
    auto format(substitute_arg<I>, FormatContext& ctx) const {
        auto visitor = [&]<class T>(T val) -> FormatContext::iterator {
            if constexpr (same_as<T, monostate>) {
                return ranges::copy(STR("monostate"sv), ctx.out()).out;
            } else if constexpr (same_as<T, typename basic_format_arg<FormatContext>::handle>) {
                default_format_parse_context<CharT> parse_ctx{STR("")};
                val.format(parse_ctx, ctx);
                return ctx.out();
            } else {
                return format_to(ctx.out(), STR("{}"), val);
            }
        };

        return visit_format_arg(visitor, ctx.arg(I));
    }
};

template <class CharT>
void check_substitute_arg_with_tuple_formatters() {
    assert(format(STR("{0:}"), tuple{substitute_arg<1>{}, substitute_arg<2>{}}, STR("thread::id"), thread::id{})
           == STR("(thread::id, 0)"));
    assert(format(STR("{0:}"), pair{substitute_arg<1>{}, substitute_arg<2>{}}, STR("thread::id"), thread::id{})
           == STR("(thread::id, 0)"));
}

template <class CharT>
void test() {
    check_formatting_of_default_constructed_thread_id<CharT, FormatFn>();
    check_formatting_of_default_constructed_thread_id<CharT, VFormatFn>();
    check_formatting_of_default_constructed_thread_id<CharT, MoveOnlyFormat>();

    check_substitute_arg_with_tuple_formatters<CharT>();

    const array checks = {
        // NB: those functions call 'this_thread::get_id' - let's check various ids
        check_formatting_of_this_thread_id<CharT, FormatFn>,
        check_formatting_of_this_thread_id<CharT, VFormatFn>,
        check_formatting_of_this_thread_id<CharT, MoveOnlyFormat>,
        check_format_versus_ostream<CharT, FormatFn>,
        check_format_versus_ostream<CharT, VFormatFn>,
        check_format_versus_ostream<CharT, MoveOnlyFormat>,
        check_invalid_specs<CharT>,
    };
    for_each(execution::par, checks.begin(), checks.end(), [](auto f) { f(); });
}

int main() {
    test<char>();
    test<wchar_t>();
}
