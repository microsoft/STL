// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <array>
#include <deque>
#include <filesystem>
#include <forward_list>
#include <list>
#include <map>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <xutility>

#include <range_algorithm_support.hpp>

using namespace std;

template <class Rng>
void test_algorithms(Rng& rng) {
    auto dist = ranges::distance(rng);
    vector<ranges::range_value_t<Rng>> other(static_cast<size_t>(dist));

    (void) ranges::mismatch(rng.begin(), rng.end(), other.begin(), other.end());
    (void) ranges::mismatch(other.begin(), other.end(), rng.begin(), rng.end());
    (void) ranges::mismatch(rng, other);
    (void) ranges::mismatch(other, rng);

    using range_ref_t   = ranges::range_reference_t<Rng>;
    using range_deref_t = remove_reference_t<range_ref_t>;
    if constexpr (is_lvalue_reference_v<range_ref_t> && !is_const_v<range_deref_t> && !is_volatile_v<range_deref_t>
                  && ranges::forward_range<Rng>) {
        (void) ranges::uninitialized_copy(rng.begin(), rng.end(), other.begin(), other.end());
        (void) ranges::uninitialized_copy(other.begin(), other.end(), rng.begin(), rng.end());
        (void) ranges::uninitialized_copy(rng, other);
        (void) ranges::uninitialized_copy(other, rng);

        (void) ranges::uninitialized_copy_n(rng.begin(), dist, other.begin(), other.end());
        (void) ranges::uninitialized_copy_n(other.begin(), dist, rng.begin(), rng.end());
    }
}

template <class Rng>
void test_unwrappable_range() {
    using It = ranges::iterator_t<Rng>;
    using Se = ranges::sentinel_t<Rng>;

    constexpr bool is_const_unwrappable = requires(const It& ci) { ci._Unwrapped(); };

    if constexpr (is_const_unwrappable) {
        STATIC_ASSERT(same_as<decltype(declval<It>()._Unwrapped()), decltype(declval<const It&>()._Unwrapped())>);
    }

    STATIC_ASSERT(ranges::_Unwrappable_sentinel_for<Se, It>);

    STATIC_ASSERT(same_as<ranges::_Unwrap_iter_t<It, Se>, decltype(declval<It>()._Unwrapped())>);
    if constexpr (is_const_unwrappable) {
        STATIC_ASSERT(same_as<ranges::_Unwrap_iter_t<const It&, Se>, decltype(declval<It>()._Unwrapped())>);
    }

    STATIC_ASSERT(same_as<ranges::_Unwrap_sent_t<Se, It>, decltype(declval<Se>()._Unwrapped())>);
    STATIC_ASSERT(same_as<ranges::_Unwrap_sent_t<const Se&, It>, decltype(declval<Se>()._Unwrapped())>);

    STATIC_ASSERT(noexcept(ranges::_Unwrap_iter<Se>(declval<It>())) == noexcept(declval<It>()._Unwrapped()));
    if constexpr (is_const_unwrappable) {
        STATIC_ASSERT(
            noexcept(ranges::_Unwrap_iter<Se>(declval<const It&>())) == noexcept(declval<const It&>()._Unwrapped()));
    }

    STATIC_ASSERT(noexcept(ranges::_Unwrap_sent<It>(declval<Se>())) == noexcept(declval<Se>()._Unwrapped()));
    STATIC_ASSERT(
        noexcept(ranges::_Unwrap_sent<It>(declval<const Se&>())) == noexcept(declval<const Se&>()._Unwrapped()));

    // instantiate without calling
    void (*p)(Rng&) = test_algorithms<Rng>;
    (void) p;
}

