// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <string>, part 1
#define TEST_NAME "<string>, part 1"

#include "tdefs.h"
#include <sstream>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <wchar.h>

typedef STD allocator<char> Myal;
typedef STD string Mycont;
typedef STD char_traits<char> Mytr;

void test_traits() { // test char_traits
    char ch[]             = "x123456789";
    Mytr::char_type* pc   = (char*) nullptr;
    Mytr::int_type* pi    = (int*) nullptr;
    Mytr::pos_type* pp    = (STD streampos*) nullptr;
    Mytr::off_type* po    = (STD streamoff*) nullptr;
    Mytr::state_type* pst = (CSTD mbstate_t*) nullptr;

    pc  = pc; // to quiet diagnostics
    pi  = pi;
    pp  = pp;
    po  = po;
    pst = pst;

    Mytr::assign(ch[0], '0');
    CHECK_INT(ch[0], '0');
    CHECK(Mytr::eq(ch[0], ch[0]));
    CHECK(!Mytr::eq(ch[0], '4'));
    CHECK(Mytr::lt(ch[0], '4'));
    CHECK(!Mytr::lt(ch[0], ch[0]));
    CHECK(Mytr::compare("abc", "abcd", 3) == 0);
    CHECK(Mytr::compare("abc", "abcd", 4) < 0);
    CHECK_SIZE_T(Mytr::length(""), 0);
    CHECK_SIZE_T(Mytr::length(ch), 10);
    CHECK_PTR(Mytr::find(ch, 3, '3'), nullptr);
    CHECK_INT(*Mytr::find("abcd", 4, 'd'), 'd');
    CHECK_STR(Mytr::move(ch, "abc", 0), "0123456789");
    CHECK_STR(Mytr::move(&ch[2], ch, 4), "01236789");
    CHECK_STR(Mytr::move(ch, &ch[2], 4), "0123236789");
    CHECK_STR(Mytr::copy(ch, "abc", 0), "0123236789");
    CHECK_STR(Mytr::copy(&ch[2], ch, 2), "01236789");
    CHECK_STR(Mytr::assign(ch, 2, '3'), "3301236789");
    CHECK_INT(Mytr::not_eof('e'), 'e');
    CHECK(Mytr::not_eof(EOF) != EOF);
    CHECK_INT(Mytr::to_char_type('x'), 'x');
    CHECK_INT(Mytr::to_int_type('x'), 'x');
    CHECK(Mytr::eq_int_type('x', 'x'));
    CHECK(Mytr::eq_int_type(EOF, EOF));
    CHECK(!Mytr::eq_int_type('x', EOF));
    CHECK_INT(Mytr::eof(), EOF);

    CHECK_TYPE(STD streampos, STD u16streampos);
    CHECK_TYPE(STD streampos, STD u32streampos);
}

