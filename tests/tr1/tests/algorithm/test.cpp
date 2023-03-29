// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <algorithm>
#define TEST_NAME "<algorithm>"

#define _HAS_AUTO_PTR_ETC                1
#define _HAS_DEPRECATED_ADAPTOR_TYPEDEFS 1

#include "tdefs.h"
#include <algorithm>
#include <ctype.h>
#include <functional>
#include <string.h>

// FUNCTION OBJECTS
STD equal_to<char> equf;
STD less<char> lessf;

bool cmp_chars(char c1, char c2) { // compare caseless for equality
    return CSTD tolower(c1) == CSTD tolower(c2);
}

void test_single(char* first, char*) { // test single-element template functions
    CHECK_INT(STD max('0', '2'), '2');
    CHECK_INT(STD max('0', '2', lessf), '2');

    CHECK_INT(STD min('0', '2'), '0');
    CHECK_INT(STD min('0', '2', lessf), '0');

    CHECK_INT(STD minmax('0', '2').first, '0');
    CHECK_INT(STD minmax('0', '2').second, '2');
    CHECK_INT(STD minmax('0', '2', lessf).first, '0');
    CHECK_INT(STD minmax('0', '2', lessf).second, '2');

    STD initializer_list<char> ilist1{'0'};
    STD initializer_list<char> ilist2{'1', '2'};
    STD initializer_list<char> ilist3{'0', '1', '2'};

    CHECK_INT(STD max(ilist1), '0');
    CHECK_INT(STD max(ilist3), '2');
    CHECK_INT(STD max(ilist1, lessf), '0');
    CHECK_INT(STD max(ilist3, lessf), '2');

    CHECK_INT(STD min(ilist2), '1');
    CHECK_INT(STD min(ilist3), '0');
    CHECK_INT(STD min(ilist2, lessf), '1');
    CHECK_INT(STD min(ilist3, lessf), '0');

    CHECK_INT(STD minmax(ilist1).first, '0');
    CHECK_INT(STD minmax(ilist1).second, '0');
    CHECK_INT(STD minmax(ilist3).first, '0');
    CHECK_INT(STD minmax(ilist3).second, '2');
    CHECK_INT(STD minmax(ilist1, lessf).first, '0');
    CHECK_INT(STD minmax(ilist1, lessf).second, '0');
    CHECK_INT(STD minmax(ilist3, lessf).first, '0');
    CHECK_INT(STD minmax(ilist3, lessf).second, '2');

    CSTD strcpy(first, "abcdefg");
    STD swap(first[0], first[1]);
    CHECK_STR(first, "bacdefg");
    STD iter_swap(&first[0], &first[1]);
    CHECK_STR(first, "abcdefg");

    {
        Movable_int mi0(0);
        Movable_int mi1(1);
        STD swap(mi0, mi1);
        CHECK_INT(mi0.val, 1);
        CHECK_INT(mi1.val, 0);
    }

    {
        char arr1[] = {"abc"};
        char arr2[] = {"def"};
        STD swap(arr1, arr2);
        CHECK_STR(&arr1[0], "def");
        CHECK_STR(&arr2[0], "abc");
    }
}

