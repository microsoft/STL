// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <atomic>
#include <cassert>
#include <execution>
#include <forward_list>
#include <functional>
#include <iterator>
#include <list>
#include <random>
#include <vector>

#include <parallel_algorithms_utilities.hpp>

using namespace std;
using namespace std::execution;

const auto add_twenty   = [](unsigned int x) { return x + 20; };
const auto equal_twenty = [](unsigned int x, unsigned int y) { return x + 20 == y; };

template <template <class...> class SourceContainer, template <class...> class DestContainer>
void test_case_unary_transform_parallel(const size_t testSize, mt19937& gen) {
    SourceContainer<unsigned int> s(testSize);
    generate(s.begin(), s.end(), ref(gen));
    DestContainer<unsigned int> d(testSize);
    assert(d.end() == transform(par, s.cbegin(), s.cend(), d.begin(), add_twenty));
    assert(equal(s.cbegin(), s.cend(), d.cbegin(), d.cend(), equal_twenty));
}

template <template <class...> class Source1Container, template <class...> class Source2Container,
    template <class...> class DestContainer>
void test_case_binary_transform_parallel(const size_t testSize, mt19937& gen) {
    Source1Container<unsigned int> s1(testSize);
    Source2Container<unsigned int> s2(testSize);
    generate(s1.begin(), s1.end(), ref(gen));
    generate(s2.begin(), s2.end(), ref(gen));
    vector<unsigned int> expected(testSize);
    transform(s1.cbegin(), s1.cend(), s2.cbegin(), expected.begin(), plus<>{});
    DestContainer<unsigned int> d(testSize);
    assert(d.end() == transform(par, s1.cbegin(), s1.cend(), s2.cbegin(), d.begin(), plus<>{}));
    assert(equal(expected.begin(), expected.end(), d.begin(), d.end()));
}

int main() {
    mt19937 gen(1729);

    parallel_test_case(test_case_unary_transform_parallel<forward_list, forward_list>, gen);
    parallel_test_case(test_case_unary_transform_parallel<forward_list, list>, gen);
    parallel_test_case(test_case_unary_transform_parallel<forward_list, vector>, gen);
    parallel_test_case(test_case_unary_transform_parallel<list, forward_list>, gen);
    parallel_test_case(test_case_unary_transform_parallel<list, list>, gen);
    parallel_test_case(test_case_unary_transform_parallel<list, vector>, gen);
    parallel_test_case(test_case_unary_transform_parallel<vector, forward_list>, gen);
    parallel_test_case(test_case_unary_transform_parallel<vector, list>, gen);
    parallel_test_case(test_case_unary_transform_parallel<vector, vector>, gen);

    parallel_test_case(test_case_binary_transform_parallel<forward_list, forward_list, forward_list>, gen);
    parallel_test_case(test_case_binary_transform_parallel<forward_list, forward_list, list>, gen);
    parallel_test_case(test_case_binary_transform_parallel<forward_list, forward_list, vector>, gen);
    parallel_test_case(test_case_binary_transform_parallel<forward_list, list, forward_list>, gen);
    parallel_test_case(test_case_binary_transform_parallel<forward_list, list, list>, gen);
    parallel_test_case(test_case_binary_transform_parallel<forward_list, list, vector>, gen);
    parallel_test_case(test_case_binary_transform_parallel<forward_list, vector, forward_list>, gen);
    parallel_test_case(test_case_binary_transform_parallel<forward_list, vector, list>, gen);
    parallel_test_case(test_case_binary_transform_parallel<forward_list, vector, vector>, gen);
    parallel_test_case(test_case_binary_transform_parallel<list, forward_list, forward_list>, gen);
    parallel_test_case(test_case_binary_transform_parallel<list, forward_list, list>, gen);
    parallel_test_case(test_case_binary_transform_parallel<list, forward_list, vector>, gen);
    parallel_test_case(test_case_binary_transform_parallel<list, list, forward_list>, gen);
    parallel_test_case(test_case_binary_transform_parallel<list, list, list>, gen);
    parallel_test_case(test_case_binary_transform_parallel<list, list, vector>, gen);
    parallel_test_case(test_case_binary_transform_parallel<list, vector, forward_list>, gen);
    parallel_test_case(test_case_binary_transform_parallel<list, vector, list>, gen);
    parallel_test_case(test_case_binary_transform_parallel<list, vector, vector>, gen);
    parallel_test_case(test_case_binary_transform_parallel<vector, forward_list, forward_list>, gen);
    parallel_test_case(test_case_binary_transform_parallel<vector, forward_list, list>, gen);
    parallel_test_case(test_case_binary_transform_parallel<vector, forward_list, vector>, gen);
    parallel_test_case(test_case_binary_transform_parallel<vector, list, forward_list>, gen);
    parallel_test_case(test_case_binary_transform_parallel<vector, list, list>, gen);
    parallel_test_case(test_case_binary_transform_parallel<vector, list, vector>, gen);
    parallel_test_case(test_case_binary_transform_parallel<vector, vector, forward_list>, gen);
    parallel_test_case(test_case_binary_transform_parallel<vector, vector, list>, gen);
    parallel_test_case(test_case_binary_transform_parallel<vector, vector, vector>, gen);
}
