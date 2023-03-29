// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <deque>
#include <vector>

using namespace std;

// Also test DevCom-776568 "vector::erase(a, a) self-move-assigns the whole vector"
struct CheckSelfMoveAssign {
    int i;

    explicit CheckSelfMoveAssign(int i_) : i(i_) {}

    CheckSelfMoveAssign(CheckSelfMoveAssign&& o) {
        i   = o.i;
        o.i = -1;
    }

    CheckSelfMoveAssign& operator=(CheckSelfMoveAssign&& o) {
        i   = o.i;
        o.i = -1;
        return *this;
    }
};

template <template <class...> class Container>
void test_case_devcom_776568() {
    Container<CheckSelfMoveAssign> c;
    c.emplace_back(1);
    c.emplace_back(2);
    c.emplace_back(3);
    const auto after = c.begin() + 1;
    auto it          = c.erase(c.begin(), c.begin());
    assert(after->i == 2); // asserts that the iterator was not invalidated by iterator debugging
    assert(it == c.begin());
    // asserts that no elements were self-move-assigned:
    for (int idx = 1; idx <= 3; ++idx) {
        assert(it->i == idx);
        ++it;
    }

    assert(it == c.end());
}

template <template <class...> class Container>
void test_case_gh_1118() {
    Container<CheckSelfMoveAssign> c;
    c.emplace_back(1);
    c.emplace_back(2);
    c.emplace_back(3);

    const auto after = c.begin() + 2;
    auto it          = c.erase(c.begin() + 1, c.begin() + 1);
    assert(after->i == 3); // asserts that the iterator was not invalidated by iterator debugging
    assert(it == c.begin() + 1);

    // asserts that no elements were self-move-assigned:
    it = c.begin();
    for (int idx = 1; idx <= 3; ++idx) {
        assert(it->i == idx);
        ++it;
    }

    assert(it == c.end());
}

int main() {
    {
        vector<int> v(1, 1729);

        const auto it = v.erase(v.begin());

        assert(it == v.end());

        assert(v.empty());
    }

    {
        vector<int> v;
        v.push_back(11);
        v.push_back(22);
        v.push_back(33);
        v.push_back(44);
        v.push_back(55);

        {
            const auto it = v.erase(v.begin());

            assert(it == v.begin());
            assert(*it == 22);

            assert(v.size() == 4 && v[0] == 22 && v[1] == 33 && v[2] == 44 && v[3] == 55);
        }

        {
            const auto it = v.erase(v.begin() + 1);

            assert(it == v.begin() + 1);
            assert(*it == 44);

            assert(v.size() == 3 && v[0] == 22 && v[1] == 44 && v[2] == 55);
        }

        {
            const auto it = v.erase(v.end() - 1);

            assert(it == v.end());

            assert(v.size() == 2 && v[0] == 22 && v[1] == 44);
        }
    }


    {
        vector<int> v;

        const auto it = v.erase(v.begin(), v.end());

        assert(it == v.end());

        assert(v.empty());
    }

    {
        vector<int> v;
        v.push_back(11);
        v.push_back(22);
        v.push_back(33);
        v.push_back(44);
        v.push_back(55);

        {
            const auto it = v.erase(v.begin(), v.begin());

            assert(it == v.begin());
            assert(*it == 11);

            assert(v.size() == 5);
        }

        {
            const auto it = v.erase(v.begin() + 2, v.begin() + 2);

            assert(it == v.begin() + 2);
            assert(*it == 33);

            assert(v.size() == 5);
        }

        {
            const auto it = v.erase(v.end(), v.end());

            assert(it == v.end());

            assert(v.size() == 5);
        }
    }

    {
        vector<int> v;
        v.push_back(11);
        v.push_back(22);
        v.push_back(33);
        v.push_back(44);
        v.push_back(55);

        const auto it = v.erase(v.begin() + 1, v.begin() + 3);

        assert(it == v.begin() + 1);
        assert(*it == 44);

        assert(v.size() == 3 && v[0] == 11 && v[1] == 44 && v[2] == 55);
    }

    {
        vector<int> v;
        v.push_back(11);
        v.push_back(22);
        v.push_back(33);
        v.push_back(44);
        v.push_back(55);

        const auto it = v.erase(v.begin() + 2, v.end());

        assert(it == v.end());

        assert(v.size() == 2 && v[0] == 11 && v[1] == 22);
    }

    {
        vector<int> v;
        v.push_back(11);
        v.push_back(22);
        v.push_back(33);
        v.push_back(44);
        v.push_back(55);

        const auto it = v.erase(v.begin(), v.begin() + 2);

        assert(it == v.begin());
        assert(*it == 33);

        assert(v.size() == 3 && v[0] == 33 && v[1] == 44 && v[2] == 55);
    }

    {
        vector<int> v;
        v.push_back(11);
        v.push_back(22);
        v.push_back(33);
        v.push_back(44);
        v.push_back(55);

        const auto it = v.erase(v.begin(), v.end());

        assert(it == v.end());

        assert(v.empty());
    }

    test_case_devcom_776568<vector>();
    test_case_devcom_776568<deque>();

    test_case_gh_1118<vector>();
    test_case_gh_1118<deque>();
}
