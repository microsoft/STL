// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <execution>
#include <forward_list>
#include <functional>
#include <iterator>
#include <list>
#include <numeric>
#include <random>
#include <string>
#include <vector>

#include <parallel_algorithms_utilities.hpp>

using namespace std;
using namespace std::execution;

struct even_gen {
    unsigned int i = 0;

    unsigned int operator()() {
        i += 2;
        return i;
    }
};

struct movable_uint {
    unsigned int value;
    /* implicit */ operator unsigned int() const {
        return value;
    }

    movable_uint() : value{} {}
    /* implicit */ movable_uint(unsigned int x) : value(x) {}
    movable_uint(const movable_uint&)            = delete;
    movable_uint(movable_uint&&)                 = default;
    movable_uint& operator=(const movable_uint&) = delete;
    movable_uint& operator=(movable_uint&&)      = default;
};

const auto is_odd  = [](unsigned int i) { return (i & 0x1u) != 0; };
const auto is_even = [](unsigned int i) { return (i & 0x1u) == 0; };

template <template <class...> class Container>
void test_case_remove_family_parallel(const size_t testSize, mt19937& gen) {
    Container<unsigned int> tmp(testSize);
    generate(tmp.begin(), tmp.end(), even_gen{});
    Container<unsigned int> tested(tmp);

    {
        // remove nothing:
        assert(tested.end() == remove_if(par, tested.begin(), tested.end(), is_odd));
        assert(tmp == tested);

        // remove 2 (basic test for remove)
        auto serialResult   = remove(tmp.begin(), tmp.end(), 2U);
        auto parallelResult = remove(par, tested.begin(), tested.end(), 2U);
        if (testSize < 1) {
            assert(parallelResult == tested.end());
        } else {
            assert(next(parallelResult) == tested.end());
        }

        assert(equal(tmp.begin(), serialResult, tested.begin(), parallelResult));

        // remove everything:
        assert(tested.begin() == remove_if(par, tested.begin(), tested.end(), is_even));
    }

    // "fuzz" testing:
    for (int i = 0; i < 100; ++i) {
        generate(tmp.begin(), tmp.end(), ref(gen));
        tested              = tmp;
        auto serialResult   = remove_if(tmp.begin(), tmp.end(), is_even);
        auto parallelResult = remove_if(par, tested.begin(), tested.end(), is_even);
        assert(equal(tmp.begin(), serialResult, tested.begin(), parallelResult));
    }
}

void test_case_move_only_elements() {
    vector<movable_uint> tmp(10);
    generate(tmp.begin(), tmp.end(), even_gen{});
    auto result = remove_if(par, tmp.begin(), tmp.end(), is_even);
    assert(none_of(tmp.begin(), result, is_even));
}

int main() {
    mt19937 gen(1729);
    parallel_test_case(test_case_remove_family_parallel<forward_list>, gen);
    parallel_test_case(test_case_remove_family_parallel<list>, gen);
    parallel_test_case(test_case_remove_family_parallel<vector>, gen);

    test_case_move_only_elements();
}
