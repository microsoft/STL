// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <tuple> header
#define TEST_NAME "<tuple>"

#include "tdefs.h"
#include <array> // for tuple_cat(array)
#include <tuple>

static void tuple0() { // tests for empty tuples
    typedef STD tuple<> tpl;
    tpl t0;
    tpl t2(t0);
    CHECK_INT(0, STD tuple_size<tpl>::value);

    CHECK_INT(t0 == t2, true);
    CHECK_INT(t0 != t2, false);
    CHECK_INT(t0 < t2, false);
    CHECK_INT(t0 <= t2, true);
    CHECK_INT(t0 > t2, false);
    CHECK_INT(t0 >= t2, true);
    CHECK_INT(t0 == t2, true);

    t2 = STD make_tuple();
    CHECK_INT(t0 == t2, true);

    { // test tuple swap
        tpl tx;
        tpl ty;
        STD swap(tx, ty);
        CHECK_INT(tx == ty, true);
    }
}

static void tuple1() { // tests for tuples with one element
    {
        typedef STD tuple<int> tpl;
        tpl t0;
        tpl t1(1);
        tpl t2(STD tuple<short>((short) 1));
        CHECK_INT(1, STD tuple_size<tpl>::value);

        typedef STD tuple_element<0, tpl>::type elt0;
        CHECK_TYPE(elt0, int);
        elt0 val0 = STD get<0>(t2);
        CHECK_INT(val0, 1);

        CHECK_INT(t1 == t2, true);
        CHECK_INT(t1 != t2, false);
        CHECK_INT(t1 < t2, false);
        CHECK_INT(t1 <= t2, true);
        CHECK_INT(t1 > t2, false);
        CHECK_INT(t1 >= t2, true);

        t2 = STD make_tuple(2L);
        CHECK_INT(t2 == STD make_tuple(2), true);
        CHECK_INT(t1 == t2, false);
        CHECK_INT(t1 != t2, true);
        CHECK_INT(t1 < t2, true);
        CHECK_INT(t1 <= t2, true);
        CHECK_INT(t1 > t2, false);
        CHECK_INT(t1 >= t2, false);

        STD tie(val0) = STD make_tuple(2);
        CHECK_INT(val0, 2);

        { // test get
            CHECK_INT(STD get<0>(t0), 0);
            CHECK_INT(STD get<0>(t1), 1);
            CHECK_INT(STD get<0>(t2), 2);

            CHECK_INT(STD get<int>(t0), 0);
            CHECK_INT(STD get<int>(t1), 1);
            CHECK_INT(STD get<int>(t2), 2);
        }
    }

    { // test decay
        int arr[3];
        CHECK_PTR(STD get<0>(STD make_tuple(arr, 3)), &arr[0]);
        CHECK(STD get<1>(STD make_tuple(3, tuple1)) == &tuple1);
    }

    { // test tuple_cat(copy, copy)
        STD tuple<> t0;
        STD tuple<> t1 = STD tuple_cat(t0, t0);
        (void) t1;

        STD tuple<int, int> t2 = STD tuple_cat(STD tuple<int>(3), STD tuple<int>(2));
        CHECK_INT(STD get<0>(t2), 3);
        CHECK_INT(STD get<1>(t2), 2);

        STD tuple<int, int, int> t3 = STD tuple_cat(t2, STD tuple<int>(5));
        CHECK_INT(STD get<0>(t3), 3);
        CHECK_INT(STD get<1>(t3), 2);
        CHECK_INT(STD get<2>(t3), 5);

        STD tuple<int, int, int, int> t4 = STD tuple_cat(STD tuple<int>(6), t3);
        CHECK_INT(STD get<0>(t4), 6);
        CHECK_INT(STD get<1>(t4), 3);
        CHECK_INT(STD get<2>(t4), 2);
        CHECK_INT(STD get<3>(t4), 5);

        STD tuple<int, int, int, int> t5 = STD tuple_cat(t4, t0);
        CHECK_INT(STD get<0>(t5), 6);
        CHECK_INT(STD get<1>(t5), 3);
        CHECK_INT(STD get<2>(t5), 2);
        CHECK_INT(STD get<3>(t5), 5);

        STD tuple<int, int, int, int> t6 = STD tuple_cat(t0, t4);
        CHECK_INT(STD get<0>(t6), 6);
        CHECK_INT(STD get<1>(t6), 3);
        CHECK_INT(STD get<2>(t6), 2);
        CHECK_INT(STD get<3>(t6), 5);

        // test tuple_cat<pair>
        STD pair<int, int> pr(10, 20);
        t2 = STD tuple_cat(pr);
        CHECK_INT(STD get<0>(t2), 10);
        CHECK_INT(STD get<1>(t2), 20);

        t4 = STD tuple_cat(t2, pr);
        CHECK_INT(STD get<2>(t4), 10);
        CHECK_INT(STD get<3>(t4), 20);

        t4 = STD tuple_cat(pr, t2);
        CHECK_INT(STD get<0>(t4), 10);
        CHECK_INT(STD get<1>(t4), 20);

        t4 = STD tuple_cat(pr, pr);
        CHECK_INT(STD get<0>(t4), 10);
        CHECK_INT(STD get<1>(t4), 20);

        // test tuple_cat<array>
        STD array<int, 2> ar;
        ar[0] = 10;
        ar[1] = 20;

        t2 = STD tuple_cat(ar);
        CHECK_INT(STD get<0>(t2), 10);
        CHECK_INT(STD get<1>(t2), 20);

        t4 = STD tuple_cat(t2, ar);
        CHECK_INT(STD get<2>(t4), 10);
        CHECK_INT(STD get<3>(t4), 20);

        t4 = STD tuple_cat(ar, t2);
        CHECK_INT(STD get<0>(t4), 10);
        CHECK_INT(STD get<1>(t4), 20);

        t4 = STD tuple_cat(ar, ar);
        CHECK_INT(STD get<0>(t4), 10);
        CHECK_INT(STD get<1>(t4), 20);
    }

    { // test get
        STD tuple<int, char, long> t10;
        STD get<0>(t10) = 3;
        STD get<1>(t10) = 'a';
        STD get<2>(t10) = 2L;
        CHECK_INT(STD get<int>(t10), 3);
        CHECK_INT(STD get<char>(t10), 'a');
        CHECK_INT(STD get<long>(t10), 2L);
    }

    { // test tuple_cat(copy, move)
        typedef STD tuple<Movable_int, Movable_int> Tuple_mi;
        typedef STD tuple<int, Movable_int, Movable_int> Tuple_imi;

        STD tuple<> t0;
        Tuple_mi t1;
        int two        = 2;
        STD get<0>(t1) = STD move(two);

        Tuple_imi t2(STD tuple_cat(STD tuple<int>(1), STD move(t1)));
        CHECK_INT(STD get<0>(t2), 1);
        CHECK_INT(STD get<1>(t2), 2);
        CHECK_INT(STD get<2>(t2), 0);
        CHECK_INT(STD get<0>(t1), -1);
        CHECK_INT(STD get<1>(t1), -1);

        STD tuple<int> t3(STD tuple_cat(STD tuple<int>(4), STD move(t0)));
        CHECK_INT(STD get<0>(t3), 4);

        STD tuple<int> t4(STD tuple_cat(t0, STD move(t3)));
        CHECK_INT(STD get<0>(t3), 4);
        CHECK_INT(STD get<0>(t4), 4);
    }

    { // test tuple_cat(move, copy)
        typedef STD tuple<Movable_int, Movable_int> Tuple_mi;
        typedef STD tuple<Movable_int, Movable_int, int> Tuple_imi;

        STD tuple<> t0;
        Tuple_mi t1;
        int two        = 2;
        STD get<0>(t1) = STD move(two);

        Tuple_imi t2(STD tuple_cat(STD move(t1), STD tuple<int>(1)));
        CHECK_INT(STD get<0>(t2), 2);
        CHECK_INT(STD get<1>(t2), 0);
        CHECK_INT(STD get<2>(t2), 1);
        CHECK_INT(STD get<0>(t1), -1);
        CHECK_INT(STD get<1>(t1), -1);

        STD tuple<int> t3(STD tuple_cat(STD tuple<int>(4), STD move(t0)));
        CHECK_INT(STD get<0>(t3), 4);

        STD tuple<int> t4(STD tuple_cat(t0, STD move(t3)));
        CHECK_INT(STD get<0>(t3), 4);
        CHECK_INT(STD get<0>(t4), 4);
    }

    { // test tuple_cat(move, move)
        typedef STD tuple<Movable_int, Movable_int> Tuple_mi;
        typedef STD tuple<Movable_int, Movable_int, Movable_int, Movable_int> Tuple_mi4;

        Tuple_mi t0, t1;
        int two        = 2;
        int four       = 4;
        STD get<0>(t0) = STD move(two);
        STD get<1>(t1) = STD move(four);

        Tuple_mi4 t2(STD tuple_cat(STD move(t0), STD move(t1)));
        CHECK_INT(STD get<0>(t2), 2);
        CHECK_INT(STD get<1>(t2), 0);
        CHECK_INT(STD get<2>(t2), 0);
        CHECK_INT(STD get<3>(t2), 4);
        CHECK_INT(STD get<0>(t0), -1);
        CHECK_INT(STD get<1>(t0), -1);
        CHECK_INT(STD get<0>(t1), -1);
        CHECK_INT(STD get<1>(t1), -1);
    }

    { // test tuple swap
        STD tuple<int> tx(1);
        STD tuple<int> ty(2);
        STD swap(tx, ty);
        CHECK_INT(STD get<0>(tx), 2);
        CHECK_INT(STD get<0>(ty), 1);
    }
}

