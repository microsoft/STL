// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <regex> header, part 3
#define TEST_NAME "<regex>, part 3"

#include "tdefs.h"
#include <regex>
#include <stddef.h>
#include <string.h>

#define ADD_FACET(loc, pfac) STD locale(loc, pfac)

#ifdef WIDE
#define T(x)                  L##x
#define xlen(x)               wcslen(x)
#define CHECKSTR(left, right) CHECK_WSTR(left, right)
typedef wchar_t CHR;
typedef STD wstring STDString;

#else // defined WIDE
#define T(x)                  x
#define xlen(x)               CSTD strlen(x)
#define CHECKSTR(left, right) CHECK_STR(left, right)
typedef char CHR;
typedef STD string STDString;
#endif // defined WIDE

typedef STD regex_traits<CHR> MyTr;
typedef STD sub_match<const CHR*> MySm;
typedef STD match_results<const CHR*> MyMr;
typedef STD basic_regex<CHR> MyRgx;

static STD locale loc;

// static data
STD regex_constants::syntax_option_type synflags[] = {
    // array of flag values
    STD regex_constants::ECMAScript,
    STD regex_constants::basic,
    STD regex_constants::extended,
    STD regex_constants::awk,
    STD regex_constants::grep,
    STD regex_constants::egrep,
    STD regex_constants::icase,
    STD regex_constants::nosubs,
    STD regex_constants::optimize,
    STD regex_constants::collate,
};

STD regex_constants::match_flag_type mtchflags[] = {
    // array of flag values
    STD regex_constants::match_default,
    STD regex_constants::match_not_bol,
    STD regex_constants::match_not_eol,
    STD regex_constants::match_not_bow,
    STD regex_constants::match_not_eow,
    STD regex_constants::match_any,
    STD regex_constants::match_not_null,
    STD regex_constants::match_continuous,
    STD regex_constants::match_prev_avail,
    STD regex_constants::format_default,
    STD regex_constants::format_sed,
    STD regex_constants::format_no_copy,
    STD regex_constants::format_first_only,
};

STD regex_constants::error_type errflags[] = {
    // array of flag values
    STD regex_constants::error_collate,
    STD regex_constants::error_ctype,
    STD regex_constants::error_escape,
    STD regex_constants::error_backref,
    STD regex_constants::error_brack,
    STD regex_constants::error_paren,
    STD regex_constants::error_brace,
    STD regex_constants::error_badbrace,
    STD regex_constants::error_range,
    STD regex_constants::error_space,
    STD regex_constants::error_badrepeat,
    STD regex_constants::error_complexity,
    STD regex_constants::error_stack,
};

const CHR* class_names[] = {
    // array of character class names
    T("d"),
    T("w"),
    T("s"),
    T("alnum"),
    T("alpha"),
    T("blank"),
    T("cntrl"),
    T("digit"),
    T("graph"),
    T("lower"),
    T("print"),
    T("punct"),
    T("space"),
    T("upper"),
    T("xdigit"),
};

struct Myxctype : public STD ctype<CHR> { // degenerate locale facet
protected:
    CHR do_tolower(CHR) const { // convert to lowercase
        return T('a');
    }

    const CHR* do_tolower(CHR*, const CHR*) const { // convert to lowercase
        return T("s");
    }
};

struct Myxcollate : public STD collate<CHR> { // degenerate locale facet
public:
    STDString do_transform(const CHR*, const CHR*) const { // generate comparison key
        return T("ABC");
    }
};

struct MyTraits : STD regex_traits<CHR> { // degenerate traits class
    CHR translate(CHR ch) const { // generate unnatural collation order
        return ch == T('a') ? T('c') : ch == T('c') ? T('a') : ch == T('d') ? T('e') : ch == T('f') ? T('e') : ch;
    }
};

typedef STD basic_regex<CHR, MyTraits> MyxRgx;

struct fwdit { // forward iterator that wraps char *
    using iterator_category = STD forward_iterator_tag;
    using value_type        = CHR;
    using difference_type   = ptrdiff_t;
    using pointer           = CHR*;
    using reference         = CHR&;

    fwdit() : ptr(nullptr) { // construct
    }

    fwdit(CHR* p) : ptr(p) { // construct from char *
    }

    bool operator==(const fwdit& other) const { // compare for equality
        return ptr == other.ptr;
    }

    bool operator!=(const fwdit& other) const { // compare for inequality
        return !(*this == other);
    }

    CHR& operator*() const { // dereference
        return *ptr;
    }

    fwdit& operator++() { // preincrement
        ++ptr;
        return *this;
    }

    fwdit operator++(int) { // postincrement
        fwdit res = *this;
        ++ptr;
        return res;
    }

private:
    CHR* ptr;
};

