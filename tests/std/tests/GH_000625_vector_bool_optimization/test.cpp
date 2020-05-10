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
                        true, false, true, false, true, true, true, false,
                        true, false, true, false, true, true, true, false };
// clang-format on

void test_copy_no_offset(const int length) {
    vector<bool> result;
    // clang-format off
    switch (length) {
    case 3:
        result = { true, false, true };
        break;
    case 8:
        result = { true, false, true, false, true, true, true, false};
        break;
    case 22:
        result = { true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true };
        break;
    case 31:
        result = { true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true };
        break;
    case 32:
        result = { true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false};
        break;
    case 67:
        result = { true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true };
        break;
    default:
        assert(false);
    }
    // clang-format on
    vector<bool> dest(length, false);
    const auto res_copy = copy(source.begin(), next(source.begin(), length), dest.begin());
    assert(dest == result);
    assert(res_copy == dest.end());

    vector<bool> dest_n(length, false);
    const auto res_copy_n = copy_n(source.begin(), length, dest_n.begin());
    assert(dest_n == result);
    assert(res_copy_n == dest_n.end());

    vector<bool> dest_backward(length, false);
    const auto res_copy_backward = copy_backward(source.begin(), next(source.begin(), length), dest_backward.end());
    assert(dest_backward == result);
    assert(res_copy_backward == dest_backward.begin());

    vector<bool> dest_move(length, false);
    const auto res_move = move(source.begin(), next(source.begin(), length), dest_move.begin());
    assert(dest_move == result);
    assert(res_move == dest_move.end());

    vector<bool> dest_move_backward(length, false);
    const auto res_move_backward =
        move_backward(source.begin(), next(source.begin(), length), dest_move_backward.end());
    assert(dest_move_backward == result);
    assert(res_move_backward == dest_move_backward.begin());
}

void test_copy_offset_source(const int length) {
    vector<bool> result;
    // clang-format off
    switch (length) {
    case 3:
        result = {       false, true, false};
        break;
    case 8:
        result = {       false, true, false, true, true, true, false,
                   true };
        break;
    case 22:
        result = {       false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true };
        break;
    case 31:
        result = {       false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false };
        break;
    case 32:
        result = {       false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true};
        break;
    case 67:
        result = {       false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false };
        break;
    default:
        assert(false);
    }
    // clang-format on
    vector<bool> dest(length, false);
    const auto res_copy = copy(next(source.begin()), next(source.begin(), length + 1), dest.begin());
    assert(dest == result);
    assert(res_copy == dest.end());

    vector<bool> dest_n(length, false);
    const auto res_copy_n = copy_n(next(source.begin()), length, dest_n.begin());
    assert(dest_n == result);
    assert(res_copy_n == dest_n.end());

    vector<bool> dest_backward(length, false);
    const auto res_copy_backward =
        copy_backward(next(source.begin()), next(source.begin(), length + 1), dest_backward.end());
    assert(dest_backward == result);
    assert(res_copy_backward == dest_backward.begin());

    vector<bool> dest_move(length, false);
    const auto res_move = move(next(source.begin()), next(source.begin(), length + 1), dest_move.begin());
    assert(dest_move == result);
    assert(res_move == dest_move.end());

    vector<bool> dest_move_backward(length, false);
    const auto res_move_backward =
        move_backward(next(source.begin()), next(source.begin(), length + 1), dest_move_backward.end());
    assert(dest_move_backward == result);
    assert(res_move_backward == dest_move_backward.begin());
}