static void tuple2() { // tests for tuples with two elements
    typedef STD tuple<int, int> tpl;
    tpl t0;
    (void) t0;
    tpl t1(1, 2);
    tpl t2(STD tuple<short, short>((short) 1, (short) 2));
    CHECK_INT(2, STD tuple_size<tpl>::value);
    tpl t3(STD make_pair(1, 2));
    CHECK_INT(t3 == t1, true);
    t3 = STD make_pair(2, 3);
    CHECK_INT(t3 == t1, false);

    typedef STD tuple_element<0, tpl>::type elt0;
    typedef STD tuple_element<1, tpl>::type elt1;
    CHECK_TYPE(elt0, int);
    CHECK_TYPE(elt1, int);
    elt0 val0 = STD get<0>(t2);
    elt1 val1 = STD get<1>(t2);
    CHECK_INT(val0, 1);
    CHECK_INT(val1, 2);

    CHECK_INT(t1 == t2, true);
    CHECK_INT(t1 != t2, false);
    CHECK_INT(t1 < t2, false);
    CHECK_INT(t1 <= t2, true);
    CHECK_INT(t1 > t2, false);
    CHECK_INT(t1 >= t2, true);

    t2 = STD make_tuple(2L, 3L);
    CHECK_INT(t2 == STD make_tuple(2, 3), true);
    CHECK_INT(t1 == t2, false);
    CHECK_INT(t1 != t2, true);
    CHECK_INT(t1 < t2, true);
    CHECK_INT(t1 <= t2, true);
    CHECK_INT(t1 > t2, false);
    CHECK_INT(t1 >= t2, false);

    STD tie(val0, val1) = STD make_tuple(2, 3);
    CHECK_INT(val0, 2);
    CHECK_INT(val1, 3);

    { // test tuple swap
        tpl tx(1, 3);
        tpl ty(2, 4);
        STD swap(tx, ty);
        CHECK_INT(STD get<0>(tx), 2);
        CHECK_INT(STD get<1>(tx), 4);
        CHECK_INT(STD get<0>(ty), 1);
        CHECK_INT(STD get<1>(ty), 3);
    }
}

static void tuple3() { // tests for tuples with three elements
    typedef STD tuple<int, int, int> tpl;
    tpl t0;
    (void) t0;
    tpl t1(1, 2, 3);
    tpl t2(STD tuple<short, short, short>((short) 1, (short) 2, (short) 3));
    CHECK_INT(3, STD tuple_size<tpl>::value);

    typedef STD tuple_element<0, tpl>::type elt0;
    typedef STD tuple_element<1, tpl>::type elt1;
    typedef STD tuple_element<2, tpl>::type elt2;
    CHECK_TYPE(elt0, int);
    CHECK_TYPE(elt1, int);
    CHECK_TYPE(elt2, int);
    elt0 val0 = STD get<0>(t2);
    elt1 val1 = STD get<1>(t2);
    elt2 val2 = STD get<2>(t2);
    CHECK_INT(val0, 1);
    CHECK_INT(val1, 2);
    CHECK_INT(val2, 3);

    CHECK_INT(t1 == t2, true);
    CHECK_INT(t1 != t2, false);
    CHECK_INT(t1 < t2, false);
    CHECK_INT(t1 <= t2, true);
    CHECK_INT(t1 > t2, false);
    CHECK_INT(t1 >= t2, true);

    t2 = STD make_tuple(2L, 3L, 4L);
    CHECK_INT(t2 == STD make_tuple(2, 3, 4), true);
    CHECK_INT(t1 == t2, false);
    CHECK_INT(t1 != t2, true);
    CHECK_INT(t1 < t2, true);
    CHECK_INT(t1 <= t2, true);
    CHECK_INT(t1 > t2, false);
    CHECK_INT(t1 >= t2, false);

    STD tie(val0, val1, val2) = STD make_tuple(2, 3, 4);
    CHECK_INT(val0, 2);
    CHECK_INT(val1, 3);
    CHECK_INT(val2, 4);

    { // test tuple swap
        tpl tx(1, 3, 5);
        tpl ty(2, 4, 6);
        STD swap(tx, ty);
        CHECK_INT(STD get<0>(tx), 2);
        CHECK_INT(STD get<1>(tx), 4);
        CHECK_INT(STD get<2>(tx), 6);
        CHECK_INT(STD get<0>(ty), 1);
        CHECK_INT(STD get<1>(ty), 3);
        CHECK_INT(STD get<2>(ty), 5);
    }
}

