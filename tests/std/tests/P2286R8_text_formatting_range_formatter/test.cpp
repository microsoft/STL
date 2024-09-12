// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <format>
#include <ranges>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

#include <range_algorithm_support.hpp>
#include <test_format_support.hpp>

#define STR(Str) TYPED_LITERAL(CharT, Str)

using namespace std;

template <class Category, class Element, test::Sized IsSized, test::CanDifference Diff, test::Common IsCommon,
    test::CanCompare Eq, test::ProxyRef Proxy, test::CanView IsView, test::Copyability Copy, class CharT>
struct std::formatter<test::range<Category, Element, IsSized, Diff, IsCommon, Eq, Proxy, IsView, Copy>, CharT> {
public:
    using range_type = test::range<Category, Element, IsSized, Diff, IsCommon, Eq, Proxy, IsView, Copy>;
    using ref_type   = remove_cvref_t<ranges::range_reference_t<range_type>>;

    template <class ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return underlying.parse(ctx);
    }

    template <class FormatContext>
    auto format(const range_type& rng, FormatContext& ctx) const {
        return underlying.format(rng, ctx);
    }

private:
    range_formatter<ref_type, CharT> underlying;
};

template <class Category, class Element, class CharT>
struct std::formatter<test::proxy_reference<Category, Element>, CharT> {
private:
    formatter<remove_cv_t<Element>, CharT> underlying;

public:
    constexpr void set_debug_format()
        requires requires { underlying.set_debug_format(); }
    {
        underlying.set_debug_format();
    }

    template <class ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return underlying.parse(ctx);
    }

    template <class FormatContext>
    auto format(const test::proxy_reference<Category, Element>& proxy, FormatContext& ctx) const {
        return underlying.format(static_cast<Element&>(proxy), ctx);
    }
};

struct FormatAsX {};

template <class CharT>
struct std::formatter<FormatAsX, CharT> {
public:
    template <class ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        auto it = ctx.begin();
        if (it != ctx.end() && *it != '}') {
            throw format_error{"Expected empty spec"};
        }

        return it;
    }

    template <class FormatContext>
    auto format(FormatAsX, FormatContext& ctx) const {
        return ranges::copy(STR("X"sv), ctx.out()).out;
    }
};

// NB: This struct will help us verify if 'range_formatter' handles creation of nested 'basic_format_context' correctly.
struct FormatNextArg {};

template <class CharT>
struct std::formatter<FormatNextArg, CharT> {
public:
    template <class ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        auto it = ctx.begin();
        if (it != ctx.end() && *it != '}') {
            throw format_error{"Expected empty spec"};
        }

        arg_id = ctx.next_arg_id();
        return it;
    }

    template <class FormatContext>
    auto format(FormatNextArg, FormatContext& ctx) const {
        auto visitor = [&](auto arg) {
            using T = decltype(arg);
            if constexpr (same_as<T, monostate>) {
                return ranges::copy(STR("monostate"sv), ctx.out()).out;
            } else if constexpr (same_as<T, typename basic_format_arg<FormatContext>::handle>) {
                ctx.advance_to(ranges::copy(STR("handle: "sv), ctx.out()).out);
                basic_format_parse_context<CharT> parse_ctx(STR(""sv));
                arg.format(parse_ctx, ctx);
                return ctx.out();
            } else {
                return format_to(ctx.out(), STR("{}"), arg);
            }
        };

        return visit_format_arg(visitor, ctx.arg(arg_id));
    }

private:
    size_t arg_id;
};

struct Debuggable {};

template <class CharT>
struct std::formatter<Debuggable, CharT> {
public:
    constexpr void set_debug_format() {
        if (!parse_called) {
            throw format_error{"Incorrect call to 'set_debug_format'; did you call 'parse' first?"};
        }

        debug = true;
    }

    template <class ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        parse_called = true;
        auto it      = ctx.begin();
        if (it == ctx.end() || *it == '}') {
            return it;
        }

        if (*it == 'D') {
            set_debug_format();
            ++it;
        }

        if (it != ctx.end() && *it != '}') {
            throw format_error{"Incorrect format-spec."};
        }

