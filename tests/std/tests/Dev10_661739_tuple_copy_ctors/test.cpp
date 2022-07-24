// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <tuple>
#include <utility>

using namespace std;

int main() {
    {
        tuple<> src;
        tuple<> dest(src);
        assert(src == dest);
    }

    {
        tuple<int> src(1729);
        tuple<int> dest(src);
        assert(src == dest);
    }

    {
        tuple<int, int> src(17, 29);
        tuple<int, int> dest(src);
        assert(src == dest);
    }

    {
        tuple<int> src(1729);
        tuple<long> dest(src);
        assert(src == dest);
    }

    {
        tuple<int, int> src(17, 29);
        tuple<long, long> dest(src);
        assert(src == dest);
    }

    {
        pair<int, int> src(17, 29);
        tuple<int, int> dest(src);
        assert(src.first == get<0>(dest) && src.second == get<1>(dest));
    }

    {
        pair<int, int> src(17, 29);
        tuple<long, long> dest(src);
        assert(src.first == get<0>(dest) && src.second == get<1>(dest));
    }

    {
        tuple<int, int> t(-1, -1);

        const pair<int, int> p(123, 456);
        t = p;
        assert(get<0>(t) == 123 && get<1>(t) == 456);

        t = make_pair(70, 80);
        assert(get<0>(t) == 70 && get<1>(t) == 80);

        t = make_tuple(100, 200);
        assert(get<0>(t) == 100 && get<1>(t) == 200);
    }
}