void test_find(char* first, char* last) { // test searching template functions
    CSTD strcpy(first, "abccefg");
    CHECK_INT(*STD max_element(first, last), 'g');
    CHECK_INT(*STD max_element(first, last, lessf), 'g');
    CHECK_INT(*STD min_element(first, last), 'a');
    CHECK_INT(*STD min_element(first, last, lessf), 'a');

    CHECK_INT(*STD minmax_element(first, last).second, 'g');
    CHECK_INT(*STD minmax_element(first, last, lessf).second, 'g');
    CHECK_INT(*STD minmax_element(first, last).first, 'a');
    CHECK_INT(*STD minmax_element(first, last, lessf).first, 'a');

    CHECK(STD is_permutation(first, last, first));
    CHECK(STD is_permutation(first, last, "abcgfec"));
    CHECK(!STD is_permutation(first, last, "abcgfed"));

    CHECK(STD is_permutation(first, last, first, &cmp_chars));
    CHECK(STD is_permutation(first, last, "aBCgfec", &cmp_chars));
    CHECK(!STD is_permutation(first, last, "abcgfed", &cmp_chars));

    CHECK(STD is_permutation(first, last, // added with C++14
        first, last));
    const char* p1 = "abcgfecxx";
    CHECK(STD is_permutation(first, last, p1, p1 + 7));
    CHECK(!STD is_permutation(first, last, p1, p1 + CSTD strlen(p1)));
    const char* p2 = "abcgfedxx";
    CHECK(!STD is_permutation(first, last, p2 + 7));
    CHECK(!STD is_permutation(first, last, p2 + CSTD strlen(p2)));

    CHECK(STD is_permutation(first, last, first, last, &cmp_chars));
    const char* p3 = "aBCgfecxx";
    CHECK(STD is_permutation(first, last, p3, p3 + 7, &cmp_chars));
    CHECK(!STD is_permutation(first, last, p3, p3 + CSTD strlen(p3), &cmp_chars));
    const char* p4 = "abcgfedxx";
    CHECK(!STD is_permutation(first, last, p4, p4 + 7, &cmp_chars));
    CHECK(!STD is_permutation(first, last, p4, p4 + CSTD strlen(p4), &cmp_chars));

    CHECK(STD equal(first, last, first));
    CHECK(STD equal(first, last, first, equf));

    CHECK(STD equal(first, last, first, last)); // added with C++14
    CHECK(STD equal(first, last, first, last, equf));

    CHECK(STD lexicographical_compare(first, last - 1, first, last));
    CHECK(STD lexicographical_compare(first, last - 1, first, last, lessf));

    CHECK_PTR(STD mismatch(first, last, first).second, last);
    CHECK_PTR(STD mismatch(first, last, first, equf).second, last);

    CHECK_PTR(STD mismatch(first, last, first, last).second, last); // added with C++14
    CHECK_PTR(STD mismatch(first, last, first, last, equf).second, last);

    char array[5] = {"abcd"};
    CHECK(STD equal(array, array + 4, array));
    CHECK(STD equal(array, array + 4, array, equf));
    CHECK_PTR(STD mismatch(array, array + 4, array).second, array + 4);
    CHECK_PTR(STD mismatch(array, array + 4, array, equf).second, array + 4);

    CHECK_PTR(STD find(first, last, 'c'), first + 2);
    CHECK_PTR(STD find_if(first, last, STD bind2nd(equf, 'c')), first + 2);
    CHECK_PTR(STD adjacent_find(first, last), first + 2);
    CHECK_PTR(STD adjacent_find(first, last, equf), first + 2);
    CHECK_INT(STD count(first, last, 'c'), 2);
    CHECK_INT(STD count_if(first, last, STD bind2nd(equf, 'c')), 2);
    CHECK_PTR(STD search(first, last, first + 2, last), first + 2);
    CHECK_PTR(STD search(first, last, first + 2, last, equf), first + 2);
    CHECK_PTR(STD search_n(first, last, 2, 'c'), first + 2);
    CHECK_PTR(STD search_n(first, last, 2, 'c', equf), first + 2);
    CHECK_PTR(STD find_end(first, last, first + 2, last), first + 2);
    CHECK_PTR(STD find_end(first, last, first + 2, last, equf), first + 2);
    CHECK_PTR(STD find_first_of(first, last, first + 2, last), first + 2);
    CHECK_PTR(STD find_first_of(first, last, first + 2, last, equf), first + 2);

    CSTD strcpy(first, "aabb");
    CHECK(STD all_of(first, first + 2, STD bind2nd(equf, 'a')));
    CHECK(!STD all_of(first, first + 3, STD bind2nd(equf, 'a')));
    CHECK(!STD any_of(first, first + 2, STD bind2nd(equf, 'b')));
    CHECK(STD any_of(first, first + 3, STD bind2nd(equf, 'b')));
    CHECK(!STD none_of(first, first + 2, STD bind2nd(equf, 'a')));
    CHECK(STD none_of(first, first + 2, STD bind2nd(equf, 'b')));
    CHECK_PTR(STD find_if_not(first, last, STD bind2nd(equf, 'a')), first + 2);
}

// TEST GENERATING TEMPLATE FUNCTIONS
CSTD size_t gen_count = 0;
void count_c(char ch) { // count calls with value 'c'
    if (ch == 'c') {
        ++gen_count;
    }
}

char gen_x() { // return literal x
    return 'x';
}

