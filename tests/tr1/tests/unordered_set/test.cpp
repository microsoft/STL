// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <unordered_set>
#define TEST_NAME "<unordered_set>"

#define _HAS_AUTO_PTR_ETC 1

#include "tdefs.h"
#include <functional>
#include <stddef.h>
#include <unordered_set>

// TEMPLATE STRUCT SPECIALIZATION hash
namespace std {
    template <>
    struct hash<Copyable_int> : public STD unary_function<Copyable_int, size_t> { // hash functor for Copyable_int
        typedef Copyable_int Kty;

        size_t operator()(const Kty& Keyval) const { // hash Keyval to size_t value by pseudorandomizing transform
            return STD hash<int>()(Keyval);
        }
    };

    template <>
    struct hash<Movable_int> : public STD unary_function<Movable_int, size_t> { // hash functor for Movable_int
        typedef Movable_int Kty;

        size_t operator()(const Kty& Keyval) const { // hash Keyval to size_t value by pseudorandomizing transform
            return STD hash<int>()(Keyval);
        }
    };
} // namespace std

void test_unordered_set() { // test unordered_set
    typedef STD hash<char> Myhash;
    typedef STD equal_to<char> Mycomp;
    typedef STD allocator<char> Myal;
    typedef STD unordered_set<char, Myhash, Mycomp, Myal> Mycont;
    char ch      = '\0';
    char carr[]  = "abc";
    char carr2[] = "def";

    Mycont::key_type* p_key         = (char*) nullptr;
    Mycont::hasher* p_hash          = (Myhash*) nullptr;
    Mycont::key_equal* p_kcomp      = (Mycomp*) nullptr;
    Mycont::allocator_type* p_alloc = (Myal*) nullptr;
    Mycont::value_type* p_val       = (char*) nullptr;
    Mycont::pointer p_ptr           = (char*) nullptr;
    Mycont::const_pointer p_cptr    = (const char*) nullptr;
    Mycont::reference p_ref         = ch;
    Mycont::const_reference p_cref  = (const char&) ch;
    Mycont::size_type* p_size       = (CSTD size_t*) nullptr;
    Mycont::difference_type* p_diff = (CSTD ptrdiff_t*) nullptr;

    p_key   = p_key; // to quiet diagnostics
    p_hash  = p_hash;
    p_kcomp = p_kcomp;
    p_alloc = p_alloc;
    p_val   = p_val;
    p_ptr   = p_ptr;
    p_cptr  = p_cptr;
    p_ptr   = &p_ref;
    p_cptr  = &p_cref;
    p_size  = p_size;
    p_diff  = p_diff;

    Mycont v0;
    Myal al     = v0.get_allocator();
    Mycomp pred = v0.key_eq();
    Myhash hfn  = v0.hash_function();
    Mycont v0a(0), v0b(10, hfn), v0c(20, hfn, pred), v0d(30, hfn, pred, al);
    CHECK(v0.empty());
    CHECK_INT(v0.size(), 0);
    CHECK_INT(v0a.size(), 0);
    CHECK(v0a.get_allocator() == al);
    CHECK_INT(v0b.size(), 0);
    CHECK(v0b.get_allocator() == al);

    Mycont v1(carr, carr + 1); // differs from set
    CHECK_INT(v1.size(), 1);
    CHECK_INT(*v1.begin(), 'a');

    Mycont v2(carr, carr + 1, 10);
    CHECK_INT(v2.size(), 1);
    CHECK_INT(*v2.begin(), 'a');

    Mycont v3(carr, carr + 1, 20, hfn);
    CHECK_INT(v3.size(), 1);
    CHECK_INT(*v3.begin(), 'a');

    const Mycont v4(carr, carr + 1, 30, hfn, pred);
    CHECK_INT(v4.size(), 1);
    CHECK_INT(*v4.begin(), 'a');

    const Mycont v5(carr, carr + 1, 40, hfn, pred, al);
    CHECK_INT(v4.size(), 1);
    CHECK_INT(*v4.begin(), 'a');

    v0 = v4;
    CHECK_INT(v0.size(), 1);
    CHECK_INT(*v0.begin(), 'a');
    CHECK(v0.size() <= v0.max_size());

    Mycont* p_cont = &v0;
    p_cont         = p_cont; // to quiet diagnostics

    { // check iterator generators
        Mycont::iterator p_it(v1.begin());
        Mycont::const_iterator p_cit(v4.begin());
        CHECK_INT(*p_it, 'a');
        CHECK_INT(*--(p_it = v1.end()), 'a');
        CHECK_INT(*p_cit, 'a');
        CHECK_INT(*--(p_cit = v4.end()), 'a');

        Mycont::const_iterator p_it1 = Mycont::const_iterator();
        Mycont::const_iterator p_it2 = Mycont::const_iterator();
        CHECK(p_it1 == p_it2); // check null forward iterator comparisons
    }

    { // check const iterators generators
        Mycont::const_iterator p_it(v1.cbegin());
        Mycont::const_iterator p_cit(v4.cbegin());
        CHECK_INT(*p_it, 'a');
        CHECK_INT(*--(p_it = v1.cend()), 'a');
        CHECK_INT(*p_cit, 'a');
        CHECK_INT(*--(p_cit = v4.cend()), 'a');

        Mycont::const_iterator p_it1 = Mycont::const_iterator();
        Mycont::const_iterator p_it2 = Mycont::const_iterator();
        CHECK(p_it1 == p_it2); // check null forward iterator comparisons
    }

    { // check comparisons
        Mycont v1x(carr, carr + 3);
        v0.clear();
        v0.insert(carr2, carr2 + 3);
        CHECK(v0 == v0);
        CHECK(v0 != v1x);
    }

    v0.clear(); // differs from unordered_multiset
    STD pair<Mycont::iterator, bool> pib = v0.insert('d');
    CHECK_INT(*pib.first, 'd');
    CHECK(pib.second);
    CHECK_INT(*--v0.end(), 'd');
    pib = v0.insert('d');
    CHECK_INT(*pib.first, 'd');
    CHECK(!pib.second);
    CHECK_INT(*v0.insert(v0.begin(), 'e'), 'e');
    v0.insert(carr, carr + 3);
    CHECK_INT(v0.size(), 5);
    v0.insert(carr2, carr2 + 3);
    CHECK_INT(v0.size(), 6);
    v0.erase(v0.begin());
    CHECK_INT(v0.size(), 5);
    v0.erase(v0.begin(), ++v0.begin());
    CHECK_INT(v0.size(), 4);
    v0.insert('y');
    CHECK_INT(v0.erase('x'), 0);
    CHECK_INT(v0.erase('y'), 1);

    { // test added C++11 functionality
        Mycont v0e(al);
        CHECK_INT(v0e.size(), 0);
        CHECK(v0e.get_allocator() == al);
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
    CHECK(v0 != v1);

    CHECK(v0.key_eq()('a', 'a'));
    CHECK(!v0.key_eq()('a', 'c'));
    CHECK_INT(*v4.find('a'), 'a');
    CHECK_INT(v4.count('x'), 0);
    CHECK_INT(v4.count('a'), 1);
    STD pair<Mycont::const_iterator, Mycont::const_iterator> pcc = v4.equal_range('a');
    CHECK_INT(*pcc.first, 'a');
    CHECK_INT(*--pcc.second, 'a'); // differs from set

    {
        Mycont v10(carr, carr + 3, 20);
        Mycont::size_type nbuckets = v10.bucket_count();
        CHECK(20 <= nbuckets);
        CHECK(nbuckets <= v10.max_bucket_count());
        Mycont::size_type bucket = v10.bucket('b');
        CHECK(bucket < nbuckets);
        Mycont::size_type bsize = v10.bucket_size(bucket);
        CHECK(0 < bsize);

        Mycont::local_iterator p_lit(v10.begin(bucket));
        CHECK(0 <= STD distance(p_lit, v10.end(bucket)));
        Mycont::const_local_iterator p_clit(v10.begin(bucket));
        CHECK(0 <= STD distance(p_clit, Mycont::const_local_iterator(v10.end(bucket))));

        Mycont::const_local_iterator p_clit2(v10.cbegin(bucket));
        CHECK(0 <= STD distance(p_clit2, Mycont::const_local_iterator(v10.cend(bucket))));

        float mlf = v10.max_load_factor();
        CHECK(v10.load_factor() <= mlf);
        v10.max_load_factor(mlf);
        CHECK(v10.max_load_factor() == mlf);
        v10.rehash(nbuckets);
        CHECK_INT(v10.size(), 3);
        CHECK_INT(v10.count('a'), 1);

        v10.reserve((Mycont::size_type)(static_cast<float>(nbuckets) * v10.max_load_factor() + 0.5));
        CHECK_INT(v10.size(), 3);
        CHECK_INT(v10.count('a'), 1);
    }

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

        typedef STD unordered_set<Movable_int> Mycont2;
        Mycont2 v9;
        Movable_int mi0('a');
        v9.insert(STD move(mi0));
        CHECK_INT(v9.size(), 1);
        CHECK_INT(v9.begin()->val, 'a');

        Mycont2 v10;
        Movable_int mi1('d');
        v10.clear();
        v10.insert(STD move(mi1));
        CHECK_INT(mi1.val, -1);
        CHECK_INT(v10.begin()->val, 'd');

        Movable_int mi2('c');
        v10.clear();
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
        typedef STD unordered_set<Copyable_int> Mycont3;
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
        STD initializer_list<char> init{'a', 'b', 'c'};
        Mycont v11(init);
        CHECK_INT(v11.size(), 3);
        CHECK_INT(*v11.find('a'), 'a');

        v11.clear();
        v11 = init;
        CHECK_INT(v11.size(), 3);
        CHECK_INT(*v11.find('a'), 'a');

        Mycont v11a(init, 30, hfn, pred, al);
        CHECK_INT(v11a.size(), 3);
        CHECK_INT(*v11a.find('a'), 'a');

        v11.clear();
        v11.insert(init);
        CHECK_INT(v11.size(), 3);
        CHECK_INT(*v11.find('a'), 'a');
    }
}

