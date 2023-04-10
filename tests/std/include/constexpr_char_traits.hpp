// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#include <cstdio>
#include <cwchar>

struct constexpr_char_traits {
    typedef char char_type;
    typedef long int_type;
    typedef long pos_type;
    typedef long off_type;
    typedef mbstate_t state_type;

    static constexpr int compare(const char* first1, const char* first2, size_t count) {
        for (; 0 < count; --count, ++first1, ++first2) {
            if (!eq(*first1, *first2)) {
                return lt(*first1, *first2) ? -1 : +1;
            }
        }

        return 0;
    }

    static constexpr size_t length(const char* first) {
        size_t count = 0;
        for (; !eq(*first, char()); ++first) {
            ++count;
        }

        return count;
    }

    static constexpr char* copy(char* first1, const char* first2, size_t count) {
        char* next = first1;
        for (; 0 < count; --count, ++next, ++first2) {
            assign(*next, *first2);
        }
        return first1;
    }

    static constexpr char* _Copy_s(char* first1, size_t, const char* first2, size_t count) {
        // let's just pretend :)
        return copy(first1, first2, count);
    }

    static constexpr const char* find(const char* first, size_t count, const char ch) {
        for (; 0 < count; --count, ++first) {
            if (eq(*first, ch)) {
                return first;
            }
        }

        return nullptr;
    }

    static constexpr char* move(char* first1, const char* first2, size_t count) {
        char* next = first1;
        if (first2 < next && next < first2 + count) {
            for (next += count, first2 += count; 0 < count; --count) {
                assign(*--next, *--first2);
            }
        } else {
            for (; 0 < count; --count, ++next, ++first2) {
                assign(*next, *first2);
            }
        }
        return first1;
    }

    static constexpr char* assign(char* first, size_t count, const char ch) {
        char* next = first;
        for (; 0 < count; --count, ++next) {
            assign(*next, ch);
        }

        return first;
    }

    static constexpr void assign(char& left, const char right) noexcept {
        left = right;
    }

    static constexpr bool eq(const char left, const char right) noexcept {
        return left == right;
    }

    static constexpr bool lt(const char left, const char right) noexcept {
        return left < right;
    }

    static constexpr char to_char_type(const int_type meta) noexcept {
        return static_cast<char>(meta);
    }

    static constexpr int_type to_int_type(const char ch) noexcept {
        return ch;
    }

    static constexpr bool eq_int_type(const int_type left, const int_type right) noexcept {
        return left == right;
    }

    static constexpr int_type not_eof(const int_type meta) noexcept {
        return meta != eof() ? meta : !eof();
    }

    static constexpr int_type eof() noexcept {
        return EOF;
    }
};
