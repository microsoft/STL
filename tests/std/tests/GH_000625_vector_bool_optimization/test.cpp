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

        vector<bool> dest_move(8, false);
        const auto res_move = move(source.begin(), next(source.begin(), 8), dest_move.begin());
        assert(dest_move == result);
        assert(res_move == dest_move.end());

        vector<bool> dest_move_backward(8, false);
        const auto res_move_backward = move_backward(source.begin(), next(source.begin(), 8), dest_move_backward.end());
        assert(dest_move_backward == result);
        assert(res_move_backward == dest_move_backward.begin());
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

        vector<bool> dest_move(8, false);
        const auto res_move = move(next(source.begin()), next(source.begin(), 9), dest_move.begin());
        assert(dest_move == result);
        assert(res_move == dest_move.end());

        vector<bool> dest_move_backward(8, false);
        const auto res_move_backward =
            move_backward(next(source.begin()), next(source.begin(), 9), dest_move_backward.end());
        assert(dest_move_backward == result);
        assert(res_move_backward == dest_move_backward.begin());
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

        vector<bool> dest_move(9, false);
        const auto res_move = move(source.begin(), next(source.begin(), 7), next(dest_move.begin()));
        assert(dest_move == result);
        assert(res_move == prev(dest_move.end()));

        vector<bool> dest_move_backward(9, false);
        const auto res_move_backward =
            move_backward(source.begin(), next(source.begin(), 7), prev(dest_move_backward.end()));
        assert(dest_move_backward == result);
        assert(res_move_backward == next(dest_move_backward.begin()));
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

        vector<bool> dest_move(9, false);
        const auto res_move = move(next(source.begin(), 1), next(source.begin(), 8), next(dest_move.begin()));
        assert(dest_move == result);
        assert(res_move == prev(dest_move.end()));

        vector<bool> dest_move_backward(9, false);
        const auto res_move_backward =
            move_backward(next(source.begin(), 1), next(source.begin(), 8), prev(dest_move_backward.end()));
        assert(dest_move_backward == result);
        assert(res_move_backward == next(dest_move_backward.begin()));
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

        vector<bool> dest_move(9, false);
        const auto res_move = move(next(source.begin(), 2), next(source.begin(), 10), next(dest_move.begin()));
        assert(dest_move == result);
        assert(res_move == dest_move.end());

        vector<bool> dest_move_backward(9, false);
        const auto res_move_backward =
            move_backward(next(source.begin(), 2), next(source.begin(), 10), dest_move_backward.end());
        assert(dest_move_backward == result);
        assert(res_move_backward == next(dest_move_backward.begin()));
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

        vector<bool> dest_move(blockSize, false);
        const auto res_move = move(source.begin(), next(source.begin(), blockSize), dest_move.begin());
        assert(dest_move == result);
        assert(res_move == dest_move.end());

        vector<bool> dest_move_backward(blockSize, false);
        const auto res_move_backward =
            move_backward(source.begin(), next(source.begin(), blockSize), dest_move_backward.end());
        assert(dest_move_backward == result);
        assert(res_move_backward == dest_move_backward.begin());
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

        vector<bool> dest_move(blockSize, false);
        const auto res_move = move(next(source.begin()), next(source.begin(), blockSize), next(dest_move.begin()));
        assert(dest_move == result);
        assert(res_move == dest_move.end());

        vector<bool> dest_move_backward(blockSize, false);
        const auto res_move_backward =
            move_backward(next(source.begin()), next(source.begin(), blockSize), dest_move_backward.end());
        assert(dest_move_backward == result);
        assert(res_move_backward == next(dest_move_backward.begin()));
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

        vector<bool> dest_move(blockSize, false);
        const auto res_move = move(next(source.begin()), next(source.begin(), blockSize), next(dest_move.begin()));
        assert(dest_move == result);
        assert(res_move == dest_move.end());

        vector<bool> dest_move_backward(blockSize, false);
        const auto res_move_backward =
            move_backward(next(source.begin()), next(source.begin(), blockSize), dest_move_backward.end());
        assert(dest_move_backward == result);
        assert(res_move_backward == next(dest_move_backward.begin()));
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

        vector<bool> dest_move(blockSize + 3, false);
        const auto res_move =
            move(next(source.begin()), next(source.begin(), blockSize + 2), next(dest_move.begin(), 2));
        assert(dest_move == result);
        assert(res_move == dest_move.end());

        vector<bool> dest_move_backward(blockSize + 3, false);
        const auto res_move_backward =
            move_backward(next(source.begin()), next(source.begin(), blockSize + 2), dest_move_backward.end());
        assert(dest_move_backward == result);
        assert(res_move_backward == next(dest_move_backward.begin(), 2));
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

        vector<bool> dest_move(2 * blockSize, false);
        const auto res_move = move(source.begin(), next(source.begin(), 2 * blockSize), dest_move.begin());
        assert(dest_move == result);
        assert(res_move == dest_move.end());

        vector<bool> dest_move_backward(2 * blockSize, false);
        const auto res_move_backward =
            move_backward(source.begin(), next(source.begin(), 2 * blockSize), dest_move_backward.end());
        assert(dest_move_backward == result);
        assert(res_move_backward == dest_move_backward.begin());
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

        vector<bool> dest_move(2 * blockSize, false);
        const auto res_move = move(next(source.begin()), next(source.begin(), 2 * blockSize), next(dest_move.begin()));
        assert(dest_move == result);
        assert(res_move == dest_move.end());

        vector<bool> dest_move_backward(2 * blockSize, false);
        const auto res_move_backward =
            move_backward(next(source.begin()), next(source.begin(), 2 * blockSize), dest_move_backward.end());
        assert(dest_move_backward == result);
        assert(res_move_backward == next(dest_move_backward.begin()));
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

        vector<bool> dest_move(2 * blockSize + 3, false);
        const auto res_move =
            move(next(source.begin()), next(source.begin(), 2 * blockSize + 2), next(dest_move.begin(), 2));
        assert(dest_move == result);
        assert(res_move == dest_move.end());

        vector<bool> dest_move_backward(2 * blockSize + 3, false);
        const auto res_move_backward =
            move_backward(next(source.begin()), next(source.begin(), 2 * blockSize + 2), dest_move_backward.end());
        assert(dest_move_backward == result);
        assert(res_move_backward == next(dest_move_backward.begin(), 2));
    }

    return true;
}