void test_unordered_multiset() { // test unordered_multiset
    typedef STD hash<char> Myhash;
    typedef STD equal_to<char> Mycomp;
    typedef STD allocator<char> Myal;
    typedef STD unordered_multiset<char, Myhash, Mycomp, Myal> Mycont;
    char ch      = '\0';
    char carr[]  = "abc";
    char carr2[] = "def";

    Mycont::key_type* p_key         = (char*) nullptr;
    Mycont::hasher* p_hash          = (Myhash*) nullptr;
    Mycont::key_equal* p_kcomp      = (Mycomp*) nullptr;
    Mycont::allocator_type* p_alloc = (Myal*) nullptr;
    Mycont::value_type* p_val       = (char*) nullptr;
    Mycont::pointer p_ptr           = (char*) nullptr;
    Mycont::const_pointer p_cptr    = (const char*) nullptr;
    Mycont::reference p_ref         = ch;
    Mycont::const_reference p_cref  = (const char&) ch;
    Mycont::size_type* p_size       = (CSTD size_t*) nullptr;
    Mycont::difference_type* p_diff = (CSTD ptrdiff_t*) nullptr;

    p_key   = p_key; // to quiet diagnostics
    p_hash  = p_hash;
    p_kcomp = p_kcomp;
    p_alloc = p_alloc;
    p_val   = p_val;
    p_ptr   = p_ptr;
    p_cptr  = p_cptr;
    p_ptr   = &p_ref;
    p_cptr  = &p_cref;
    p_size  = p_size;
    p_diff  = p_diff;

    Mycont v0;
    Myal al     = v0.get_allocator();
    Mycomp pred = v0.key_eq();
    Myhash hfn  = v0.hash_function();
    Mycont v0a(0), v0b(10, hfn), v0c(20, hfn, pred), v0d(30, hfn, pred, al);
    CHECK(v0.empty());
    CHECK_INT(v0.size(), 0);
    CHECK_INT(v0a.size(), 0);
    CHECK(v0a.get_allocator() == al);
    CHECK_INT(v0b.size(), 0);
    CHECK(v0b.get_allocator() == al);

    Mycont v1(carr, carr + 1); // differs from multiset
    CHECK_INT(v1.size(), 1);
    CHECK_INT(*v1.begin(), 'a');

    Mycont v2(carr, carr + 1, 10);
    CHECK_INT(v2.size(), 1);
    CHECK_INT(*v2.begin(), 'a');

    Mycont v3(carr, carr + 1, 20, hfn);
    CHECK_INT(v3.size(), 1);
    CHECK_INT(*v3.begin(), 'a');

    const Mycont v4(carr, carr + 1, 30, hfn, pred);
    CHECK_INT(v4.size(), 1);
    CHECK_INT(*v4.begin(), 'a');

    const Mycont v5(carr, carr + 1, 40, hfn, pred, al);
    CHECK_INT(v4.size(), 1);
    CHECK_INT(*v4.begin(), 'a');

    v0 = v4;
    CHECK_INT(v0.size(), 1);
    CHECK_INT(*v0.begin(), 'a');
    CHECK(v0.size() <= v0.max_size());

    Mycont* p_cont = &v0;
    p_cont         = p_cont; // to quiet diagnostics

    { // check iterator generators
        Mycont::iterator p_it(v1.begin());
        Mycont::const_iterator p_cit(v4.begin());
        CHECK_INT(*p_it, 'a');
        CHECK_INT(*--(p_it = v1.end()), 'a');
        CHECK_INT(*p_cit, 'a');
        CHECK_INT(*--(p_cit = v4.end()), 'a');

        Mycont::const_iterator p_it1 = Mycont::const_iterator();
        Mycont::const_iterator p_it2 = Mycont::const_iterator();
        CHECK(p_it1 == p_it2); // check null forward iterator comparisons
    }

    { // check const iterators generators
        Mycont::const_iterator p_it(v1.cbegin());
        Mycont::const_iterator p_cit(v4.cbegin());
        CHECK_INT(*p_it, 'a');
        CHECK_INT(*--(p_it = v1.cend()), 'a');
        CHECK_INT(*p_cit, 'a');
        CHECK_INT(*--(p_cit = v4.cend()), 'a');

        Mycont::const_iterator p_it1 = Mycont::const_iterator();
        Mycont::const_iterator p_it2 = Mycont::const_iterator();
        CHECK(p_it1 == p_it2); // check null forward iterator comparisons
    }

    { // check comparisons
        Mycont v1x(carr, carr + 3);
        v1.insert(carr2, carr2 + 3);
        v1.insert(carr2, carr2 + 3);
        v0.clear();
        v0.insert(carr2, carr2 + 3);
        v0.insert(carr2, carr2 + 3);
        CHECK(v0 == v0);
        CHECK(v0 != v1x);
    }

    v0.clear(); // differs from unordered_set
    CHECK_INT(*v0.insert('d'), 'd');
    CHECK_INT(*--v0.end(), 'd');
    CHECK_INT(*v0.insert('d'), 'd');
    CHECK_INT(v0.size(), 2);
    CHECK_INT(*v0.insert(v0.begin(), 'e'), 'e');
    v0.insert(carr, carr + 3);
    CHECK_INT(v0.size(), 6);
    v0.insert(carr2, carr2 + 3);
    CHECK_INT(v0.size(), 9);
    v0.erase(v0.begin());
    CHECK_INT(v0.size(), 8);
    v0.erase(v0.begin(), ++v0.begin());
    CHECK_INT(v0.size(), 7);
    v0.insert('y');
    v0.insert('y');
    CHECK_INT(v0.erase('x'), 0);
    CHECK_INT(v0.erase('y'), 2);

    { // test added C++11 functionality
        Mycont v0e(al);
        CHECK_INT(v0e.size(), 0);
        CHECK(v0e.get_allocator() == al);
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
    CHECK(v0 != v1);

    CHECK(v0.key_eq()('a', 'a'));
    CHECK(!v0.key_eq()('a', 'c'));
    CHECK_INT(*v4.find('a'), 'a');
    CHECK_INT(v4.count('x'), 0);
    CHECK_INT(v4.count('a'), 1);
    STD pair<Mycont::const_iterator, Mycont::const_iterator> pcc = v4.equal_range('a');
    CHECK_INT(*pcc.first, 'a');
    CHECK_INT(*--pcc.second, 'a'); // differs from multiset

    {
        Mycont v10(carr, carr + 3, 20);
        Mycont::size_type nbuckets = v10.bucket_count();
        CHECK(20 <= nbuckets);
        CHECK(nbuckets <= v10.max_bucket_count());
        Mycont::size_type bucket = v10.bucket('b');
        CHECK(bucket < nbuckets);
        Mycont::size_type bsize = v10.bucket_size(bucket);
        CHECK(0 < bsize);

        Mycont::local_iterator p_lit(v10.begin(bucket));
        CHECK(0 <= STD distance(p_lit, v10.end(bucket)));
        Mycont::const_local_iterator p_clit(v10.begin(bucket));
        CHECK(0 <= STD distance(p_clit, Mycont::const_local_iterator(v10.end(bucket))));

        Mycont::const_local_iterator p_clit2(v10.cbegin(bucket));
        CHECK(0 <= STD distance(p_clit2, Mycont::const_local_iterator(v10.cend(bucket))));

        float mlf = v10.max_load_factor();
        CHECK(v10.load_factor() <= mlf);
        v10.max_load_factor(mlf);
        CHECK(v10.max_load_factor() == mlf);
        v10.rehash(nbuckets);
        CHECK_INT(v10.size(), 3);
        CHECK_INT(v10.count('a'), 1);
    }

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

        typedef STD unordered_multiset<Movable_int> Mycont2;
        Mycont2 v9;
        Movable_int mi0('a');
        v9.insert(STD move(mi0));
        CHECK_INT(v9.size(), 1);
        CHECK_INT(v9.begin()->val, 'a');

        Mycont2 v10;
        Movable_int mi1('d');
        v10.insert(STD move(mi1));
        CHECK_INT(mi1.val, -1);
        CHECK_INT(v10.begin()->val, 'd');

        Movable_int mi2('c');
        v10.clear();
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
        typedef STD unordered_multiset<Copyable_int> Mycont3;
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
        STD initializer_list<char> init{'a', 'b', 'c'};
        Mycont v11(init);
        CHECK_INT(v11.size(), 3);
        CHECK_INT(*v11.find('a'), 'a');

        v11.clear();
        v11 = init;
        CHECK_INT(v11.size(), 3);
        CHECK_INT(*v11.find('a'), 'a');

        v11.clear();
        v11.insert(init);
        CHECK_INT(v11.size(), 3);
        CHECK_INT(*v11.find('a'), 'a');

        Mycont v11a(init, 30, hfn, pred, al);
        CHECK_INT(v11a.size(), 3);
        CHECK_INT(*v11a.find('a'), 'a');
    }
}

void test_main() { // test basic workings of set definitions
    test_unordered_set();
    test_unordered_multiset();
}