struct bidit { // bidirectional iterator that wraps const char *
    using iterator_category = STD bidirectional_iterator_tag;
    using value_type        = CHR;
    using difference_type   = ptrdiff_t;
    using pointer           = CHR*;
    using reference         = CHR&;

    bidit() : ptr(nullptr) { // construct
    }

    bidit(const CHR* p) : ptr(p) { // construct from pointer
    }

    bool operator==(const bidit& other) const { // compare for equality
        return ptr == other.ptr;
    }

    bool operator!=(const bidit& other) const { // compare for inequality
        return !(*this == other);
    }

    const CHR& operator*() const { // dereference
        return *ptr;
    }

    bidit& operator++() { // preincrement
        ++ptr;
        return *this;
    }

    bidit operator++(int) { // postincrement
        bidit res = *this;
        ++ptr;
        return res;
    }

    bidit& operator--() { // predecrement
        --ptr;
        return *this;
    }

    bidit operator--(int) { // postdecrement
        bidit res = *this;
        --ptr;
        return res;
    }

private:
    const CHR* ptr;
};

typedef STD regex_iterator<bidit> MyIter;
typedef STD regex_token_iterator<bidit> MyTokIter;

static void test_uncoveredfunctions() {
    // basic_regex& assign(InIt first, InIt last,
    // flag_type flags = ECMAScript)
    STDString str(T("a(b)cd"));
    MyRgx r0;
    r0.assign(str.begin(), str.end() - 1);
    CHECK_INT(r0.flags(), MyRgx::ECMAScript);
    CHECK_INT(r0.mark_count(), 1);

    r0.assign(str.begin(), str.begin() + 1);
    CHECK_INT(r0.flags(), MyRgx::ECMAScript);
    CHECK_INT(r0.mark_count(), 0);

    // size_type max_size() const
    // match_results& operator=(const match_results& right)
    MyRgx rgx(T("c(a*)|(b)"));
    MyMr mr;
    MyMr mr1;
    CHR carr[] = T("xcaaay");
    CHECK(STD regex_search(carr, mr, rgx));
    mr1 = mr;

    MyMr::size_type maxsiz = mr1.max_size(); // how to check max_size?
    maxsiz                 = maxsiz; // to quiet "unused" warnings
    CHECK(!mr1.empty());
    CHECKSTR(mr1.str().c_str(), T("caaa"));

    // const basic_string<Elem> *operator->()
    const CHR* pat = T("aaxaayaaz");
    MyTokIter::regex_type rx(T("(a)a"));
    MyTokIter next(pat, pat + xlen(pat), rx);
    MyTokIter end;

    for (; next != end; ++next) {
        CHECKSTR(next->str().c_str(), T("aa"));
    }

    // difference_type length() const
    MySm sub = mr[1];
    CHECK_INT(sub.matched, 1);
    CHECK_INT(sub.length(), 3);

    // int compare(const sub_match& right) const
    // int compare(const basic_string<value_type>& right) const
    // int compare(const value_type *right) const
    const MySm::value_type* ptr = T("aab");
    CHECK(sub.compare(ptr) < 0);
    CHECK(0 < sub.compare(STDString(T("AAA"))));
    CHECK_INT(sub.compare(sub), 0);

    // basic_string<value_type> str() const
    CHECKSTR(sub.str().c_str(), T("aaa"));
    // operator basic_string<value_type>() const
    STDString str1 = sub;
    CHECKSTR(str1.c_str(), sub.str().c_str());

    // void swap(match_results<BidIt, Alloc>& left,
    // match_results<BidIt, Alloc>& right) throw()
    MyMr mr0;
    STD swap(mr0, mr);
    CHECK(mr.empty());
    CHECK(!mr0.empty());
    CHECKSTR(mr0.str().c_str(), T("caaa"));
}

static void test_error(const CHR* expr, STD regex_constants::error_type err) {
#if NO_EXCEPTIONS
#else // NO_EXCEPTIONS
    try {
        MyRgx rgx(expr);
    } catch (const STD regex_error& ex) {
        CHECK_INT(ex.code(), err);
        return;
    }

    CHECK_MSG("error should have been caught", false);
#endif // NO_EXCEPTIONS
}

static void test_uncoveredflags() {
    // error_collate -- the expression contained an invalid collating
    // element name
    // error_complexity -- an attempted match failed because it was
    // too complex
    test_error(T("[[....]-1.]"), STD regex_constants::error_collate);
    test_error(T("[[====]1=]"), STD regex_constants::error_collate);
    test_error(T("[[....]1.]"), STD regex_constants::error_collate);
    // test_error(T("((((((((((((((((((((((((((((((((a")
    //     T("))))))))))))))))))))))))))))))))"),
    //     STD regex_constants::error_complexity);
}

