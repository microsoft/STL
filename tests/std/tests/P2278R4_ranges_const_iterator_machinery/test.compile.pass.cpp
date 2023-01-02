// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <deque>
#include <forward_list>
#include <istream>
#include <list>
#include <ranges>
#include <type_traits>
#include <utility>
#include <vector>

using namespace std;

template <class It>
concept CanIterConstRef = requires { typename iter_const_reference_t<It>; };

template <class It>
concept CanConstIterator = requires(It it) {
                               typename const_iterator<It>;
                               { make_const_iterator(move(it)) } -> same_as<const_iterator<It>>;
                           };

template <class Se>
concept CanConstSentinel = requires(Se se) {
                               typename const_sentinel<Se>;
                               { make_const_sentinel(move(se)) } -> same_as<const_sentinel<Se>>;
                           };

static_assert(!CanIterConstRef<int>);
static_assert(!CanIterConstRef<list<int>>);
static_assert(!CanIterConstRef<deque<int>>);

namespace test_pointer {
    using Ptr = int*;
    static_assert(CanIterConstRef<Ptr>);
    static_assert(CanConstIterator<Ptr>);
    static_assert(CanConstSentinel<Ptr>);
    static_assert(same_as<iter_const_reference_t<Ptr>, const int&>);
    static_assert(same_as<const_iterator<Ptr>, basic_const_iterator<Ptr>>);
    static_assert(same_as<iter_reference_t<const_iterator<Ptr>>, const int&>);
    static_assert(same_as<const_sentinel<Ptr>, basic_const_iterator<Ptr>>);
    static_assert(same_as<iter_reference_t<const_sentinel<Ptr>>, const int&>);

    using ConstPtr = const int*;
    static_assert(CanIterConstRef<ConstPtr>);
    static_assert(CanConstIterator<ConstPtr>);
    static_assert(CanConstSentinel<ConstPtr>);
    static_assert(same_as<iter_const_reference_t<ConstPtr>, const int&>);
    static_assert(same_as<const_iterator<ConstPtr>, ConstPtr>);
    static_assert(same_as<iter_reference_t<const_iterator<ConstPtr>>, const int&>);
    static_assert(same_as<const_sentinel<ConstPtr>, ConstPtr>);

    // Validate common_type
    static_assert(same_as<common_type_t<basic_const_iterator<Ptr>, ConstPtr>, basic_const_iterator<ConstPtr>>);
    static_assert(same_as<common_type_t<ConstPtr, basic_const_iterator<Ptr>>, basic_const_iterator<ConstPtr>>);
    static_assert(same_as<common_type_t<basic_const_iterator<Ptr>, basic_const_iterator<ConstPtr>>,
        basic_const_iterator<ConstPtr>>);
} // namespace test_pointer

namespace test_random_access_iter {
    using Iter = deque<int>::iterator;
    static_assert(CanIterConstRef<Iter>);
    static_assert(CanConstIterator<Iter>);
    static_assert(CanConstSentinel<Iter>);
    static_assert(same_as<iter_const_reference_t<Iter>, const int&>);
    static_assert(same_as<const_iterator<Iter>, basic_const_iterator<Iter>>);
    static_assert(same_as<iter_reference_t<const_iterator<Iter>>, const int&>);
    static_assert(same_as<const_sentinel<Iter>, basic_const_iterator<Iter>>);
    static_assert(same_as<iter_reference_t<const_sentinel<Iter>>, const int&>);

    using ConstIter = deque<int>::const_iterator;
    static_assert(CanIterConstRef<ConstIter>);
    static_assert(CanConstIterator<ConstIter>);
    static_assert(CanConstSentinel<ConstIter>);
    static_assert(same_as<iter_const_reference_t<ConstIter>, const int&>);
    static_assert(same_as<const_iterator<ConstIter>, ConstIter>);
    static_assert(same_as<iter_reference_t<const_iterator<ConstIter>>, const int&>);
    static_assert(same_as<const_sentinel<ConstIter>, ConstIter>);
} // namespace test_random_access_iter

namespace test_bidirectional_iter {
    using Iter = list<int>::iterator;
    static_assert(CanIterConstRef<Iter>);
    static_assert(CanConstIterator<Iter>);
    static_assert(CanConstSentinel<Iter>);
    static_assert(same_as<iter_const_reference_t<Iter>, const int&>);
    static_assert(same_as<const_iterator<Iter>, basic_const_iterator<Iter>>);
    static_assert(same_as<iter_reference_t<const_iterator<Iter>>, const int&>);
    static_assert(same_as<const_sentinel<Iter>, basic_const_iterator<Iter>>);
    static_assert(same_as<iter_reference_t<const_sentinel<Iter>>, const int&>);

    using ConstIter = list<int>::const_iterator;
    static_assert(CanIterConstRef<ConstIter>);
    static_assert(CanConstIterator<ConstIter>);
    static_assert(CanConstSentinel<ConstIter>);
    static_assert(same_as<iter_const_reference_t<ConstIter>, const int&>);
    static_assert(same_as<const_iterator<ConstIter>, ConstIter>);
    static_assert(same_as<iter_reference_t<const_iterator<ConstIter>>, const int&>);
    static_assert(same_as<const_sentinel<ConstIter>, ConstIter>);
} // namespace test_bidirectional_iter

