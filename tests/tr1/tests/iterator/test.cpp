// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <iterator>
#define TEST_NAME "<iterator>"

#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING

#include "tdefs.h"
#include <deque>
#include <iterator>
#include <sstream>
#include <stddef.h>
#include <string>

typedef STD char_traits<char> Char_traits_char;
typedef STD basic_istream<char, STD char_traits<char>> Char_istream;
typedef STD basic_ostream<char, STD char_traits<char>> Char_ostream;
typedef STD basic_streambuf<char, STD char_traits<char>> Char_streambuf;

#define MAKE_PTRIT(str) &*strit
typedef const char* CPtrIt;
typedef char* PtrIt;

void takes_ran_tag(STD random_access_iterator_tag) { // callable only with random-access iterator
}

void test_prop() { // test iterator properties
    STD random_access_iterator_tag* ran_tag = (STD random_access_iterator_tag*) nullptr;
    STD bidirectional_iterator_tag* bid_tag = ran_tag;
    STD forward_iterator_tag* fwd_tag       = bid_tag;
    STD input_iterator_tag* in_tag          = fwd_tag;
    STD output_iterator_tag* out_tag        = nullptr;

    in_tag  = in_tag; // to quiet diagnostics
    out_tag = out_tag;

    typedef STD iterator<STD input_iterator_tag, float, short, float*, float&> Iter;
    float fl                        = 0.0f;
    Iter::iterator_category* it_tag = (STD input_iterator_tag*) nullptr;
    Iter::value_type* it_val        = (float*) nullptr;
    Iter::difference_type* it_dist  = (short*) nullptr;
    Iter::pointer it_ptr            = (float*) nullptr;
    Iter::reference it_ref          = fl;

    it_tag  = it_tag; // to quiet diagnostics
    it_val  = it_val;
    it_dist = it_dist;
    it_ptr  = it_ptr;
    it_ptr  = &it_ref;

    typedef STD iterator_traits<Iter> Traits;
    Traits::iterator_category* tr_tag = (STD input_iterator_tag*) nullptr;
    Traits::value_type* tr_val        = (float*) nullptr;
    Traits::difference_type* tr_dist  = (short*) nullptr;
    Traits::pointer tr_ptr            = (float*) nullptr;
    Traits::reference tr_ref          = fl;

    tr_tag  = tr_tag; // to quiet diagnostics
    tr_val  = tr_val;
    tr_dist = tr_dist;
    tr_ptr  = tr_ptr;
    tr_ptr  = &tr_ref;

    typedef STD iterator_traits<PtrIt> Ptraits;
    char ch = '\0';
    takes_ran_tag(Ptraits::iterator_category());
    Ptraits::value_type* ptr_val       = (char*) nullptr;
    Ptraits::difference_type* ptr_dist = (CSTD ptrdiff_t*) nullptr;
    Ptraits::pointer ptr_ptr           = (char*) nullptr;
    Ptraits::reference ptr_ref         = ch;

    ptr_val  = ptr_val; // to quiet diagnostics
    ptr_dist = ptr_dist;
    ptr_ptr  = ptr_ptr;
    ptr_ptr  = &ptr_ref;

    typedef STD iterator_traits<CPtrIt> CPtraits;
    takes_ran_tag(CPtraits::iterator_category());
    CPtraits::value_type* cptr_val       = (char*) nullptr;
    CPtraits::difference_type* cptr_dist = (CSTD ptrdiff_t*) nullptr;
    CPtraits::pointer cptr_ptr           = (char*) nullptr;
    CPtraits::reference cptr_ref         = ch;

    cptr_val  = cptr_val; // to quiet diagnostics
    cptr_dist = cptr_dist;
    cptr_ptr  = cptr_ptr;
    cptr_ptr  = &cptr_ref;

    const char* pc = "abcdefg";
    STD advance(pc, 4);
    CHECK_INT(*pc, 'e');
    STD advance(pc, -1);
    CHECK_INT(*pc, 'd');
    CHECK_INT(STD distance(pc, pc + 3), 3);

    const char* pc2 = "abcdefg";
    CHECK_INT(*STD next(pc2), 'b');
    pc2 = STD next(pc2, 3);
    CHECK_INT(*pc2, 'd');
    CHECK_INT(*STD prev(pc2), 'c');
    CHECK_INT(*STD prev(pc2, 2), 'b');
}

typedef STD reverse_iterator<PtrIt> RevIt;

