// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <memory>
#include <random>
#include <vector>

#if _HAS_CXX20
#define CONSTEXPR20 constexpr
#else
#define CONSTEXPR20 inline
#endif

using namespace std;

constexpr int blockSize = 32;
static_assert(blockSize == _VBITS, "Invalid block size");

// This test data is not random, but irregular enough to ensure confidence in the tests
constexpr bool source_raw[] = { //
    true, false, true, false, true, true, true, false, //
    true, false, true, false, true, true, true, false, //
    true, false, true, false, true, true, true, false, //
    true, false, true, false, true, true, true, false, //
    true, false, true, false, true, true, true, false, //
    true, false, true, false, true, true, true, false, //
    true, false, true, false, true, true, true, false, //
    true, false, true, false, true, true, true, false, //
    true, false, true, false, true, true, true, false, //
    true, false, true, false, true, true, true, false, //
    true, false, true, false, true, true, true, false, //
    true, false, true, false, true, true, true, false, //
    true, false, true, false, true, true, true, false, //
    true, false, true, false, true, true, true, false, //
    true, false, true, false, true, true, true, false, //
    true, false, true, false, true, true, true, false, //
    true, false, true, false, true, true, true, false};

CONSTEXPR20 void test_transform_helper(const size_t length) {
    // Only no offset case

    // This test data is not random, but irregular enough to ensure confidence in the tests
    constexpr bool source2_raw[] = {//
        true, true, false, false, false, false, true, true, //
        true, true, false, false, false, false, true, true, //
        true, true, false, false, false, false, true, true, //
        true, true, false, false, false, false, true, true, //
        true, true, false, false, false, false, true, true, //
        true, true, false, false, false, false, true, true, //
        true, true, false, false, false, false, true, true, //
        true, true, false, false, false, false, true, true, //
        true, true, false, false, false, false, true, true, //
        true, true, false, false, false, false, true, true, //
        true, true, false, false, false, false, true, true, //
        true, true, false, false, false, false, true, true, //
        true, true, false, false, false, false, true, true, //
        true, true, false, false, false, false, true, true, //
        true, true, false, false, false, false, true, true, //
        true, true, false, false, false, false, true, true, //
        true, true, false, false, false, false, true, true};

#if _HAS_CXX17
    static_assert(size(source_raw) == size(source2_raw));
#endif // _HAS_CXX17

    bool and_expected_raw[size(source_raw)];
    bool or_expected_raw[size(source_raw)];
    bool xor_expected_raw[size(source_raw)];
    bool xnor_expected_raw[size(source_raw)];
    bool l_expected_raw[size(source_raw)];
    bool le_expected_raw[size(source_raw)];
    bool g_expected_raw[size(source_raw)];
    bool ge_expected_raw[size(source_raw)];
    bool not_expected_raw[size(source_raw)];

    transform(begin(source_raw), end(source_raw), begin(source2_raw), begin(and_expected_raw), logical_and<>{});
    transform(begin(source_raw), end(source_raw), begin(source2_raw), begin(or_expected_raw), logical_or<>{});
    transform(begin(source_raw), end(source_raw), begin(source2_raw), begin(xor_expected_raw), not_equal_to<>{});
    transform(begin(source_raw), end(source_raw), begin(source2_raw), begin(xnor_expected_raw), equal_to<>{});
    transform(begin(source_raw), end(source_raw), begin(source2_raw), begin(l_expected_raw), less<>{});
    transform(begin(source_raw), end(source_raw), begin(source2_raw), begin(le_expected_raw), less_equal<>{});
    transform(begin(source_raw), end(source_raw), begin(source2_raw), begin(g_expected_raw), greater<>{});
    transform(begin(source_raw), end(source_raw), begin(source2_raw), begin(ge_expected_raw), greater_equal<>{});
    transform(begin(source_raw), end(source_raw), begin(not_expected_raw), logical_not<>{});

    const vector<bool> source1(source_raw, source_raw + static_cast<ptrdiff_t>(length));
    const vector<bool> source2(source2_raw, source2_raw + static_cast<ptrdiff_t>(length));

    vector<bool> and_expected(and_expected_raw, and_expected_raw + static_cast<ptrdiff_t>(length));
    vector<bool> or_expected(or_expected_raw, or_expected_raw + static_cast<ptrdiff_t>(length));
    vector<bool> xor_expected(xor_expected_raw, xor_expected_raw + static_cast<ptrdiff_t>(length));
    vector<bool> xnor_expected(xnor_expected_raw, xnor_expected_raw + static_cast<ptrdiff_t>(length));
    vector<bool> less_expected(l_expected_raw, l_expected_raw + static_cast<ptrdiff_t>(length));
    vector<bool> less_equal_expected(le_expected_raw, le_expected_raw + static_cast<ptrdiff_t>(length));
    vector<bool> greater_expected(g_expected_raw, g_expected_raw + static_cast<ptrdiff_t>(length));
    vector<bool> greater_equal_expected(ge_expected_raw, ge_expected_raw + static_cast<ptrdiff_t>(length));
    vector<bool> not_expected(not_expected_raw, not_expected_raw + static_cast<ptrdiff_t>(length));

    and_expected.resize(length + 3, false);
    or_expected.resize(length + 3, false);
    xor_expected.resize(length + 3, false);
    xnor_expected.resize(length + 3, false);
    less_expected.resize(length + 3, false);
    less_equal_expected.resize(length + 3, false);
    greater_expected.resize(length + 3, false);
    greater_equal_expected.resize(length + 3, false);
    not_expected.resize(length + 3, false);

    vector<bool> and_actual(length + 3);
    vector<bool> or_actual(length + 3);
    vector<bool> xor_actual(length + 3);
    vector<bool> xnor_actual(length + 3);
    vector<bool> less_actual(length + 3);
    vector<bool> less_equal_actual(length + 3);
    vector<bool> greater_actual(length + 3);
    vector<bool> greater_equal_actual(length + 3);
    vector<bool> not_actual(length + 3);

    // Also test combinations of vector<bool>::iterator and vector<bool>::const_iterator for the inputs.
    const auto first1  = source1.begin();
    const auto cfirst1 = source1.cbegin();
    const auto first2  = source2.begin();
    const auto cfirst2 = source2.cbegin();
    const auto last1   = first1 + static_cast<ptrdiff_t>(length);
    const auto clast1  = cfirst1 + static_cast<ptrdiff_t>(length);

    {
        auto and_ret = transform(first1, last1, first2, and_actual.begin(), logical_and<>{});
        assert(and_actual == and_expected);
        assert(and_ret == and_actual.begin() + static_cast<ptrdiff_t>(length));

        and_actual.assign(and_actual.size(), false);

        and_ret = transform(first1, last1, first2, and_actual.begin(), bit_and<>{});
        assert(and_actual == and_expected);
        assert(and_ret == and_actual.begin() + static_cast<ptrdiff_t>(length));
    }

    {
        auto or_ret = transform(first1, last1, cfirst2, or_actual.begin(), logical_or<>{});
        assert(or_actual == or_expected);
        assert(or_ret == or_actual.begin() + static_cast<ptrdiff_t>(length));

        or_actual.assign(or_actual.size(), false);

        or_ret = transform(first1, last1, cfirst2, or_actual.begin(), bit_or<>{});
        assert(or_actual == or_expected);
        assert(or_ret == or_actual.begin() + static_cast<ptrdiff_t>(length));
    }

    {
        auto xor_ret = transform(cfirst1, clast1, first2, xor_actual.begin(), not_equal_to<>{});
        assert(xor_actual == xor_expected);
        assert(xor_ret == xor_actual.begin() + static_cast<ptrdiff_t>(length));

        xor_actual.assign(xor_actual.size(), false);

        xor_ret = transform(cfirst1, clast1, first2, xor_actual.begin(), bit_xor<>{});
        assert(xor_actual == xor_expected);
        assert(xor_ret == xor_actual.begin() + static_cast<ptrdiff_t>(length));
    }

    {
        const auto xnor_ret = transform(cfirst1, clast1, cfirst2, xnor_actual.begin(), equal_to<>{});
        assert(xnor_actual == xnor_expected);
        assert(xnor_ret == xnor_actual.begin() + static_cast<ptrdiff_t>(length));

        // bit_xnor doesn't exist in the Standard
    }

    {
        const auto less_ret = transform(cfirst1, clast1, cfirst2, less_actual.begin(), less<>{});
        assert(less_actual == less_expected);
        assert(less_ret == less_actual.begin() + static_cast<ptrdiff_t>(length));
    }

    {
        const auto less_equal_ret = transform(cfirst1, clast1, cfirst2, less_equal_actual.begin(), less_equal<>{});
        assert(less_equal_actual == less_equal_expected);
        assert(less_equal_ret == less_equal_actual.begin() + static_cast<ptrdiff_t>(length));
    }

    {
        const auto greater_ret = transform(cfirst1, clast1, cfirst2, greater_actual.begin(), greater<>{});
        assert(greater_actual == greater_expected);
        assert(greater_ret == greater_actual.begin() + static_cast<ptrdiff_t>(length));
    }

    {
        const auto greater_equal_ret =
            transform(cfirst1, clast1, cfirst2, greater_equal_actual.begin(), greater_equal<>{});
        assert(greater_equal_actual == greater_equal_expected);
        assert(greater_equal_ret == greater_equal_actual.begin() + static_cast<ptrdiff_t>(length));
    }

    {
        auto not_ret = transform(first1, last1, not_actual.begin(), logical_not<>{});
        assert(not_actual == not_expected);
        assert(not_ret == not_actual.begin() + static_cast<ptrdiff_t>(length));

        not_actual.assign(not_actual.size(), false);

        // bit_not emits MSVC and Clang warnings, so it isn't optimized.
        // Continue using logical_not to test vector<bool>::const_iterator:
        not_ret = transform(cfirst1, clast1, not_actual.begin(), logical_not<>{});
        assert(not_actual == not_expected);
        assert(not_ret == not_actual.begin() + static_cast<ptrdiff_t>(length));
    }
}