void test_generate(char* first, char* last, char* dest) { // test generating template functions
    STD plus<char> plusf;

    CSTD strcpy(first, "abccefg");
    STD for_each(first, last, &count_c);
    CHECK_INT(gen_count, 2);
    STD generate(first, first + 2, &gen_x);
    CHECK_STR(first, "xxccefg");
    CHECK_PTR(STD generate_n(first + 3, last - first - 3, &gen_x), last);
    CHECK_STR(first, "xxcxxxx");
    CHECK_PTR(STD transform(first, last, dest, STD bind2nd(plusf, '\1')), dest + 7);
    CHECK_STR(dest, "yydyyyy");
    CHECK_PTR(STD transform(first, last, "\1\1\1\2\1\1\1", dest, plusf), dest + 7);
    CHECK_STR(dest, "yydzyyy");

    char array1[5] = {""};
    char array2[5] = {"\1\2\3\4"};
    CSTD strcpy(first, "abcd");
    CHECK_PTR(STD generate_n(array1, 4, &gen_x), &array1[4]);
    CHECK_STR(array1, "xxxx");
    CHECK_PTR(STD transform(first, first + 4, array1, STD bind2nd(plusf, '\1')), array1 + 4);
    CHECK_STR(array1, "bcde");
    CHECK_PTR(STD transform(first, first + 4, &array2[0], array1, plusf), array1 + 4);
    CHECK_PTR(STD transform(first, first + 4, array2, &array1[0], plusf), array1 + 4);
    CHECK_PTR(STD transform(first, first + 4, array2, array1, plusf), array1 + 4);
    CHECK_STR(array1, "bdfh");
}

void test_copy(char* first, char* last, char* dest) { // test copying template functions
    char array1[5];
    char array2[5];

    CSTD strcpy(array1, "xxxx");
    CSTD strcpy(array2, "xxxx");

    CSTD strcpy(first, "abcdefg");
    CHECK_PTR(STD copy(first, first + 3, first + 3), first + 6);
    CHECK_STR(first, "abcabcg");
    CHECK_PTR(STD copy(first, first + 4, array2), array2 + 4);
    CHECK_STR(array2, "abca");

    CHECK_PTR(STD copy_backward(first + 4, first + 7, first + 3), first);
    CHECK_STR(first, "bcgabcg");
    CHECK_PTR(STD copy_backward(first, first + 4, array2 + 4), array2);
    CHECK_STR(array2, "bcga");

    STD fill(first, first + 3, 'x');
    CHECK_STR(first, "xxxabcg");
    CSTD strcpy(array2, "xxxx");
    STD fill(array2, array2 + 4, 'z');
    CHECK_STR(array2, "zzzz");
    CHECK_PTR(STD fill_n(first, 2, 'y'), first + 2);
    CHECK_STR(first, "yyxabcg");
    CHECK_PTR(STD fill_n(array2, 4, 'w'), &array2[4]);
    CHECK_STR(array2, "wwww");

    STD swap_ranges(first, first + 3, first + 3);
    CHECK_STR(first, "abcyyxg");

    STD replace(first, last, 'y', 'c');
    CHECK_STR(first, "abcccxg");
    STD replace_if(first, last, STD bind2nd(equf, 'c'), 'z');
    CHECK_STR(first, "abzzzxg");
    STD replace_copy(first, last, dest, 'z', 'c');
    CHECK_STR(dest, "abcccxg");
    STD replace_copy_if(first, last, dest, STD bind2nd(equf, 'z'), 'y');
    CHECK_STR(dest, "abyyyxg");

    CSTD strcpy(array2, "xxxx");
    CSTD strcpy(first, "abcdefg");
    STD swap_ranges(first, first + 3, array2);
    CHECK_STR(first, "xxxdefg");
    CHECK_STR(array2, "abcx");
    STD replace_copy(first, first + 4, array2, 'x', 'c');
    CHECK_STR(array2, "cccd");
    STD replace_copy_if(first, first + 4, array2, STD bind2nd(equf, 'x'), 'y');
    CHECK_STR(array2, "yyyd");

    {
        CSTD strcpy(first, "abcdefg");
        CHECK_PTR(STD move(first, first + 3, first + 3), first + 6);
        CHECK_STR(first, "abcabcg");
        CSTD strcpy(array2, "xxxx");
        CHECK_PTR(STD move(first, first + 4, array2), array2 + 4);
        CHECK_STR(array2, "abca");

        CHECK_PTR(STD move_backward(first + 4, first + 7, first + 3), first);
        CHECK_STR(first, "bcgabcg");
        CHECK_PTR(STD move_backward(first, first + 4, array2 + 4), array2);
        CHECK_STR(array2, "bcga");

        Movable_int mi[2];
        CHECK_PTR(STD move(&mi[0], &mi[1], &mi[1]), &mi[2]);
        CHECK_INT(mi[0].val, -1);
        CHECK_INT(mi[1].val, 0);

        CHECK_PTR(STD move_backward(&mi[1], &mi[2], &mi[1]), &mi[0]);
        CHECK_INT(mi[0].val, 0);
        CHECK_INT(mi[1].val, -1);
    }

    CSTD strcpy(first, "aabbxxx");
    CHECK_PTR(STD copy_n(first, 2, first + 2), first + 4);
    CHECK_STR(first, "aaaaxxx");

    CSTD strcpy(array1, "aabb");
    CSTD strcpy(array2, "xxxx");
    CHECK_PTR(STD copy_n(&array1[0], 4, array2), array2 + 4);
    CHECK_PTR(STD copy_n(array1, 4, &array2[0]), array2 + 4);
    CHECK_PTR(STD copy_n(array1, 4, array2), array2 + 4);
    CHECK_STR(array2, "aabb");

    CSTD strcpy(first, "ababxxx");
    CHECK_PTR(STD copy_if(first, first + 4, first + 4, STD bind2nd(equf, 'a')), first + 6);
    CHECK_STR(first, "ababaax");
    CSTD strcpy(array2, "xxxx");
    CHECK_PTR(STD copy_if(first, first + 4, array2, STD bind2nd(equf, 'a')), array2 + 2);
    CHECK_STR(array2, "aaxx");
}