void test_cont() { // test string as container
    char ch     = '\0';
    char carr[] = "abc";

    Mycont::allocator_type* p_alloc = (Myal*) nullptr;
    Mycont::pointer p_ptr           = (char*) nullptr;
    Mycont::const_pointer p_cptr    = (const char*) nullptr;
    Mycont::reference p_ref         = ch;
    Mycont::const_reference p_cref  = (const char&) ch;
    Mycont::value_type* p_val       = (char*) nullptr;
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

    Mycont v1(5, '\0'), v1a(6, 'x'), v1b(7, 'y', al);
    CHECK_SIZE_T(v1.size(), 5);
    CHECK_INT(v1.end()[-1], '\0');
    CHECK_SIZE_T(v1a.size(), 6);
    CHECK_INT(v1a.end()[-1], 'x');
    CHECK_SIZE_T(v1b.size(), 7);
    CHECK_INT(v1b.end()[-1], 'y');

    Mycont v2(v1a);
    CHECK_SIZE_T(v2.size(), 6);
    CHECK_INT(*v2.begin(), 'x');

    Mycont v3(v1a.begin(), v1a.end());
    CHECK_SIZE_T(v3.size(), 6);
    CHECK_INT(*v3.begin(), 'x');

    const Mycont v4(v1a.begin(), v1a.end(), al);
    CHECK_SIZE_T(v4.size(), 6);
    CHECK_INT(*v4.begin(), 'x');
    v0 = v4;
    CHECK_SIZE_T(v0.size(), 6);
    CHECK_INT(*v0.begin(), 'x');
    CHECK_INT(v0[0], 'x');
    CHECK_INT(v0.at(5), 'x');

    v0.reserve(12);
    CHECK(12 <= v0.capacity());
    v0.resize(8);
    CHECK_SIZE_T(v0.size(), 8);
    CHECK_INT(v0.end()[-1], '\0');
    v0.resize(10, 'z');
    CHECK_SIZE_T(v0.size(), 10);
    CHECK_INT(v0.end()[-1], 'z');
    CHECK(v0.size() <= v0.max_size());

    STD basic_string<char, STD char_traits<char>, STD allocator<char>>* p_cont = &v0;

    p_cont = p_cont; // to quiet diagnostics

    { // check iterators generators
        Mycont::iterator p_it(v0.begin());
        Mycont::const_iterator p_cit(v4.begin());
        Mycont::reverse_iterator p_rit(v0.rbegin());
        Mycont::const_reverse_iterator p_crit(v4.rbegin());
        CHECK_INT(*p_it, 'x');
        CHECK_INT(*--(p_it = v0.end()), 'z');
        CHECK_INT(*p_cit, 'x');
        CHECK_INT(*--(p_cit = v4.end()), 'x');
        CHECK_INT(*p_rit, 'z');
        CHECK_INT(*--(p_rit = v0.rend()), 'x');
        CHECK_INT(*p_crit, 'x');
        CHECK_INT(*--(p_crit = v4.rend()), 'x');
    }

    { // check const iterators generators
        Mycont::const_iterator p_it(v0.cbegin());
        Mycont::const_iterator p_cit(v4.cbegin());
        Mycont::const_reverse_iterator p_rit(v0.crbegin());
        Mycont::const_reverse_iterator p_crit(v4.crbegin());
        CHECK_INT(*p_it, 'x');
        CHECK_INT(*--(p_it = v0.cend()), 'z');
        CHECK_INT(*p_cit, 'x');
        CHECK_INT(*--(p_cit = v4.cend()), 'x');
        CHECK_INT(*p_rit, 'z');
        CHECK_INT(*--(p_rit = v0.crend()), 'x');
        CHECK_INT(*p_crit, 'x');
        CHECK_INT(*--(p_crit = v4.crend()), 'x');
    }

    CHECK_INT(*v0.begin(), 'x');
    CHECK_INT(*v4.begin(), 'x');

    v0.push_back('a');
    CHECK_INT(v0.end()[-1], 'a');

    v0.pop_back();
    CHECK_INT(v0.front(), 'x');
    CHECK_INT(v0.back(), 'z');

    v0.shrink_to_fit();
    CHECK_INT(v0.front(), 'x');

    {
        Mycont v5(20, 'x');
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
    v0.assign(4, 'w');
    CHECK_SIZE_T(v0.size(), 4);
    CHECK_INT(*v0.begin(), 'w');
    CHECK_INT(*v0.insert(v0.begin(), 'a'), 'a');
    CHECK_INT(*v0.begin(), 'a');
    CHECK_INT(v0.begin()[1], 'w');
    CHECK_INT(*v0.insert(v0.begin(), 2, 'b'), 'b');
    CHECK_INT(*v0.begin(), 'b');
    CHECK_INT(v0.begin()[1], 'b');
    CHECK_INT(v0.begin()[2], 'a');
    CHECK_INT(*v0.insert(v0.end(), v4.begin(), v4.end()), *v4.begin());
    CHECK_INT(v0.end()[-1], v4.end()[-1]);
    CHECK_INT(*v0.insert(v0.end(), carr, carr + 3), *carr);
    CHECK_INT(v0.end()[-1], 'c');
    v0.erase(v0.begin());
    CHECK_INT(*v0.begin(), 'b');
    CHECK_INT(v0.begin()[1], 'a');
    v0.erase(v0.begin(), v0.begin() + 1);
    CHECK_INT(*v0.begin(), 'a');

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
    Mycont s1, s2(30, '\0');
    Mycont s3(4, '\0');
    Mycont s4("s4"), s5("s5xxx", 2), s6(3, 'a');
    Mycont s7(5, 'b'), s8(1, 'c');
    Mycont s9(s7);
    Mycont s10("s5xxx", 1, 4);

    Mycont::traits_type* p_tr = (Mytr*) nullptr;

    p_tr = p_tr; // to quiet diagnostics

    CHECK_STR(s1.c_str(), "");
    CHECK_STR(s2.c_str(), "");
    CHECK_MEM(s3.c_str(), "\0\0\0\0", 5);
    CHECK_STR(s4.c_str(), "s4");
    CHECK_STR(s5.c_str(), "s5");
    CHECK_STR(s6.c_str(), "aaa");
    CHECK_STR(s7.c_str(), "bbbbb");
    CHECK_STR(s8.c_str(), "c");
    CHECK_SIZE_T(s7.length(), 5);
    CHECK_STR(s9.c_str(), "bbbbb");
    CHECK_STR(s10.c_str(), "5xxx");

    // test allocator constructors
    Myal al;
    Mycont s1a(al);
    Mycont s4a("s4", al);
    Mycont s5a("s5xxx", 2, al);
    Mycont s6a(3, 'a', al);
    Mycont s10a(Mycont("s5xxx"), 1, 4, al);

    CHECK_STR(s1a.c_str(), "");
    CHECK_STR(s4a.c_str(), "s4");
    CHECK_STR(s5a.c_str(), "s5");
    CHECK_STR(s6a.c_str(), "aaa");
    CHECK_STR(s10a.c_str(), "5xxx");

    Mycont s9a(s7, al);
    CHECK_STR(s9a.c_str(), "bbbbb");

    // test assignments
    s1 = "hello";
    CHECK_STR(s1.c_str(), "hello");
    s1 = 'x';
    CHECK_STR(s1.c_str(), "x");
    s1 = s4;
    CHECK_STR(s1.c_str(), "s4");
    s1.assign("AB");
    CHECK_STR(s1.c_str(), "AB");
    s1.assign(1, 'C');
    CHECK_STR(s1.c_str(), "C");
    s1.assign(s4);
    CHECK_STR(s1.c_str(), "s4");

    // test appends
    s1 += "abc";
    CHECK_STR(s1.c_str(), "s4abc");
    s1 += 'd';
    CHECK_STR(s1.c_str(), "s4abcd");
    s1 += s4;
    CHECK_STR(s1.c_str(), "s4abcds4");
    s1 = "A";
    s1.append("BC");
    CHECK_STR(s1.c_str(), "ABC");
    s1.append(1, 'D');
    CHECK_STR(s1.c_str(), "ABCD");
    s1.append(s4);
    CHECK_STR(s1.c_str(), "ABCDs4");
    CHECK_STR((s4 + s5).c_str(), "s4s5");
    CHECK_STR((s4 + "s5").c_str(), "s4s5");
    CHECK_STR(("s4" + s5).c_str(), "s4s5");
    CHECK_STR((s4 + '5').c_str(), "s45");
    CHECK_STR(('4' + s5).c_str(), "4s5");

    CHECK(s4 + "more" == STD string("s4more"));
    CHECK("more" + s4 == STD string("mores4"));
    CHECK(s4 + '+' == STD string("s4+"));
    CHECK('+' + s4 == STD string("+s4"));

    {
        Mycont s11("abc");
        Mycont s12("def");

        CHECK_STR((s11 + STD string("def")).c_str(), "abcdef");
        CHECK_STR((STD string("abc") + s12).c_str(), "abcdef");
        CHECK_STR((STD string("abc") + STD string("def")).c_str(), "abcdef");

        CHECK_STR((STD string("abc") + "def").c_str(), "abcdef");
        CHECK_STR((STD string("abc") + 'd').c_str(), "abcd");
        CHECK_STR(("abc" + STD string("def")).c_str(), "abcdef");
        CHECK_STR(('a' + STD string("def")).c_str(), "adef");

        Mycont s13(STD move(s12));
        CHECK_STR(s13.c_str(), "def");
    }

    // test inserts
    s1 = "abc";
    s1.insert(3, "Dd");
    s1.insert(1, "BC", 1);
    s1.insert(0, "A");
    CHECK_STR(s1.c_str(), "AaBbcDd");
    s1.insert(7, 2, 'E');
    s1.insert(4, 1, 'C');
    CHECK_STR(s1.c_str(), "AaBbCcDdEE");
    s1.insert(10, s4);
    s1.insert(0, s4, 0, 1);
    CHECK_STR(s1.c_str(), "sAaBbCcDdEEs4");

    // test replaces
    s1 = "";
    s1.replace((CSTD size_t) 0, (CSTD size_t) 0, "123ab789");
    s1.replace(3, 2, "45678", 3);
    CHECK_STR(s1.c_str(), "123456789");
    s1.replace(1, 3, "xx", 2);
    s1.replace((CSTD size_t) 0, (CSTD size_t) 0, "0");
    CHECK_STR(s1.c_str(), "01xx56789");
    s1.replace(3, 1, s4, 1, 1);
    s1.replace(2, 1, s4);
    CHECK_STR(s1.c_str(), "01s4456789");

    // test copy
    char buf[10];
    s1 = "012XX";
    CHECK_SIZE_T(s1.copy(buf, sizeof(buf)), 5);
    CHECK_MEM(buf, "012XX", 5);
    CHECK_SIZE_T(s1.copy(buf, 3, 1), 3);
    CHECK_MEM(buf, "12X", 3);

    // test finds
    s1 = "s4s4";
    CHECK_SIZE_T(s1.find(s4), 0);
    CHECK_SIZE_T(s1.find(s4, 1), 2);
    CHECK_SIZE_T(s1.find(s4, 3), STD string::npos);
    CHECK_SIZE_T(s1.find("s4"), 0);
    CHECK_SIZE_T(s1.find("s4", 3), STD string::npos);
    CHECK_SIZE_T(s1.find("s4XX", 1, 2), 2);
    CHECK_SIZE_T(s1.find('s'), 0);
    CHECK_SIZE_T(s1.find('s', 1), 2);
    CHECK_SIZE_T(s1.find('x'), STD string::npos);

    CHECK_SIZE_T(s1.rfind(s4), 2);
    CHECK_SIZE_T(s1.rfind(s4, 1), 0);
    CHECK_SIZE_T(s1.rfind(s5, 3), STD string::npos);
    CHECK_SIZE_T(s1.rfind("s4"), 2);
    CHECK_SIZE_T(s1.rfind("s4", 3), 2);
    CHECK_SIZE_T(s1.rfind("s4XX", 1, 3), STD string::npos);
    CHECK_SIZE_T(s1.rfind('s'), 2);
    CHECK_SIZE_T(s1.rfind('s', 2), 2);
    CHECK_SIZE_T(s1.rfind('x'), STD string::npos);

    CHECK_SIZE_T(s1.find_first_of(s4), 0);
    CHECK_SIZE_T(s1.find_first_of(s4, 1), 1);
    CHECK_SIZE_T(s1.find_first_of(s4, 4), STD string::npos);
    CHECK_SIZE_T(s1.find_first_of("s4"), 0);
    CHECK_SIZE_T(s1.find_first_of("s4", 3), 3);
    CHECK_SIZE_T(s1.find_first_of("abs", 1, 2), STD string::npos);
    CHECK_SIZE_T(s1.find_first_of('s'), 0);
    CHECK_SIZE_T(s1.find_first_of('s', 1), 2);
    CHECK_SIZE_T(s1.find_first_of('x'), STD string::npos);

    CHECK_SIZE_T(s1.find_last_of(s4), 3);
    CHECK_SIZE_T(s1.find_last_of(s4, 1), 1);
    CHECK_SIZE_T(s1.find_last_of(s6), STD string::npos);
    CHECK_SIZE_T(s1.find_last_of("s4"), 3);
    CHECK_SIZE_T(s1.find_last_of("s4", 2), 2);
    CHECK_SIZE_T(s1.find_last_of("abs", 1, 2), STD string::npos);
    CHECK_SIZE_T(s1.find_last_of('s'), 2);
    CHECK_SIZE_T(s1.find_last_of('s', 1), 0);
    CHECK_SIZE_T(s1.find_last_of('x'), STD string::npos);

    CHECK_SIZE_T(s1.find_first_not_of(s5), 1);
    CHECK_SIZE_T(s1.find_first_not_of(s5, 2), 3);
    CHECK_SIZE_T(s1.find_first_not_of(s4), STD string::npos);
    CHECK_SIZE_T(s1.find_first_not_of("s5"), 1);
    CHECK_SIZE_T(s1.find_first_not_of("s5", 2), 3);
    CHECK_SIZE_T(s1.find_first_not_of("s4a", 1, 2), STD string::npos);
    CHECK_SIZE_T(s1.find_first_not_of('s'), 1);
    CHECK_SIZE_T(s1.find_first_not_of('s', 2), 3);
    CHECK_SIZE_T(s1.find_first_not_of('s', 4), STD string::npos);

    CHECK_SIZE_T(s1.find_last_not_of(s5), 3);
    CHECK_SIZE_T(s1.find_last_not_of(s5, 2), 1);
    CHECK_SIZE_T(s1.find_last_not_of(s4), STD string::npos);
    CHECK_SIZE_T(s1.find_last_not_of("s5"), 3);
    CHECK_SIZE_T(s1.find_last_not_of("s5", 2), 1);
    CHECK_SIZE_T(s1.find_last_not_of("s4a", 1, 2), STD string::npos);
    CHECK_SIZE_T(s1.find_last_not_of('s'), 3);
    CHECK_SIZE_T(s1.find_last_not_of('s', 2), 1);
    CHECK_SIZE_T(s1.find_last_not_of('s', 0), STD string::npos);

    // test compares
    CHECK(s1.compare(s1) == 0);
    CHECK(s1.compare(0, 6, s1) == 0);
    CHECK(s1.compare(2, 2, s4) == 0);
    CHECK(s1.compare(2, 2, s4, 0, 2) == 0);
    CHECK(s1.compare("s4s4") == 0);
    CHECK(s1.compare(0, 2, "s4") == 0);
    CHECK(s1.compare(0, 2, "s4", 2) == 0);
    CHECK(s1.compare(2, 2, "s4xx", 0, 2) == 0);
    CHECK(s1.compare(s4) != 0);
    CHECK(s1.compare(1, 4, s1) != 0);
    CHECK(s1.compare(0, 4, s4, 0, 2) != 0);
    CHECK(s1.compare("s4s5") != 0);
    CHECK(s1.compare(0, 4, "s44", 1) != 0);
    CHECK(s1.compare(1, 4, "s4xx", 0, 2) != 0);

    CHECK(s1 == s1);
    CHECK(s1 == "s4s4");
    CHECK("s4" == s4);
    CHECK(s1 != s4);
    CHECK(s1 != "xx");
    CHECK("s4" != s1);

    CHECK(s4 < s1);
    CHECK(s4 < "s5");
    CHECK("s3" < s4);
    CHECK(s4 <= s1);
    CHECK(s4 <= "s5");
    CHECK("s3" <= s4);

    CHECK(s1 > s4);
    CHECK("s5" > s4);
    CHECK(s4 > "s3");
    CHECK(s1 >= s4);
    CHECK("s5" >= s4);
    CHECK(s4 >= "s3");

    // test I/O
    static const char input[] = "s1 s2 s3\n   abc";
    STD istringstream ins(input);
    STD ostringstream outs;

    ins >> s1;
    CHECK_STR(s1.c_str(), "s1");
    ins >> s1;
    CHECK_STR(s1.c_str(), "s2");
    STD getline(ins, s1);
    CHECK_STR(s1.c_str(), " s3");
    STD getline(ins, s1, 'c');
    CHECK_STR(s1.c_str(), "   ab");
    outs << s1;
    s1 = outs.str();
    CHECK_STR(s1.c_str(), "   ab");

    {
        STD stringbuf sbuf;
        sbuf.str("x");
        STD istream(&sbuf) >> s1;
        CHECK_STR(s1.c_str(), "x");

        sbuf.str("xx\n");
        STD getline(STD istream(&sbuf), s1);
        CHECK_STR(s1.c_str(), "xx");

        sbuf.str("xxxc\n");
        STD getline(STD istream(&sbuf), s1, 'c');
        CHECK_STR(s1.c_str(), "xxx");

        sbuf.str("");
        s1 = "yy";
        STD ostream(&sbuf) << s1;
        CHECK_STR(sbuf.str().c_str(), "yy");
    }

    { // test sto* functions
        STD size_t idx = 0;
        bool ok        = false;
        STD string str("x");
        try {
            (void) STD stoi(str, &idx);
        } catch (STD invalid_argument) {
            ok = true;
        } catch (...) {
            CHECK_MSG("unknown exception thrown", false);
        }
        CHECK_MSG("invalid_argument not thrown", ok);
        CHECK_SIZE_T(idx, 0);

        try {
            (void) STD stoi("0xfffffffff", &idx, 0);
        } catch (STD out_of_range) {
            ok = true;
        } catch (...) {
            CHECK_MSG("unknown exception thrown", false);
        }
        CHECK_MSG("out_of_range not thrown", ok);

        CHECK_INT(STD stoi("-37", &idx), -37);
        CHECK_SIZE_T(idx, 3);
        CHECK_INT(STD stol("0x5a", nullptr, 0), 0x5a);
        CHECK_INT(STD stoul("5c", nullptr, 16), 0x5c);

        float fl = STD stof("-19.25");
        CHECK_DOUBLE(fl, -19.25);
        CHECK_DOUBLE(STD stod("-19.25"), -19.25);
        CHECK_DOUBLE(STD stold("-19.25", &idx), -19.25);
        CHECK_SIZE_T(idx, 6);

        long long ll = 0xffffffff;
        ll <<= 8;
        CHECK(STD stoll("0xffffffff00", nullptr, 0) == ll);
        CHECK(STD stoull("0xffffffff00", nullptr, 0) == (unsigned long long) ll);

        CHECK_STR(STD to_string((int) -23).c_str(), "-23");
        CHECK_STR(STD to_string((unsigned int) 23).c_str(), "23");
        CHECK_STR(STD to_string((long) -23).c_str(), "-23");
        CHECK_STR(STD to_string((unsigned long) 23).c_str(), "23");
        CHECK_STR(STD to_string((long long) -23).c_str(), "-23");
        CHECK_STR(STD to_string((unsigned long long) 23).c_str(), "23");
        CHECK_STR(STD to_string((long double) 23.5).c_str(), "23.500000");
        CHECK_STR(STD to_string((float) -23.0f).c_str(), "-23.000000");
        CHECK_STR(STD to_string((double) 23.0).c_str(), "23.000000");
        CHECK_STR(STD to_string((long double) -23.0L).c_str(), "-23.000000");
    }

    { // test hash functors
        size_t hash_val = STD hash<STD string>()(STD string("abc"));
        hash_val        = STD hash<STD wstring>()(STD wstring(L"abc"));
        hash_val        = STD hash<STD u16string>()(STD u16string(3, 'x'));
        hash_val        = STD hash<STD u32string>()(STD u32string(3, 'x'));
        (void) hash_val;
    }

    {
        STD initializer_list<char> init{'a', 'b', 'c'};
        STD string s11(init);
        CHECK_SIZE_T(s11.size(), 3);
        CHECK_INT(s11[2], 'c');

        STD string s12(init, al);
        CHECK_SIZE_T(s12.size(), 3);
        CHECK_INT(s12[2], 'c');

        s11 += init;
        CHECK_SIZE_T(s11.size(), 6);
        CHECK_INT(s11[5], 'c');

        s11 = init;
        CHECK_SIZE_T(s11.size(), 3);
        CHECK_INT(s11[2], 'c');

        s11.append(init);
        CHECK_SIZE_T(s11.size(), 6);
        CHECK_INT(s11[5], 'c');

        s11.assign(init);
        CHECK_SIZE_T(s11.size(), 3);
        CHECK_INT(s11[2], 'c');

        CHECK_INT(*s11.insert(s11.begin() + 1, init), *init.begin());
        CHECK_SIZE_T(s11.size(), 6);
        CHECK_INT(s11[2], 'b');

        s11.replace(s11.begin(), s11.begin() + 2, init);
        CHECK_SIZE_T(s11.size(), 7);
        CHECK_INT(s11[2], 'c');
    }

    // test template equivalence
    STD basic_string<char, STD char_traits<char>>* ps = (STD string*) nullptr;

    ps = ps; // to quiet diagnostics

    test_traits();
    test_cont();
}
