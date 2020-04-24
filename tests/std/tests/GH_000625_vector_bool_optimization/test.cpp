// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <assert.h>
#include <iterator>
#include <vector>

using namespace std;

constexpr int blockSize = 32;
static_assert(blockSize == _VBITS, "Invalid block size");

// clang-format off
vector<bool> source = { true, false, true, false, true, true, true, false,
                        true, false, true, false, true, true, true, false,
                        true, false, true, false, true, true, true, false,
                        true, false, true, false, true, true, true, false,
                        true, false, true, false, true, true, true, false,
                        true, false, true, false, true, true, true, false,
                        true, false, true, false, true, true, true, false,
                        true, false, true, false, true, true, true, false,
                        true, false, true, false, true, true, true, false };
// clang-format on

bool test_copy() {
    // No offset, less than blockSize
    {
        vector<bool> result = {true, false, true, false, true, true, true, false};
        vector<bool> dest(8, false);
        const auto res_copy = copy(source.begin(), next(source.begin(), 8), dest.begin());
        assert(dest == result);
        assert(res_copy == dest.end());

        vector<bool> dest_n(8, false);
        const auto res_copy_n = copy_n(source.begin(), 8, dest_n.begin());
        assert(dest_n == result);
        assert(res_copy_n == dest_n.end());

        vector<bool> dest_backward(8, false);
        const auto res_copy_backward = copy_backward(source.begin(), next(source.begin(), 8), dest_backward.end());
        assert(dest_backward == result);
        assert(res_copy_backward == dest_backward.begin());
    }

    // With offset source, less than blockSize
    {
        vector<bool> result = {false, true, false, true, true, true, false, true};
        vector<bool> dest(8, false);
        const auto res_copy = copy(next(source.begin()), next(source.begin(), 9), dest.begin());
        assert(dest == result);
        assert(res_copy == dest.end());

        vector<bool> dest_n(8, false);
        const auto res_copy_n = copy_n(next(source.begin()), 8, dest_n.begin());
        assert(dest_n == result);
        assert(res_copy_n == dest_n.end());

        vector<bool> dest_backward(8, false);
        const auto res_copy_backward =
            copy_backward(next(source.begin()), next(source.begin(), 9), dest_backward.end());
        assert(dest_backward == result);
        assert(res_copy_backward == dest_backward.begin());
    }

    // With offset dest, less than blockSize
    {
        vector<bool> result = {false, true, false, true, false, true, true, true, false};
        vector<bool> dest(9, false);
        const auto res_copy = copy(source.begin(), next(source.begin(), 7), next(dest.begin()));
        assert(dest == result);
        assert(res_copy == prev(dest.end()));

        vector<bool> dest_n(9, false);
        const auto res_copy_n = copy_n(source.begin(), 7, next(dest_n.begin()));
        assert(dest_n == result);
        assert(res_copy_n == prev(dest_n.end()));

        vector<bool> dest_backward(9, false);
        const auto res_copy_backward =
            copy_backward(source.begin(), next(source.begin(), 7), prev(dest_backward.end()));
        assert(dest_backward == result);
        assert(res_copy_backward == next(dest_backward.begin()));
    }

    // With offset match, less than blockSize
    {
        vector<bool> result = {false, false, true, false, true, true, true, false, false};
        vector<bool> dest(9, false);
        const auto res_copy = copy(next(source.begin(), 1), next(source.begin(), 8), next(dest.begin()));
        assert(dest == result);
        assert(res_copy == prev(dest.end()));

        vector<bool> dest_n(9, false);
        const auto res_copy_n = copy_n(next(source.begin(), 1), 7, next(dest_n.begin()));
        assert(dest_n == result);
        assert(res_copy_n == prev(dest_n.end()));

        vector<bool> dest_backward(9, false);
        const auto res_copy_backward =
            copy_backward(next(source.begin(), 1), next(source.begin(), 8), prev(dest_backward.end()));
        assert(dest_backward == result);
        assert(res_copy_backward == next(dest_backward.begin()));
    }

    // With offset missmatch, less than blockSize
    {
        vector<bool> result = {false, true, false, true, true, true, false, true, false};
        vector<bool> dest(9, false);
        const auto res_copy = copy(next(source.begin(), 2), next(source.begin(), 10), next(dest.begin()));
        assert(dest == result);
        assert(res_copy == dest.end());

        vector<bool> dest_n(9, false);
        const auto res_copy_n = copy_n(next(source.begin(), 2), 8, next(dest_n.begin()));
        assert(dest_n == result);
        assert(res_copy_n == dest_n.end());

        vector<bool> dest_backward(9, false);
        const auto res_copy_backward =
            copy_backward(next(source.begin(), 2), next(source.begin(), 10), dest_backward.end());
        assert(dest_backward == result);
        assert(res_copy_backward == next(dest_backward.begin()));
    }

    // No offset, exactly blockSize
    {
        // clang-format off
        vector<bool> result = { true, false, true, false, true, true, true, false,
                                true, false, true, false, true, true, true, false,
                                true, false, true, false, true, true, true, false,
                                true, false, true, false, true, true, true, false };
        // clang-format on
        vector<bool> dest(blockSize, false);
        const auto res_copy = copy(source.begin(), next(source.begin(), blockSize), dest.begin());
        assert(dest == result);
        assert(res_copy == dest.end());

        vector<bool> dest_n(blockSize, false);
        const auto res_copy_n = copy_n(source.begin(), blockSize, dest_n.begin());
        assert(dest_n == result);
        assert(res_copy_n == dest_n.end());

        vector<bool> dest_backward(blockSize, false);
        const auto res_copy_backward =
            copy_backward(source.begin(), next(source.begin(), blockSize), dest_backward.end());
        assert(dest_backward == result);
        assert(res_copy_backward == dest_backward.begin());
    }

    // With offset, end on boundary
    {
        // clang-format off
        vector<bool> result = { false, false, true, false, true, true, true, false,
                                true, false, true, false, true, true, true, false,
                                true, false, true, false, true, true, true, false,
                                true, false, true, false, true, true, true, false };
        // clang-format on
        vector<bool> dest(blockSize, false);
        const auto res_copy = copy(next(source.begin()), next(source.begin(), blockSize), next(dest.begin()));
        assert(dest == result);
        assert(res_copy == dest.end());

        vector<bool> dest_n(blockSize, false);
        const auto res_copy_n = copy_n(next(source.begin()), blockSize - 1, next(dest_n.begin()));
        assert(dest_n == result);
        assert(res_copy_n == dest_n.end());

        vector<bool> dest_backward(blockSize, false);
        const auto res_copy_backward =
            copy_backward(next(source.begin()), next(source.begin(), blockSize), dest_backward.end());
        assert(dest_backward == result);
        assert(res_copy_backward == next(dest_backward.begin()));
    }

    // With offset, exactly blockSize
    {
        // clang-format off
        vector<bool> result = { false, false, true, false, true, true, true, false,
                                true, false, true, false, true, true, true, false,
                                true, false, true, false, true, true, true, false,
                                true, false, true, false, true, true, true, false };
        // clang-format on
        vector<bool> dest(blockSize, false);
        const auto res_copy = copy(next(source.begin()), next(source.begin(), blockSize), next(dest.begin()));
        assert(dest == result);
        assert(res_copy == dest.end());

        vector<bool> dest_n(blockSize, false);
        const auto res_copy_n = copy_n(next(source.begin()), blockSize - 1, next(dest_n.begin()));
        assert(dest_n == result);
        assert(res_copy_n == dest_n.end());

        vector<bool> dest_backward(blockSize, false);
        const auto res_copy_backward =
            copy_backward(next(source.begin()), next(source.begin(), blockSize), dest_backward.end());
        assert(dest_backward == result);
        assert(res_copy_backward == next(dest_backward.begin()));
    }

    // With offset missmatch
    {
        // clang-format off
        vector<bool> result = { false, false,
                                      false, true, false, true, true, true, false,
                                true, false, true, false, true, true, true, false,
                                true, false, true, false, true, true, true, false,
                                true, false, true, false, true, true, true, false,
                                true, false };
        // clang-format on
        vector<bool> dest(blockSize + 3, false);
        const auto res_copy = copy(next(source.begin()), next(source.begin(), blockSize + 2), next(dest.begin(), 2));
        assert(dest == result);
        assert(res_copy == dest.end());

        vector<bool> dest_n(blockSize + 3, false);
        const auto res_copy_n = copy_n(next(source.begin()), blockSize + 1, next(dest_n.begin(), 2));
        assert(dest_n == result);
        assert(res_copy_n == dest_n.end());

        vector<bool> dest_backward(blockSize + 3, false);
        const auto res_copy_backward =
            copy_backward(next(source.begin()), next(source.begin(), blockSize + 2), dest_backward.end());
        assert(dest_backward == result);
        assert(res_copy_backward == next(dest_backward.begin(), 2));
    }

    // No offset, multiple blockSize
    {
        // clang-format off
        vector<bool> result = { true, false, true, false, true, true, true, false,
                                true, false, true, false, true, true, true, false,
                                true, false, true, false, true, true, true, false,
                                true, false, true, false, true, true, true, false,
                                true, false, true, false, true, true, true, false,
                                true, false, true, false, true, true, true, false,
                                true, false, true, false, true, true, true, false,
                                true, false, true, false, true, true, true, false };
        // clang-format on
        vector<bool> dest(2 * blockSize, false);
        const auto res_copy = copy(source.begin(), next(source.begin(), 2 * blockSize), dest.begin());
        assert(dest == result);
        assert(res_copy == dest.end());

        vector<bool> dest_n(2 * blockSize, false);
        const auto res_copy_n = copy_n(source.begin(), 2 * blockSize, dest_n.begin());
        assert(dest_n == result);
        assert(res_copy_n == dest_n.end());

        vector<bool> dest_backward(2 * blockSize, false);
        const auto res_copy_backward =
            copy_backward(source.begin(), next(source.begin(), 2 * blockSize), dest_backward.end());
        assert(dest_backward == result);
        assert(res_copy_backward == dest_backward.begin());
    }

    // With offset, multiple blockSize
    {
        // clang-format off
        vector<bool> result = { false, false, true, false, true, true, true, false,
                                true, false, true, false, true, true, true, false,
                                true, false, true, false, true, true, true, false,
                                true, false, true, false, true, true, true, false,
                                true, false, true, false, true, true, true, false,
                                true, false, true, false, true, true, true, false,
                                true, false, true, false, true, true, true, false,
                                true, false, true, false, true, true, true, false };
        // clang-format on
        vector<bool> dest(2 * blockSize, false);
        const auto res_copy = copy(next(source.begin()), next(source.begin(), 2 * blockSize), next(dest.begin()));
        assert(dest == result);
        assert(res_copy == dest.end());

        vector<bool> dest_n(2 * blockSize, false);
        const auto res_copy_n = copy_n(next(source.begin()), 2 * blockSize - 1, next(dest_n.begin()));
        assert(dest_n == result);
        assert(res_copy_n == dest_n.end());

        vector<bool> dest_backward(2 * blockSize, false);
        const auto res_copy_backward =
            copy_backward(next(source.begin()), next(source.begin(), 2 * blockSize), dest_backward.end());
        assert(dest_backward == result);
        assert(res_copy_backward == next(dest_backward.begin()));
    }

    // With offset missmatch, multiple blockSize
    {
        // clang-format off
        vector<bool> result = { false, false,
                                      false, true, false, true, true, true, false,
                                true, false, true, false, true, true, true, false,
                                true, false, true, false, true, true, true, false,
                                true, false, true, false, true, true, true, false,
                                true, false, true, false, true, true, true, false,
                                true, false, true, false, true, true, true, false,
                                true, false, true, false, true, true, true, false,
                                true, false, true, false, true, true, true, false,
                                true, false };
        // clang-format on
        vector<bool> dest(2 * blockSize + 3, false);
        const auto res_copy =
            copy(next(source.begin()), next(source.begin(), 2 * blockSize + 2), next(dest.begin(), 2));
        assert(dest == result);
        assert(res_copy == dest.end());

        vector<bool> dest_n(2 * blockSize + 3, false);
        const auto res_copy_n = copy_n(next(source.begin()), 2 * blockSize + 1, next(dest_n.begin(), 2));
        assert(dest_n == result);
        assert(res_copy_n == dest_n.end());

        vector<bool> dest_backward(2 * blockSize + 3, false);
        const auto res_copy_backward =
            copy_backward(next(source.begin()), next(source.begin(), 2 * blockSize + 2), dest_backward.end());
        assert(dest_backward == result);
        assert(res_copy_backward == next(dest_backward.begin(), 2));
    }

    return true;
}

int main() {
    test_copy();
}