CSTD size_t frand(CSTD size_t nmax) { // return random value in [0, nmax)
    return CSTD rand() % nmax;
}

struct rand_gen { // uniform random number generator
    typedef CSTD size_t result_type;

    result_type operator()() { // get random value
        return CSTD rand() & 0xfffff;
    }

    static result_type(min)() { // get minimum value
        return 0;
    }

    static result_type(max)() { // get maximum value
        return 0xfffff;
    }
};

struct equ_mi1 { // test if Movable_int == 1
    bool operator()(const Movable_int& left) { // test
        return left.val == 1;
    }
};

struct equ_mi2 { // test if Movable_ints are equal
    bool operator()(const Movable_int& left,
        const Movable_int& right) { // test
        return left.val == right.val;
    }
};

struct lt_mi2 { // test if Movable_ints are ordered
    bool operator()(const Movable_int& left,
        const Movable_int& right) { // test
        return left.val < right.val;
    }
};

struct gt_mi2 { // test if Movable_ints are reverse ordered
    bool operator()(const Movable_int& left,
        const Movable_int& right) { // test
        return right.val < left.val;
    }
};

void test_mutate(char* first, char* last, char* dest) { // test mutating template functions
    char array[5] = {"xxxx"};

    CSTD strcpy(first, "abcdefg");
    CSTD strcpy(dest, first);
    (void) STD remove(first, last, 'c');
    CHECK_STR(first, "abdefgg");
    (void) STD remove_if(first, last, STD bind2nd(equf, 'd'));
    CHECK_STR(first, "abefggg");
    STD remove_copy(first, last, dest, 'e');
    CHECK_STR(dest, "abfgggg");
    STD remove_copy_if(first, last, dest, STD bind2nd(equf, 'e'));
    CHECK_STR(dest, "abfgggg");

    STD remove_copy(first, first + 4, array, 'b');
    CHECK_STR(array, "aefx");
    STD remove_copy_if(first, first + 4, array, STD bind2nd(equf, 'a'));
    CHECK_STR(array, "befx");

    {
        Movable_int mi1(1);
        Movable_int mia1[3];
        mia1[1].val = 1;
        (void) STD remove(&mia1[0], &mia1[3], mi1);
        CHECK_INT(mia1[0].val, 0);
        CHECK_INT(mia1[1].val, 0);
        CHECK_INT(mia1[2].val, -1);

        Movable_int mia2[3];
        mia2[1].val = 1;
        (void) STD remove_if(&mia2[0], &mia2[3], equ_mi1());
        CHECK_INT(mia2[0].val, 0);
        CHECK_INT(mia2[1].val, 0);
        CHECK_INT(mia2[2].val, -1);
    }

    (void) STD unique(dest, dest + 8);
    CHECK_STR(dest, "abfg");
    (void) STD unique(dest, dest + 5, lessf);
    CHECK_STR(dest, "a");
    STD unique_copy(first, last + 1, dest);
    CHECK_STR(dest, "abefg");
    STD unique_copy(first, last + 1, dest, equf);
    CHECK_STR(dest, "abefg");

    CSTD strcpy(first, "aabbbcc");
    CSTD strcpy(array, "xxxx");
    STD unique_copy(first, last, array);
    CHECK_STR(array, "abcx");
    CSTD strcpy(array, "xxxx");
    STD unique_copy(first, last, array, equf);
    CHECK_STR(array, "abcx");

    {
        Movable_int mi1(1);
        Movable_int mia1[3];
        mia1[2].val = 1;
        (void) STD unique(&mia1[0], &mia1[3]);
        CHECK_INT(mia1[0].val, 0);
        CHECK_INT(mia1[1].val, 1);
        CHECK_INT(mia1[2].val, -1);

        Movable_int mia2[3];
        mia2[2].val = 1;
        (void) STD unique(&mia2[0], &mia2[3], equ_mi2());
        CHECK_INT(mia2[0].val, 0);
        CHECK_INT(mia2[1].val, 1);
        CHECK_INT(mia2[2].val, -1);
    }

    CSTD strcpy(first, "abefggg");
    STD reverse(first, last);
    CHECK_STR(first, "gggfeba");
    STD reverse_copy(first, last, dest);
    CHECK_STR(dest, "abefggg");
    STD rotate(first, first + 2, last);
    CHECK_STR(first, "gfebagg");
    STD rotate_copy(first, first + 2, last, dest);
    CHECK_STR(dest, "ebagggf");

    CSTD strcpy(array, "xxxx");
    STD reverse_copy(first, first + 4, array);
    CHECK_STR(array, "befg");
    CSTD strcpy(array, "xxxx");
    STD rotate_copy(first, first + 2, first + 4, array);
    CHECK_STR(array, "ebgf");

    STD random_shuffle(first, last);
    CSTD size_t (*prand)(CSTD size_t) = &frand;
    STD random_shuffle(first, last, prand);

    rand_gen urng;
    STD shuffle(first, last, urng);

    {
        Movable_int mi1(1);
        Movable_int mia1[3];
        mia1[0].val = 0;
        mia1[1].val = 1;
        mia1[2].val = 2;
        STD reverse(&mia1[0], &mia1[3]);
        CHECK_INT(mia1[0].val, 2);
        CHECK_INT(mia1[1].val, 1);
        CHECK_INT(mia1[2].val, 0);

        Movable_int mia2[3];
        mia2[0].val = 0;
        mia2[1].val = 1;
        mia2[2].val = 2;
        STD rotate(&mia2[0], &mia2[1], &mia2[3]);
        CHECK_INT(mia2[0].val, 1);
        CHECK_INT(mia2[1].val, 2);
        CHECK_INT(mia2[2].val, 0);
    }
}

