// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <string>, part 2
#define TEST_NAME "<string>, part 2"

#include "tdefs.h"
#include <sstream>
#include <stddef.h>
#include <string.h>
#include <string>
#include <wchar.h>

typedef STD allocator<wchar_t> Myal;
typedef STD wstring Mycont;
typedef STD char_traits<wchar_t> Mytr;

void test_traits() { // test char_traits
    wchar_t ch[]          = L"x123456789";
    Mytr::char_type* pc   = (wchar_t*) nullptr;
    Mytr::int_type* pi    = (CSTD wint_t*) nullptr;
    Mytr::pos_type* pp    = (STD wstreampos*) nullptr;
    Mytr::off_type* po    = (STD streamoff*) nullptr;
    Mytr::state_type* pst = (CSTD mbstate_t*) nullptr;

    pc  = pc; // to quiet diagnostics
    pi  = pi;
    pp  = pp;
    po  = po;
    pst = pst;

    Mytr::assign(ch[0], L'0');
    CHECK_INT(ch[0], L'0');
    CHECK(Mytr::eq(ch[0], ch[0]));
    CHECK(!Mytr::eq(ch[0], L'4'));
    CHECK(Mytr::lt(ch[0], L'4'));
    CHECK(!Mytr::lt(ch[0], ch[0]));
    CHECK(Mytr::compare(L"abc", L"abcd", 3) == 0);
    CHECK(Mytr::compare(L"abc", L"abcd", 4) < 0);
    CHECK_SIZE_T(Mytr::length(L""), 0);
    CHECK_SIZE_T(Mytr::length(ch), 10);
    CHECK_PTR(Mytr::find(ch, 3, L'3'), nullptr);
    CHECK_INT(*Mytr::find(L"abcd", 4, L'd'), L'd');
    CHECK_WSTR(Mytr::move(ch, L"abc", 0), L"0123456789");
    CHECK_WSTR(Mytr::move(&ch[2], ch, 4), L"01236789");
    CHECK_WSTR(Mytr::move(ch, &ch[2], 4), L"0123236789");
    CHECK_WSTR(Mytr::copy(ch, L"abc", 0), L"0123236789");
    CHECK_WSTR(Mytr::copy(&ch[2], ch, 2), L"01236789");
    CHECK_WSTR(Mytr::assign(ch, 2, L'3'), L"3301236789");
    CHECK_INT(Mytr::not_eof(L'e'), L'e');
    CHECK(Mytr::not_eof(WEOF) != WEOF);
    CHECK_INT(Mytr::to_char_type(L'x'), L'x');
    CHECK_INT(Mytr::to_int_type(L'x'), L'x');
    CHECK(Mytr::eq_int_type(L'x', L'x'));
    CHECK(Mytr::eq_int_type(WEOF, WEOF));
    CHECK(!Mytr::eq_int_type(L'x', WEOF));
    CHECK_INT(Mytr::eof(), WEOF);
}

