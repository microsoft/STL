// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <set>
#define TEST_NAME "<set>"

#define _HAS_AUTO_PTR_ETC 1

#include "tdefs.h"
#include <functional>
#include <set>
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
void test_set() { // test set
    typedef STD allocator<char> Myal;
    typedef STD set<char, Mypred, Myal> Mycont;
    char ch = '\0', carr[] = "abc", carr2[] = "def";

    typename Mycont::key_type* p_key         = (char*) nullptr;
    typename Mycont::key_compare* p_kcomp    = (Mypred*) nullptr;
    typename Mycont::allocator_type* p_alloc = (Myal*) nullptr;
    typename Mycont::value_type* p_val       = (char*) nullptr;
    typename Mycont::value_compare* p_vcomp  = (Mypred*) nullptr;
    typename Mycont::pointer p_ptr           = (char*) nullptr;
    typename Mycont::const_pointer p_cptr    = (const char*) nullptr;
    typename Mycont::reference p_ref         = ch;
    typename Mycont::const_reference p_cref  = (const char&) ch;
    typename Mycont::size_type* p_size       = (CSTD size_t*) nullptr;
    typename Mycont::difference_type* p_diff = (CSTD ptrdiff_t*) nullptr;

    p_key   = p_key; // to quiet diagnostics
    p_kcomp = p_kcomp;
    p_alloc = p_alloc;
    p_val   = p_val;
    p_vcomp = p_vcomp;
    p_ptr   = p_ptr;
    p_cptr  = p_cptr;
    p_ptr   = &p_ref;
    p_cptr  = &p_cref;
    p_size  = p_size;
    p_diff  = p_diff;

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

    Mycont v1(carr, carr + 3); // differs from hash_set
    CHECK_INT(v1.size(), 3);
    CHECK_INT(*v1.begin(), 'a');

    Mycont v2(carr, carr + 3, pred);
    CHECK_INT(v2.size(), 3);
    CHECK_INT(*v2.begin(), 'a');

    Mycont v3(carr, carr + 3, pred, al);
    CHECK_INT(v3.size(), 3);
    CHECK_INT(*v3.begin(), 'a');

    const Mycont v4(carr, carr + 3);
    CHECK_INT(v4.size(), 3);
    CHECK_INT(*v4.begin(), 'a');
    v0 = v4;
    CHECK_INT(v0.size(), 3);
    CHECK_INT(*v0.begin(), 'a');
    CHECK(v0.size() <= v0.max_size());

    STD set<char, Mypred>* p_cont = &v0;

    p_cont = p_cont; // to quiet diagnostics

    { // check iterator generators
        typename Mycont::iterator p_it(v1.begin());
        typename Mycont::const_iterator p_cit(v4.begin());
        typename Mycont::reverse_iterator p_rit(v1.rbegin());
        typename Mycont::const_reverse_iterator p_crit(v4.rbegin());
        CHECK_INT(*p_it, 'a');
        CHECK_INT(*--(p_it = v1.end()), 'c');
        CHECK_INT(*p_cit, 'a');
        CHECK_INT(*--(p_cit = v4.end()), 'c');
        CHECK_INT(*p_rit, 'c');
        CHECK_INT(*--(p_rit = v1.rend()), 'a');
        CHECK_INT(*p_crit, 'c');
        CHECK_INT(*--(p_crit = v4.rend()), 'a');

        typename Mycont::const_iterator p_it1 = typename Mycont::const_iterator();
        typename Mycont::const_iterator p_it2 = typename Mycont::const_iterator();
        CHECK(p_it1 == p_it2); // check null forward iterator comparisons
    }

    { // check const iterators generators
        typename Mycont::const_iterator p_it(v1.cbegin());
        typename Mycont::const_iterator p_cit(v4.cbegin());
        typename Mycont::const_reverse_iterator p_rit(v1.crbegin());
        typename Mycont::const_reverse_iterator p_crit(v4.crbegin());
        CHECK_INT(*p_it, 'a');
        CHECK_INT(*--(p_it = v1.cend()), 'c');
        CHECK_INT(*p_cit, 'a');
        CHECK_INT(*--(p_cit = v4.cend()), 'c');
        CHECK_INT(*p_rit, 'c');
        CHECK_INT(*--(p_rit = v1.crend()), 'a');
        CHECK_INT(*p_crit, 'c');
        CHECK_INT(*--(p_crit = v4.crend()), 'a');

        typename Mycont::const_iterator p_it1 = typename Mycont::const_iterator();
        typename Mycont::const_iterator p_it2 = typename Mycont::const_iterator();
        CHECK(p_it1 == p_it2); // check null forward iterator comparisons
    }

    v0.clear(); // differs from multiset
    STD pair<typename Mycont::iterator, bool> pib = v0.insert('d');
    CHECK_INT(*pib.first, 'd');
    CHECK(pib.second);
    CHECK_INT(*--v0.end(), 'd');
    pib = v0.insert('d');
    CHECK_INT(*pib.first, 'd');
    CHECK(!pib.second);
    CHECK_INT(*v0.insert(v0.begin(), 'e'), 'e');
    v0.insert(carr, carr + 3);
    CHECK_INT(v0.size(), 5);
    CHECK_INT(*v0.begin(), 'a');
    v0.insert(carr2, carr2 + 3);
    CHECK_INT(v0.size(), 6);
    CHECK_INT(*--v0.end(), 'f');
    v0.erase(v0.begin());
    CHECK_INT(v0.size(), 5);
    CHECK_INT(*v0.begin(), 'b');
    v0.erase(v0.begin(), ++v0.begin());
    CHECK_INT(v0.size(), 4);
    CHECK_INT(*v0.begin(), 'c');
    v0.insert('y');
    CHECK_INT(v0.erase('x'), 0);
    CHECK_INT(v0.erase('y'), 1);

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
    CHECK(v0.value_comp()('a', 'c'));
    CHECK(!v0.value_comp()('a', 'a'));
    CHECK_INT(*v4.find('a'), 'a');
    CHECK_INT(v4.count('x'), 0);
    CHECK_INT(v4.count('a'), 1);
    CHECK_INT(*v4.lower_bound('a'), 'a');
    CHECK_INT(*v4.upper_bound('a'), 'b'); // differs from hash_set
    STD pair<typename Mycont::const_iterator, typename Mycont::const_iterator> pcc = v4.equal_range('a');
    CHECK_INT(*pcc.first, 'a');
    CHECK_INT(*pcc.second, 'b'); // differs from hash_set

    {
        Mycont v6;
        v6.insert('a');
        v6.insert('b');
        Mycont v7(STD move(v6));
        CHECK_INT(v6.size(), 0);
        CHECK_INT(v7.size(), 2);

        Mycont v6a;
        v6a.insert('a');
        v6a.insert('b');
        Mycont v7a(STD move(v6a), al);
        CHECK_INT(v6a.size(), 0);
        CHECK_INT(v7a.size(), 2);

        Mycont v8;
        v8 = STD move(v7);
        CHECK_INT(v7.size(), 0);
        CHECK_INT(v8.size(), 2);

        typedef STD set<Movable_int> Mycont2;
        Mycont2 v9;
        v9.insert(Movable_int('a'));
        CHECK_INT(v9.size(), 1);
        CHECK_INT(v9.begin()->val, 'a');

        Mycont2 v10;
        Movable_int mi1('d');
        v10.insert(STD move(mi1));
        CHECK_INT(mi1.val, -1);
        CHECK_INT(v10.begin()->val, 'd');

        Movable_int mi2('c');
        v10.insert(v10.end(), STD move(mi2));
        CHECK_INT(mi2.val, -1);
        CHECK_INT(v10.begin()->val, 'c');

        v10.clear();
        v10.emplace_hint(v10.end());
        CHECK_INT(v10.begin()->val, 0);
        v10.clear();
        v10.emplace_hint(v10.end(), 2);
        CHECK_INT(v10.begin()->val, 2);
        v10.clear();
        v10.emplace_hint(v10.end(), 3, 2);
        CHECK_INT(v10.begin()->val, 0x32);
        v10.clear();
        v10.emplace_hint(v10.end(), 4, 3, 2);
        CHECK_INT(v10.begin()->val, 0x432);
        v10.clear();
        v10.emplace_hint(v10.end(), 5, 4, 3, 2);
        CHECK_INT(v10.begin()->val, 0x5432);
        v10.clear();
        v10.emplace_hint(v10.end(), 6, 5, 4, 3, 2);
        CHECK_INT(v10.begin()->val, 0x65432);

        v10.clear();
        v10.emplace();
        CHECK_INT(v10.begin()->val, 0);
        v10.clear();
        v10.emplace(2);
        CHECK_INT(v10.begin()->val, 2);
        v10.clear();
        v10.emplace(3, 2);
        CHECK_INT(v10.begin()->val, 0x32);
        v10.clear();
        v10.emplace(4, 3, 2);
        CHECK_INT(v10.begin()->val, 0x432);
        v10.clear();
        v10.emplace(5, 4, 3, 2);
        CHECK_INT(v10.begin()->val, 0x5432);
        v10.clear();
        v10.emplace(6, 5, 4, 3, 2);
        CHECK_INT(v10.begin()->val, 0x65432);
    }

    { // check for lvalue stealing
        typedef STD set<Copyable_int> Mycont3;
        Mycont3 v11;
        Copyable_int ci1('d');
        v11.insert(ci1);
        CHECK_INT(ci1.val, 'd');
        CHECK_INT(v11.begin()->val, 'd');

        Copyable_int ci2('c');
        v11.clear();
        v11.insert(v11.end(), ci2);
        CHECK_INT(ci2.val, 'c');
        CHECK_INT(v11.begin()->val, 'c');

        Mycont3 v12(v11);
        CHECK(v11 == v12);
    }

    {
        STD initializer_list<char> init{carr[0], carr[1], carr[2]};
        Mycont v11(init);
        CHECK_INT(v11.size(), 3);
        CHECK_INT(*v11.begin(), 'a');

        v11.clear();
        v11 = init;
        CHECK_INT(v11.size(), 3);
        CHECK_INT(*v11.begin(), 'a');

        Mycont v11a(init, al);
        CHECK_INT(v11a.size(), 3);
        CHECK_INT(*v11a.begin(), 'a');

        v11.clear();
        v11.insert(init);
        CHECK_INT(v11.size(), 3);
        CHECK_INT(*v11.begin(), 'a');
    }
}

