// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <__msvc_int128.hpp>
#include <algorithm>
#include <cassert>
#include <compare>
#include <iterator>

using namespace std;

enum class magic_word : unsigned short {}; // prevent vectorization paths from construction iterator from pointer

template <class UnderlyingPtr>
class angry_contiguous_iterator {
public:
    using iterator_category    = contiguous_iterator_tag;
    using reference            = typename iterator_traits<UnderlyingPtr>::reference;
    using value_type           = typename iterator_traits<UnderlyingPtr>::value_type;
    using pointer              = typename iterator_traits<UnderlyingPtr>::pointer;
    using underlying_diff_type = typename iterator_traits<UnderlyingPtr>::difference_type;
    using difference_type      = _Signed128;

    angry_contiguous_iterator() : ptr{} {}
    angry_contiguous_iterator(const UnderlyingPtr ptr_, magic_word) : ptr(ptr_) {}

    reference operator*() const {
        return *ptr;
    }

    pointer operator->() const {
        return ptr;
    }

    decltype(auto) operator++() {
        ++ptr;
        return *this;
    }

    auto operator++(int) {
        angry_contiguous_iterator old(*this);
        ++ptr;
        return old;
    }

    decltype(auto) operator--() {
        --ptr;
        return *this;
    }

    auto operator--(int) {
        angry_contiguous_iterator old(*this);
        --ptr;
        return old;
    }

    auto operator<=>(const angry_contiguous_iterator o) const {
        return ptr <=> o.ptr;
    }

    auto operator==(const angry_contiguous_iterator o) const {
        return ptr == o.ptr;
    }

    friend auto operator+(const difference_type diff, const angry_contiguous_iterator it) {
        return angry_contiguous_iterator{static_cast<underlying_diff_type>(diff) + it.ptr, magic_word{}};
    }

    friend auto operator+(const angry_contiguous_iterator it, const difference_type diff) {
        return angry_contiguous_iterator{it.ptr + static_cast<underlying_diff_type>(diff), magic_word{}};
    }

    friend auto operator-(const angry_contiguous_iterator it, const difference_type diff) {
        return angry_contiguous_iterator{it.ptr - static_cast<underlying_diff_type>(diff), magic_word{}};
    }

    friend auto operator-(const angry_contiguous_iterator it, angry_contiguous_iterator it_other) {
        return difference_type{it.ptr - it_other.ptr};
    }

    decltype(auto) operator+=(const difference_type diff) {
        ptr += static_cast<underlying_diff_type>(diff);
        return *this;
    }

    decltype(auto) operator-=(const difference_type diff) {
        ptr -= static_cast<underlying_diff_type>(diff);
        return *this;
    }

    decltype(auto) operator[](const difference_type diff) const {
        return ptr[static_cast<underlying_diff_type>(diff)];
    }

    UnderlyingPtr ptr;
};

static_assert(contiguous_iterator<angry_contiguous_iterator<int*>>);