static void tuple4() { // tests for tuples with four elements
    typedef STD tuple<int, int, int, int> tpl;
    tpl t0;
    (void) t0;
    tpl t1(1, 2, 3, 4);
    tpl t2(STD tuple<short, short, short, short>((short) 1, (short) 2, (short) 3, (short) 4));
    CHECK_INT(4, STD tuple_size<tpl>::value);

    typedef STD tuple_element<0, tpl>::type elt0;
    typedef STD tuple_element<1, tpl>::type elt1;
    typedef STD tuple_element<2, tpl>::type elt2;
    typedef STD tuple_element<3, tpl>::type elt3;
    CHECK_TYPE(elt0, int);
    CHECK_TYPE(elt1, int);
    CHECK_TYPE(elt2, int);
    CHECK_TYPE(elt3, int);
    elt0 val0 = STD get<0>(t2);
    elt1 val1 = STD get<1>(t2);
    elt2 val2 = STD get<2>(t2);
    elt3 val3 = STD get<3>(t2);
    CHECK_INT(val0, 1);
    CHECK_INT(val1, 2);
    CHECK_INT(val2, 3);
    CHECK_INT(val3, 4);

    CHECK_INT(t1 == t2, true);
    CHECK_INT(t1 != t2, false);
    CHECK_INT(t1 < t2, false);
    CHECK_INT(t1 <= t2, true);
    CHECK_INT(t1 > t2, false);
    CHECK_INT(t1 >= t2, true);

    t2 = STD make_tuple(2L, 3L, 4L, 5L);
    CHECK_INT(t2 == STD make_tuple(2, 3, 4, 5), true);
    CHECK_INT(t1 == t2, false);
    CHECK_INT(t1 != t2, true);
    CHECK_INT(t1 < t2, true);
    CHECK_INT(t1 <= t2, true);
    CHECK_INT(t1 > t2, false);
    CHECK_INT(t1 >= t2, false);

    STD tie(val0, val1, val2, val3) = STD make_tuple(2, 3, 4, 5);
    CHECK_INT(val0, 2);
    CHECK_INT(val1, 3);
    CHECK_INT(val2, 4);
    CHECK_INT(val3, 5);

    { // test tuple swap
        tpl tx(1, 3, 5, 7);
        tpl ty(2, 4, 6, 8);
        STD swap(tx, ty);
        CHECK_INT(STD get<0>(tx), 2);
        CHECK_INT(STD get<1>(tx), 4);
        CHECK_INT(STD get<2>(tx), 6);
        CHECK_INT(STD get<3>(tx), 8);
        CHECK_INT(STD get<0>(ty), 1);
        CHECK_INT(STD get<1>(ty), 3);
        CHECK_INT(STD get<2>(ty), 5);
        CHECK_INT(STD get<3>(ty), 7);
    }
}

static void tuple5() { // tests for tuples with five elements
    typedef STD tuple<int, int, int, int, int> tpl;
    tpl t0;
    (void) t0;
    tpl t1(1, 2, 3, 4, 5);
    tpl t2(STD tuple<short, short, short, short, short>((short) 1, (short) 2, (short) 3, (short) 4, (short) 5));
    CHECK_INT(5, STD tuple_size<tpl>::value);

    typedef STD tuple_element<0, tpl>::type elt0;
    typedef STD tuple_element<1, tpl>::type elt1;
    typedef STD tuple_element<2, tpl>::type elt2;
    typedef STD tuple_element<3, tpl>::type elt3;
    typedef STD tuple_element<4, tpl>::type elt4;
    CHECK_TYPE(elt0, int);
    CHECK_TYPE(elt1, int);
    CHECK_TYPE(elt2, int);
    CHECK_TYPE(elt3, int);
    CHECK_TYPE(elt4, int);
    elt0 val0 = STD get<0>(t2);
    elt1 val1 = STD get<1>(t2);
    elt2 val2 = STD get<2>(t2);
    elt3 val3 = STD get<3>(t2);
    elt4 val4 = STD get<4>(t2);
    CHECK_INT(val0, 1);
    CHECK_INT(val1, 2);
    CHECK_INT(val2, 3);
    CHECK_INT(val3, 4);
    CHECK_INT(val4, 5);

    CHECK_INT(t1 == t2, true);
    CHECK_INT(t1 != t2, false);
    CHECK_INT(t1 < t2, false);
    CHECK_INT(t1 <= t2, true);
    CHECK_INT(t1 > t2, false);
    CHECK_INT(t1 >= t2, true);

    t2 = STD make_tuple(2L, 3L, 4L, 5L, 6L);
    CHECK_INT(t2 == STD make_tuple(2, 3, 4, 5, 6), true);
    CHECK_INT(t1 == t2, false);
    CHECK_INT(t1 != t2, true);
    CHECK_INT(t1 < t2, true);
    CHECK_INT(t1 <= t2, true);
    CHECK_INT(t1 > t2, false);
    CHECK_INT(t1 >= t2, false);

    STD tie(val0, val1, val2, val3, val4) = STD make_tuple(2, 3, 4, 5, 6);
    CHECK_INT(val0, 2);
    CHECK_INT(val1, 3);
    CHECK_INT(val2, 4);
    CHECK_INT(val3, 5);
    CHECK_INT(val4, 6);

    { // test tuple swap
        tpl tx(1, 3, 5, 7, 9);
        tpl ty(2, 4, 6, 8, 10);
        STD swap(tx, ty);
        CHECK_INT(STD get<0>(tx), 2);
        CHECK_INT(STD get<1>(tx), 4);
        CHECK_INT(STD get<2>(tx), 6);
        CHECK_INT(STD get<3>(tx), 8);
        CHECK_INT(STD get<4>(tx), 10);
        CHECK_INT(STD get<0>(ty), 1);
        CHECK_INT(STD get<1>(ty), 3);
        CHECK_INT(STD get<2>(ty), 5);
        CHECK_INT(STD get<3>(ty), 7);
        CHECK_INT(STD get<4>(ty), 9);
    }
}

