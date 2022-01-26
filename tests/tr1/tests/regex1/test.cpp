// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <regex> header, part 1
#define TEST_NAME "<regex>, part 1"

#include "tdefs.h"
#include <regex>
#include <stddef.h>
#include <string.h>

#define ADD_FACET(loc, pfac) STD locale(loc, pfac)

#ifdef WIDE
#define T(x)                     L##x
#define xlen(x)                  wcslen(x)
#define CHECKSTR(left, right)    CHECK_WSTR(left, right)
#define CHECKSTRING(left, right) CHECK_WSTRING(left, right)
typedef wchar_t CHR;
typedef STD wstring STDString;

#else // defined WIDE
#define T(x)                     x
#define xlen(x)                  CSTD strlen(x)
#define CHECKSTR(left, right)    CHECK_STR(left, right)
#define CHECKSTRING(left, right) CHECK_STRING(left, right)
typedef char CHR;
typedef STD string STDString;
#endif // defined WIDE

typedef STD regex_traits<CHR> MyTr;
typedef STD sub_match<const CHR*> MySm;
typedef STD match_results<const CHR*> MyMr;
typedef STD basic_regex<CHR> MyRgx;

static STD locale loc;
static STD locale cloc;

// static data
static const STD regex_constants::syntax_option_type synflags[] = {
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

static const STD regex_constants::match_flag_type mtchflags[] = {
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

static const STD regex_constants::error_type errflags[] = {
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

static const CHR* const class_names[] = {
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

template <class T>
struct Myalloc {
    typedef T value_type;

    Myalloc() {}

    template <class U>
    Myalloc(const Myalloc<U>&) {}

    T* allocate(size_t n) {
        return STD allocator<T>{}.allocate(n);
    }

    void deallocate(T* p, size_t n) {
        STD allocator<T>{}.deallocate(p, n);
    }

    template <class U>
    bool operator==(const Myalloc<U>&) const {
        return true;
    }

    template <class U>
    bool operator!=(const Myalloc<U>&) const {
        return false;
    }
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

static void test_traits() { // test template regex_traits
    MyTr::char_type* p_char   = (CHR*) nullptr;
    MyTr::size_type* p_size   = (CSTD size_t*) nullptr;
    MyTr::string_type* p_str  = (STD basic_string<CHR>*) nullptr;
    MyTr::locale_type* p_loc  = (STD locale*) nullptr;
    MyTr::char_class_type chr = (MyTr::char_class_type) 0;

    p_char = p_char; // to quiet diagnostics
    p_size = p_size;
    p_str  = p_str;
    p_loc  = p_loc;
    chr    = chr;

    MyTr v0;
    CHR carr[]  = T("abc");
    CHR carr0[] = T("ABC");
    CHR carr1[] = T("def");
    CHECK_INT(MyTr::length(carr), xlen(carr));
    CHECK_INT(v0.translate('a'), v0.translate('a'));
    CHECK_INT(v0.translate_nocase('a'), v0.translate_nocase('A'));
    CHECKSTRING(v0.transform(carr, carr + xlen(carr)), v0.transform(carr, carr + xlen(carr)));
    CHECK(v0.transform(carr, carr + xlen(carr)) != v0.transform(carr0, carr0 + xlen(carr0)));
    CHECK(v0.transform(carr, carr + xlen(carr)) < v0.transform(carr1, carr1 + xlen(carr1)));
    CHECK(v0.transform_primary(carr, carr + xlen(carr)) == v0.transform_primary(carr, carr + xlen(carr)));
    CHECK(v0.transform_primary(carr, carr + xlen(carr)) == v0.transform_primary(carr0, carr0 + xlen(carr0)));
    CHECK(v0.transform_primary(carr0, carr0 + xlen(carr0)) < v0.transform_primary(carr1, carr1 + xlen(carr1)));

    for (size_t i = 0; i < sizeof(class_names) / sizeof(*class_names); ++i)
        CHECK(v0.lookup_classname(class_names[i], class_names[i] + xlen(class_names[i])) != 0);
    CHECK(v0.lookup_classname(carr, carr) == 0);
    CHECK(v0.lookup_collatename(carr, carr + xlen(carr)) != STDString());
    CHECK(v0.isctype('0', v0.lookup_classname(class_names[0], class_names[0] + xlen(class_names[0]))));
    CHECK_INT(v0.value('7', 8), 7);
    CHECK_INT(v0.value('9', 8), -1);
    CHECK_INT(v0.value('9', 10), 9);
    CHECK_INT(v0.value('a', 10), -1);
    CHECK_INT(v0.value('a', 16), 10);
    MyTr v1;
    CHECK(v1.getloc() == loc);
    CHECK(v1.imbue(cloc) == loc);
    CHECK(v1.getloc() == cloc);

    STD locale loc_myctype = ADD_FACET(loc, new Myxctype);
    v1.imbue(loc_myctype);
    CHECK_INT(v1.translate_nocase('b'), 'a');

    STD locale loc_mycollate = ADD_FACET(loc, new Myxcollate);
    v1.imbue(loc_mycollate);
    CHECKSTR(v1.transform(carr, carr + xlen(carr)).c_str(), T("ABC"));
}

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

static void test_regex() { // test template basic_regex
    MyxRgx::value_type* p_char = (CHR*) nullptr;
    MyxRgx::flag_type* p_flag  = (MyxRgx::flag_type*) nullptr;
    MyxRgx::locale_type* p_loc = (MyTraits::locale_type*) nullptr;
    MyxRgx::string_type* p_str = (MyTraits::string_type*) nullptr;
    MyxRgx::traits_type* p_tr  = (MyTraits*) nullptr;

    p_char = p_char; // to quiet diagnostics
    p_flag = p_flag;
    p_loc  = p_loc;
    p_str  = p_str;
    p_tr   = p_tr;

    CHECK_INT(MyxRgx::icase, STD regex_constants::icase);
    CHECK_INT(MyxRgx::nosubs, STD regex_constants::nosubs);
    CHECK_INT(MyxRgx::optimize, STD regex_constants::optimize);
    CHECK_INT(MyxRgx::collate, STD regex_constants::collate);
    CHECK_INT(MyxRgx::ECMAScript, STD regex_constants::ECMAScript);
    CHECK_INT(MyxRgx::basic, STD regex_constants::basic);
    CHECK_INT(MyxRgx::extended, STD regex_constants::extended);
    CHECK_INT(MyxRgx::awk, STD regex_constants::awk);
    CHECK_INT(MyxRgx::grep, STD regex_constants::grep);
    CHECK_INT(MyxRgx::egrep, STD regex_constants::egrep);

    MyRgx r0;
    CHECK_INT(r0.flags(), 0);
    CHECK_INT(r0.mark_count(), 0);

    MyRgx r1(T("a"), MyRgx::icase);
    CHECK_INT(r1.flags(), MyRgx::icase);
    CHECK_INT(r1.mark_count(), 0);

#if NO_EXCEPTIONS
#else // NO_EXCEPTIONS
    bool ok = false;
    try { // test exception for invalid expression
        MyRgx r1x(T("a{"));
    } catch (const STD regex_error&) { // handle invalid expression
        ok = true;
    }
    CHECK_MSG("basic_regex(const char *) throws regex_error on error", ok);
#endif // NO_EXCEPTIONS

    MyRgx r2(T("(a)bcd"), 3, MyRgx::icase);
    CHECK_INT(r2.flags(), MyRgx::icase);
    CHECK_INT(r2.mark_count(), 1);

#if NO_EXCEPTIONS
#else // NO_EXCEPTIONS
    ok = false;
    try { // test exception for invalid expression
        MyRgx r2x(T("a{1}"), 2);
    } catch (const STD regex_error&) { // handle invalid expression
        ok = true;
    }
    CHECK_MSG("basic_regex(const char *, size_t) throws regex_error on error", ok);
#endif // NO_EXCEPTIONS

    MyRgx r3(r2);
    CHECK_INT(r3.flags(), MyRgx::icase);
    CHECK_INT(r3.mark_count(), 1);

    STDString arg(T("((d(e))f)"));
    MyRgx r4(arg, (MyRgx::flag_type)(MyRgx::icase | MyRgx::extended));
    CHECK_INT(r4.flags(), MyRgx::icase | MyRgx::extended);
    CHECK_INT(r4.mark_count(), 3);

#if NO_EXCEPTIONS
#else // NO_EXCEPTIONS
    ok = false;
    try { // test exception for invalid expression
        STDString arg2(T("((d(e))f"));
        MyRgx r4x(arg2);
    } catch (const STD regex_error&) { // handle invalid expression
        ok = true;
    }
    CHECK_MSG("basic_regex(string) throws regex_error on error", ok);
#endif // NO_EXCEPTIONS

    CHR carr[] = T("a{1}");
    fwdit begin(carr);
    fwdit end(carr + xlen(carr));
    MyRgx r5(begin, end);
    CHECK_INT(r5.flags(), MyRgx::ECMAScript);
    CHECK_INT(r5.mark_count(), 0);

#if NO_EXCEPTIONS
#else // NO_EXCEPTIONS
    ok = false;
    try { // test exception for invalid expression
        fwdit endx(carr + xlen(carr) - 1);
        MyRgx r5x(begin, endx);
    } catch (const STD regex_error&) { // handle invalid expression
        ok = true;
    }
    CHECK_MSG("basic_regex(iter, iter) throws regex_error on error", ok);
#endif // NO_EXCEPTIONS

    r4 = r1;
    CHECK_INT(r4.flags(), MyRgx::icase);
    CHECK_INT(r4.mark_count(), 0);

    r4 = T("a(b)");
    CHECK_INT(r4.flags(), MyRgx::ECMAScript);
    CHECK_INT(r4.mark_count(), 1);

    r4 = STDString(T("(a)(b)"));
    CHECK_INT(r4.flags(), MyRgx::ECMAScript);
    CHECK_INT(r4.mark_count(), 2);

    r5.assign(r4);
    CHECK_INT(r5.flags(), MyRgx::ECMAScript);
    CHECK_INT(r5.mark_count(), 2);

    r5.assign(T("a(b)"));
    CHECK_INT(r5.flags(), MyRgx::ECMAScript);
    CHECK_INT(r5.mark_count(), 1);

    r5.assign(T("a(b)"), 1, MyRgx::extended);
    CHECK_INT(r5.flags(), MyRgx::extended);
    CHECK_INT(r5.mark_count(), 0);

    r5.assign(STDString(T("(b)")), MyRgx::awk);
    CHECK_INT(r5.flags(), MyRgx::awk);
    CHECK_INT(r5.mark_count(), 1);

    CHECK(r5.getloc() == loc);
    CHECK(r5.imbue(cloc) == loc);
    CHECK(r5.getloc() == cloc);

    r1.swap(r2);
    CHECK_INT(r1.flags(), MyRgx::icase);
    CHECK_INT(r1.mark_count(), 1);
    CHECK_INT(r2.flags(), MyRgx::icase);
    CHECK_INT(r2.mark_count(), 0);

    STD swap(r1, r2);
    CHECK_INT(r1.flags(), MyRgx::icase);
    CHECK_INT(r1.mark_count(), 0);
    CHECK_INT(r2.flags(), MyRgx::icase);
    CHECK_INT(r2.mark_count(), 1);

    {
        STD initializer_list<CHR> init{'(', '(', 'd', '(', 'e', ')', ')', 'f', ')'};
        MyRgx r11(init, (MyRgx::flag_type)(MyRgx::icase | MyRgx::extended));
        CHECK_INT(r11.flags(), MyRgx::icase | MyRgx::extended);
        CHECK_INT(r11.mark_count(), 3);

        STD initializer_list<CHR> init2{'(', 'b', ')'};
        r11.assign(init2, MyRgx::awk);
        CHECK_INT(r11.flags(), MyRgx::awk);
        CHECK_INT(r11.mark_count(), 1);

        r11 = init;
        CHECK_INT(r11.flags(), MyRgx::ECMAScript);
        CHECK_INT(r11.mark_count(), 3);
    }

    {
        MyRgx r0x;
        CHECK_INT(r0x.flags(), 0);
        CHECK_INT(r0x.mark_count(), 0);

        MyRgx r1x(T("a"), MyRgx::icase);
        CHECK_INT(r1x.flags(), MyRgx::icase);
        CHECK_INT(r1x.mark_count(), 0);

        MyRgx r2x(T("a"), MyRgx::icase);
        r2x = STD move(r0x);
        CHECK_INT(r2x.flags(), 0);
        CHECK_INT(r2x.mark_count(), 0);

        MyRgx r3x(STD move(r1x));
        CHECK_INT(r3x.flags(), MyRgx::icase);
        CHECK_INT(r3x.mark_count(), 0);

        r0x.assign(STD move(r3x));
        CHECK_INT(r0x.flags(), MyRgx::icase);
        CHECK_INT(r0x.mark_count(), 0);
    }
}

static void test_match_results() { // test template match_results
    MySm::value_type* p_sval       = (CHR*) nullptr;
    MySm::difference_type* p_sdiff = (CSTD ptrdiff_t*) nullptr;
    MySm::iterator s_iter          = (CHR*) nullptr;
    MySm::string_type* p_sstr      = (STD basic_string<CHR>*) nullptr;

    p_sval  = p_sval; // to quiet diagnostics
    p_sdiff = p_sdiff;
    s_iter  = s_iter;
    p_sstr  = p_sstr;

    MySm sm;
    MyMr::value_type* p_val    = (MySm*) nullptr;
    MyMr::const_reference cref = sm;
    MyMr::reference ref        = sm;
    MyMr::const_iterator citer;
    MyMr::iterator* p_iter        = (MyMr::const_iterator*) nullptr;
    MyMr::difference_type* p_diff = (MySm::difference_type*) nullptr;
    MyMr::size_type* p_size       = (CSTD size_t*) nullptr;
    MyMr::allocator_type* p_alloc = (STD allocator<MySm>*) nullptr;
    MyMr::char_type* p_char       = (MySm::value_type*) nullptr;
    MyMr::string_type* p_str      = (STD basic_string<CHR>*) nullptr;

    p_val = p_val; // to quiet diagnostics
    (void) citer;
    if (cref.length() == ref.length())
        p_iter = p_iter;
    p_diff  = p_diff;
    p_size  = p_size;
    p_alloc = p_alloc;
    p_char  = p_char;
    p_str   = p_str;

    MyMr mr;
    CHECK_INT(mr.ready(), false);
    CHECK_INT(mr.size(), 0);
    CHECK(mr.empty());
    CHECKSTRING(mr.str(), STDString());
    CHECK_INT(mr.ready(), false);
    CHECK(!mr[0].matched);
    CHECK_PTR(mr[0].first, mr[0].second);

    STD allocator<MySm> alloc;
    MyMr mr1(alloc);
    CHECK_INT(mr1.size(), 0);
    CHECK(mr1.empty());
    CHECKSTRING(mr1.str(), STDString());
    CHECK(mr1.get_allocator() == alloc);

    MyRgx rgx(T("c(a*)|(b)"));
    CHR carr[] = T("xcaaay");
    CHECK(STD regex_search(carr, mr, rgx));

    MyMr mr2(mr);
    CHECK(!mr2.empty());
    CHECK_INT(mr2.size(), 3);

    CHECK(mr[0].matched);
    CHECK_INT(mr.ready(), true);
    CHECK_PTR(mr[0].first, carr + 1);
    CHECK_PTR(mr[0].second, carr + 5);
    CHECK_INT(mr.length(), 4);
    CHECK_INT(mr.length(0), 4);
    CHECK_INT(mr.position(), 1);
    CHECK_INT(mr.position(0), 1);
    CHECKSTR(mr.str().c_str(), T("caaa"));
    CHECKSTR(mr.str(0).c_str(), T("caaa"));
    CHECK(mr[1].matched);
    CHECK_PTR(mr[1].first, carr + 2);
    CHECK_PTR(mr[1].second, carr + 5);
    CHECK_INT(mr.length(1), 3);
    CHECK_INT(mr.position(1), 2);
    CHECK(mr.str(1) == T("aaa"));
    CHECK(!mr[2].matched);
    CHECK_PTR(mr[2].first, carr + 6);
    CHECK_PTR(mr[2].second, carr + 6);
    CHECK_INT(mr.length(2), 0);
    CHECK_INT(mr.position(2), 6);
    CHECKSTR(mr.str(2).c_str(), T(""));
    const MySm& pfx = mr.prefix();
    CHECK(pfx.matched);
    CHECK_PTR(pfx.first, carr);
    CHECK_PTR(pfx.second, carr + 1);
    const MySm& sfx = mr.suffix();
    CHECK(sfx.matched);
    CHECK_PTR(sfx.first, carr + 5);
    CHECK_PTR(sfx.second, carr + 6);

    MyMr::iterator iter = mr.begin();
    CHECK(iter != mr.end());
    CHECK(*iter++ == mr[0]);
    CHECK(*iter++ == mr[1]);
    CHECK(*iter++ == mr[2]);
    CHECK(iter == mr.end());

    iter = mr.cbegin();
    CHECK(iter != mr.cend());
    mr.swap(mr1);
    CHECK_INT(mr.size(), 0);
    CHECK(mr.empty());
    CHECKSTRING(mr.str(), STDString());
    CHECK_INT(mr1.size(), 3);
    CHECK(!mr1.empty());
    CHECKSTR(mr1.str().c_str(), T("caaa"));

    MyMr mr3(STD move(mr1));
    CHECK(!mr3.empty());
    CHECKSTR(mr3.str().c_str(), T("caaa"));

    MyMr mr4;
    mr4 = STD move(mr3);
    CHECK(!mr4.empty());
    CHECKSTR(mr4.str().c_str(), T("caaa"));
}

static void test_format() { // test format functions in template match_results
    CHR chr[100];
    MyRgx r1(T("(ab)(bc)(cd)(de)(ef)(fg)(gh)(hi)(ij)(jk)"));
    MyMr mr;
    CHECK(STD regex_search(T("xabbccddeeffgghhiijjky"), mr, r1));

    fwdit out(chr);
    typedef STD basic_string<char, STD char_traits<char>, Myalloc<char>> MyString;
    MyString fmt                     = T("|$&|$0|$$|$`|$'|$10$9$8$7$6$5$4$3$2$1|$11|");
    const CHR* rslt                  = T("|abbccddeeffgghhiijjk|$0|") T("$|x|y|jkijhighfgefdecdbcab||");
    const MyMr::char_type* fmt_first = fmt.c_str();
    fwdit res                        = mr.format(out, fmt_first, fmt_first + fmt.size());
    *res                             = '\0';

    CHECKSTR(chr, rslt);
    MyString str = mr.format(fmt);
    CHECKSTR(str.c_str(), rslt);

    out  = chr;
    fmt  = T("|&|\\0|\\&|\\9\\8\\7\\6\\5\\4\\3\\2\\1|\\10\\\\1|");
    rslt = T("|abbccddeeffgghhiijjk|abbccddeeffgghhiijjk|") T("&|ijhighfgefdecdbcab|ab0\\1|");
    res  = mr.format(out, fmt, STD regex_constants::format_sed);
    *res = '\0';
    CHECKSTR(chr, rslt);
    str = mr.format(fmt, STD regex_constants::format_sed);
    CHECKSTR(str.c_str(), rslt);
}

typedef STD regex_iterator<bidit> MyIter;
typedef STD regex_token_iterator<bidit> MyTokIter;

static void test_regex_iterator() { // test template regex_iterator
    const CHR* rgx = T("a");
    bidit rgxbegin(rgx);
    bidit rgxend(rgx + xlen(rgx));
    const CHR* chr = T("ababab");
    bidit begin(chr);
    bidit end(chr + xlen(chr));

    MyIter::regex_type re(rgxbegin, rgxend);
    MyIter iter(begin, end, re);
    CHECK(iter != MyIter());
    CHECK_INT(iter->position(0), 0);
    CHECK_INT(iter->length(0), 1);
    MyIter iter1(iter);
    STD match_results<bidit> mr(*++iter1);
    CHECK_INT(mr.position(0), 2);
    CHECK_INT(mr.length(0), 1);
    iter = iter1;
    iter++;
    mr = *iter++;
    CHECK_INT(mr.position(0), 4);
    CHECK_INT(mr.length(0), 1);
    CHECK(iter == MyIter());

    MyIter::regex_type* p_rgx       = (STD basic_regex<CHR>*) nullptr;
    MyIter::value_type* p_val       = (STD match_results<bidit>*) nullptr;
    MyIter::difference_type* p_dif  = (CSTD ptrdiff_t*) nullptr;
    MyIter::pointer ptr             = (STD match_results<bidit>*) nullptr;
    MyIter::reference ref           = mr;
    STD forward_iterator_tag* p_tag = (MyIter::iterator_category*) nullptr;

    p_rgx = p_rgx; // to quiet diagnostics
    p_val = p_val;
    p_dif = p_dif;
    ptr   = ptr;
    if (ref.size())
        p_tag = p_tag;
}

static void test_regex_token_iterator() { // test template regex_token_iterator
    const CHR* rgx = T("a(b)");
    bidit rgxbegin(rgx);
    bidit rgxend(rgx + xlen(rgx));
    const CHR* chr = T("abcabcabc");
    bidit begin(chr);
    bidit end(chr + xlen(chr));
    MyTokIter::regex_type re(rgxbegin, rgxend);
    MyTokIter end_it;

    MyTokIter iter(begin, end, re);
    CHECK(iter != end_it);
    STD sub_match<bidit> sm(*iter);
    CHECK_PTR(&*sm.first, chr);
    CHECK_PTR(&*sm.second, chr + 2);
    MyTokIter iter1(iter);
    sm = *++iter1;
    CHECK_PTR(&*sm.first, chr + 3);
    CHECK_PTR(&*sm.second, chr + 5);
    iter = iter1;
    iter++;
    sm = *iter;
    CHECK_PTR(&*sm.first, chr + 6);
    CHECK_PTR(&*sm.second, chr + 8);
    ++iter;
    CHECK(iter == end_it);

    MyTokIter iter2(begin, end, re, -1);
    CHECK(iter2 != end_it);
    sm = *iter2;
    CHECK_PTR(&*sm.first, chr);
    CHECK_PTR(&*sm.second, chr);
    ++iter2;
    sm = *iter2;
    CHECK_PTR(&*sm.first, chr + 2);
    CHECK_PTR(&*sm.second, chr + 3);
    sm = *++iter2;
    CHECK_PTR(&*sm.first, chr + 5);
    CHECK_PTR(&*sm.second, chr + 6);
    sm = *++iter2;
    CHECK_PTR(&*sm.first, chr + 8);
    CHECK_PTR(&*sm.second, chr + 9);
    ++iter2;
    CHECK(iter2 == end_it);

    int init[] = {1, -1, 0};
    { // test vector initializer
        MyTokIter iter3(begin, end, re, STD vector<int>(&init[0], &init[3]));

        CHECK(iter3 != end_it);
        sm = *iter3;
        CHECK_PTR(&*sm.first, chr + 1);
        CHECK_PTR(&*sm.second, chr + 2);
        sm = *++iter3;
        CHECK_PTR(&*sm.first, chr);
        CHECK_PTR(&*sm.second, chr);
        sm = *++iter3;
        CHECK_PTR(&*sm.first, chr + 0);
        CHECK_PTR(&*sm.second, chr + 2);
        sm = *++iter3;
        CHECK_PTR(&*sm.first, chr + 4);
        CHECK_PTR(&*sm.second, chr + 5);
        sm = *++iter3;
        CHECK_PTR(&*sm.first, chr + 2);
        CHECK_PTR(&*sm.second, chr + 3);
        sm = *++iter3;
        CHECK_PTR(&*sm.first, chr + 3);
        CHECK_PTR(&*sm.second, chr + 5);
        sm = *++iter3;
        CHECK_PTR(&*sm.first, chr + 7);
        CHECK_PTR(&*sm.second, chr + 8);
        sm = *++iter3;
        CHECK_PTR(&*sm.first, chr + 5);
        CHECK_PTR(&*sm.second, chr + 6);
        sm = *++iter3;
        CHECK_PTR(&*sm.first, chr + 6);
        CHECK_PTR(&*sm.second, chr + 8);
        sm = *++iter3;
        CHECK_PTR(&*sm.first, chr + 8);
        CHECK_PTR(&*sm.second, chr + 9);
        CHECK(++iter3 == end_it);
    }

    { // test array initializer
        MyTokIter iter3(begin, end, re, init);

        CHECK(iter3 != end_it);
        sm = *iter3;
        CHECK_PTR(&*sm.first, chr + 1);
        CHECK_PTR(&*sm.second, chr + 2);
        sm = *++iter3;
        CHECK_PTR(&*sm.first, chr);
        CHECK_PTR(&*sm.second, chr);
        sm = *++iter3;
        CHECK_PTR(&*sm.first, chr + 0);
        CHECK_PTR(&*sm.second, chr + 2);
        sm = *++iter3;
        CHECK_PTR(&*sm.first, chr + 4);
        CHECK_PTR(&*sm.second, chr + 5);
        sm = *++iter3;
        CHECK_PTR(&*sm.first, chr + 2);
        CHECK_PTR(&*sm.second, chr + 3);
        sm = *++iter3;
        CHECK_PTR(&*sm.first, chr + 3);
        CHECK_PTR(&*sm.second, chr + 5);
        sm = *++iter3;
        CHECK_PTR(&*sm.first, chr + 7);
        CHECK_PTR(&*sm.second, chr + 8);
        sm = *++iter3;
        CHECK_PTR(&*sm.first, chr + 5);
        CHECK_PTR(&*sm.second, chr + 6);
        sm = *++iter3;
        CHECK_PTR(&*sm.first, chr + 6);
        CHECK_PTR(&*sm.second, chr + 8);
        sm = *++iter3;
        CHECK_PTR(&*sm.first, chr + 8);
        CHECK_PTR(&*sm.second, chr + 9);
        CHECK(++iter3 == end_it);
    }

    { // test initializer list
        STD initializer_list<int> ilist{init[0], init[1], init[2]};
        MyTokIter iter3(begin, end, re, ilist);

        CHECK(iter3 != end_it);
        sm = *iter3;
        CHECK_PTR(&*sm.first, chr + 1);
        CHECK_PTR(&*sm.second, chr + 2);
        sm = *++iter3;
        CHECK_PTR(&*sm.first, chr);
        CHECK_PTR(&*sm.second, chr);
        sm = *++iter3;
        CHECK_PTR(&*sm.first, chr + 0);
        CHECK_PTR(&*sm.second, chr + 2);
        sm = *++iter3;
        CHECK_PTR(&*sm.first, chr + 4);
        CHECK_PTR(&*sm.second, chr + 5);
        sm = *++iter3;
        CHECK_PTR(&*sm.first, chr + 2);
        CHECK_PTR(&*sm.second, chr + 3);
        sm = *++iter3;
        CHECK_PTR(&*sm.first, chr + 3);
        CHECK_PTR(&*sm.second, chr + 5);
        sm = *++iter3;
        CHECK_PTR(&*sm.first, chr + 7);
        CHECK_PTR(&*sm.second, chr + 8);
        sm = *++iter3;
        CHECK_PTR(&*sm.first, chr + 5);
        CHECK_PTR(&*sm.second, chr + 6);
        sm = *++iter3;
        CHECK_PTR(&*sm.first, chr + 6);
        CHECK_PTR(&*sm.second, chr + 8);
        sm = *++iter3;
        CHECK_PTR(&*sm.first, chr + 8);
        CHECK_PTR(&*sm.second, chr + 9);
        CHECK(++iter3 == end_it);
    }

    STD vector<int> v(init, init + 3);
    MyTokIter iter4(begin, end, re, v);
    CHECK(iter4 != end_it);
    sm = *iter4;
    CHECK_PTR(&*sm.first, chr + 1);
    CHECK_PTR(&*sm.second, chr + 2);
    sm = *++iter4;
    CHECK_PTR(&*sm.first, chr);
    CHECK_PTR(&*sm.second, chr);
    sm = *++iter4;
    CHECK_PTR(&*sm.first, chr + 0);
    CHECK_PTR(&*sm.second, chr + 2);
    sm = *++iter4;
    CHECK_PTR(&*sm.first, chr + 4);
    CHECK_PTR(&*sm.second, chr + 5);
    sm = *++iter4;
    CHECK_PTR(&*sm.first, chr + 2);
    CHECK_PTR(&*sm.second, chr + 3);
    sm = *++iter4;
    CHECK_PTR(&*sm.first, chr + 3);
    CHECK_PTR(&*sm.second, chr + 5);
    sm = *++iter4;
    CHECK_PTR(&*sm.first, chr + 7);
    CHECK_PTR(&*sm.second, chr + 8);
    sm = *++iter4;
    CHECK_PTR(&*sm.first, chr + 5);
    CHECK_PTR(&*sm.second, chr + 6);
    sm = *++iter4;
    CHECK_PTR(&*sm.first, chr + 6);
    CHECK_PTR(&*sm.second, chr + 8);
    sm = *++iter4;
    CHECK_PTR(&*sm.first, chr + 8);
    CHECK_PTR(&*sm.second, chr + 9);
    CHECK(++iter4 == end_it);

    const CHR* edges = T("zb");
    bidit edgesfirst(edges), edgeslast(edges + xlen(edges));
    MyTokIter::regex_type re1(T("z"));
    MyTokIter iter5(edgesfirst, edgeslast, re1, -1);
    sm = *iter5;
    CHECK_PTR(&*sm.first, edges);
    CHECK_PTR(&*sm.second, edges);
    CHECK(iter5 != end_it);
    MyTokIter iter6(++iter5);
    sm = *iter6;
    CHECK_PTR(&*sm.first, edges + 1);
    CHECK_PTR(&*sm.second, edges + 2);
    CHECK(iter6 != end_it);
    CHECK(++iter6 == end_it);

    MyTokIter::regex_type* p_rgx      = (STD basic_regex<CHR>*) nullptr;
    MyTokIter::value_type* p_val      = (STD sub_match<bidit>*) nullptr;
    MyTokIter::difference_type* p_dif = (CSTD ptrdiff_t*) nullptr;
    MyTokIter::pointer ptr            = (STD sub_match<bidit>*) nullptr;
    MyTokIter::reference ref          = sm;
    STD forward_iterator_tag* p_tag   = (MyTokIter::iterator_category*) nullptr;

    p_rgx = p_rgx; // to quiet diagnostics
    p_val = p_val;
    p_dif = p_dif;
    ptr   = ptr;
    if (ref.length())
        p_tag = p_tag;
}

static void test_match() { // test function regex_match
    MyRgx r0;
    CHECK(!STD regex_match(T("aa"), r0));

    MyRgx r(T("aa"));
    const CHR* chr = T("aa");
    bidit begin(chr);
    bidit end(chr + xlen(chr));
    STDString str(chr);
    const CHR* beg = &*str.begin();

    STD match_results<bidit> mr;
    CHECK(STD regex_match(begin, end, mr, r, STD regex_constants::match_default));
    CHECK_INT(mr.size(), 1);
    CHECK(!mr.empty());
    CHECK(!mr.prefix().matched);
    CHECK_PTR(&*mr.prefix().first, chr);
    CHECK_PTR(&*mr.prefix().second, chr);
    CHECK(!mr.suffix().matched);
    CHECK_PTR(&*mr.suffix().first, chr + 2);
    CHECK_PTR(&*mr.suffix().second, chr + 2);
    CHECK(mr[0].matched);
    CHECK_PTR(&*mr[0].first, chr);
    CHECK_PTR(&*mr[0].second, chr + 2);

    STD match_results<const CHR*> mr1;
    CHECK(STD regex_match(chr, mr1, r, STD regex_constants::match_default));
    CHECK_INT(mr1.size(), 1);
    CHECK(!mr1.empty());
    CHECK(!mr1.prefix().matched);
    CHECK_PTR(&*mr1.prefix().first, chr);
    CHECK_PTR(&*mr1.prefix().second, chr);
    CHECK(!mr1.suffix().matched);
    CHECK_PTR(&*mr1.suffix().first, chr + 2);
    CHECK_PTR(&*mr1.suffix().second, chr + 2);
    CHECK(mr1[0].matched);
    CHECK_PTR(&*mr1[0].first, chr);
    CHECK_PTR(&*mr1[0].second, chr + 2);

    STD match_results<STDString::const_iterator> mr2;
    CHECK(STD regex_match(str, mr2, r, STD regex_constants::match_default));
    CHECK_INT(mr2.size(), 1);
    CHECK(!mr2.empty());
    CHECK(!mr2.prefix().matched);
    CHECK_PTR(&*mr2.prefix().first, beg);
    CHECK_PTR(&*mr2.prefix().second, beg);
    CHECK(!mr2.suffix().matched);
    CHECK_INT(mr2.suffix().first - str.begin(), 2);
    CHECK_INT(mr2.suffix().second - str.begin(), 2);
    CHECK(mr2[0].matched);
    CHECK_PTR(&*mr2[0].first, beg);
    CHECK_INT(mr2.suffix().second - str.begin(), 2);

    CHECK(STD regex_match(begin, end, r, STD regex_constants::match_default));
    CHECK(STD regex_match(chr, r, STD regex_constants::match_default));
    CHECK(STD regex_match(str, r, STD regex_constants::match_default));
}

static void test_search() { // test functions regex_search
    MyRgx r0;
    CHECK(!STD regex_search(T("aa"), r0));

    MyRgx r(T("a"));
    const CHR* chr = T("aa");
    bidit begin(chr);
    bidit end(chr + xlen(chr));
    STDString str(chr);
    const CHR* beg = &*str.begin();

    STD match_results<bidit> mr;
    CHECK(STD regex_search(begin, end, mr, r, STD regex_constants::match_default));
    CHECK_INT(mr.size(), 1);
    CHECK(!mr.empty());
    CHECK(!mr.prefix().matched);
    CHECK_PTR(&*mr.prefix().first, chr);
    CHECK_PTR(&*mr.prefix().second, chr);
    CHECK(mr.suffix().matched);
    CHECK_PTR(&*mr.suffix().first, chr + 1);
    CHECK_PTR(&*mr.suffix().second, chr + 2);
    CHECK(mr[0].matched);
    CHECK_PTR(&*mr[0].first, chr);
    CHECK_PTR(&*mr[0].second, chr + 1);

    STD match_results<const CHR*> mr1;
    CHECK(STD regex_search(chr, mr1, r, STD regex_constants::match_default));
    CHECK_INT(mr1.size(), 1);
    CHECK(!mr1.empty());
    CHECK(!mr1.prefix().matched);
    CHECK_PTR(&*mr1.prefix().first, chr);
    CHECK_PTR(&*mr1.prefix().second, chr);
    CHECK(mr1.suffix().matched);
    CHECK_PTR(&*mr1.suffix().first, chr + 1);
    CHECK_PTR(&*mr1.suffix().second, chr + 2);
    CHECK(mr1[0].matched);
    CHECK_PTR(&*mr1[0].first, chr);
    CHECK_PTR(&*mr1[0].second, chr + 1);

    STD match_results<STDString::const_iterator> mr2;
    CHECK(STD regex_search(str, mr2, r, STD regex_constants::match_default));
    CHECK_INT(mr2.size(), 1);
    CHECK(!mr2.empty());
    CHECK(!mr2.prefix().matched);
    CHECK_PTR(&*mr2.prefix().first, beg);
    CHECK_PTR(&*mr2.prefix().second, beg);
    CHECK(mr2.suffix().matched);
    CHECK_PTR(&*mr2.suffix().first, beg + 1);
    CHECK_INT(mr2.suffix().second - str.begin(), 2);
    CHECK(mr2[0].matched);
    CHECK_PTR(&*mr2[0].first, beg);
    CHECK_PTR(&*mr2[0].second, beg + 1);

    CHECK(STD regex_search(begin, end, r, STD regex_constants::match_default));
    CHECK(STD regex_search(chr, r, STD regex_constants::match_default));
    CHECK(STD regex_search(str, r, STD regex_constants::match_default));
}

static void test_replace() { // test function regex_replace
    CHR out[100];
    STD basic_string<char, STD char_traits<char>, Myalloc<char>> fmt(T("x"));

    const CHR* chr = T("b");
    bidit begin(chr);
    bidit end(chr + xlen(chr));

    MyRgx r0;
    CHR* res = STD regex_replace(out, begin, end, r0, fmt, STD regex_constants::match_default);
    *res     = '\0';
    CHECKSTR(out, T("b"));

    MyRgx r(T("a"));
    res  = STD regex_replace(out, begin, end, r, fmt, STD regex_constants::match_default);
    *res = '\0';
    CHECKSTR(out, T("b"));
    res  = STD regex_replace(out, begin, end, r, fmt, STD regex_constants::format_no_copy);
    *res = '\0';
    CHECKSTR(out, T(""));

    chr   = T("babab");
    begin = bidit(chr);
    end   = bidit(chr + xlen(chr));
    res   = STD regex_replace(out, begin, end, r, fmt, STD regex_constants::match_default);
    *res  = '\0';
    CHECKSTR(out, T("bxbxb"));
    res  = STD regex_replace(out, begin, end, r, fmt, STD regex_constants::format_no_copy);
    *res = '\0';
    CHECKSTR(out, T("xx"));
    res  = STD regex_replace(out, begin, end, r, fmt, STD regex_constants::format_first_only);
    *res = '\0';
    CHECKSTR(out, T("bxbab"));
    res = STD regex_replace(
        out, begin, end, r, fmt, STD regex_constants::format_first_only | STD regex_constants::format_no_copy);
    *res = '\0';
    CHECKSTR(out, T("x"));

    STDString str1(T("b"));
    STDString res1 = STD regex_replace(str1, r, fmt, STD regex_constants::match_default);
    CHECKSTR(res1.c_str(), T("b"));
    res1 = STD regex_replace(str1, r, fmt, STD regex_constants::format_no_copy);
    CHECKSTR(res1.c_str(), T(""));

    str1 = T("babab");
    res1 = STD regex_replace(str1, r, fmt, STD regex_constants::match_default);
    CHECKSTR(res1.c_str(), T("bxbxb"));
    res1 = STD regex_replace(str1, r, fmt, STD regex_constants::format_no_copy);
    CHECKSTR(res1.c_str(), T("xx"));
    res1 = STD regex_replace(str1, r, fmt, STD regex_constants::format_first_only);
    CHECKSTR(res1.c_str(), T("bxbab"));
    res1 =
        STD regex_replace(str1, r, fmt, STD regex_constants::format_first_only | STD regex_constants::format_no_copy);
    CHECKSTR(res1.c_str(), T("x"));
}

static void test_syntax_flags() { // test syntax flags
    MyRgx r0;
    int i, j, ok;
    for (ok = 1, i = sizeof(synflags) / sizeof(synflags[0]); 0 < i;)
        for (j = --i; 0 < j;) { // test an (i, j) pair
            int testno = i * 0x100 + j;
            ok         = CHECK0(ok, testno, synflags[i] != synflags[--j]);
        }
    CHECK_MSG("regex_constants::syntax_option_type values are distinct", ok);

    r0.assign(T("a"));
    CHECK(!STD regex_match(T("A"), r0));
    r0.assign(T("a"), STD regex_constants::icase | STD regex_constants::optimize);
    CHECK(STD regex_match(T("A"), r0));

    r0.assign(T("a(b)(c)"));
    MyMr mr;
    CHECK(STD regex_match(T("abc"), mr, r0));
    CHECK(mr[1].matched);
    r0.assign(T("a(b)(c)"), STD regex_constants::nosubs);
    CHECK(STD regex_match(T("abc"), mr, r0));
    CHECK(!mr[1].matched);

    MyxRgx r1(T("[c-a]"), STD regex_constants::collate);
    CHECK(STD regex_match(T("b"), r1));
    r1.assign(T("d"), STD regex_constants::collate);
    CHECK(STD regex_match(T("f"), r1));
}

static void test_match_flags() { // test match flags
    MyRgx r0;
    CHECK_INT(mtchflags[0], 0);
    CHECK_INT(mtchflags[9], 0);
    int i, j, ok;
    for (ok = 1, i = sizeof(mtchflags) / sizeof(mtchflags[0]); 0 < i;)
        for (j = --i; 0 < j;) { // test an (i, j) pair
            int testno = i * 0x100 + j;
            ok         = CHECK0(ok, testno, mtchflags[i] != mtchflags[--j] || mtchflags[i] == 0);
        }
    CHECK_MSG("regex_constants::match_flag_type values are distinct", ok);

    r0.assign(T("^a"));
    CHECK(STD regex_match(T("a"), r0));
    CHECK(!STD regex_match(T("a"), r0, STD regex_constants::match_not_bol));
    r0.assign(T("a$"));
    CHECK(STD regex_match(T("a"), r0));
    CHECK(!STD regex_match(T("a"), r0, STD regex_constants::match_not_eol));

    r0.assign(T("\\ba"));
    CHECK(STD regex_match(T("a"), r0));
    CHECK(!STD regex_match(T("a"), r0, STD regex_constants::match_not_bow));
    r0.assign(T("a\\b"));
    CHECK(STD regex_match(T("a"), r0));
    CHECK(!STD regex_match(T("a"), r0, STD regex_constants::match_not_eow));

    r0.assign(T("a|ab"), STD regex_constants::extended);
    STD match_results<const CHR*> mr;
    CHECK(STD regex_search(T("ab"), mr, r0, STD regex_constants::match_any));
    CHECK_INT(mr.length(), 1);
    CHECK(STD regex_match(T("ab"), r0, STD regex_constants::match_any));

    r0.assign(T("(a*)"));
    CHECK(STD regex_search(T("b"), r0));
    CHECK(!STD regex_search(T("b"), r0, STD regex_constants::match_not_null));

    r0.assign(T("b"));
    CHECK(STD regex_search(T("ab"), r0));
    CHECK(!STD regex_search(T("ab"), r0, STD regex_constants::match_continuous));
    CHECK(STD regex_search(T("ba"), r0, STD regex_constants::match_continuous));

    r0.assign(T("^a$"));
    const CHR* nl_a = T("\na");
    CHECK(!STD regex_match(nl_a + 1, r0,
        STD regex_constants::match_prev_avail | STD regex_constants::match_not_bol
            | STD regex_constants::match_not_eol));
}

static void test_error_flags() { // test error flags
    int i, j, ok;
    for (ok = 1, i = sizeof(errflags) / sizeof(errflags[0]); 0 < i;)
        for (j = --i; 0 < j;) { // test an (i, j) pair
            int testno = i * 0x100 + j;
            ok         = CHECK0(ok, testno, errflags[i] != errflags[--j]);
        }
    CHECK_MSG("regex_constants::error_type values are distinct", ok);

    STD regex_error x(errflags[0]);
    STD runtime_error* pex = &x;

    if (!terse)
        printf("regex_error().what() returns \"%s\"\n", pex->what());

    CHECK(pex->what() != nullptr);
    CHECK_INT(x.code(), errflags[0]);
}

static void test_error(const CHR* expr, STD regex_constants::error_type err) {
#if NO_EXCEPTIONS
#else // NO_EXCEPTIONS
    try {
        STD basic_regex<CHR> rgx(expr);
    } catch (const STD regex_error& ex) {
        CHECK_INT(ex.code(), err);
        return;
    }
    CHECK_MSG("error should have been caught", false);
#endif // NO_EXCEPTIONS
}

static void test_errors() {
    test_error(T("a{3,2}"), STD regex_constants::error_badbrace);
    test_error(T("a{-1,2}"), STD regex_constants::error_badbrace);
    test_error(T("a{3,-1}"), STD regex_constants::error_badbrace);
    test_error(T("^*"), STD regex_constants::error_badrepeat);
    test_error(T("^?"), STD regex_constants::error_badrepeat);
    test_error(T("^+"), STD regex_constants::error_badrepeat);
    test_error(T("(?!a)*"), STD regex_constants::error_badrepeat);
    test_error(T("(?=a)*"), STD regex_constants::error_badrepeat);
    test_error(T("$*"), STD regex_constants::error_badrepeat);
    test_error(T("$?"), STD regex_constants::error_badrepeat);
    test_error(T("$+"), STD regex_constants::error_badrepeat);
    test_error(T("a{"), STD regex_constants::error_badbrace);
    test_error(T("a}"), STD regex_constants::error_brace);
    test_error(T("a["), STD regex_constants::error_brack);
    test_error(T("a]"), STD regex_constants::error_brack);
    test_error(T("[[:invalid:]]"), STD regex_constants::error_ctype);
    test_error(T("\\c"), STD regex_constants::error_escape);
    test_error(T("("), STD regex_constants::error_paren);
    test_error(T(")"), STD regex_constants::error_paren);
    test_error(T("[b-a]"), STD regex_constants::error_range);
    test_error(T("a\\1"), STD regex_constants::error_backref);
}

void test_main() { // test header <regex>
    test_traits();
    test_regex();
    test_match_results();
    test_format();
    test_regex_iterator();
    test_regex_token_iterator();
    test_match();
    test_search();
    test_replace();
    test_syntax_flags();
    test_match_flags();
    test_error_flags();
    test_errors();
}
