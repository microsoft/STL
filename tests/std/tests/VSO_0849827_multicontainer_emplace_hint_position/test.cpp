// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <iterator>
#include <map>
#include <utility>

using namespace std;

static constexpr pair<const int, int> expected4[] = {
    {0, 0},
    {1, 0},
    {2, 0},
    {3, 0},
};

int main() {
    // All zero special cases:
    {
        map<int, int> m;
        m.emplace_hint(m.begin(), 2, 42);
        assert((m.size() == 1 && *m.begin() == pair<const int, int>(2, 42)));
    }

    {
        map<int, int> m;
        m.emplace_hint(m.end(), 2, 42);
        assert((m.size() == 1 && *m.begin() == pair<const int, int>(2, 42)));
    }

    {
        multimap<int, int> m;
        m.emplace_hint(m.begin(), 2, 42);
        assert((m.size() == 1 && *m.begin() == pair<const int, int>(2, 42)));
    }

    {
        multimap<int, int> m;
        m.emplace_hint(m.end(), 2, 42);
        assert((m.size() == 1 && *m.begin() == pair<const int, int>(2, 42)));
    }

    // Map begin/end special cases:
    {
        map<int, int> m{
            {2, 0},
            {3, 0},
            {4, 0},
        };

        m.emplace_hint(m.end(), 6, 0);
        m.emplace_hint(m.end(), 5, 0);
        m.emplace_hint(m.begin(), 0, 0);
        m.emplace_hint(m.begin(), 1, 0);

        static constexpr pair<const int, int> expected[] = {
            {0, 0},
            {1, 0},
            {2, 0},
            {3, 0},
            {4, 0},
            {5, 0},
            {6, 0},
        };

        assert(equal(m.begin(), m.end(), begin(expected), end(expected)));
    }

    {
        // Inserted value goes immediately before the hint
        map<int, int> m{
            {0, 0},
            {2, 0},
            {3, 0},
        };

        m.emplace_hint(next(m.begin()), 1, 0);
        assert(equal(m.begin(), m.end(), begin(expected4), end(expected4)));
    }

    {
        // Inserted value goes before the hint, but not immediately before
        map<int, int> m{
            {1, 0},
            {2, 0},
            {3, 0},
        };

        m.emplace_hint(next(m.begin()), 0, 0);
        assert(equal(m.begin(), m.end(), begin(expected4), end(expected4)));
    }

    {
        // Inserted value goes immediately after the hint
        map<int, int> m{
            {0, 0},
            {1, 0},
            {3, 0},
        };

        m.emplace_hint(next(m.begin()), 2, 0);
        assert(equal(m.begin(), m.end(), begin(expected4), end(expected4)));
    }

    {
        // Inserted value goes after the hint, but isn't the hint's immediate successor
        map<int, int> m{
            {0, 0},
            {1, 0},
            {2, 0},
        };

        m.emplace_hint(next(m.begin()), 3, 0);
        assert(equal(m.begin(), m.end(), begin(expected4), end(expected4)));
    }

    {
        // Inserted value is exactly the hint (and no insertion occurs)
        map<int, int> m{
            {0, 0},
            {1, 0},
            {2, 0},
            {3, 0},
        };

        const auto where = next(m.begin());
        assert(where == m.emplace_hint(where, 1, 0));
        assert(equal(m.begin(), m.end(), begin(expected4), end(expected4)));
    }

    // Multimap special cases:
    {
        multimap<int, int> m{
            {1, 0},
            {2, 0},
            {3, 0},
            {4, 2},
            {4, 4},
            {4, 5},
            {5, 0},
            {6, 0},
            {7, 0},
        };

        m.emplace_hint(next(m.begin(), 4), 4, 3); // in the middle of 4s, should insert at position 6
        // should insert at the beginning of the 4s:
        m.emplace_hint(m.begin(), 4, 1);
        m.emplace_hint(next(m.begin()), 4, 0);
        // should insert at the end of the 4s:
        m.emplace_hint(prev(m.end()), 4, 6);
        m.emplace_hint(m.end(), 4, 7);

        // other edge cases just to make sure we didn't break anything with this change :)
        m.emplace_hint(m.begin(), 0, 0);
        m.emplace_hint(m.end(), 8, 0);

        static constexpr pair<const int, int> expected[] = {
            {0, 0},
            {1, 0},
            {2, 0},
            {3, 0},
            {4, 0},
            {4, 1},
            {4, 2},
            {4, 3},
            {4, 4},
            {4, 5},
            {4, 6},
            {4, 7},
            {5, 0},
            {6, 0},
            {7, 0},
            {8, 0},
        };

        assert(equal(m.begin(), m.end(), begin(expected), end(expected)));
    }
}
