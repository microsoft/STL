// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <numeric>
#include <utility>
#include <vector>

using namespace std;

class MoveCopyCounterType {
public:
    int moves  = 0;
    int copies = 0;

    MoveCopyCounterType() = default;

    MoveCopyCounterType(const MoveCopyCounterType& o) {
        copies = o.copies + 1;
        moves  = o.moves;
    }

    MoveCopyCounterType(MoveCopyCounterType&& o) {
        moves  = o.moves + 1;
        copies = o.copies;
    }

    MoveCopyCounterType& operator=(const MoveCopyCounterType&) = default;
};

struct CopyFirstArgBinOpFunctor {
    MoveCopyCounterType operator()(MoveCopyCounterType a, const MoveCopyCounterType&) {
        return a;
    }
};

struct CopySecondArgBinOpFunctor {
    MoveCopyCounterType operator()(const MoveCopyCounterType&, MoveCopyCounterType b) {
        return b;
    }
};

void assert_accumulate() {
    vector<MoveCopyCounterType> test_vector(10);
    MoveCopyCounterType output;
    output = accumulate(begin(test_vector), end(test_vector), MoveCopyCounterType{}, CopyFirstArgBinOpFunctor{});
    if (_HAS_CXX20) {
        assert(output.copies == 0);
        assert(output.moves == 21);
    } else {
        assert(output.copies == 10);
        assert(output.moves == 11);
    }
}

void assert_inner_product() {
    vector<MoveCopyCounterType> test_vector1(10);
    vector<MoveCopyCounterType> test_vector2(10);
    MoveCopyCounterType output;
    output = inner_product(begin(test_vector1), end(test_vector1), begin(test_vector2), MoveCopyCounterType{},
        CopyFirstArgBinOpFunctor{}, CopyFirstArgBinOpFunctor{});
    if (_HAS_CXX20) {
        assert(output.copies == 0);
        assert(output.moves == 21);
    } else {
        assert(output.copies == 10);
        assert(output.moves == 11);
    }
}

void assert_partial_sum() {
    vector<MoveCopyCounterType> test_vector(10);
    const auto output_end =
        partial_sum(begin(test_vector), end(test_vector), begin(test_vector), CopyFirstArgBinOpFunctor{});
    for (auto it = begin(test_vector); it != output_end; ++it) {
        if (_HAS_CXX20) {
            assert(it->copies == 1);
            assert(it->moves == (it - begin(test_vector)) * 2);
        } else {
            assert(it->copies == it - begin(test_vector) + 1);
            assert(it->moves == it - begin(test_vector));
        }
    }
}

void assert_adjacent_difference() {
    vector<MoveCopyCounterType> test_vector1(10);
    vector<MoveCopyCounterType> test_vector2(10);
    const auto output_end =
        adjacent_difference(begin(test_vector1), end(test_vector1), begin(test_vector2), CopySecondArgBinOpFunctor{});
    assert(test_vector2[0].moves == 0);
    assert(test_vector2[0].copies == 1);
    for (auto it = begin(test_vector2) + 1; it != output_end; ++it) {
        if (_HAS_CXX20) {
            assert(it->moves == 2);
            assert(it->copies == 1);
        } else {
            assert(it->moves == 1);
            assert(it->copies == 2);
        }
    }
}

int main() {
    assert_accumulate();
    assert_inner_product();
    assert_partial_sum();
    assert_adjacent_difference();
}