int main() {
    int arr[] = {200, 210, 220, 250, 240, 250, 250, 270, 280, 290, 300, 310, 320, 250, 340, 250, 250, 370, 380, 390};
    constexpr auto arr_size = size(arr);

    angry_contiguous_iterator arr_begin(begin(arr), magic_word{});
    angry_contiguous_iterator arr_end(end(arr), magic_word{});

    assert(find(arr_begin, arr_end, 250) == arr_begin + _Signed128{3});
    assert(ranges::find(arr_begin, arr_end, 250) == arr_begin + _Signed128{3});
#if _HAS_CXX23
    assert(begin(ranges::find_last(arr_begin, arr_end, 250)) == arr_begin + _Signed128{16});
#endif
    assert(search_n(arr_begin, arr_end, 2, 250) == arr_begin + _Signed128{5});
    assert(begin(ranges::search_n(arr_begin, arr_end, 2, 250)) == arr_begin + _Signed128{5});

    {
        int needle[] = {100, 300, 500, 700, 900};
        angry_contiguous_iterator needle_begin(begin(needle), magic_word{});
        angry_contiguous_iterator needle_end(end(needle), magic_word{});

        assert(find_first_of(arr_begin, arr_end, needle_begin, needle_end) == arr_begin + _Signed128{10});
        assert(ranges::find_first_of(arr_begin, arr_end, needle_begin, needle_end) == arr_begin + _Signed128{10});
    }

    assert(adjacent_find(arr_begin, arr_end) == arr_begin + _Signed128{5});
    assert(ranges::adjacent_find(arr_begin, arr_end) == arr_begin + _Signed128{5});

    {
        // We're short of 32 and 64 bit elements 'search' and 'find_end' for now
        short short_arr[arr_size];
        angry_contiguous_iterator short_arr_begin(begin(short_arr), magic_word{});
        angry_contiguous_iterator short_arr_end(end(short_arr), magic_word{});

        transform(arr_begin, arr_end, short_arr_begin, [](int v) { return static_cast<short>(v); });

        short short_needle[] = {300, 310, 320};

        angry_contiguous_iterator short_needle_begin(begin(short_needle), magic_word{});
        angry_contiguous_iterator short_needle_end(end(short_needle), magic_word{});

        assert(search(short_arr_begin, short_arr_end, short_needle_begin, short_needle_end)
               == short_arr_begin + _Signed128{10});
        assert(begin(ranges::search(short_arr_begin, short_arr_end, short_needle_begin, short_needle_end))
               == short_arr_begin + _Signed128{10});

        assert(find_end(short_arr_begin, short_arr_end, short_needle_begin, short_needle_end)
               == short_arr_begin + _Signed128{10});
        assert(begin(ranges::find_end(short_arr_begin, short_arr_end, short_needle_begin, short_needle_end))
               == short_arr_begin + _Signed128{10});
    }

    assert(count(arr_begin, arr_end, 250) == 6);
    assert(ranges::count(arr_begin, arr_end, 250) == 6);

    {
        int arr_cmp[] = {200, 210, 220, 230, 240, 250};

        angry_contiguous_iterator arr_cmp_begin(begin(arr_cmp), magic_word{});
        angry_contiguous_iterator arr_cmp_end(end(arr_cmp), magic_word{});

        assert(!equal(arr_cmp_begin, arr_cmp_end, arr_begin));
        assert(!equal(arr_cmp_begin, arr_cmp_end, arr_begin, arr_end));
        assert(!ranges::equal(arr_cmp_begin, arr_cmp_end, arr_begin, arr_end));
        assert(mismatch(arr_cmp_begin, arr_cmp_end, arr_begin).first == arr_cmp_begin + _Signed128{3});
        assert(mismatch(arr_cmp_begin, arr_cmp_end, arr_begin, arr_end).first == arr_cmp_begin + _Signed128{3});
        assert(ranges::mismatch(arr_cmp_begin, arr_cmp_end, arr_begin, arr_end).in1 == arr_cmp_begin + _Signed128{3});

        assert(lexicographical_compare(arr_cmp_begin, arr_cmp_end, arr_begin, arr_end));
        assert(ranges::lexicographical_compare(arr_cmp_begin, arr_cmp_end, arr_begin, arr_end));
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
        angry_contiguous_iterator float_arr_begin(begin(float_arr), magic_word{});
        angry_contiguous_iterator float_arr_end(end(float_arr), magic_word{});

        transform(arr_begin, arr_end, float_arr_begin, [](int v) { return static_cast<short>(v); });

        assert(ranges::min(ranges::subrange(float_arr_begin, float_arr_end)) == 200.0);
        assert(ranges::max(ranges::subrange(float_arr_begin, float_arr_end)) == 390.0);
        assert(ranges::minmax(ranges::subrange(float_arr_begin, float_arr_end)).max == 390.0);
    }

    assert(is_sorted_until(arr_begin, arr_end) == arr_begin + _Signed128{4});
    assert(ranges::is_sorted_until(arr_begin, arr_end) == arr_begin + _Signed128{4});

    {
        int arr_copy[arr_size];
        angry_contiguous_iterator arr_copy_begin(begin(arr_copy), magic_word{});
        angry_contiguous_iterator arr_copy_end(end(arr_copy), magic_word{});

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
        int arr_src[arr_size];
        angry_contiguous_iterator arr_src_begin(begin(arr_src), magic_word{});
        angry_contiguous_iterator arr_src_end(end(arr_src), magic_word{});

        int arr_dest[arr_size] = {};
        angry_contiguous_iterator arr_dest_begin(begin(arr_dest), magic_word{});
        angry_contiguous_iterator arr_dest_end(end(arr_dest), magic_word{});

        int remove_expected[] = {200, 210, 220, 240, 270, 280, 290, 300, 310, 320, 340, 370, 380, 390};

        copy(arr_begin, arr_end, arr_src_begin);
        auto rem_copy_it = remove_copy(arr_src_begin, arr_src_end, arr_dest_begin, 250);
        assert(equal(arr_dest_begin, rem_copy_it, begin(remove_expected), end(remove_expected)));
        auto rem_it = remove(arr_src_begin, arr_src_end, 250);
        assert(equal(arr_src_begin, rem_it, begin(remove_expected), end(remove_expected)));

        ranges::copy(arr_begin, arr_end, arr_src_begin);
        auto r_rem_copy_it = ranges::remove_copy(arr_src_begin, arr_src_end, arr_dest_begin, 250);
        assert(ranges::equal(arr_dest_begin, r_rem_copy_it.out, begin(remove_expected), end(remove_expected)));
        auto r_rem_it = ranges::remove(arr_src_begin, arr_src_end, 250);
        assert(ranges::equal(arr_src_begin, begin(r_rem_it), begin(remove_expected), end(remove_expected)));

        int unique_expected[] = {
            200, 210, 220, 250, 240, 250, 270, 280, 290, 300, 310, 320, 250, 340, 250, 370, 380, 390};

        copy(arr_begin, arr_end, arr_src_begin);
        auto un_copy_it = unique_copy(arr_src_begin, arr_src_end, arr_dest_begin);
        assert(equal(arr_dest_begin, un_copy_it, begin(unique_expected), end(unique_expected)));
        auto un_it = unique(arr_src_begin, arr_src_end);
        assert(equal(arr_src_begin, un_it, begin(unique_expected), end(unique_expected)));

        ranges::copy(arr_begin, arr_end, arr_src_begin);
        auto r_un_copy_it = ranges::unique_copy(arr_src_begin, arr_src_end, arr_dest_begin);
        assert(ranges::equal(arr_dest_begin, r_un_copy_it.out, begin(unique_expected), end(unique_expected)));
        auto r_un_it = ranges::unique(arr_src_begin, arr_src_end);
        assert(ranges::equal(arr_src_begin, begin(r_un_it), begin(unique_expected), end(unique_expected)));

        int reverse_expected[] = {
            390, 380, 370, 250, 250, 340, 250, 320, 310, 300, 290, 280, 270, 250, 250, 240, 250, 220, 210, 200};

        copy(arr_begin, arr_end, arr_src_begin);
        reverse_copy(arr_src_begin, arr_src_end, arr_dest_begin);
        assert(equal(arr_dest_begin, arr_dest_end, begin(reverse_expected), end(reverse_expected)));
        reverse(arr_src_begin, arr_src_end);
        assert(equal(arr_src_begin, arr_src_end, begin(reverse_expected), end(reverse_expected)));

        ranges::copy(arr_begin, arr_end, arr_src_begin);
        ranges::reverse_copy(arr_src_begin, arr_src_end, arr_dest_begin);
        assert(ranges::equal(arr_dest_begin, arr_dest_end, begin(reverse_expected), end(reverse_expected)));
        ranges::reverse(arr_src_begin, arr_src_end);
        assert(ranges::equal(arr_src_begin, arr_src_end, begin(reverse_expected), end(reverse_expected)));

        // Out of replace family, only replace for 32 and 64 bytes elements is manually vectorized,
        // the _copy version is auto vectorized
        int replace_expected[] = {
            200, 210, 220, 333, 240, 333, 333, 270, 280, 290, 300, 310, 320, 333, 340, 333, 333, 370, 380, 390};

        copy(arr_begin, arr_end, arr_src_begin);
        replace(arr_src_begin, arr_src_end, 250, 333);
        assert(equal(arr_src_begin, arr_src_end, begin(replace_expected), end(replace_expected)));

        ranges::copy(arr_begin, arr_end, arr_src_begin);
        ranges::replace(arr_src_begin, arr_src_end, 250, 333);
        assert(ranges::equal(arr_src_begin, arr_src_end, begin(replace_expected), end(replace_expected)));

        int swap_ranges_expected[] = {
            300, 310, 320, 250, 340, 250, 250, 370, 380, 390, 200, 210, 220, 250, 240, 250, 250, 270, 280, 290};

        copy(arr_begin, arr_end, arr_src_begin);
        swap_ranges(arr_src_begin, arr_src_begin + _Signed128{10}, arr_src_begin + _Signed128{10});
        assert(equal(arr_src_begin, arr_src_end, begin(swap_ranges_expected), end(swap_ranges_expected)));

        ranges::copy(arr_begin, arr_end, arr_src_begin);
        ranges::swap_ranges(arr_src_begin, arr_src_begin + _Signed128{10}, arr_src_begin + _Signed128{10}, arr_end);
        assert(ranges::equal(arr_src_begin, arr_src_end, begin(swap_ranges_expected), end(swap_ranges_expected)));
    }
}