CONSTEXPR20 bool test_transform() {
    // Empty range
    test_transform_helper(0);

    // One block, ends within block
    test_transform_helper(15);

    // One block, ends at block boundary
    test_transform_helper(blockSize);

    // Multiple blocks, within block
    test_transform_helper(3 * blockSize + 5);

    // Multiple blocks, ends at block boundary
    test_transform_helper(4 * blockSize);
    return true;
}

CONSTEXPR20 void test_fill_helper(const size_t length) {
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

CONSTEXPR20 bool test_fill() {
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

CONSTEXPR20 void test_find_helper(const size_t length) {
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

CONSTEXPR20 bool test_find() {
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

CONSTEXPR20 void test_count_helper(const ptrdiff_t length) {
    const vector<bool> source(begin(source_raw), end(source_raw));
    const int counts_true[]  = {0, 1, 1, 2, 2, 3, 4, 5};
    const int counts_false[] = {0, 0, 1, 1, 2, 2, 2, 2};
    const div_t expected     = {static_cast<int>(length) / 8, static_cast<int>(length) % 8};
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

CONSTEXPR20 bool test_count() {
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

CONSTEXPR20 void test_copy_no_offset(const size_t length) {
    vector<bool> result;
    switch (length) {
    case 3:
        result = {true, false, true};
        break;
    case 8:
        result = {true, false, true, false, true, true, true, false};
        break;
    case 22:
        result = {//
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true};
        break;
    case 31:
        result = {//
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true};
        break;
    case 32:
        result = {//
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false};
        break;
    case 67:
        result = {//
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true};
        break;
    default:
        assert(false);
    }

    const vector<bool> source(begin(source_raw), end(source_raw));

    {
        vector<bool> dest(length, false);
        const auto res_copy = copy(source.begin(), source.begin() + static_cast<ptrdiff_t>(length), dest.begin());
        assert(dest == result);
        assert(res_copy == dest.end());
    }

    {
        vector<bool> dest_n(length, false);
        const auto res_copy_n = copy_n(source.begin(), static_cast<ptrdiff_t>(length), dest_n.begin());
        assert(dest_n == result);
        assert(res_copy_n == dest_n.end());
    }

    {
        vector<bool> dest_backward(length, false);
        const auto res_copy_backward =
            copy_backward(source.begin(), source.begin() + static_cast<ptrdiff_t>(length), dest_backward.end());
        assert(dest_backward == result);
        assert(res_copy_backward == dest_backward.begin());
    }

    {
        vector<bool> dest_move(length, false);
        const auto res_move = move(source.begin(), source.begin() + static_cast<ptrdiff_t>(length), dest_move.begin());
        assert(dest_move == result);
        assert(res_move == dest_move.end());
    }

    {
        vector<bool> dest_move_backward(length, false);
        const auto res_move_backward =
            move_backward(source.begin(), source.begin() + static_cast<ptrdiff_t>(length), dest_move_backward.end());
        assert(dest_move_backward == result);
        assert(res_move_backward == dest_move_backward.begin());
    }
}

CONSTEXPR20 void test_copy_offset_source(const size_t length) {
    vector<bool> result;
    switch (length) {
    case 3:
        result = {/***/ false, true, false};
        break;
    case 8:
        result = {//
            /***/ false, true, false, true, true, true, false, //
            true};
        break;
    case 22:
        result = {//
            /***/ false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true};
        break;
    case 31:
        result = {//
            /***/ false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false};
        break;
    case 32:
        result = {//
            /***/ false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true};
        break;
    case 67:
        result = {//
            /***/ false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false};
        break;
    default:
        assert(false);
    }

    const vector<bool> source(begin(source_raw), end(source_raw));

    {
        vector<bool> dest(length, false);
        const auto res_copy =
            copy(next(source.begin()), source.begin() + static_cast<ptrdiff_t>(length) + 1, dest.begin());
        assert(dest == result);
        assert(res_copy == dest.end());
    }

    {
        vector<bool> dest_n(length, false);
        const auto res_copy_n = copy_n(next(source.begin()), static_cast<ptrdiff_t>(length), dest_n.begin());
        assert(dest_n == result);
        assert(res_copy_n == dest_n.end());
    }

    {
        vector<bool> dest_backward(length, false);
        const auto res_copy_backward = copy_backward(
            next(source.begin()), source.begin() + static_cast<ptrdiff_t>(length) + 1, dest_backward.end());
        assert(dest_backward == result);
        assert(res_copy_backward == dest_backward.begin());
    }

    {
        vector<bool> dest_move(length, false);
        const auto res_move =
            move(next(source.begin()), source.begin() + static_cast<ptrdiff_t>(length) + 1, dest_move.begin());
        assert(dest_move == result);
        assert(res_move == dest_move.end());
    }

    {
        vector<bool> dest_move_backward(length, false);
        const auto res_move_backward = move_backward(
            next(source.begin()), source.begin() + static_cast<ptrdiff_t>(length) + 1, dest_move_backward.end());
        assert(dest_move_backward == result);
        assert(res_move_backward == dest_move_backward.begin());
    }
}

CONSTEXPR20 void test_copy_offset_dest(const size_t length) {
    vector<bool> result;
    switch (length) {
    case 3:
        result = {//
            false, //
            true, false, true};
        break;
    case 8:
        result = {//
            false, //
            true, false, true, false, true, true, true, false};
        break;
    case 22:
        result = {//
            false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true};
        break;
    case 31:
        result = {//
            false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true};
        break;
    case 32:
        result = {//
            false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false};
        break;
    case 67:
        result = {//
            false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true};
        break;
    default:
        assert(false);
    }

    const vector<bool> source(begin(source_raw), end(source_raw));

    {
        vector<bool> dest(length + 1, false);
        const auto res_copy = copy(source.begin(), source.begin() + static_cast<ptrdiff_t>(length), next(dest.begin()));
        assert(dest == result);
        assert(res_copy == dest.end());
    }

    {
        vector<bool> dest_n(length + 1, false);
        const auto res_copy_n = copy_n(source.begin(), static_cast<ptrdiff_t>(length), next(dest_n.begin()));
        assert(dest_n == result);
        assert(res_copy_n == dest_n.end());
    }

    {
        vector<bool> dest_backward(length + 1, false);
        const auto res_copy_backward =
            copy_backward(source.begin(), source.begin() + static_cast<ptrdiff_t>(length), dest_backward.end());
        assert(dest_backward == result);
        assert(res_copy_backward == next(dest_backward.begin()));
    }

    {
        vector<bool> dest_move(length + 1, false);
        const auto res_move =
            move(source.begin(), source.begin() + static_cast<ptrdiff_t>(length), next(dest_move.begin()));
        assert(dest_move == result);
        assert(res_move == dest_move.end());
    }

    {
        vector<bool> dest_move_backward(length + 1, false);
        const auto res_move_backward =
            move_backward(source.begin(), source.begin() + static_cast<ptrdiff_t>(length), dest_move_backward.end());
        assert(dest_move_backward == result);
        assert(res_move_backward == next(dest_move_backward.begin()));
    }
}

CONSTEXPR20 void test_copy_offset_match(const size_t length) {
    vector<bool> result;
    switch (length) {
    case 3:
        result = {//
            false, //
            /***/ false, true};
        break;
    case 8:
        result = {//
            false, //
            /***/ false, true, false, true, true, true, false};
        break;
    case 22:
        result = {//
            false, //
            /***/ false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true};
        break;
    case 31:
        result = {//
            false, //
            /***/ false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true};
        break;
    case 32:
        result = {//
            false, //
            /***/ false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false};
        break;
    case 67:
        result = {//
            false, //
            /***/ false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true};
        break;
    default:
        assert(false);
    }

    const vector<bool> source(begin(source_raw), end(source_raw));

    {
        vector<bool> dest(length, false);
        const auto res_copy =
            copy(next(source.begin()), source.begin() + static_cast<ptrdiff_t>(length), next(dest.begin()));
        assert(dest == result);
        assert(res_copy == dest.end());
    }

    {
        vector<bool> dest_n(length, false);
        const auto res_copy_n = copy_n(next(source.begin()), static_cast<ptrdiff_t>(length) - 1, next(dest_n.begin()));
        assert(dest_n == result);
        assert(res_copy_n == dest_n.end());
    }

    {
        vector<bool> dest_backward(length, false);
        const auto res_copy_backward =
            copy_backward(next(source.begin()), source.begin() + static_cast<ptrdiff_t>(length), dest_backward.end());
        assert(dest_backward == result);
        assert(res_copy_backward == next(dest_backward.begin()));
    }

    {
        vector<bool> dest_move(length, false);
        const auto res_move =
            move(next(source.begin()), source.begin() + static_cast<ptrdiff_t>(length), next(dest_move.begin()));
        assert(dest_move == result);
        assert(res_move == dest_move.end());
    }

    {
        vector<bool> dest_move_backward(length, false);
        const auto res_move_backward = move_backward(
            next(source.begin()), source.begin() + static_cast<ptrdiff_t>(length), dest_move_backward.end());
        assert(dest_move_backward == result);
        assert(res_move_backward == next(dest_move_backward.begin()));
    }
}

CONSTEXPR20 void test_copy_offset_mismatch_leftshift(const size_t length) {
    vector<bool> result;
    switch (length) {
    case 3:
        result = {//
            false, false, //
            /***/ false, true};
        break;
    case 8:
        result = {//
            false, false, //
            /***/ false, true, false, true, true, true, false};
        break;
    case 22:
        result = {//
            false, false, //
            /***/ false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true};
        break;
    case 31:
        result = {//
            false, false, //
            /***/ false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true};
        break;
    case 32:
        result = {//
            false, false, //
            /***/ false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false};
        break;
    case 67:
        result = {//
            false, false, //
            /***/ false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true};
        break;
    default:
        assert(false);
    }

    const vector<bool> source(begin(source_raw), end(source_raw));

    {
        vector<bool> dest(length + 1, false);
        const auto res_copy =
            copy(next(source.begin()), source.begin() + static_cast<ptrdiff_t>(length), next(dest.begin(), 2));
        assert(dest == result);
        assert(res_copy == dest.end());
    }

    {
        vector<bool> dest_n(length + 1, false);
        const auto res_copy_n =
            copy_n(next(source.begin()), static_cast<ptrdiff_t>(length) - 1, next(dest_n.begin(), 2));
        assert(dest_n == result);
        assert(res_copy_n == dest_n.end());
    }

    {
        vector<bool> dest_backward(length + 1, false);
        const auto res_copy_backward =
            copy_backward(next(source.begin()), source.begin() + static_cast<ptrdiff_t>(length), dest_backward.end());
        assert(dest_backward == result);
        assert(res_copy_backward == next(dest_backward.begin(), 2));
    }

    {
        vector<bool> dest_move(length + 1, false);
        const auto res_move =
            move(next(source.begin()), source.begin() + static_cast<ptrdiff_t>(length), next(dest_move.begin(), 2));
        assert(dest_move == result);
        assert(res_move == dest_move.end());
    }

    {
        vector<bool> dest_move_backward(length + 1, false);
        const auto res_move_backward = move_backward(
            next(source.begin()), source.begin() + static_cast<ptrdiff_t>(length), dest_move_backward.end());
        assert(dest_move_backward == result);
        assert(res_move_backward == next(dest_move_backward.begin(), 2));
    }
}

CONSTEXPR20 void test_copy_offset_mismatch_rightshift(const size_t length) {
    vector<bool> result;
    switch (length) {
    case 3:
        result = {//
            false, //
            /**********/ true, false};
        break;
    case 8:
        result = {//
            false, //
            /**********/ true, false, true, true, true, false, //
            true};
        break;
    case 22:
        result = {//
            false, //
            /**********/ true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true};
        break;
    case 31:
        result = {//
            false, //
            /**********/ true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false};
        break;
    case 32:
        result = {//
            false, //
            /**********/ true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true};
        break;
    case 67:
        result = {//
            false, //
            /**********/ true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false};
        break;
    default:
        assert(false);
    }

    const vector<bool> source(begin(source_raw), end(source_raw));

    {
        vector<bool> dest(length, false);
        const auto res_copy =
            copy(next(source.begin(), 2), source.begin() + static_cast<ptrdiff_t>(length) + 1, next(dest.begin()));
        assert(dest == result);
        assert(res_copy == dest.end());
    }

    {
        vector<bool> dest_n(length, false);
        const auto res_copy_n =
            copy_n(next(source.begin(), 2), static_cast<ptrdiff_t>(length) - 1, next(dest_n.begin()));
        assert(dest_n == result);
        assert(res_copy_n == dest_n.end());
    }

    {
        vector<bool> dest_backward(length, false);
        const auto res_copy_backward = copy_backward(
            next(source.begin(), 2), source.begin() + static_cast<ptrdiff_t>(length) + 1, dest_backward.end());
        assert(dest_backward == result);
        assert(res_copy_backward == next(dest_backward.begin()));
    }

    {
        vector<bool> dest_move(length, false);
        const auto res_move =
            move(next(source.begin(), 2), source.begin() + static_cast<ptrdiff_t>(length) + 1, next(dest_move.begin()));
        assert(dest_move == result);
        assert(res_move == dest_move.end());
    }

    {
        vector<bool> dest_move_backward(length, false);
        const auto res_move_backward = move_backward(
            next(source.begin(), 2), source.begin() + static_cast<ptrdiff_t>(length) + 1, dest_move_backward.end());
        assert(dest_move_backward == result);
        assert(res_move_backward == next(dest_move_backward.begin()));
    }
}

CONSTEXPR20 void test_copy_offset_aligned(const size_t length) {
    vector<bool> result;
    switch (length) {
    case 3:
        result = {//
            false, //
            /***/ false, true};
        break;
    case 8:
        result = {//
            false, //
            /***/ false, true, false, true, true, true, false};
        break;
    case 22:
        result = {//
            false, //
            /***/ false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true};
        break;
    case 31:
        result = {//
            false, //
            /***/ false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true};
        break;
    case 32:
        result = {//
            false, //
            /***/ false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false};
        break;
    case 67:
        result = {//
            false, //
            /***/ false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true, false, true, true, true, false, //
            true, false, true};
        break;
    default:
        assert(false);
    }

    const vector<bool> source(begin(source_raw), end(source_raw));

    {
        vector<bool> dest(length, false);
        const auto res_copy =
            copy(next(source.begin(), 9), source.begin() + static_cast<ptrdiff_t>(length) + 8, next(dest.begin()));
        assert(dest == result);
        assert(res_copy == dest.end());
    }

    {
        vector<bool> dest_n(length, false);
        const auto res_copy_n =
            copy_n(next(source.begin(), 9), static_cast<ptrdiff_t>(length) - 1, next(dest_n.begin()));
        assert(dest_n == result);
        assert(res_copy_n == dest_n.end());
    }

    {
        vector<bool> dest_backward(length, false);
        const auto res_copy_backward = copy_backward(
            next(source.begin(), 9), source.begin() + static_cast<ptrdiff_t>(length) + 8, dest_backward.end());
        assert(dest_backward == result);
        assert(res_copy_backward == next(dest_backward.begin()));
    }

    {
        vector<bool> dest_move(length, false);
        const auto res_move =
            move(next(source.begin(), 9), source.begin() + static_cast<ptrdiff_t>(length) + 8, next(dest_move.begin()));
        assert(dest_move == result);
        assert(res_move == dest_move.end());
    }

    {
        vector<bool> dest_move_backward(length, false);
        const auto res_move_backward = move_backward(
            next(source.begin(), 9), source.begin() + static_cast<ptrdiff_t>(length) + 8, dest_move_backward.end());
        assert(dest_move_backward == result);
        assert(res_move_backward == next(dest_move_backward.begin()));
    }
}

CONSTEXPR20 void test_copy_sub_char() {
    const vector<bool> source(begin(source_raw), end(source_raw));

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

CONSTEXPR20 void test_copy_regression() {
    // This specific case was found by the randomized coverage below.
    const vector<bool> src = {0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 1, 0,
        1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0,
        1, 0, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0};

    vector<bool> dst = {0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0,
        0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1,
        0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1};

    const int src_prefix = 24;
    const int dst_prefix = 7;
    const int copy_len   = 48;

    copy(src.begin() + src_prefix, src.begin() + src_prefix + copy_len, dst.begin() + dst_prefix);

    const vector<bool> correct = {0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0,
        0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0,
        0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1};

    assert(dst == correct);
}

// Test copy() in multiple parts to stay within constexpr step limits.
CONSTEXPR20 bool test_copy_part_1() {
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

    return true;
}

CONSTEXPR20 bool test_copy_part_2() {
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

    test_copy_regression();

    return true;
}

void initialize_randomness(mt19937_64& gen) {
    constexpr size_t n = mt19937_64::state_size;
    constexpr size_t w = mt19937_64::word_size;
    static_assert(w % 32 == 0, "w should be evenly divisible by 32");
    constexpr size_t k = w / 32;

    vector<uint32_t> vec(n * k);

    random_device rd;
    generate(vec.begin(), vec.end(), ref(rd));

    printf("This is a randomized test.\n");
    printf("DO NOT IGNORE/RERUN ANY FAILURES.\n");
    printf("You must report them to the STL maintainers.\n\n");

    seed_seq seq(vec.cbegin(), vec.cend());
    gen.seed(seq);
}

template <class T>
void print_vec(const char* const name, const vector<T>& v) {
    printf("%s: ", name);
    for (const auto& e : v) {
        printf("%d", static_cast<int>(e));
    }
    printf("\n");
}

void randomized_test_copy(mt19937_64& gen) {
    uniform_int_distribution<int> affix_dist{0, 2 * blockSize - 1}; // from nothing to a partial then whole block
    uniform_int_distribution<int> copy_len_dist{1, 3 * blockSize}; // from 1 bit to leading/middle/trailing blocks
    auto bool_dist = [&gen] { return static_cast<bool>(gen() & 1); };

    constexpr int repetitions = 10'000; // tune the number of tests to balance coverage vs. execution time

    for (int k = 0; k < repetitions; ++k) {
        const int src_prefix = affix_dist(gen);
        const int dst_prefix = affix_dist(gen);
        const int copy_len   = copy_len_dist(gen);
        const int src_suffix = affix_dist(gen);
        const int dst_suffix = affix_dist(gen);

        // src vector: <src_prefix> <copy_len> <src_suffix>
        // dst vector: <dst_prefix> <copy_len> <dst_suffix>

        vector<bool> vb_src(static_cast<size_t>(src_prefix + copy_len + src_suffix));
        vector<bool> vb_dst(static_cast<size_t>(dst_prefix + copy_len + dst_suffix));

        generate(vb_src.begin(), vb_src.end(), bool_dist);
        generate(vb_dst.begin(), vb_dst.end(), bool_dist);

        const vector<uint8_t> v8_src(vb_src.cbegin(), vb_src.cend());
        vector<uint8_t> v8_dst(vb_dst.cbegin(), vb_dst.cend());

        const vector<uint8_t> orig_v8_dst = v8_dst;

        copy(vb_src.cbegin() + src_prefix, vb_src.cbegin() + src_prefix + copy_len, vb_dst.begin() + dst_prefix);
        copy(v8_src.cbegin() + src_prefix, v8_src.cbegin() + src_prefix + copy_len, v8_dst.begin() + dst_prefix);

        if (!equal(vb_dst.cbegin(), vb_dst.cend(), v8_dst.cbegin(), v8_dst.cend(), equal_to<uint8_t>{})) {
            printf("src_prefix: %d\n", src_prefix);
            printf("dst_prefix: %d\n", dst_prefix);
            printf("  copy_len: %d\n", copy_len);
            printf("src_suffix: %d\n", src_suffix);
            printf("dst_suffix: %d\n", dst_suffix);

            print_vec("     Src", v8_src);
            print_vec("Original", orig_v8_dst);
            print_vec("     Got", vb_dst);
            print_vec("Expected", v8_dst);

            assert(false);
        }
    }

    // Overlapping is permitted when the dst range starts before the src range.
    for (int k = 0; k < repetitions; ++k) {
        const int prefix   = affix_dist(gen);
        const int copy_len = copy_len_dist(gen);
        const int overhang = uniform_int_distribution<int>{1, copy_len + 30}(gen);
        const int suffix   = affix_dist(gen);

        // An overhang of 1 results in the maximum possible overlap.
        // An overhang of copy_len results in non-overlapping but adjacent ranges.
        // An overhang of copy_len + 30 is the maximum value where the dst and src ranges could share a block.

        // Vector diagram:
        // <prefix> <overhang> <src is copy_len> <suffix>
        //          ^
        //          <dst is copy_len>

        vector<bool> vb(static_cast<size_t>(prefix + overhang + copy_len + suffix));

        generate(vb.begin(), vb.end(), bool_dist);

        vector<uint8_t> v8(vb.cbegin(), vb.cend());

        const vector<uint8_t> orig_v8 = v8;

        copy(vb.cbegin() + prefix + overhang, vb.cbegin() + prefix + overhang + copy_len, vb.begin() + prefix);
        copy(v8.cbegin() + prefix + overhang, v8.cbegin() + prefix + overhang + copy_len, v8.begin() + prefix);

        if (!equal(vb.cbegin(), vb.cend(), v8.cbegin(), v8.cend(), equal_to<uint8_t>{})) {
            printf("  prefix: %d\n", prefix);
            printf("copy_len: %d\n", copy_len);
            printf("overhang: %d\n", overhang);
            printf("  suffix: %d\n", suffix);

            print_vec("Original", orig_v8);
            print_vec("Got", vb);
            print_vec("Expected", v8);

            assert(false);
        }
    }

    uniform_int_distribution<int> gap_dist{0, 30}; // 0 gap is adjacent; 30 is the max gap between 2 bits in a block

    // One more interesting case: when the src range is before the dst range,
    // and they're non-overlapping but close enough that they could share a block.
    for (int k = 0; k < repetitions; ++k) {
        const int prefix   = affix_dist(gen);
        const int copy_len = copy_len_dist(gen);
        const int gap      = gap_dist(gen);
        const int suffix   = affix_dist(gen);

        // Vector diagram:
        // <prefix> <src is copy_len> <gap> <dst is copy_len> <suffix>

        vector<bool> vb(static_cast<size_t>(prefix + copy_len + gap + copy_len + suffix));

        generate(vb.begin(), vb.end(), bool_dist);

        vector<uint8_t> v8(vb.cbegin(), vb.cend());

        const vector<uint8_t> orig_v8 = v8;

        copy(vb.cbegin() + prefix, vb.cbegin() + prefix + copy_len, vb.begin() + prefix + copy_len + gap);
        copy(v8.cbegin() + prefix, v8.cbegin() + prefix + copy_len, v8.begin() + prefix + copy_len + gap);

        if (!equal(vb.cbegin(), vb.cend(), v8.cbegin(), v8.cend(), equal_to<uint8_t>{})) {
            printf("  prefix: %d\n", prefix);
            printf("copy_len: %d\n", copy_len);
            printf("     gap: %d\n", gap);
            printf("  suffix: %d\n", suffix);

            print_vec("Original", orig_v8);
            print_vec("Got", vb);
            print_vec("Expected", v8);

            assert(false);
        }
    }
}

#if _HAS_CXX20
template <size_t N, size_t Offset = 0>
constexpr bool test_gh_5345() {
    // GH-5345 <vector>: _Copy_vbool() mishandles vector<bool>s of size 32 and 64, revealed by constexpr Clang
    vector<bool> src(N, true);

    for (size_t i = 2; i != N; ++i) {
        for (size_t j = i * 2; j < N; j += i) {
            src[j] = false;
        }
    }

    vector<bool> dst(N, false);
    copy(src.begin() + Offset, src.end(), dst.begin() + Offset);
    return equal(src.begin() + Offset, src.end(), dst.begin() + Offset, dst.end());
}

static_assert(test_gh_5345<17>());
static_assert(test_gh_5345<17, 1>());
static_assert(test_gh_5345<32>());
static_assert(test_gh_5345<32, 5>());
static_assert(test_gh_5345<43>());
static_assert(test_gh_5345<43, 10>());
static_assert(test_gh_5345<64>());
static_assert(test_gh_5345<64, 16>());
static_assert(test_gh_5345<120>());
static_assert(test_gh_5345<120, 31>());

static_assert(test_fill());
static_assert(test_find());
static_assert(test_count());
static_assert(test_transform());

#if defined(__clang__) || defined(__EDG__) // TRANSITION, VSO-2574489
static_assert(test_copy_part_1());
static_assert(test_copy_part_2());
#endif // ^^^ no workaround ^^^
#endif // _HAS_CXX20

int main() {
    test_fill();
    test_find();
    test_count();
    test_transform();
    test_copy_part_1();
    test_copy_part_2();

    test_huge_vector_bool();

    mt19937_64 gen;
    initialize_randomness(gen);
    randomized_test_copy(gen);
}
