// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <cctype>
#include <charconv>
#include <cstddef>
#include <cstdint>
#include <cwctype>
#include <format>
#include <functional>
#include <ranges>
#include <string>
#include <string_view>
#include <system_error>

#include <test_format_support.hpp>

#define STR(Str) TYPED_LITERAL(CharT, Str)

using namespace std;

template <class CharT>
basic_string<CharT> pointer_to_string(const void* ptr) {
    constexpr size_t hexits = 2 * sizeof(void*);
    char buffer[hexits];
    auto [end, ec] = to_chars(buffer, buffer + hexits, reinterpret_cast<uintptr_t>(ptr), 16);
    assert(ec == errc{});
    basic_string<CharT> out;
    ranges::transform(buffer, end, back_inserter(out), [](char c) { return static_cast<CharT>(c); });
    return out;
}

template <class CharT>
basic_string<CharT> string_to_uppercase(const basic_string<CharT>& str) {
    basic_string<CharT> out;
    ranges::transform(str, back_inserter(out), [](auto c) {
        if constexpr (same_as<CharT, char>) {
            return static_cast<char>(toupper(static_cast<unsigned char>(c)));
        } else if constexpr (same_as<CharT, wchar_t>) {
            return static_cast<wchar_t>(towupper(static_cast<wint_t>(c)));
        }
    });
    return out;
}

template <ranges::input_range R, class T>
    requires indirect_binary_predicate<ranges::equal_to, ranges::iterator_t<R>, const T*>
constexpr bool all_equal_to(R&& r, const T& val) {
    return ranges::all_of(r, [&val](const auto& elem) { return val == elem; });
}

static_assert(all_equal_to(array{1, 1, 1, 1}, 1));
static_assert(!all_equal_to(array{1, 1, 1, 1}, 2));
static_assert(!all_equal_to(array{1, 1, 1, 2}, 1));

