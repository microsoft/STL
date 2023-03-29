// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <functional>
#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

int main() {
    {
        unique_ptr<int> u1(new int(11));
        unique_ptr<int> u2(new int(22));
        unique_ptr<int> u3(new int(33));
        unique_ptr<int> u4(new int(44));

        pair<unique_ptr<int>, unique_ptr<int>> p(move(u1), move(u2));

        assert(*p.first == 11);
        assert(*p.second == 22);

        p = make_pair(move(u3), move(u4));

        assert(*p.first == 33);
        assert(*p.second == 44);
    }

    {
        unique_ptr<int> u1(new int(110));
        unique_ptr<int> u2(new int(220));
        unique_ptr<int> u3(new int(330));
        unique_ptr<int> u4(new int(440));

        tuple<unique_ptr<int>, unique_ptr<int>> t(move(u1), move(u2));

        assert(*get<0>(t) == 110);
        assert(*get<1>(t) == 220);

        t = make_tuple(move(u3), move(u4));

        assert(*get<0>(t) == 330);
        assert(*get<1>(t) == 440);
    }

    {
        int i          = 1729;
        double d       = 3.14;
        const double e = 2.71828;

        reference_wrapper<int> r1(i);
        const reference_wrapper<int> r2(i);
        reference_wrapper<const int> r3(i);
        const reference_wrapper<const int> r4(i);

        int x[]       = {100, 200, 300};
        const int y[] = {400, 500, 600};

        STATIC_ASSERT(is_same_v<decltype(make_pair(r1, d)), pair<int&, double>>);
        STATIC_ASSERT(is_same_v<decltype(make_pair(r2, e)), pair<int&, double>>);
        STATIC_ASSERT(is_same_v<decltype(make_pair(r3, move(d))), pair<const int&, double>>);
        STATIC_ASSERT(is_same_v<decltype(make_pair(r4, move(e))), pair<const int&, double>>);

        STATIC_ASSERT(is_same_v<decltype(make_pair(move(r1), x)), pair<int&, int*>>);
        STATIC_ASSERT(is_same_v<decltype(make_pair(move(r2), y)), pair<int&, const int*>>);
        STATIC_ASSERT(is_same_v<decltype(make_pair(move(r3), move(x))), pair<const int&, int*>>);
        STATIC_ASSERT(is_same_v<decltype(make_pair(move(r4), move(y))), pair<const int&, const int*>>);

        STATIC_ASSERT(is_same_v<decltype(make_tuple(r1, d)), tuple<int&, double>>);
        STATIC_ASSERT(is_same_v<decltype(make_tuple(r2, e)), tuple<int&, double>>);
        STATIC_ASSERT(is_same_v<decltype(make_tuple(r3, move(d))), tuple<const int&, double>>);
        STATIC_ASSERT(is_same_v<decltype(make_tuple(r4, move(e))), tuple<const int&, double>>);

        STATIC_ASSERT(is_same_v<decltype(make_tuple(move(r1), x)), tuple<int&, int*>>);
        STATIC_ASSERT(is_same_v<decltype(make_tuple(move(r2), y)), tuple<int&, const int*>>);
        STATIC_ASSERT(is_same_v<decltype(make_tuple(move(r3), move(x))), tuple<const int&, int*>>);
        STATIC_ASSERT(is_same_v<decltype(make_tuple(move(r4), move(y))), tuple<const int&, const int*>>);

        (void) d;
        (void) e;
        (void) x;
        (void) y;
        (void) r1;
        (void) r2;
        (void) r3;
        (void) r4;
    }
}