void test_cont() { // test string as container
    wchar_t ch     = L'\0';
    wchar_t carr[] = L"abc";

    Mycont::allocator_type* p_alloc = (Myal*) nullptr;
    Mycont::pointer p_ptr           = (wchar_t*) nullptr;
    Mycont::const_pointer p_cptr    = (const wchar_t*) nullptr;
    Mycont::reference p_ref         = ch;
    Mycont::const_reference p_cref  = (const wchar_t&) ch;
    Mycont::value_type* p_val       = (wchar_t*) nullptr;
    Mycont::size_type* p_size       = (CSTD size_t*) nullptr;
    Mycont::difference_type* p_diff = (CSTD ptrdiff_t*) nullptr;

    p_alloc = p_alloc; // to quiet diagnostics
    p_ptr   = p_ptr;
    p_cptr  = p_cptr;
    p_ref   = p_cref;
    p_size  = p_size;
    p_diff  = p_diff;
    p_val   = p_val;

    Mycont v0;
    Myal al = v0.get_allocator();
    Mycont v0a(al);
    CHECK(v0.empty());
    CHECK_SIZE_T(v0.size(), 0);
    CHECK_SIZE_T(v0a.size(), 0);
    CHECK(v0a.get_allocator() == al);

    Mycont v1(5, L'\0'), v1a(6, L'x'), v1b(7, L'y', al);
    CHECK_SIZE_T(v1.size(), 5);
    CHECK_INT(v1.end()[-1], L'\0');
    CHECK_SIZE_T(v1a.size(), 6);
    CHECK_INT(v1a.end()[-1], L'x');
    CHECK_SIZE_T(v1b.size(), 7);
    CHECK_INT(v1b.end()[-1], L'y');

    Mycont v2(v1a);
    CHECK_SIZE_T(v2.size(), 6);
    CHECK_INT(*v2.begin(), L'x');

    Mycont v3(v1a.begin(), v1a.end());
    CHECK_SIZE_T(v3.size(), 6);
    CHECK_INT(*v3.begin(), L'x');

    const Mycont v4(v1a.begin(), v1a.end(), al);
    CHECK_SIZE_T(v4.size(), 6);
    CHECK_INT(*v4.begin(), L'x');
    v0 = v4;
    CHECK_SIZE_T(v0.size(), 6);
    CHECK_INT(*v0.begin(), L'x');
    CHECK_INT(v0[0], L'x');
    CHECK_INT(v0.at(5), L'x');

    v0.reserve(12);
    CHECK(12 <= v0.capacity());
    v0.resize(8);
    CHECK_SIZE_T(v0.size(), 8);
    CHECK_INT(v0.end()[-1], L'\0');
    v0.resize(10, L'z');
    CHECK_SIZE_T(v0.size(), 10);
    CHECK_INT(v0.end()[-1], L'z');
    CHECK(v0.size() <= v0.max_size());

    STD basic_string<wchar_t, STD char_traits<wchar_t>, STD allocator<wchar_t>>* p_cont = &v0;
    p_cont = p_cont; // to quiet diagnostics

    Mycont::iterator p_it(v0.begin());
    Mycont::const_iterator p_cit(v4.begin());
    Mycont::reverse_iterator p_rit(v0.rbegin());
    Mycont::const_reverse_iterator p_crit(v4.rbegin());
    CHECK_INT(*p_it, L'x');
    CHECK_INT(*--(p_it = v0.end()), L'z');
    CHECK_INT(*p_cit, L'x');
    CHECK_INT(*--(p_cit = v4.end()), L'x');
    CHECK_INT(*p_rit, L'z');
    CHECK_INT(*--(p_rit = v0.rend()), L'x');
    CHECK_INT(*p_crit, L'x');
    CHECK_INT(*--(p_crit = v4.rend()), L'x');

    CHECK_INT(*v0.begin(), L'x');
    CHECK_INT(*v4.begin(), L'x');

    v0.push_back(L'a');
    CHECK_INT(v0.end()[-1], L'a');

    v0.pop_back();
    CHECK_INT(v0.front(), L'x');
    CHECK_INT(v0.back(), L'z');

    v0.shrink_to_fit();
    CHECK_INT(v0.front(), 'x');

    {
        Mycont v5(20, L'x');
        Mycont v6(STD move(v5));
        CHECK_SIZE_T(v5.size(), 0);
        CHECK_SIZE_T(v6.size(), 20);

        Mycont v7;
        v7.assign(STD move(v6));
        CHECK_SIZE_T(v6.size(), 0);
        CHECK_SIZE_T(v7.size(), 20);

        Mycont v8;
        v8 = STD move(v7);
        CHECK_SIZE_T(v7.size(), 0);
        CHECK_SIZE_T(v8.size(), 20);

        Mycont v8a(STD move(v8), Myal());
        CHECK_SIZE_T(v8.size(), 0);
        CHECK_SIZE_T(v8a.size(), 20);
    }

    v0.assign(v4.begin(), v4.end());
    CHECK_SIZE_T(v0.size(), v4.size());
    CHECK_INT(*v0.begin(), *v4.begin());
    v0.assign(4, L'w');
    CHECK_SIZE_T(v0.size(), 4);
    CHECK_INT(*v0.begin(), L'w');
    CHECK_INT(*v0.insert(v0.begin(), L'a'), L'a');
    CHECK_INT(*v0.begin(), L'a');
    CHECK_INT(v0.begin()[1], L'w');
    CHECK_INT(*v0.insert(v0.begin(), 2, L'b'), L'b');
    CHECK_INT(*v0.begin(), L'b');
    CHECK_INT(v0.begin()[1], L'b');
    CHECK_INT(v0.begin()[2], L'a');
    CHECK_INT(*v0.insert(v0.end(), v4.begin(), v4.end()), *v4.begin());
    CHECK_INT(v0.end()[-1], v4.end()[-1]);
    CHECK_INT(*v0.insert(v0.end(), carr, carr + 3), *carr);
    CHECK_INT(v0.end()[-1], L'c');
    v0.erase(v0.begin());
    CHECK_INT(*v0.begin(), L'b');
    CHECK_INT(v0.begin()[1], L'a');
    v0.erase(v0.begin(), v0.begin() + 1);
    CHECK_INT(*v0.begin(), L'a');

    v0.clear();
    CHECK(v0.empty());
    v0.swap(v1);
    CHECK(!v0.empty());
    CHECK(v1.empty());
    STD swap(v0, v1);
    CHECK(v0.empty());
    CHECK(!v1.empty());
    CHECK(v1 == v1);
    CHECK(v0 < v1);
    CHECK(v0 != v1);
    CHECK(v1 > v0);
    CHECK(v0 <= v1);
    CHECK(v1 >= v0);
}