template <class CharT, template <class> class Fmt>
void check_pointer_formatter() {
    Fmt<CharT> fmt;
    const int variable       = 0;
    const void* const ptr    = &variable;
    const auto rep           = pointer_to_string<CharT>(ptr);
    const auto lowercase_rep = STR("0x") + rep;
    const auto uppercase_rep = STR("0X") + string_to_uppercase(rep);

    { // type == '\0'
        assert(fmt(STR("{}"), ptr) == lowercase_rep);
        assert(fmt(STR("{:}"), ptr) == lowercase_rep);
        assert(fmt(STR("{0}"), ptr) == lowercase_rep);
        assert(fmt(STR("{0:}"), ptr) == lowercase_rep);
    }

    { // align only
        assert(fmt(STR("{:<}"), ptr) == lowercase_rep);
        assert(fmt(STR("{:^}"), ptr) == lowercase_rep);
        assert(fmt(STR("{:>}"), ptr) == lowercase_rep);
    }

    { // fill-and-align only
        assert(fmt(STR("{:_<}"), ptr) == lowercase_rep);
        assert(fmt(STR("{:-^}"), ptr) == lowercase_rep);
        assert(fmt(STR("{:=>}"), ptr) == lowercase_rep);
    }

    { // type == 'p' || type == 'P'
        assert(fmt(STR("{:p}"), ptr) == lowercase_rep);
        assert(fmt(STR("{:P}"), ptr) == uppercase_rep);
    }

    { // leading zero and type
        assert(fmt(STR("{:0}"), ptr) == lowercase_rep);
        assert(fmt(STR("{:0p}"), ptr) == lowercase_rep);
        assert(fmt(STR("{:0P}"), ptr) == uppercase_rep);
    }

    { // width only
        const int pad  = static_cast<int>(25 - lowercase_rep.size());
        const auto str = fmt(STR("{:25}"), ptr);
        assert(all_equal_to(views::take(str, pad), STR(' ')));
        assert(ranges::equal(views::drop(str, pad), lowercase_rep));
    }

    { // width && align
        const auto str = fmt(STR("{:<31}"), ptr);
        assert(ranges::equal(views::take(str, static_cast<ptrdiff_t>(lowercase_rep.size())), lowercase_rep));
        assert(all_equal_to(views::drop(str, static_cast<ptrdiff_t>(lowercase_rep.size())), ' '));
    }

    { // width && fill-and-align (2)
        const int w    = 22;
        const int pad  = static_cast<int>(w - lowercase_rep.size());
        const auto str = fmt(STR("{:*^{}}"), ptr, w);
        basic_string_view<CharT> v{str};

        const int left_pad = pad / 2;
        assert(all_equal_to(views::take(v, left_pad), STR('*')));
        v.remove_prefix(static_cast<size_t>(left_pad));
        assert(v.starts_with(lowercase_rep));
        v.remove_prefix(lowercase_rep.size());
        assert(all_equal_to(v, STR('*')));
    }

    { // leading zero && width && type == 'p'
        const int w    = 35;
        const auto str = fmt(STR("{:0{}p}"), ptr, w);
        basic_string_view<CharT> v{str};
        assert(v.starts_with(STR("0x")));
        v.remove_prefix(2);
        const int zero_count = w - static_cast<int>(lowercase_rep.size());
        assert(all_equal_to(views::take(v, zero_count), STR('0')));
        v.remove_prefix(static_cast<size_t>(zero_count));
        assert(ranges::equal(v, views::drop(lowercase_rep, 2)));
    }

    { // leading zero && width && type == 'P'
        const int w    = 39;
        const auto str = fmt(STR("{1:0{0}P}"), w, ptr);
        basic_string_view<CharT> v{str};
        assert(v.starts_with(STR("0X")));
        v.remove_prefix(2);
        const int zero_count = w - static_cast<int>(uppercase_rep.size());
        assert(all_equal_to(views::take(v, zero_count), STR('0')));
        v.remove_prefix(static_cast<size_t>(zero_count));
        assert(ranges::equal(v, views::drop(uppercase_rep, 2)));
    }

    { // align && leading zero (should have no effect) && width && type == 'p'
        const int w        = 42;
        const int pad      = static_cast<int>(w - lowercase_rep.size());
        const auto str     = fmt(STR("{:^0{}p}"), ptr, w);
        const int left_pad = pad / 2;
        basic_string_view<CharT> v{str};
        assert(all_equal_to(views::take(v, left_pad), STR(' ')));
        v.remove_prefix(static_cast<size_t>(left_pad));
        assert(v.starts_with(lowercase_rep));
        v.remove_prefix(lowercase_rep.size());
        assert(all_equal_to(v, STR(' ')));
    }

    { // fill-and-align && leading zero (should have no effect) && width && type == 'P'
        const int w    = 44;
        const int pad  = static_cast<int>(w - uppercase_rep.size());
        const auto str = fmt(STR("{1:#>0{0}P}"), w, ptr);
        basic_string_view<CharT> v{str};
        assert(all_equal_to(views::take(v, pad), STR('#')));
        v.remove_prefix(static_cast<size_t>(pad));
        assert(ranges::equal(v, uppercase_rep));
    }
}

