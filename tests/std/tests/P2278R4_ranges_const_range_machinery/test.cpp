// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <concepts>
#include <deque>
#include <forward_list>
#include <istream>
#include <list>
#include <ranges>
#include <type_traits>
#include <vector>

#include <range_algorithm_support.hpp>

// New members (cbegin and cend) of `view_interface` are tested in `P0896R4_ranges_subrange\test.cpp`.
// Updated CPOs (cbegin, cdata, ...) are tested in `P0896R4_ranges_range_machinery\test.cpp`

using namespace std;

template <class Rng>
concept CanRangeConstIterator = requires { typename ranges::const_iterator_t<Rng>; };

template <class Rng>
concept CanRangeConstSentinel = requires { typename ranges::const_sentinel_t<Rng>; };

template <class Rng>
concept CanRangeConstReference = requires { typename ranges::range_const_reference_t<Rng>; };

static_assert(!CanRangeConstIterator<void*>);
static_assert(!CanRangeConstSentinel<void*>);
static_assert(!CanRangeConstReference<void*>);

namespace test_array {
    using Arr = int[10];
    static_assert(CanRangeConstIterator<Arr>);
    static_assert(CanRangeConstSentinel<Arr>);
    static_assert(CanRangeConstReference<Arr>);
    static_assert(same_as<ranges::const_iterator_t<Arr>, const_iterator<int*>>);
    static_assert(same_as<ranges::const_sentinel_t<Arr>, const_iterator<int*>>);
    static_assert(same_as<ranges::range_const_reference_t<Arr>, const int&>);
    static_assert(!ranges::constant_range<Arr>);

    using ConstArr = const Arr;
    static_assert(CanRangeConstIterator<ConstArr>);
    static_assert(CanRangeConstSentinel<ConstArr>);
    static_assert(CanRangeConstReference<ConstArr>);
    static_assert(same_as<ranges::const_iterator_t<ConstArr>, const int*>);
    static_assert(same_as<ranges::const_sentinel_t<ConstArr>, const int*>);
    static_assert(same_as<ranges::range_const_reference_t<ConstArr>, const int&>);
    static_assert(ranges::constant_range<ConstArr>);
} // namespace test_array

namespace test_random_access_range {
    using Rng = deque<int>;
    static_assert(CanRangeConstIterator<Rng>);
    static_assert(CanRangeConstSentinel<Rng>);
    static_assert(CanRangeConstReference<Rng>);
    static_assert(same_as<ranges::const_iterator_t<Rng>, const_iterator<Rng::iterator>>);
    static_assert(same_as<ranges::const_sentinel_t<Rng>, const_iterator<Rng::iterator>>);
    static_assert(same_as<ranges::range_const_reference_t<Rng>, const int&>);
    static_assert(!ranges::constant_range<Rng>);

    using ConstRng = const deque<int>;
    static_assert(CanRangeConstIterator<ConstRng>);
    static_assert(CanRangeConstSentinel<ConstRng>);
    static_assert(CanRangeConstReference<ConstRng>);
    static_assert(same_as<ranges::const_iterator_t<ConstRng>, ConstRng::const_iterator>);
    static_assert(same_as<ranges::const_sentinel_t<ConstRng>, ConstRng::const_iterator>);
    static_assert(same_as<ranges::range_const_reference_t<ConstRng>, const int&>);
    static_assert(ranges::constant_range<ConstRng>);
} // namespace test_random_access_range

namespace test_bidirectional_range {
    using Rng = list<int>;
    static_assert(CanRangeConstIterator<Rng>);
    static_assert(CanRangeConstSentinel<Rng>);
    static_assert(CanRangeConstReference<Rng>);
    static_assert(same_as<ranges::const_iterator_t<Rng>, const_iterator<Rng::iterator>>);
    static_assert(same_as<ranges::const_sentinel_t<Rng>, const_iterator<Rng::iterator>>);
    static_assert(same_as<ranges::range_const_reference_t<Rng>, const int&>);
    static_assert(!ranges::constant_range<Rng>);