static void tuple6() { // tests for tuples with six elements
    typedef STD tuple<int, int, int, int, int, int> tpl;
    tpl t0;
    (void) t0;
    tpl t1(1, 2, 3, 4, 5, 6);
    tpl t2(STD tuple<short, short, short, short, short, short>(
        (short) 1, (short) 2, (short) 3, (short) 4, (short) 5, (short) 6));
    CHECK_INT(6, STD tuple_size<tpl>::value);

    typedef STD tuple_element<0, tpl>::type elt0;
    typedef STD tuple_element<1, tpl>::type elt1;
    typedef STD tuple_element<2, tpl>::type elt2;
    typedef STD tuple_element<3, tpl>::type elt3;
    typedef STD tuple_element<4, tpl>::type elt4;
    typedef STD tuple_element<5, tpl>::type elt5;
    CHECK_TYPE(elt0, int);
    CHECK_TYPE(elt1, int);
    CHECK_TYPE(elt2, int);
    CHECK_TYPE(elt3, int);
    CHECK_TYPE(elt4, int);
    CHECK_TYPE(elt5, int);
    elt0 val0 = STD get<0>(t2);
    elt1 val1 = STD get<1>(t2);
    elt2 val2 = STD get<2>(t2);
    elt3 val3 = STD get<3>(t2);
    elt4 val4 = STD get<4>(t2);
    elt5 val5 = STD get<5>(t2);
    CHECK_INT(val0, 1);
    CHECK_INT(val1, 2);
    CHECK_INT(val2, 3);
    CHECK_INT(val3, 4);
    CHECK_INT(val4, 5);
    CHECK_INT(val5, 6);

    CHECK_INT(t1 == t2, true);
    CHECK_INT(t1 != t2, false);
    CHECK_INT(t1 < t2, false);
    CHECK_INT(t1 <= t2, true);
    CHECK_INT(t1 > t2, false);
    CHECK_INT(t1 >= t2, true);

    t2 = STD make_tuple(2L, 3L, 4L, 5L, 6L, 7L);
    CHECK_INT(t2 == STD make_tuple(2, 3, 4, 5, 6, 7), true);
    CHECK_INT(t1 == t2, false);
    CHECK_INT(t1 != t2, true);
    CHECK_INT(t1 < t2, true);
    CHECK_INT(t1 <= t2, true);
    CHECK_INT(t1 > t2, false);
    CHECK_INT(t1 >= t2, false);

    STD tie(val0, val1, val2, val3, val4, val5) = STD make_tuple(2, 3, 4, 5, 6, 7);
    CHECK_INT(val0, 2);
    CHECK_INT(val1, 3);
    CHECK_INT(val2, 4);
    CHECK_INT(val3, 5);
    CHECK_INT(val4, 6);
    CHECK_INT(val5, 7);

    { // test tuple swap
        tpl tx(1, 3, 5, 7, 9, 11);
        tpl ty(2, 4, 6, 8, 10, 12);
        STD swap(tx, ty);
        CHECK_INT(STD get<0>(tx), 2);
        CHECK_INT(STD get<1>(tx), 4);
        CHECK_INT(STD get<2>(tx), 6);
        CHECK_INT(STD get<3>(tx), 8);
        CHECK_INT(STD get<4>(tx), 10);
        CHECK_INT(STD get<5>(tx), 12);
        CHECK_INT(STD get<0>(ty), 1);
        CHECK_INT(STD get<1>(ty), 3);
        CHECK_INT(STD get<2>(ty), 5);
        CHECK_INT(STD get<3>(ty), 7);
        CHECK_INT(STD get<4>(ty), 9);
        CHECK_INT(STD get<5>(ty), 11);
    }
}

static void tuple7() { // tests for tuples with seven elements
    typedef STD tuple<int, int, int, int, int, int, int> tpl;
    tpl t0;
    (void) t0;
    tpl t1(1, 2, 3, 4, 5, 6, 7);
    tpl t2(STD tuple<short, short, short, short, short, short, short>(
        (short) 1, (short) 2, (short) 3, (short) 4, (short) 5, (short) 6, (short) 7));
    CHECK_INT(7, STD tuple_size<tpl>::value);

    typedef STD tuple_element<0, tpl>::type elt0;
    typedef STD tuple_element<1, tpl>::type elt1;
    typedef STD tuple_element<2, tpl>::type elt2;
    typedef STD tuple_element<3, tpl>::type elt3;
    typedef STD tuple_element<4, tpl>::type elt4;
    typedef STD tuple_element<5, tpl>::type elt5;
    typedef STD tuple_element<6, tpl>::type elt6;
    CHECK_TYPE(elt0, int);
    CHECK_TYPE(elt1, int);
    CHECK_TYPE(elt2, int);
    CHECK_TYPE(elt3, int);
    CHECK_TYPE(elt4, int);
    CHECK_TYPE(elt5, int);
    CHECK_TYPE(elt6, int);
    elt0 val0 = STD get<0>(t2);
    elt1 val1 = STD get<1>(t2);
    elt2 val2 = STD get<2>(t2);
    elt3 val3 = STD get<3>(t2);
    elt4 val4 = STD get<4>(t2);
    elt5 val5 = STD get<5>(t2);
    elt6 val6 = STD get<6>(t2);
    CHECK_INT(val0, 1);
    CHECK_INT(val1, 2);
    CHECK_INT(val2, 3);
    CHECK_INT(val3, 4);
    CHECK_INT(val4, 5);
    CHECK_INT(val5, 6);
    CHECK_INT(val6, 7);

    CHECK_INT(t1 == t2, true);
    CHECK_INT(t1 != t2, false);
    CHECK_INT(t1 < t2, false);
    CHECK_INT(t1 <= t2, true);
    CHECK_INT(t1 > t2, false);
    CHECK_INT(t1 >= t2, true);

    t2 = STD make_tuple(2L, 3L, 4L, 5L, 6L, 7L, 8L);
    CHECK_INT(t2 == STD make_tuple(2, 3, 4, 5, 6, 7, 8), true);
    CHECK_INT(t1 == t2, false);
    CHECK_INT(t1 != t2, true);
    CHECK_INT(t1 < t2, true);
    CHECK_INT(t1 <= t2, true);
    CHECK_INT(t1 > t2, false);
    CHECK_INT(t1 >= t2, false);

    STD tie(val0, val1, val2, val3, val4, val5, val6) = STD make_tuple(2, 3, 4, 5, 6, 7, 8);
    CHECK_INT(val0, 2);
    CHECK_INT(val1, 3);
    CHECK_INT(val2, 4);
    CHECK_INT(val3, 5);
    CHECK_INT(val4, 6);
    CHECK_INT(val5, 7);
    CHECK_INT(val6, 8);

    { // test tuple swap
        tpl tx(1, 3, 5, 7, 9, 11, 13);
        tpl ty(2, 4, 6, 8, 10, 12, 14);
        STD swap(tx, ty);
        CHECK_INT(STD get<0>(tx), 2);
        CHECK_INT(STD get<1>(tx), 4);
        CHECK_INT(STD get<2>(tx), 6);
        CHECK_INT(STD get<3>(tx), 8);
        CHECK_INT(STD get<4>(tx), 10);
        CHECK_INT(STD get<5>(tx), 12);
        CHECK_INT(STD get<6>(tx), 14);
        CHECK_INT(STD get<0>(ty), 1);
        CHECK_INT(STD get<1>(ty), 3);
        CHECK_INT(STD get<2>(ty), 5);
        CHECK_INT(STD get<3>(ty), 7);
        CHECK_INT(STD get<4>(ty), 9);
        CHECK_INT(STD get<5>(ty), 11);
        CHECK_INT(STD get<6>(ty), 13);
    }
}