template <class Rng>
void test_not_unwrappable_range() {
    using It = ranges::iterator_t<Rng>;
    using Se = ranges::sentinel_t<Rng>;

    STATIC_ASSERT(!ranges::_Unwrappable_sentinel_for<Se, It>);
    STATIC_ASSERT(same_as<ranges::_Unwrap_iter_t<It, Se>, It>);
    STATIC_ASSERT(same_as<ranges::_Unwrap_iter_t<const It&, Se>, It>);

    STATIC_ASSERT(same_as<ranges::_Unwrap_sent_t<Se, It>, Se>);
    STATIC_ASSERT(same_as<ranges::_Unwrap_sent_t<const Se&, It>, Se>);

    STATIC_ASSERT(noexcept(ranges::_Unwrap_iter<Se>(declval<It>())));
    STATIC_ASSERT(noexcept(ranges::_Unwrap_iter<Se>(declval<const It&>())));

    STATIC_ASSERT(noexcept(ranges::_Unwrap_sent<It>(declval<Se>())));
    STATIC_ASSERT(noexcept(ranges::_Unwrap_sent<It>(declval<const Se&>())));

    // instantiate without calling
    void (*p)(Rng&) = test_algorithms<Rng>;
    (void) p;
}

template <class Rng>
void test_classic_range() {
    test_unwrappable_range<Rng>();
    test_unwrappable_range<const Rng>();
}
void test_classic_ranges() {
    test_classic_range<string>();
    test_classic_range<wstring>();

    test_classic_range<array<int, 32>>();
    test_classic_range<deque<int>>();
    test_classic_range<forward_list<int>>();
    test_classic_range<list<int>>();
    test_classic_range<set<int>>();
    test_classic_range<map<int, int>>();
    test_classic_range<unordered_set<int>>();
    test_classic_range<unordered_map<int, int>>();
    test_classic_range<vector<int>>();
    test_classic_range<filesystem::path>();

#if _HAS_CXX23
    test_classic_range<ranges::as_rvalue_view<string_view>>();
#endif
}

struct Nontrivial {
    Nontrivial() {}
    Nontrivial(const Nontrivial&) {}
    Nontrivial(Nontrivial&&) noexcept {}
    Nontrivial& operator=(const Nontrivial&) {
        return *this;
    }
    Nontrivial& operator=(Nontrivial&&) noexcept {
        return *this;
    }
    ~Nontrivial() {}

    bool operator==(const Nontrivial&) const noexcept = default;
};

template <class Iter, class Sent>
void test_unwrappable_views() {
    using R = ranges::subrange<Iter, Sent>;

    test_unwrappable_range<R>();

#if _HAS_CXX23
    test_unwrappable_range<ranges::as_rvalue_view<R>>();
#endif
}

template <class Iter, class Sent>
void test_not_unwrappable_views() {
    using R = ranges::subrange<Iter, Sent>;

    test_not_unwrappable_range<R>();

#if _HAS_CXX23
    test_not_unwrappable_range<ranges::as_rvalue_view<R>>();
#endif
}

void test_both_unwrappable() {
    using test::contiguous, test::random, test::bidi, test::fwd, test::input;

    using sent_int = test::sentinel<int>;
    using sent_nt  = test::sentinel<Nontrivial>;

    test_unwrappable_views<test::iterator<contiguous, int>, sent_int>();
    test_unwrappable_views<test::iterator<random, int>, sent_int>();
    test_unwrappable_views<test::iterator<bidi, int>, sent_int>();
    test_unwrappable_views<test::iterator<fwd, int>, sent_int>();
    test_unwrappable_views<test::iterator<input, int>, sent_int>();

    test_unwrappable_views<test::iterator<contiguous, Nontrivial>, sent_nt>();
    test_unwrappable_views<test::iterator<random, Nontrivial>, sent_nt>();
    test_unwrappable_views<test::iterator<bidi, Nontrivial>, sent_nt>();
    test_unwrappable_views<test::iterator<fwd, Nontrivial>, sent_nt>();
    test_unwrappable_views<test::iterator<input, Nontrivial>, sent_nt>();
}

