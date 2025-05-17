// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <__msvc_int128.hpp>
#include <algorithm>
#include <cassert>
#include <compare>
#include <iterator>
#include <utility>

#include "range_algorithm_support.hpp"

using namespace std;

template <class T>
using picky_contiguous_iterator = test::redifference_iterator<_Signed128, T*>;

static_assert(contiguous_iterator<picky_contiguous_iterator<int>>);

int main() {
    const int arr[] = {
        200, 210, 220, 250, 240, 250, 250, 270, 280, 290, 300, 310, 320, 250, 340, 250, 250, 370, 380, 390};
    constexpr auto arr_size = size(arr);

    picky_contiguous_iterator arr_begin(begin(arr));
    picky_contiguous_iterator arr_end(end(arr));

    assert(find(arr_begin, arr_end, 250) == arr_begin + _Signed128{3});
    assert(ranges::find(arr_begin, arr_end, 250) == arr_begin + _Signed128{3});
#if _HAS_CXX23
    assert(begin(ranges::find_last(arr_begin, arr_end, 250)) == arr_begin + _Signed128{16});
#endif
    assert(search_n(arr_begin, arr_end, 2, 250) == arr_begin + _Signed128{5});
    assert(begin(ranges::search_n(arr_begin, arr_end, 2, 250)) == arr_begin + _Signed128{5});

    {
        const int needle[] = {100, 300, 500, 700, 900};
        picky_contiguous_iterator needle_begin(begin(needle));
        picky_contiguous_iterator needle_end(end(needle));

        assert(find_first_of(arr_begin, arr_end, needle_begin, needle_end) == arr_begin + _Signed128{10});
        assert(ranges::find_first_of(arr_begin, arr_end, needle_begin, needle_end) == arr_begin + _Signed128{10});
    }

    assert(adjacent_find(arr_begin, arr_end) == arr_begin + _Signed128{5});
    assert(ranges::adjacent_find(arr_begin, arr_end) == arr_begin + _Signed128{5});

    {
        const int needle[] = {300, 310, 320};

        picky_contiguous_iterator needle_begin(begin(needle));
        picky_contiguous_iterator needle_end(end(needle));

        assert(search(arr_begin, arr_end, needle_begin, needle_end) == arr_begin + _Signed128{10});
        assert(begin(ranges::search(arr_begin, arr_end, needle_begin, needle_end)) == arr_begin + _Signed128{10});

        assert(find_end(arr_begin, arr_end, needle_begin, needle_end) == arr_begin + _Signed128{10});
        assert(begin(ranges::find_end(arr_begin, arr_end, needle_begin, needle_end)) == arr_begin + _Signed128{10});
    }

    assert(count(arr_begin, arr_end, 250) == 6);
    assert(ranges::count(arr_begin, arr_end, 250) == 6);

    {
        const int arr_cmp[] = {200, 210, 220, 230, 240, 250};

        picky_contiguous_iterator arr_cmp_begin(begin(arr_cmp));
        picky_contiguous_iterator arr_cmp_end(end(arr_cmp));

        assert(!equal(arr_cmp_begin, arr_cmp_end, arr_begin));
        assert(!equal(arr_cmp_begin, arr_cmp_end, arr_begin, arr_end));
        assert(!ranges::equal(arr_cmp_begin, arr_cmp_end, arr_begin, arr_end));
        assert(mismatch(arr_cmp_begin, arr_cmp_end, arr_begin).first == arr_cmp_begin + _Signed128{3});
        assert(mismatch(arr_cmp_begin, arr_cmp_end, arr_begin, arr_end).first == arr_cmp_begin + _Signed128{3});
        assert(ranges::mismatch(arr_cmp_begin, arr_cmp_end, arr_begin, arr_end).in1 == arr_cmp_begin + _Signed128{3});

        assert(lexicographical_compare(arr_cmp_begin, arr_cmp_end, arr_begin, arr_end));
        assert(ranges::lexicographical_compare(arr_cmp_begin, arr_cmp_end, arr_begin, arr_end));
        assert(
            lexicographical_compare_three_way(arr_cmp_begin, arr_cmp_end, arr_begin, arr_end) == strong_ordering::less);
    }

    assert(min_element(arr_begin, arr_end) == arr_begin);
    assert(max_element(arr_begin, arr_end) == arr_end - _Signed128{1});
    assert(minmax_element(arr_begin, arr_end) == pair(arr_begin, arr_end - _Signed128{1}));

    assert(ranges::min_element(arr_begin, arr_end) == arr_begin);
    assert(ranges::max_element(arr_begin, arr_end) == arr_end - _Signed128{1});
    assert(ranges::minmax_element(arr_begin, arr_end).max == arr_end - _Signed128{1});

    assert(ranges::min(ranges::subrange(arr_begin, arr_end)) == 200);
    assert(ranges::max(ranges::subrange(arr_begin, arr_end)) == 390);
    assert(ranges::minmax(ranges::subrange(arr_begin, arr_end)).max == 390);

    {
        // floating minmax is distinct codepath unless /fp:fast
        float float_arr[arr_size];
        picky_contiguous_iterator float_arr_begin(begin(float_arr));
        picky_contiguous_iterator float_arr_end(end(float_arr));

        transform(arr_begin, arr_end, float_arr_begin, [](const int v) { return static_cast<float>(v); });

        assert(ranges::min(ranges::subrange(float_arr_begin, float_arr_end)) == 200.0);
        assert(ranges::max(ranges::subrange(float_arr_begin, float_arr_end)) == 390.0);
        assert(ranges::minmax(ranges::subrange(float_arr_begin, float_arr_end)).max == 390.0);
    }

    assert(is_sorted_until(arr_begin, arr_end) == arr_begin + _Signed128{4});
    assert(ranges::is_sorted_until(arr_begin, arr_end) == arr_begin + _Signed128{4});

    {
        int arr_copy[arr_size];
        picky_contiguous_iterator arr_copy_begin(begin(arr_copy));
        picky_contiguous_iterator arr_copy_end(end(arr_copy));

        copy(arr_begin, arr_end, arr_copy_begin);
        assert(equal(arr_begin, arr_end, arr_copy_begin, arr_copy_end));
        fill(arr_copy_begin, arr_copy_end, 0);
        assert(count(arr_copy_begin, arr_copy_end, 0) == arr_size);

        ranges::copy(arr_begin, arr_end, arr_copy_begin);
        assert(ranges::equal(arr_begin, arr_end, arr_copy_begin, arr_copy_end));
        ranges::fill(arr_copy_begin, arr_copy_end, 0);
        assert(ranges::count(arr_copy_begin, arr_copy_end, 0) == arr_size);

        copy_n(arr_begin, arr_size, arr_copy_begin);
        assert(equal(arr_begin, arr_end, arr_copy_begin, arr_copy_end));
        fill_n(arr_copy_begin, arr_size, 0);
        assert(count(arr_copy_begin, arr_copy_end, 0) == arr_size);

        ranges::copy_n(arr_begin, arr_size, arr_copy_begin);
        assert(ranges::equal(arr_begin, arr_end, arr_copy_begin, arr_copy_end));
        ranges::fill_n(arr_copy_begin, arr_size, 0);
        assert(ranges::count(arr_copy_begin, arr_copy_end, 0) == arr_size);
    }

    {
        int temp[arr_size];
        picky_contiguous_iterator temp_begin(begin(temp));
        picky_contiguous_iterator temp_end(end(temp));

        {
            const int remove_expected[] = {200, 210, 220, 240, 270, 280, 290, 300, 310, 320, 340, 370, 380, 390};

            fill(temp_begin, temp_end, 0);
            auto rem_copy_it = remove_copy(arr_begin, arr_end, temp_begin, 250);
            assert(equal(temp_begin, rem_copy_it, begin(remove_expected), end(remove_expected)));

            copy(arr_begin, arr_end, temp_begin);
            auto rem_it = remove(temp_begin, temp_end, 250);
            assert(equal(temp_begin, rem_it, begin(remove_expected), end(remove_expected)));

            ranges::fill(temp_begin, temp_end, 0);
            auto r_rem_copy_it = ranges::remove_copy(arr_begin, arr_end, temp_begin, 250);
            assert(ranges::equal(temp_begin, r_rem_copy_it.out, begin(remove_expected), end(remove_expected)));

            ranges::copy(arr_begin, arr_end, temp_begin);
            auto r_rem_it = ranges::remove(temp_begin, temp_end, 250);
            assert(ranges::equal(temp_begin, begin(r_rem_it), begin(remove_expected), end(remove_expected)));
        }
        {
            const int unique_expected[] = {
                200, 210, 220, 250, 240, 250, 270, 280, 290, 300, 310, 320, 250, 340, 250, 370, 380, 390};

            fill(temp_begin, temp_end, 0);
            auto un_copy_it = unique_copy(arr_begin, arr_end, temp_begin);
            assert(equal(temp_begin, un_copy_it, begin(unique_expected), end(unique_expected)));

            copy(arr_begin, arr_end, temp_begin);
            auto un_it = unique(temp_begin, temp_end);
            assert(equal(temp_begin, un_it, begin(unique_expected), end(unique_expected)));

            ranges::fill(temp_begin, temp_end, 0);
            auto r_un_copy_it = ranges::unique_copy(arr_begin, arr_end, temp_begin);
            assert(ranges::equal(temp_begin, r_un_copy_it.out, begin(unique_expected), end(unique_expected)));

            ranges::copy(arr_begin, arr_end, temp_begin);
            auto r_un_it = ranges::unique(temp_begin, temp_end);
            assert(ranges::equal(temp_begin, begin(r_un_it), begin(unique_expected), end(unique_expected)));
        }
        {
            const int reverse_expected[] = {
                390, 380, 370, 250, 250, 340, 250, 320, 310, 300, 290, 280, 270, 250, 250, 240, 250, 220, 210, 200};

            reverse_copy(arr_begin, arr_end, temp_begin);
            assert(equal(temp_begin, temp_end, begin(reverse_expected), end(reverse_expected)));

            copy(arr_begin, arr_end, temp_begin);
            reverse(temp_begin, temp_end);
            assert(equal(temp_begin, temp_end, begin(reverse_expected), end(reverse_expected)));

            ranges::reverse_copy(arr_begin, arr_end, temp_begin);
            assert(ranges::equal(temp_begin, temp_end, begin(reverse_expected), end(reverse_expected)));

            ranges::copy(arr_begin, arr_end, temp_begin);
            ranges::reverse(temp_begin, temp_end);
            assert(ranges::equal(temp_begin, temp_end, begin(reverse_expected), end(reverse_expected)));
        }
        {
            const int rotate_expected[] = {
                250, 270, 280, 290, 300, 310, 320, 250, 340, 250, 250, 370, 380, 390, 200, 210, 220, 250, 240, 250};

            const _Signed128 rotate_pos = 6;

            auto rot_copy_it = rotate_copy(arr_begin, arr_begin + rotate_pos, arr_end, temp_begin);
            assert(equal(temp_begin, temp_end, begin(rotate_expected), end(rotate_expected)));
            assert(rot_copy_it == temp_end);

            copy(arr_begin, arr_end, temp_begin);
            auto rot_it = rotate(temp_begin, temp_begin + rotate_pos, temp_end);
            assert(equal(temp_begin, temp_end, begin(rotate_expected), end(rotate_expected)));
            assert(rot_it == temp_end - rotate_pos);

            auto r_rot_copy_it = ranges::rotate_copy(arr_begin, arr_begin + rotate_pos, arr_end, temp_begin).out;
            assert(ranges::equal(temp_begin, temp_end, begin(rotate_expected), end(rotate_expected)));
            assert(r_rot_copy_it == temp_end);

            ranges::copy(arr_begin, arr_end, temp_begin);
            auto r_rot_it = begin(ranges::rotate(temp_begin, temp_begin + rotate_pos, temp_end));
            assert(ranges::equal(temp_begin, temp_end, begin(rotate_expected), end(rotate_expected)));
            assert(r_rot_it == temp_end - rotate_pos);
        }
        {
            // Out of replace family, only replace for 32-bit and 64-bit elements is manually vectorized,
            // replace_copy is auto vectorized (along with replace_copy_if)
            const int replace_expected[] = {
                200, 210, 220, 333, 240, 333, 333, 270, 280, 290, 300, 310, 320, 333, 340, 333, 333, 370, 380, 390};

            copy(arr_begin, arr_end, temp_begin);
            replace(temp_begin, temp_end, 250, 333);
            assert(equal(temp_begin, temp_end, begin(replace_expected), end(replace_expected)));

            ranges::copy(arr_begin, arr_end, temp_begin);
            ranges::replace(temp_begin, temp_end, 250, 333);
            assert(ranges::equal(temp_begin, temp_end, begin(replace_expected), end(replace_expected)));
        }
        {
            const int swap_ranges_expected[] = {
                300, 310, 320, 250, 340, 250, 250, 370, 380, 390, 200, 210, 220, 250, 240, 250, 250, 270, 280, 290};

            const auto temp_mid = temp_begin + _Signed128{10};

            copy(arr_begin, arr_end, temp_begin);
            swap_ranges(temp_begin, temp_mid, temp_mid);
            assert(equal(temp_begin, temp_end, begin(swap_ranges_expected), end(swap_ranges_expected)));

            ranges::copy(arr_begin, arr_end, temp_begin);
            ranges::swap_ranges(temp_begin, temp_mid, temp_mid, temp_end);
            assert(ranges::equal(temp_begin, temp_end, begin(swap_ranges_expected), end(swap_ranges_expected)));
        }
    }
}
