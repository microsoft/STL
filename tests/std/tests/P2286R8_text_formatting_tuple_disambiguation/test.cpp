// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <format>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

#include <test_format_support.hpp>

using namespace std;

#define CSTR(Str) TYPED_LITERAL(CharT, Str)
#define STR(Str)  basic_string(CSTR(Str))
#define SV(Str)   basic_string_view(CSTR(Str))

// If a pair or tuple is not formattable as a tuple but is formattable as an input range,
// it should be formattable as a range.
namespace not_formatted_as_tuple {
    struct tag {};

    template <class T, class A>
    constexpr auto begin(tuple<tag, vector<T, A>>& t) noexcept {
        return get<1>(t).begin();
    }
    template <class T, class A>
    constexpr auto begin(const tuple<tag, vector<T, A>>& t) noexcept {
        return get<1>(t).begin();
    }

    template <class T, class A>
    constexpr auto end(tuple<tag, vector<T, A>>& t) noexcept {
        return get<1>(t).end();
    }
    template <class T, class A>
    constexpr auto end(const tuple<tag, vector<T, A>>& t) noexcept {
        return get<1>(t).end();
    }

    template <class T, class A>
    constexpr auto begin(pair<tag, vector<T, A>>& t) noexcept {
        return get<1>(t).begin();
    }
    template <class T, class A>
    constexpr auto begin(const pair<tag, vector<T, A>>& t) noexcept {
        return get<1>(t).begin();
    }

    template <class T, class A>
    constexpr auto end(pair<tag, vector<T, A>>& t) noexcept {
        return get<1>(t).end();
    }
    template <class T, class A>
    constexpr auto end(const pair<tag, vector<T, A>>& t) noexcept {
        return get<1>(t).end();
    }
} // namespace not_formatted_as_tuple

// If a pair or tuple is formattable both as a tuple and as an input range, it should be formattable as a tuple.
namespace formattable_as_tuple_and_range {
    struct tag {};

    template <class T, class A>
    constexpr auto begin(tuple<tag, vector<T, A>>& t) noexcept {
        return get<1>(t).begin();
    }
    template <class T, class A>
    constexpr auto begin(const tuple<tag, vector<T, A>>& t) noexcept {
        return get<1>(t).begin();
    }

    template <class T, class A>
    constexpr auto end(tuple<tag, vector<T, A>>& t) noexcept {
        return get<1>(t).end();
    }
    template <class T, class A>
    constexpr auto end(const tuple<tag, vector<T, A>>& t) noexcept {
        return get<1>(t).end();
    }

    template <class T, class A>
    constexpr auto begin(pair<tag, vector<T, A>>& t) noexcept {
        return get<1>(t).begin();
    }
    template <class T, class A>
    constexpr auto begin(const pair<tag, vector<T, A>>& t) noexcept {
        return get<1>(t).begin();
    }

    template <class T, class A>
    constexpr auto end(pair<tag, vector<T, A>>& t) noexcept {
        return get<1>(t).end();
    }
    template <class T, class A>
    constexpr auto end(const pair<tag, vector<T, A>>& t) noexcept {
        return get<1>(t).end();
    }
} // namespace formattable_as_tuple_and_range

template <class CharT>
struct std::formatter<formattable_as_tuple_and_range::tag, CharT> {
    constexpr auto parse(basic_format_parse_context<CharT>& parse_ctx) {
        if (parse_ctx.begin() != parse_ctx.end() && *parse_ctx.begin() != '}') {
            throw format_error{"empty specs expected"};
        }
        return parse_ctx.begin();
    }

    template <class FmtCtx>
    auto format(formattable_as_tuple_and_range::tag, FmtCtx& ctx) const {
        ctx.advance_to(ranges::copy(SV("test-tag"), ctx.out()).out);
        return ctx.out();
    }
};

template <class CharT>
void test() {
    {
        auto s = format(CSTR("{}"), make_tuple(not_formatted_as_tuple::tag{}, vector{3, 1, 4, 1, 5, 9}));
        assert(s == SV("[3, 1, 4, 1, 5, 9]"));
    }
    {
        auto s = format(CSTR("{}"), make_pair(not_formatted_as_tuple::tag{}, vector{3, 1, 4, 1, 5, 9}));
        assert(s == SV("[3, 1, 4, 1, 5, 9]"));
    }
    {
        auto s = format(CSTR("{}"), make_tuple(formattable_as_tuple_and_range::tag{}, vector{3, 1, 4, 1, 5, 9}));
        assert(s == SV("(test-tag, [3, 1, 4, 1, 5, 9])"));
    }
    {
        auto s = format(CSTR("{}"), make_pair(formattable_as_tuple_and_range::tag{}, vector{3, 1, 4, 1, 5, 9}));
        assert(s == SV("(test-tag, [3, 1, 4, 1, 5, 9])"));
    }
}

int main() {
    test<char>();
    test<wchar_t>();
}