    using ConstRng = const list<int>;
    static_assert(CanRangeConstIterator<ConstRng>);
    static_assert(CanRangeConstSentinel<ConstRng>);
    static_assert(CanRangeConstReference<ConstRng>);
    static_assert(same_as<ranges::const_iterator_t<ConstRng>, ConstRng::const_iterator>);
    static_assert(same_as<ranges::const_sentinel_t<ConstRng>, ConstRng::const_iterator>);
    static_assert(same_as<ranges::range_const_reference_t<ConstRng>, const int&>);
    static_assert(ranges::constant_range<ConstRng>);
} // namespace test_bidirectional_range

namespace test_forward_range {
    using Rng = forward_list<int>;
    static_assert(CanRangeConstIterator<Rng>);
    static_assert(CanRangeConstSentinel<Rng>);
    static_assert(CanRangeConstReference<Rng>);
    static_assert(same_as<ranges::const_iterator_t<Rng>, const_iterator<Rng::iterator>>);
    static_assert(same_as<ranges::const_sentinel_t<Rng>, const_iterator<Rng::iterator>>);
    static_assert(same_as<ranges::range_const_reference_t<Rng>, const int&>);
    static_assert(!ranges::constant_range<Rng>);

    using ConstRng = const forward_list<int>;
    static_assert(CanRangeConstIterator<ConstRng>);
    static_assert(CanRangeConstSentinel<ConstRng>);
    static_assert(CanRangeConstReference<ConstRng>);
    static_assert(same_as<ranges::const_iterator_t<ConstRng>, ConstRng::const_iterator>);
    static_assert(same_as<ranges::const_sentinel_t<ConstRng>, ConstRng::const_iterator>);
    static_assert(same_as<ranges::range_const_reference_t<ConstRng>, const int&>);
    static_assert(ranges::constant_range<ConstRng>);
} // namespace test_forward_range

namespace test_input_range {
    using Rng = ranges::istream_view<int>;
    static_assert(CanRangeConstIterator<Rng>);
    static_assert(CanRangeConstSentinel<Rng>);
    static_assert(CanRangeConstReference<Rng>);
    static_assert(same_as<ranges::const_iterator_t<Rng>, const_iterator<ranges::iterator_t<Rng>>>);
    static_assert(same_as<ranges::const_sentinel_t<Rng>, default_sentinel_t>);
    static_assert(same_as<ranges::range_const_reference_t<Rng>, const int&>);
    static_assert(!ranges::constant_range<Rng>);
} // namespace test_input_range

namespace test_prvalue_range {
    using Rng = ranges::transform_view<ranges::ref_view<deque<float>>, int (*)(float)>;
    static_assert(CanRangeConstIterator<Rng>);
    static_assert(CanRangeConstSentinel<Rng>);
    static_assert(CanRangeConstReference<Rng>);
    static_assert(same_as<ranges::const_iterator_t<Rng>, const_iterator<ranges::iterator_t<Rng>>>);
    static_assert(same_as<ranges::const_sentinel_t<Rng>, const_iterator<ranges::iterator_t<Rng>>>);
    static_assert(same_as<ranges::range_const_reference_t<Rng>, int>);
    static_assert(ranges::constant_range<Rng>);

    using ConstRng = const Rng;
    static_assert(CanRangeConstIterator<ConstRng>);
    static_assert(CanRangeConstSentinel<ConstRng>);
    static_assert(CanRangeConstReference<ConstRng>);
    static_assert(same_as<ranges::const_iterator_t<ConstRng>, const_iterator<ranges::iterator_t<ConstRng>>>);
    static_assert(same_as<ranges::const_sentinel_t<ConstRng>, const_iterator<ranges::iterator_t<ConstRng>>>);
    static_assert(same_as<ranges::range_const_reference_t<ConstRng>, int>);
    static_assert(ranges::constant_range<ConstRng>);
} // namespace test_prvalue_range

