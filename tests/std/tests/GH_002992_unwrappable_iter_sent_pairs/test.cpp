// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <array>
#include <filesystem>
#include <forward_list>
#include <list>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <xutility>

#include <range_algorithm_support.hpp>

using namespace std;

template <class Rng>
void test_algorithms(Rng& rng) {
    auto dist  = ranges::distance(rng);
    auto other = vector<ranges::range_value_t<Rng>>(static_cast<size_t>(dist));

    (void) ranges::mismatch(rng.begin(), rng.end(), other.begin(), other.end());
    (void) ranges::mismatch(other.begin(), other.end(), rng.begin(), rng.end());
    (void) ranges::mismatch(rng, other);
    (void) ranges::mismatch(other, rng);

    if constexpr (is_lvalue_reference_v<ranges::range_reference_t<Rng>> && ranges::forward_range<Rng>) {
        (void) ranges::uninitialized_copy(rng.begin(), rng.end(), other.begin(), other.end());
        (void) ranges::uninitialized_copy(other.begin(), other.end(), rng.begin(), rng.end());
        (void) ranges::uninitialized_copy(rng, other);
        (void) ranges::uninitialized_copy(other, rng);

        (void) ranges::uninitialized_copy_n(rng.begin(), dist, other.begin(), other.end());
        (void) ranges::uninitialized_copy_n(other.begin(), dist, rng.begin(), rng.end());
    }
}

template <class It, class Se>
void test_unwrappable_pair() {
    constexpr static bool is_const_unwrappable = requires(const It& ci) {
        ci._Unwrapped();
    };

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
    void (*p)(ranges::subrange<It, Se>&) = test_algorithms<ranges::subrange<It, Se>>;
    (void) p;
}
template <class It, class Se>
void test_not_unwrappable_pair() {
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
    void (*p)(ranges::subrange<It, Se>&) = test_algorithms<ranges::subrange<It, Se>>;
    (void) p;
}

template <class Rng>
void test_classic_range() {
    test_unwrappable_pair<ranges::iterator_t<Rng>, ranges::sentinel_t<Rng>>();
    test_unwrappable_pair<ranges::iterator_t<const Rng>, ranges::sentinel_t<const Rng>>();
}
void test_classic_ranges() {
    test_classic_range<string>();
    test_classic_range<wstring>();

    test_classic_range<array<int, 32>>();
    test_classic_range<forward_list<int>>();
    test_classic_range<list<int>>();
    test_classic_range<set<int>>();
    test_classic_range<map<int, int>>();
    test_classic_range<unordered_set<int>>();
    test_classic_range<unordered_map<int, int>>();
    test_classic_range<vector<int>>();
    test_classic_range<filesystem::path>();
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

    bool operator==(const Nontrivial&) const noexcept {
        return true;
    }
};

template <class Category, class Element, test::IsWrapped Wrapped>
using test_iter =
    test::iterator<Category, Element, test::CanDifference{derived_from<Category, random_access_iterator_tag>},
        test::CanCompare{derived_from<Category, forward_iterator_tag>},
        test::ProxyRef{!derived_from<Category, contiguous_iterator_tag>}, Wrapped>;
template <class Element, test::IsWrapped Wrapped>
using test_sent = test::sentinel<Element, Wrapped>;

void test_both_unwrappable() {
    constexpr static auto wyes = test::IsWrapped::yes;

    using test::contiguous, test::random, test::bidi, test::input;

    using sent_int = test_sent<int, wyes>;
    using sent_nt  = test_sent<Nontrivial, wyes>;

    test_unwrappable_pair<test_iter<contiguous, int, wyes>, sent_int>();
    test_unwrappable_pair<test_iter<random, int, wyes>, sent_int>();
    test_unwrappable_pair<test_iter<bidi, int, wyes>, sent_int>();
    test_unwrappable_pair<test_iter<input, int, wyes>, sent_int>();

    test_unwrappable_pair<test_iter<contiguous, Nontrivial, wyes>, sent_nt>();
    test_unwrappable_pair<test_iter<random, Nontrivial, wyes>, sent_nt>();
    test_unwrappable_pair<test_iter<bidi, Nontrivial, wyes>, sent_nt>();
    test_unwrappable_pair<test_iter<input, Nontrivial, wyes>, sent_nt>();
}