        return it;
    }

    template <class FormatContext>
    auto format(Debuggable, FormatContext& ctx) const {
        return ranges::copy((debug ? STR("debug"sv) : STR("regular"sv)), ctx.out()).out;
    }

private:
    bool debug        = false;
    bool parse_called = false;
};

template <template <class> class FmtFn, class CharT, class Range>
void check_range_formatter() {
    FmtFn<CharT> fmt;

    // Check formatting with empty range-format-spec.
    assert(fmt(STR("{}"), Range(array<int, 0>{})) == STR("[]"));
    assert(fmt(STR("{}"), Range(array{1})) == STR("[1]"));
    assert(fmt(STR("{:}"), Range(array{2})) == STR("[2]"));
    assert(fmt(STR("{}"), Range(array{3, 4})) == STR("[3, 4]"));
    assert(fmt(STR("{:}"), Range(array{5, 6})) == STR("[5, 6]"));
    assert(fmt(STR("{:}"), Range(array{7, 8, 9})) == STR("[7, 8, 9]"));
    assert(fmt(STR("{:}"), Range(array{10, 11, 12, 13, 14})) == STR("[10, 11, 12, 13, 14]"));
    assert(fmt(STR("{0:}"), Range(views::iota(15, 30) | ranges::to<vector>()))
           == STR("[15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29]"));

    // Check formatting with non-empty range-fill-and-align.
    assert(fmt(STR("{:^}"), Range(array{0})) == STR("[0]"));
    assert(fmt(STR("{:%<}"), Range(array{1, 2})) == STR("[1, 2]"));
    assert(fmt(STR("{: >}"), Range(array{3, 4, 5})) == STR("[3, 4, 5]"));

    // Check formatting with non-empty range-fill-and-align and width.
    assert(fmt(STR("{:20}"), Range(array{0})) == STR("[0]                 "));
    assert(fmt(STR("{:<10}"), Range(array{1})) == STR("[1]       "));
    assert(fmt(STR("{:>10}"), Range(array{2})) == STR("       [2]"));
    assert(fmt(STR("{:^11}"), Range(array{3, 4})) == STR("  [3, 4]   "));
    assert(fmt(STR("{:*<10}"), Range(array{5, 6})) == STR("[5, 6]****"));
    assert(fmt(STR("{0:=^{1}}"), Range(array{7, 8, 9}), 20) == STR("=====[7, 8, 9]======"));
    assert(fmt(STR("{:_>{}}"), Range(array{10, 11, 12}), 30) == STR("__________________[10, 11, 12]"));
    assert(fmt(STR("{::^20}"), Range(array{13})) == STR("[         13         ]"));

    // Check 'n' option.
    assert(fmt(STR("{:20n}"), Range(array{0})) == STR("0                   "));
    assert(fmt(STR("{:<10n}"), Range(array{1})) == STR("1         "));
    assert(fmt(STR("{:>10n}"), Range(array{2})) == STR("         2"));
    assert(fmt(STR("{:^11n}"), Range(array{3, 4})) == STR("   3, 4    "));
    assert(fmt(STR("{:*<10n}"), Range(array{5, 6})) == STR("5, 6******"));
    assert(fmt(STR("{:=^{}n}"), Range(array{7, 8, 9}), 20) == STR("======7, 8, 9======="));
    assert(fmt(STR("{0:_>{1}n}"), Range(array{10, 11, 12}), 30) == STR("____________________10, 11, 12"));
    assert(fmt(STR("{:>20n}"), Range(array{13})) == STR("                  13"));

    if constexpr (Range::proxy_ref != test::ProxyRef::yes) { // Check 'm' type.
        using Pair      = pair<int, char>;
        using PairRange = Range::template RebindElement<const Pair>;

        assert(fmt(STR("{:m}"), PairRange(array<Pair, 0>{})) == STR("{}"));
        assert(fmt(STR("{:m}"), PairRange(array{Pair{1, 'a'}, Pair{2, 'b'}})) == STR("{1: 'a', 2: 'b'}"));
        assert(fmt(STR("{:20m}"), PairRange(array{Pair{3, 'c'}, Pair{4, 'd'}})) == STR("{3: 'c', 4: 'd'}    "));
        assert(fmt(STR("{:%>20m}"), PairRange(array{Pair{5, 'e'}, Pair{6, 'f'}})) == STR("%%%%{5: 'e', 6: 'f'}"));
        assert(fmt(STR("{:_>20nm}"), PairRange(array{Pair{7, 'g'}, Pair{8, '\n'}})) == STR("_____7: 'g', 8: '\\n'"));
        assert(fmt(STR("{:_>20nm:}"), PairRange(array{Pair{7, 'g'}, Pair{8, '\n'}})) == STR("_____7: 'g', 8: '\\n'"));

        using Tuple      = tuple<const CharT*, int>;
        using TupleRange = Range::template RebindElement<const Tuple>;

        assert(fmt(STR("{:m}"), TupleRange(array<Tuple, 0>{})) == STR("{}"));
        assert(
            fmt(STR("{:m}"), TupleRange(array{Tuple{STR("A"), 1}, Tuple{STR("B"), 2}})) == STR(R"({"A": 1, "B": 2})"));
        assert(fmt(STR("{:20m}"), TupleRange(array{Tuple{STR("C"), 3}, Tuple{STR("D"), 4}}))
               == STR(R"({"C": 3, "D": 4}    )"));
        assert(fmt(STR("{:%>20m}"), TupleRange(array{Tuple{STR("\n"), 5}, Tuple{STR("\t"), 6}}))
               == STR(R"(%%{"\n": 5, "\t": 6})"));
        assert(fmt(STR("{:_>{}nm}"), TupleRange(array{Tuple{STR("\a"), 7}, Tuple{STR("\b"), 8}}), 24)
               == STR(R"(__"\u{7}": 7, "\u{8}": 8)"));
        assert(fmt(STR("{0:_>{1}nm:}"), TupleRange(array{Tuple{STR("AB\tC"), 9}, Tuple{STR("XY\nZ"), 10}}), 25)
               == STR(R"(__"AB\tC": 9, "XY\nZ": 10)"));
    }

    if constexpr (Range::proxy_ref != test::ProxyRef::yes) { // Check 's' and '?s' types.
        using CharRange = Range::template RebindElement<const CharT>;

        assert(fmt(STR("{:>s}"), CharRange(STR("Hello"sv))) == STR("Hello"));
        assert(fmt(STR("{:s}"), CharRange(STR("Hello"sv))) == STR("Hello"));
        assert(fmt(STR("{:10s}"), CharRange(STR("Hello"sv))) == STR("Hello     "));
        assert(fmt(STR("{:>20s}"), CharRange(STR("Hello"sv))) == STR("               Hello"));
        assert(fmt(STR("{:_^20s}"), CharRange(STR("Hello"sv))) == STR("_______Hello________"));

        assert(fmt(STR("{:^?s}"), CharRange(STR("Hell\no"sv))) == STR(R"("Hell\no")"));
        assert(fmt(STR("{:?s}"), CharRange(STR("Hel\tlo"sv))) == STR(R"("Hel\tlo")"));
        assert(fmt(STR("{:10?s}"), CharRange(STR("Hel\nlo"sv))) == STR(R"("Hel\nlo" )"));
        assert(fmt(STR("{:>{}?s}"), CharRange(STR("Hel\alo"sv)), 20) == STR(R"(        "Hel\u{7}lo")"));
#if defined(__clang__) || defined(__EDG__) // TRANSITION, DevCom-1318584
        assert(fmt(STR("{0:-^{1}?s}"), CharRange(STR("H\"ello"sv)), 20) == STR(R"(-----"H\"ello"------)"));
#else // ^^^ no workaround / workaround vvv
        assert(fmt(STR("{0:-^{1}?s}"), CharRange(STR("H\"ello"sv)), 20) == STR("-----\"H\\\"ello\"------"));
#endif // ^^^ workaround ^^^
    }

    // Check formatting with range-underlying-spec.
    assert(fmt(STR("{:20:b}"), Range(array{0})) == STR("[0]                 "));
    assert(fmt(STR("{:<10:#B}"), Range(array{1})) == STR("[0B1]     "));
    assert(fmt(STR("{:>{}:+d}"), Range(array{2}), 10) == STR("      [+2]"));
    assert(fmt(STR("{:^24:+#05}"), Range(array{3, 4})) == STR("     [+0003, +0004]     "));
    assert(fmt(STR("{0:_<{1}:^^{2}}"), Range(array{5, 6}), 20, 4) == STR("[^5^^, ^6^^]________"));
    assert(fmt(STR("{0:=^{1}}"), Range(array{7, 8, 9}), 20) == STR("=====[7, 8, 9]======"));
    assert(fmt(STR("{:_>{}}"), Range(array{10, 11, 12}), 30) == STR("__________________[10, 11, 12]"));

    { // Check formatting with range-underlying-spec when range_reference_t<R> has debug-enabled formatter.
        using StringRange = Range::template RebindElement<const CharT* const>;

        assert(fmt(STR("{}"), StringRange(array{STR("0"), STR("1"), STR("2")})) == STR(R"(["0", "1", "2"])"));
        assert(fmt(STR("{:}"), StringRange(array{STR("3"), STR("4"), STR("5")})) == STR(R"(["3", "4", "5"])"));
        assert(fmt(STR("{::}"), StringRange(array{STR("6"), STR("7"), STR("8")})) == STR(R"([6, 7, 8])"));
        assert(fmt(STR("{::?}"), StringRange(array{STR("9"), STR("10"), STR("11")})) == STR(R"(["9", "10", "11"])"));
        assert(fmt(STR("{::_^8?}"), StringRange(array{STR("12"), STR("13"), STR("14")}))
               == STR(R"([__"12"__, __"13"__, __"14"__])"));
        assert(fmt(STR("{::_^6}"), StringRange(array{STR("15"), STR("16"), STR("17")}))
               == STR(R"([__15__, __16__, __17__])"));
        assert(fmt(STR("{0:/^26:_^{2}}"), StringRange(array{STR("18"), STR("19"), STR("20")}), 8, 6)
               == STR(R"(/[__18__, __19__, __20__]/)"));
        assert(fmt(STR("{:/^{}n:_^6}"), StringRange(array{STR("18"), STR("19"), STR("20")}), 26)
               == STR(R"(//__18__, __19__, __20__//)"));
    }

    { // Check formatting range of ranges.
        using OuterRange = Range::template RebindElement<const Range>;

        assert(fmt(STR("{}"), OuterRange(array{Range(array{0, 1, 2}), Range(array{3, 4, 5}), Range(array{6, 7, 8})}))
               == STR("[[0, 1, 2], [3, 4, 5], [6, 7, 8]]"));
        assert(fmt(STR("{::n}"), OuterRange(array{Range(array{0, 1, 2}), Range(array{3, 4, 5}), Range(array{6, 7, 8})}))
               == STR("[0, 1, 2, 3, 4, 5, 6, 7, 8]"));
        assert(fmt(STR("{0:_>{2}n:n}"),
                   OuterRange(array{Range(array{0, 1, 2}), Range(array{3, 4, 5}), Range(array{6, 7, 8})}), 20, 30)
               == STR("_____0, 1, 2, 3, 4, 5, 6, 7, 8"));

        assert(fmt(STR("{:_<{}:*^{}:=^#{}x}"),
                   OuterRange(array{Range(array{0, 1, 2}), Range(array{3, 4, 5}), Range(array{6, 7, 8})}), 90, 25, 5)
               == STR("[**[=0x0=, =0x1=, =0x2=]**, **[=0x3=, =0x4=, =0x5=]**, **[=0x6=, =0x7=, =0x8=]**]_________"));
    }

    { // Check formatting range of 'FormatNextArg's.
        using TestRange = Range::template RebindElement<const FormatNextArg>;

        assert(fmt(STR("{}"), TestRange(array<FormatNextArg, 1>{}), 0) == STR("[0]"));
        assert(fmt(STR("{}"), TestRange(array<FormatNextArg, 1>{}), STR("mmm")) == STR("[mmm]"));
        assert(fmt(STR("{}"), TestRange(array<FormatNextArg, 1>{}), FormatAsX{}) == STR("[handle: X]"));
        assert(fmt(STR("{}"), TestRange(array<FormatNextArg, 2>{}), FormatAsX{}) == STR("[handle: X, handle: X]"));
    }

    { // Check formatting range of custom type with 'set_debug_format'.
        using TestRange = Range::template RebindElement<const Debuggable>;

        assert(fmt(STR("{}"), TestRange(array<Debuggable, 1>{})) == STR("[debug]"));
        assert(fmt(STR("{:}"), TestRange(array<Debuggable, 2>{})) == STR("[debug, debug]"));
        assert(fmt(STR("{::}"), TestRange(array<Debuggable, 3>{})) == STR("[regular, regular, regular]"));
        assert(fmt(STR("{::D}"), TestRange(array<Debuggable, 4>{})) == STR("[debug, debug, debug, debug]"));
    }
}

template <class CharT, class Range>
void check_incorrect_use_of_range_formatter() {
    ExpectFormatError<CharT> fmt;

    const array vals = {1, 2, 3};

    { // damaged fields
        fmt(STR("{"), Range(vals));
        fmt(STR("{}}"), Range(vals));
        fmt(STR("{{}"), Range(vals));
        fmt(STR("{:}}"), Range(vals));
    }

    { // invalid range-format-spec
        fmt(STR("{:*,20}"), Range(vals));
        fmt(STR("{:nn}"), Range(vals));
        fmt(STR("{:x:}"), Range(vals));
        fmt(STR("{:x}"), Range(vals));
        fmt(STR("{:mx}"), Range(vals));
        fmt(STR("{:ns}"), Range(vals));
        fmt(STR("{:n?s}"), Range(vals));
        fmt(STR("{:m}"), Range(vals));
        fmt(STR("{:s:x}"), Range(vals));
        fmt(STR("{:sz}"), Range(vals));
        fmt(STR("{:?s:x}"), Range(vals));
        fmt(STR("{:?}"), Range(vals));
        fmt(STR("{:?z}"), Range(vals));
        fmt(STR("{:mn}"), Range(vals));
    }

    { // std-format-spec instead of range-format-spec
        fmt(STR("{:+}"), Range(vals));
        fmt(STR("{:-}"), Range(vals));
        fmt(STR("{: }"), Range(vals));
        fmt(STR("{:#}"), Range(vals));
        fmt(STR("{:.2}"), Range(vals));
        fmt(STR("{:L}"), Range(vals));
    }

    { // invalid range-underlying-spec
        fmt(STR("{::=m}"), Range(vals));
        fmt(STR("{::##x}"), Range(vals));
        fmt(STR("{::L#o}"), Range(vals));
    }

    { // mixed invalid specs
        fmt(STR("{:+#}"), Range(vals));
        fmt(STR("{:::::}"), Range(vals));
        fmt(STR("{:-.3}"), Range(vals));
        fmt(STR("{: #}"), Range(vals));
        fmt(STR("{:#.4}"), Range(vals));
        fmt(STR("{:.3o}"), Range(vals));
        fmt(STR("{:X}"), Range(vals));
        fmt(STR("{:invalid for sure}"), Range(vals));
    }
}

struct range_formatter_check_instantiator {
    template <ranges::input_range R>
    static void call() {
        check_range_formatter<FormatFn, char, R>();
        check_range_formatter<FormatFn, wchar_t, R>();

        check_range_formatter<VFormatFn, char, R>();
        check_range_formatter<VFormatFn, wchar_t, R>();

        check_incorrect_use_of_range_formatter<char, R>();
        check_incorrect_use_of_range_formatter<wchar_t, R>();
    }
};

void instantiation_test() {
#ifdef TEST_EVERYTHING
    test_in<range_formatter_check_instantiator, const int>();
#else // ^^^ test all input range permutations / test only "interesting" permutations vvv
    range_formatter_check_instantiator::call<test::range<test::input, const int>>();
    range_formatter_check_instantiator::call<test::range<test::fwd, const int>>();
    range_formatter_check_instantiator::call<test::range<test::bidi, const int>>();
    range_formatter_check_instantiator::call<test::range<test::random, const int>>();
    range_formatter_check_instantiator::call<test::range<test::contiguous, const int>>();
#endif // TEST_EVERYTHING
}

template <class CharT>
constexpr bool check_other_functions() {
    using Sv = basic_string_view<CharT>;
    range_formatter<int, CharT> fmt;

    // Check 'set_separator'
    fmt.set_separator(STR("; "sv));
    static_assert(same_as<decltype(fmt.set_separator(STR(""sv))), void>);
    static_assert(noexcept(fmt.set_separator(declval<Sv>())));

    // Check 'set_brackets'
    fmt.set_brackets(STR("<"sv), STR(">"sv));
    static_assert(same_as<decltype(fmt.set_brackets(STR(""sv), STR(""sv))), void>);
    static_assert(noexcept(fmt.set_brackets(declval<Sv>(), declval<Sv>())));

    // Check 'underlying'
    [[maybe_unused]] same_as<formatter<int, CharT>&> decltype(auto) u1       = fmt.underlying();
    [[maybe_unused]] same_as<const formatter<int, CharT>&> decltype(auto) u2 = as_const(fmt).underlying();
    static_assert(noexcept(fmt.underlying()));
    static_assert(noexcept(as_const(fmt).underlying()));

    return true;
}

template <class CharT>
void check_range_formatter_with_arbitrary_format_context(basic_format_context<CharT*, CharT>& ctx) { // COMPILE-ONLY
    range_formatter<int, CharT> fmt;
    fmt.format(array{0}, ctx);
}

template void check_range_formatter_with_arbitrary_format_context(basic_format_context<char*, char>&);
template void check_range_formatter_with_arbitrary_format_context(basic_format_context<wchar_t*, wchar_t>&);

template <class T, class CharT>
concept CanInstantiateRangeFormatter = requires { typename range_formatter<T, CharT>; };

template <class CharT>
consteval bool check_constraints() {
    static_assert(CanInstantiateRangeFormatter<int, CharT>);
    static_assert(!CanInstantiateRangeFormatter<const int, CharT>);
    static_assert(!CanInstantiateRangeFormatter<int&, CharT>);
    static_assert(!CanInstantiateRangeFormatter<int&&, CharT>);
    static_assert(!CanInstantiateRangeFormatter<const int&&, CharT>);
    static_assert(!CanInstantiateRangeFormatter<const volatile int&&, CharT>);

    struct NotFormattable {};
    static_assert(!CanInstantiateRangeFormatter<NotFormattable, CharT>);

    return true;
}

template <class T>
struct WrappedVector : vector<T> {
    using vector<T>::vector;
};

template <class T, class CharT>
struct std::formatter<WrappedVector<T>, CharT> {
public:
    template <class ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        underlying.set_brackets(STR("<"sv), STR(">"sv));
        underlying.set_separator(STR("|"sv));
        return underlying.parse(ctx);
    }

    template <class FormatContext>
    auto format(const WrappedVector<T>& rng, FormatContext& ctx) const {
        return underlying.format(rng, ctx);
    }

private:
    range_formatter<T, CharT> underlying;
};