static void tuple8() { // tests for tuples with eight elements
    typedef STD tuple<int, int, int, int, int, int, int, int> tpl;
    tpl t0;
    (void) t0;
    tpl t1(1, 2, 3, 4, 5, 6, 7, 8);
    tpl t2(STD tuple<short, short, short, short, short, short, short, short>(
        (short) 1, (short) 2, (short) 3, (short) 4, (short) 5, (short) 6, (short) 7, (short) 8));
    CHECK_INT(8, STD tuple_size<tpl>::value);

    typedef STD tuple_element<0, tpl>::type elt0;
    typedef STD tuple_element<1, tpl>::type elt1;
    typedef STD tuple_element<2, tpl>::type elt2;
    typedef STD tuple_element<3, tpl>::type elt3;
    typedef STD tuple_element<4, tpl>::type elt4;
    typedef STD tuple_element<5, tpl>::type elt5;
    typedef STD tuple_element<6, tpl>::type elt6;
    typedef STD tuple_element<7, tpl>::type elt7;
    CHECK_TYPE(elt0, int);
    CHECK_TYPE(elt1, int);
    CHECK_TYPE(elt2, int);
    CHECK_TYPE(elt3, int);
    CHECK_TYPE(elt4, int);
    CHECK_TYPE(elt5, int);
    CHECK_TYPE(elt6, int);
    CHECK_TYPE(elt7, int);
    elt0 val0 = STD get<0>(t2);
    elt1 val1 = STD get<1>(t2);
    elt2 val2 = STD get<2>(t2);
    elt3 val3 = STD get<3>(t2);
    elt4 val4 = STD get<4>(t2);
    elt5 val5 = STD get<5>(t2);
    elt6 val6 = STD get<6>(t2);
    elt7 val7 = STD get<7>(t2);
    CHECK_INT(val0, 1);
    CHECK_INT(val1, 2);
    CHECK_INT(val2, 3);
    CHECK_INT(val3, 4);
    CHECK_INT(val4, 5);
    CHECK_INT(val5, 6);
    CHECK_INT(val6, 7);
    CHECK_INT(val7, 8);

    CHECK_INT(t1 == t2, true);
    CHECK_INT(t1 != t2, false);
    CHECK_INT(t1 < t2, false);
    CHECK_INT(t1 <= t2, true);
    CHECK_INT(t1 > t2, false);
    CHECK_INT(t1 >= t2, true);

    t2 = STD make_tuple(2L, 3L, 4L, 5L, 6L, 7L, 8L, 9L);
    CHECK_INT(t2 == STD make_tuple(2, 3, 4, 5, 6, 7, 8, 9), true);
    CHECK_INT(t1 == t2, false);
    CHECK_INT(t1 != t2, true);
    CHECK_INT(t1 < t2, true);
    CHECK_INT(t1 <= t2, true);
    CHECK_INT(t1 > t2, false);
    CHECK_INT(t1 >= t2, false);

    STD tie(val0, val1, val2, val3, val4, val5, val6, val7) = STD make_tuple(2, 3, 4, 5, 6, 7, 8, 9);
    CHECK_INT(val0, 2);
    CHECK_INT(val1, 3);
    CHECK_INT(val2, 4);
    CHECK_INT(val3, 5);
    CHECK_INT(val4, 6);
    CHECK_INT(val5, 7);
    CHECK_INT(val6, 8);
    CHECK_INT(val7, 9);

    { // test tuple swap
        tpl tx(1, 3, 5, 7, 9, 11, 13, 15);
        tpl ty(2, 4, 6, 8, 10, 12, 14, 16);
        STD swap(tx, ty);
        CHECK_INT(STD get<0>(tx), 2);
        CHECK_INT(STD get<1>(tx), 4);
        CHECK_INT(STD get<2>(tx), 6);
        CHECK_INT(STD get<3>(tx), 8);
        CHECK_INT(STD get<4>(tx), 10);
        CHECK_INT(STD get<5>(tx), 12);
        CHECK_INT(STD get<6>(tx), 14);
        CHECK_INT(STD get<7>(tx), 16);
        CHECK_INT(STD get<0>(ty), 1);
        CHECK_INT(STD get<1>(ty), 3);
        CHECK_INT(STD get<2>(ty), 5);
        CHECK_INT(STD get<3>(ty), 7);
        CHECK_INT(STD get<4>(ty), 9);
        CHECK_INT(STD get<5>(ty), 11);
        CHECK_INT(STD get<6>(ty), 13);
        CHECK_INT(STD get<7>(ty), 15);
    }
}