bool cmp_caseless(char c1, char c2) { // compare as lowercase
    return CSTD tolower(c1) < CSTD tolower(c2);
}

void test_order(char* first, char* last, char* dest) { // test ordering template functions
    STD greater<char> greatf;

    CSTD strcpy(first, "gfedcba");
    STD stable_partition(first, last, STD bind2nd(lessf, 'd'));
    CHECK_STR(first, "cbagfed");
    CHECK_PTR(STD partition(first, last, STD bind2nd(equf, 'd')), first + 1);
    STD sort(first, last);
    CHECK_STR(first, "abcdefg");
    STD sort(first, last, greatf);
    CHECK_STR(first, "gfedcba");

    {
        CSTD strcpy(first, "gfedcba");
        STD pair<char*, char*> ans = STD partition_copy(first, last, dest, dest + 3, STD bind2nd(lessf, 'd'));
        CHECK_PTR(ans.first, dest + 3);
        CHECK_PTR(ans.second, dest + 7);
        CHECK_STR(dest, "cbagfed");

        char array1[5] = {"xxxx"};
        char array2[5] = {"xxxx"};
        ans            = STD partition_copy(first, last, &array1[0], array2, STD bind2nd(lessf, 'd'));
        ans            = STD partition_copy(first, last, array1, &array2[0], STD bind2nd(lessf, 'd'));
        ans            = STD partition_copy(first, last, array1, array2, STD bind2nd(lessf, 'd'));
        CHECK_PTR(ans.first, &array1[3]);
        CHECK_PTR(ans.second, &array2[4]);
        CHECK_STR(array1, "cbax");
        CHECK_STR(array2, "gfed");

        CHECK(!STD is_partitioned(first, last, STD bind2nd(lessf, 'd')));
        CHECK(STD is_partitioned(dest, dest + 7, STD bind2nd(lessf, 'd')));
        CHECK_PTR(STD partition_point(dest, dest + 7, STD bind2nd(lessf, 'd')), dest + 3);
    }

    {
        Movable_int mi1(1);
        Movable_int mia1[3];
        mia1[1].val = 1;
        STD stable_partition(&mia1[0], &mia1[3], equ_mi1());
        CHECK_INT(mia1[0].val, 1);
        CHECK_INT(mia1[1].val, 0);
        CHECK_INT(mia1[2].val, 0);
        STD partition(&mia1[0], &mia1[3], equ_mi1());
        CHECK_INT(mia1[0].val, 1);
        CHECK_INT(mia1[1].val, 0);
        CHECK_INT(mia1[2].val, 0);

        Movable_int mia2[3];
        mia2[0].val = 1;
        mia2[1].val = 0;
        mia2[2].val = 2;
        STD sort(&mia2[0], &mia2[3]);
        CHECK_INT(mia2[0].val, 0);
        CHECK_INT(mia2[1].val, 1);
        CHECK_INT(mia2[2].val, 2);

        STD sort(&mia2[0], &mia2[3], gt_mi2());
        CHECK_INT(mia2[0].val, 2);
        CHECK_INT(mia2[1].val, 1);
        CHECK_INT(mia2[2].val, 0);
    }

    CSTD strcpy(first, "gfedcba");
    STD partial_sort(first, first + 2, last);
    CHECK_INT(first[0], 'a');
    CHECK_INT(first[1], 'b');
    STD partial_sort(first, first + 2, last, greatf);
    CHECK_INT(first[0], 'g');
    CHECK_INT(first[1], 'f');
    STD stable_sort(first, last);
    CHECK_STR(first, "abcdefg");
    STD rotate(first, first + 2, last);
    STD inplace_merge(first, last - 2, last);
    CHECK_STR(first, "abcdefg");
    STD rotate(first, first + 2, last);
    STD inplace_merge(first, last - 2, last, lessf);
    CHECK_STR(first, "abcdefg");

    {
        Movable_int mi1(1);
        Movable_int mia1[3];
        mia1[0].val = 2;
        mia1[1].val = 1;
        mia1[2].val = 0;
        STD partial_sort(&mia1[0], &mia1[2], &mia1[3]);
        CHECK_INT(mia1[0].val, 0);
        CHECK_INT(mia1[1].val, 1);
        CHECK_INT(mia1[2].val, 2);
        STD partial_sort(&mia1[0], &mia1[2], &mia1[3], gt_mi2());
        CHECK_INT(mia1[0].val, 2);
        CHECK_INT(mia1[1].val, 1);
        CHECK_INT(mia1[2].val, 0);

        Movable_int mia2[3];
        mia2[0].val = 1;
        mia2[1].val = 0;
        mia2[2].val = 2;
        STD stable_sort(&mia2[0], &mia2[3]);
        CHECK_INT(mia2[0].val, 0);
        CHECK_INT(mia2[1].val, 1);
        CHECK_INT(mia2[2].val, 2);

        STD stable_sort(&mia2[0], &mia2[3], gt_mi2());
        CHECK_INT(mia2[0].val, 2);
        CHECK_INT(mia2[1].val, 1);
        CHECK_INT(mia2[2].val, 0);
    }

    {
        CSTD strcpy(dest, "tuvwxyz");
        STD partial_sort_copy(first, last, dest, dest + 1);
        CHECK_STR(dest, "auvwxyz");
        STD partial_sort_copy(first, last, dest, dest + 1, greatf);
        CHECK_STR(dest, "guvwxyz");
        STD nth_element(first, first + 2, last, greatf);
        CHECK_INT(first[2], 'e');
        STD nth_element(first, first + 2, last);
        CHECK_INT(first[2], 'c');

        CSTD strcpy(first, "dCcbBba");
        STD stable_sort(first, last, &cmp_caseless);
        CHECK_STR(first, "abBbCcd");
        STD stable_sort(first, last);
        CHECK_STR(first, "BCabbcd");
        STD merge(first + 2, last, first, first + 2, dest);
        CHECK_STR(dest, "BCabbcd");
        STD merge(first + 2, last, first, first + 2, dest, &cmp_caseless);
        CHECK_STR(dest, "abbBcCd");

        char array[5] = {"xxxx"};
        STD merge(first + 2, first + 4, first, first + 2, array);
        CHECK_STR(array, "BCab");
        STD merge(first + 2, first + 4, first + 4, first + 6, array, &cmp_caseless);
        CHECK_STR(array, "abbc");
    }

    {
        Movable_int mi1(1);
        Movable_int mia1[3];
        mia1[0].val = 1;
        mia1[1].val = 2;
        mia1[2].val = 0;
        STD nth_element(&mia1[0], &mia1[1], &mia1[3]);
        CHECK_INT(mia1[1].val, 1);
        STD nth_element(&mia1[0], &mia1[0], &mia1[3], gt_mi2());
        CHECK_INT(mia1[0].val, 2);

        Movable_int mia2[3];
        mia2[0].val = 1;
        mia2[1].val = 0;
        mia2[2].val = 2;
        STD inplace_merge(&mia2[0], &mia2[1], &mia2[3]);
        CHECK_INT(mia2[0].val, 0);
        CHECK_INT(mia2[1].val, 1);
        CHECK_INT(mia2[2].val, 2);

        mia2[0].val = 1;
        mia2[1].val = 0;
        mia2[2].val = 2;
        STD inplace_merge(&mia2[0], &mia2[2], &mia2[3], gt_mi2());
        CHECK_INT(mia2[0].val, 2);
        CHECK_INT(mia2[1].val, 1);
        CHECK_INT(mia2[2].val, 0);
    }

    STD sort(first, last);
    CHECK(STD is_sorted(first, last));
    CHECK(STD is_sorted_until(first, last) == last);
    STD sort(first, last, lessf);
    CHECK(STD is_sorted(first, last, lessf));
    CHECK(STD is_sorted_until(first, last, lessf) == last);
}