void test_main() { // test basic workings of string definitions
    Mycont s1, s2(30, L'\0');
    Mycont s3(4, L'\0');
    Mycont s4(L"s4"), s5(L"s5xxx", 2), s6(3, L'a');
    Mycont s7(5, L'b'), s8(1, L'c');
    Mycont s9(s7);
    Mycont s10(L"s5xxx", 1, 4);

    Mycont::traits_type* p_tr = (Mytr*) nullptr;

    p_tr = p_tr; // to quiet diagnostics

    CHECK_WSTR(s1.c_str(), L"");
    CHECK_WSTR(s2.c_str(), L"");
    CHECK_WMEM(s3.c_str(), L"\0\0\0\0", 5);
    CHECK_WSTR(s4.c_str(), L"s4");
    CHECK_WSTR(s5.c_str(), L"s5");
    CHECK_WSTR(s6.c_str(), L"aaa");
    CHECK_WSTR(s7.c_str(), L"bbbbb");
    CHECK_WSTR(s8.c_str(), L"c");
    CHECK_SIZE_T(s7.length(), 5);
    CHECK_WSTR(s9.c_str(), L"bbbbb");
    CHECK_WSTR(s10.c_str(), L"5xxx");

    // test allocator constructors
    Myal al;
    Mycont s1a(al);
    Mycont s4a(L"s4", al);
    Mycont s5a(L"s5xxx", 2, al);
    Mycont s6a(3, L'a', al);
    Mycont s10a(Mycont(L"s5xxx"), 1, 4, al);

    CHECK_WSTR(s1a.c_str(), L"");
    CHECK_WSTR(s4a.c_str(), L"s4");
    CHECK_WSTR(s5a.c_str(), L"s5");
    CHECK_WSTR(s6a.c_str(), L"aaa");
    CHECK_WSTR(s10a.c_str(), L"5xxx");

    Mycont s9a(s7, al);
    CHECK_WSTR(s9a.c_str(), L"bbbbb");

    // test assignments
    s1 = L"hello";
    CHECK_WSTR(s1.c_str(), L"hello");
    s1 = L'x';
    CHECK_WSTR(s1.c_str(), L"x");
    s1 = s4;
    CHECK_WSTR(s1.c_str(), L"s4");
    s1.assign(L"AB");
    CHECK_WSTR(s1.c_str(), L"AB");
    s1.assign(1, L'C');
    CHECK_WSTR(s1.c_str(), L"C");
    s1.assign(s4);
    CHECK_WSTR(s1.c_str(), L"s4");

    // test appends
    s1 += L"abc";
    CHECK_WSTR(s1.c_str(), L"s4abc");
    s1 += L'd';
    CHECK_WSTR(s1.c_str(), L"s4abcd");
    s1 += s4;
    CHECK_WSTR(s1.c_str(), L"s4abcds4");
    s1 = L"A";
    s1.append(L"BC");
    CHECK_WSTR(s1.c_str(), L"ABC");
    s1.append(1, L'D');
    CHECK_WSTR(s1.c_str(), L"ABCD");
    s1.append(s4);
    CHECK_WSTR(s1.c_str(), L"ABCDs4");
    CHECK_WSTR((s4 + s5).c_str(), L"s4s5");
    CHECK_WSTR((s4 + L"s5").c_str(), L"s4s5");
    CHECK_WSTR((L"s4" + s5).c_str(), L"s4s5");
    CHECK_WSTR((s4 + L'5').c_str(), L"s45");
    CHECK_WSTR((L'4' + s5).c_str(), L"4s5");

    CHECK(s4 + L"more" == STD wstring(L"s4more"));
    CHECK(L"more" + s4 == STD wstring(L"mores4"));
    CHECK(s4 + L'+' == STD wstring(L"s4+"));
    CHECK(L'+' + s4 == STD wstring(L"+s4"));

    {
        Mycont s11(L"abc");
        Mycont s12(L"def");

        CHECK_WSTR((s11 + STD wstring(L"def")).c_str(), L"abcdef");
        CHECK_WSTR((STD wstring(L"abc") + s12).c_str(), L"abcdef");
        CHECK_WSTR((STD wstring(L"abc") + STD wstring(L"def")).c_str(), L"abcdef");

        CHECK_WSTR((STD wstring(L"abc") + L"def").c_str(), L"abcdef");
        CHECK_WSTR((STD wstring(L"abc") + L'd').c_str(), L"abcd");
        CHECK_WSTR((L"abc" + STD wstring(L"def")).c_str(), L"abcdef");
        CHECK_WSTR((L'a' + STD wstring(L"def")).c_str(), L"adef");

        Mycont s13(STD move(s12));
        CHECK_WSTR(s13.c_str(), L"def");
    }

    // test inserts
    s1 = L"abc";
    s1.insert(3, L"Dd");
    s1.insert(1, L"BC", 1);
    s1.insert(0, L"A");
    CHECK_WSTR(s1.c_str(), L"AaBbcDd");
    s1.insert(7, 2, L'E');
    s1.insert(4, 1, L'C');
    CHECK_WSTR(s1.c_str(), L"AaBbCcDdEE");
    s1.insert(10, s4);
    s1.insert(0, s4, 0, 1);
    CHECK_WSTR(s1.c_str(), L"sAaBbCcDdEEs4");

    // test replaces
    s1 = L"";
    s1.replace((CSTD size_t) 0, (CSTD size_t) 0, L"123ab789");
    s1.replace(3, 2, L"45678", 3);
    CHECK_WSTR(s1.c_str(), L"123456789");
    s1.replace(1, 3, L"xx", 2);
    s1.replace((CSTD size_t) 0, (CSTD size_t) 0, L"0");
    CHECK_WSTR(s1.c_str(), L"01xx56789");
    s1.replace(3, 1, s4, 1, 1);
    s1.replace(2, 1, s4);
    CHECK_WSTR(s1.c_str(), L"01s4456789");

    // test copy
    wchar_t buf[10];
    s1 = L"012XX";
    CHECK_SIZE_T(s1.copy(buf, sizeof(buf) / sizeof(buf[0])), 5);
    CHECK_WMEM(buf, L"012XX", 5);
    CHECK_SIZE_T(s1.copy(buf, 3, 1), 3);
    CHECK_WMEM(buf, L"12X", 3);

    // test finds
    s1 = L"s4s4";
    CHECK_SIZE_T(s1.find(s4), 0);
    CHECK_SIZE_T(s1.find(s4, 1), 2);
    CHECK_SIZE_T(s1.find(s4, 3), STD wstring::npos);
    CHECK_SIZE_T(s1.find(L"s4"), 0);
    CHECK_SIZE_T(s1.find(L"s4", 3), STD wstring::npos);
    CHECK_SIZE_T(s1.find(L"s4XX", 1, 2), 2);
    CHECK_SIZE_T(s1.find(L's'), 0);
    CHECK_SIZE_T(s1.find(L's', 1), 2);
    CHECK_SIZE_T(s1.find(L'x'), STD wstring::npos);

    CHECK_SIZE_T(s1.rfind(s4), 2);
    CHECK_SIZE_T(s1.rfind(s4, 1), 0);
    CHECK_SIZE_T(s1.rfind(s5, 3), STD wstring::npos);
    CHECK_SIZE_T(s1.rfind(L"s4"), 2);
    CHECK_SIZE_T(s1.rfind(L"s4", 3), 2);
    CHECK_SIZE_T(s1.rfind(L"s4XX", 1, 3), STD wstring::npos);
    CHECK_SIZE_T(s1.rfind(L's'), 2);
    CHECK_SIZE_T(s1.rfind(L's', 2), 2);
    CHECK_SIZE_T(s1.rfind(L'x'), STD wstring::npos);

    CHECK_SIZE_T(s1.find_first_of(s4), 0);
    CHECK_SIZE_T(s1.find_first_of(s4, 1), 1);
    CHECK_SIZE_T(s1.find_first_of(s4, 4), STD wstring::npos);
    CHECK_SIZE_T(s1.find_first_of(L"s4"), 0);
    CHECK_SIZE_T(s1.find_first_of(L"s4", 3), 3);
    CHECK_SIZE_T(s1.find_first_of(L"abs", 1, 2), STD wstring::npos);
    CHECK_SIZE_T(s1.find_first_of(L's'), 0);
    CHECK_SIZE_T(s1.find_first_of(L's', 1), 2);
    CHECK_SIZE_T(s1.find_first_of(L'x'), STD wstring::npos);

    CHECK_SIZE_T(s1.find_last_of(s4), 3);
    CHECK_SIZE_T(s1.find_last_of(s4, 1), 1);
    CHECK_SIZE_T(s1.find_last_of(s6), STD wstring::npos);
    CHECK_SIZE_T(s1.find_last_of(L"s4"), 3);
    CHECK_SIZE_T(s1.find_last_of(L"s4", 2), 2);
    CHECK_SIZE_T(s1.find_last_of(L"abs", 1, 2), STD wstring::npos);
    CHECK_SIZE_T(s1.find_last_of(L's'), 2);
    CHECK_SIZE_T(s1.find_last_of(L's', 1), 0);
    CHECK_SIZE_T(s1.find_last_of(L'x'), STD wstring::npos);

    CHECK_SIZE_T(s1.find_first_not_of(s5), 1);
    CHECK_SIZE_T(s1.find_first_not_of(s5, 2), 3);
    CHECK_SIZE_T(s1.find_first_not_of(s4), STD wstring::npos);
    CHECK_SIZE_T(s1.find_first_not_of(L"s5"), 1);
    CHECK_SIZE_T(s1.find_first_not_of(L"s5", 2), 3);
    CHECK_SIZE_T(s1.find_first_not_of(L"s4a", 1, 2), STD wstring::npos);
    CHECK_SIZE_T(s1.find_first_not_of(L's'), 1);
    CHECK_SIZE_T(s1.find_first_not_of(L's', 2), 3);
    CHECK_SIZE_T(s1.find_first_not_of(L's', 4), STD wstring::npos);

    CHECK_SIZE_T(s1.find_last_not_of(s5), 3);
    CHECK_SIZE_T(s1.find_last_not_of(s5, 2), 1);
    CHECK_SIZE_T(s1.find_last_not_of(s4), STD wstring::npos);
    CHECK_SIZE_T(s1.find_last_not_of(L"s5"), 3);
    CHECK_SIZE_T(s1.find_last_not_of(L"s5", 2), 1);
    CHECK_SIZE_T(s1.find_last_not_of(L"s4a", 1, 2), STD wstring::npos);
    CHECK_SIZE_T(s1.find_last_not_of(L's'), 3);
    CHECK_SIZE_T(s1.find_last_not_of(L's', 2), 1);
    CHECK_SIZE_T(s1.find_last_not_of(L's', 0), STD wstring::npos);

    // test compares
    CHECK(s1.compare(s1) == 0);
    CHECK(s1.compare(0, 6, s1) == 0);
    CHECK(s1.compare(2, 2, s4) == 0);
    CHECK(s1.compare(2, 2, s4, 0, 2) == 0);
    CHECK(s1.compare(L"s4s4") == 0);
    CHECK(s1.compare(0, 2, L"s4") == 0);
    CHECK(s1.compare(0, 2, L"s4", 2) == 0);
    CHECK(s1.compare(2, 2, L"s4xx", 0, 2) == 0);
    CHECK(s1.compare(s4) != 0);
    CHECK(s1.compare(1, 4, s1) != 0);
    CHECK(s1.compare(0, 4, s4, 0, 2) != 0);
    CHECK(s1.compare(L"s4s5") != 0);
    CHECK(s1.compare(0, 4, L"s44", 1) != 0);
    CHECK(s1.compare(1, 4, L"s4xx", 0, 2) != 0);

    CHECK(s1 == s1);
    CHECK(s1 == L"s4s4");
    CHECK(L"s4" == s4);
    CHECK(s1 != s4);
    CHECK(s1 != L"xx");
    CHECK(L"s4" != s1);

    CHECK(s4 < s1);
    CHECK(s4 < L"s5");
    CHECK(L"s3" < s4);
    CHECK(s4 <= s1);
    CHECK(s4 <= L"s5");
    CHECK(L"s3" <= s4);

    CHECK(s1 > s4);
    CHECK(L"s5" > s4);
    CHECK(s4 > L"s3");
    CHECK(s1 >= s4);
    CHECK(L"s5" >= s4);
    CHECK(s4 >= L"s3");

    // test I/O
    static const wchar_t input[] = L"s1 s2 s3\n   abc";
    STD wistringstream ins(input);
    STD wostringstream outs;

    ins >> s1;
    CHECK_WSTR(s1.c_str(), L"s1");
    ins >> s1;
    CHECK_WSTR(s1.c_str(), L"s2");
    STD getline(ins, s1);
    CHECK_WSTR(s1.c_str(), L" s3");
    STD getline(ins, s1, L'c');
    CHECK_WSTR(s1.c_str(), L"   ab");
    outs << s1;
    s1 = outs.str();
    CHECK_WSTR(s1.c_str(), L"   ab");

    {
        STD wstringbuf sbuf;
        sbuf.str(L"x");
        STD wistream(&sbuf) >> s1;
        CHECK_WSTR(s1.c_str(), L"x");

        sbuf.str(L"xx\n");
        STD getline(STD wistream(&sbuf), s1);
        CHECK_WSTR(s1.c_str(), L"xx");

        sbuf.str(L"xxxc\n");
        STD getline(STD wistream(&sbuf), s1, L'c');
        CHECK_WSTR(s1.c_str(), L"xxx");

        sbuf.str(L"");
        s1 = L"yy";
        STD wostream(&sbuf) << s1;
        CHECK_WSTR(sbuf.str().c_str(), L"yy");
    }

    { // test sto* functions
        STD size_t idx = 0;
        bool ok        = false;
        STD wstring str(L"x");
        try {
            STD stoi(str, &idx);
        } catch (STD invalid_argument) {
            ok = true;
        } catch (...) {
            CHECK_MSG("unknown exception thrown", false);
        }
        CHECK_MSG("invalid_argument not thrown", ok);
        CHECK_SIZE_T(idx, 0);

        try {
            STD stoi(L"0xfffffffff", &idx, 0);
        } catch (STD out_of_range) {
            ok = true;
        } catch (...) {
            CHECK_MSG("unknown exception thrown", false);
        }
        CHECK_MSG("out_of_range not thrown", ok);

        CHECK_INT(STD stoi(L"-37", &idx), -37);
        CHECK_SIZE_T(idx, 3);
        CHECK_INT(STD stol(L"0x5a", nullptr, 0), 0x5a);
        CHECK_INT(STD stoul(L"5c", nullptr, 16), 0x5c);

        float fl = STD stof(L"-19.25");
        CHECK_DOUBLE(fl, -19.25);
        CHECK_DOUBLE(STD stod(L"-19.25"), -19.25);
        CHECK_DOUBLE(STD stold(L"-19.25", &idx), -19.25);
        CHECK_SIZE_T(idx, 6);

        long long ll = 0xffffffff;
        ll <<= 8;
        CHECK(STD stoll(L"0xffffffff00", nullptr, 0) == ll);
        CHECK(STD stoull(L"0xffffffff00", nullptr, 0) == (unsigned long long) ll);

        CHECK_WSTR(STD to_wstring((int) -23).c_str(), L"-23");
        CHECK_WSTR(STD to_wstring((unsigned int) 23).c_str(), L"23");
        CHECK_WSTR(STD to_wstring((long) -23).c_str(), L"-23");
        CHECK_WSTR(STD to_wstring((unsigned long) 23).c_str(), L"23");
        CHECK_WSTR(STD to_wstring((long long) -23).c_str(), L"-23");
        CHECK_WSTR(STD to_wstring((unsigned long long) 23).c_str(), L"23");
        CHECK_WSTR(STD to_wstring((long double) 23.5).c_str(), L"23.500000");
        CHECK_WSTR(STD to_wstring((float) -23.0f).c_str(), L"-23.000000");
        CHECK_WSTR(STD to_wstring((double) 23.0).c_str(), L"23.000000");
        CHECK_WSTR(STD to_wstring((long double) -23.0L).c_str(), L"-23.000000");
    }

    {
        const wchar_t* data = L"abc";
        STD initializer_list<wchar_t> init(data, data + CSTD wcslen(data));
        STD wstring s11(init);
        CHECK_SIZE_T(s11.size(), 3);
        CHECK_INT(s11[2], L'c');

        STD wstring s12(init, al);
        CHECK_SIZE_T(s12.size(), 3);
        CHECK_INT(s12[2], L'c');

        s11 += init;
        CHECK_SIZE_T(s11.size(), 6);
        CHECK_INT(s11[5], L'c');

        s11 = init;
        CHECK_SIZE_T(s11.size(), 3);
        CHECK_INT(s11[2], L'c');

        s11.append(init);
        CHECK_SIZE_T(s11.size(), 6);
        CHECK_INT(s11[5], L'c');

        s11.assign(init);
        CHECK_SIZE_T(s11.size(), 3);
        CHECK_INT(s11[2], L'c');

        CHECK_INT(*s11.insert(s11.begin() + 1, init), data[0]);
        CHECK_SIZE_T(s11.size(), 6);
        CHECK_INT(s11[2], L'b');

        s11.replace(s11.begin(), s11.begin() + 2, init);
        CHECK_SIZE_T(s11.size(), 7);
        CHECK_INT(s11[2], L'c');
    }

    // test template equivalence
    STD basic_string<wchar_t, STD char_traits<wchar_t>>* ps = (STD wstring*) nullptr;

    ps = ps; // to quiet diagnostics

    test_traits();
    test_cont();

    { // test u16/u32 string
        STD u16string str1(5, 'x');
        STD u32string str2(4, 'y');
        CHECK(str1[4] == 'x');
        CHECK(str2[3] == 'y');
    }
}
