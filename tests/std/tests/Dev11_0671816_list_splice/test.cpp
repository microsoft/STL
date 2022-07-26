// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <forward_list>
#include <iterator>
#include <list>

using namespace std;

int main() {
#if _ITERATOR_DEBUG_LEVEL != 1
    {
        list<int> l = {100, 200, 300, 400, 500};

        auto a0 = l.begin();
        auto a1 = next(l.begin(), 1);
        auto a2 = next(l.begin(), 2);
        auto a3 = next(l.begin(), 3);
        auto a4 = next(l.begin(), 4);
        auto a5 = l.end();

        list<int> other = {11, 22, 33, 44, 55};

        auto b0 = other.begin();
        auto b1 = next(other.begin(), 1);
        auto b2 = next(other.begin(), 2);
        auto b3 = next(other.begin(), 3);
        auto b4 = next(other.begin(), 4);
        auto b5 = other.end();

        l.splice(next(l.begin(), 3), other);

        assert(*a0 == 100);
        assert(*a1 == 200);
        assert(*a2 == 300);
        assert(*a3 == 400);
        assert(*a4 == 500);
        assert(a5 == l.end());

        assert(*b0 == 11);
        assert(*b1 == 22);
        assert(*b2 == 33);
        assert(*b3 == 44);
        assert(*b4 == 55);
        assert(b5 == other.end());

        assert(l.begin() == a0);
        assert(next(l.begin(), 1) == a1);
        assert(next(l.begin(), 2) == a2);
        assert(next(l.begin(), 3) == b0);
        assert(next(l.begin(), 4) == b1);
        assert(next(l.begin(), 5) == b2);
        assert(next(l.begin(), 6) == b3);
        assert(next(l.begin(), 7) == b4);
        assert(next(l.begin(), 8) == a3);
        assert(next(l.begin(), 9) == a4);
        assert(next(l.begin(), 10) == a5);

        assert(++b4 == a3);
    }

    {
        list<int> l = {100, 200, 300, 400, 500};

        auto a0 = l.begin();
        auto a1 = next(l.begin(), 1);
        auto a2 = next(l.begin(), 2);
        auto a3 = next(l.begin(), 3);
        auto a4 = next(l.begin(), 4);
        auto a5 = l.end();

        list<int> other = {11, 22, 33, 44, 55};

        auto b0  = other.begin();
        auto b1  = next(other.begin(), 1);
        auto b2  = next(other.begin(), 2);
        auto b3x = next(other.begin(), 3);
        auto b4  = next(other.begin(), 4);
        auto b3y = next(other.begin(), 3);
        auto b5  = other.end();
        auto b3z = next(other.begin(), 3);

        l.splice(next(l.begin(), 2), other, next(other.begin(), 3));

        assert(*a0 == 100);
        assert(*a1 == 200);
        assert(*a2 == 300);
        assert(*a3 == 400);
        assert(*a4 == 500);
        assert(a5 == l.end());

        assert(*b0 == 11);
        assert(*b1 == 22);
        assert(*b2 == 33);
        assert(*b3x == 44);
        assert(*b4 == 55);
        assert(*b3y == 44);
        assert(b5 == other.end());
        assert(*b3z == 44);

        assert(l.begin() == a0);
        assert(next(l.begin(), 1) == a1);
        assert(next(l.begin(), 2) == b3x);
        assert(next(l.begin(), 2) == b3y);
        assert(next(l.begin(), 2) == b3z);
        assert(next(l.begin(), 3) == a2);
        assert(next(l.begin(), 4) == a3);
        assert(next(l.begin(), 5) == a4);
        assert(next(l.begin(), 6) == a5);

        assert(++b3z == a2);
    }

    {
        list<int> l = {100, 200, 300, 400, 500};

        auto a0  = l.begin();
        auto a1  = next(l.begin(), 1);
        auto a2  = next(l.begin(), 2);
        auto a3x = next(l.begin(), 3);
        auto a4  = next(l.begin(), 4);
        auto a3y = next(l.begin(), 3);
        auto a5  = l.end();
        auto a3z = next(l.begin(), 3);

        l.splice(next(l.begin(), 1), l, next(l.begin(), 3));

        assert(*a0 == 100);
        assert(*a1 == 200);
        assert(*a2 == 300);
        assert(*a3x == 400);
        assert(*a4 == 500);
        assert(*a3y == 400);
        assert(a5 == l.end());
        assert(*a3z == 400);

        assert(l.begin() == a0);
        assert(next(l.begin(), 1) == a3x);
        assert(next(l.begin(), 1) == a3y);
        assert(next(l.begin(), 1) == a3z);
        assert(next(l.begin(), 2) == a1);
        assert(next(l.begin(), 3) == a2);
        assert(next(l.begin(), 4) == a4);
        assert(next(l.begin(), 5) == a5);

        assert(++a3z == a1);
    }

    {
        list<int> l = {100, 200, 300, 400, 500};

        auto a0  = l.begin();
        auto a1  = next(l.begin(), 1);
        auto a2  = next(l.begin(), 2);
        auto a3x = next(l.begin(), 3);
        auto a4  = next(l.begin(), 4);
        auto a3y = next(l.begin(), 3);
        auto a5  = l.end();
        auto a3z = next(l.begin(), 3);

        l.splice(next(l.begin(), 3), l, next(l.begin(), 3));

        assert(*a0 == 100);
        assert(*a1 == 200);
        assert(*a2 == 300);
        assert(*a3x == 400);
        assert(*a4 == 500);
        assert(*a3y == 400);
        assert(a5 == l.end());
        assert(*a3z == 400);

        assert(l.begin() == a0);
        assert(next(l.begin(), 1) == a1);
        assert(next(l.begin(), 2) == a2);
        assert(next(l.begin(), 3) == a3x);
        assert(next(l.begin(), 3) == a3y);
        assert(next(l.begin(), 3) == a3z);
        assert(next(l.begin(), 4) == a4);
        assert(next(l.begin(), 5) == a5);

        assert(++a3z == a4);
    }

    {
        list<int> l = {100, 200, 300, 400, 500};

        auto a0  = l.begin();
        auto a1  = next(l.begin(), 1);
        auto a2  = next(l.begin(), 2);
        auto a3x = next(l.begin(), 3);
        auto a4  = next(l.begin(), 4);
        auto a3y = next(l.begin(), 3);
        auto a5  = l.end();
        auto a3z = next(l.begin(), 3);

        l.splice(next(l.begin(), 4), l, next(l.begin(), 3));

        assert(*a0 == 100);
        assert(*a1 == 200);
        assert(*a2 == 300);
        assert(*a3x == 400);
        assert(*a4 == 500);
        assert(*a3y == 400);
        assert(a5 == l.end());
        assert(*a3z == 400);

        assert(l.begin() == a0);
        assert(next(l.begin(), 1) == a1);
        assert(next(l.begin(), 2) == a2);
        assert(next(l.begin(), 3) == a3x);
        assert(next(l.begin(), 3) == a3y);
        assert(next(l.begin(), 3) == a3z);
        assert(next(l.begin(), 4) == a4);
        assert(next(l.begin(), 5) == a5);

        assert(++a3z == a4);
    }

    {
        list<int> l = {100, 200, 300, 400, 500};

        auto a0 = l.begin();
        auto a1 = next(l.begin(), 1);
        auto a2 = next(l.begin(), 2);
        auto a3 = next(l.begin(), 3);
        auto a4 = next(l.begin(), 4);
        auto a5 = l.end();

        list<int> other = {11, 22, 33, 44, 55};

        auto b0 = other.begin();
        auto b1 = next(other.begin(), 1);
        auto b2 = next(other.begin(), 2);
        auto b3 = next(other.begin(), 3);
        auto b4 = next(other.begin(), 4);
        auto b5 = other.end();

        auto c0 = b0;
        auto c1 = b1;
        auto c2 = b2;
        auto c3 = b3;
        auto c4 = b4;
        auto c5 = b5;

        l.splice(next(l.begin(), 3), other, next(other.begin(), 1), next(other.begin(), 4));

        assert(*a0 == 100);
        assert(*a1 == 200);
        assert(*a2 == 300);
        assert(*a3 == 400);
        assert(*a4 == 500);
        assert(a5 == l.end());

        assert(*b0 == 11);
        assert(*b1 == 22);
        assert(*b2 == 33);
        assert(*b3 == 44);
        assert(*b4 == 55);
        assert(b5 == other.end());

        assert(*c0 == 11);
        assert(*c1 == 22);
        assert(*c2 == 33);
        assert(*c3 == 44);
        assert(*c4 == 55);
        assert(c5 == other.end());

        assert(l.begin() == a0);
        assert(next(l.begin(), 1) == a1);
        assert(next(l.begin(), 2) == a2);
        assert(next(l.begin(), 3) == b1);
        assert(next(l.begin(), 3) == c1);
        assert(next(l.begin(), 4) == b2);
        assert(next(l.begin(), 4) == c2);
        assert(next(l.begin(), 5) == b3);
        assert(next(l.begin(), 5) == c3);
        assert(next(l.begin(), 6) == a3);
        assert(next(l.begin(), 7) == a4);
        assert(next(l.begin(), 8) == a5);

        assert(other.begin() == b0);
        assert(other.begin() == c0);
        assert(next(other.begin(), 1) == b4);
        assert(next(other.begin(), 1) == c4);
        assert(next(other.begin(), 2) == b5);
        assert(next(other.begin(), 2) == c5);

        assert(l.end() == a5);
        assert(prev(l.end(), 1) == a4);
        assert(prev(l.end(), 2) == a3);
        assert(prev(l.end(), 3) == b3);
        assert(prev(l.end(), 3) == c3);
        assert(prev(l.end(), 4) == b2);
        assert(prev(l.end(), 4) == c2);
        assert(prev(l.end(), 5) == b1);
        assert(prev(l.end(), 5) == c1);
        assert(prev(l.end(), 6) == a2);
        assert(prev(l.end(), 7) == a1);
        assert(prev(l.end(), 8) == a0);

        assert(other.end() == b5);
        assert(other.end() == c5);
        assert(prev(other.end(), 1) == b4);
        assert(prev(other.end(), 1) == c4);
        assert(prev(other.end(), 2) == b0);
        assert(prev(other.end(), 2) == c0);

        assert(++c3 == a3);
        assert(--b1 == a2);
    }

    {
        list<int> l = {100, 200, 300, 400, 500};

        auto a0 = l.begin();
        auto a1 = next(l.begin(), 1);
        auto a2 = next(l.begin(), 2);
        auto a3 = next(l.begin(), 3);
        auto a4 = next(l.begin(), 4);
        auto a5 = l.end();

        l.splice(l.begin(), l, next(l.begin(), 2), next(l.begin(), 4));

        assert(*a0 == 100);
        assert(*a1 == 200);
        assert(*a2 == 300);
        assert(*a3 == 400);
        assert(*a4 == 500);
        assert(a5 == l.end());

        assert(l.begin() == a2);
        assert(next(l.begin(), 1) == a3);
        assert(next(l.begin(), 2) == a0);
        assert(next(l.begin(), 3) == a1);
        assert(next(l.begin(), 4) == a4);
        assert(next(l.begin(), 5) == a5);

        assert(++a3 == a0);
    }

    {
        list<int> l = {100, 200, 300, 400, 500};

        auto a0 = l.begin();
        auto a1 = next(l.begin(), 1);
        auto a2 = next(l.begin(), 2);
        auto a3 = next(l.begin(), 3);
        auto a4 = next(l.begin(), 4);
        auto a5 = l.end();

        l.splice(next(l.begin(), 4), l, next(l.begin(), 2), next(l.begin(), 4));

        assert(*a0 == 100);
        assert(*a1 == 200);
        assert(*a2 == 300);
        assert(*a3 == 400);
        assert(*a4 == 500);
        assert(a5 == l.end());

        assert(l.begin() == a0);
        assert(next(l.begin(), 1) == a1);
        assert(next(l.begin(), 2) == a2);
        assert(next(l.begin(), 3) == a3);
        assert(next(l.begin(), 4) == a4);
        assert(next(l.begin(), 5) == a5);
    }

    {
        forward_list<int> l = {1, 2, 3};
        auto it             = l.begin();
        forward_list<int> other;
        other.splice_after(other.before_begin(), l);
        for (int idx = 1; idx <= 3; ++idx) {
            assert(*it == idx);
            ++it;
        }

        assert(it == other.end());
    }
#endif // _ITERATOR_DEBUG_LEVEL != 1
}
