// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <utility>
#define TEST_NAME "<utility>"
#define _SILENCE_CXX20_REL_OPS_DEPRECATION_WARNING

#include "tdefs.h"
#include <tuple>
#include <utility>

typedef STD pair<int, char> Pair_ic;
Pair_ic p0;

void fun() { // do nothing
}

void test_main() { // test basic workings of utility definitions
    Pair_ic p1 = p0, p2(3, 'a');

    CHECK_INT(p1.first, 0);
    CHECK_INT(p1.second, '\0');
    CHECK_INT(p2.first, 3);
    CHECK_INT(p2.second, 'a');

    // test get
    CHECK_INT(STD get<0>(p1), 0);
    CHECK_INT(STD get<1>(p1), '\0');
    CHECK_INT(STD get<int>(p1), 0);
    CHECK_INT(STD get<char>(p1), '\0');

    CHECK_INT(STD get<0>(p2), 3);
    CHECK_INT(STD get<1>(p2), 'a');
    CHECK_INT(STD get<int>(p2), 3);
    CHECK_INT(STD get<char>(p2), 'a');

    CHECK(p2 == STD make_pair((Pair_ic::first_type) 3, (Pair_ic::second_type) 'a'));
    CHECK(p2 < STD make_pair((Pair_ic::first_type) 4, (Pair_ic::second_type) 'a'));
    CHECK(p2 < STD make_pair((Pair_ic::first_type) 3, (Pair_ic::second_type) 'b'));
    CHECK(p1 != p2);
    CHECK(p2 > p1);
    CHECK(p2 <= p2);
    CHECK(p2 >= p2);

    // test rel_ops
    using namespace STD rel_ops;
    Copyable_int a(2), b(3);
    CHECK(a != b);
    CHECK(b > a);
    CHECK(a <= b);
    CHECK(b >= a);

    STD swap(p1, p2);
    CHECK_INT(p1.first, 3);
    CHECK_INT(p1.second, 'a');

    {
        int arr[3];
        CHECK_PTR(STD make_pair(&arr[0], 3).first, &arr[0]);
        CHECK(STD make_pair(3, &fun).second == &fun);

        typedef STD pair<long, short> Pair_ls;
        Pair_ls p8(30L, (short) 7);
        Pair_ic p9(4, 'b');
        p8 = p9;
        CHECK_INT(p8.first, 4);
        CHECK_INT(p8.second, 'b');
    }

    {
        // test forward, move

        Movable_int mi0(1);
        Movable_int mi1(STD move(mi0));
        CHECK_INT(mi0.val, -1);
        CHECK_INT(mi1.val, 1);

        int x = 1;
        int y = 0;
        y     = STD forward<int>(x);
        CHECK_INT(y, 1);

        Pair_ic p3(STD move(2), STD move('c'));
        Pair_ic p4(STD move(p3));
        CHECK_INT(p4.first, 2);
        CHECK_INT(p4.second, 'c');
        p4 = STD move(p1);
        CHECK_INT(p4.first, 3);
        CHECK_INT(p4.second, 'a');

        STD pair<Movable_int, Movable_int> p5(STD move(2), STD move(3));
        CHECK_INT(p5.first.val, 2);
        CHECK_INT(p5.second.val, 3);

        STD pair<Movable_int, Movable_int> p6(STD move(p5));
        CHECK_INT(p5.first.val, -1);
        CHECK_INT(p5.second.val, -1);
        CHECK_INT(p6.first.val, 2);
        CHECK_INT(p6.second.val, 3);

        p5 = STD move(p6);
        CHECK_INT(p5.first.val, 2);
        CHECK_INT(p5.second.val, 3);
        CHECK_INT(p6.first.val, -1);
        CHECK_INT(p6.second.val, -1);

        int two   = 2;
        int three = 3;

        STD piecewise_construct_t pc = STD piecewise_construct;
        STD pair<Movable_int, Movable_int> p7(pc, STD tuple<int>(1), STD forward_as_tuple(two, three));
        CHECK_INT(p7.first.val, 1);
        CHECK_INT(p7.second.val, 0x23);

        {
            char first[8];
            CSTD strcpy(first, "abcdefg");
            STD swap(first[0], first[1]);
            CHECK_STR(first, "bacdefg");

            char arr1[] = {"abc"};
            char arr2[] = {"def"};
            STD swap(arr1, arr2);
            CHECK_STR(&arr1[0], "def");
            CHECK_STR(&arr2[0], "abc");
        }
    }
}
