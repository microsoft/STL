// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <format>
#include <memory_resource>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#include <test_format_support.hpp>

#define STR(Str) TYPED_LITERAL(CharT, Str)

using namespace std;

template <class F>
concept DebugEnabledSpecialization = is_default_constructible_v<F> && requires(F& fmt) {
    { fmt.set_debug_format() } noexcept -> same_as<void>;
};

template <class CharT>
consteval bool check_debug_enabled_specializations() {
    static_assert(DebugEnabledSpecialization<formatter<char, CharT>>);
    static_assert(DebugEnabledSpecialization<formatter<CharT, CharT>>);
    static_assert(DebugEnabledSpecialization<formatter<CharT*, CharT>>);
    static_assert(DebugEnabledSpecialization<formatter<const CharT*, CharT>>);
    static_assert(DebugEnabledSpecialization<formatter<CharT[3], CharT>>);
    static_assert(DebugEnabledSpecialization<formatter<basic_string<CharT>, CharT>>);
    static_assert(DebugEnabledSpecialization<formatter<pmr::basic_string<CharT>, CharT>>);
    static_assert(DebugEnabledSpecialization<formatter<basic_string_view<CharT>, CharT>>);

    static_assert(!DebugEnabledSpecialization<formatter<signed char, CharT>>);
    static_assert(!DebugEnabledSpecialization<formatter<short, CharT>>);
    static_assert(!DebugEnabledSpecialization<formatter<int, CharT>>);
    static_assert(!DebugEnabledSpecialization<formatter<long, CharT>>);
    static_assert(!DebugEnabledSpecialization<formatter<long long, CharT>>);

    static_assert(!DebugEnabledSpecialization<formatter<unsigned char, CharT>>);
    // NB: formatter<unsigned short, CharT> is special case, see below
    static_assert(!DebugEnabledSpecialization<formatter<unsigned int, CharT>>);
    static_assert(!DebugEnabledSpecialization<formatter<unsigned long, CharT>>);
    static_assert(!DebugEnabledSpecialization<formatter<unsigned long long, CharT>>);

    static_assert(!DebugEnabledSpecialization<formatter<bool, CharT>>);
    static_assert(!DebugEnabledSpecialization<formatter<nullptr_t, CharT>>);
    static_assert(!DebugEnabledSpecialization<formatter<void*, CharT>>);
    static_assert(!DebugEnabledSpecialization<formatter<const void*, CharT>>);

    // NB: wchar_t might be defined as a typedef for unsigned short (with '/Zc:wchar_t-')
    static_assert(DebugEnabledSpecialization<formatter<unsigned short, CharT>> == same_as<CharT, unsigned short>);

    return true;
}

template <class CharT>
struct Holder {
    char narrow_ch;
    CharT ch;
    const CharT* const_ptr;
    basic_string<CharT> str;
    CharT* non_const_ptr;
    basic_string_view<CharT> str_view;
    CharT arr[11];
};

// holder-format-specs:
//   member debug-format(opt)
// member:
//   0 1 2 ... N (index of member object, single digit)
// debug-format:
//   $ (use debug format)
template <class CharT>
struct std::formatter<Holder<CharT>, CharT> {
public:
    constexpr auto parse(basic_format_parse_context<CharT>& ctx) {
        auto it = ctx.begin();
        if (it == ctx.end() || *it == STR('}')) {
            throw format_error{"Invalid holder-format-specs."};
        }

        if (STR('0') <= *it && *it <= STR('9')) {
            member_index = *it - STR('0');
        } else {
            throw format_error{"Expected member index in holder-format-specs."};
        }

        ++it;
        if (it == ctx.end() || *it == STR('}')) {
            return it;
        }

        if (*it == '$') {
            switch (member_index) {
            case 0:
                fmt0.set_debug_format();
                break;
            case 1:
                fmt1.set_debug_format();
                break;
            case 2:
                fmt2.set_debug_format();
                break;
            case 3:
                fmt3.set_debug_format();
                break;
            case 4:
                fmt4.set_debug_format();
                break;
            case 5:
                fmt5.set_debug_format();
                break;
            case 6:
                fmt6.set_debug_format();
                break;
            }
        } else {
            throw format_error{"Unexpected symbols in holder-format-specs."};
        }

        ++it;
        if (it != ctx.end() && *it != STR('}')) {
            throw format_error{"Expected '}' at the end of holder-format-specs."};
        }

        return it;
    }

    template <class FormatContext>
    auto format(const Holder<CharT>& val, FormatContext& ctx) const {
        switch (member_index) {
        case 0:
            return fmt0.format(val.narrow_ch, ctx);
        case 1:
            return fmt1.format(val.ch, ctx);
        case 2:
            return fmt2.format(val.const_ptr, ctx);
        case 3:
            return fmt3.format(val.str, ctx);
        case 4:
            return fmt4.format(val.non_const_ptr, ctx);
        case 5:
            return fmt5.format(val.str_view, ctx);
        case 6:
            return fmt6.format(val.arr, ctx);
        }

        unreachable();
    }

private:
    int member_index{-1};

    formatter<char, CharT> fmt0;
    formatter<CharT, CharT> fmt1;
    formatter<const CharT*, CharT> fmt2;
    formatter<basic_string<CharT>, CharT> fmt3;
    formatter<CharT*, CharT> fmt4;
    formatter<basic_string_view<CharT>, CharT> fmt5;
    formatter<CharT[11], CharT> fmt6;
};

template <class CharT>
void check_set_debug_format_function() {
    Holder<CharT> val;

    val.narrow_ch     = '\t';
    val.ch            = STR('\t');
    val.const_ptr     = STR("const\tCharT\t*");
    val.str           = STR("basic\tstring");
    val.non_const_ptr = val.str.data();
    val.str_view      = STR("basic\tstring\tview");
    ranges::copy(STR("CharT\t[11]\0"sv), val.arr);

    assert(format(STR("{:0}"), val) == STR("\t"));
    assert(format(STR("{:1}"), val) == STR("\t"));
    assert(format(STR("{:2}"), val) == STR("const\tCharT\t*"));
    assert(format(STR("{:3}"), val) == STR("basic\tstring"));
    assert(format(STR("{:4}"), val) == STR("basic\tstring"));
    assert(format(STR("{:5}"), val) == STR("basic\tstring\tview"));
    assert(format(STR("{:6}"), val) == STR("CharT\t[11]"));

    assert(format(STR("{:0$}"), val) == STR(R"('\t')"));
    assert(format(STR("{:1$}"), val) == STR(R"('\t')"));
    assert(format(STR("{:2$}"), val) == STR(R"("const\tCharT\t*")"));
    assert(format(STR("{:3$}"), val) == STR(R"("basic\tstring")"));
    assert(format(STR("{:4$}"), val) == STR(R"("basic\tstring")"));
    assert(format(STR("{:5$}"), val) == STR(R"("basic\tstring\tview")"));
    assert(format(STR("{:6$}"), val) == STR(R"("CharT\t[11]")"));
}

void set_debug_format(auto&) {}

struct name_lookup_in_formatter_checker : formatter<int> {
    auto parse(auto& ctx) { // COMPILE-ONLY
        set_debug_format(*this);
        return ctx.begin();
    }
};

int main() {
    static_assert(check_debug_enabled_specializations<char>());
    static_assert(check_debug_enabled_specializations<wchar_t>());

    check_set_debug_format_function<char>();
    check_set_debug_format_function<wchar_t>();
}