namespace test_vector_bool {
    using Vb = vector<bool>;
    static_assert(CanRangeConstIterator<Vb>);
    static_assert(CanRangeConstSentinel<Vb>);
    static_assert(CanRangeConstReference<Vb>);
    static_assert(same_as<ranges::const_iterator_t<Vb>, const_iterator<Vb::iterator>>);
    static_assert(same_as<ranges::const_sentinel_t<Vb>, const_iterator<Vb::iterator>>);
    static_assert(same_as<ranges::range_const_reference_t<Vb>, bool>);
    static_assert(!ranges::constant_range<Vb>);

    using ConstVb = const vector<bool>;
    static_assert(CanRangeConstIterator<ConstVb>);
    static_assert(CanRangeConstSentinel<ConstVb>);
    static_assert(CanRangeConstReference<ConstVb>);
    static_assert(same_as<ranges::const_iterator_t<ConstVb>, ConstVb::const_iterator>);
    static_assert(same_as<ranges::const_sentinel_t<ConstVb>, ConstVb::const_iterator>);
    static_assert(same_as<ranges::range_const_reference_t<ConstVb>, bool>);
    static_assert(ranges::constant_range<ConstVb>);
} // namespace test_vector_bool

template <class Rng>
static constexpr void test_cpos(Rng&& rng) {
    using ranges::iterator_t, ranges::sentinel_t;
    using R = conditional_t<ranges::constant_range<const Rng&> && !ranges::constant_range<Rng&>, const Rng&, Rng&>;

    { // Validate ranges::cbegin
        using It = iterator_t<R>;

        const same_as<const_iterator<It>> auto it = ranges::cbegin(rng);
        if constexpr (equality_comparable<It>) {
            if (ranges::forward_range<Rng>) { // intentionally not if constexpr
                assert(it == ranges::begin(rng));
            }
        }

        static_assert(noexcept(ranges::cbegin(rng))
                      == (noexcept(ranges::begin(rng)) && is_nothrow_constructible_v<const_iterator<It>, It>) );
    }

    { // Validate ranges::cend
        using Se = sentinel_t<R>;

        const same_as<const_sentinel<Se>> auto se = ranges::cend(rng);
        if constexpr (equality_comparable<Se>) {
            assert(se == ranges::end(rng));
        }

        static_assert(noexcept(ranges::cend(rng))
                      == (noexcept(ranges::end(rng)) && is_nothrow_constructible_v<const_sentinel<Se>, Se>) );
    }

    if constexpr (ranges::bidirectional_range<Rng>) {
        if constexpr (CanRBegin<Rng>) { // Validate ranges::crbegin
            using RevIt = decltype(ranges::rbegin(declval<R&>()));

            const same_as<const_iterator<RevIt>> auto it = ranges::crbegin(rng);
            assert(it == ranges::rbegin(rng));

            static_assert(
                noexcept(ranges::crbegin(rng))
                == (noexcept(ranges::rbegin(rng)) && is_nothrow_constructible_v<const_iterator<RevIt>, RevIt>) );
        }

        if constexpr (CanREnd<Rng>) { // Validate ranges::crend
            using RevSe = decltype(ranges::rend(declval<R&>()));

            const same_as<const_sentinel<RevSe>> auto it = ranges::crend(rng);
            assert(it == ranges::rend(rng));

            static_assert(
                noexcept(ranges::crend(rng))
                == (noexcept(ranges::rend(rng)) && is_nothrow_constructible_v<const_sentinel<RevSe>, RevSe>) );
        }
    }

    if constexpr (ranges::contiguous_range<Rng>) { // Validate ranges::cdata
        const same_as<const ranges::range_value_t<Rng>*> auto ptr = ranges::cdata(rng);
        assert(ptr == ranges::data(rng));

        static_assert(noexcept(ranges::cdata(rng)) == noexcept(ranges::data(rng)));
    }
}

struct instantiator {
    template <class Rng>
    static constexpr void call() {
        int some_ints[] = {1, 2, 3};
        Rng rng{some_ints};
        test_cpos(rng);
    }
};

int main() {
    STATIC_ASSERT((test_in<instantiator, const int>(), true));
    test_in<instantiator, const int>();
}
