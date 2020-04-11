// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <array>
#include <assert.h>
#include <functional>
#include <iterator>
#include <memory>
#include <stddef.h>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>

using namespace std;

struct constexpr_container {
    using value_type = int;
    using iterator   = array<value_type, 6>::iterator;

    array<value_type, 6> buffer{};
    size_t selected = 0;

    // Test back_insert_iterator
    constexpr void push_back(const value_type i) {
        buffer[selected++] = i;
    }

    // Test front_insert_iterator
    constexpr void push_front(const value_type i) {
        buffer[selected++] = i;
    }

    // Test insert_iterator
    constexpr iterator insert(iterator where, const value_type i) {
        *where = i;
        return where;
    }

    // Fake begin to ensure that we insert at the correct position
    // for insert_iterator
    constexpr iterator begin() {
        return next(buffer.begin());
    }
};

constexpr bool run_tests() {
    // test pair piecewise constructor
    {
        tuple<int, int, double, double> first{1, 2, 0.5, 2.0};
        tuple<double, int> second{2.0, 1};
        pair<tuple<int, int, double, double>, tuple<double, int>> meow{piecewise_construct, first, second};
    }

    // test pair assignment operator
    {
        pair<int, int> input{1, 2};
        pair<int, int> copyAssignment;
        copyAssignment = input;

        pair<int, int> moveAssignment;
        moveAssignment = move(input);

        pair<double, double> copyAssignmentConvertible;
        copyAssignmentConvertible = moveAssignment;

        pair<double, double> moveAssignmentConvertible;
        moveAssignmentConvertible = move(moveAssignment);
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
        tuple<> tuple_alloc{allocator_arg, alloc};
        tuple<> tuple_alloc_copy{allocator_arg, alloc, tuple_alloc};
        tuple<> tuple_alloc_value{_Alloc_exact_args_t{}, alloc};

        swap(tuple_alloc, tuple_alloc_copy);
        tuple_alloc.swap(tuple_alloc_copy);
    }

    // test tuple
    {
        allocator<int> alloc;
        tuple<short, int> conversionInput{static_cast<short>(1), 1};
        const tuple<short, int> constConversionInput{static_cast<short>(1), 1};
        pair<short, int> conversionInputPair{static_cast<short>(1), 1};
        const pair<short, int> constConversionInputPair{static_cast<short>(1), 1};

        tuple<int, double> tuple_alloc{allocator_arg, alloc};
        tuple<int, double> tuple_alloc_value{allocator_arg, alloc, 1, 2.0};
        tuple<int, double> tuple_alloc_conversion{allocator_arg, alloc, static_cast<short>(1), 1};
        tuple<int, double> tuple_alloc_conversion_tuple{allocator_arg, alloc, conversionInput};
        tuple<int, double> tuple_alloc_conversion_const_tuple{allocator_arg, alloc, constConversionInput};
        tuple<int, double> tuple_alloc_conversion_pair{allocator_arg, alloc, conversionInputPair};
        tuple<int, double> tuple_alloc_conversion_const_pair{allocator_arg, alloc, constConversionInputPair};
        tuple<int, double> tuple_alloc_copy{allocator_arg, alloc, tuple_alloc};
        tuple<int, double> tuple_alloc_move{allocator_arg, alloc, move(tuple_alloc)};

        swap(tuple_alloc, tuple_alloc_copy);
        tuple_alloc.swap(tuple_alloc_copy);
    }

    // test array::swap
    {
        array<int, 2> array1{{1, 2}};
        array<int, 2> array2{{3, 4}};

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
        array<int, 2> meow = {};
        meow.fill(1);
        assert(meow[0] == 1 && meow[1] == 1);
    }

    // test back_inserter
    {
        constexpr_container input;
        int toBeMoved = 5;
        auto tested   = back_inserter(input);

        *tested++ = 42;
        *++tested = 1729;
        *tested++ = 1234;
        tested    = 4;
        tested    = move(toBeMoved);

        assert(input.buffer[0] == 42 && input.buffer[1] == 1729 && input.buffer[2] == 1234 && input.buffer[3] == 4
               && input.buffer[4] == 5 && input.buffer[5] == 0);
    }

    // test front_inserter
    {
        constexpr_container input;
        int toBeMoved = 5;
        auto tested   = front_inserter(input);

        *tested++ = 42;
        *++tested = 1729;
        *tested++ = 1234;
        tested    = 4;
        tested    = move(toBeMoved);

        assert(input.buffer[0] == 42 && input.buffer[1] == 1729 && input.buffer[2] == 1234 && input.buffer[3] == 4
               && input.buffer[4] == 5 && input.buffer[5] == 0);
    }

    // test insert_inserter
    {
        constexpr_container input;
        int toBeMoved = 5;
        auto tested   = inserter(input, input.begin());

        *tested++ = 42;
        *++tested = 1729;
        *tested++ = 1234;
        tested    = 4;
        tested    = move(toBeMoved);

        assert(input.buffer[0] == 0 && input.buffer[1] == 42 && input.buffer[2] == 1729 && input.buffer[3] == 1234
               && input.buffer[4] == 4 && input.buffer[5] == 5);
    }

    // test default_searcher
    {
        string_view in     = "This is some string";
        string_view needle = "some";
        default_searcher meow{needle.begin(), needle.end()};
        auto [first, second] = meow(in.begin(), in.end());
    }

    return true;
}

int main() {
    static_assert(run_tests());
    assert(run_tests());
}
