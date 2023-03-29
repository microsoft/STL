// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <forward_list>
#include <iterator>
#include <list>
#include <type_traits>
#include <vector>

using namespace std;

template <typename Container, typename Predicate>
void test_partition_point(const Container& c, Predicate p, int correct) {
    assert(is_partitioned(c.begin(), c.end(), p));

    const auto i = partition_point(c.begin(), c.end(), p);

    assert(distance(c.begin(), i) == correct);
}

struct Cat {
    explicit Cat(int n) : m_n(n) {}

    int m_n;
};

size_t g_swaps = 0;
size_t g_preds = 0;

void swap(Cat& l, Cat& r) {
    ++g_swaps;
    swap(l.m_n, r.m_n);
}

template <typename Container>
void test_partition(const vector<int>& vi, const int x) {
    auto pred = [x](const Cat& cat) -> bool {
        ++g_preds;
        return cat.m_n < x;
    };

    Container c(vi.begin(), vi.end());

    g_swaps = 0;
    g_preds = 0;

    const auto i = partition(c.begin(), c.end(), pred);

    typedef typename iterator_traits<typename Container::iterator>::iterator_category category_t;

    if (is_same_v<category_t, forward_iterator_tag>) {
        assert(g_swaps <= vi.size());
    } else {
        assert(g_swaps <= vi.size() / 2);
    }

    assert(g_preds == vi.size());

    assert(is_partitioned(c.begin(), c.end(), pred));

    assert(distance(c.begin(), i) == x);

    assert(i == partition_point(c.begin(), c.end(), pred));
}

int main() {
    for (int k = 0; k < 100; ++k) {
        vector<int> v;

        for (int i = 0; i < k; ++i) {
            v.push_back(i);
        }

        forward_list<int> fl(v.begin(), v.end());

        for (int x = 0; x <= k; ++x) {
            test_partition_point(
                v, [x](int e) { return e < x; }, x);
            test_partition_point(
                fl, [x](int e) { return e < x; }, x);
        }
    }

    for (int k = 0; k < 7; ++k) {
        vector<int> vi;

        for (int i = 0; i < k; ++i) {
            vi.push_back(i);
        }

        do {
            for (int x = 0; x <= k; ++x) {
                test_partition<vector<Cat>>(vi, x);
                test_partition<list<Cat>>(vi, x);
                test_partition<forward_list<Cat>>(vi, x);
            }
        } while (next_permutation(vi.begin(), vi.end()));
    }
}