void test_copy_offset_dest(const int length) {
    vector<bool> result;
    // clang-format off
    switch (length) {
    case 3:
        result = { false,
                   true, false, true };
        break;
    case 8:
        result = { false,
                   true, false, true, false, true, true, true, false };
        break;
    case 22:
        result = { false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true };
        break;
    case 31:
        result = { false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true };
        break;
    case 32:
        result = { false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false };
        break;
    case 67:
        result = { false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true };
        break;
    default:
        assert(false);
    }
    // clang-format on
    vector<bool> dest(length + 1, false);
    const auto res_copy = copy(source.begin(), next(source.begin(), length), next(dest.begin()));
    assert(dest == result);
    assert(res_copy == dest.end());

    vector<bool> dest_n(length + 1, false);
    const auto res_copy_n = copy_n(source.begin(), length, next(dest_n.begin()));
    assert(dest_n == result);
    assert(res_copy_n == dest_n.end());

    vector<bool> dest_backward(length + 1, false);
    const auto res_copy_backward = copy_backward(source.begin(), next(source.begin(), length), dest_backward.end());
    assert(dest_backward == result);
    assert(res_copy_backward == next(dest_backward.begin()));

    vector<bool> dest_move(length + 1, false);
    const auto res_move = move(source.begin(), next(source.begin(), length), next(dest_move.begin()));
    assert(dest_move == result);
    assert(res_move == dest_move.end());

    vector<bool> dest_move_backward(length + 1, false);
    const auto res_move_backward =
        move_backward(source.begin(), next(source.begin(), length), dest_move_backward.end());
    assert(dest_move_backward == result);
    assert(res_move_backward == next(dest_move_backward.begin()));
}

void test_copy_offset_match(const int length) {
    vector<bool> result;
    // clang-format off
    switch (length) {
    case 3:
        result = { false,
                         false, true };
        break;
    case 8:
        result = { false,
                         false, true, false, true, true, true, false };
        break;
    case 22:
        result = { false,
                         false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true };
        break;
    case 31:
        result = { false,
                         false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true };
        break;
    case 32:
        result = { false,
                         false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false };
        break;
    case 67:
        result = { false,
                         false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true };
        break;
    default:
        assert(false);
    }
    // clang-format on
    vector<bool> dest(length, false);
    const auto res_copy = copy(next(source.begin()), next(source.begin(), length), next(dest.begin()));
    assert(dest == result);
    assert(res_copy == dest.end());

    vector<bool> dest_n(length, false);
    const auto res_copy_n = copy_n(next(source.begin()), length - 1, next(dest_n.begin()));
    assert(dest_n == result);
    assert(res_copy_n == dest_n.end());

    vector<bool> dest_backward(length, false);
    const auto res_copy_backward =
        copy_backward(next(source.begin()), next(source.begin(), length), dest_backward.end());
    assert(dest_backward == result);
    assert(res_copy_backward == next(dest_backward.begin()));

    vector<bool> dest_move(length, false);
    const auto res_move = move(next(source.begin()), next(source.begin(), length), next(dest_move.begin()));
    assert(dest_move == result);
    assert(res_move == dest_move.end());

    vector<bool> dest_move_backward(length, false);
    const auto res_move_backward =
        move_backward(next(source.begin()), next(source.begin(), length), dest_move_backward.end());
    assert(dest_move_backward == result);
    assert(res_move_backward == next(dest_move_backward.begin()));
}

void test_copy_offset_mismatch_leftshift(const int length) {
    vector<bool> result;
    // clang-format off
    switch (length) {
    case 3:
        result = { false, false,
                         false, true };
        break;
    case 8:
        result = { false, false,
                         false, true, false, true, true, true, false };
        break;
    case 22:
        result = { false, false,
                         false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true };
        break;
    case 31:
        result = { false, false,
                         false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true };
        break;
    case 32:
        result = { false, false,
                         false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false };
        break;
    case 67:
        result = { false, false,
                         false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true };
        break;
    default:
        assert(false);
    }
    // clang-format on
    vector<bool> dest(length + 1, false);
    const auto res_copy = copy(next(source.begin()), next(source.begin(), length), next(dest.begin(), 2));
    assert(dest == result);
    assert(res_copy == dest.end());

    vector<bool> dest_n(length + 1, false);
    const auto res_copy_n = copy_n(next(source.begin()), length - 1, next(dest_n.begin(), 2));
    assert(dest_n == result);
    assert(res_copy_n == dest_n.end());

    vector<bool> dest_backward(length + 1, false);
    const auto res_copy_backward =
        copy_backward(next(source.begin()), next(source.begin(), length), dest_backward.end());
    assert(dest_backward == result);
    assert(res_copy_backward == next(dest_backward.begin(), 2));

    vector<bool> dest_move(length + 1, false);
    const auto res_move = move(next(source.begin()), next(source.begin(), length), next(dest_move.begin(), 2));
    assert(dest_move == result);
    assert(res_move == dest_move.end());

    vector<bool> dest_move_backward(length + 1, false);
    const auto res_move_backward =
        move_backward(next(source.begin()), next(source.begin(), length), dest_move_backward.end());
    assert(dest_move_backward == result);
    assert(res_move_backward == next(dest_move_backward.begin(), 2));
}

