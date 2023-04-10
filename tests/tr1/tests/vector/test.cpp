// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <vector>
#define TEST_NAME "<vector>"

#include "tdefs.h"
#include <stddef.h>
#include <vector>

typedef STD allocator<char> Myal;
typedef STD vector<char, Myal> Mycont;

void test_main() { // test basic workings of vector definitions
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

    Mycont v2a(v2, al);
    CHECK_INT(v2a.size(), 6);
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
    CHECK_INT(v0[0], 'x');
    CHECK_INT(v0.at(5), 'x');

    v0.reserve(12);
    CHECK(12 <= v0.capacity());
    v0.resize(8);
    CHECK_INT(v0.size(), 8);
    CHECK_INT(v0.back(), '\0');
    v0.resize(10, 'z');
    CHECK_INT(v0.size(), 10);
    CHECK_INT(v0.back(), 'z');
    CHECK(v0.size() <= v0.max_size());

    STD vector<char>* p_cont = &v0;
    p_cont                   = p_cont; // to quiet diagnostics

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

    v0.push_back('a');
    CHECK_INT(v0.back(), 'a');
    v0.pop_back();
    CHECK_INT(v0.back(), 'z');
    CHECK_INT(v4.back(), 'x');

    {
        Mycont v5;
        v5.resize(10);
        CHECK_INT(v5.size(), 10);
        CHECK_INT(v5[9], 0);

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

        STD vector<Movable_int> v9;
        v9.resize(10);
        CHECK_INT(v9.size(), 10);
        CHECK_INT(v9[9].val, 0);

        STD vector<Movable_int> v10;
        Movable_int mi1(1);
        v10.push_back(STD move(mi1));
        CHECK_INT(mi1.val, -1);
        CHECK_INT(v10[0].val, 1);

        Movable_int mi3(3);
        v10.insert(v10.begin(), STD move(mi3));
        CHECK_INT(mi3.val, -1);
        CHECK_INT(v10[0].val, 3);
        CHECK_INT(v10[1].val, 1);

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

        v10.emplace(v10.begin() + 1);
        CHECK_INT(v10[1].val, 0);
        v10.emplace(v10.begin() + 1, 2);
        CHECK_INT(v10[1].val, 2);
        v10.emplace(v10.begin() + 1, 3, 2);
        CHECK_INT(v10[1].val, 0x32);
        v10.emplace(v10.begin() + 1, 4, 3, 2);
        CHECK_INT(v10[1].val, 0x432);
        v10.emplace(v10.begin() + 1, 5, 4, 3, 2);
        CHECK_INT(v10[1].val, 0x5432);
        v10.emplace(v10.begin() + 1, 6, 5, 4, 3, 2);
        CHECK_INT(v10[1].val, 0x65432);
    }

    { // check for lvalue stealing
        STD vector<Copyable_int> v11;
        Copyable_int ci1(1);
        v11.push_back(ci1);
        CHECK_INT(ci1.val, 1);
        CHECK_INT(v11[0].val, 1);

        Copyable_int ci3(3);
        v11.insert(v11.begin(), ci3);
        CHECK_INT(ci3.val, 3);
        CHECK_INT(v11[0].val, 3);
        CHECK_INT(v11[1].val, 1);

        STD vector<Copyable_int> v12(v11);
        CHECK(v11 == v12);
        v11 = v12;
        CHECK(v11 == v12);
    }

    { // check front/back
        Mycont::iterator p_it;
        v0.assign(v4.begin(), v4.end());
        CHECK_INT(v0.size(), v4.size());
        CHECK_INT(v0.front(), v4.front());
        v0.assign(4, 'w');
        CHECK_INT(v0.size(), 4);
        CHECK_INT(v0.front(), 'w');
        CHECK_INT(*v0.insert(v0.begin(), 'a'), 'a');
        CHECK_INT(v0.front(), 'a');
        CHECK_INT(*++(p_it = v0.begin()), 'w');
        CHECK_INT(*v0.insert(v0.begin(), 2, 'b'), 'b');
        CHECK_INT(v0.front(), 'b');
        CHECK_INT(*++(p_it = v0.begin()), 'b');
        CHECK_INT(*++ ++(p_it = v0.begin()), 'a');
        CHECK_INT(*v0.insert(v0.end(), v4.begin(), v4.end()), *v4.begin());
        CHECK_INT(v0.back(), v4.back());
        CHECK_INT(*v0.insert(v0.end(), carr, carr + 3), *carr);
        CHECK_INT(v0.back(), 'c');
        v0.erase(v0.begin());
        CHECK_INT(v0.front(), 'b');
        CHECK_INT(*++(p_it = v0.begin()), 'a');
        v0.erase(v0.begin(), ++(p_it = v0.begin()));
        CHECK_INT(v0.front(), 'a');
    }

    { // test added C++11 functionality
        Mycont v0x;

        char* pd = v0x.data();
        CHECK_PTR(pd, nullptr);
        v0x.push_back('a');
        CHECK_INT(*v0x.data(), 'a');

        v0x.shrink_to_fit();
        CHECK_INT(*v0x.data(), 'a');
    }

    {
        STD initializer_list<char> init{'a', 'b', 'c'};
        Mycont v11(init);
        CHECK_INT(v11.size(), 3);
        CHECK_INT(v11[2], 'c');

        v11.clear();
        v11 = init;
        CHECK_INT(v11.size(), 3);
        CHECK_INT(v11[2], 'c');

        v11.insert(v11.begin() + 1, init);
        CHECK_INT(v11.size(), 6);
        CHECK_INT(v11[2], 'b');

        v11.assign(init);
        CHECK_INT(v11.size(), 3);
        CHECK_INT(v11[2], 'c');
    }

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

    { // test vector<bool>
        typedef STD vector<bool, STD allocator<bool>> Bvector;

        Bvector bv(3);
        bv[0] = !bv[1];
        bv.flip();
        CHECK(!bv[0]);
        CHECK(bv[1]);
        CHECK(bv[2]);

        Bvector::swap(bv[0], bv[1]);
        CHECK(bv[0]);
        CHECK(!bv[1]);

        Bvector::reference bref = bv[0];
        bref.flip();
        CHECK(!bv[0]);
        bref = bv.back();
        CHECK(bv[0]);

        Bvector::iterator bit = bv.end();
        bref                  = !bit[-1];
        CHECK(!bv[0]);

        Bvector* p_bv = &bv;
        p_bv          = p_bv; // to quiet diagnostics

        {
            Bvector bv1(3);
            Bvector bv2(STD move(bv1));
            CHECK_INT(bv1.size(), 0);
            CHECK_INT(bv2.size(), 3);

            Bvector bv3;
            bv3 = STD move(bv2);
            CHECK_INT(bv2.size(), 0);
            CHECK_INT(bv3.size(), 3);
        }

        {
            Bvector bv1(3);
            bv1[1].flip();
            bv1.shrink_to_fit();
            CHECK(bv1[1]);

            bv1.emplace_back();
            CHECK(!bv1[3]);
            bv1.emplace_back(true);
            CHECK(bv1[4]);
            bv1.emplace_back(false);
            CHECK(!bv1[5]);

            bv1.emplace(bv1.begin(), true);
            CHECK(bv1[0]);
            bv1.emplace(bv1.begin(), false);
            CHECK(!bv1[0]);
            bv1.emplace(bv1.begin());
            CHECK(!bv1[0]);

            STD hash<Bvector> hasher;
            CHECK_INT(hasher(bv1), hasher(bv1));
        }

        {
            STD initializer_list<bool> init{false, true, false};
            Bvector bv11(init);
            CHECK_INT(bv11.size(), 3);
            CHECK_INT(bv11[2], false);

            bv11.clear();
            bv11 = init;
            CHECK_INT(bv11.size(), 3);
            CHECK_INT(bv11[2], false);

            CHECK_INT(*bv11.insert(bv11.begin() + 1, init), false);
            CHECK_INT(bv11.size(), 6);
            CHECK_INT(bv11[2], true);

            bv11.assign(init);
            CHECK_INT(bv11.size(), 3);
            CHECK_INT(bv11[2], false);
        }
    }
}
