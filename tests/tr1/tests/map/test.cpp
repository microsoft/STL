// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <map>
#define TEST_NAME "<map>"

#define _HAS_AUTO_PTR_ETC 1

#include "tdefs.h"
#include <functional>
#include <map>
#include <stddef.h>

struct Wrapped_char { // wrap a char
    Wrapped_char(char ch = 0) : mych(ch) { // construct from char
    }

    operator char() const { // cast to char
        return mych;
    }

    char mych;
};

template <class _Ty>
struct Myless : public STD binary_function<_Ty, _Ty, bool> { // functor for operator<
    typedef int is_transparent;

    bool operator()(const _Ty& _Left, const _Ty& _Right) const { // apply operator< to operands
        return (char) _Left < (char) _Right;
    }
};

template <class Mypred>
void test_map() { // test map
    typedef STD pair<const char, int> Myval;
    typedef STD allocator<Myval> Myal;
    typedef STD map<char, int, Mypred, Myal> Mycont;

    Myval x, xarr[3], xarr2[3];
    for (int i = 0; i < 3; ++i) { // initialize arrays
        new (&xarr[i]) Myval((char) ('a' + i), 1 + i);
        new (&xarr2[i]) Myval((char) ('d' + i), 4 + i);
    }

    typename Mycont::key_type* p_key         = (char*) nullptr;
    typename Mycont::mapped_type* p_mapped   = (int*) nullptr;
    typename Mycont::key_compare* p_kcomp    = (Mypred*) nullptr;
    typename Mycont::allocator_type* p_alloc = (Myal*) nullptr;
    typename Mycont::value_type* p_val       = (Myval*) nullptr;
    typename Mycont::value_compare* p_vcomp  = nullptr;
    typename Mycont::pointer p_ptr           = (Myval*) nullptr;
    typename Mycont::const_pointer p_cptr    = (const Myval*) nullptr;
    typename Mycont::reference p_ref         = x;
    typename Mycont::const_reference p_cref  = (const Myval&) x;
    typename Mycont::size_type* p_size       = (CSTD size_t*) nullptr;
    typename Mycont::difference_type* p_diff = (CSTD ptrdiff_t*) nullptr;

    p_key    = p_key; // to quiet diagnostics
    p_mapped = p_mapped;
    p_kcomp  = p_kcomp;
    p_alloc  = p_alloc;
    p_val    = p_val;
    p_vcomp  = p_vcomp;
    p_ptr    = p_ptr;
    p_cptr   = p_cptr;
    p_ptr    = &p_ref;
    p_cptr   = &p_cref;
    p_size   = p_size;
    p_diff   = p_diff;

    Mycont v0;
    Myal al = v0.get_allocator();
    Mypred pred;
    Mycont v0a(pred), v0b(pred, al);
    CHECK(v0.empty());
    CHECK_INT(v0.size(), 0);
    CHECK_INT(v0a.size(), 0);
    CHECK(v0a.get_allocator() == al);
    CHECK_INT(v0b.size(), 0);
    CHECK(v0b.get_allocator() == al);

    Mycont v1(xarr, xarr + 3);
    CHECK_INT(v1.size(), 3);
    CHECK_INT((*v1.begin()).first, 'a');

    Mycont v2(xarr, xarr + 3, pred);
    CHECK_INT(v2.size(), 3);
    CHECK_INT((*v2.begin()).first, 'a');

    Mycont v3(xarr, xarr + 3, pred, al);
    CHECK_INT(v3.size(), 3);
    CHECK_INT((*v3.begin()).first, 'a');

    const Mycont v4(xarr, xarr + 3);
    CHECK_INT(v4.size(), 3);
    CHECK_INT((*v4.begin()).first, 'a');
    v0 = v4;
    CHECK_INT(v0.size(), 3);
    CHECK_INT((*v0.begin()).first, 'a');
    CHECK(v0.size() <= v0.max_size());

    STD map<char, int, Mypred>* p_cont = &v0;

    p_cont = p_cont; // to quiet diagnostics

    { // check iterator generators
        typename Mycont::iterator p_it(v1.begin());
        typename Mycont::const_iterator p_cit(v4.begin());
        typename Mycont::reverse_iterator p_rit(v1.rbegin());
        typename Mycont::const_reverse_iterator p_crit(v4.rbegin());
        CHECK_INT((*p_it).first, 'a');
        CHECK_INT((*p_it).second, 1);
        CHECK_INT((*--(p_it = v1.end())).first, 'c');
        CHECK_INT((*p_cit).first, 'a');
        CHECK_INT((*--(p_cit = v4.end())).first, 'c');
        CHECK_INT((*p_rit).first, 'c');
        CHECK_INT((*p_rit).second, 3);
        CHECK_INT((*--(p_rit = v1.rend())).first, 'a');
        CHECK_INT((*p_crit).first, 'c');
        CHECK_INT((*--(p_crit = v4.rend())).first, 'a');

        typename Mycont::const_iterator p_it1 = typename Mycont::const_iterator();
        typename Mycont::const_iterator p_it2 = typename Mycont::const_iterator();
        CHECK(p_it1 == p_it2); // check null forward iterator comparisons
    }

    { // check const iterators generators
        typename Mycont::const_iterator p_it(v1.cbegin());
        typename Mycont::const_iterator p_cit(v4.cbegin());
        typename Mycont::const_reverse_iterator p_rit(v1.crbegin());
        typename Mycont::const_reverse_iterator p_crit(v4.crbegin());
        CHECK_INT((*p_it).first, 'a');
        CHECK_INT((*p_it).second, 1);
        CHECK_INT((*--(p_it = v1.cend())).first, 'c');
        CHECK_INT((*p_cit).first, 'a');
        CHECK_INT((*--(p_cit = v4.cend())).first, 'c');
        CHECK_INT((*p_rit).first, 'c');
        CHECK_INT((*p_rit).second, 3);
        CHECK_INT((*--(p_rit = v1.crend())).first, 'a');
        CHECK_INT((*p_crit).first, 'c');
        CHECK_INT((*--(p_crit = v4.crend())).first, 'a');

        typename Mycont::const_iterator p_it1 = typename Mycont::const_iterator();
        typename Mycont::const_iterator p_it2 = typename Mycont::const_iterator();
        CHECK(p_it1 == p_it2); // check null forward iterator comparisons
    }

    v0.clear(); // differs from multimap
    STD pair<typename Mycont::iterator, bool> pib = v0.insert(Myval('d', 4));
    CHECK_INT((*pib.first).first, 'd');
    CHECK(pib.second);
    CHECK_INT((*--v0.end()).first, 'd');
    pib = v0.insert(Myval('d', 5));
    CHECK_INT((*pib.first).first, 'd');
    CHECK_INT((*pib.first).second, 4);
    CHECK(!pib.second);
    CHECK((*v0.insert(v0.begin(), Myval('e', 5))).first == 'e');
    v0.insert(xarr, xarr + 3);
    CHECK_INT(v0.size(), 5);
    CHECK_INT((*v0.begin()).first, 'a');
    v0.insert(xarr2, xarr2 + 3);
    CHECK_INT(v0.size(), 6);
    CHECK_INT((*--v0.end()).first, 'f');
    CHECK_INT(v0['c'], 3);
    v0.erase(v0.begin());
    CHECK_INT(v0.size(), 5);
    CHECK_INT((*v0.begin()).first, 'b');
    v0.erase(v0.begin(), ++v0.begin());
    CHECK_INT(v0.size(), 4);
    CHECK_INT((*v0.begin()).first, 'c');
    v0.insert(Myval('y', 99));
    CHECK_INT(v0.erase('x'), 0);
    CHECK_INT(v0.erase('y'), 1);

    { // test added C++11 functionality
        Mycont v0c(al);
        CHECK_INT(v0c.size(), 0);
        CHECK(v0c.get_allocator() == al);

        Mycont v1x(xarr, xarr + 3);
        CHECK_INT(v1x.at('c'), 3);
        Mycont v2x(v1x, al);
        CHECK_INT(v2x.at('c'), 3);
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

    CHECK(v0.key_comp()('a', 'c'));
    CHECK(!v0.key_comp()('a', 'a'));
    CHECK(v0.value_comp()(Myval('a', 0), Myval('c', 0)));
    CHECK(!v0.value_comp()(Myval('a', 0), Myval('a', 1)));
    CHECK_INT((*v4.find('a')).first, 'a');
    CHECK_INT(v4.count('x'), 0);
    CHECK_INT(v4.count('a'), 1);
    CHECK_INT((*v4.lower_bound('a')).first, 'a');
    CHECK((*v4.upper_bound('a')).first == 'b');

    STD pair<typename Mycont::const_iterator, typename Mycont::const_iterator> pcc = v4.equal_range('a');
    CHECK_INT((*pcc.first).first, 'a');
    CHECK_INT((*pcc.second).first, 'b');

    {
        Mycont v6;
        v6.insert(Myval('a', 1));
        v6.insert(Myval('b', 2));
        Mycont v7(STD move(v6));
        CHECK_INT(v6.size(), 0);
        CHECK_INT(v7.size(), 2);

        Mycont v6a;
        v6a.insert(Myval('a', 1));
        v6a.insert(Myval('b', 2));
        Mycont v7a(STD move(v6a), al);
        CHECK_INT(v6a.size(), 0);
        CHECK_INT(v7a.size(), 2);

        Mycont v8;
        v8 = STD move(v7);
        CHECK_INT(v7.size(), 0);
        CHECK_INT(v8.size(), 2);

        typedef STD map<Movable_int, int> Mycont2;
        Mycont2 v9;
        v9.insert(STD pair<Movable_int, int>(Movable_int('a'), 1));
        CHECK_INT(v9.size(), 1);
        CHECK_INT(v9.begin()->first.val, 'a');

        Mycont2 v10;
        STD pair<Movable_int, int> pmi1(Movable_int('e'), 5);
        v10.insert(STD move(pmi1));
        CHECK_INT(pmi1.first.val, -1);
        CHECK_INT(v10.begin()->first.val, 'e');

        STD pair<Movable_int, int> pmi2(Movable_int('d'), 4);
        v10.insert(v10.end(), STD move(pmi2));
        CHECK_INT(pmi2.first.val, -1);
        CHECK_INT(v10.begin()->first.val, 'd');

        Movable_int mi3('c');
        CHECK_INT(v10[STD move(mi3)], 0);
        CHECK_INT(mi3.val, -1);
        CHECK_INT(v10.begin()->first.val, 'c');

        v10.clear();
        v10.emplace_hint(v10.end());
        CHECK_INT(v10.begin()->first.val, 0);
        CHECK_INT(v10.begin()->second, 0);
        v10.clear();
        v10.emplace_hint(v10.end(), 'b', 2);
        CHECK_INT(v10.begin()->first.val, 'b');
        CHECK_INT(v10.begin()->second, 2);

        v10.clear();
        v10.emplace();
        CHECK_INT(v10.begin()->first.val, 0);
        CHECK_INT(v10.begin()->second, 0);

        v10.clear();
        v10.emplace('b', 2);
        CHECK_INT(v10.begin()->first.val, 'b');
        CHECK_INT(v10.begin()->second, 2);

        v8.clear(); // copyable key
        v8.try_emplace('b', 2);
        CHECK_INT(v8.begin()->first, 'b');
        CHECK_INT(v8.begin()->second, 2);

        v8.try_emplace('b', 3);
        CHECK_INT(v8.begin()->first, 'b');
        CHECK_INT(v8.begin()->second, 2);

        v8.insert_or_assign('b', 3);
        CHECK_INT(v8.begin()->first, 'b');
        CHECK_INT(v8.begin()->second, 3);

        v10.clear(); // movable key
        v10.try_emplace('b', 2);
        CHECK_INT(v10.begin()->first.val, 'b');
        CHECK_INT(v10.begin()->second, 2);

        v10.try_emplace('b', 3);
        CHECK_INT(v10.begin()->first.val, 'b');
        CHECK_INT(v10.begin()->second, 2);

        v10.insert_or_assign('b', 3);
        CHECK_INT(v10.begin()->first.val, 'b');
        CHECK_INT(v10.begin()->second, 3);
    }

    { // check for lvalue stealing
        typedef STD map<Copyable_int, int> Mycont3;
        Mycont3 v11;
        STD pair<Copyable_int, int> pci1(Copyable_int('d'), 4);
        v11.insert(pci1);
        CHECK_INT(pci1.first.val, 'd');
        CHECK_INT(v11.begin()->first.val, 'd');

        STD pair<Copyable_int, int> pci2(Copyable_int('c'), 3);
        v11.clear();
        v11.insert(v11.end(), pci2);
        CHECK_INT(pci2.first.val, 'c');
        CHECK_INT(v11.begin()->first.val, 'c');

        Mycont3 v12(v11);
        CHECK(v11 == v12);
    }

    {
        STD initializer_list<Myval> init{xarr[0], xarr[1], xarr[2]};
        Mycont v11(init);
        CHECK_INT(v11.size(), 3);
        CHECK_INT(v11.begin()->first, 'a');

        Mycont v11a(init, al);
        CHECK_INT(v11a.size(), 3);
        CHECK_INT(v11a.begin()->first, 'a');

        v11.clear();
        v11 = init;
        CHECK_INT(v11.size(), 3);
        CHECK_INT(v11.begin()->first, 'a');

        v11.clear();
        v11.insert(init);
        CHECK_INT(v11.size(), 3);
        CHECK_INT(v11.begin()->first, 'a');
    }
}

template <class Mypred>
void test_multimap() { // test multimap
    typedef STD pair<const char, int> Myval;
    typedef STD allocator<Myval> Myal;
    typedef STD multimap<char, int, Mypred, Myal> Mycont;

    Myval x, xarr[3], xarr2[3];
    for (int i = 0; i < 3; ++i) { // initialize arrays
        new (&xarr[i]) Myval((char) ('a' + i), 1 + i);
        new (&xarr2[i]) Myval((char) ('d' + i), 4 + i);
    }

    typename Mycont::key_type* p_key         = (char*) nullptr;
    typename Mycont::mapped_type* p_mapped   = (int*) nullptr;
    typename Mycont::key_compare* p_kcomp    = (Mypred*) nullptr;
    typename Mycont::allocator_type* p_alloc = (Myal*) nullptr;
    typename Mycont::value_type* p_val       = (Myval*) nullptr;
    typename Mycont::value_compare* p_vcomp  = nullptr;
    typename Mycont::pointer p_ptr           = (Myval*) nullptr;
    typename Mycont::const_pointer p_cptr    = (const Myval*) nullptr;
    typename Mycont::reference p_ref         = x;
    typename Mycont::const_reference p_cref  = (const Myval&) x;
    typename Mycont::size_type* p_size       = (CSTD size_t*) nullptr;
    typename Mycont::difference_type* p_diff = (CSTD ptrdiff_t*) nullptr;

    p_key    = p_key; // to quiet diagnostics
    p_mapped = p_mapped;
    p_kcomp  = p_kcomp;
    p_alloc  = p_alloc;
    p_val    = p_val;
    p_vcomp  = p_vcomp;
    p_ptr    = p_ptr;
    p_cptr   = p_cptr;
    p_ptr    = &p_ref;
    p_cptr   = &p_cref;
    p_size   = p_size;
    p_diff   = p_diff;

    Mycont v0;
    Myal al = v0.get_allocator();
    Mypred pred;
    Mycont v0a(pred), v0b(pred, al);
    CHECK(v0.empty());
    CHECK_INT(v0.size(), 0);
    CHECK_INT(v0a.size(), 0);
    CHECK(v0a.get_allocator() == al);
    CHECK_INT(v0b.size(), 0);
    CHECK(v0b.get_allocator() == al);

    Mycont v1(xarr, xarr + 3);
    CHECK_INT(v1.size(), 3);
    CHECK_INT((*v1.begin()).first, 'a');

    Mycont v2(xarr, xarr + 3, pred);
    CHECK_INT(v2.size(), 3);
    CHECK_INT((*v2.begin()).first, 'a');

    Mycont v3(xarr, xarr + 3, pred, al);
    CHECK_INT(v3.size(), 3);
    CHECK_INT((*v3.begin()).first, 'a');

    const Mycont v4(xarr, xarr + 3);
    CHECK_INT(v4.size(), 3);
    CHECK_INT((*v4.begin()).first, 'a');
    v0 = v4;
    CHECK_INT(v0.size(), 3);
    CHECK_INT((*v0.begin()).first, 'a');
    CHECK(v0.size() <= v0.max_size());

    STD multimap<char, int, Mypred>* p_cont = &v0;

    p_cont = p_cont; // to quiet diagnostics

    { // check iterator generators
        typename Mycont::iterator p_it(v1.begin());
        typename Mycont::const_iterator p_cit(v4.begin());
        typename Mycont::reverse_iterator p_rit(v1.rbegin());
        typename Mycont::const_reverse_iterator p_crit(v4.rbegin());
        CHECK_INT((*p_it).first, 'a');
        CHECK_INT((*p_it).second, 1);
        CHECK_INT((*--(p_it = v1.end())).first, 'c');
        CHECK_INT((*p_cit).first, 'a');
        CHECK_INT((*--(p_cit = v4.end())).first, 'c');
        CHECK_INT((*p_rit).first, 'c');
        CHECK_INT((*p_rit).second, 3);
        CHECK_INT((*--(p_rit = v1.rend())).first, 'a');
        CHECK_INT((*p_crit).first, 'c');
        CHECK_INT((*--(p_crit = v4.rend())).first, 'a');

        typename Mycont::const_iterator p_it1 = typename Mycont::const_iterator();
        typename Mycont::const_iterator p_it2 = typename Mycont::const_iterator();
        CHECK(p_it1 == p_it2); // check null forward iterator comparisons
    }

    { // check const iterators generators
        typename Mycont::const_iterator p_it(v1.cbegin());
        typename Mycont::const_iterator p_cit(v4.cbegin());
        typename Mycont::const_reverse_iterator p_rit(v1.crbegin());
        typename Mycont::const_reverse_iterator p_crit(v4.crbegin());
        CHECK_INT((*p_it).first, 'a');
        CHECK_INT((*p_it).second, 1);
        CHECK_INT((*--(p_it = v1.cend())).first, 'c');
        CHECK_INT((*p_cit).first, 'a');
        CHECK_INT((*--(p_cit = v4.cend())).first, 'c');
        CHECK_INT((*p_rit).first, 'c');
        CHECK_INT((*p_rit).second, 3);
        CHECK_INT((*--(p_rit = v1.crend())).first, 'a');
        CHECK_INT((*p_crit).first, 'c');
        CHECK_INT((*--(p_crit = v4.crend())).first, 'a');

        typename Mycont::const_iterator p_it1 = typename Mycont::const_iterator();
        typename Mycont::const_iterator p_it2 = typename Mycont::const_iterator();
        CHECK(p_it1 == p_it2); // check null forward iterator comparisons
    }

    v0.clear(); // differs from map
    CHECK_INT((*v0.insert(Myval('d', 4))).first, 'd');
    CHECK_INT((*--v0.end()).first, 'd');
    CHECK_INT((*v0.insert(Myval('d', 5))).first, 'd');
    CHECK_INT(v0.size(), 2);
    CHECK_INT((*v0.insert(v0.begin(), Myval('e', 5))).first, 'e');
    v0.insert(xarr, xarr + 3);
    CHECK_INT(v0.size(), 6);
    CHECK_INT((*v0.begin()).first, 'a');
    v0.insert(xarr2, xarr2 + 3);
    CHECK_INT(v0.size(), 9);
    CHECK_INT((*--v0.end()).first, 'f');
    v0.erase(v0.begin());
    CHECK_INT(v0.size(), 8);
    CHECK_INT((*v0.begin()).first, 'b');
    v0.erase(v0.begin(), ++v0.begin());
    CHECK_INT(v0.size(), 7);
    CHECK_INT((*v0.begin()).first, 'c');
    v0.insert(Myval('y', 98));
    v0.insert(Myval('y', 99));
    CHECK_INT(v0.erase('x'), 0);
    CHECK_INT(v0.erase('y'), 2);

    { // test added C++11 functionality
        Mycont v0c(al);
        CHECK_INT(v0c.size(), 0);
        CHECK(v0c.get_allocator() == al);
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

    CHECK(v0.key_comp()('a', 'c'));
    CHECK(!v0.key_comp()('a', 'a'));
    CHECK(v0.value_comp()(Myval('a', 0), Myval('c', 0)));
    CHECK(!v0.value_comp()(Myval('a', 0), Myval('a', 1)));
    CHECK_INT((*v4.find('a')).first, 'a');
    CHECK_INT(v4.count('x'), 0);
    CHECK_INT(v4.count('a'), 1);
    CHECK_INT((*v4.lower_bound('a')).first, 'a');
    CHECK((*v4.upper_bound('a')).first == 'b');

    STD pair<typename Mycont::const_iterator, typename Mycont::const_iterator> pcc = v4.equal_range('a');
    CHECK_INT((*pcc.first).first, 'a');
    CHECK_INT((*pcc.second).first, 'b');

    {
        Mycont v6;
        v6.insert(Myval('a', 1));
        v6.insert(Myval('b', 2));
        Mycont v7(STD move(v6));
        CHECK_INT(v6.size(), 0);
        CHECK_INT(v7.size(), 2);

        Mycont v6a;
        v6a.insert(Myval('a', 1));
        v6a.insert(Myval('b', 2));
        Mycont v7a(STD move(v6a), al);
        CHECK_INT(v6a.size(), 0);
        CHECK_INT(v7a.size(), 2);

        Mycont v8;
        v8 = STD move(v7);
        CHECK_INT(v7.size(), 0);
        CHECK_INT(v8.size(), 2);

        typedef STD multimap<Movable_int, int> Mycont2;
        Mycont2 v9;
        v9.insert(STD pair<Movable_int, int>(Movable_int('a'), 1));
        CHECK_INT(v9.size(), 1);
        CHECK_INT(v9.begin()->first.val, 'a');

        Mycont2 v10;
        STD pair<Movable_int, int> pmi1(Movable_int('d'), 4);
        v10.insert(STD move(pmi1));
        CHECK_INT(pmi1.first.val, -1);
        CHECK_INT(v10.begin()->first.val, 'd');

        STD pair<Movable_int, int> pmi2(Movable_int('c'), 3);
        v10.insert(v10.end(), STD move(pmi2));
        CHECK_INT(pmi2.first.val, -1);
        CHECK_INT(v10.begin()->first.val, 'c');

        v10.clear();
        v10.emplace_hint(v10.end());
        CHECK_INT(v10.begin()->first.val, 0);
        CHECK_INT(v10.begin()->second, 0);
        v10.clear();
        v10.emplace_hint(v10.end(), 'b', 2);
        CHECK_INT(v10.begin()->first.val, 'b');
        CHECK_INT(v10.begin()->second, 2);

        v10.clear();
        v10.emplace();
        CHECK_INT(v10.begin()->first.val, 0);
        CHECK_INT(v10.begin()->second, 0);

        v10.clear();
        v10.emplace('b', 2);
        CHECK_INT(v10.begin()->first.val, 'b');
        CHECK_INT(v10.begin()->second, 2);
    }

    { // check for lvalue stealing
        typedef STD multimap<Copyable_int, int> Mycont3;
        Mycont3 v11;
        STD pair<Copyable_int, int> pci1(Copyable_int('d'), 4);
        v11.insert(pci1);
        CHECK_INT(pci1.first.val, 'd');
        CHECK_INT(v11.begin()->first.val, 'd');

        STD pair<Copyable_int, int> pci2(Copyable_int('c'), 3);
        v11.clear();
        v11.insert(v11.end(), pci2);
        CHECK_INT(pci2.first.val, 'c');
        CHECK_INT(v11.begin()->first.val, 'c');

        Mycont3 v12(v11);
        CHECK(v11 == v12);
    }

    {
        STD initializer_list<Myval> init{xarr[0], xarr[1], xarr[2]};
        Mycont v11(init);
        CHECK_INT(v11.size(), 3);
        CHECK_INT(v11.begin()->first, 'a');

        Mycont v11a(init, al);
        CHECK_INT(v11a.size(), 3);
        CHECK_INT(v11a.begin()->first, 'a');

        v11.clear();
        v11 = init;
        CHECK_INT(v11.size(), 3);
        CHECK_INT(v11.begin()->first, 'a');

        v11.clear();
        v11.insert(init);
        CHECK_INT(v11.size(), 3);
        CHECK_INT(v11.begin()->first, 'a');
    }
}

void test_main() { // test basic workings of map definitions
    test_map<STD less<char>>();
    test_map<Myless<Wrapped_char>>();
    test_multimap<STD less<char>>();
    test_multimap<Myless<Wrapped_char>>();
}