template <class Mypred>
void test_multiset() { // test multiset
    typedef STD allocator<char> Myal;
    typedef STD multiset<char, Mypred, Myal> Mycont;
    char ch = '\0', carr[] = "abc", carr2[] = "def";

    typename Mycont::key_type* p_key         = (char*) nullptr;
    typename Mycont::key_compare* p_kcomp    = (Mypred*) nullptr;
    typename Mycont::allocator_type* p_alloc = (Myal*) nullptr;
    typename Mycont::value_type* p_val       = (char*) nullptr;
    typename Mycont::value_compare* p_vcomp  = (Mypred*) nullptr;
    typename Mycont::pointer p_ptr           = (char*) nullptr;
    typename Mycont::const_pointer p_cptr    = (const char*) nullptr;
    typename Mycont::reference p_ref         = ch;
    typename Mycont::const_reference p_cref  = (const char&) ch;
    typename Mycont::size_type* p_size       = (CSTD size_t*) nullptr;
    typename Mycont::difference_type* p_diff = (CSTD ptrdiff_t*) nullptr;

    p_key   = p_key; // to quiet diagnostics
    p_kcomp = p_kcomp;
    p_alloc = p_alloc;
    p_val   = p_val;
    p_vcomp = p_vcomp;
    p_ptr   = p_ptr;
    p_cptr  = p_cptr;
    p_ptr   = &p_ref;
    p_cptr  = &p_cref;
    p_size  = p_size;
    p_diff  = p_diff;

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

    Mycont v1(carr, carr + 3); // differs from hash_multiset
    CHECK_INT(v1.size(), 3);
    CHECK_INT(*v1.begin(), 'a');

    Mycont v2(carr, carr + 3, pred);
    CHECK_INT(v2.size(), 3);
    CHECK_INT(*v2.begin(), 'a');

    Mycont v3(carr, carr + 3, pred, al);
    CHECK_INT(v3.size(), 3);
    CHECK_INT(*v3.begin(), 'a');

    const Mycont v4(carr, carr + 3);
    CHECK_INT(v4.size(), 3);
    CHECK_INT(*v4.begin(), 'a');
    v0 = v4;
    CHECK_INT(v0.size(), 3);
    CHECK_INT(*v0.begin(), 'a');
    CHECK(v0.size() <= v0.max_size());

    STD multiset<char, Mypred>* p_cont = &v0;

    p_cont = p_cont; // to quiet diagnostics

    { // check iterator generators
        typename Mycont::iterator p_it(v1.begin());
        typename Mycont::const_iterator p_cit(v4.begin());
        typename Mycont::reverse_iterator p_rit(v1.rbegin());
        typename Mycont::const_reverse_iterator p_crit(v4.rbegin());
        CHECK_INT(*p_it, 'a');
        CHECK_INT(*--(p_it = v1.end()), 'c');
        CHECK_INT(*p_cit, 'a');
        CHECK_INT(*--(p_cit = v4.end()), 'c');
        CHECK_INT(*p_rit, 'c');
        CHECK_INT(*--(p_rit = v1.rend()), 'a');
        CHECK_INT(*p_crit, 'c');
        CHECK_INT(*--(p_crit = v4.rend()), 'a');

        typename Mycont::const_iterator p_it1 = typename Mycont::const_iterator();
        typename Mycont::const_iterator p_it2 = typename Mycont::const_iterator();
        CHECK(p_it1 == p_it2); // check null forward iterator comparisons
    }

    { // check const iterators generators
        typename Mycont::const_iterator p_it(v1.cbegin());
        typename Mycont::const_iterator p_cit(v4.cbegin());
        typename Mycont::const_reverse_iterator p_rit(v1.crbegin());
        typename Mycont::const_reverse_iterator p_crit(v4.crbegin());
        CHECK_INT(*p_it, 'a');
        CHECK_INT(*--(p_it = v1.cend()), 'c');
        CHECK_INT(*p_cit, 'a');
        CHECK_INT(*--(p_cit = v4.cend()), 'c');
        CHECK_INT(*p_rit, 'c');
        CHECK_INT(*--(p_rit = v1.crend()), 'a');
        CHECK_INT(*p_crit, 'c');
        CHECK_INT(*--(p_crit = v4.crend()), 'a');

        typename Mycont::const_iterator p_it1 = typename Mycont::const_iterator();
        typename Mycont::const_iterator p_it2 = typename Mycont::const_iterator();
        CHECK(p_it1 == p_it2); // check null forward iterator comparisons
    }

    v0.clear(); // differs from set
    CHECK_INT(*v0.insert('d'), 'd');
    CHECK_INT(*--v0.end(), 'd');
    CHECK_INT(*v0.insert('d'), 'd');
    CHECK_INT(v0.size(), 2);
    CHECK_INT(*v0.insert(v0.begin(), 'e'), 'e');
    v0.insert(carr, carr + 3);
    CHECK_INT(v0.size(), 6);
    CHECK_INT(*v0.begin(), 'a');
    v0.insert(carr2, carr2 + 3);
    CHECK_INT(v0.size(), 9);
    CHECK_INT(*--v0.end(), 'f');
    v0.erase(v0.begin());
    CHECK_INT(v0.size(), 8);
    CHECK_INT(*v0.begin(), 'b');
    v0.erase(v0.begin(), ++v0.begin());
    CHECK_INT(v0.size(), 7);
    CHECK_INT(*v0.begin(), 'c');
    v0.insert('y');
    v0.insert('y');
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
    CHECK(v0.value_comp()('a', 'c'));
    CHECK(!v0.value_comp()('a', 'a'));
    CHECK_INT(*v4.find('a'), 'a');
    CHECK_INT(v4.count('x'), 0);
    CHECK_INT(v4.count('a'), 1);
    CHECK_INT(*v4.lower_bound('a'), 'a');
    CHECK_INT(*v4.upper_bound('a'), 'b'); // differs from hash_multiset
    STD pair<typename Mycont::const_iterator, typename Mycont::const_iterator> pcc = v4.equal_range('a');
    CHECK_INT(*pcc.first, 'a');
    CHECK_INT(*pcc.second, 'b'); // differs from hash_multiset

    {
        Mycont v6;
        v6.insert('a');
        v6.insert('b');
        Mycont v7(STD move(v6));
        CHECK_INT(v6.size(), 0);
        CHECK_INT(v7.size(), 2);

        Mycont v6a;
        v6a.insert('a');
        v6a.insert('b');
        Mycont v7a(STD move(v6a), al);
        CHECK_INT(v6a.size(), 0);
        CHECK_INT(v7a.size(), 2);

        Mycont v8;
        v8 = STD move(v7);
        CHECK_INT(v7.size(), 0);
        CHECK_INT(v8.size(), 2);

        typedef STD multiset<Movable_int> Mycont2;
        Mycont2 v9;
        v9.insert(Movable_int('a'));
        CHECK_INT(v9.size(), 1);
        CHECK_INT(v9.begin()->val, 'a');

        Mycont2 v10;
        Movable_int mi1('d');
        v10.insert(STD move(mi1));
        CHECK_INT(mi1.val, -1);
        CHECK_INT(v10.begin()->val, 'd');

        Movable_int mi2('c');
        v10.insert(v10.end(), STD move(mi2));
        CHECK_INT(mi2.val, -1);
        CHECK_INT(v10.begin()->val, 'c');

        v10.clear();
        v10.emplace_hint(v10.end());
        CHECK_INT(v10.begin()->val, 0);
        v10.clear();
        v10.emplace_hint(v10.end(), 2);
        CHECK_INT(v10.begin()->val, 2);
        v10.clear();
        v10.emplace_hint(v10.end(), 3, 2);
        CHECK_INT(v10.begin()->val, 0x32);
        v10.clear();
        v10.emplace_hint(v10.end(), 4, 3, 2);
        CHECK_INT(v10.begin()->val, 0x432);
        v10.clear();
        v10.emplace_hint(v10.end(), 5, 4, 3, 2);
        CHECK_INT(v10.begin()->val, 0x5432);
        v10.clear();
        v10.emplace_hint(v10.end(), 6, 5, 4, 3, 2);
        CHECK_INT(v10.begin()->val, 0x65432);

        v10.clear();
        v10.emplace();
        CHECK_INT(v10.begin()->val, 0);
        v10.clear();
        v10.emplace(2);
        CHECK_INT(v10.begin()->val, 2);
        v10.clear();
        v10.emplace(3, 2);
        CHECK_INT(v10.begin()->val, 0x32);
        v10.clear();
        v10.emplace(4, 3, 2);
        CHECK_INT(v10.begin()->val, 0x432);
        v10.clear();
        v10.emplace(5, 4, 3, 2);
        CHECK_INT(v10.begin()->val, 0x5432);
        v10.clear();
        v10.emplace(6, 5, 4, 3, 2);
        CHECK_INT(v10.begin()->val, 0x65432);
    }

    { // check for lvalue stealing
        typedef STD multiset<Copyable_int> Mycont3;
        Mycont3 v11;
        Copyable_int ci1('d');
        v11.insert(ci1);
        CHECK_INT(ci1.val, 'd');
        CHECK_INT(v11.begin()->val, 'd');

        Copyable_int ci2('c');
        v11.clear();
        v11.insert(v11.end(), ci2);
        CHECK_INT(ci2.val, 'c');
        CHECK_INT(v11.begin()->val, 'c');

        Mycont3 v12(v11);
        CHECK(v11 == v12);
    }

    {
        STD initializer_list<char> init{carr[0], carr[1], carr[2]};
        Mycont v11(init);
        CHECK_INT(v11.size(), 3);
        CHECK_INT(*v11.begin(), 'a');

        v11.clear();
        v11 = init;
        CHECK_INT(v11.size(), 3);
        CHECK_INT(*v11.begin(), 'a');

        v11.clear();
        v11.insert(init);
        CHECK_INT(v11.size(), 3);
        CHECK_INT(*v11.begin(), 'a');

        Mycont v11a(init, al);
        CHECK_INT(v11a.size(), 3);
        CHECK_INT(*v11a.begin(), 'a');
    }
}

void test_main() { // test basic workings of set definitions
    test_set<STD less<char>>();
    test_set<Myless<Wrapped_char>>();
    test_multiset<STD less<char>>();
    test_multiset<Myless<Wrapped_char>>();
}