static void tuple9() { // tests for tuples with nine elements
    typedef STD tuple<int, int, int, int, int, int, int, int, int> tpl;
    tpl t0;
    (void) t0;
    tpl t1(1, 2, 3, 4, 5, 6, 7, 8, 9);
    tpl t2(STD tuple<short, short, short, short, short, short, short, short, short>(
        (short) 1, (short) 2, (short) 3, (short) 4, (short) 5, (short) 6, (short) 7, (short) 8, (short) 9));
    CHECK_INT(9, STD tuple_size<tpl>::value);

    typedef STD tuple_element<0, tpl>::type elt0;
    typedef STD tuple_element<1, tpl>::type elt1;
    typedef STD tuple_element<2, tpl>::type elt2;
    typedef STD tuple_element<3, tpl>::type elt3;
    typedef STD tuple_element<4, tpl>::type elt4;
    typedef STD tuple_element<5, tpl>::type elt5;
    typedef STD tuple_element<6, tpl>::type elt6;
    typedef STD tuple_element<7, tpl>::type elt7;
    typedef STD tuple_element<8, tpl>::type elt8;
    CHECK_TYPE(elt0, int);
    CHECK_TYPE(elt1, int);
    CHECK_TYPE(elt2, int);
    CHECK_TYPE(elt3, int);
    CHECK_TYPE(elt4, int);
    CHECK_TYPE(elt5, int);
    CHECK_TYPE(elt6, int);
    CHECK_TYPE(elt7, int);
    CHECK_TYPE(elt8, int);
    elt0 val0 = STD get<0>(t2);
    elt1 val1 = STD get<1>(t2);
    elt2 val2 = STD get<2>(t2);
    elt3 val3 = STD get<3>(t2);
    elt4 val4 = STD get<4>(t2);
    elt5 val5 = STD get<5>(t2);
    elt6 val6 = STD get<6>(t2);
    elt7 val7 = STD get<7>(t2);
    elt8 val8 = STD get<8>(t2);
    CHECK_INT(val0, 1);
    CHECK_INT(val1, 2);
    CHECK_INT(val2, 3);
    CHECK_INT(val3, 4);
    CHECK_INT(val4, 5);
    CHECK_INT(val5, 6);
    CHECK_INT(val6, 7);
    CHECK_INT(val7, 8);
    CHECK_INT(val8, 9);

    CHECK_INT(t1 == t2, true);
    CHECK_INT(t1 != t2, false);
    CHECK_INT(t1 < t2, false);
    CHECK_INT(t1 <= t2, true);
    CHECK_INT(t1 > t2, false);
    CHECK_INT(t1 >= t2, true);

    t2 = STD make_tuple(2L, 3L, 4L, 5L, 6L, 7L, 8L, 9L, 10L);
    CHECK_INT(t2 == STD make_tuple(2, 3, 4, 5, 6, 7, 8, 9, 10), true);
    CHECK_INT(t1 == t2, false);
    CHECK_INT(t1 != t2, true);
    CHECK_INT(t1 < t2, true);
    CHECK_INT(t1 <= t2, true);
    CHECK_INT(t1 > t2, false);
    CHECK_INT(t1 >= t2, false);

    STD tie(val0, val1, val2, val3, val4, val5, val6, val7, val8) = STD make_tuple(2, 3, 4, 5, 6, 7, 8, 9, 10);
    CHECK_INT(val0, 2);
    CHECK_INT(val1, 3);
    CHECK_INT(val2, 4);
    CHECK_INT(val3, 5);
    CHECK_INT(val4, 6);
    CHECK_INT(val5, 7);
    CHECK_INT(val6, 8);
    CHECK_INT(val7, 9);
    CHECK_INT(val8, 10);

    { // test tuple swap
        tpl tx(1, 3, 5, 7, 9, 11, 13, 15, 17);
        tpl ty(2, 4, 6, 8, 10, 12, 14, 16, 18);
        STD swap(tx, ty);
        CHECK_INT(STD get<0>(tx), 2);
        CHECK_INT(STD get<1>(tx), 4);
        CHECK_INT(STD get<2>(tx), 6);
        CHECK_INT(STD get<3>(tx), 8);
        CHECK_INT(STD get<4>(tx), 10);
        CHECK_INT(STD get<5>(tx), 12);
        CHECK_INT(STD get<6>(tx), 14);
        CHECK_INT(STD get<7>(tx), 16);
        CHECK_INT(STD get<8>(tx), 18);
        CHECK_INT(STD get<0>(ty), 1);
        CHECK_INT(STD get<1>(ty), 3);
        CHECK_INT(STD get<2>(ty), 5);
        CHECK_INT(STD get<3>(ty), 7);
        CHECK_INT(STD get<4>(ty), 9);
        CHECK_INT(STD get<5>(ty), 11);
        CHECK_INT(STD get<6>(ty), 13);
        CHECK_INT(STD get<7>(ty), 15);
        CHECK_INT(STD get<8>(ty), 17);
    }
}

static void tuple10() { // tests for tuples with ten elements
    typedef STD tuple<int, int, int, int, int, int, int, int, int, int> tpl;
    tpl t0;
    (void) t0;
    tpl t1(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
    tpl t2(STD tuple<short, short, short, short, short, short, short, short, short, short>(
        (short) 1, (short) 2, (short) 3, (short) 4, (short) 5, (short) 6, (short) 7, (short) 8, (short) 9, (short) 10));
    CHECK_INT(10, STD tuple_size<tpl>::value);
    CHECK_INT(10, STD tuple_size<const tpl>::value);
    CHECK_INT(10, STD tuple_size<volatile tpl>::value);
    CHECK_INT(10, STD tuple_size<const volatile tpl>::value);

    typedef STD tuple_element<0, tpl>::type elt0;
    typedef STD tuple_element<0, const tpl>::type elt0c;
    CHECK_TYPE(const elt0*, elt0c*);
    typedef STD tuple_element<0, volatile tpl>::type elt0v;
    CHECK_TYPE(volatile elt0*, elt0v*);
    typedef STD tuple_element<0, const volatile tpl>::type elt0cv;
    CHECK_TYPE(const volatile elt0*, elt0cv*);

    typedef STD tuple_element<1, tpl>::type elt1;
    typedef STD tuple_element<2, tpl>::type elt2;
    typedef STD tuple_element<3, tpl>::type elt3;
    typedef STD tuple_element<4, tpl>::type elt4;
    typedef STD tuple_element<5, tpl>::type elt5;
    typedef STD tuple_element<6, tpl>::type elt6;
    typedef STD tuple_element<7, tpl>::type elt7;
    typedef STD tuple_element<8, tpl>::type elt8;
    typedef STD tuple_element<9, tpl>::type elt9;
    CHECK_TYPE(elt0, int);
    CHECK_TYPE(elt1, int);
    CHECK_TYPE(elt2, int);
    CHECK_TYPE(elt3, int);
    CHECK_TYPE(elt4, int);
    CHECK_TYPE(elt5, int);
    CHECK_TYPE(elt6, int);
    CHECK_TYPE(elt7, int);
    CHECK_TYPE(elt8, int);
    CHECK_TYPE(elt9, int);
    elt0 val0 = STD get<0>(t2);
    elt1 val1 = STD get<1>(t2);
    elt2 val2 = STD get<2>(t2);
    elt3 val3 = STD get<3>(t2);
    elt4 val4 = STD get<4>(t2);
    elt5 val5 = STD get<5>(t2);
    elt6 val6 = STD get<6>(t2);
    elt7 val7 = STD get<7>(t2);
    elt8 val8 = STD get<8>(t2);
    elt9 val9 = STD get<9>(t2);
    CHECK_INT(val0, 1);
    CHECK_INT(val1, 2);
    CHECK_INT(val2, 3);
    CHECK_INT(val3, 4);
    CHECK_INT(val4, 5);
    CHECK_INT(val5, 6);
    CHECK_INT(val6, 7);
    CHECK_INT(val7, 8);
    CHECK_INT(val8, 9);
    CHECK_INT(val9, 10);

    CHECK_INT(t1 == t2, true);
    CHECK_INT(t1 != t2, false);
    CHECK_INT(t1 < t2, false);
    CHECK_INT(t1 <= t2, true);
    CHECK_INT(t1 > t2, false);
    CHECK_INT(t1 >= t2, true);

    t2 = STD make_tuple(2L, 3L, 4L, 5L, 6L, 7L, 8L, 9L, 10L, 11L);
    CHECK_INT(t2 == STD make_tuple(2, 3, 4, 5, 6, 7, 8, 9, 10, 11), true);
    CHECK_INT(t1 == t2, false);
    CHECK_INT(t1 != t2, true);
    CHECK_INT(t1 < t2, true);
    CHECK_INT(t1 <= t2, true);
    CHECK_INT(t1 > t2, false);
    CHECK_INT(t1 >= t2, false);

    STD tie(val0, val1, val2, val3, val4, val5, val6, val7, val8, val9) =
        STD make_tuple(2, 3, 4, 5, 6, 7, 8, 9, 10, 11);
    CHECK_INT(val0, 2);
    CHECK_INT(val1, 3);
    CHECK_INT(val2, 4);
    CHECK_INT(val3, 5);
    CHECK_INT(val4, 6);
    CHECK_INT(val5, 7);
    CHECK_INT(val6, 8);
    CHECK_INT(val7, 9);
    CHECK_INT(val8, 10);
    CHECK_INT(val9, 11);

    { // test tuple swap
        tpl tx(1, 3, 5, 7, 9, 11, 13, 15, 17, 19);
        tpl ty(2, 4, 6, 8, 10, 12, 14, 16, 18, 20);
        STD swap(tx, ty);
        CHECK_INT(STD get<0>(tx), 2);
        CHECK_INT(STD get<1>(tx), 4);
        CHECK_INT(STD get<2>(tx), 6);
        CHECK_INT(STD get<3>(tx), 8);
        CHECK_INT(STD get<4>(tx), 10);
        CHECK_INT(STD get<5>(tx), 12);
        CHECK_INT(STD get<6>(tx), 14);
        CHECK_INT(STD get<7>(tx), 16);
        CHECK_INT(STD get<8>(tx), 18);
        CHECK_INT(STD get<9>(tx), 20);
        CHECK_INT(STD get<0>(ty), 1);
        CHECK_INT(STD get<1>(ty), 3);
        CHECK_INT(STD get<2>(ty), 5);
        CHECK_INT(STD get<3>(ty), 7);
        CHECK_INT(STD get<4>(ty), 9);
        CHECK_INT(STD get<5>(ty), 11);
        CHECK_INT(STD get<6>(ty), 13);
        CHECK_INT(STD get<7>(ty), 15);
        CHECK_INT(STD get<8>(ty), 17);
        CHECK_INT(STD get<9>(ty), 19);
    }
}

static void t_make_tuple() { // reference_wrapper
    int i0 = 0;
    int i1 = 1;
    int i2 = 2;
    int i3 = 3;
    int i4 = 4;

    STD make_tuple(STD ref(i0), STD ref(i1), STD ref(i2), STD ref(i3), STD ref(i4)) = STD make_tuple(1, 2, 3, 4, 5);
    CHECK_INT(STD make_tuple(i0, i1, i2, i3, i4) == STD make_tuple(1, 2, 3, 4, 5), true);

    int i5 = 5;
    int i6 = 6;
    int i7 = 7;
    int i8 = 8;
    int i9 = 9;

    STD make_tuple(STD ref(i0), STD ref(i1), STD ref(i2), STD ref(i3), STD ref(i4), STD ref(i5), STD ref(i6),
        STD ref(i7), STD ref(i8), STD ref(i9)) = STD make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
    CHECK_INT(
        STD make_tuple(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9) == STD make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10), true);
}

