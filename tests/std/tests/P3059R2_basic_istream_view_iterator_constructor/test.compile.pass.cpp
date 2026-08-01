// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <concepts>
#include <ranges>
#include <vector>
#include <cassert>
#include <sstream>

using namespace std;

using View     = ranges::basic_istream_view<int, char>;
using Iterator = ranges::iterator_t<View>;

// P3059R2: users must not be able to construct
// basic_istream_view::iterator directly from the parent view.
static_assert(!constructible_from<Iterator, View&>);

// The iterator must remain non-copyable but movable through its public interface.
static_assert(!copy_constructible<Iterator>);
static_assert(move_constructible<Iterator>);


//Verify normal stream extraction through the iterator returned by begin().
bool test_basic_istream_view() {
    istringstream input{"10 20 30 40"};
    auto view = ranges::istream_view<int>(input);

    vector<int> result;

    for (const int value : view) {
        result.push_back(value);
    }
    return result == vector{10, 20, 30, 40};
}

int main()
{
    assert(test_basic_istream_view());
}

