// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <assert.h>
#include <memory>
#include <random>
#include <vector>

using namespace std;

int main() {

    {
        mt19937 eng(1729);

        uniform_int_distribution<int> dist(0, 1);

        const size_t N = 137;

        vector<bool> x(N);
        vector<bool> y(N);

        for (size_t i = 0; i < N; ++i) {
            const bool b = dist(eng) != 0;

            x[i] = b;
            y[i] = b;
        }

        assert(x == y);

        y.push_back(0);

        assert(x != y);

        y.pop_back();

        assert(x == y);

        y.push_back(1);

        assert(x != y);

        y.pop_back();

        assert(x == y);

        x.back().flip();

        assert(x != y);

        y.back().flip();

        assert(x == y);
    }

    {
        // Also test DevDiv-850453 "<vector>: Missing emplace methods in std::vector<bool> container".

        vector<bool> v(47, allocator<bool>());

        v.emplace_back(make_shared<int>(123));
        v.emplace_back(shared_ptr<int>());

        v.emplace(v.cbegin(), make_shared<double>(3.14));
        v.emplace(v.cbegin(), make_unique<int>(456));
        v.emplace(v.cbegin(), shared_ptr<double>());
        v.emplace(v.cbegin(), unique_ptr<int>());
        v.emplace(v.cbegin(), unique_ptr<int[]>());


        vector<bool> correct;

        correct.insert(correct.cend(), 3, false);
        correct.insert(correct.cend(), 2, true);
        correct.insert(correct.cend(), 47, false);
        correct.insert(correct.cend(), 1, true);
        correct.insert(correct.cend(), 1, false);

        assert(v == correct);
    }
}
