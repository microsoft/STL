// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdlib>
#include <deque>
#include <forward_list>
#include <functional>
#include <iterator>
#include <list>
#include <string>
#include <utility>
#include <vector>

using namespace std;

int g_equal = 0;

struct Meow {
    bool operator==(const Meow&) const {
        ++g_equal;
        return true;
    }
};

int g_pred = 0;

bool MeowPred(const Meow&, const Meow&) {
    ++g_pred;
    return true;
}

void more_tests();

int main() {
    { // Test exchange().
        string s("meow");

        assert(exchange(s, "purr") == "meow");

        assert(s == "purr");
    }


    { // Dual-range mismatch()/equal()/is_permutation() shouldn't warn about raw pointers.
        const int x[] = {11, 22, 33};
        const int y[] = {11, 22, 33};

        assert(mismatch(begin(x), end(x), begin(y), end(y)) == make_pair(end(x), end(y)));
        assert(mismatch(begin(x), end(x), begin(y), end(y), equal_to<int>()) == make_pair(end(x), end(y)));

        assert(equal(begin(x), end(x), begin(y), end(y)));
        assert(equal(begin(x), end(x), begin(y), end(y), equal_to<int>()));

        assert(is_permutation(begin(x), end(x), begin(y), end(y)));
        assert(is_permutation(begin(x), end(x), begin(y), end(y), equal_to<int>()));
    }


    { // Test dual-range mismatch() with different elements, same lengths.
        vector<int> a = {10, 20, 30, 999, 50};
        vector<int> b = {10, 20, 30, 40, 50};

        assert(mismatch(a.begin(), a.end(), b.begin(), b.end()) == make_pair(a.begin() + 3, b.begin() + 3));
        assert(mismatch(a.begin(), a.end(), b.begin(), b.end(), equal_to<int>())
               == make_pair(a.begin() + 3, b.begin() + 3));
    }

    { // Test dual-range mismatch() with different elements, first longer.
        vector<int> a = {10, 20, 30, 999, 50, 60, 70};
        vector<int> b = {10, 20, 30, 40, 50};

        assert(mismatch(a.begin(), a.end(), b.begin(), b.end()) == make_pair(a.begin() + 3, b.begin() + 3));
        assert(mismatch(a.begin(), a.end(), b.begin(), b.end(), equal_to<int>())
               == make_pair(a.begin() + 3, b.begin() + 3));
    }

    { // Test dual-range mismatch() with different elements, second longer.
        vector<int> a = {10, 20, 30, 999, 50};
        vector<int> b = {10, 20, 30, 40, 50, 60, 70};

        assert(mismatch(a.begin(), a.end(), b.begin(), b.end()) == make_pair(a.begin() + 3, b.begin() + 3));
        assert(mismatch(a.begin(), a.end(), b.begin(), b.end(), equal_to<int>())
               == make_pair(a.begin() + 3, b.begin() + 3));
    }


    { // Test dual-range mismatch() with same elements, same lengths.
        vector<int> a = {10, 20, 30, 40, 50};
        vector<int> b = {10, 20, 30, 40, 50};

        assert(mismatch(a.begin(), a.end(), b.begin(), b.end()) == make_pair(a.begin() + 5, b.begin() + 5));
        assert(mismatch(a.begin(), a.end(), b.begin(), b.end(), equal_to<int>())
               == make_pair(a.begin() + 5, b.begin() + 5));
    }

    { // Test dual-range mismatch() with same elements, first longer.
        vector<int> a = {10, 20, 30, 40, 50, 60, 70};
        vector<int> b = {10, 20, 30, 40, 50};

        assert(mismatch(a.begin(), a.end(), b.begin(), b.end()) == make_pair(a.begin() + 5, b.begin() + 5));
        assert(mismatch(a.begin(), a.end(), b.begin(), b.end(), equal_to<int>())
               == make_pair(a.begin() + 5, b.begin() + 5));
    }

    { // Test dual-range mismatch() with same elements, second longer.
        vector<int> a = {10, 20, 30, 40, 50};
        vector<int> b = {10, 20, 30, 40, 50, 60, 70};

        assert(mismatch(a.begin(), a.end(), b.begin(), b.end()) == make_pair(a.begin() + 5, b.begin() + 5));
        assert(mismatch(a.begin(), a.end(), b.begin(), b.end(), equal_to<int>())
               == make_pair(a.begin() + 5, b.begin() + 5));
    }


    { // Test dual-range equal() with random-access sequences of different lengths and counting comparisons.
        array<Meow, 10> a;
        deque<Meow> d(20);

        assert(!equal(a.begin(), a.end(), d.begin(), d.end()));
        assert(g_equal == 0);
        assert(!equal(a.begin(), a.end(), d.begin(), d.end(), MeowPred));
        assert(g_pred == 0);

        // Verify that the counting comparisons actually count.
        assert(equal(a.begin(), a.begin() + 5, d.begin(), d.begin() + 5));
        assert(g_equal == 5);
        assert(equal(a.begin(), a.begin() + 7, d.begin(), d.begin() + 7, MeowPred));
        assert(g_pred == 7);

        g_equal = 0;
        g_pred  = 0;
    }

    { // Test dual-range equal() with random-access sequences, same lengths, same/different elements.
        vector<int> a = {10, 20, 30, 40, 50};
        vector<int> b = {10, 20, 30, 40, 50};

        assert(equal(a.begin(), a.end(), b.begin(), b.end()));
        assert(equal(a.begin(), a.end(), b.begin(), b.end(), equal_to<int>()));

        b[2] = 999;

        assert(!equal(a.begin(), a.end(), b.begin(), b.end()));
        assert(!equal(a.begin(), a.end(), b.begin(), b.end(), equal_to<int>()));
    }

    { // Test dual-range equal() with weaker sequences, same lengths, same/different elements.
        list<int> a   = {10, 20, 30, 40, 50};
        vector<int> b = {10, 20, 30, 40, 50};

        assert(equal(a.begin(), a.end(), b.begin(), b.end()));
        assert(equal(a.begin(), a.end(), b.begin(), b.end(), equal_to<int>()));

        b[2] = 999;

        assert(!equal(a.begin(), a.end(), b.begin(), b.end()));
        assert(!equal(a.begin(), a.end(), b.begin(), b.end(), equal_to<int>()));
    }

    { // Test dual-range equal() with weaker sequences, first longer, same/different elements.
        list<int> a   = {10, 20, 30, 40, 50, 60, 70};
        vector<int> b = {10, 20, 30, 40, 50};

        assert(!equal(a.begin(), a.end(), b.begin(), b.end()));
        assert(!equal(a.begin(), a.end(), b.begin(), b.end(), equal_to<int>()));

        b[2] = 999;

        assert(!equal(a.begin(), a.end(), b.begin(), b.end()));
        assert(!equal(a.begin(), a.end(), b.begin(), b.end(), equal_to<int>()));
    }

    { // Test dual-range equal() with weaker sequences, second longer, same/different elements.
        list<int> a   = {10, 20, 30, 40, 50};
        vector<int> b = {10, 20, 30, 40, 50, 60, 70};

        assert(!equal(a.begin(), a.end(), b.begin(), b.end()));
        assert(!equal(a.begin(), a.end(), b.begin(), b.end(), equal_to<int>()));

        b[2] = 999;

        assert(!equal(a.begin(), a.end(), b.begin(), b.end()));
        assert(!equal(a.begin(), a.end(), b.begin(), b.end(), equal_to<int>()));
    }


    { // Test memcmp() optimization.
        const char c1[] = {'A', 'B', 'C'};
        const char c2[] = {'A', 'X', 'C'};

        assert(equal(begin(c1), end(c1), begin(c1), end(c1)));
        assert(equal(begin(c1), end(c1), begin(c1), end(c1), equal_to<>()));
        assert(equal(begin(c1), end(c1), c1));
        assert(equal(begin(c1), end(c1), c1, equal_to<>()));
        assert(!equal(begin(c1), end(c1), begin(c2), end(c2)));
        assert(!equal(begin(c1), end(c1), begin(c2), end(c2), equal_to<>()));
        assert(!equal(begin(c1), end(c1), c2));
        assert(!equal(begin(c1), end(c1), c2, equal_to<>()));

        const signed char sc1[] = {'D', 'E', 'F'};
        const signed char sc2[] = {'D', 'X', 'F'};

        assert(equal(begin(sc1), end(sc1), begin(sc1), end(sc1)));
        assert(equal(begin(sc1), end(sc1), begin(sc1), end(sc1), equal_to<>()));
        assert(equal(begin(sc1), end(sc1), sc1));
        assert(equal(begin(sc1), end(sc1), sc1, equal_to<>()));
        assert(!equal(begin(sc1), end(sc1), begin(sc2), end(sc2)));
        assert(!equal(begin(sc1), end(sc1), begin(sc2), end(sc2), equal_to<>()));
        assert(!equal(begin(sc1), end(sc1), sc2));
        assert(!equal(begin(sc1), end(sc1), sc2, equal_to<>()));

        const unsigned char uc1[] = {'G', 'H', 'I'};
        const unsigned char uc2[] = {'G', 'X', 'I'};

        assert(equal(begin(uc1), end(uc1), begin(uc1), end(uc1)));
        assert(equal(begin(uc1), end(uc1), begin(uc1), end(uc1), equal_to<>()));
        assert(equal(begin(uc1), end(uc1), uc1));
        assert(equal(begin(uc1), end(uc1), uc1, equal_to<>()));
        assert(!equal(begin(uc1), end(uc1), begin(uc2), end(uc2)));
        assert(!equal(begin(uc1), end(uc1), begin(uc2), end(uc2), equal_to<>()));
        assert(!equal(begin(uc1), end(uc1), uc2));
        assert(!equal(begin(uc1), end(uc1), uc2, equal_to<>()));
    }


    { // Test dual-range is_permutation() with random-access sequences of different lengths and counting comparisons.
        array<Meow, 10> a;
        deque<Meow> d(20);

        assert(!is_permutation(a.begin(), a.end(), d.begin(), d.end()));
        assert(g_equal == 0);
        assert(!is_permutation(a.begin(), a.end(), d.begin(), d.end(), MeowPred));
        assert(g_pred == 0);

        // N3797 25.2.12 [alg.is_permutation]/4:
        // Complexity: No applications of the corresponding predicate if ForwardIterator1 and ForwardIterator2
        // meet the requirements of random access iterators and last1 - first1 != last2 - first2.
        // Otherwise, exactly distance(first1, last1) applications of the corresponding predicate if
        // equal(first1, last1, first2, last2) would return true if pred was not given in the argument list or
        // equal(first1, last1, first2, last2, pred) would return true if pred was given in the argument list;
        // otherwise, at worst O(N^2), where N has the value distance(first1, last1).
        assert(is_permutation(a.begin(), a.begin() + 5, d.begin(), d.begin() + 5));
        assert(g_equal == 5);
        assert(is_permutation(a.begin(), a.begin() + 7, d.begin(), d.begin() + 7, MeowPred));
        assert(g_pred == 7);

        g_equal = 0;
        g_pred  = 0;
    }

    { // Test dual-range is_permutation() with random-access sequences, same lengths, same/different elements.
        vector<int> a = {10, 20, 30, 40, 50};
        vector<int> b = {10, 20, 30, 40, 50};

        assert(is_permutation(a.begin(), a.end(), b.begin(), b.end()));
        assert(is_permutation(a.begin(), a.end(), b.begin(), b.end(), equal_to<int>()));

        b[2] = 999;

        assert(!is_permutation(a.begin(), a.end(), b.begin(), b.end()));
        assert(!is_permutation(a.begin(), a.end(), b.begin(), b.end(), equal_to<int>()));
    }

    { // Test dual-range is_permutation() with weaker sequences, same lengths, same/different elements.
        list<int> a   = {10, 20, 30, 40, 50};
        vector<int> b = {10, 20, 30, 40, 50};

        assert(is_permutation(a.begin(), a.end(), b.begin(), b.end()));
        assert(is_permutation(a.begin(), a.end(), b.begin(), b.end(), equal_to<int>()));

        b[2] = 999;

        assert(!is_permutation(a.begin(), a.end(), b.begin(), b.end()));
        assert(!is_permutation(a.begin(), a.end(), b.begin(), b.end(), equal_to<int>()));
    }

    { // Test dual-range is_permutation() with weaker sequences, first longer, same/different elements.
        list<int> a   = {10, 20, 30, 40, 50, 60, 70};
        vector<int> b = {10, 20, 30, 40, 50};

        assert(!is_permutation(a.begin(), a.end(), b.begin(), b.end()));
        assert(!is_permutation(a.begin(), a.end(), b.begin(), b.end(), equal_to<int>()));

        b[2] = 999;

        assert(!is_permutation(a.begin(), a.end(), b.begin(), b.end()));
        assert(!is_permutation(a.begin(), a.end(), b.begin(), b.end(), equal_to<int>()));
    }

    { // Test dual-range is_permutation() with weaker sequences, second longer, same/different elements.
        list<int> a   = {10, 20, 30, 40, 50};
        vector<int> b = {10, 20, 30, 40, 50, 60, 70};

        assert(!is_permutation(a.begin(), a.end(), b.begin(), b.end()));
        assert(!is_permutation(a.begin(), a.end(), b.begin(), b.end(), equal_to<int>()));

        b[2] = 999;

        assert(!is_permutation(a.begin(), a.end(), b.begin(), b.end()));
        assert(!is_permutation(a.begin(), a.end(), b.begin(), b.end(), equal_to<int>()));
    }

    { // Test dual-range is_permutation() with random-access sequences, same lengths, permuted/non-permuted elements.
        vector<int> a = {10, 20, 30, 40, 50};
        vector<int> b = {30, 50, 40, 10, 20};

        assert(is_permutation(a.begin(), a.end(), b.begin(), b.end()));
        assert(is_permutation(a.begin(), a.end(), b.begin(), b.end(), equal_to<int>()));

        a = {11, 22, 22, 33, 33, 33};
        b = {11, 11, 22, 22, 33, 33};

        assert(!is_permutation(a.begin(), a.end(), b.begin(), b.end()));
        assert(!is_permutation(a.begin(), a.end(), b.begin(), b.end(), equal_to<int>()));
    }

    { // Test dual-range is_permutation() with weaker sequences, same lengths, permuted/non-permuted elements.
        list<int> a   = {10, 20, 30, 40, 50};
        vector<int> b = {30, 50, 40, 10, 20};

        assert(is_permutation(a.begin(), a.end(), b.begin(), b.end()));
        assert(is_permutation(a.begin(), a.end(), b.begin(), b.end(), equal_to<int>()));

        a = {11, 22, 22, 33, 33, 33};
        b = {11, 11, 22, 22, 33, 33};

        assert(!is_permutation(a.begin(), a.end(), b.begin(), b.end()));
        assert(!is_permutation(a.begin(), a.end(), b.begin(), b.end(), equal_to<int>()));
    }

    {
        int arr1[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        int arr2[] = {9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
        assert(is_permutation(begin(arr1), end(arr1), begin(arr2), end(arr2)));
    }
    {
        int arr1[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        int arr2[] = {9, 8, 7, 3, 4, 5, 6, 2, 1, 0};
        assert(is_permutation(begin(arr1), end(arr1), begin(arr2), end(arr2)));
    }
    {
        int arr1[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        int arr2[] = {9, 1, 7, 3, 5, 4, 6, 2, 8, 0};
        assert(is_permutation(begin(arr1), end(arr1), begin(arr2), end(arr2)));
    }
    {
        int arr1[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        int arr2[] = {9, 1, 7, 5, 6, 3, 4, 2, 8, 0};
        assert(is_permutation(begin(arr1), end(arr1), begin(arr2), end(arr2)));
    }
    {
        int arr1[] = {0, 1, 2, 3, 4, 10, 5, 6, 7, 8, 9};
        int arr2[] = {9, 1, 7, 3, 5, 11, 4, 6, 2, 8, 0};
        assert(!is_permutation(begin(arr1), end(arr1), begin(arr2), end(arr2)));
    }

    { // Test that _ITERATOR_DEBUG_ARRAY_OVERLOADS is not needed anymore
        int arr[8] = {};
        assert(mismatch(arr, arr, arr, arr) == make_pair(begin(arr), begin(arr)));
        assert(mismatch(arr, arr, arr, arr + 0) == make_pair(begin(arr), begin(arr)));

        assert(equal(arr, arr, arr, arr));
        assert(equal(arr, arr, arr, arr + 0));

        assert(is_permutation(arr, arr, arr, arr));
        assert(is_permutation(arr, arr, arr, arr + 0));
    }

    more_tests();
}

template <typename A, typename B>
void test_helper1(const list<int>& one, const list<int>& two, const bool expected) {
    A a(one.begin(), one.end());
    B b(two.begin(), two.end());

    assert(is_permutation(a.begin(), a.end(), b.begin(), b.end()) == expected);
}

void test_helper2(const list<int>& one, const list<int>& two, const bool expected) {
    typedef forward_list<int> F;
    typedef list<int> L;
    typedef vector<int> V;

    test_helper1<F, F>(one, two, expected);
    test_helper1<F, L>(one, two, expected);
    test_helper1<F, V>(one, two, expected);
    test_helper1<L, F>(one, two, expected);
    test_helper1<L, L>(one, two, expected);
    test_helper1<L, V>(one, two, expected);
    test_helper1<V, F>(one, two, expected);
    test_helper1<V, L>(one, two, expected);
    test_helper1<V, V>(one, two, expected);
}

void test_helper3(initializer_list<int> il_a, initializer_list<int> il_b, const bool expected,
    initializer_list<int> prefix, initializer_list<int> suffix) {

    list<int> one(il_a);
    list<int> two(il_b);

    one.insert(one.begin(), prefix);
    one.insert(one.end(), suffix);

    two.insert(two.begin(), prefix);
    two.insert(two.end(), suffix);

    test_helper2(one, two, expected);
}

void test_helper4(initializer_list<int> il_a, initializer_list<int> il_b, const bool expected) {
    test_helper3(il_a, il_b, expected, {}, {});
    test_helper3(il_a, il_b, expected, {70}, {});
    test_helper3(il_a, il_b, expected, {70, 80}, {});
    test_helper3(il_a, il_b, expected, {70, 80, 90}, {});

    test_helper3(il_a, il_b, expected, {}, {700});
    test_helper3(il_a, il_b, expected, {70}, {700});
    test_helper3(il_a, il_b, expected, {70, 80}, {700});
    test_helper3(il_a, il_b, expected, {70, 80, 90}, {700});

    test_helper3(il_a, il_b, expected, {}, {700, 800});
    test_helper3(il_a, il_b, expected, {70}, {700, 800});
    test_helper3(il_a, il_b, expected, {70, 80}, {700, 800});
    test_helper3(il_a, il_b, expected, {70, 80, 90}, {700, 800});

    test_helper3(il_a, il_b, expected, {}, {700, 800, 900});
    test_helper3(il_a, il_b, expected, {70}, {700, 800, 900});
    test_helper3(il_a, il_b, expected, {70, 80}, {700, 800, 900});
    test_helper3(il_a, il_b, expected, {70, 80, 90}, {700, 800, 900});

    test_helper3(il_a, il_b, expected, {10, 20, 30}, {11, 22, 33});
}

void more_tests() {
    test_helper4({}, {}, true);
    test_helper4({11}, {11}, true);
    test_helper4({11, 22}, {11, 22}, true);
    test_helper4({11, 22, 33}, {11, 22, 33}, true);

#define SEQ_A 20, 30, 40, 22, 44, 22, 33, 10, 33, 40, 20, 44, 11, 40, 44, 33, 30, 40, 30, 44
#define SEQ_B 40, 44, 44, 11, 40, 20, 20, 40, 33, 30, 22, 22, 40, 10, 33, 33, 44, 30, 44, 30

    test_helper4({SEQ_A}, {SEQ_B}, true);

#undef SEQ_A
#undef SEQ_B

// baseline
#define SEQ_C 22, 44, 22, 10, 40, 11, 30, 30, 40, 33, 30, 33, 33, 20, 44, 44, 40, 40, 20, 44

// good
#define SEQ_D 40, 11, 44, 33, 30, 30, 22, 44, 33, 22, 10, 44, 44, 40, 40, 20, 33, 40, 20, 30

// bad, zero 33s (they were replaced by 20s)
#define SEQ_E 40, 11, 44, 20, 30, 30, 22, 44, 20, 22, 10, 44, 44, 40, 40, 20, 20, 40, 20, 30

// bad, one 33
#define SEQ_F 40, 11, 44, 20, 30, 30, 22, 44, 20, 22, 10, 44, 44, 40, 40, 20, 33, 40, 20, 30

// bad, two 33s
#define SEQ_G 40, 11, 44, 20, 30, 30, 22, 44, 33, 22, 10, 44, 44, 40, 40, 20, 33, 40, 20, 30

// bad, four 33s (consumed a 20)
#define SEQ_H 40, 11, 44, 33, 30, 30, 22, 44, 33, 22, 10, 44, 44, 40, 40, 33, 33, 40, 20, 30

// bad, five 33s (consumed both 20s)
#define SEQ_I 40, 11, 44, 33, 30, 30, 22, 44, 33, 22, 10, 44, 44, 40, 40, 33, 33, 40, 33, 30

    test_helper4({SEQ_C}, {SEQ_D}, true);
    test_helper4({SEQ_C}, {SEQ_E}, false);
    test_helper4({SEQ_C}, {SEQ_F}, false);
    test_helper4({SEQ_C}, {SEQ_G}, false);
    test_helper4({SEQ_C}, {SEQ_H}, false);
    test_helper4({SEQ_C}, {SEQ_I}, false);

#undef SEQ_C
#undef SEQ_D
#undef SEQ_E
#undef SEQ_F
#undef SEQ_G
#undef SEQ_H
#undef SEQ_I
}
