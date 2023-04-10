// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <forward_list>
#define TEST_NAME "<forward_list>"

#define _HAS_AUTO_PTR_ETC                1
#define _HAS_DEPRECATED_ADAPTOR_TYPEDEFS 1

#include "tdefs.h"
#include <forward_list>
#include <functional>
#include <stddef.h>

typedef STD allocator<char> Myal;
typedef STD forward_list<char, Myal> Mycont;

CSTD size_t size(const Mycont& flist) { // get size of list
    CSTD size_t ans           = 0;
    Mycont::const_iterator it = flist.begin();
    for (; it != flist.end(); ++it) {
        ++ans;
    }
    return ans;
}

Mycont::const_iterator before_end(const Mycont& flist) { // get iterator for last element of list
    Mycont::const_iterator it = flist.before_begin();
    Mycont::const_iterator it2;
    for (it2 = it; ++it2 != flist.end(); it = it2) {
        ;
    }
    return it;
}

Mycont::value_type back(const Mycont& flist) { // get last element of list
    return *before_end(flist);
}

void test_main() { // test basic workings of forward_list definitions
    char ch     = '\0';
    char carr[] = "abc";

    Mycont::allocator_type* p_alloc = (Myal*) nullptr;
    Mycont::pointer p_ptr           = (char*) nullptr;
    Mycont::const_pointer p_cptr    = (const char*) nullptr;
    Mycont::reference p_ref         = ch;
    Mycont::const_reference p_cref  = (const char&) ch;
    Mycont::size_type* p_size       = (CSTD size_t*) nullptr;
    Mycont::difference_type* p_diff = (CSTD ptrdiff_t*) nullptr;
    Mycont::value_type* p_val       = (char*) nullptr;

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
    CHECK_INT(size(v0), 0);
    CHECK_INT(size(v0a), 0);
    CHECK(v0a.get_allocator() == al);

    Mycont v1(5), v1a(6, 'x'), v1b(7, 'y', al);
    CHECK_INT(size(v1), 5);
    CHECK_INT(v1.front(), '\0');
    CHECK_INT(size(v1a), 6);
    CHECK_INT(v1a.front(), 'x');
    CHECK_INT(size(v1b), 7);
    CHECK_INT(v1b.front(), 'y');

    Mycont v2(v1a);
    CHECK_INT(size(v2), 6);
    CHECK_INT(v2.front(), 'x');

    Mycont v2a(v1a, al);
    CHECK_INT(size(v2a), 6);
    CHECK_INT(v2a.front(), 'x');

    Mycont v3(v1a.begin(), v1a.end());
    CHECK_INT(size(v3), 6);
    CHECK_INT(v3.front(), 'x');

    const Mycont v4(v1a.begin(), v1a.end(), al);
    CHECK_INT(size(v4), 6);
    CHECK_INT(v4.front(), 'x');
    v0 = v4;
    CHECK_INT(size(v0), 6);
    CHECK_INT(v0.front(), 'x');

    v0.resize(8);
    CHECK_INT(size(v0), 8);
    CHECK_INT(back(v0), '\0');
    v0.resize(10, 'z');
    CHECK_INT(size(v0), 10);
    CHECK_INT(back(v0), 'z');
    CHECK(size(v0) <= v0.max_size());

    STD forward_list<char>* p_cont = &v0;

    p_cont = p_cont; // to quiet diagnostics

    { // check iterator generators
        Mycont::iterator p_it(v0.begin());
        Mycont::const_iterator p_cit(v4.begin());
        CHECK_INT(*p_it, 'x');
        CHECK_INT(*p_cit, 'x');

        Mycont::const_iterator p_it1 = Mycont::const_iterator();
        Mycont::const_iterator p_it2 = Mycont::const_iterator();
        CHECK(p_it1 == p_it2); // check null forward iterator comparisons
    }

    Mycont::iterator p_bit(v0.before_begin());
    Mycont::const_iterator p_bcit(v0.before_begin());
    Mycont::const_iterator p_bccit(v0.cbefore_begin());
    CHECK_INT(*++p_bit, 'x');
    CHECK_INT(*++p_bcit, 'x');
    CHECK_INT(*++p_bccit, 'x');

    { // check const iterators generators
        Mycont::const_iterator p_it(v0.cbegin());
        Mycont::const_iterator p_cit(v4.cbegin());
        CHECK_INT(*p_it, 'x');
        CHECK_INT(*p_cit, 'x');

        Mycont::const_iterator p_it1 = Mycont::const_iterator();
        Mycont::const_iterator p_it2 = Mycont::const_iterator();
        CHECK(p_it1 == p_it2); // check null forward iterator comparisons
    }

    CHECK_INT(v0.front(), 'x');
    CHECK_INT(v4.front(), 'x');
    v0.push_front('a');
    CHECK_INT(v0.front(), 'a');
    v0.pop_front();
    CHECK_INT(v0.front(), 'x');
    CHECK_INT(v4.front(), 'x');

    {
        Mycont v5;
        v5.resize(10);
        CHECK_INT(size(v5), 10);
        CHECK_INT(*++v5.begin(), 0);

        Mycont v6(20, 'x');
        Mycont v7(STD move(v6));
        CHECK_INT(size(v6), 0);
        CHECK_INT(size(v7), 20);

        Mycont v8;
        v8 = STD move(v7);
        CHECK_INT(size(v7), 0);
        CHECK_INT(size(v8), 20);

        Mycont v8a(STD move(v8), Myal());
        CHECK_INT(size(v8), 0);
        CHECK_INT(size(v8a), 20);

        STD forward_list<Movable_int> v9;
        v9.resize(10);
        CHECK_INT((*++v9.begin()).val, 0);

        STD forward_list<Movable_int> v10;

        Movable_int mi2(2);
        v10.push_front(STD move(mi2));
        CHECK_INT(mi2.val, -1);
        CHECK_INT(v10.front().val, 2);

        Movable_int mi3(3);
        STD forward_list<Movable_int>::iterator it = v10.insert_after(v10.before_begin(), STD move(mi3));
        CHECK(it == v10.begin());
        CHECK_INT(mi3.val, -1);
        CHECK_INT(v10.front().val, 3);

        v10.emplace_front();
        CHECK_INT(v10.front().val, 0);
        v10.emplace_front(2);
        CHECK_INT(v10.front().val, 2);
        v10.emplace_front(3, 2);
        CHECK_INT(v10.front().val, 0x32);
        v10.emplace_front(4, 3, 2);
        CHECK_INT(v10.front().val, 0x432);
        v10.emplace_front(5, 4, 3, 2);
        CHECK_INT(v10.front().val, 0x5432);
        v10.emplace_front(6, 5, 4, 3, 2);
        CHECK_INT(v10.front().val, 0x65432);

        v10.emplace_after(v10.begin());
        CHECK_INT((*++v10.begin()).val, 0);
        v10.emplace_after(v10.begin(), 2);
        CHECK_INT((*++v10.begin()).val, 2);
        v10.emplace_after(v10.begin(), 3, 2);
        CHECK_INT((*++v10.begin()).val, 0x32);
        v10.emplace_after(v10.begin(), 4, 3, 2);
        CHECK_INT((*++v10.begin()).val, 0x432);
        v10.emplace_after(v10.begin(), 5, 4, 3, 2);
        CHECK_INT((*++v10.begin()).val, 0x5432);
        v10.emplace_after(v10.begin(), 6, 5, 4, 3, 2);
        CHECK_INT((*++v10.begin()).val, 0x65432);
    }

    { // check for lvalue stealing
        STD forward_list<Copyable_int> v11;

        Copyable_int ci2(2);
        v11.push_front(ci2);
        CHECK_INT(ci2.val, 2);
        CHECK_INT(v11.front().val, 2);

        Copyable_int ci3(3);
        STD forward_list<Copyable_int>::iterator it = v11.insert_after(v11.before_begin(), ci3);
        CHECK(it == v11.begin());
        CHECK_INT(ci3.val, 3);
        CHECK_INT(v11.front().val, 3);

        STD forward_list<Copyable_int> v12(v11);
        CHECK(v11 == v12);
        v11 = v12;
        CHECK(v11 == v12);

        STD forward_list<Copyable_int> v13(STD make_move_iterator(v11.begin()), STD make_move_iterator(v11.end()));
        CHECK_INT(v13.front().val, 3);
        CHECK_INT(v11.front().val, -1);
    }

    v0.assign(v4.begin(), v4.end());
    CHECK_INT(size(v0), size(v4));
    CHECK_INT(v0.front(), v4.front());
    v0.assign(4, 'w');
    CHECK_INT(size(v0), 4);
    CHECK_INT(v0.front(), 'w');
    CHECK_INT(*v0.insert_after(v0.before_begin(), 'a'), 'a');
    CHECK_INT(v0.front(), 'a');
    CHECK_INT(*++v0.begin(), 'w');
    Mycont::iterator it = v0.insert_after(v0.before_begin(), 2, 'b');
    CHECK(it == ++v0.begin());
    CHECK_INT(v0.front(), 'b');
    CHECK_INT(*++v0.begin(), 'b');
    CHECK_INT(*++ ++v0.begin(), 'a');
    it = v0.insert_after(before_end(v0), v4.begin(), v4.end());
    CHECK(++it == v0.end());
    CHECK_INT(back(v0), back(v4));
    it = v0.insert_after(before_end(v0), carr, carr + 3);
    CHECK(++it == v0.end());
    CHECK_INT(back(v0), 'c');
    v0.erase_after(v0.before_begin());
    CHECK_INT(v0.front(), 'b');
    CHECK_INT(*++v0.begin(), 'a');
    v0.erase_after(v0.before_begin(), ++v0.begin());
    CHECK_INT(v0.front(), 'a');

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

    v0.insert_after(v0.before_begin(), carr, carr + 3);
    v1.splice_after(v1.before_begin(), v0);
    CHECK(v0.empty());
    CHECK_INT(v1.front(), 'a');
    v0.splice_after(before_end(v0), v1, v1.before_begin());
    CHECK_INT(size(v0), 1);
    CHECK_INT(v0.front(), 'a');
    v0.splice_after(v0.before_begin(), v1, v1.before_begin(), v1.end());
    CHECK_INT(v0.front(), 'b');
    CHECK(v1.empty());
    v0.remove('b');
    CHECK_INT(v0.front(), 'c');
    v0.remove_if(STD binder2nd<STD not_equal_to<char>>(STD not_equal_to<char>(), 'c'));
    CHECK_INT(v0.front(), 'c');
    CHECK_INT(size(v0), 1);

    v0.merge(v1, STD greater<char>());
    CHECK_INT(v0.front(), 'c');
    CHECK_INT(size(v0), 1);
    v0.insert_after(v0.before_begin(), carr, carr + 3);
    v0.unique();
    CHECK_INT(back(v0), 'c');
    CHECK_INT(size(v0), 3);
    v0.unique(STD not_equal_to<char>());
    CHECK_INT(v0.front(), 'a');
    CHECK_INT(size(v0), 1);
    v1.insert_after(v1.before_begin(), carr, carr + 3);
    v0.merge(v1);
    CHECK_INT(back(v0), 'c');
    CHECK_INT(size(v0), 4);
    v0.sort(STD greater<char>());
    CHECK_INT(back(v0), 'a');
    CHECK_INT(size(v0), 4);
    v0.sort();
    CHECK_INT(back(v0), 'c');
    CHECK_INT(size(v0), 4);
    v0.reverse();
    CHECK_INT(back(v0), 'a');
    CHECK_INT(size(v0), 4);

    v0.clear();
    v1.clear();
    v0.insert_after(v0.before_begin(), carr, carr + 3);
    v1.splice_after(v1.before_begin(), STD move(v0));
    CHECK(v0.empty());
    CHECK_INT(v1.front(), 'a');
    v0.splice_after(before_end(v0), STD move(v1), v1.before_begin());
    CHECK_INT(size(v0), 1);
    CHECK_INT(v0.front(), 'a');
    v0.splice_after(v0.before_begin(), STD move(v1), v1.before_begin(), v1.end());
    CHECK_INT(v0.front(), 'b');
    CHECK(v1.empty());

    v0.assign(1, 'c');
    v1.assign(2, 'b');
    v0.merge(STD move(v1), STD greater<char>());
    CHECK_INT(v0.front(), 'c');

    v0.assign(1, 'c');
    v1.assign(2, 'b');
    v0.merge(STD move(v1));
    CHECK_INT(v0.front(), 'b');

    {
        STD initializer_list<char> init{'a', 'b', 'c'};
        Mycont v11(init);
        CHECK_INT(size(v11), 3);
        CHECK_INT(v11.front(), 'a');

        v11.clear();
        v11 = init;
        CHECK_INT(size(v11), 3);
        CHECK_INT(v11.front(), 'a');

        Mycont::iterator it0 = v11.insert_after(v11.begin(), init);
        CHECK_INT(*it0, 'c');
        CHECK_INT(size(v11), 6);
        CHECK_INT(*++v11.begin(), 'a');

        v11.assign(init);
        CHECK_INT(size(v11), 3);
        CHECK_INT(v11.front(), 'a');
    }
}