class MyrevIt : public RevIt {
public:
    MyrevIt(RevIt::iterator_type p) : RevIt(p) { // construct from iterator p
    }

    RevIt::iterator_type get_current() const { // get value
        return current;
    }
};

void test_revit() { // test reverse_iterator
    const char abcdefg[] = "abcdefg";
    STD deque<char> str(&abcdefg[0], &abcdefg[7]);
    STD deque<char>::iterator strit = str.begin() + 3;
    PtrIt pcit                      = MAKE_PTRIT(strit);
    RevIt::iterator_type* p_iter    = (PtrIt*) nullptr;
    RevIt rit0, rit(pcit);
    (void) rit0;

    p_iter = p_iter; // to quiet diagnostics

    CHECK_PTR(&*rit.base(), &*pcit);
    CHECK_INT(*rit, 'c');
    CHECK_INT(*++rit, 'b');
    CHECK_INT(*rit++, 'b');
    CHECK_INT(*rit, 'a');
    CHECK_INT(*--rit, 'b');
    CHECK_INT(*rit--, 'b');
    CHECK_INT(*rit, 'c');
    CHECK_INT(*(rit += 2), 'a');
    CHECK_INT(*(rit -= 2), 'c');
    CHECK_INT(*(rit + 2), 'a');
    CHECK_INT(*rit, 'c');
    CHECK_INT(*(rit - 2), 'e');
    CHECK_INT(*rit, 'c');
    CHECK_INT(rit[2], 'a');
    CHECK(rit == rit);
    CHECK(!(rit != rit));
    CHECK(!(rit < rit));
    CHECK(rit < rit + 1);
    CHECK(!(rit > rit));
    CHECK(rit <= rit);
    CHECK(rit >= rit);
    CHECK_INT((rit + 2) - rit, 2);

    MyrevIt myrit(pcit);
    CHECK(myrit.get_current() == pcit);
}

void test_movit() { // test move_iterator
    typedef STD move_iterator<PtrIt> MovIt;

    STD string str("abcde");
    STD string::iterator strit   = str.begin() + 2;
    PtrIt pcit                   = MAKE_PTRIT(strit);
    MovIt::iterator_type* p_iter = (PtrIt*) nullptr;
    MovIt mit0, mit(pcit);
    (void) mit0;

    p_iter = p_iter; // to quiet diagnostics

    CHECK_PTR(&*mit.base(), &*pcit);
    CHECK_INT(*mit, 'c');
    CHECK_INT(*++mit, 'd');
    CHECK_INT(*mit++, 'd');
    CHECK_INT(*mit, 'e');
    CHECK_INT(*--mit, 'd');
    CHECK_INT(*mit--, 'd');
    CHECK_INT(*mit, 'c');
    CHECK_INT(*(mit += 2), 'e');
    CHECK_INT(*(mit -= 2), 'c');
    CHECK_INT(*(mit + 2), 'e');
    CHECK_INT(*mit, 'c');
    CHECK_INT(*(mit - 2), 'a');
    CHECK_INT(*mit, 'c');
    CHECK_INT(mit[2], 'e');
    CHECK(mit == mit);
    CHECK(!(mit != mit));
    CHECK(!(mit < mit));
    CHECK(mit < mit + 1);
    CHECK(!(mit > mit));
    CHECK(mit <= mit);
    CHECK(mit >= mit);
    CHECK_INT((mit + 2) - mit, 2);

    MovIt mymit(pcit);
    CHECK(mymit.base() == pcit);

    Movable_int mi1(3);
    Movable_int mi2 = *STD make_move_iterator(&mi1);
    CHECK_INT(mi1.val, -1);
    CHECK_INT(mi2.val, 3);
}

// test insertion iterators
typedef STD deque<char> Cont;
typedef STD back_insert_iterator<Cont> BackIt;
typedef STD front_insert_iterator<Cont> FrontIt;
typedef STD insert_iterator<Cont> InsIt;

class MybackIt : public BackIt {
public:
    MybackIt(BackIt::container_type& c) : BackIt(c) { // construct from container
    }

    BackIt::container_type* get_container() const { // get pointer to container
        return container;
    }
};

class MyfrontIt : public FrontIt {
public:
    MyfrontIt(FrontIt::container_type& c) : FrontIt(c) { // construct from container
    }

    FrontIt::container_type* get_container() const { // get pointer to container
        return container;
    }
};

class MyinsIt : public InsIt {
public:
    MyinsIt(InsIt::container_type& c, Cont::iterator it) : InsIt(c, it) { // construct from container and iterator
    }

