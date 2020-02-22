// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <utility>
#define TEST_NAME "<utility>, part 1"

#include "tdefs.h"
#include <utility>

void test_main() { // test header <utility>
    typedef STD pair<int, long> Pair;
    CHECK_INT(STD tuple_size<Pair>::value, 2);

    typedef STD tuple_element<0, Pair>::type Type0;
    typedef STD tuple_element<1, Pair>::type Type1;
    CHECK_TYPE(Type0, int);
    CHECK_TYPE(Type1, long);

    Pair p        = STD make_pair(0, 1L);
    const Pair cp = STD make_pair(2, 3L);
    CHECK_INT(STD get<0>(p), 0);
    CHECK_INT(STD get<1>(p), 1);
    CHECK_INT(STD get<0>(cp), 2);
    CHECK_INT(STD get<1>(cp), 3);

    {
        Movable_int mi0(1);
        STD pair<Movable_int, int> pmi(STD move(mi0), 2);
        CHECK_INT(pmi.first.val, 1);
        Movable_int mi1(STD move(STD get<0>(pmi)));
        CHECK_INT(mi1.val, 1);
        CHECK_INT(pmi.first.val, -1);
    }

    {
        int x = 3;
        CHECK_INT(STD move_if_noexcept(x), 3);
        Movable_int mi1(3);

        Movable_int mi2 = STD move(mi1);
        CHECK_INT((int) mi2, 3);
        CHECK_INT((int) mi1, -1);

        CHECK_TYPE(decltype(STD declval<int>()), int);
    }
}
