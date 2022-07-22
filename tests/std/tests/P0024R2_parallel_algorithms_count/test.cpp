// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <execution>
#include <forward_list>
#include <list>
#include <numeric>
#include <random>
#include <vector>

#include <parallel_algorithms_utilities.hpp>

using namespace std;
using namespace std::execution;

const auto is_zero = [](char x) { return x == '\x00'; };
const auto is_one  = [](char x) { return x == '\x01'; };

template <template <class...> class Container>
void test_case_count_parallel(const size_t testSize, mt19937& gen) {
    Container<char> c(testSize, '\x01');
    assert(count(par, c.begin(), c.end(), '\x00') == 0);
    assert(count(par, c.begin(), c.end(), '\x01') == static_cast<ptrdiff_t>(testSize));
    assert(count_if(par, c.begin(), c.end(), is_zero) == 0);
    assert(count_if(par, c.begin(), c.end(), is_one) == static_cast<ptrdiff_t>(testSize));

    vector<typename Container<char>::iterator> iterators(testSize);
    iota(iterators.begin(), iterators.end(), c.begin());
    shuffle(iterators.begin(), iterators.end(), gen);

    auto remainingAttempts = quadratic_complexity_case_limit;
    ptrdiff_t consumed{};
    for (const auto& iter : iterators) {
        *iter = '\x00';
        ++consumed;
        assert(count(par, c.begin(), c.end(), '\x00') == consumed);
        assert(count(par, c.begin(), c.end(), '\x01') == static_cast<ptrdiff_t>(testSize) - consumed);
        assert(count_if(par, c.begin(), c.end(), is_zero) == consumed);
        assert(count_if(par, c.begin(), c.end(), is_one) == static_cast<ptrdiff_t>(testSize) - consumed);
        if (--remainingAttempts == 0) {
            return;
        }
    }
}

int main() {
    mt19937 gen(random_device{}());
    parallel_test_case(test_case_count_parallel<forward_list>, gen);
    parallel_test_case(test_case_count_parallel<list>, gen);
    parallel_test_case(test_case_count_parallel<vector>, gen);
}