void test_search(char* first, char* last) { // test searching template functions
    char val = 'c';

    CSTD strcpy(first, "abcccfg");
    CHECK_PTR(STD lower_bound(first, last, val), first + 2);
    CHECK_PTR(STD lower_bound(first, last, val, lessf), first + 2);
    CHECK_PTR(STD upper_bound(first, last, val), first + 5);
    CHECK_PTR(STD upper_bound(first, last, val, lessf), first + 5);
    CHECK_PTR(STD equal_range(first, last, val).first, first + 2);
    CHECK_PTR(STD equal_range(first, last, val, lessf).second, first + 5);
    CHECK(STD binary_search(first, last, val));
    CHECK(STD binary_search(first, last, val, lessf));
    CHECK(STD includes(first, last, first + 3, last));
    CHECK(STD includes(first, last, first + 3, last, lessf));
}

void test_set(char* first, char* last, char* dest) { // test set template functions
    CSTD strcpy(first, "abccefg");
    CSTD strcpy(dest, first);
    STD set_union(first, first + 3, first + 3, last, dest);
    CHECK_STR(dest, "abcefgg");
    STD set_union(first, first + 3, first + 3, last, dest, lessf);
    CHECK_STR(dest, "abcefgg");
    STD set_intersection(first, first + 3, first + 3, last, dest);
    CHECK_STR(dest, "cbcefgg");
    STD set_intersection(first, first + 3, first + 3, last, dest, lessf);
    CHECK_STR(dest, "cbcefgg");
    STD set_difference(first, first + 3, first + 3, last, dest);
    CHECK_STR(dest, "abcefgg");
    STD set_difference(first, first + 3, first + 3, last, dest, lessf);
    CHECK_STR(dest, "abcefgg");
    STD set_symmetric_difference(first, first + 3, first + 3, last, dest);
    CHECK_STR(dest, "abefggg");
    STD set_symmetric_difference(first, first + 3, first + 3, last, dest, lessf);
    CHECK_STR(dest, "abefggg");

    CSTD strcpy(first, "abbc");
    char array[5] = {"xxxx"};
    STD set_union(first, first + 2, first + 2, first + 4, array);
    CHECK_STR(array, "abcx");
    STD set_union(first, first + 2, first + 2, first + 4, array, lessf);
    CHECK_STR(array, "abcx");
    CSTD strcpy(array, "xxxx");
    STD set_intersection(first, first + 2, first + 2, first + 4, array);
    CHECK_STR(array, "bxxx");
    STD set_intersection(first, first + 2, first + 2, first + 4, array, lessf);
    CHECK_STR(array, "bxxx");
    CSTD strcpy(array, "xxxx");
    STD set_difference(first, first + 2, first + 2, first + 4, array);
    CHECK_STR(array, "axxx");
    STD set_difference(first, first + 2, first + 2, first + 4, array, lessf);
    CHECK_STR(array, "axxx");
    CSTD strcpy(array, "xxxx");
    STD set_symmetric_difference(first, first + 2, first + 2, first + 4, array);
    CHECK_STR(array, "acxx");
    STD set_symmetric_difference(first, first + 2, first + 2, first + 4, array, lessf);
    CHECK_STR(array, "acxx");
}

