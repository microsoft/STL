// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <future>
#include <list>
#include <set>
#include <vector>

using namespace std;

// Concurrently destroy and invalidate iterators
template <class Container>
void test_concurrent_destruction() {
    Container c;
    c.insert(c.begin(), 0);

    vector<typename Container::iterator> iters(1000, c.begin());
    {
        auto destroyIters = async(launch::async, [&]() { iters.clear(); });

        auto invalidateIters = async(launch::async, [&]() { c.clear(); });
    }
}

// Concurrently create and invalidate iterators
template <class Container>
void test_concurrent_creation() {
    Container c;
    c.insert(c.begin(), 0);

    vector<typename Container::iterator> iters;
    iters.reserve(1000);

    const auto iter = c.begin();
    {
        auto copyIters = async(launch::async, [&]() {
            for (int i = 0; i < 1000; ++i) {
                iters.push_back(iter);
            };
        });

        auto invalidateIters = async(launch::async, [&]() { c.clear(); });
    }
}

int main() {
    test_concurrent_destruction<list<int>>();
    test_concurrent_destruction<set<int>>();
    test_concurrent_destruction<vector<int>>();

    test_concurrent_creation<list<int>>();
    test_concurrent_creation<set<int>>();
    test_concurrent_creation<vector<int>>();
}
