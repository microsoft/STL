// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <array>
#include <assert.h>
#include <iterator>
#include <string>
#include <tuple>
#include <utility>

using namespace std;

struct constexpr_container {
    using value_type = int;

    array<value_type, 6> buffer{};
    size_t selected = 0;
    constexpr void push_back(const value_type i) {
        buffer[selected++] = i;
    }
};

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

    // test empty tuple
    {
        allocator<int> alloc;
        tuple<> tuple_alloc{allocator_arg_t{}, alloc};
        tuple<> tuple_alloc_copy{allocator_arg_t{}, alloc, tuple_alloc};
        tuple<> tuple_alloc_value{_Alloc_exact_args_t{}, alloc};

        swap(tuple_alloc, tuple_alloc_copy);
        tuple_alloc.swap(tuple_alloc_copy);
    }

    // test tuple
    {
        allocator<int> alloc;
        tuple<short, int> conversionInput{short(1), 1};
        const tuple<short, int> constConversionInput{short(1), 1};
        pair<short, int> conversionInputPair{short(1), 1};
        const pair<short, int> constConversionInputPair{short(1), 1};

        tuple<int, double> tuple_alloc{allocator_arg_t{}, alloc};
        tuple<int, double> tuple_alloc_value{allocator_arg_t{}, alloc, 1, 2.0};
        tuple<int, double> tuple_alloc_conversion{allocator_arg_t{}, alloc, short(1), 1};
        tuple<int, double> tuple_alloc_conversion_tuple{allocator_arg_t{}, alloc, conversionInput};
        tuple<int, double> tuple_alloc_conversion_const_tuple{allocator_arg_t{}, alloc, constConversionInput};
        tuple<int, double> tuple_alloc_conversion_pair{allocator_arg_t{}, alloc, conversionInputPair};
        tuple<int, double> tuple_alloc_conversion_const_pair{allocator_arg_t{}, alloc, constConversionInputPair};
        tuple<int, double> tuple_alloc_copy{allocator_arg_t{}, alloc, tuple_alloc};
        tuple<int, double> tuple_alloc_move{allocator_arg_t{}, alloc, std::move(tuple_alloc)};

        swap(tuple_alloc, tuple_alloc_copy);
        tuple_alloc.swap(tuple_alloc_copy);
    }

    // test array::swap
    {
        array<int, 2> array1{1, 2};
        array<int, 2> array2{3, 4};

        swap(array1, array2);
        assert(array1[0] == 3 && array1[1] == 4 && array2[0] == 1 && array2[1] == 2);

        array1.swap(array2);
        assert(array1[0] == 1 && array1[1] == 2 && array2[0] == 3 && array2[1] == 4);

        array<int, 0> array_empty1 = {};
        array<int, 0> array_empty2 = {};

        swap(array_empty1, array_empty2);
        array_empty1.swap(array_empty2);
    }

    // test array::fill
    {
        array<int, 2> foo = {};
        foo.fill(1);
        assert(foo[0] == 1 && foo[1] == 1);
    }

    // test back_inserter
    {
        constexpr_container input;
        int toBeMoved = 5;
        auto tested   = back_inserter(input);

        *tested++   = 42;
        *(++tested) = 1729;
        *tested++   = 1234;
        tested      = 4;
        tested      = std::move(toBeMoved);

        assert(input.buffer[0] == 42 && input.buffer[1] == 1729 && input.buffer[2] == 1234 && input.buffer[3] == 4
               && input.buffer[4] == 5 && input.buffer[5] == 0);
    }

    return true;
}

int main() {
    static_assert(run_tests());
    assert(run_tests());
}
