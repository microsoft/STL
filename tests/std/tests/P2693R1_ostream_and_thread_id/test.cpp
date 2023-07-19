// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <functional>
#include <ios>
#include <locale>
#include <sstream>
#include <string>
#include <thread>

using namespace std;

// copied from the 'test_format_support.hpp' header
template <class CharT>
struct choose_literal; // not defined

template <>
struct choose_literal<char> {
    static constexpr const char* choose(const char* s, const wchar_t*) {
        return s;
    }
};

template <>
struct choose_literal<wchar_t> {
    static constexpr const wchar_t* choose(const char*, const wchar_t* s) {
        return s;
    }
};

#define STR(Literal) (choose_literal<CharT>::choose(Literal, L##Literal))

template <class CharT>
void check_fmtflags_and_locale(thread::id id) {
    // changing fmtflags other than fill or align should not affect text representation of thread::id
    auto make_text_rep = [id](ios_base::fmtflags flags = {}, ios_base::fmtflags mask = {}) {
        basic_ostringstream<CharT> ss;
        ss.setf(flags, mask);
        ss << id;
        return ss.str();
    };

    // changing locale should not affect text representation of thread::id
    auto make_localized_text_rep = [id](const char* name) {
        basic_ostringstream<CharT> ss;
        try {
            ss.imbue(locale{name});
        } catch (...) {
        }
        ss << id;
        return ss.str();
    };

    // changing precision should not affect text representation of thread::id
    auto make_text_rep_with_precision = [id](int prec) {
        basic_ostringstream<CharT> ss;
        ss.precision(prec);
        ss << id;
        return ss.str();
    };

    const array<basic_string<CharT>, 14> reps = {
        make_text_rep(),
        make_text_rep(ios_base::fixed, ios_base::basefield),
        make_text_rep(ios_base::hex | ios_base::uppercase, ios_base::basefield),
        make_text_rep(ios_base::hex, ios_base::basefield),
        make_text_rep(ios_base::oct | ios_base::showbase, ios_base::basefield),
        make_text_rep(ios_base::oct, ios_base::basefield),
        make_text_rep(ios_base::showpos, ios_base::basefield),
        make_localized_text_rep(""),
        make_localized_text_rep("de-DE"),
        make_localized_text_rep("en-US"),
        make_localized_text_rep("pl-PL"),
        make_text_rep_with_precision(1),
        make_text_rep_with_precision(6),
        make_text_rep_with_precision(32),
    };

    // all text representations should be the same
    assert(adjacent_find(reps.begin(), reps.end(), not_equal_to<>{}) == reps.end());
}

template <class CharT>
void check_fill_and_align() {
    thread::id id;

    { // Align left
        basic_ostringstream<CharT> ss;
        ss.setf(ios_base::left, ios_base::adjustfield);
        ss.fill('*');
        ss.width(5);
        ss << id;
        assert(ss.str() == STR("0****"));
    }

    { // Align right
        basic_ostringstream<CharT> ss;
        ss.setf(ios_base::right, ios_base::adjustfield);
        ss.fill(':');
        ss.width(10);
        ss << id;
        assert(ss.str() == STR(":::::::::0"));
    }

    { // Align internal
        basic_ostringstream<CharT> ss;
        ss.setf(ios_base::internal, ios_base::adjustfield);
        ss.fill('_');
        ss.width(3);
        ss << id;
        assert(ss.str() == STR("__0"));
    }
}

template <class CharT>
void test() {
    check_fmtflags_and_locale<CharT>(thread::id{});
    check_fmtflags_and_locale<CharT>(this_thread::get_id());
    check_fill_and_align<CharT>();
}

int main() {
    test<char>();
    test<wchar_t>();
}