static void t_tie() { // ignore
    int i0 = 0;
    int i1 = 1;
    int i2 = 2;
    int i3 = 3;
    int i4 = 4;

    STD make_tuple(STD ignore, STD ignore, STD ignore, STD ignore, STD ignore) = STD make_tuple(1, 2, 3, 4, 5);
    CHECK_INT(STD make_tuple(i0, i1, i2, i3, i4) == STD make_tuple(0, 1, 2, 3, 4), true);

    int i5 = 5;
    int i6 = 6;
    int i7 = 7;
    int i8 = 8;
    int i9 = 9;

    STD make_tuple(STD ignore, STD ignore, STD ignore, STD ignore, STD ignore, STD ignore, STD ignore, STD ignore,
        STD ignore, STD ignore) = STD make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
    CHECK_INT(
        STD make_tuple(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9) == STD make_tuple(0, 1, 2, 3, 4, 5, 6, 7, 8, 9), true);
}

static void t_move() { // test moves
    {
        typedef STD pair<Movable_int, Movable_int> Pair_mi;
        typedef STD tuple<int, int> Tuple_i;
        typedef STD tuple<Movable_int, Movable_int> Tuple_mi;

        Tuple_mi t1;
        int one        = 1;
        STD get<0>(t1) = STD move(one);
        CHECK_INT(STD get<0>(t1).val, 1);
        CHECK_INT(STD get<1>(t1).val, 0);

        Tuple_mi t2(STD move(t1));
        CHECK_INT(STD get<0>(t1).val, -1);
        CHECK_INT(STD get<1>(t1).val, -1);
        CHECK_INT(STD get<0>(t2).val, 1);
        CHECK_INT(STD get<1>(t2).val, 0);

        Tuple_mi t3(STD move(2), STD move(3));
        CHECK_INT(STD get<0>(t3).val, 2);
        CHECK_INT(STD get<1>(t3).val, 3);

        Pair_mi p4(STD move(4), STD move(5));
        Tuple_mi t4(STD move(p4));
        CHECK_INT(p4.first.val, -1);
        CHECK_INT(p4.second.val, -1);
        CHECK_INT(STD get<0>(t4).val, 4);
        CHECK_INT(STD get<1>(t4).val, 5);

        Tuple_i i5(6, 7);
        Tuple_mi t5(STD move(i5));
        CHECK_INT(STD get<0>(i5), 6);
        CHECK_INT(STD get<1>(i5), 7);
        CHECK_INT(STD get<0>(t5).val, 6);
        CHECK_INT(STD get<1>(t5).val, 7);

        Tuple_mi t6;
        t6 = STD move(t3);
        CHECK_INT(STD get<0>(t3).val, -1);
        CHECK_INT(STD get<1>(t3).val, -1);
        CHECK_INT(STD get<0>(t6).val, 2);
        CHECK_INT(STD get<1>(t6).val, 3);

        Tuple_mi t7;
        t7 = STD move(i5);
        CHECK_INT(STD get<0>(i5), 6);
        CHECK_INT(STD get<1>(i5), 7);
        CHECK_INT(STD get<0>(t7).val, 6);
        CHECK_INT(STD get<1>(t7).val, 7);

        Pair_mi p8(STD move(8), STD move(9));
        Tuple_mi t8;
        t8 = STD move(p8);
        CHECK_INT(p8.first.val, -1);
        CHECK_INT(p8.second.val, -1);
        CHECK_INT(STD get<0>(t8).val, 8);
        CHECK_INT(STD get<1>(t8).val, 9);
    }

    { // check for lvalue stealing
        typedef STD tuple<Copyable_int, Copyable_int> Tuple_ci;

        Copyable_int ci1(1);
        Copyable_int ci2(2);
        Tuple_ci t10(ci1, ci2);
        CHECK_INT(STD get<0>(t10).val, 1);
        CHECK_INT(STD get<1>(t10).val, 2);

        Tuple_ci t11(t10);
        CHECK_INT(STD get<0>(t10).val, 1);
        CHECK_INT(STD get<1>(t10).val, 2);
        CHECK_INT(STD get<0>(t11).val, 1);
        CHECK_INT(STD get<1>(t11).val, 2);

        t10 = t11;
        CHECK_INT(STD get<0>(t10).val, 1);
        CHECK_INT(STD get<1>(t10).val, 2);
        CHECK_INT(STD get<0>(t11).val, 1);
        CHECK_INT(STD get<1>(t11).val, 2);
    }
}