template <class CharT, template <class> class Fmt>
void check_nullptr_t_formatter() {
    Fmt<CharT> fmt;

    { // type == '\0'
        assert(fmt(STR("{}"), nullptr) == STR("0x0"));
        assert(fmt(STR("{:}"), nullptr) == STR("0x0"));
        assert(fmt(STR("{0}"), nullptr) == STR("0x0"));
        assert(fmt(STR("{0:}"), nullptr) == STR("0x0"));
    }

    { // align only
        assert(fmt(STR("{:<}"), nullptr) == STR("0x0"));
        assert(fmt(STR("{:^}"), nullptr) == STR("0x0"));
        assert(fmt(STR("{:>}"), nullptr) == STR("0x0"));
    }

    { // fill-and-align only
        assert(fmt(STR("{:_<}"), nullptr) == STR("0x0"));
        assert(fmt(STR("{:-^}"), nullptr) == STR("0x0"));
        assert(fmt(STR("{:=>}"), nullptr) == STR("0x0"));
    }

    { // type == 'p' || type == 'P'
        assert(fmt(STR("{:p}"), nullptr) == STR("0x0"));
        assert(fmt(STR("{:P}"), nullptr) == STR("0X0"));
    }

    { // leading zero and type
        assert(fmt(STR("{:0}"), nullptr) == STR("0x0"));
        assert(fmt(STR("{:0p}"), nullptr) == STR("0x0"));
        assert(fmt(STR("{:0P}"), nullptr) == STR("0X0"));
    }

    { // width only
        assert(fmt(STR("{:5}"), nullptr) == STR("  0x0"));
        assert(fmt(STR("{:7}"), nullptr) == STR("    0x0"));
        assert(fmt(STR("{:11}"), nullptr) == STR("        0x0"));
    }

    { // width && align
        assert(fmt(STR("{:<5}"), nullptr) == STR("0x0  "));
        assert(fmt(STR("{:^7}"), nullptr) == STR("  0x0  "));
        assert(fmt(STR("{:>11}"), nullptr) == STR("        0x0"));
    }

    { // width && fill-and-align
        assert(fmt(STR("{:#<5}"), nullptr) == STR("0x0##"));
        assert(fmt(STR("{:*^7}"), nullptr) == STR("**0x0**"));
        assert(fmt(STR("{:=>11}"), nullptr) == STR("========0x0"));
    }

    { // leading zero && width && type == '\0'
        assert(fmt(STR("{:05}"), nullptr) == STR("0x000"));
        assert(fmt(STR("{:07}"), nullptr) == STR("0x00000"));
        assert(fmt(STR("{:011}"), nullptr) == STR("0x000000000"));
    }

    { // leading zero && width && type == 'p'
        assert(fmt(STR("{:05p}"), nullptr) == STR("0x000"));
        assert(fmt(STR("{:07p}"), nullptr) == STR("0x00000"));
        assert(fmt(STR("{:011p}"), nullptr) == STR("0x000000000"));
    }

    { // leading zero && width && type == 'P'
        assert(fmt(STR("{:05P}"), nullptr) == STR("0X000"));
        assert(fmt(STR("{:07P}"), nullptr) == STR("0X00000"));
        assert(fmt(STR("{:011P}"), nullptr) == STR("0X000000000"));
    }

    { // align && leading zero (should have no effect) && width && type in ('\0', 'p', 'P')
        assert(fmt(STR("{:<05}"), nullptr) == STR("0x0  "));
        assert(fmt(STR("{:^07p}"), nullptr) == STR("  0x0  "));
        assert(fmt(STR("{:>011P}"), nullptr) == STR("        0X0"));
    }

    { // fill-and-align && leading zero (should have no effect) && width && type in ('\0', 'p', 'P')
        assert(fmt(STR("{:!<05}"), nullptr) == STR("0x0!!"));
        assert(fmt(STR("{:@^07p}"), nullptr) == STR("@@0x0@@"));
        assert(fmt(STR("{:#>011P}"), nullptr) == STR("########0X0"));
    }
}

template <class CharT, class PtrType>
void check_invalid_format_spec() {
    ExpectFormatError<CharT> fmt;
    PtrType ptr = nullptr;

    { // damaged fields
        fmt(STR("{"), ptr);
        fmt(STR("{}}"), ptr);
        fmt(STR("{{}"), ptr);
        fmt(STR("{:}}"), ptr);
    }

    { // sign || '#' || precision || 'L'
        fmt(STR("{:+}"), ptr);
        fmt(STR("{:-}"), ptr);
        fmt(STR("{: }"), ptr);
        fmt(STR("{:#}"), ptr);
        fmt(STR("{:.2}"), ptr);
        fmt(STR("{:L}"), ptr);
    }

    { // mixed invalid specs
        fmt(STR("{:+#}"), ptr);
        fmt(STR("{:-.3}"), ptr);
        fmt(STR("{: #}"), ptr);
        fmt(STR("{:#.4}"), ptr);
        fmt(STR("{:.3o}"), ptr);
        fmt(STR("{:X}"), ptr);
        fmt(STR("{:invalid for sure}"), ptr);
    }
}

template <class CharT>
void test() {
    check_pointer_formatter<CharT, FormatFn>();
    check_pointer_formatter<CharT, VFormatFn>();
    check_nullptr_t_formatter<CharT, FormatFn>();
    check_nullptr_t_formatter<CharT, VFormatFn>();
    check_invalid_format_spec<CharT, const void*>();
    check_invalid_format_spec<CharT, nullptr_t>();
}

int main() {
    test<char>();
    test<wchar_t>();
}