void test_copy_offset_mismatch_rightshift(const int length) {
    vector<bool> result;
    // clang-format off
    switch (length) {
    case 3:
        result = { false,
                                true, false };
        break;
    case 8:
        result = { false,
                                true, false, true, true, true, false,
                   true };
        break;
    case 22:
        result = { false,
                                true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true };
        break;
    case 31:
        result = { false,
                                true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false };
        break;
    case 32:
        result = { false,
                                true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true };
        break;
    case 67:
        result = { false,
                                true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false };
        break;
    default:
        assert(false);
    }
    // clang-format on
    vector<bool> dest(length, false);
    const auto res_copy = copy(next(source.begin(), 2), next(source.begin(), length + 1), next(dest.begin()));
    assert(dest == result);
    assert(res_copy == dest.end());

    vector<bool> dest_n(length, false);
    const auto res_copy_n = copy_n(next(source.begin(), 2), length - 1, next(dest_n.begin()));
    assert(dest_n == result);
    assert(res_copy_n == dest_n.end());

    vector<bool> dest_backward(length, false);
    const auto res_copy_backward =
        copy_backward(next(source.begin(), 2), next(source.begin(), length + 1), dest_backward.end());
    assert(dest_backward == result);
    assert(res_copy_backward == next(dest_backward.begin()));

    vector<bool> dest_move(length, false);
    const auto res_move = move(next(source.begin(), 2), next(source.begin(), length + 1), next(dest_move.begin()));
    assert(dest_move == result);
    assert(res_move == dest_move.end());

    vector<bool> dest_move_backward(length, false);
    const auto res_move_backward =
        move_backward(next(source.begin(), 2), next(source.begin(), length + 1), dest_move_backward.end());
    assert(dest_move_backward == result);
    assert(res_move_backward == next(dest_move_backward.begin()));
}

void test_copy_offset_aligned(const int length) {
    vector<bool> result;
    // clang-format off
    switch (length) {
    case 3:
        result = { false,
                         false, true };
        break;
    case 8:
        result = { false,
                         false, true, false, true, true, true, false };
        break;
    case 22:
        result = { false,
                         false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true };
        break;
    case 31:
        result = { false,
                         false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true };
        break;
    case 32:
        result = { false,
                         false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false };
        break;
    case 67:
        result = { false,
                         false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true, false, true, true, true, false,
                   true, false, true };
        break;
    default:
        assert(false);
    }
    // clang-format on
    vector<bool> dest(length, false);
    const auto res_copy = copy(next(source.begin(), 9), next(source.begin(), length + 8), next(dest.begin()));
    assert(dest == result);
    assert(res_copy == dest.end());

    vector<bool> dest_n(length, false);
    const auto res_copy_n = copy_n(next(source.begin(), 9), length - 1, next(dest_n.begin()));
    assert(dest_n == result);
    assert(res_copy_n == dest_n.end());

    vector<bool> dest_backward(length, false);
    const auto res_copy_backward =
        copy_backward(next(source.begin(), 9), next(source.begin(), length + 8), dest_backward.end());
    assert(dest_backward == result);
    assert(res_copy_backward == next(dest_backward.begin()));

    vector<bool> dest_move(length, false);
    const auto res_move = move(next(source.begin(), 9), next(source.begin(), length + 8), next(dest_move.begin()));
    assert(dest_move == result);
    assert(res_move == dest_move.end());

    vector<bool> dest_move_backward(length, false);
    const auto res_move_backward =
        move_backward(next(source.begin(), 9), next(source.begin(), length + 8), dest_move_backward.end());
    assert(dest_move_backward == result);
    assert(res_move_backward == next(dest_move_backward.begin()));
}

