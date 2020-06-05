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
        fill(dest_true.begin(), prev(dest_true.end(), 3), true);
        assert(dest_true == result_true);

        vector<bool> result_false(length, false);
        result_false.resize(length + 3, true);
        vector<bool> dest_false(length + 3, true);
        fill(dest_false.begin(), prev(dest_false.end(), 3), false);
        assert(dest_false == result_false);
    }

    // With offset
    {
        vector<bool> result_true(length, true);
        result_true.resize(length + 3, false);
        result_true.insert(result_true.begin(), false);
        vector<bool> dest_true(length + 4, false);
        fill(next(dest_true.begin()), prev(dest_true.end(), 3), true);
        assert(dest_true == result_true);

        vector<bool> result_false(length, false);
        result_false.resize(length + 3, true);
        result_false.insert(result_false.begin(), true);
        vector<bool> dest_false(length + 4, true);
        fill(next(dest_false.begin()), prev(dest_false.end(), 3), false);
        assert(dest_false == result_false);
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
