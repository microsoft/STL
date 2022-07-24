// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <execution>
#include <forward_list>
#include <iterator>
#include <list>
#include <vector>

#include <parallel_algorithms_utilities.hpp>

using namespace std;
using namespace std::execution;

constexpr char one_char    = static_cast<char>(1);
const auto my_not_equal_to = [](auto a, auto b) { return a != b; };

template <class T>
void add_example(forward_list<T>& f, T x) {
    f.push_front(x);
}

template <class T>
void add_example(list<T>& l, T x) {
    l.push_back(x);
}

template <class T>
void add_example(vector<T>& v, T x) {
    v.push_back(x);
}

template <template <class...> class Container>
void test_case_equal_parallel(const size_t testSize) {
    Container<char> defaults(testSize);

    // test each signature:
    {
        Container<char> ones(testSize, one_char);
        assert(equal(par, defaults.begin(), defaults.end(), defaults.begin()));
        assert((testSize == 0) == equal(par, defaults.begin(), defaults.end(), ones.begin()));

        assert(equal(par, defaults.begin(), defaults.end(), ones.begin(), my_not_equal_to));
        assert((testSize == 0) == equal(par, defaults.begin(), defaults.end(), defaults.begin(), my_not_equal_to));

        assert(equal(par, defaults.begin(), defaults.end(), defaults.begin(), defaults.end()));
        assert((testSize == 0) == equal(par, defaults.begin(), defaults.end(), ones.begin(), ones.end()));

        assert(equal(par, defaults.begin(), defaults.end(), ones.begin(), ones.end(), my_not_equal_to));
        assert((testSize == 0)
               == equal(par, defaults.begin(), defaults.end(), defaults.begin(), defaults.end(), my_not_equal_to));

        // test mismatched lengths
        add_example(ones, one_char);
        assert(!equal(par, defaults.begin(), defaults.end(), ones.begin(), ones.end(), my_not_equal_to));
        assert(!equal(par, ones.begin(), ones.end(), defaults.begin(), defaults.end(), my_not_equal_to));
    }

    // test counterexample positions:
    {
        Container<char> tmp(testSize);
        // testing every possible combo takes too long
#ifdef EXHAUSTIVE
        for (char& b : tmp) {
            b = one_char;
            assert(!equal(par, defaults.begin(), defaults.end(), tmp.begin(), tmp.end()));
            b = {};
        }
#else // ^^^ EXHAUSTIVE ^^^ // vvv !EXHAUSTIVE vvv
        if (testSize != 0) {
            auto middle = tmp.begin();
            advance(middle, static_cast<ptrdiff_t>(testSize / 2));
            *middle = one_char;
            assert(!equal(par, defaults.begin(), defaults.end(), tmp.begin(), tmp.end()));
        }
#endif // EXHAUSTIVE
    }
}

int main() {
    parallel_test_case(test_case_equal_parallel<forward_list>);
    parallel_test_case(test_case_equal_parallel<list>);
    parallel_test_case(test_case_equal_parallel<vector>);
}