void test_iter_unwrappable() {
    using test::contiguous, test::random, test::bidi, test::fwd, test::input;

    using sent_int = test::sentinel<int, test::WrappedState::ignorant>;
    using sent_nt  = test::sentinel<Nontrivial, test::WrappedState::ignorant>;

    test_not_unwrappable_views<test::iterator<contiguous, int>, sent_int>();
    test_not_unwrappable_views<test::iterator<random, int>, sent_int>();
    test_not_unwrappable_views<test::iterator<bidi, int>, sent_int>();
    test_not_unwrappable_views<test::iterator<fwd, int>, sent_int>();
    test_not_unwrappable_views<test::iterator<input, int>, sent_int>();

    test_not_unwrappable_views<test::iterator<contiguous, Nontrivial>, sent_nt>();
    test_not_unwrappable_views<test::iterator<random, Nontrivial>, sent_nt>();
    test_not_unwrappable_views<test::iterator<bidi, Nontrivial>, sent_nt>();
    test_not_unwrappable_views<test::iterator<fwd, Nontrivial>, sent_nt>();
    test_not_unwrappable_views<test::iterator<input, Nontrivial>, sent_nt>();
}

void test_sent_unwrappable() {
    using test::contiguous, test::random, test::bidi, test::fwd, test::input;

    using sent_int = test::sentinel<int>;
    using sent_nt  = test::sentinel<Nontrivial>;

    test_not_unwrappable_views<test::iterator<contiguous, int>::unwrapping_ignorant, sent_int>();
    test_not_unwrappable_views<test::iterator<random, int>::unwrapping_ignorant, sent_int>();
    test_not_unwrappable_views<test::iterator<bidi, int>::unwrapping_ignorant, sent_int>();
    test_not_unwrappable_views<test::iterator<fwd, int>::unwrapping_ignorant, sent_int>();
    test_not_unwrappable_views<test::iterator<input, int>::unwrapping_ignorant, sent_int>();

    test_not_unwrappable_views<test::iterator<contiguous, Nontrivial>::unwrapping_ignorant, sent_nt>();
    test_not_unwrappable_views<test::iterator<random, Nontrivial>::unwrapping_ignorant, sent_nt>();
    test_not_unwrappable_views<test::iterator<bidi, Nontrivial>::unwrapping_ignorant, sent_nt>();
    test_not_unwrappable_views<test::iterator<fwd, Nontrivial>::unwrapping_ignorant, sent_nt>();
    test_not_unwrappable_views<test::iterator<input, Nontrivial>::unwrapping_ignorant, sent_nt>();
}

void test_neither_unwrappable() {
    using test::contiguous, test::random, test::bidi, test::fwd, test::input;

    using sent_int = test::sentinel<int, test::WrappedState::ignorant>;
    using sent_nt  = test::sentinel<Nontrivial, test::WrappedState::ignorant>;

    test_not_unwrappable_views<test::iterator<contiguous, int>::unwrapping_ignorant, sent_int>();
    test_not_unwrappable_views<test::iterator<random, int>::unwrapping_ignorant, sent_int>();
    test_not_unwrappable_views<test::iterator<bidi, int>::unwrapping_ignorant, sent_int>();
    test_not_unwrappable_views<test::iterator<fwd, int>::unwrapping_ignorant, sent_int>();
    test_not_unwrappable_views<test::iterator<input, int>::unwrapping_ignorant, sent_int>();

    test_not_unwrappable_views<test::iterator<contiguous, Nontrivial>::unwrapping_ignorant, sent_nt>();
    test_not_unwrappable_views<test::iterator<random, Nontrivial>::unwrapping_ignorant, sent_nt>();
    test_not_unwrappable_views<test::iterator<bidi, Nontrivial>::unwrapping_ignorant, sent_nt>();
    test_not_unwrappable_views<test::iterator<fwd, Nontrivial>::unwrapping_ignorant, sent_nt>();
    test_not_unwrappable_views<test::iterator<input, Nontrivial>::unwrapping_ignorant, sent_nt>();
}