void test_heap(char* first, char* last) { // test heap template functions
    CSTD strcpy(first, "abccefg");
    STD make_heap(first, last);
    CHECK_INT(first[0], 'g');
    STD make_heap(first, last, lessf);
    CHECK_INT(first[0], 'g');
    STD pop_heap(first, last);
    CHECK_INT(last[-1], 'g');
    CHECK_INT(first[0], 'f');
    STD pop_heap(first, last - 1, lessf);
    CHECK_INT(last[-2], 'f');
    CHECK_INT(first[0], 'e');

    STD push_heap(first, last - 1);
    CHECK_INT(first[0], 'f');
    STD push_heap(first, last, lessf);
    CHECK_INT(first[0], 'g');
    STD sort_heap(first, last);
    CHECK_STR(first, "abccefg");
    STD make_heap(first, last, lessf);
    STD sort_heap(first, last, lessf);
    CHECK_STR(first, "abccefg");

    {
        Movable_int mi1(1);
        Movable_int mia1[3];
        mia1[0].val = 1;
        mia1[1].val = 2;
        mia1[2].val = 0;
        STD make_heap(&mia1[0], &mia1[3]);
        CHECK_INT(mia1[0].val, 2);
        STD make_heap(&mia1[0], &mia1[3], lt_mi2());
        CHECK_INT(mia1[0], 2);
        STD pop_heap(&mia1[0], &mia1[3]);
        CHECK_INT(mia1[2].val, 2);
        CHECK_INT(mia1[0].val, 1);
        STD pop_heap(&mia1[0], &mia1[2], lt_mi2());
        CHECK_INT(mia1[1].val, 1);
        CHECK_INT(mia1[0].val, 0);

        STD push_heap(&mia1[0], &mia1[2]);
        CHECK_INT(mia1[0], 1);
        STD push_heap(&mia1[0], &mia1[3], lt_mi2());
        CHECK_INT(mia1[0], 2);
        STD sort_heap(&mia1[0], &mia1[3]);
        CHECK_INT(mia1[0], 0);
        CHECK_INT(mia1[1], 1);
        CHECK_INT(mia1[2], 2);
        STD make_heap(&mia1[0], &mia1[3], lt_mi2());
        STD sort_heap(&mia1[0], &mia1[3], lt_mi2());
        CHECK_INT(mia1[0], 0);
        CHECK_INT(mia1[1], 1);
        CHECK_INT(mia1[2], 2);
    }

    STD make_heap(first, last);
    CHECK(STD is_heap(first, last));
    CHECK(STD is_heap_until(first, last) == last);
    STD make_heap(first, last, lessf);
    CHECK(STD is_heap(first, last, lessf));
    CHECK(STD is_heap_until(first, last, lessf) == last);
}

void test_permute(char* first, char* last) { // test permuting template functions
    CSTD strcpy(first, "abcdefg");
    STD next_permutation(first, last);
    CHECK_STR(first, "abcdegf");
    STD next_permutation(first, last, lessf);
    CHECK_STR(first, "abcdfeg");
    STD prev_permutation(first, last);
    CHECK_STR(first, "abcdegf");
    STD prev_permutation(first, last, lessf);
    CHECK_STR(first, "abcdefg");
}

void test_main() { // test basic workings of algorithms
    char buf[]  = "abccefg";
    char dest[] = "1234567";
    char *first = buf, *last = buf + 7;

    test_single(first, last);
    test_find(first, last);
    test_generate(first, last, dest);
    test_copy(first, last, dest);
    test_mutate(first, last, dest);
    test_order(first, last, dest);
    test_search(first, last);
    test_set(first, last, dest);
    test_heap(first, last);
    test_permute(first, last);
}
