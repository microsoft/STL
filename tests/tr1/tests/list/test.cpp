// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <list>
#define TEST_NAME "<list>"

#define _HAS_AUTO_PTR_ETC                1
#define _HAS_DEPRECATED_ADAPTOR_TYPEDEFS 1

#include "tdefs.h"
#include <functional>
#include <list>
#include <stddef.h>

typedef STD allocator<char> Myal;
typedef STD list<char, Myal> Mycont;

void test_main() { // test basic workings of list definitions
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
    CHECK_INT(v0.size(), 0);
    CHECK_INT(v0a.size(), 0);
    CHECK(v0a.get_allocator() == al);

    Mycont v1(5), v1a(6, 'x'), v1b(7, 'y', al);
    CHECK_INT(v1.size(), 5);
    CHECK_INT(v1.back(), '\0');
    CHECK_INT(v1a.size(), 6);
    CHECK_INT(v1a.back(), 'x');
    CHECK_INT(v1b.size(), 7);
    CHECK_INT(v1b.back(), 'y');

    Mycont v2(v1a);
    CHECK_INT(v2.size(), 6);
    CHECK_INT(v2.front(), 'x');

    Mycont v2a(v1a, al);
    CHECK_INT(v2.size(), 6);
    CHECK_INT(v2a.front(), 'x');

    Mycont v3(v1a.begin(), v1a.end());
    CHECK_INT(v3.size(), 6);
    CHECK_INT(v3.front(), 'x');

    const Mycont v4(v1a.begin(), v1a.end(), al);
    CHECK_INT(v4.size(), 6);
    CHECK_INT(v4.front(), 'x');
    v0 = v4;
    CHECK_INT(v0.size(), 6);
    CHECK_INT(v0.front(), 'x');

    v0.resize(8);
    CHECK_INT(v0.size(), 8);
    CHECK_INT(v0.back(), '\0');
    v0.resize(10, 'z');
    CHECK_INT(v0.size(), 10);
    CHECK_INT(v0.back(), 'z');
    CHECK(v0.size() <= v0.max_size());

    STD list<char>* p_cont = &v0;
    p_cont                 = p_cont; // to quiet diagnostics

    { // check iterator generators
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

        Mycont::const_iterator p_it1 = Mycont::const_iterator();
        Mycont::const_iterator p_it2 = Mycont::const_iterator();
        CHECK(p_it1 == p_it2); // check null forward iterator comparisons
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

    v0.push_back('a');
    CHECK_INT(v0.back(), 'a');
    v0.pop_back();
    CHECK_INT(v0.back(), 'z');
    CHECK_INT(v4.back(), 'x');

    {
        Mycont v5;
        v5.resize(10);
        CHECK_INT(v5.size(), 10);
        CHECK_INT(*++v5.begin(), 0);

        Mycont v6(20, 'x');
        Mycont v7(STD move(v6));
        CHECK_INT(v6.size(), 0);
        CHECK_INT(v7.size(), 20);

        Mycont v8;
        v8 = STD move(v7);
        CHECK_INT(v7.size(), 0);
        CHECK_INT(v8.size(), 20);

        Mycont v8a(STD move(v8), Myal());
        CHECK_INT(v8.size(), 0);
        CHECK_INT(v8a.size(), 20);

        STD list<Movable_int> v9;
        v9.resize(10);
        CHECK_INT(v9.size(), 10);
        CHECK_INT((*++v9.begin()).val, 0);

        STD list<Movable_int> v10;
        Movable_int mi1(1);
        v10.push_back(STD move(mi1));
        CHECK_INT(mi1.val, -1);
        CHECK_INT(v10.front().val, 1);

        Movable_int mi2(2);
        v10.push_front(STD move(mi2));
        CHECK_INT(mi2.val, -1);
        CHECK_INT(v10.front().val, 2);

        Movable_int mi3(3);
        v10.insert(v10.begin(), STD move(mi3));
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

        v10.emplace_back();
        CHECK_INT(v10.back().val, 0);
        v10.emplace_back(2);
        CHECK_INT(v10.back().val, 2);
        v10.emplace_back(3, 2);
        CHECK_INT(v10.back().val, 0x32);
        v10.emplace_back(4, 3, 2);
        CHECK_INT(v10.back().val, 0x432);
        v10.emplace_back(5, 4, 3, 2);
        CHECK_INT(v10.back().val, 0x5432);
        v10.emplace_back(6, 5, 4, 3, 2);
        CHECK_INT(v10.back().val, 0x65432);

        v10.emplace(++v10.begin());
        CHECK_INT((*++v10.begin()).val, 0);
        v10.emplace(++v10.begin(), 2);
        CHECK_INT((*++v10.begin()).val, 2);
        v10.emplace(++v10.begin(), 3, 2);
        CHECK_INT((*++v10.begin()).val, 0x32);
        v10.emplace(++v10.begin(), 4, 3, 2);
        CHECK_INT((*++v10.begin()).val, 0x432);
        v10.emplace(++v10.begin(), 5, 4, 3, 2);
        CHECK_INT((*++v10.begin()).val, 0x5432);
        v10.emplace(++v10.begin(), 6, 5, 4, 3, 2);
        CHECK_INT((*++v10.begin()).val, 0x65432);
    }

    { // check for lvalue stealing
        STD list<Copyable_int> v11;
        Copyable_int ci1(1);
        v11.push_back(ci1);
        CHECK_INT(ci1.val, 1);
        CHECK_INT(v11.back().val, 1);

        Copyable_int ci2(2);
        v11.push_front(ci2);
        CHECK_INT(ci2.val, 2);
        CHECK_INT(v11.front().val, 2);

        Copyable_int ci3(3);
        v11.insert(v11.begin(), ci3);
        CHECK_INT(ci3.val, 3);
        CHECK_INT(v11.front().val, 3);
        CHECK_INT(v11.back().val, 1);

        STD list<Copyable_int> v12(v11);
        CHECK(v11 == v12);
        v11 = v12;
        CHECK(v11 == v12);

        STD list<Copyable_int> v13(STD make_move_iterator(v11.begin()), STD make_move_iterator(v11.end()));
        CHECK_INT(v13.front().val, 3);
        CHECK_INT(v11.front().val, -1);
    }

    v0.assign(v4.begin(), v4.end());
    CHECK_INT(v0.size(), v4.size());
    CHECK_INT(v0.front(), v4.front());
    v0.assign(4, 'w');
    CHECK_INT(v0.size(), 4);
    CHECK_INT(v0.front(), 'w');
    CHECK_INT(*v0.insert(v0.begin(), 'a'), 'a');
    CHECK_INT(v0.front(), 'a');
    CHECK_INT(*++v0.begin(), 'w');
    CHECK_INT(*v0.insert(v0.begin(), 2, 'b'), 'b');
    CHECK_INT(v0.front(), 'b');
    CHECK_INT(*++v0.begin(), 'b');
    CHECK_INT(*++ ++v0.begin(), 'a');
    CHECK_INT(*v0.insert(v0.end(), v4.begin(), v4.end()), *v4.begin());
    CHECK_INT(v0.back(), v4.back());
    CHECK_INT(*v0.insert(v0.end(), carr, carr + 3), *carr);
    CHECK_INT(v0.back(), 'c');
    v0.erase(v0.begin());
    CHECK_INT(v0.front(), 'b');
    CHECK_INT(*++v0.begin(), 'a');
    v0.erase(v0.begin(), ++v0.begin());
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

    v0.insert(v0.begin(), carr, carr + 3);
    v1.splice(v1.begin(), v0);
    CHECK(v0.empty());
    CHECK_INT(v1.front(), 'a');
    v0.splice(v0.end(), v1, v1.begin());
    CHECK_INT(v0.size(), 1);
    CHECK_INT(v0.front(), 'a');
    v0.splice(v0.begin(), v1, v1.begin(), v1.end());
    CHECK_INT(v0.front(), 'b');
    CHECK(v1.empty());
    v0.remove('b');
    CHECK_INT(v0.front(), 'c');
    v0.remove_if(STD binder2nd<STD not_equal_to<char>>(STD not_equal_to<char>(), 'c'));
    CHECK_INT(v0.front(), 'c');
    CHECK_INT(v0.size(), 1);

    v0.merge(v1, STD greater<char>());
    CHECK_INT(v0.front(), 'c');
    CHECK_INT(v0.size(), 1);
    v0.insert(v0.begin(), carr, carr + 3);
    v0.unique();
    CHECK_INT(v0.back(), 'c');
    CHECK_INT(v0.size(), 3);
    v0.unique(STD not_equal_to<char>());
    CHECK_INT(v0.front(), 'a');
    CHECK_INT(v0.size(), 1);
    v1.insert(v1.begin(), carr, carr + 3);
    v0.merge(v1);
    CHECK_INT(v0.back(), 'c');
    CHECK_INT(v0.size(), 4);
    v0.sort(STD greater<char>());
    CHECK_INT(v0.back(), 'a');
    CHECK_INT(v0.size(), 4);
    v0.sort();
    CHECK_INT(v0.back(), 'c');
    CHECK_INT(v0.size(), 4);
    v0.reverse();
    CHECK_INT(v0.back(), 'a');
    CHECK_INT(v0.size(), 4);

    v0.clear();
    v1.clear();
    v0.insert(v0.begin(), carr, carr + 3);
    v1.splice(v1.begin(), STD move(v0));
    CHECK(v0.empty());
    CHECK_INT(v1.front(), 'a');
    v0.splice(v0.end(), STD move(v1), v1.begin());
    CHECK_INT(v0.size(), 1);
    CHECK_INT(v0.front(), 'a');
    v0.splice(v0.begin(), STD move(v1), v1.begin(), v1.end());
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
        CHECK_INT(v11.size(), 3);
        CHECK_INT(v11.back(), 'c');

        v11.clear();
        v11 = init;
        CHECK_INT(v11.size(), 3);
        CHECK_INT(v11.back(), 'c');

        CHECK_INT(*v11.insert(++v11.begin(), init), *init.begin());
        CHECK_INT(v11.size(), 6);
        CHECK_INT(*++v11.begin(), 'a');

        v11.assign(init);
        CHECK_INT(v11.size(), 3);
        CHECK_INT(v11.back(), 'c');
    }
}