bool test_copy() {
    test_copy_no_offset(3);
    test_copy_no_offset(8);
    test_copy_no_offset(22);
    test_copy_no_offset(31);
    test_copy_no_offset(32);
    test_copy_no_offset(67);

    test_copy_offset_source(3);
    test_copy_offset_source(8);
    test_copy_offset_source(22);
    test_copy_offset_source(31);
    test_copy_offset_source(32);
    test_copy_offset_source(67);

    test_copy_offset_dest(3);
    test_copy_offset_dest(8);
    test_copy_offset_dest(22);
    test_copy_offset_dest(31);
    test_copy_offset_dest(32);
    test_copy_offset_dest(67);

    test_copy_offset_match(3);
    test_copy_offset_match(8);
    test_copy_offset_match(22);
    test_copy_offset_match(31);
    test_copy_offset_match(32);
    test_copy_offset_match(67);

    test_copy_offset_mismatch_leftshift(3);
    test_copy_offset_mismatch_leftshift(8);
    test_copy_offset_mismatch_leftshift(22);
    test_copy_offset_mismatch_leftshift(31);
    test_copy_offset_mismatch_leftshift(32);
    test_copy_offset_mismatch_leftshift(67);

    test_copy_offset_mismatch_rightshift(3);
    test_copy_offset_mismatch_rightshift(8);
    test_copy_offset_mismatch_rightshift(22);
    test_copy_offset_mismatch_rightshift(31);
    test_copy_offset_mismatch_rightshift(32);
    test_copy_offset_mismatch_rightshift(67);

    test_copy_offset_aligned(3);
    test_copy_offset_aligned(8);
    test_copy_offset_aligned(22);
    test_copy_offset_aligned(31);
    test_copy_offset_aligned(32);
    test_copy_offset_aligned(67);

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

        const auto res_fill_n = fill_n(dest.begin(), 8, true);
        assert(dest == result);
        assert(res_fill_n == dest.end());

        fill_n(dest.begin(), 8, false);
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

        const auto res_fill_n = fill_n(next(dest.begin()), 7, true);
        assert(dest == result);
        assert(res_fill_n == dest.end());

        fill_n(next(dest.begin()), 7, false);
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

        const auto res_fill_n = fill_n(dest.begin(), blockSize, true);
        assert(dest == result);
        assert(res_fill_n == dest.end());

        fill_n(dest.begin(), blockSize, false);
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

        const auto res_fill_n = fill_n(next(dest.begin()), blockSize - 1, true);
        assert(dest == result);
        assert(res_fill_n == dest.end());

        fill_n(next(dest.begin()), blockSize - 1, false);
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

        const auto res_fill_n = fill_n(next(dest.begin()), blockSize, true);
        assert(dest == result);
        assert(res_fill_n == dest.end());

        fill_n(next(dest.begin()), blockSize, false);
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

        const auto res_fill_n = fill_n(dest.begin(), 2 * blockSize, true);
        assert(dest == result);
        assert(res_fill_n == dest.end());

        fill_n(dest.begin(), 2 * blockSize, false);
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

        const auto res_fill_n = fill_n(next(dest.begin()), 2 * blockSize + 4, true);
        assert(dest == result);
        assert(res_fill_n == dest.end());

        fill_n(next(dest.begin()), 2 * blockSize + 4, false);
        assert(dest == vector<bool>(2 * blockSize + 5, false));
    }

    return true;
}

