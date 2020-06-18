// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <assert.h>
#include <vector>

using namespace std;

constexpr int blockSize = 32;
static_assert(blockSize == _VBITS, "Invalid block size");

void test_fill_helper(const size_t length) {
    // No offset
    {
        vector<bool> result_true(length, true);
        result_true.resize(length + 3, false);
        vector<bool> dest_true(length + 3, false);
        fill(dest_true.cbegin(), prev(dest_true.cend(), 3), true);
        assert(dest_true == result_true);

        vector<bool> result_false(length, false);
        result_false.resize(length + 3, true);
        vector<bool> dest_false(length + 3, true);
        fill(dest_false.cbegin(), prev(dest_false.cend(), 3), false);
        assert(dest_false == result_false);

        vector<bool> result_true_n(length, true);
        result_true_n.resize(length + 3, false);
        vector<bool> dest_true_n(length + 3, false);
        const auto res_fill_n = fill_n(dest_true_n.cbegin(), length, true);
        assert(dest_true_n == result_true_n);
        assert(res_fill_n == prev(dest_true_n.cend(), 3));

        vector<bool> result_false_n(length, false);
        result_false_n.resize(length + 3, true);
        vector<bool> dest_false_n(length + 3, true);
        fill_n(dest_false_n.cbegin(), length, false);
        assert(dest_false_n == result_false_n);
    }

    // With offset
    {
        vector<bool> result_true(length, true);
        result_true.resize(length + 3, false);
        result_true.insert(result_true.cbegin(), false);
        vector<bool> dest_true(length + 4, false);
        fill(next(dest_true.cbegin()), prev(dest_true.cend(), 3), true);
        assert(dest_true == result_true);

        vector<bool> result_false(length, false);
        result_false.resize(length + 3, true);
        result_false.insert(result_false.cbegin(), true);
        vector<bool> dest_false(length + 4, true);
        fill(next(dest_false.cbegin()), prev(dest_false.cend(), 3), false);
        assert(dest_false == result_false);

        vector<bool> result_true_n(length, true);
        result_true_n.resize(length + 3, false);
        result_true_n.insert(result_true_n.cbegin(), false);
        vector<bool> dest_true_n(length + 4, false);
        const auto res_fill_n = fill_n(next(dest_true_n.cbegin()), length, true);
        assert(dest_true_n == result_true_n);
        assert(res_fill_n == prev(dest_true_n.cend(), 3));

        vector<bool> result_false_n(length, false);
        result_false_n.resize(length + 3, true);
        result_false_n.insert(result_false_n.cbegin(), true);
        vector<bool> dest_false_n(length + 4, true);
        fill_n(next(dest_false_n.cbegin()), length, false);
        assert(dest_false_n == result_false_n);
    }
}

bool test_fill() {
    // Empty
    test_fill_helper(0);

    // One block, ends within block
    test_fill_helper(15);

    // One block, ends at block boundary
    test_fill_helper(blockSize);

    // Multiple blocks, no memset, within block
    test_fill_helper(blockSize + 11);

    // Multiple blocks, no memset, ends at block boundary
    test_fill_helper(2 * blockSize);

    // Multiple blocks, with memset, within block
    test_fill_helper(3 * blockSize + 5);

    // Multiple blocks, with memset, ends at block boundary
    test_fill_helper(4 * blockSize);
    return true;
}

int main() {
    test_fill();
}
