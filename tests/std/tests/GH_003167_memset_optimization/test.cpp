// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>

using namespace std;

int main()
{
    // bool
    static_assert(_Fill_memset_is_safe<bool*, bool>, "should optimize");

    bool bool_array[]{true, true, false, false, true, false};

    fill(begin(bool_array), end(bool_array), true);
    for (bool v : bool_array) {
        assert(v);
    }

    fill(begin(bool_array), end(bool_array), false);
    for (bool v : bool_array) {
        assert(!v);
    }

    fill(begin(bool_array), end(bool_array), 256); // implicit conversion
    for (bool v : bool_array) {
        assert(v);
    }

    // struct
    struct tiny_point {
        signed char x;
        signed char y;
    };
    
    static_assert(_Fill_memset_is_safe<tiny_point*, tiny_point>, "should optimize");

    tiny_point pts[]{{1, 2}, {3, 4}, {5, 6}};
    fill(begin(pts), end(pts), tiny_point{7, 8}); // implicit conversion
    for (tiny_point pt : pts) {
        assert(pt.x == 7);
        assert(pt.y == 8);
    }

    // odd size
    struct rgb {
        unsigned char r, g, b;
    };

    static_assert(!_Fill_memset_is_safe<rgb*, rgb>, "shouldn't optimize");

    rgb colors[]{{255, 0, 0}, {0, 255, 0}, {0, 0, 255}};
    fill(begin(colors), end(colors), rgb{127, 63, 191});
    for (rgb c : colors) {
        assert(c.r == 127);
        assert(c.g == 63);
        assert(c.b == 191);
    }

    // user-defined assignment
    struct snowflake {
        char v;
        snowflake& operator=(const snowflake&) {
            return *this;
        }
    };

    static_assert(!_Fill_memset_is_safe<snowflake*, snowflake>, "shouldn't optimize");

    snowflake snow[]{{1}, {2}, {3}};
    fill(begin(snow), end(snow), snowflake{});
    assert(snow[0].v == 1);
    assert(snow[1].v == 2);
    assert(snow[2].v == 3);
}