bool test_equal() {
    // No offset, less than blockSize
    {
        const vector<bool> input(source.begin(), next(source.begin(), 8));
        const vector<bool> valid = input;
        assert(equal(input.begin(), input.end(), valid.begin()));
        assert(equal(input.begin(), input.end(), valid.begin(), valid.end()));

        vector<bool> missmatchFirst = valid;
        missmatchFirst[3]           = !missmatchFirst[3];
        assert(!equal(input.begin(), input.end(), missmatchFirst.begin()));
        assert(!equal(input.begin(), input.end(), missmatchFirst.begin(), missmatchFirst.end()));
    }

    // With Offset input, less than blockSize
    {
        const vector<bool> input(source.begin(), next(source.begin(), 8));
        const vector<bool> valid(next(input.begin()), input.end());
        assert(equal(next(input.begin()), input.end(), valid.begin()));
        assert(equal(next(input.begin()), input.end(), valid.begin(), valid.end()));

        vector<bool> missmatchFirst = valid;
        missmatchFirst[3]           = !missmatchFirst[3];
        assert(!equal(next(input.begin()), input.end(), missmatchFirst.begin()));
        assert(!equal(next(input.begin()), input.end(), missmatchFirst.begin(), missmatchFirst.end()));
    }

    // With Offset valid, less than blockSize
    {
        const vector<bool> valid(source.begin(), next(source.begin(), 8));
        const vector<bool> input(next(valid.begin()), valid.end());
        assert(equal(input.begin(), input.end(), next(valid.begin())));
        assert(equal(input.begin(), input.end(), next(valid.begin()), valid.end()));

        vector<bool> missmatchFirst = valid;
        missmatchFirst[3]           = !missmatchFirst[3];
        assert(!equal(input.begin(), input.end(), next(missmatchFirst.begin())));
        assert(!equal(input.begin(), input.end(), next(missmatchFirst.begin()), missmatchFirst.end()));
    }

    // With matching Offset, less than blockSize
    {
        const vector<bool> input(source.begin(), next(source.begin(), 8));
        const vector<bool> valid = input;
        assert(equal(next(input.begin()), input.end(), next(valid.begin())));
        assert(equal(next(input.begin()), input.end(), next(valid.begin()), valid.end()));

        vector<bool> missmatchFirst = valid;
        missmatchFirst[3]           = !missmatchFirst[3];
        assert(!equal(next(input.begin()), input.end(), next(missmatchFirst.begin())));
        assert(!equal(next(input.begin()), input.end(), next(missmatchFirst.begin()), missmatchFirst.end()));
    }

    // No offset, exactly blockSize
    {
        const vector<bool> input(source.begin(), next(source.begin(), blockSize));
        const vector<bool> valid = input;
        assert(equal(input.begin(), input.end(), valid.begin()));
        assert(equal(input.begin(), input.end(), valid.begin(), valid.end()));

        vector<bool> missmatchFirst = valid;
        missmatchFirst[3]           = !missmatchFirst[3];
        assert(!equal(input.begin(), input.end(), missmatchFirst.begin()));
        assert(!equal(input.begin(), input.end(), missmatchFirst.begin(), missmatchFirst.end()));
    }

    // With Offset input, exactly blockSize
    {
        const vector<bool> input(source.begin(), next(source.begin(), blockSize + 1));
        const vector<bool> valid(next(input.begin()), input.end());
        assert(equal(next(input.begin()), input.end(), valid.begin()));
        assert(equal(next(input.begin()), input.end(), valid.begin(), valid.end()));

        vector<bool> missmatchFirst = valid;
        missmatchFirst[3]           = !missmatchFirst[3];
        assert(!equal(next(input.begin()), input.end(), missmatchFirst.begin()));
        assert(!equal(next(input.begin()), input.end(), missmatchFirst.begin(), missmatchFirst.end()));

        vector<bool> missmatchLast = valid;
        missmatchLast.back()       = !missmatchLast.back();
        assert(!equal(next(input.begin()), input.end(), missmatchLast.begin()));
        assert(!equal(next(input.begin()), input.end(), missmatchLast.begin(), missmatchLast.end()));
    }

    // With Offset valid, exactly blockSize
    {
        const vector<bool> valid(source.begin(), next(source.begin(), blockSize + 1));
        const vector<bool> input(next(valid.begin()), valid.end());
        assert(equal(input.begin(), input.end(), next(valid.begin())));
        assert(equal(input.begin(), input.end(), next(valid.begin()), valid.end()));

        vector<bool> missmatchFirst = valid;
        missmatchFirst[3]           = !missmatchFirst[3];
        assert(!equal(input.begin(), input.end(), next(missmatchFirst.begin())));
        assert(!equal(input.begin(), input.end(), next(missmatchFirst.begin()), missmatchFirst.end()));

        vector<bool> missmatchLast = valid;
        missmatchLast.back()       = !missmatchLast.back();
        assert(!equal(input.begin(), input.end(), next(missmatchLast.begin())));
        assert(!equal(input.begin(), input.end(), next(missmatchLast.begin()), missmatchLast.end()));
    }

    // With matching Offset, exactly blockSize
    {
        const vector<bool> input(source.begin(), next(source.begin(), blockSize + 1));
        const vector<bool> valid = input;
        assert(equal(next(input.begin()), input.end(), next(valid.begin())));
        assert(equal(next(input.begin()), input.end(), next(valid.begin()), valid.end()));

        vector<bool> missmatchFirst = valid;
        missmatchFirst[3]           = !missmatchFirst[3];
        assert(!equal(next(input.begin()), input.end(), next(missmatchFirst.begin())));
        assert(!equal(next(input.begin()), input.end(), next(missmatchFirst.begin()), missmatchFirst.end()));

        vector<bool> missmatchLast = valid;
        missmatchLast.back()       = !missmatchLast.back();
        assert(!equal(next(input.begin()), input.end(), next(missmatchLast.begin())));
        assert(!equal(next(input.begin()), input.end(), next(missmatchLast.begin()), missmatchLast.end()));
    }

    // No offset, multiple blockSize
    {
        const vector<bool> input(source.begin(), next(source.begin(), 2 * blockSize + 2));
        const vector<bool> valid = input;
        assert(equal(input.begin(), input.end(), valid.begin()));
        assert(equal(input.begin(), input.end(), valid.begin(), valid.end()));

        vector<bool> missmatchFirst = valid;
        missmatchFirst[3]           = !missmatchFirst[3];
        assert(!equal(input.begin(), input.end(), missmatchFirst.begin()));
        assert(!equal(input.begin(), input.end(), missmatchFirst.begin(), missmatchFirst.end()));

        vector<bool> missmatchMemCmp = valid;
        missmatchMemCmp[40]          = !missmatchMemCmp[40];
        assert(!equal(next(input.begin()), input.end(), next(missmatchMemCmp.begin())));
        assert(!equal(next(input.begin()), input.end(), next(missmatchMemCmp.begin()), missmatchMemCmp.end()));

        vector<bool> missmatchLast = valid;
        missmatchLast.back()       = !missmatchLast.back();
        assert(!equal(next(input.begin()), input.end(), next(missmatchLast.begin())));
        assert(!equal(next(input.begin()), input.end(), next(missmatchLast.begin()), missmatchLast.end()));
    }

    // With Offset input, exactly blockSize
    {
        const vector<bool> input(source.begin(), next(source.begin(), 2 * blockSize + 5));
        const vector<bool> valid(next(input.begin()), input.end());
        assert(equal(next(input.begin()), input.end(), valid.begin()));
        assert(equal(next(input.begin()), input.end(), valid.begin(), valid.end()));

        vector<bool> missmatchFirst = valid;
        missmatchFirst[3]           = !missmatchFirst[3];
        assert(!equal(next(input.begin()), input.end(), missmatchFirst.begin()));
        assert(!equal(next(input.begin()), input.end(), missmatchFirst.begin(), missmatchFirst.end()));

        vector<bool> missmatchMemCmp = valid;
        missmatchMemCmp[40]          = !missmatchMemCmp[40];
        assert(!equal(next(input.begin()), input.end(), missmatchMemCmp.begin()));
        assert(!equal(next(input.begin()), input.end(), missmatchMemCmp.begin(), missmatchMemCmp.end()));

        vector<bool> missmatchLast = valid;
        missmatchLast.back()       = !missmatchLast.back();
        assert(!equal(next(input.begin()), input.end(), missmatchLast.begin()));
        assert(!equal(next(input.begin()), input.end(), missmatchLast.begin(), missmatchLast.end()));
    }

    // With Offset valid, exactly blockSize
    {
        const vector<bool> valid(source.begin(), next(source.begin(), 2 * blockSize + 5));
        const vector<bool> input(next(valid.begin()), valid.end());
        assert(equal(input.begin(), input.end(), next(valid.begin())));
        assert(equal(input.begin(), input.end(), next(valid.begin()), valid.end()));

        vector<bool> missmatchFirst = valid;
        missmatchFirst[3]           = !missmatchFirst[3];
        assert(!equal(input.begin(), input.end(), next(missmatchFirst.begin())));
        assert(!equal(input.begin(), input.end(), next(missmatchFirst.begin()), missmatchFirst.end()));

        vector<bool> missmatchMemCmp = valid;
        missmatchMemCmp[40]          = !missmatchMemCmp[40];
        assert(!equal(input.begin(), input.end(), next(missmatchMemCmp.begin())));
        assert(!equal(input.begin(), input.end(), next(missmatchMemCmp.begin()), missmatchMemCmp.end()));

        vector<bool> missmatchLast = valid;
        missmatchLast.back()       = !missmatchLast.back();
        assert(!equal(input.begin(), input.end(), next(missmatchLast.begin())));
        assert(!equal(input.begin(), input.end(), next(missmatchLast.begin()), missmatchLast.end()));
    }

    // With matching Offset, exactly blockSize
    {
        const vector<bool> input(source.begin(), next(source.begin(), 2 * blockSize + 5));
        const vector<bool> valid = input;
        assert(equal(next(input.begin()), input.end(), next(valid.begin())));
        assert(equal(next(input.begin()), input.end(), next(valid.begin()), valid.end()));

        vector<bool> missmatchFirst = valid;
        missmatchFirst[3]           = !missmatchFirst[3];
        assert(!equal(next(input.begin()), input.end(), next(missmatchFirst.begin())));
        assert(!equal(next(input.begin()), input.end(), next(missmatchFirst.begin()), missmatchFirst.end()));


        vector<bool> missmatchMemCmp = valid;
        missmatchMemCmp[40]          = !missmatchMemCmp[40];
        assert(!equal(next(input.begin()), input.end(), next(missmatchMemCmp.begin())));
        assert(!equal(next(input.begin()), input.end(), next(missmatchMemCmp.begin()), missmatchMemCmp.end()));

        vector<bool> missmatchLast = valid;
        missmatchLast.back()       = !missmatchLast.back();
        assert(!equal(next(input.begin()), input.end(), next(missmatchLast.begin())));
        assert(!equal(next(input.begin()), input.end(), next(missmatchLast.begin()), missmatchLast.end()));
    }

    return true;
}

