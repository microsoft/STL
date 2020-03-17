// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
// This header contains all code for the instantiate_algorithms_int* test cases.
// The instantiate_algorithm* tests take too long individually, so must be split into two parts.

#include <input_iterator.hpp>
#include <instantiate_algorithms.hpp>

void test() {
    std_testing::input_iterator_container<int> input_it{};
    std::forward_list<int> fwd_it{};
    std::list<int> bidi_it{};
    std::vector<int> rand_it{};

    std_testing::instantiate_std_algorithms_with<int>(
        input_it.begin(), fwd_it.begin(), bidi_it.begin(), rand_it.begin(), std::front_inserter(fwd_it));
}
