// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <vector>

#pragma warning(disable : 4365) // conversion from 'unsigned __int64' to 'const __int64', signed/unsigned mismatch

using namespace std;

constexpr int blockSize = 32;
static_assert(blockSize == _VBITS, "Invalid block size");

// This test data is not random, but irregular enough to ensure confidence in the tests
// clang-format off
const vector<bool> source = { true, false, true, false, true, true, true, false,
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
// clang-format on


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
    const int counts_true[]  = {0, 1, 1, 2, 2, 3, 4, 5};
    const int counts_false[] = {0, 0, 1, 1, 2, 2, 2, 2};
    const auto expected      = div(static_cast<int>(length), 8);
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

// Also test the behavior of a huge vector<bool, A> whose size is greater than SIZE_MAX,
// which is practical on 32-bit platforms.
template <class T>
struct huge_allocator {
    huge_allocator() = default;
    template <class U>
    constexpr huge_allocator(const huge_allocator<U>&) noexcept {}

    using value_type      = T;
    using size_type       = uint64_t;
    using difference_type = int64_t;

    T* allocate(uint64_t n) {
        return allocator<T>{}.allocate(static_cast<size_t>(n));
    }

    void deallocate(T* p, uint64_t n) {
        allocator<T>{}.deallocate(p, static_cast<size_t>(n));
    }
};

void test_huge_vector_bool() {
    constexpr uint64_t small_bit_length = 0x7000'4321ULL;
    constexpr uint64_t large_bit_length = 0x1'2000'4321ULL; // overflows uint32_t
    constexpr auto large_bit_diff       = static_cast<int64_t>(large_bit_length);

    vector<bool, huge_allocator<bool>> v(small_bit_length);
    v.resize(large_bit_length);
    assert(v.end() - v.begin() == large_bit_diff);

    v.back() = true;
    assert(find(v.begin(), v.end(), true) - v.begin() == large_bit_diff - 1);

    v[small_bit_length]                    = true;
    v[large_bit_length - small_bit_length] = true;
    assert(count(v.begin(), v.end(), false) == large_bit_diff - 3);
}

void test_copy_no_offset(const size_t length) {
    vector<bool> result;
    // clang-format off
    switch (length) {
    case 3:
        result = { true, false, true };
        break;
    case 8:
        result = { true, false, true, false, true, true, true, false };
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
                   true, false, true, false, true, true, true, false };
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

    {
        vector<bool> dest(length, false);
        const auto res_copy = copy(source.begin(), next(source.begin(), length), dest.begin());
        assert(dest == result);
        assert(res_copy == dest.end());
    }

    {
        vector<bool> dest_n(length, false);
        const auto res_copy_n = copy_n(source.begin(), length, dest_n.begin());
        assert(dest_n == result);
        assert(res_copy_n == dest_n.end());
    }

    {
        vector<bool> dest_backward(length, false);
        const auto res_copy_backward = copy_backward(source.begin(), next(source.begin(), length), dest_backward.end());
        assert(dest_backward == result);
        assert(res_copy_backward == dest_backward.begin());
    }

    {
        vector<bool> dest_move(length, false);
        const auto res_move = move(source.begin(), next(source.begin(), length), dest_move.begin());
        assert(dest_move == result);
        assert(res_move == dest_move.end());
    }

    {
        vector<bool> dest_move_backward(length, false);
        const auto res_move_backward =
            move_backward(source.begin(), next(source.begin(), length), dest_move_backward.end());
        assert(dest_move_backward == result);
        assert(res_move_backward == dest_move_backward.begin());
    }
}

void test_copy_offset_source(const size_t length) {
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

    {
        vector<bool> dest(length, false);
        const auto res_copy = copy(next(source.begin()), next(source.begin(), length + 1), dest.begin());
        assert(dest == result);
        assert(res_copy == dest.end());
    }

    {
        vector<bool> dest_n(length, false);
        const auto res_copy_n = copy_n(next(source.begin()), length, dest_n.begin());
        assert(dest_n == result);
        assert(res_copy_n == dest_n.end());
    }

    {
        vector<bool> dest_backward(length, false);
        const auto res_copy_backward =
            copy_backward(next(source.begin()), next(source.begin(), length + 1), dest_backward.end());
        assert(dest_backward == result);
        assert(res_copy_backward == dest_backward.begin());
    }

    {
        vector<bool> dest_move(length, false);
        const auto res_move = move(next(source.begin()), next(source.begin(), length + 1), dest_move.begin());
        assert(dest_move == result);
        assert(res_move == dest_move.end());
    }

    {
        vector<bool> dest_move_backward(length, false);
        const auto res_move_backward =
            move_backward(next(source.begin()), next(source.begin(), length + 1), dest_move_backward.end());
        assert(dest_move_backward == result);
        assert(res_move_backward == dest_move_backward.begin());
    }
}

void test_copy_offset_dest(const size_t length) {
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

    {
        vector<bool> dest(length + 1, false);
        const auto res_copy = copy(source.begin(), next(source.begin(), length), next(dest.begin()));
        assert(dest == result);
        assert(res_copy == dest.end());
    }

    {
        vector<bool> dest_n(length + 1, false);
        const auto res_copy_n = copy_n(source.begin(), length, next(dest_n.begin()));
        assert(dest_n == result);
        assert(res_copy_n == dest_n.end());
    }

    {
        vector<bool> dest_backward(length + 1, false);
        const auto res_copy_backward = copy_backward(source.begin(), next(source.begin(), length), dest_backward.end());
        assert(dest_backward == result);
        assert(res_copy_backward == next(dest_backward.begin()));
    }

    {
        vector<bool> dest_move(length + 1, false);
        const auto res_move = move(source.begin(), next(source.begin(), length), next(dest_move.begin()));
        assert(dest_move == result);
        assert(res_move == dest_move.end());
    }

    {
        vector<bool> dest_move_backward(length + 1, false);
        const auto res_move_backward =
            move_backward(source.begin(), next(source.begin(), length), dest_move_backward.end());
        assert(dest_move_backward == result);
        assert(res_move_backward == next(dest_move_backward.begin()));
    }
}

void test_copy_offset_match(const size_t length) {
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

    {
        vector<bool> dest(length, false);
        const auto res_copy = copy(next(source.begin()), next(source.begin(), length), next(dest.begin()));
        assert(dest == result);
        assert(res_copy == dest.end());
    }

    {
        vector<bool> dest_n(length, false);
        const auto res_copy_n = copy_n(next(source.begin()), length - 1, next(dest_n.begin()));
        assert(dest_n == result);
        assert(res_copy_n == dest_n.end());
    }

    {
        vector<bool> dest_backward(length, false);
        const auto res_copy_backward =
            copy_backward(next(source.begin()), next(source.begin(), length), dest_backward.end());
        assert(dest_backward == result);
        assert(res_copy_backward == next(dest_backward.begin()));
    }

    {
        vector<bool> dest_move(length, false);
        const auto res_move = move(next(source.begin()), next(source.begin(), length), next(dest_move.begin()));
        assert(dest_move == result);
        assert(res_move == dest_move.end());
    }

    {
        vector<bool> dest_move_backward(length, false);
        const auto res_move_backward =
            move_backward(next(source.begin()), next(source.begin(), length), dest_move_backward.end());
        assert(dest_move_backward == result);
        assert(res_move_backward == next(dest_move_backward.begin()));
    }
}

void test_copy_offset_mismatch_leftshift(const size_t length) {
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

    {
        vector<bool> dest(length + 1, false);
        const auto res_copy = copy(next(source.begin()), next(source.begin(), length), next(dest.begin(), 2));
        assert(dest == result);
        assert(res_copy == dest.end());
    }

    {
        vector<bool> dest_n(length + 1, false);
        const auto res_copy_n = copy_n(next(source.begin()), length - 1, next(dest_n.begin(), 2));
        assert(dest_n == result);
        assert(res_copy_n == dest_n.end());
    }

    {
        vector<bool> dest_backward(length + 1, false);
        const auto res_copy_backward =
            copy_backward(next(source.begin()), next(source.begin(), length), dest_backward.end());
        assert(dest_backward == result);
        assert(res_copy_backward == next(dest_backward.begin(), 2));
    }

    {
        vector<bool> dest_move(length + 1, false);
        const auto res_move = move(next(source.begin()), next(source.begin(), length), next(dest_move.begin(), 2));
        assert(dest_move == result);
        assert(res_move == dest_move.end());
    }

    {
        vector<bool> dest_move_backward(length + 1, false);
        const auto res_move_backward =
            move_backward(next(source.begin()), next(source.begin(), length), dest_move_backward.end());
        assert(dest_move_backward == result);
        assert(res_move_backward == next(dest_move_backward.begin(), 2));
    }
}

void test_copy_offset_mismatch_rightshift(const size_t length) {
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

    {
        vector<bool> dest(length, false);
        const auto res_copy = copy(next(source.begin(), 2), next(source.begin(), length + 1), next(dest.begin()));
        assert(dest == result);
        assert(res_copy == dest.end());
    }

    {
        vector<bool> dest_n(length, false);
        const auto res_copy_n = copy_n(next(source.begin(), 2), length - 1, next(dest_n.begin()));
        assert(dest_n == result);
        assert(res_copy_n == dest_n.end());
    }

    {
        vector<bool> dest_backward(length, false);
        const auto res_copy_backward =
            copy_backward(next(source.begin(), 2), next(source.begin(), length + 1), dest_backward.end());
        assert(dest_backward == result);
        assert(res_copy_backward == next(dest_backward.begin()));
    }

    {
        vector<bool> dest_move(length, false);
        const auto res_move = move(next(source.begin(), 2), next(source.begin(), length + 1), next(dest_move.begin()));
        assert(dest_move == result);
        assert(res_move == dest_move.end());
    }

    {
        vector<bool> dest_move_backward(length, false);
        const auto res_move_backward =
            move_backward(next(source.begin(), 2), next(source.begin(), length + 1), dest_move_backward.end());
        assert(dest_move_backward == result);
        assert(res_move_backward == next(dest_move_backward.begin()));
    }
}

void test_copy_offset_aligned(const size_t length) {
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

    {
        vector<bool> dest(length, false);
        const auto res_copy = copy(next(source.begin(), 9), next(source.begin(), length + 8), next(dest.begin()));
        assert(dest == result);
        assert(res_copy == dest.end());
    }

    {
        vector<bool> dest_n(length, false);
        const auto res_copy_n = copy_n(next(source.begin(), 9), length - 1, next(dest_n.begin()));
        assert(dest_n == result);
        assert(res_copy_n == dest_n.end());
    }

    {
        vector<bool> dest_backward(length, false);
        const auto res_copy_backward =
            copy_backward(next(source.begin(), 9), next(source.begin(), length + 8), dest_backward.end());
        assert(dest_backward == result);
        assert(res_copy_backward == next(dest_backward.begin()));
    }

    {
        vector<bool> dest_move(length, false);
        const auto res_move = move(next(source.begin(), 9), next(source.begin(), length + 8), next(dest_move.begin()));
        assert(dest_move == result);
        assert(res_move == dest_move.end());
    }

    {
        vector<bool> dest_move_backward(length, false);
        const auto res_move_backward =
            move_backward(next(source.begin(), 9), next(source.begin(), length + 8), dest_move_backward.end());
        assert(dest_move_backward == result);
        assert(res_move_backward == next(dest_move_backward.begin()));
    }
}

void test_copy_sub_char() {
    { // sub char copy unaligned
        const vector<bool> result = {false, false, true, false, true, false, false, false};

        {
            vector<bool> dest(8, false);
            const auto res_copy = copy(source.begin(), next(source.begin(), 3), next(dest.begin(), 2));
            assert(dest == result);
            assert(res_copy == next(dest.begin(), 5));
        }

        {
            vector<bool> dest_n(8, false);
            const auto res_copy_n = copy_n(source.begin(), 3, next(dest_n.begin(), 2));
            assert(dest_n == result);
            assert(res_copy_n == next(dest_n.begin(), 5));
        }

        {
            vector<bool> dest_backward(8, false);
            const auto res_copy_backward =
                copy_backward(source.begin(), next(source.begin(), 3), next(dest_backward.begin(), 5));
            assert(dest_backward == result);
            assert(res_copy_backward == next(dest_backward.begin(), 2));
        }

        {
            vector<bool> dest_move(8, false);
            const auto res_move = move(source.begin(), next(source.begin(), 3), next(dest_move.begin(), 2));
            assert(dest_move == result);
            assert(res_move == next(dest_move.begin(), 5));
        }

        {
            vector<bool> dest_move_backward(8, false);
            const auto res_move_backward =
                move_backward(source.begin(), next(source.begin(), 3), next(dest_move_backward.begin(), 5));
            assert(dest_move_backward == result);
            assert(res_move_backward == next(dest_move_backward.begin(), 2));
        }
    }

    { // sub char copy until char alignment source
        const vector<bool> result = {false, false, true, true, false, false, false, false};

        {
            vector<bool> dest(8, false);
            const auto res_copy = copy(next(source.begin(), 5), next(source.begin(), 8), next(dest.begin(), 2));
            assert(dest == result);
            assert(res_copy == next(dest.begin(), 5));
        }

        {
            vector<bool> dest_n(8, false);
            const auto res_copy_n = copy_n(next(source.begin(), 5), 3, next(dest_n.begin(), 2));
            assert(dest_n == result);
            assert(res_copy_n == next(dest_n.begin(), 5));
        }

        {
            vector<bool> dest_backward(8, false);
            const auto res_copy_backward =
                copy_backward(next(source.begin(), 5), next(source.begin(), 8), next(dest_backward.begin(), 5));
            assert(dest_backward == result);
            assert(res_copy_backward == next(dest_backward.begin(), 2));
        }

        {
            vector<bool> dest_move(8, false);
            const auto res_move = move(next(source.begin(), 5), next(source.begin(), 8), next(dest_move.begin(), 2));
            assert(dest_move == result);
            assert(res_move == next(dest_move.begin(), 5));
        }

        {
            vector<bool> dest_move_backward(8, false);
            const auto res_move_backward =
                move_backward(next(source.begin(), 5), next(source.begin(), 8), next(dest_move_backward.begin(), 5));
            assert(dest_move_backward == result);
            assert(res_move_backward == next(dest_move_backward.begin(), 2));
        }
    }

    { // sub char copy until char alignment dest
        const vector<bool> result = {false, false, false, false, false, true, false, true, false, false, false, false,
            false, false, false, false};

        {
            vector<bool> dest(16, false);
            const auto res_copy = copy(source.begin(), next(source.begin(), 3), next(dest.begin(), 5));
            assert(dest == result);
            assert(res_copy == next(dest.begin(), 8));
        }

        {
            vector<bool> dest_n(16, false);
            const auto res_copy_n = copy_n(source.begin(), 3, next(dest_n.begin(), 5));
            assert(dest_n == result);
            assert(res_copy_n == next(dest_n.begin(), 8));
        }

        {
            vector<bool> dest_backward(16, false);
            const auto res_copy_backward =
                copy_backward(source.begin(), next(source.begin(), 3), next(dest_backward.begin(), 8));
            assert(dest_backward == result);
            assert(res_copy_backward == next(dest_backward.begin(), 5));
        }

        {
            vector<bool> dest_move(16, false);
            const auto res_move = move(source.begin(), next(source.begin(), 3), next(dest_move.begin(), 5));
            assert(dest_move == result);
            assert(res_move == next(dest_move.begin(), 8));
        }

        {
            vector<bool> dest_move_backward(16, false);
            const auto res_move_backward =
                move_backward(source.begin(), next(source.begin(), 3), next(dest_move_backward.begin(), 8));
            assert(dest_move_backward == result);
            assert(res_move_backward == next(dest_move_backward.begin(), 5));
        }
    }

    { // sub char copy over char alignment source
        const vector<bool> result = {false, false, false, true, false, false, false, false};

        {
            vector<bool> dest(8, false);
            const auto res_copy = copy(next(source.begin(), 7), next(source.begin(), 10), next(dest.begin(), 2));
            assert(dest == result);
            assert(res_copy == next(dest.begin(), 5));
        }

        {
            vector<bool> dest_n(8, false);
            const auto res_copy_n = copy_n(next(source.begin(), 7), 3, next(dest_n.begin(), 2));
            assert(dest_n == result);
            assert(res_copy_n == next(dest_n.begin(), 5));
        }

        {
            vector<bool> dest_backward(8, false);
            const auto res_copy_backward =
                copy_backward(next(source.begin(), 7), next(source.begin(), 10), next(dest_backward.begin(), 5));
            assert(dest_backward == result);
            assert(res_copy_backward == next(dest_backward.begin(), 2));
        }

        {
            vector<bool> dest_move(8, false);
            const auto res_move = move(next(source.begin(), 7), next(source.begin(), 10), next(dest_move.begin(), 2));
            assert(dest_move == result);
            assert(res_move == next(dest_move.begin(), 5));
        }

        {
            vector<bool> dest_move_backward(8, false);
            const auto res_move_backward =
                move_backward(next(source.begin(), 7), next(source.begin(), 10), next(dest_move_backward.begin(), 5));
            assert(dest_move_backward == result);
            assert(res_move_backward == next(dest_move_backward.begin(), 2));
        }
    }

    { // sub char copy over char alignment dest
        const vector<bool> result = {false, false, false, false, false, false, false, true, false, true, false, false,
            false, false, false, false};

        {
            vector<bool> dest(16, false);
            const auto res_copy = copy(source.begin(), next(source.begin(), 3), next(dest.begin(), 7));
            assert(dest == result);
            assert(res_copy == next(dest.begin(), 10));
        }

        {
            vector<bool> dest_n(16, false);
            const auto res_copy_n = copy_n(source.begin(), 3, next(dest_n.begin(), 7));
            assert(dest_n == result);
            assert(res_copy_n == next(dest_n.begin(), 10));
        }

        {
            vector<bool> dest_backward(16, false);
            const auto res_copy_backward =
                copy_backward(source.begin(), next(source.begin(), 3), next(dest_backward.begin(), 10));
            assert(dest_backward == result);
            assert(res_copy_backward == next(dest_backward.begin(), 7));
        }

        {
            vector<bool> dest_move(16, false);
            const auto res_move = move(source.begin(), next(source.begin(), 3), next(dest_move.begin(), 7));
            assert(dest_move == result);
            assert(res_move == next(dest_move.begin(), 10));
        }

        {
            vector<bool> dest_move_backward(16, false);
            const auto res_move_backward =
                move_backward(source.begin(), next(source.begin(), 3), next(dest_move_backward.begin(), 10));
            assert(dest_move_backward == result);
            assert(res_move_backward == next(dest_move_backward.begin(), 7));
        }
    }
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

    test_copy_sub_char();

    return true;
}

int main() {
    test_fill();
    test_find();
    test_count();
    test_copy();

    test_huge_vector_bool();
}
