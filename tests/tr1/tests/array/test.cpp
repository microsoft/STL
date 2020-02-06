// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <array> STD header
#define TEST_NAME "<array>"

#include "tdefs.h"
#include <array>
#include <stdexcept>

void test_main() { // test header <array>
    STD array<int, 0> a0 = {};

    const STD array<int, 0> a0c = {};

    CHECK_INT(a0.size(), 0);
    CHECK_INT(a0.max_size(), 0);
    CHECK(a0.empty());

    STD array<int, 0> a0a;
    a0.swap(a0a);
    CHECK(a0.empty() && a0a.empty());
    CHECK_INT(a0.end() - a0.begin(), 0);
    CHECK_INT(a0c.end() - a0c.begin(), 0);
    CHECK_INT(a0.rend() - a0.rbegin(), 0);
    CHECK_INT(a0c.rend() - a0c.rbegin(), 0);

    { // check const iterators generators
        CHECK_INT(a0.cend() - a0.cbegin(), 0);
        CHECK_INT(a0c.cend() - a0c.cbegin(), 0);
        CHECK_INT(a0.crend() - a0.crbegin(), 0);
        CHECK_INT(a0c.crend() - a0c.crbegin(), 0);
        a0.fill(1);
        CHECK(a0.empty());
    }

#if NO_EXCEPTIONS
#else // NO_EXCEPTIONS
    bool ok;

    ok = false;
    try { // test exception for invalid index
        (void) a0.at(0);
    } catch (const STD out_of_range&) { // handle invalid index
        ok = true;
    }
    CHECK_MSG("'array<int,0>::at' throws range_error", ok);

    ok = false;
    try { // test exception for invalid index
        (void) a0c.at(0);
    } catch (const STD out_of_range&) { // handle invalid index
        ok = true;
    }
    CHECK_MSG("'array<int,0>::at const' throws range_error", ok);
#endif // NO_EXCEPTIONS

    typedef STD array<int, 10> arr10;
    arr10 a1;
    const arr10 a1c = {{0}};
    CHECK_INT(a1.size(), 10);
    CHECK_INT(a1.max_size(), 10);
    CHECK_INT(a1c.size(), 10);
    CHECK(!a1.empty());
    CHECK(!a1c.empty());

    arr10 a2 = {{1}};
    CHECK_INT(a2.size(), 10);
    CHECK_INT(a2.at(0), 1);
    CHECK_INT(a2[0], 1);
    CHECK_INT(a2.data()[0], 1);
    CHECK_INT(a2.at(1), 0);
    CHECK_INT(a2[1], 0);
    CHECK_INT(a2.data()[1], 0);

    arr10 a2a = {{2}};
    a2a.swap(a2);
    CHECK_INT(a2[0], 2);
    CHECK_INT(a2a[0], 1);

#if NO_EXCEPTIONS
#else // NO_EXCEPTIONS
    ok = false;
    try { // test exception for invalid index
        (void) a1.at(11);
    } catch (const STD out_of_range&) { // handle invalid index
        ok = true;
    }
    CHECK_MSG("'array<int,N>::at' throws range_error", ok);

    ok = false;
    try { // test exception for invalid index
        (void) a1c.at(11);
    } catch (const STD out_of_range&) { // handle invalid index
        ok = true;
    }
    CHECK_MSG("'array<int,N>::at const' throws range_error", ok);
#endif // NO_EXCEPTIONS

    const arr10 a10 = {{9, 8, 7, 6, 5, 4, 3, 2, 1, 0}};
    CHECK_INT(a10.size(), 10);
    CHECK_INT(a10.at(0), 9);
    CHECK_INT(a10[0], 9);
    CHECK_INT(a10[9], 0);

    a1 = a10;
    CHECK_INT(a1.size(), 10);
    CHECK_INT(a1.front(), 9);
    a1.front() = 8;
    CHECK_INT(a1.front(), 8);
    CHECK_INT(a1.back(), 0);
    a1.back() = 2;
    CHECK_INT(a1.back(), 2);
    CHECK_INT(a1.end() - a1.begin(), 10);
    CHECK_INT(a1c.end() - a1c.begin(), 10);
    CHECK_INT(a10.end() - a10.begin(), 10);

    arr10 a3(a10);
    CHECK(a10 == a3);
    CHECK(a3 != a2);
    CHECK(a2 < a3);
    CHECK(a2 <= a3);
    CHECK(a3 > a2);
    CHECK(a3 >= a2);

    a1.fill(4);
    CHECK_INT(a1.front(), 4);
    CHECK_INT(a1.back(), 4);

    typedef STD array<int, 3> arr3;

    CHECK_INT(STD tuple_size<arr3>::value, 3);

    typedef STD tuple_element<0, arr3>::type tp0;
    typedef STD tuple_element<1, arr3>::type tp1;
    typedef STD tuple_element<2, arr3>::type tp2;
    CHECK_TYPE(tp0, int);
    CHECK_TYPE(tp1, int);
    CHECK_TYPE(tp2, int);

    arr3 arr        = {{0, 1, 2}};
    const arr3 carr = {{3, 4, 5}};
    CHECK_INT(STD get<0>(arr), 0);
    CHECK_INT(STD get<1>(arr), 1);
    CHECK_INT(STD get<2>(arr), 2);
    CHECK_INT(STD get<0>(carr), 3);
    CHECK_INT(STD get<1>(carr), 4);
    CHECK_INT(STD get<2>(carr), 5);

    {
        STD array<Movable_int, 3> a20;

        Movable_int mi(STD get<0>(STD move(a20)));
        CHECK_INT(mi.val, 0);
        CHECK_INT(a20[0].val, -1);

        STD array<Copyable_int, 3> a22, a23(a22);
        CHECK(a22 == a23);
        a22 = a23;
        CHECK(a22 == a23);
    }
}