namespace test_forward_iter {
    using Iter = forward_list<int>::iterator;
    static_assert(CanIterConstRef<Iter>);
    static_assert(CanConstIterator<Iter>);
    static_assert(CanConstSentinel<Iter>);
    static_assert(same_as<iter_const_reference_t<Iter>, const int&>);
    static_assert(same_as<const_iterator<Iter>, basic_const_iterator<Iter>>);
    static_assert(same_as<iter_reference_t<const_iterator<Iter>>, const int&>);
    static_assert(same_as<const_sentinel<Iter>, basic_const_iterator<Iter>>);
    static_assert(same_as<iter_reference_t<const_sentinel<Iter>>, const int&>);

    using ConstIter = forward_list<int>::const_iterator;
    static_assert(CanIterConstRef<ConstIter>);
    static_assert(CanConstIterator<ConstIter>);
    static_assert(CanConstSentinel<ConstIter>);
    static_assert(same_as<iter_const_reference_t<ConstIter>, const int&>);
    static_assert(same_as<const_iterator<ConstIter>, ConstIter>);
    static_assert(same_as<iter_reference_t<const_iterator<ConstIter>>, const int&>);
    static_assert(same_as<const_sentinel<ConstIter>, ConstIter>);
} // namespace test_forward_iter

namespace test_input_iter {
    using Iter = ranges::iterator_t<ranges::istream_view<int>>;
    static_assert(CanIterConstRef<Iter>);
    static_assert(CanConstIterator<Iter>);
    static_assert(!CanConstSentinel<Iter>);
    static_assert(same_as<iter_const_reference_t<Iter>, const int&>);
    static_assert(same_as<const_iterator<Iter>, basic_const_iterator<Iter>>);
    static_assert(same_as<iter_reference_t<const_iterator<Iter>>, const int&>);
} // namespace test_input_iter

namespace test_prvalue_iter {
    using TransformView = ranges::transform_view<ranges::ref_view<deque<float>>, int (*)(float)>;

    using Iter = ranges::iterator_t<TransformView>;
    static_assert(CanIterConstRef<Iter>);
    static_assert(CanConstIterator<Iter>);
    static_assert(CanConstSentinel<Iter>);
    static_assert(same_as<iter_const_reference_t<Iter>, int>);
    static_assert(same_as<const_iterator<Iter>, Iter>);
    static_assert(same_as<iter_reference_t<const_iterator<Iter>>, int>);
    static_assert(same_as<const_sentinel<Iter>, Iter>);
    static_assert(same_as<iter_reference_t<const_sentinel<Iter>>, int>);

    using ConstIter = ranges::iterator_t<const TransformView>;
    static_assert(CanIterConstRef<ConstIter>);
    static_assert(CanConstIterator<ConstIter>);
    static_assert(CanConstSentinel<ConstIter>);
    static_assert(same_as<iter_const_reference_t<ConstIter>, int>);
    static_assert(same_as<const_iterator<ConstIter>, ConstIter>);
    static_assert(same_as<iter_reference_t<const_iterator<ConstIter>>, int>);
    static_assert(same_as<const_sentinel<ConstIter>, ConstIter>);
} // namespace test_prvalue_iter

namespace test_vector_bool_iter {
    using Iter = vector<bool>::iterator;
    static_assert(CanIterConstRef<Iter>);
    static_assert(CanConstIterator<Iter>);
    static_assert(CanConstSentinel<Iter>);
    static_assert(same_as<iter_const_reference_t<Iter>, bool>);
    static_assert(same_as<const_iterator<Iter>, basic_const_iterator<Iter>>);
    static_assert(same_as<iter_reference_t<const_iterator<Iter>>, bool>);
    static_assert(same_as<const_sentinel<Iter>, basic_const_iterator<Iter>>);
    static_assert(same_as<iter_reference_t<const_sentinel<Iter>>, bool>);

    using ConstIter = vector<bool>::const_iterator;
    static_assert(CanIterConstRef<ConstIter>);
    static_assert(CanConstIterator<ConstIter>);
    static_assert(CanConstSentinel<ConstIter>);
    static_assert(same_as<iter_const_reference_t<ConstIter>, bool>);
    static_assert(same_as<const_iterator<ConstIter>, ConstIter>);
    static_assert(same_as<iter_reference_t<const_iterator<ConstIter>>, bool>);
    static_assert(same_as<const_sentinel<ConstIter>, ConstIter>);
} // namespace test_vector_bool_iter

// Test standard sentinels
static_assert(!CanIterConstRef<default_sentinel_t>);
static_assert(!CanConstIterator<default_sentinel_t>);
static_assert(CanConstSentinel<default_sentinel_t>);
static_assert(same_as<const_sentinel<default_sentinel_t>, default_sentinel_t>);

static_assert(!CanIterConstRef<unreachable_sentinel_t>);
static_assert(!CanConstIterator<unreachable_sentinel_t>);
static_assert(CanConstSentinel<unreachable_sentinel_t>);
static_assert(same_as<const_sentinel<unreachable_sentinel_t>, unreachable_sentinel_t>);

struct NotSemiregular {
    NotSemiregular()                      = default;
    NotSemiregular(const NotSemiregular&) = delete;
};

static_assert(!CanConstSentinel<NotSemiregular>);

int main() {} // COMPILE-ONLY
