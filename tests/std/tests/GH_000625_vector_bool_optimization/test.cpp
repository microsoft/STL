// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <cstdlib>
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

        vector<bool> result_true_n(length, true);
        result_true_n.resize(length + 3, false);
        vector<bool> dest_true_n(length + 3, false);
        const auto res_fill_n = fill_n(dest_true_n.begin(), length, true);
        assert(dest_true_n == result_true_n);
        assert(res_fill_n == prev(dest_true_n.end(), 3));

        vector<bool> result_false_n(length, false);
        result_false_n.resize(length + 3, true);
        vector<bool> dest_false_n(length + 3, true);
        fill_n(dest_false_n.begin(), length, false);
        assert(dest_false_n == result_false_n);
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

        vector<bool> result_true_n(length, true);
        result_true_n.resize(length + 3, false);
        result_true_n.insert(result_true_n.begin(), false);
        vector<bool> dest_true_n(length + 4, false);
        const auto res_fill_n = fill_n(next(dest_true_n.begin()), length, true);
        assert(dest_true_n == result_true_n);
        assert(res_fill_n == prev(dest_true_n.end(), 3));

        vector<bool> result_false_n(length, false);
        result_false_n.resize(length + 3, true);
        result_false_n.insert(result_false_n.begin(), true);
        vector<bool> dest_false_n(length + 4, true);
        fill_n(next(dest_false_n.begin()), length, false);
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

void test_find_helper(const size_t length) {
    // No offset
    {
        vector<bool> input_true(length + 3, false);
        input_true.resize(length + 6, true);
        input_true[length + 1].flip();
        const auto result_true = find(input_true.cbegin(), prev(input_true.cend(), 3), true);
        assert(result_true == next(input_true.cbegin(), static_cast<ptrdiff_t>(length + 1)));

        vector<bool> input_false(length + 3, true);
        input_false.resize(length + 6, false);
        input_false[length + 1].flip();
        const auto result_false = find(input_false.cbegin(), prev(input_false.cend(), 3), false);
        assert(result_false == next(input_false.cbegin(), static_cast<ptrdiff_t>(length + 1)));
    }

    // With offset
    {
        vector<bool> input_true(length + 3, false);
        input_true.resize(length + 6, true);
        input_true[length + 1].flip();
        input_true[0].flip();
        const auto result_true = find(next(input_true.cbegin()), prev(input_true.cend(), 3), true);
        assert(result_true == next(input_true.cbegin(), static_cast<ptrdiff_t>(length + 1)));

        vector<bool> input_false(length + 3, true);
        input_false.resize(length + 6, false);
        input_false[length + 1].flip();
        input_false[0].flip();
        const auto result_false = find(next(input_false.cbegin()), prev(input_false.cend(), 3), false);
        assert(result_false == next(input_false.cbegin(), static_cast<ptrdiff_t>(length + 1)));
    }
}

bool test_find() {
    // Empty range
    test_find_helper(0);

    // One block, ends within block
    test_find_helper(15);

    // One block, ends at block boundary
    test_find_helper(blockSize);

    // Multiple blocks, within block
    test_find_helper(3 * blockSize + 5);

    // Multiple blocks, ends at block boundary
    test_find_helper(4 * blockSize);
    return true;
}

void test_count_helper(const ptrdiff_t length) {
    // This test data is not random, but irregular enough to ensure confidence in the tests
    // clang-format off
    vector<bool> source = { true, false, true, false, true, true, true, false,
                            true, false, true, false, true, true, true, false,
                            true, false, true, false, true, true, true, false,
                            true, false, true, false, true, true, true, false,
                            true, false, true, false, true, true, true, false,
                            true, false, true, false, true, true, true, false,
                            true, false, true, false, true, true, true, false,
                            true, false, true, false, true, true, true, false,
                            true, false, true, false, true, true, true, false,
                            true, false, true, false, true, true, true, false,
                            true, false, true, false, true, true, true, false,
                            true, false, true, false, true, true, true, false,
                            true, false, true, false, true, true, true, false,
                            true, false, true, false, true, true, true, false,
                            true, false, true, false, true, true, true, false,
                            true, false, true, false, true, true, true, false,
                            true, false, true, false, true, true, true, false
                        };
    const int counts_true[8] = { 0, 1, 1, 2, 2, 3, 4, 5 };
    const int counts_false[8] = { 0, 0, 1, 1, 2, 2, 2, 2 };
    // clang-format on
    const auto expected = div(static_cast<int>(length), 8);
    // No offset
    {
        const auto result_true = static_cast<int>(count(source.cbegin(), next(source.cbegin(), length), true));
        assert(result_true == expected.quot * 5 + counts_true[expected.rem]);

        const auto result_false = static_cast<int>(count(source.cbegin(), next(source.cbegin(), length), false));
        assert(result_false == expected.quot * 3 + counts_false[expected.rem]);
    }

    // With offset
    {
        const auto result_true =
            static_cast<int>(count(next(source.cbegin(), 2), next(source.cbegin(), length + 2), true));
        assert(result_true == expected.quot * 5 + counts_true[expected.rem]);

        const auto result_false =
            static_cast<int>(count(next(source.cbegin(), 2), next(source.cbegin(), length + 2), false));
        assert(result_false == expected.quot * 3 + counts_false[expected.rem]);
    }
}

bool test_count() {
    // Empty range
    test_count_helper(0);

    // One block, ends within block
    test_count_helper(15);

    // One block, ends at block boundary
    test_count_helper(blockSize);

    // Multiple blocks, within block
    test_count_helper(3 * blockSize + 8);

    // Multiple blocks, ends at block boundary
    test_count_helper(4 * blockSize);
    return true;
}

int main() {
    test_fill();
    test_find();
    test_count();
}