bool test_find() {
    // Less than blockSize
    {
        vector<bool> input_true(blockSize, false);
        input_true[5].flip();
        const auto result_true = find(input_true.begin(), next(input_true.begin(), 8), true);
        assert(result_true == next(input_true.begin(), 5));

        vector<bool> input_false(blockSize, true);
        input_false[6].flip();
        const auto result_false = find(input_false.begin(), next(input_false.begin(), 8), false);
        assert(result_false == next(input_false.begin(), 6));
    }

    // Exactly blockSize
    {
        vector<bool> input_true(blockSize, false);
        input_true[24].flip();
        const auto result_true = find(input_true.begin(), next(input_true.begin(), blockSize), true);
        assert(result_true == next(input_true.begin(), 24));

        vector<bool> input_false(blockSize, true);
        input_false[27].flip();
        const auto result_false = find(input_false.begin(), next(input_false.begin(), blockSize), false);
        assert(result_false == next(input_false.begin(), 27));
    }

    // More than blockSize
    {
        vector<bool> input_true(blockSize + 5, false);
        input_true[24].flip();
        const auto result_true = find(input_true.begin(), next(input_true.begin(), blockSize), true);
        assert(result_true == next(input_true.begin(), 24));

        vector<bool> input_false(blockSize + 5, true);
        input_false[27].flip();
        const auto result_false = find(input_false.begin(), next(input_false.begin(), blockSize), false);
        assert(result_false == next(input_false.begin(), 27));
    }

    // More than blockSize ends with offset
    {
        vector<bool> input_true(blockSize + 8, false);
        input_true[33].flip();
        const auto result_true = find(input_true.begin(), next(input_true.begin(), blockSize + 8), true);
        assert(result_true == next(input_true.begin(), 33));

        vector<bool> input_false(blockSize + 8, true);
        input_false[35].flip();
        const auto result_false = find(input_false.begin(), next(input_false.begin(), blockSize + 8), false);
        assert(result_false == next(input_false.begin(), 35));
    }

    // Multiple blockSize
    {
        vector<bool> input_true(3 * blockSize, false);
        input_true[33].flip();
        const auto result_true = find(input_true.begin(), next(input_true.begin(), 3 * blockSize), true);
        assert(result_true == next(input_true.begin(), 33));

        vector<bool> input_false(3 * blockSize, true);
        input_false[35].flip();
        const auto result_false = find(input_false.begin(), next(input_false.begin(), 3 * blockSize), false);
        assert(result_false == next(input_false.begin(), 35));
    }

    // Multiple blockSize, ends with offset
    {
        vector<bool> input_true(3 * blockSize + 5, false);
        input_true[3 * blockSize + 3].flip();
        const auto result_true = find(input_true.begin(), next(input_true.begin(), 3 * blockSize + 5), true);
        assert(result_true == next(input_true.begin(), 3 * blockSize + 3));

        vector<bool> input_false(3 * blockSize + 5, true);
        input_false[3 * blockSize + 2].flip();
        const auto result_false = find(input_false.begin(), next(input_false.begin(), 3 * blockSize + 5), false);
        assert(result_false == next(input_false.begin(), 3 * blockSize + 2));
    }

    return true;
}

