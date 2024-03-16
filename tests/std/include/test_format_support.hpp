// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <cassert>
#include <concepts>
#include <cstddef>
#include <format>
#include <iterator>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

// copied from the string_view tests
template <typename CharT>
struct choose_literal; // not defined

template <>
struct choose_literal<char> {
    static constexpr const char* choose(const char* s, const wchar_t*) {
        return s;
    }

    static constexpr char choose(char c, wchar_t) {
        return c;
    }

    static constexpr std::string_view choose(std::string_view sv, std::wstring_view) {
        return sv;
    }
};

template <>
struct choose_literal<wchar_t> {
    static constexpr const wchar_t* choose(const char*, const wchar_t* s) {
        return s;
    }

    static constexpr wchar_t choose(char, wchar_t c) {
        return c;
    }

    static constexpr std::wstring_view choose(std::string_view, std::wstring_view sv) {
        return sv;
    }
};

#define TYPED_LITERAL(CharT, Literal) (choose_literal<CharT>::choose(Literal, L##Literal))

template <typename CharT>
struct noop_testing_callbacks {
    constexpr void _On_align(std::_Fmt_align) {}
    constexpr void _On_fill(std::basic_string_view<CharT>) {}
    constexpr void _On_width(unsigned int) {}
    constexpr void _On_dynamic_width(std::size_t) {}
    constexpr void _On_dynamic_width(std::_Auto_id_tag) {}
    constexpr void _On_precision(unsigned int) {}
    constexpr void _On_dynamic_precision(std::size_t) {}
    constexpr void _On_dynamic_precision(std::_Auto_id_tag) {}
    constexpr void _On_sign(std::_Fmt_sign) {}
    constexpr void _On_hash() {}
    constexpr void _On_zero() {}
    constexpr void _On_localized() {}
    constexpr void _On_type(CharT) {}
};

template <typename CharT>
struct testing_callbacks {
    std::_Fmt_align expected_alignment = std::_Fmt_align::_None;
    std::_Fmt_sign expected_sign       = std::_Fmt_sign::_None;
    std::basic_string_view<CharT> expected_fill{};
    int expected_width                     = -1;
    std::size_t expected_dynamic_width     = static_cast<std::size_t>(-1);
    bool expected_auto_dynamic_width       = false;
    int expected_precision                 = -1;
    std::size_t expected_dynamic_precision = static_cast<std::size_t>(-1);
    bool expected_auto_dynamic_precision   = false;
    bool expected_hash                     = false;
    bool expected_zero                     = false;
    bool expected_localized                = false;
    CharT expected_type                    = '\0';

    constexpr void _On_align(std::_Fmt_align aln) {
        assert(aln == expected_alignment);
    }
    constexpr void _On_fill(std::basic_string_view<CharT> str_view) {
        assert(str_view == expected_fill);
    }
    constexpr void _On_width(int width) {
        assert(width == expected_width);
    }
    constexpr void _On_dynamic_width(std::size_t id) {
        assert(id == expected_dynamic_width);
    }
    constexpr void _On_dynamic_width(std::_Auto_id_tag) {
        assert(expected_auto_dynamic_width);
    }
    constexpr void _On_precision(int pre) {
        assert(pre == expected_precision);
    }
    constexpr void _On_dynamic_precision(std::size_t id) {
        assert(id == expected_dynamic_precision);
    }
    constexpr void _On_dynamic_precision(std::_Auto_id_tag) {
        assert(expected_auto_dynamic_precision);
    }
    constexpr void _On_sign(std::_Fmt_sign sgn) {
        assert(sgn == expected_sign);
    }
    constexpr void _On_hash() {
        assert(expected_hash);
    }
    constexpr void _On_zero() {
        assert(expected_zero);
    }
    constexpr void _On_localized() {
        assert(expected_localized);
    }
    constexpr void _On_type(CharT type) {
        assert(type == expected_type);
    }
};
template <typename CharT>
testing_callbacks(std::_Fmt_align, std::basic_string_view<CharT>) -> testing_callbacks<CharT>;

struct testing_arg_id_callbacks {
    constexpr void _On_auto_id() {}
    constexpr void _On_manual_id(std::size_t) {}
};

template <typename CharT, typename callback_type>
void test_parse_helper(const CharT* (*func)(const CharT*, const CharT*, callback_type&&),
    std::basic_string_view<CharT> view, bool err_expected = false,
    typename std::basic_string_view<CharT>::size_type expected_end_position = std::basic_string_view<CharT>::npos,
    callback_type&& callbacks                                               = {}) {
    try {
        auto end = func(view.data(), view.data() + view.size(), std::move(callbacks));
        if (expected_end_position == std::basic_string_view<CharT>::npos) {
            expected_end_position = view.size();
        }
        assert(end == view.data() + expected_end_position);
        assert(!err_expected);
    } catch (const std::format_error&) {
        assert(err_expected);
    }
}

template <class CharT>
struct FormatFn {
    template <class... Args>
    [[nodiscard]] auto operator()(
        const std::basic_format_string<CharT, std::type_identity_t<Args>...> str, Args&&... args) const {
        return std::format(str, std::forward<Args>(args)...);
    }
};

template <class CharT>
struct VFormatFn {
    template <class... Args>
    [[nodiscard]] auto operator()(const std::basic_string_view<CharT> str, Args&&... args) const {
        if constexpr (std::same_as<CharT, char>) {
            return std::vformat(str, std::make_format_args(args...));
        } else {
            return std::vformat(str, std::make_wformat_args(args...));
        }
    }
};

template <class CharT>
struct ExpectFormatError {
private:
    VFormatFn<CharT> base;

public:
    template <class... Args>
    void operator()(const std::basic_string_view<CharT> str, Args&&... args) const {
        try {
            (void) base(str, std::forward<Args>(args)...);
            assert(false && "No exception.");
        } catch (const std::format_error&) {
            return;
        } catch (...) {
            assert(false && "Incorrect exception.");
        }
    }
};

template <class CharT>
struct MoveOnlyFormat {
private:
    struct StringInserter {
        using iterator_category = std::output_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using container_type    = std::basic_string<CharT>;

        StringInserter()                            = default;
        StringInserter(StringInserter&&)            = default;
        StringInserter& operator=(StringInserter&&) = default;

        StringInserter(const StringInserter&)            = delete;
        StringInserter& operator=(const StringInserter&) = delete;

        StringInserter& operator=(const CharT val) {
            str.push_back(val);
            return *this;
        }

        StringInserter& operator*() {
            return *this;
        }

        StringInserter& operator++() {
            return *this;
        }

        StringInserter operator++(int) {
            return *this;
        }

        std::basic_string<CharT> str;
    };

public:
    static_assert(std::output_iterator<StringInserter, CharT>);
    static_assert(!std::copyable<StringInserter>);
    static_assert(std::movable<StringInserter>);

    template <class... Args>
    [[nodiscard]] auto operator()(
        const std::basic_format_string<CharT, std::type_identity_t<Args>...> str, Args&&... args) const {
        return std::format_to(StringInserter{}, str, std::forward<Args>(args)...).str;
    }
};