#include <memory>

struct Allocable0 { // supports no allocator only
    Allocable0(int val = 0) : Myval(val) {}

    Allocable0(const Allocable0& right) : Myval(right.Myval) {}

    int Myval;
};

struct Allocable1 { // supports leading allocator only
    Allocable1(int val = 0) : Myval(val) {}

    Allocable1(const Allocable1& right) : Myval(right.Myval) {}

    Allocable1(STD allocator_arg_t, const STD allocator<int>&) : Myval(0) {}

    Allocable1(STD allocator_arg_t, const STD allocator<int>&, int val) : Myval(val) {}

    Allocable1(STD allocator_arg_t, const STD allocator<int>&, const Allocable1& right) : Myval(right.Myval) {}

    int Myval;
};

struct Allocable2 { // supports trailing allocator only
    Allocable2(int val = 0) : Myval(val) {}

    Allocable2(const Allocable2& right) : Myval(right.Myval) {}

    Allocable2(const STD allocator<int>&) : Myval(0) {}

    Allocable2(const Allocable2& right, const STD allocator<int>&) : Myval(right.Myval) {}

    Allocable2(int val, const STD allocator<int>&) : Myval(val) {}

    int Myval;
};

namespace std {
    template <>
    struct uses_allocator<Allocable1, allocator<int>> : true_type {};

    template <>
    struct uses_allocator<Allocable2, allocator<int>> : true_type {};
} // namespace std

static void t_alloc() { // test allocator constructors
    {
        typedef STD tuple<Allocable0> Tal;
        Tal t0(0);
        CHECK_INT(STD get<0>(t0).Myval, 0);

        Allocable0 ab1(1);
        Tal t1(ab1);
        CHECK_INT(STD get<0>(t1).Myval, 1);

        STD tuple<int> tint(2);
        Tal t2(tint);
        CHECK_INT(STD get<0>(t2).Myval, 2);

        typedef STD tuple<Allocable0, Allocable0> Tal2;
        STD pair<int, int> pa3(3, 30);
        Tal2 t3(pa3);
        CHECK_INT(STD get<0>(t3).Myval, 3);
        CHECK_INT(STD get<1>(t3).Myval, 30);

        Tal2 t4(4, 0);
        CHECK_INT(STD get<0>(t4).Myval, 4);

        typedef STD tuple<Movable_int> Tmi;
        Tmi t5a(5);
        Tmi t5b(STD move(t5a));
        CHECK_INT(STD get<0>(t5b), 5);

        Tal t6(STD tuple<int>(6));
        CHECK_INT(STD get<0>(t6).Myval, 6);

        STD pair<int, int> pa7(7, 70);
        Tal2 t7(STD move(pa7));
        CHECK_INT(STD get<0>(t7).Myval, 7);
        CHECK_INT(STD get<1>(t7).Myval, 70);

        Tal2 t8(t7);
        CHECK_INT(STD get<0>(t8).Myval, 7);
        CHECK_INT(STD get<1>(t8).Myval, 70);
    }

    {
        typedef STD tuple<Allocable1> Tal;
        STD allocator<int> al;
        Tal t0(STD allocator_arg, al);
        CHECK_INT(STD get<0>(t0).Myval, 0);

        Allocable1 ab1(STD allocator_arg, al, 1);
        Tal t1(STD allocator_arg, al, ab1);
        CHECK_INT(STD get<0>(t1).Myval, 1);

        STD tuple<int> tint(2);
        Tal t2(STD allocator_arg, al, tint);
        CHECK_INT(STD get<0>(t2).Myval, 2);

        typedef STD tuple<Allocable1, Allocable1> Tal2;
        STD pair<int, int> pa3(3, 30);
        Tal2 t3(STD allocator_arg, al, pa3);
        CHECK_INT(STD get<0>(t3).Myval, 3);
        CHECK_INT(STD get<1>(t3).Myval, 30);

        Tal t4(STD allocator_arg, al, 4);
        CHECK_INT(STD get<0>(t4).Myval, 4);

        typedef STD tuple<Movable_int> Tmi;
        Tmi t5a(5);
        Tmi t5b(STD move(t5a));
        CHECK_INT(STD get<0>(t5b), 5);

        Tal t6(STD allocator_arg, al, STD tuple<int>(6));
        CHECK_INT(STD get<0>(t6).Myval, 6);

        STD pair<int, int> pa7(7, 70);
        Tal2 t7(STD allocator_arg, al, STD move(pa7));
        CHECK_INT(STD get<0>(t7).Myval, 7);
        CHECK_INT(STD get<1>(t7).Myval, 70);

        Tal2 t8(STD allocator_arg, al, t7);
        CHECK_INT(STD get<0>(t8).Myval, 7);
        CHECK_INT(STD get<1>(t8).Myval, 70);
    }

    {
        typedef STD tuple<Allocable2> Tal;
        STD allocator<int> al;
        Tal t0(al);
        CHECK_INT(STD get<0>(t0).Myval, 0);

        Allocable2 ab1(1, al);
        Tal t1(STD allocator_arg, al, ab1);
        CHECK_INT(STD get<0>(t1).Myval, 1);

        STD tuple<int> tint(2);
        Tal t2(STD allocator_arg, al, tint);
        CHECK_INT(STD get<0>(t2).Myval, 2);

        typedef STD tuple<Allocable2, Allocable2> Tal2;
        STD pair<int, int> pa3(3, 30);
        Tal2 t3(STD allocator_arg, al, pa3);
        CHECK_INT(STD get<0>(t3).Myval, 3);
        CHECK_INT(STD get<1>(t3).Myval, 30);

        Tal t4(STD allocator_arg, al, 4);
        CHECK_INT(STD get<0>(t4).Myval, 4);

        typedef STD tuple<Movable_int> Tmi;
        Tmi t5a(5);
        Tmi t5b(STD move(t5a));
        CHECK_INT(STD get<0>(t5b), 5);

        Tal t6(STD allocator_arg, al, STD tuple<int>(6));
        CHECK_INT(STD get<0>(t6).Myval, 6);

        STD pair<int, int> pa7(7, 70);
        Tal2 t7(STD allocator_arg, al, STD move(pa7));
        CHECK_INT(STD get<0>(t7).Myval, 7);
        CHECK_INT(STD get<1>(t7).Myval, 70);

        Tal2 t8(STD allocator_arg, al, t7);
        CHECK_INT(STD get<0>(t8).Myval, 7);
        CHECK_INT(STD get<1>(t8).Myval, 70);
    }
}

void test_main() { // test header <tuple>
    tuple0();
    tuple1();
    tuple2();
    tuple3();
    tuple4();
    tuple5();

    tuple6();
    tuple7();
    tuple8();
    tuple9();
    tuple10();
    t_make_tuple();
    t_tie();
    t_move();
    t_alloc();
}