template <class CharT>
void check_runtime_behavior_of_setters() {
    {
        const WrappedVector<int> v1{11, 22, 33, 44};
        assert(format(STR("{}"), v1) == STR("<11|22|33|44>"));
        assert(format(STR("{:}"), v1) == STR("<11|22|33|44>"));
        assert(format(STR("{:n}"), v1) == STR("11|22|33|44"));
    }

    {
        const WrappedVector<pair<int, char>> v2{{10, 'x'}, {20, 'y'}, {30, 'z'}};
        assert(format(STR("{}"), v2) == STR("<(10, 'x')|(20, 'y')|(30, 'z')>"));
        assert(format(STR("{:}"), v2) == STR("<(10, 'x')|(20, 'y')|(30, 'z')>"));
        assert(format(STR("{:n}"), v2) == STR("(10, 'x')|(20, 'y')|(30, 'z')"));
        assert(format(STR("{:m}"), v2) == STR("{10: 'x', 20: 'y', 30: 'z'}"));
        assert(format(STR("{:nm}"), v2) == STR("10: 'x', 20: 'y', 30: 'z'"));
    }
}

int main() {
    instantiation_test();

    static_assert(check_other_functions<char>());
    static_assert(check_other_functions<wchar_t>());
    check_other_functions<char>();
    check_other_functions<wchar_t>();

    static_assert(check_constraints<char>());
    static_assert(check_constraints<wchar_t>());

    check_runtime_behavior_of_setters<char>();
    check_runtime_behavior_of_setters<wchar_t>();
}