bool test_count() {
    // Less than blockSize
    {
        const auto result_true = count(source.begin(), next(source.begin(), 8), true);
        assert(result_true == 5);

        const auto result_false = count(source.begin(), next(source.begin(), 8), false);
        assert(result_false == 3);
    }

    // Exactly blockSize
    {
        const auto result_true = count(source.begin(), next(source.begin(), blockSize), true);
        assert(result_true == 20);

        const auto result_false = count(source.begin(), next(source.begin(), blockSize), false);
        assert(result_false == 12);
    }

    // More than blockSize
    {
        const auto result_true = count(source.begin(), next(source.begin(), blockSize + 3), true);
        assert(result_true == 22);

        const auto result_false = count(source.begin(), next(source.begin(), blockSize + 3), false);
        assert(result_false == 13);
    }

    // Multiple blockSize ends at boundary
    {
        const auto result_true = count(source.begin(), next(source.begin(), 2 * blockSize), true);
        assert(result_true == 40);

        const auto result_false = count(source.begin(), next(source.begin(), 2 * blockSize), false);
        assert(result_false == 24);
    }

    // Multiple blockSize ends with offset
    {
        const auto result_true = count(source.begin(), next(source.begin(), 2 * blockSize + 5), true);
        assert(result_true == 43);

        const auto result_false = count(source.begin(), next(source.begin(), 2 * blockSize + 5), false);
        assert(result_false == 26);
    }

    return true;
}

int main() {
    test_copy();
    test_fill();
    test_equal();
    test_find();
    test_count();
}