    InsIt::container_type* get_container() const { // get pointer to container
        return container;
    }

    Cont::iterator get_iterator() const { // return iterator
        return iter;
    }
};

void test_inserts() { // test insertion iterators
    Cont c0;
    BackIt::container_type* pbi_cont = (Cont*) nullptr;
    BackIt::value_type* pbi_val      = (char*) nullptr;
    BackIt bit(c0);

    pbi_cont = pbi_cont; // to quiet diagnostics
    pbi_val  = pbi_val;

    *bit   = 'a', ++bit;
    *bit++ = 'b';
    CHECK_INT(c0[0], 'a');
    CHECK_INT(c0[1], 'b');

    MybackIt mybkit(c0);
    CHECK_PTR(mybkit.get_container(), &c0);
    *STD back_inserter(c0)++ = 'x';
    CHECK_INT(c0[2], 'x');

    {
        typedef STD deque<Movable_int> Mycont2;
        typedef STD back_insert_iterator<Mycont2> MyBackIt2;
        Mycont2 c1;
        MyBackIt2 bi1(c1);
        Movable_int mi1(3);
        *bi1 = STD move(mi1);
        CHECK_INT(mi1.val, -1);
        CHECK_INT(c1[0].val, 3);
    }

    FrontIt::container_type* pfi_cont = (Cont*) nullptr;
    FrontIt::value_type* pfi_val      = (char*) nullptr;
    FrontIt fit(c0);

    pfi_cont = pfi_cont; // to quiet diagnostics
    pfi_val  = pfi_val;

    *fit   = 'c', ++fit;
    *fit++ = 'd';
    CHECK_INT(c0[0], 'd');
    CHECK_INT(c0[1], 'c');

    MyfrontIt myfrit(c0);
    CHECK_PTR(myfrit.get_container(), &c0);
    *STD front_inserter(c0)++ = 'y';
    CHECK_INT(c0[0], 'y');

    {
        typedef STD deque<Movable_int> Mycont2;
        typedef STD front_insert_iterator<Mycont2> MyFrontIt2;
        Mycont2 c1;
        MyFrontIt2 bi1(c1);
        Movable_int mi1(3);
        *bi1 = STD move(mi1);
        CHECK_INT(mi1.val, -1);
        CHECK_INT(c1[0].val, 3);
    }

    InsIt::container_type* pii_cont = (Cont*) nullptr;
    InsIt::value_type* pii_val      = (char*) nullptr;
    InsIt iit(c0, c0.begin());

    pii_cont = pii_cont; // to quiet diagnostics
    pii_val  = pii_val;

    *iit   = 'e', ++iit;
    *iit++ = 'f';
    CHECK_INT(c0[0], 'e');
    CHECK_INT(c0[1], 'f');

    MyinsIt myinsit(c0, c0.begin());
    CHECK_PTR(myinsit.get_container(), &c0);
    CHECK(myinsit.get_iterator() == c0.begin());
    *STD inserter(c0, c0.begin())++ = 'z';
    CHECK_INT(c0[0], 'z');

    {
        typedef STD deque<Movable_int> Mycont2;
        typedef STD insert_iterator<Mycont2> MyBackIt2;
        Mycont2 c1;
        MyBackIt2 bi1(c1, c1.begin());
        Movable_int mi1(3);
        *bi1 = STD move(mi1);
        CHECK_INT(mi1.val, -1);
        CHECK_INT(c1[0].val, 3);
    }
}

void test_istreamit() { // test istream_iterator
    STD istringstream istr("0 1 2 3");
    typedef STD istream_iterator<int, char, Char_traits_char, CSTD ptrdiff_t> IstrIt;
    IstrIt::char_type* p_char       = (char*) nullptr;
    IstrIt::traits_type* p_traits   = (Char_traits_char*) nullptr;
    IstrIt::istream_type* p_istream = (STD istream*) nullptr;
    IstrIt iit0, iit(istr);
    int n;

    p_char    = p_char; // to quiet diagnostics
    p_traits  = p_traits;
    p_istream = p_istream;

    for (n = 0; n < 5 && iit != iit0; ++n) {
        CHECK_INT(*iit++, n);
    }
    CHECK(!(iit != iit0));
    CHECK_INT(n, 4);
}

