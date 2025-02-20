// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <ranges>

#include "range_algorithm_support.hpp"

using namespace std;

constexpr auto pred = [](int i) { return i <= 42; };

template <class I>
void test_iota_transform() {
    constexpr int orig[]{42, 1729};
    int a[]{42, 1729};
    auto vw = views::iota(I{}, static_cast<I>(ranges::size(a)))
            | views::transform([&a](I i) -> auto& { return a[static_cast<size_t>(i)]; });

    static_assert(three_way_comparable<ranges::iterator_t<ranges::iota_view<I>>>); // TRANSITION, /permissive

    ranges::stable_sort(vw);
    assert(ranges::equal(a, orig));

    ranges::stable_sort(vw.begin(), vw.end());
    assert(ranges::equal(a, orig));

    ranges::inplace_merge(vw, ranges::next(vw.begin()));
    assert(ranges::equal(a, orig));
    ranges::inplace_merge(vw.begin(), ranges::next(vw.begin()), vw.end());
    assert(ranges::equal(a, orig));

    ranges::stable_partition(vw, pred);
    assert(ranges::equal(a, orig));
    ranges::stable_partition(vw.begin(), vw.end(), pred);
    assert(ranges::equal(a, orig));
}

void test_iota_transform_all() {
    test_iota_transform<signed char>();
    test_iota_transform<short>();
    test_iota_transform<int>();
    test_iota_transform<long>();
    test_iota_transform<long long>();

    test_iota_transform<unsigned char>();
    test_iota_transform<unsigned short>();
    test_iota_transform<unsigned int>();
    test_iota_transform<unsigned long>();
    test_iota_transform<unsigned long long>();

    test_iota_transform<char>();
#ifdef __cpp_char8_t
    test_iota_transform<char8_t>();
#endif // defined(__cpp_char8_t)
    test_iota_transform<char16_t>();
    test_iota_transform<char32_t>();
    test_iota_transform<wchar_t>();
}

template <class I>
void test_redifference() {
    constexpr int orig[]{42, 1729};
    int a[]{42, 1729};
    auto vw = test::make_redifference_subrange<I>(a);

    ranges::stable_sort(vw);
    assert(ranges::equal(a, orig));

    ranges::stable_sort(vw.begin(), vw.end());
    assert(ranges::equal(a, orig));

    ranges::inplace_merge(vw, ranges::next(vw.begin()));
    assert(ranges::equal(a, orig));
    ranges::inplace_merge(vw.begin(), ranges::next(vw.begin()), vw.end());
    assert(ranges::equal(a, orig));

    ranges::stable_partition(vw, pred);
    assert(ranges::equal(a, orig));
    ranges::stable_partition(vw.begin(), vw.end(), pred);
    assert(ranges::equal(a, orig));
}

void test_redifference_all() {
    test_redifference<signed char>();
    test_redifference<short>();
    test_redifference<int>();
    test_redifference<long>();
    test_redifference<long long>();
    test_redifference<_Signed128>();
}

int main() {
    test_iota_transform_all();
    test_redifference_all();
}
