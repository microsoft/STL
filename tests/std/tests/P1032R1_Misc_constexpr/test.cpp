// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <assert.h>
#include <string>
#include <tuple>
#include <utility>

using namespace std;

constexpr bool run_tests() {
    // test tuple pair constructor
    {
        tuple<int, int, double, double> first{1, 2, 0.5, 2.0};
        tuple<double, int> second{2.0, 1};
        pair<tuple<int, int, double, double>, tuple<double, int>> foo{
            first, second, make_index_sequence<4>{}, make_index_sequence<2>{}};

        pair<tuple<int, int, double, double>, tuple<double, int>> bar{piecewise_construct_t{}, first, second};
    }

    // test pair assignment operator
    {
        pair<int, int> input{1, 2};
        pair<int, int> copyAssignment;
        copyAssignment = input;

        pair<int, int> moveAssignment;
        moveAssignment = std::move(input);

        pair<double, double> copyAssignmentConvertible;
        copyAssignmentConvertible = moveAssignment;

        pair<double, double> moveAssignmentConvertible;
        moveAssignmentConvertible = std::move(moveAssignment);
    }

    // test pair::swap
    {
        pair<int, int> pair1{1, 2};
        pair<int, int> pair2{3, 4};

        swap(pair1, pair2);
        assert(pair1.first == 3 && pair1.second == 4 && pair2.first == 1 && pair2.second == 2);

        pair1.swap(pair2);
        assert(pair1.first == 1 && pair1.second == 2 && pair2.first == 3 && pair2.second == 4);
    }

    return true;
}

int main() {
    static_assert(run_tests());
    assert(run_tests());
}