void test_ostreamit() { // test ostream_iterator
    STD ostringstream ostr0, ostr;
    typedef STD ostream_iterator<int, char, Char_traits_char> OstrIt;
    OstrIt::value_type* p_val       = (int*) nullptr;
    OstrIt::char_type* p_char       = (char*) nullptr;
    OstrIt::traits_type* p_traits   = (Char_traits_char*) nullptr;
    OstrIt::ostream_type* p_ostream = (Char_ostream*) nullptr;
    OstrIt oit0(ostr0), oit(ostr, "||");

    p_val     = p_val; // to quiet diagnostics
    p_char    = p_char;
    p_traits  = p_traits;
    p_ostream = p_ostream;

    *oit0   = 1, ++oit0;
    *oit0++ = 2;
    CHECK_STR(ostr0.str().c_str(), "12");

    *oit   = 1, ++oit;
    *oit++ = 2;
    CHECK_STR(ostr.str().c_str(), "1||2||");
}

void test_istrbufit() { // test istreambuf_iterator
    STD istringstream istr("0123"), istr1("");
    typedef STD istreambuf_iterator<char, Char_traits_char> IsbIt;
    IsbIt::char_type* p_char           = (char*) nullptr;
    IsbIt::traits_type* p_traits       = (Char_traits_char*) nullptr;
    IsbIt::int_type* p_int             = (int*) nullptr;
    IsbIt::streambuf_type* p_streambuf = (Char_streambuf*) nullptr;
    IsbIt::istream_type* p_istream     = (Char_istream*) nullptr;
    IsbIt iit0, iit(istr), iit1(istr1.rdbuf());

    p_char      = p_char; // to quiet diagnostics
    p_traits    = p_traits;
    p_int       = p_int;
    p_streambuf = p_streambuf;
    p_istream   = p_istream;

    int n;
    for (n = 0; n < 5 && iit != iit0; ++n) {
        CHECK_INT(*iit++, n + '0');
    }
    CHECK(!(iit != iit0));
    CHECK_INT(n, 4);
    CHECK(iit0.equal(iit1));
}

void test_ostrbufit() { // test ostreambuf_iterator
    STD ostringstream ostr;
    typedef STD ostreambuf_iterator<char, Char_traits_char> OsbIt;
    OsbIt::char_type* p_char           = (char*) nullptr;
    OsbIt::traits_type* p_traits       = (Char_traits_char*) nullptr;
    OsbIt::streambuf_type* p_streambuf = (Char_streambuf*) nullptr;
    OsbIt::ostream_type* p_ostream     = (Char_ostream*) nullptr;
    OsbIt oit0((OsbIt::streambuf_type*) nullptr), oit(ostr);

    p_char      = p_char; // to quiet diagnostics
    p_traits    = p_traits;
    p_streambuf = p_streambuf;
    p_ostream   = p_ostream;

    *oit0++ = 'x';
    CHECK(oit0.failed());

    *oit   = '1', ++oit;
    *oit++ = '2';
    CHECK_STR(ostr.str().c_str(), "12");
    CHECK(!oit.failed());
}

void test_begin_end() { // test begin/end
    int arr[] = {1, 2, 3};
    CHECK_PTR(STD begin(arr) + 3, STD end(arr));
    CHECK_INT(*STD begin(arr), 1);

    CHECK_PTR(STD cbegin(arr) + 3, STD end(arr));
    CHECK_INT(*STD cbegin(arr), 1);

    STD deque<int> c0;
    c0.push_back(1);
    CHECK(STD begin(c0) + 1 == STD end(c0));
    CHECK_INT(*STD begin(c0), 1);

    CHECK(STD cbegin(c0) + 1 == STD cend(c0));
    CHECK_INT(*STD cbegin(c0), 1);
}

void test_size_data() { // test size/empty/data
    int arr[] = {1, 2, 3};
    CHECK_INT(STD size(arr), 3);
    CHECK(!STD empty(arr));
    CHECK_PTR(STD data(arr), &arr[0]);
    CHECK_INT(*STD begin(arr), 1);

    STD string c0("xyz");
    CHECK_INT(STD size(c0), 3);
    CHECK(!STD empty(c0));
    CHECK_PTR(STD data(c0), c0.data());

    STD initializer_list<char> ilist{c0[0], c0[1], c0[2]};
    CHECK_INT(STD size(ilist), 3);
    CHECK_PTR(STD data(ilist), ilist.begin());
}

void test_main() { // test basic properties of iterator definitions
    test_prop();
    test_revit();
    test_inserts();
    test_istreamit();
    test_ostreamit();
    test_istrbufit();
    test_ostrbufit();
    test_movit();
    test_begin_end();
    test_size_data();
}