void test_iter_unwrappable() {
    constexpr static auto wyes = test::IsWrapped::yes;
    constexpr static auto wno  = test::IsWrapped::no;

    using test::contiguous, test::random, test::bidi, test::input;

    using sent_int = test_sent<int, wno>;
    using sent_nt  = test_sent<Nontrivial, wno>;

    test_not_unwrappable_pair<test_iter<contiguous, int, wyes>, sent_int>();
    test_not_unwrappable_pair<test_iter<random, int, wyes>, sent_int>();
    test_not_unwrappable_pair<test_iter<bidi, int, wyes>, sent_int>();
    test_not_unwrappable_pair<test_iter<input, int, wyes>, sent_int>();

    test_not_unwrappable_pair<test_iter<contiguous, Nontrivial, wyes>, sent_nt>();
    test_not_unwrappable_pair<test_iter<random, Nontrivial, wyes>, sent_nt>();
    test_not_unwrappable_pair<test_iter<bidi, Nontrivial, wyes>, sent_nt>();
    test_not_unwrappable_pair<test_iter<input, Nontrivial, wyes>, sent_nt>();
}

void test_sent_unwrappable() {
    constexpr static auto wyes = test::IsWrapped::yes;
    constexpr static auto wno  = test::IsWrapped::no;

    using test::contiguous, test::random, test::bidi, test::input;

    using sent_int = test_sent<int, wyes>;
    using sent_nt  = test_sent<Nontrivial, wyes>;

    test_not_unwrappable_pair<test_iter<contiguous, int, wno>, sent_int>();
    test_not_unwrappable_pair<test_iter<random, int, wno>, sent_int>();
    test_not_unwrappable_pair<test_iter<bidi, int, wno>, sent_int>();
    test_not_unwrappable_pair<test_iter<input, int, wno>, sent_int>();

    test_not_unwrappable_pair<test_iter<contiguous, Nontrivial, wno>, sent_nt>();
    test_not_unwrappable_pair<test_iter<random, Nontrivial, wno>, sent_nt>();
    test_not_unwrappable_pair<test_iter<bidi, Nontrivial, wno>, sent_nt>();
    test_not_unwrappable_pair<test_iter<input, Nontrivial, wno>, sent_nt>();
}

void test_no_unwrappable() {
    constexpr static auto wno = test::IsWrapped::no;

    using test::contiguous, test::random, test::bidi, test::input;

    using sent_int = test_sent<int, wno>;
    using sent_nt  = test_sent<Nontrivial, wno>;

    test_not_unwrappable_pair<test_iter<contiguous, int, wno>, sent_int>();
    test_not_unwrappable_pair<test_iter<random, int, wno>, sent_int>();
    test_not_unwrappable_pair<test_iter<bidi, int, wno>, sent_int>();
    test_not_unwrappable_pair<test_iter<input, int, wno>, sent_int>();

    test_not_unwrappable_pair<test_iter<contiguous, Nontrivial, wno>, sent_nt>();
    test_not_unwrappable_pair<test_iter<random, Nontrivial, wno>, sent_nt>();
    test_not_unwrappable_pair<test_iter<bidi, Nontrivial, wno>, sent_nt>();
    test_not_unwrappable_pair<test_iter<input, Nontrivial, wno>, sent_nt>();
}

int main() {
    test_classic_ranges();
    test_both_unwrappable();
    test_iter_unwrappable();
    test_sent_unwrappable();
    test_no_unwrappable();
}