static void test_uncoveredgrammar() {
    // test "."
    MyRgx rx1;
    MyRgx rx2;
    STDString str1(T("Cats are cute\n Dogs are friendly"));
    STDString str2(T("Prime number"));
    STDString str3(T("aaaqxzbbb"));
    STDString str4(T("aaa\nxzbbb"));
    static const STD regex_constants::syntax_option_type flag[6] = {
        STD regex_constants::ECMAScript,
        STD regex_constants::basic,
        STD regex_constants::extended,
        STD regex_constants::awk,
        STD regex_constants::grep,
        STD regex_constants::egrep,
    };
    for (int i = 0; i <= 5; i++) {
        rx1.assign(T(".*"), flag[i]);
        rx2.assign(T("aaa...bbb"), flag[i]);
        CHECK(!STD regex_match(str1, rx1));
        CHECK(STD regex_match(str2, rx1));
        CHECK(STD regex_match(str3, rx2));
        CHECK(!STD regex_match(str4, rx2));
    }
}

static void test_capturegroups() {
    // There are limitations to the number of supported capture groups
    // (31 in case of ECMAScript)
    // And in BRE and grep grammars, only the first 9 capture groups
    // will be considered.  ?? how to verify
    MyMr mr0;
    MyMr mr1;
    MyMr mr2;
    const CHR* chr  = T("abcdefghijklmnopqrstuvwxyz123456");
    const CHR* chr1 = T("abcdefghijklmnoi");

    MyRgx rx0(T("(a)(b)(c)(d)(e)(f)(g)(h)(i)(j)(k)(l)(m)(n)(o)") T("(p)(q)(r)(s)(t)(u)(v)(w)(x)(y)(z)(1)(2)(3)(4)(5)"),
        MyRgx::ECMAScript);
    CHECK(STD regex_search(chr, mr0, rx0));
    CHECKSTR(mr0[31].str().c_str(), T("5"));

    MyRgx rx1(T("\\(a\\)\\(b\\)\\(c\\)\\(d\\)\\(e\\)\\(f\\)\\(g\\)")
                  T("\\(h\\)\\(i\\)\\(j\\)\\(k\\)\\(l\\)\\(m\\)\\(n\\)\\(o\\)\\9"),
        MyRgx::basic);
    STD regex_search(chr, mr1, rx1);
    CHECKSTR(mr1[15].str().c_str(), T(""));
    CHECK(STD regex_search(chr1, mr1, rx1));
    CHECKSTR(mr1[15].str().c_str(), T("o"));
    CHECKSTR(mr1[9].str().c_str(), T("i"));

    MyRgx rx2(T("\\(a\\)\\(b\\)\\(c\\)\\(d\\)\\(e\\)\\(f\\)\\(g\\)")
                  T("\\(h\\)\\(i\\)\\(j\\)\\(k\\)\\(l\\)\\(m\\)\\(n\\)\\(o\\)\\9"),
        MyRgx::grep);
    STD regex_search(chr, mr2, rx2);
    CHECKSTR(mr2[15].str().c_str(), T(""));
    CHECK(STD regex_search(chr1, mr2, rx2));
    CHECKSTR(mr2[15].str().c_str(), T("o"));
    CHECKSTR(mr2[9].str().c_str(), T("i"));

#if NO_EXCEPTIONS
#else // NO_EXCEPTIONS
    try {
        MyRgx rgx(T("\\(a\\)\\(b\\)\\(c\\)\\(d\\)\\(e\\)\\(f\\)\\(g\\)")
                      T("\\(h\\)\\(i\\)\\(j\\)\\(k\\)\\(l\\)\\(m\\)\\(n\\)\\(o\\)\\10"),
            MyRgx::grep);
    } catch (const STD regex_error& ex) {
        CHECK_INT(ex.code(), STD regex_constants::error_backref);
        CHECK_MSG("error has been caught", true);
    }
#endif // NO_EXCEPTIONS
}

static void test_iterators() { // needs refining after ++ fixed
    const CHR* pat = T("ax");
    MyIter::regex_type rx(T("a"));
    MyIter iter(pat, pat + xlen(pat) + 10, rx);
    CHECKSTR(iter->str().c_str(), T("a"));
    STD match_results<bidit> mr = *iter;
    CHECK_INT(mr.position(0), 0);
    CHECK_INT(mr.length(0), 1);
    MyTokIter toIter(pat, pat + xlen(pat) + 10, rx);
    CHECKSTR(toIter->str().c_str(), T("a"));
    toIter++;
#if NO_EXCEPTIONS
#else // NO_EXCEPTIONS
    try { // we can't handle all exceptions now.
        // STD sub_match<bidit> sm(*toIter);
        // STD sub_match<bidit> sm(*end);
        // toIter->str();
        // end->str();
        // MyTokIter temp = ++end;
    } catch (...) {
    }
#endif // NO_EXCEPTIONS
}

void test_main() { // test header <regex>
    test_uncoveredfunctions();
    test_uncoveredflags();
    test_uncoveredgrammar();
    test_capturegroups();
    test_iterators();
}