bool test_fill() {
    // No offset, less than char size
    {
        vector<bool> result(3, true);
        vector<bool> dest(3, false);
        fill(dest.begin(), dest.end(), true);
        assert(dest == result);

        fill(dest.begin(), dest.end(), false);
        assert(dest == vector<bool>(3, false));

        const auto res_fill_n = fill_n(dest.begin(), 3, true);
        assert(dest == result);
        assert(res_fill_n == dest.end());

        fill_n(dest.begin(), 3, false);
        assert(dest == vector<bool>(3, false));
    }

    // With Offset, less than blockSize
    {
        vector<bool> result(3, true);
        result[0] = false;

        vector<bool> dest(3, false);
        fill(next(dest.begin()), dest.end(), true);
        assert(dest == result);

        fill(next(dest.begin()), dest.end(), false);
        assert(dest == vector<bool>(3, false));

        const auto res_fill_n = fill_n(next(dest.begin()), 2, true);
        assert(dest == result);
        assert(res_fill_n == dest.end());

        fill_n(next(dest.begin()), 2, false);
        assert(dest == vector<bool>(3, false));
    }

    // No offset, less than blockSize
    {
        vector<bool> result(8, true);
        vector<bool> dest(8, false);
        fill(dest.begin(), dest.end(), true);
        assert(dest == result);

        fill(dest.begin(), dest.end(), false);
        assert(dest == vector<bool>(8, false));
    }

    // With Offset, less than blockSize
    {
        vector<bool> result(8, true);
        result[0] = false;

        vector<bool> dest(8, false);
        fill(next(dest.begin()), dest.end(), true);
        assert(dest == result);

        fill(next(dest.begin()), dest.end(), false);
        assert(dest == vector<bool>(8, false));
    }

    // With offset, end at boundary
    {
        vector<bool> result(blockSize, true);
        result[0] = false;

        vector<bool> dest(blockSize, false);
        fill(next(dest.begin()), dest.end(), true);
        assert(dest == result);

        fill(next(dest.begin()), dest.end(), false);
        assert(dest == vector<bool>(blockSize, false));

        const auto res_fill_n = fill_n(next(dest.begin()), blockSize - 1, true);
        assert(dest == result);
        assert(res_fill_n == dest.end());

        fill_n(next(dest.begin()), blockSize - 1, false);
        assert(dest == vector<bool>(blockSize, false));
    }

    // No offset, exactly blockSize
    {
        vector<bool> result(blockSize, true);
        vector<bool> dest(blockSize, false);
        fill(dest.begin(), dest.end(), true);
        assert(dest == result);

        fill(dest.begin(), dest.end(), false);
        assert(dest == vector<bool>(blockSize, false));
    }

    // With offset, ends at boundary
    {
        vector<bool> result(blockSize, true);
        result[0] = false;

        vector<bool> dest(blockSize, false);
        fill(next(dest.begin()), dest.end(), true);
        assert(dest == result);

        fill(next(dest.begin()), dest.end(), false);
        assert(dest == vector<bool>(blockSize, false));
    }

    // With offset, exactly blockSize
    {
        vector<bool> result(blockSize + 1, true);
        result[0] = false;

        vector<bool> dest(blockSize + 1, false);
        fill(next(dest.begin()), dest.end(), true);
        assert(dest == result);

        fill(next(dest.begin()), dest.end(), false);
        assert(dest == vector<bool>(blockSize + 1, false));
    }

    // No offset, multiple blockSize
    {
        vector<bool> result(2 * blockSize, true);
        vector<bool> dest(2 * blockSize, false);
        fill(dest.begin(), dest.end(), true);
        assert(dest == result);

        fill(dest.begin(), dest.end(), false);
        assert(dest == vector<bool>(2 * blockSize, false));
    }

    // With offset, multiple blockSize
    {
        vector<bool> result(2 * blockSize + 5, true);
        result[0] = false;

        vector<bool> dest(2 * blockSize + 5, false);
        fill(next(dest.begin()), dest.end(), true);
        assert(dest == result);

        fill(next(dest.begin()), dest.end(), false);
        assert(dest == vector<bool>(2 * blockSize + 5, false));
    }

    return true;
}

int main() {
    test_copy();
    test_fill();
}
