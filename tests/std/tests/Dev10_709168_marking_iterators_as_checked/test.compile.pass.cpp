// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <array>
#include <cstddef>
#include <deque>
#include <filesystem>
#include <forward_list>
#include <iterator>
#include <list>
#include <map>
#include <set>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

struct MeowIterator;

namespace std {
    // Previously pseudo-documented way to mark iterators checked should still compile, no longer used by the STL:
    template <>
    struct _Is_checked_helper<MeowIterator> : true_type {};
} // namespace std

using namespace std;

template <class T>
struct DerivedFrom : T {};

constexpr bool stl_checked  = _ITERATOR_DEBUG_LEVEL != 0;
constexpr bool stl_checked2 = _ITERATOR_DEBUG_LEVEL == 2;

STATIC_ASSERT(stl_checked == _Range_verifiable_v<string::iterator>);
STATIC_ASSERT(stl_checked == _Range_verifiable_v<string::const_iterator>);
STATIC_ASSERT(stl_checked == _Range_verifiable_v<string::reverse_iterator>);
STATIC_ASSERT(stl_checked == _Range_verifiable_v<string::const_reverse_iterator>);

STATIC_ASSERT(stl_checked == _Range_verifiable_v<array<int, 2>::iterator>);
STATIC_ASSERT(stl_checked == _Range_verifiable_v<array<int, 2>::const_iterator>);
STATIC_ASSERT(stl_checked == _Range_verifiable_v<array<int, 2>::reverse_iterator>);
STATIC_ASSERT(stl_checked == _Range_verifiable_v<array<int, 2>::const_reverse_iterator>);
STATIC_ASSERT(stl_checked == _Range_verifiable_v<deque<int>::iterator>);
STATIC_ASSERT(stl_checked == _Range_verifiable_v<deque<int>::const_iterator>);
STATIC_ASSERT(stl_checked == _Range_verifiable_v<deque<int>::reverse_iterator>);
STATIC_ASSERT(stl_checked == _Range_verifiable_v<deque<int>::const_reverse_iterator>);
STATIC_ASSERT(stl_checked2 == _Range_verifiable_v<forward_list<int>::iterator>);
STATIC_ASSERT(stl_checked2 == _Range_verifiable_v<forward_list<int>::const_iterator>);
STATIC_ASSERT(stl_checked2 == _Range_verifiable_v<list<int>::iterator>);
STATIC_ASSERT(stl_checked2 == _Range_verifiable_v<list<int>::const_iterator>);
STATIC_ASSERT(stl_checked2 == _Range_verifiable_v<list<int>::reverse_iterator>);
STATIC_ASSERT(stl_checked2 == _Range_verifiable_v<list<int>::const_reverse_iterator>);
STATIC_ASSERT(stl_checked == _Range_verifiable_v<vector<int>::iterator>);
STATIC_ASSERT(stl_checked == _Range_verifiable_v<vector<int>::const_iterator>);
STATIC_ASSERT(stl_checked == _Range_verifiable_v<vector<int>::reverse_iterator>);
STATIC_ASSERT(stl_checked == _Range_verifiable_v<vector<int>::const_reverse_iterator>);
STATIC_ASSERT(stl_checked == _Range_verifiable_v<vector<bool>::iterator>);
STATIC_ASSERT(stl_checked == _Range_verifiable_v<vector<bool>::const_iterator>);
STATIC_ASSERT(stl_checked == _Range_verifiable_v<vector<bool>::reverse_iterator>);
STATIC_ASSERT(stl_checked == _Range_verifiable_v<vector<bool>::const_reverse_iterator>);

STATIC_ASSERT(stl_checked2 == _Range_verifiable_v<map<int, int>::iterator>);
STATIC_ASSERT(stl_checked2 == _Range_verifiable_v<map<int, int>::const_iterator>);
STATIC_ASSERT(stl_checked2 == _Range_verifiable_v<map<int, int>::reverse_iterator>);
STATIC_ASSERT(stl_checked2 == _Range_verifiable_v<map<int, int>::const_reverse_iterator>);
STATIC_ASSERT(stl_checked2 == _Range_verifiable_v<multimap<int, int>::iterator>);
STATIC_ASSERT(stl_checked2 == _Range_verifiable_v<multimap<int, int>::const_iterator>);
STATIC_ASSERT(stl_checked2 == _Range_verifiable_v<multimap<int, int>::reverse_iterator>);
STATIC_ASSERT(stl_checked2 == _Range_verifiable_v<multimap<int, int>::const_reverse_iterator>);
STATIC_ASSERT(stl_checked2 == _Range_verifiable_v<set<int>::iterator>);
STATIC_ASSERT(stl_checked2 == _Range_verifiable_v<set<int>::const_iterator>);
STATIC_ASSERT(stl_checked2 == _Range_verifiable_v<set<int>::reverse_iterator>);
STATIC_ASSERT(stl_checked2 == _Range_verifiable_v<set<int>::const_reverse_iterator>);
STATIC_ASSERT(stl_checked2 == _Range_verifiable_v<multiset<int>::iterator>);
STATIC_ASSERT(stl_checked2 == _Range_verifiable_v<multiset<int>::const_iterator>);
STATIC_ASSERT(stl_checked2 == _Range_verifiable_v<multiset<int>::reverse_iterator>);
STATIC_ASSERT(stl_checked2 == _Range_verifiable_v<multiset<int>::const_reverse_iterator>);

STATIC_ASSERT(stl_checked2 == _Range_verifiable_v<unordered_map<int, int>::iterator>);
STATIC_ASSERT(stl_checked2 == _Range_verifiable_v<unordered_map<int, int>::const_iterator>);
STATIC_ASSERT(stl_checked2 == _Range_verifiable_v<unordered_multimap<int, int>::iterator>);
STATIC_ASSERT(stl_checked2 == _Range_verifiable_v<unordered_multimap<int, int>::const_iterator>);
STATIC_ASSERT(stl_checked2 == _Range_verifiable_v<unordered_set<int>::iterator>);
STATIC_ASSERT(stl_checked2 == _Range_verifiable_v<unordered_set<int>::const_iterator>);
STATIC_ASSERT(stl_checked2 == _Range_verifiable_v<unordered_multiset<int>::iterator>);
STATIC_ASSERT(stl_checked2 == _Range_verifiable_v<unordered_multiset<int>::const_iterator>);

STATIC_ASSERT(stl_checked == _Range_verifiable_v<int*>);
STATIC_ASSERT(stl_checked == _Range_verifiable_v<move_iterator<int*>>);
STATIC_ASSERT(stl_checked == _Range_verifiable_v<reverse_iterator<int*>>);

#if _HAS_CXX17
STATIC_ASSERT(stl_checked == _Range_verifiable_v<string_view::iterator>);
STATIC_ASSERT(stl_checked == _Range_verifiable_v<string_view::const_iterator>);
STATIC_ASSERT(stl_checked == _Range_verifiable_v<string_view::reverse_iterator>);
STATIC_ASSERT(stl_checked == _Range_verifiable_v<string_view::const_reverse_iterator>);

STATIC_ASSERT(stl_checked == _Range_verifiable_v<std::filesystem::path::iterator>);
STATIC_ASSERT(stl_checked == _Range_verifiable_v<std::filesystem::path::const_iterator>);
#endif // _HAS_CXX17

#if _HAS_CXX20
STATIC_ASSERT(stl_checked == _Range_verifiable_v<span<int>::iterator>);
STATIC_ASSERT(stl_checked == _Range_verifiable_v<span<int>::reverse_iterator>);
#endif // _HAS_CXX20

STATIC_ASSERT(_Range_verifiable_v<stdext::checked_array_iterator<int*>>);
STATIC_ASSERT(stl_checked == _Range_verifiable_v<stdext::unchecked_array_iterator<int*>>);

STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<string::iterator>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<string::const_iterator>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<string::reverse_iterator>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<string::const_reverse_iterator>>);

STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<array<int, 2>::iterator>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<array<int, 2>::const_iterator>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<array<int, 2>::reverse_iterator>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<array<int, 2>::const_reverse_iterator>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<deque<int>::iterator>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<deque<int>::const_iterator>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<deque<int>::reverse_iterator>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<deque<int>::const_reverse_iterator>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<forward_list<int>::iterator>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<forward_list<int>::const_iterator>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<list<int>::iterator>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<list<int>::const_iterator>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<list<int>::reverse_iterator>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<list<int>::const_reverse_iterator>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<vector<int>::iterator>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<vector<int>::const_iterator>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<vector<int>::reverse_iterator>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<vector<int>::const_reverse_iterator>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<vector<bool>::iterator>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<vector<bool>::const_iterator>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<vector<bool>::reverse_iterator>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<vector<bool>::const_reverse_iterator>>);

STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<map<int, int>::iterator>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<map<int, int>::const_iterator>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<map<int, int>::reverse_iterator>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<map<int, int>::const_reverse_iterator>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<multimap<int, int>::iterator>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<multimap<int, int>::const_iterator>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<multimap<int, int>::reverse_iterator>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<multimap<int, int>::const_reverse_iterator>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<set<int>::iterator>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<set<int>::const_iterator>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<set<int>::reverse_iterator>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<set<int>::const_reverse_iterator>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<multiset<int>::iterator>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<multiset<int>::const_iterator>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<multiset<int>::reverse_iterator>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<multiset<int>::const_reverse_iterator>>);

STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<unordered_map<int, int>::iterator>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<unordered_map<int, int>::const_iterator>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<unordered_multimap<int, int>::iterator>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<unordered_multimap<int, int>::const_iterator>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<unordered_set<int>::iterator>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<unordered_set<int>::const_iterator>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<unordered_multiset<int>::iterator>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<unordered_multiset<int>::const_iterator>>);

STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<move_iterator<int*>>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<reverse_iterator<int*>>>);

#if _HAS_CXX17
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<string_view::iterator>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<string_view::const_iterator>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<string_view::reverse_iterator>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<string_view::const_reverse_iterator>>);

STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<std::filesystem::path::iterator>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<std::filesystem::path::const_iterator>>);
#endif // _HAS_CXX17

#if _HAS_CXX20
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<span<int>::iterator>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<span<int>::reverse_iterator>>);
#endif // _HAS_CXX20

STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<stdext::checked_array_iterator<int*>>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<stdext::unchecked_array_iterator<int*>>>);

template <class I, bool Expected>
constexpr bool test_unwrappable() {
    STATIC_ASSERT(_Unwrappable_v<I&> == Expected);
    STATIC_ASSERT(_Unwrappable_v<const I&> == Expected);
    STATIC_ASSERT(_Unwrappable_v<I> == Expected);
    STATIC_ASSERT(_Unwrappable_v<const I> == Expected);
    return true;
}

template <class I, bool Expected>
constexpr bool test_unwrappable_for_unverified() {
    STATIC_ASSERT(_Unwrappable_for_unverified_v<I&> == Expected);
    STATIC_ASSERT(_Unwrappable_for_unverified_v<const I&> == Expected);
    STATIC_ASSERT(_Unwrappable_for_unverified_v<I> == Expected);
    STATIC_ASSERT(_Unwrappable_for_unverified_v<const I> == Expected);
    return true;
}

template <class I, bool Expected>
constexpr bool test_unwrappable_for_offset() {
    STATIC_ASSERT(_Unwrappable_for_offset_v<I&> == Expected);
    STATIC_ASSERT(_Unwrappable_for_offset_v<const I&> == Expected);
    STATIC_ASSERT(_Unwrappable_for_offset_v<I> == Expected);
    STATIC_ASSERT(_Unwrappable_for_offset_v<const I> == Expected);
    return true;
}

struct SimpleBidIt {
    using iterator_category = bidirectional_iterator_tag;
    using value_type        = int;
    using difference_type   = ptrdiff_t;
    using pointer           = int*;
    using reference         = int&;

    SimpleBidIt()                              = default;
    SimpleBidIt(const SimpleBidIt&)            = default;
    SimpleBidIt(SimpleBidIt&&)                 = default;
    SimpleBidIt& operator=(const SimpleBidIt&) = default;
    SimpleBidIt& operator=(SimpleBidIt&&)      = default;
    ~SimpleBidIt()                             = default;

    int& operator*() const {
        static int val{};
        return val;
    }
    int* operator->() const {
        return nullptr;
    }
    SimpleBidIt& operator++() {
        return *this;
    }
    void operator++(int) = delete; // avoid postincrement
    bool operator==(const SimpleBidIt&) const {
        return true;
    }
    bool operator!=(const SimpleBidIt&) const {
        return false;
    }

    SimpleBidIt& operator--() {
        return *this;
    }
    void operator--(int) = delete; // avoid postdecrement
};

STATIC_ASSERT(!_Range_verifiable_v<SimpleBidIt>);
STATIC_ASSERT(!_Range_verifiable_v<move_iterator<SimpleBidIt>>);
STATIC_ASSERT(!_Range_verifiable_v<reverse_iterator<SimpleBidIt>>);
STATIC_ASSERT(test_unwrappable<SimpleBidIt, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<SimpleBidIt, false>());
STATIC_ASSERT(test_unwrappable_for_offset<SimpleBidIt, false>());

STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<SimpleBidIt>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<move_iterator<SimpleBidIt>>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<reverse_iterator<SimpleBidIt>>>);
STATIC_ASSERT(test_unwrappable<::DerivedFrom<SimpleBidIt>, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<SimpleBidIt>, false>());
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<SimpleBidIt>, false>());

struct FancyBidIt {
    using iterator_category = bidirectional_iterator_tag;
    using value_type        = int;
    using difference_type   = ptrdiff_t;
    using pointer           = int*;
    using reference         = int&;

    FancyBidIt()                             = default;
    FancyBidIt(const FancyBidIt&)            = default;
    FancyBidIt(FancyBidIt&&)                 = default;
    FancyBidIt& operator=(const FancyBidIt&) = default;
    FancyBidIt& operator=(FancyBidIt&&)      = default;
    ~FancyBidIt()                            = default;

    int& operator*() const {
        static int val{};
        return val;
    }
    int* operator->() const {
        return nullptr;
    }
    FancyBidIt& operator++() {
        return *this;
    }
    void operator++(int) = delete; // avoid postincrement
    bool operator==(const FancyBidIt&) const {
        return true;
    }
    bool operator!=(const FancyBidIt&) const {
        return false;
    }

    FancyBidIt& operator--() {
        return *this;
    }
    void operator--(int) = delete; // avoid postdecrement

    friend void _Verify_range(FancyBidIt, FancyBidIt) {}
    void _Verify_offset(difference_type) const {}
    FancyBidIt _Unwrapped() const {
        return {};
    }
    void _Seek_to(FancyBidIt) {}
    static constexpr bool _Unwrap_when_unverified = true;
};

STATIC_ASSERT(_Range_verifiable_v<FancyBidIt>);
STATIC_ASSERT(_Range_verifiable_v<move_iterator<FancyBidIt>>);
STATIC_ASSERT(_Range_verifiable_v<reverse_iterator<FancyBidIt>>);
STATIC_ASSERT(test_unwrappable<FancyBidIt, true>());
STATIC_ASSERT(test_unwrappable_for_unverified<FancyBidIt, true>());
STATIC_ASSERT(test_unwrappable_for_offset<FancyBidIt, true>());

// User iterator types that want to use derivation can still do so even following resolution of VSO-664880
STATIC_ASSERT(_Range_verifiable_v<::DerivedFrom<FancyBidIt>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<move_iterator<FancyBidIt>>>);
STATIC_ASSERT(!_Range_verifiable_v<::DerivedFrom<reverse_iterator<FancyBidIt>>>);
STATIC_ASSERT(test_unwrappable<::DerivedFrom<FancyBidIt>, true>());
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<FancyBidIt>, true>());
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<FancyBidIt>, true>());

STATIC_ASSERT(test_unwrappable_for_unverified<string::iterator, !stl_checked>());
STATIC_ASSERT(test_unwrappable_for_unverified<string::const_iterator, !stl_checked>());
STATIC_ASSERT(test_unwrappable_for_unverified<string::reverse_iterator, !stl_checked>());
STATIC_ASSERT(test_unwrappable_for_unverified<string::const_reverse_iterator, !stl_checked>());

STATIC_ASSERT(test_unwrappable_for_unverified<array<int, 2>::iterator, !stl_checked>());
STATIC_ASSERT(test_unwrappable_for_unverified<array<int, 2>::const_iterator, !stl_checked>());
STATIC_ASSERT(test_unwrappable_for_unverified<array<int, 2>::reverse_iterator, !stl_checked>());
STATIC_ASSERT(test_unwrappable_for_unverified<array<int, 2>::const_reverse_iterator, !stl_checked>());
STATIC_ASSERT(test_unwrappable_for_unverified<deque<int>::iterator, !stl_checked>());
STATIC_ASSERT(test_unwrappable_for_unverified<deque<int>::const_iterator, !stl_checked>());
STATIC_ASSERT(test_unwrappable_for_unverified<deque<int>::reverse_iterator, !stl_checked>());
STATIC_ASSERT(test_unwrappable_for_unverified<deque<int>::const_reverse_iterator, !stl_checked>());
STATIC_ASSERT(test_unwrappable_for_unverified<forward_list<int>::iterator, !stl_checked>());
STATIC_ASSERT(test_unwrappable_for_unverified<forward_list<int>::const_iterator, !stl_checked>());
STATIC_ASSERT(test_unwrappable_for_unverified<list<int>::iterator, !stl_checked>());
STATIC_ASSERT(test_unwrappable_for_unverified<list<int>::const_iterator, !stl_checked>());
STATIC_ASSERT(test_unwrappable_for_unverified<list<int>::reverse_iterator, !stl_checked>());
STATIC_ASSERT(test_unwrappable_for_unverified<list<int>::const_reverse_iterator, !stl_checked>());
STATIC_ASSERT(test_unwrappable_for_unverified<vector<int>::iterator, !stl_checked>());
STATIC_ASSERT(test_unwrappable_for_unverified<vector<int>::const_iterator, !stl_checked>());
STATIC_ASSERT(test_unwrappable_for_unverified<vector<int>::reverse_iterator, !stl_checked>());
STATIC_ASSERT(test_unwrappable_for_unverified<vector<int>::const_reverse_iterator, !stl_checked>());
STATIC_ASSERT(test_unwrappable_for_unverified<vector<bool>::iterator, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<vector<bool>::const_iterator, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<vector<bool>::reverse_iterator, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<vector<bool>::const_reverse_iterator, false>());

STATIC_ASSERT(test_unwrappable_for_unverified<map<int, int>::iterator, !stl_checked>());
STATIC_ASSERT(test_unwrappable_for_unverified<map<int, int>::const_iterator, !stl_checked>());
STATIC_ASSERT(test_unwrappable_for_unverified<map<int, int>::reverse_iterator, !stl_checked>());
STATIC_ASSERT(test_unwrappable_for_unverified<map<int, int>::const_reverse_iterator, !stl_checked>());
STATIC_ASSERT(test_unwrappable_for_unverified<multimap<int, int>::iterator, !stl_checked>());
STATIC_ASSERT(test_unwrappable_for_unverified<multimap<int, int>::const_iterator, !stl_checked>());
STATIC_ASSERT(test_unwrappable_for_unverified<multimap<int, int>::reverse_iterator, !stl_checked>());
STATIC_ASSERT(test_unwrappable_for_unverified<multimap<int, int>::const_reverse_iterator, !stl_checked>());
STATIC_ASSERT(test_unwrappable_for_unverified<set<int>::iterator, !stl_checked>());
STATIC_ASSERT(test_unwrappable_for_unverified<set<int>::const_iterator, !stl_checked>());
STATIC_ASSERT(test_unwrappable_for_unverified<set<int>::reverse_iterator, !stl_checked>());
STATIC_ASSERT(test_unwrappable_for_unverified<set<int>::const_reverse_iterator, !stl_checked>());
STATIC_ASSERT(test_unwrappable_for_unverified<multiset<int>::iterator, !stl_checked>());
STATIC_ASSERT(test_unwrappable_for_unverified<multiset<int>::const_iterator, !stl_checked>());
STATIC_ASSERT(test_unwrappable_for_unverified<multiset<int>::reverse_iterator, !stl_checked>());
STATIC_ASSERT(test_unwrappable_for_unverified<multiset<int>::const_reverse_iterator, !stl_checked>());

STATIC_ASSERT(test_unwrappable_for_unverified<unordered_map<int, int>::iterator, !stl_checked>());
STATIC_ASSERT(test_unwrappable_for_unverified<unordered_map<int, int>::const_iterator, !stl_checked>());
STATIC_ASSERT(test_unwrappable_for_unverified<unordered_multimap<int, int>::iterator, !stl_checked>());
STATIC_ASSERT(test_unwrappable_for_unverified<unordered_multimap<int, int>::const_iterator, !stl_checked>());
STATIC_ASSERT(test_unwrappable_for_unverified<unordered_set<int>::iterator, !stl_checked>());
STATIC_ASSERT(test_unwrappable_for_unverified<unordered_set<int>::const_iterator, !stl_checked>());
STATIC_ASSERT(test_unwrappable_for_unverified<unordered_multiset<int>::iterator, !stl_checked>());
STATIC_ASSERT(test_unwrappable_for_unverified<unordered_multiset<int>::const_iterator, !stl_checked>());

STATIC_ASSERT(test_unwrappable_for_unverified<int*, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<move_iterator<int*>, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<reverse_iterator<int*>, false>());

#if _HAS_CXX17
STATIC_ASSERT(test_unwrappable_for_unverified<string_view::iterator, !stl_checked>());
STATIC_ASSERT(test_unwrappable_for_unverified<string_view::const_iterator, !stl_checked>());
STATIC_ASSERT(test_unwrappable_for_unverified<string_view::reverse_iterator, !stl_checked>());
STATIC_ASSERT(test_unwrappable_for_unverified<string_view::const_reverse_iterator, !stl_checked>());

STATIC_ASSERT(test_unwrappable_for_unverified<std::filesystem::path::iterator, !stl_checked>());
STATIC_ASSERT(test_unwrappable_for_unverified<std::filesystem::path::const_iterator, !stl_checked>());
#endif // _HAS_CXX17

#if _HAS_CXX20
STATIC_ASSERT(test_unwrappable_for_unverified<span<int>::iterator, !stl_checked>());
STATIC_ASSERT(test_unwrappable_for_unverified<span<int>::reverse_iterator, !stl_checked>());
#endif // _HAS_CXX20

STATIC_ASSERT(test_unwrappable_for_unverified<stdext::checked_array_iterator<int*>, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<stdext::unchecked_array_iterator<int*>, true>());

STATIC_ASSERT(test_unwrappable<string::iterator, true>());
STATIC_ASSERT(test_unwrappable<string::const_iterator, true>());
STATIC_ASSERT(test_unwrappable<string::reverse_iterator, true>());
STATIC_ASSERT(test_unwrappable<string::const_reverse_iterator, true>());

STATIC_ASSERT(test_unwrappable<array<int, 2>::iterator, true>());
STATIC_ASSERT(test_unwrappable<array<int, 2>::const_iterator, true>());
STATIC_ASSERT(test_unwrappable<array<int, 2>::reverse_iterator, true>());
STATIC_ASSERT(test_unwrappable<array<int, 2>::const_reverse_iterator, true>());
STATIC_ASSERT(test_unwrappable<deque<int>::iterator, true>());
STATIC_ASSERT(test_unwrappable<deque<int>::const_iterator, true>());
STATIC_ASSERT(test_unwrappable<deque<int>::reverse_iterator, true>());
STATIC_ASSERT(test_unwrappable<deque<int>::const_reverse_iterator, true>());
STATIC_ASSERT(test_unwrappable<forward_list<int>::iterator, true>());
STATIC_ASSERT(test_unwrappable<forward_list<int>::const_iterator, true>());
STATIC_ASSERT(test_unwrappable<list<int>::iterator, true>());
STATIC_ASSERT(test_unwrappable<list<int>::const_iterator, true>());
STATIC_ASSERT(test_unwrappable<list<int>::reverse_iterator, true>());
STATIC_ASSERT(test_unwrappable<list<int>::const_reverse_iterator, true>());
STATIC_ASSERT(test_unwrappable<vector<int>::iterator, true>());
STATIC_ASSERT(test_unwrappable<vector<int>::const_iterator, true>());
STATIC_ASSERT(test_unwrappable<vector<int>::reverse_iterator, true>());
STATIC_ASSERT(test_unwrappable<vector<int>::const_reverse_iterator, true>());
STATIC_ASSERT(test_unwrappable<vector<bool>::iterator, false>()); // :(
STATIC_ASSERT(test_unwrappable<vector<bool>::const_iterator, false>());
STATIC_ASSERT(test_unwrappable<vector<bool>::reverse_iterator, false>());
STATIC_ASSERT(test_unwrappable<vector<bool>::const_reverse_iterator, false>());

STATIC_ASSERT(test_unwrappable<map<int, int>::iterator, true>());
STATIC_ASSERT(test_unwrappable<map<int, int>::const_iterator, true>());
STATIC_ASSERT(test_unwrappable<map<int, int>::reverse_iterator, true>());
STATIC_ASSERT(test_unwrappable<map<int, int>::const_reverse_iterator, true>());
STATIC_ASSERT(test_unwrappable<multimap<int, int>::iterator, true>());
STATIC_ASSERT(test_unwrappable<multimap<int, int>::const_iterator, true>());
STATIC_ASSERT(test_unwrappable<multimap<int, int>::reverse_iterator, true>());
STATIC_ASSERT(test_unwrappable<multimap<int, int>::const_reverse_iterator, true>());
STATIC_ASSERT(test_unwrappable<set<int>::iterator, true>());
STATIC_ASSERT(test_unwrappable<set<int>::const_iterator, true>());
STATIC_ASSERT(test_unwrappable<set<int>::reverse_iterator, true>());
STATIC_ASSERT(test_unwrappable<set<int>::const_reverse_iterator, true>());
STATIC_ASSERT(test_unwrappable<multiset<int>::iterator, true>());
STATIC_ASSERT(test_unwrappable<multiset<int>::const_iterator, true>());
STATIC_ASSERT(test_unwrappable<multiset<int>::reverse_iterator, true>());
STATIC_ASSERT(test_unwrappable<multiset<int>::const_reverse_iterator, true>());

STATIC_ASSERT(test_unwrappable<unordered_map<int, int>::iterator, true>());
STATIC_ASSERT(test_unwrappable<unordered_map<int, int>::const_iterator, true>());
STATIC_ASSERT(test_unwrappable<unordered_multimap<int, int>::iterator, true>());
STATIC_ASSERT(test_unwrappable<unordered_multimap<int, int>::const_iterator, true>());
STATIC_ASSERT(test_unwrappable<unordered_set<int>::iterator, true>());
STATIC_ASSERT(test_unwrappable<unordered_set<int>::const_iterator, true>());
STATIC_ASSERT(test_unwrappable<unordered_multiset<int>::iterator, true>());
STATIC_ASSERT(test_unwrappable<unordered_multiset<int>::const_iterator, true>());

STATIC_ASSERT(test_unwrappable<int*, false>());
STATIC_ASSERT(test_unwrappable<move_iterator<int*>, false>());
STATIC_ASSERT(test_unwrappable<reverse_iterator<int*>, false>());

#if _HAS_CXX17
STATIC_ASSERT(test_unwrappable<string_view::iterator, true>());
STATIC_ASSERT(test_unwrappable<string_view::const_iterator, true>());
STATIC_ASSERT(test_unwrappable<string_view::reverse_iterator, true>());
STATIC_ASSERT(test_unwrappable<string_view::const_reverse_iterator, true>());

STATIC_ASSERT(test_unwrappable<std::filesystem::path::iterator, true>());
STATIC_ASSERT(test_unwrappable<std::filesystem::path::const_iterator, true>());
#endif // _HAS_CXX17

#if _HAS_CXX20
STATIC_ASSERT(test_unwrappable<span<int>::iterator, true>());
STATIC_ASSERT(test_unwrappable<span<int>::reverse_iterator, true>());
#endif // _HAS_CXX20

STATIC_ASSERT(test_unwrappable<stdext::checked_array_iterator<int*>, true>());
STATIC_ASSERT(test_unwrappable<stdext::unchecked_array_iterator<int*>, true>());

STATIC_ASSERT(test_unwrappable_for_offset<string::iterator, true>());
STATIC_ASSERT(test_unwrappable_for_offset<string::const_iterator, true>());
STATIC_ASSERT(test_unwrappable_for_offset<string::reverse_iterator, true>());
STATIC_ASSERT(test_unwrappable_for_offset<string::const_reverse_iterator, true>());

STATIC_ASSERT(test_unwrappable_for_offset<array<int, 2>::iterator, stl_checked>());
STATIC_ASSERT(test_unwrappable_for_offset<array<int, 2>::const_iterator, stl_checked>());
STATIC_ASSERT(test_unwrappable_for_offset<array<int, 2>::reverse_iterator, stl_checked>());
STATIC_ASSERT(test_unwrappable_for_offset<array<int, 2>::const_reverse_iterator, stl_checked>());
STATIC_ASSERT(test_unwrappable_for_offset<deque<int>::iterator, true>());
STATIC_ASSERT(test_unwrappable_for_offset<deque<int>::const_iterator, true>());
STATIC_ASSERT(test_unwrappable_for_offset<deque<int>::reverse_iterator, true>());
STATIC_ASSERT(test_unwrappable_for_offset<deque<int>::const_reverse_iterator, true>());
STATIC_ASSERT(test_unwrappable_for_offset<forward_list<int>::iterator, false>());
STATIC_ASSERT(test_unwrappable_for_offset<forward_list<int>::const_iterator, false>());
STATIC_ASSERT(test_unwrappable_for_offset<list<int>::iterator, false>());
STATIC_ASSERT(test_unwrappable_for_offset<list<int>::const_iterator, false>());
STATIC_ASSERT(test_unwrappable_for_offset<list<int>::reverse_iterator, false>());
STATIC_ASSERT(test_unwrappable_for_offset<list<int>::const_reverse_iterator, false>());
STATIC_ASSERT(test_unwrappable_for_offset<vector<int>::iterator, true>());
STATIC_ASSERT(test_unwrappable_for_offset<vector<int>::const_iterator, true>());
STATIC_ASSERT(test_unwrappable_for_offset<vector<int>::reverse_iterator, true>());
STATIC_ASSERT(test_unwrappable_for_offset<vector<int>::const_reverse_iterator, true>());
STATIC_ASSERT(test_unwrappable_for_offset<vector<bool>::iterator, false>()); // :(
STATIC_ASSERT(test_unwrappable_for_offset<vector<bool>::const_iterator, false>());
STATIC_ASSERT(test_unwrappable_for_offset<vector<bool>::reverse_iterator, false>());
STATIC_ASSERT(test_unwrappable_for_offset<vector<bool>::const_reverse_iterator, false>());

STATIC_ASSERT(test_unwrappable_for_offset<map<int, int>::iterator, false>());
STATIC_ASSERT(test_unwrappable_for_offset<map<int, int>::const_iterator, false>());
STATIC_ASSERT(test_unwrappable_for_offset<map<int, int>::reverse_iterator, false>());
STATIC_ASSERT(test_unwrappable_for_offset<map<int, int>::const_reverse_iterator, false>());
STATIC_ASSERT(test_unwrappable_for_offset<multimap<int, int>::iterator, false>());
STATIC_ASSERT(test_unwrappable_for_offset<multimap<int, int>::const_iterator, false>());
STATIC_ASSERT(test_unwrappable_for_offset<multimap<int, int>::reverse_iterator, false>());
STATIC_ASSERT(test_unwrappable_for_offset<multimap<int, int>::const_reverse_iterator, false>());
STATIC_ASSERT(test_unwrappable_for_offset<set<int>::iterator, false>());
STATIC_ASSERT(test_unwrappable_for_offset<set<int>::const_iterator, false>());
STATIC_ASSERT(test_unwrappable_for_offset<set<int>::reverse_iterator, false>());
STATIC_ASSERT(test_unwrappable_for_offset<set<int>::const_reverse_iterator, false>());
STATIC_ASSERT(test_unwrappable_for_offset<multiset<int>::iterator, false>());
STATIC_ASSERT(test_unwrappable_for_offset<multiset<int>::const_iterator, false>());
STATIC_ASSERT(test_unwrappable_for_offset<multiset<int>::reverse_iterator, false>());
STATIC_ASSERT(test_unwrappable_for_offset<multiset<int>::const_reverse_iterator, false>());

STATIC_ASSERT(test_unwrappable_for_offset<unordered_map<int, int>::iterator, false>());
STATIC_ASSERT(test_unwrappable_for_offset<unordered_map<int, int>::const_iterator, false>());
STATIC_ASSERT(test_unwrappable_for_offset<unordered_multimap<int, int>::iterator, false>());
STATIC_ASSERT(test_unwrappable_for_offset<unordered_multimap<int, int>::const_iterator, false>());
STATIC_ASSERT(test_unwrappable_for_offset<unordered_set<int>::iterator, false>());
STATIC_ASSERT(test_unwrappable_for_offset<unordered_set<int>::const_iterator, false>());
STATIC_ASSERT(test_unwrappable_for_offset<unordered_multiset<int>::iterator, false>());
STATIC_ASSERT(test_unwrappable_for_offset<unordered_multiset<int>::const_iterator, false>());

STATIC_ASSERT(test_unwrappable_for_offset<int*, false>());
STATIC_ASSERT(test_unwrappable_for_offset<move_iterator<int*>, false>());
STATIC_ASSERT(test_unwrappable_for_offset<reverse_iterator<int*>, false>());

#if _HAS_CXX17
STATIC_ASSERT(test_unwrappable_for_offset<string_view::iterator, true>());
STATIC_ASSERT(test_unwrappable_for_offset<string_view::const_iterator, true>());
STATIC_ASSERT(test_unwrappable_for_offset<string_view::reverse_iterator, true>());
STATIC_ASSERT(test_unwrappable_for_offset<string_view::const_reverse_iterator, true>());

STATIC_ASSERT(test_unwrappable_for_offset<std::filesystem::path::iterator, false>());
STATIC_ASSERT(test_unwrappable_for_offset<std::filesystem::path::const_iterator, false>());
#endif // _HAS_CXX17

#if _HAS_CXX20
STATIC_ASSERT(test_unwrappable_for_offset<span<int>::iterator, true>());
STATIC_ASSERT(test_unwrappable_for_offset<span<int>::reverse_iterator, true>());
#endif // _HAS_CXX20

STATIC_ASSERT(test_unwrappable_for_offset<stdext::checked_array_iterator<int*>, true>());
STATIC_ASSERT(test_unwrappable_for_offset<stdext::unchecked_array_iterator<int*>, false>());

STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<string::iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<string::const_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<string::reverse_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<string::const_reverse_iterator>, false>());

STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<array<int, 2>::iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<array<int, 2>::const_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<array<int, 2>::reverse_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<array<int, 2>::const_reverse_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<deque<int>::iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<deque<int>::const_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<deque<int>::reverse_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<deque<int>::const_reverse_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<forward_list<int>::iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<forward_list<int>::const_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<list<int>::iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<list<int>::const_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<list<int>::reverse_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<list<int>::const_reverse_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<vector<int>::iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<vector<int>::const_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<vector<int>::reverse_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<vector<int>::const_reverse_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<vector<bool>::iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<vector<bool>::const_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<vector<bool>::reverse_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<vector<bool>::const_reverse_iterator>, false>());

STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<map<int, int>::iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<map<int, int>::const_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<map<int, int>::reverse_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<map<int, int>::const_reverse_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<multimap<int, int>::iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<multimap<int, int>::const_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<multimap<int, int>::reverse_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<multimap<int, int>::const_reverse_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<set<int>::iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<set<int>::const_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<set<int>::reverse_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<set<int>::const_reverse_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<multiset<int>::iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<multiset<int>::const_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<multiset<int>::reverse_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<multiset<int>::const_reverse_iterator>, false>());

STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<unordered_map<int, int>::iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<unordered_map<int, int>::const_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<unordered_multimap<int, int>::iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<unordered_multimap<int, int>::const_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<unordered_set<int>::iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<unordered_set<int>::const_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<unordered_multiset<int>::iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<unordered_multiset<int>::const_iterator>, false>());

STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<move_iterator<int*>>, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<reverse_iterator<int*>>, false>());

#if _HAS_CXX17
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<string_view::iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<string_view::const_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<string_view::reverse_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<string_view::const_reverse_iterator>, false>());

STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<std::filesystem::path::iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<std::filesystem::path::const_iterator>, false>());
#endif // _HAS_CXX17

#if _HAS_CXX20
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<span<int>::iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<span<int>::reverse_iterator>, false>());
#endif // _HAS_CXX20

STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<stdext::checked_array_iterator<int*>>, false>());
STATIC_ASSERT(test_unwrappable_for_unverified<::DerivedFrom<stdext::unchecked_array_iterator<int*>>, false>());

STATIC_ASSERT(test_unwrappable<::DerivedFrom<string::iterator>, false>());
STATIC_ASSERT(test_unwrappable<::DerivedFrom<string::const_iterator>, false>());
STATIC_ASSERT(test_unwrappable<::DerivedFrom<string::reverse_iterator>, false>());
STATIC_ASSERT(test_unwrappable<::DerivedFrom<string::const_reverse_iterator>, false>());

STATIC_ASSERT(test_unwrappable<::DerivedFrom<array<int, 2>::iterator>, false>());
STATIC_ASSERT(test_unwrappable<::DerivedFrom<array<int, 2>::const_iterator>, false>());
STATIC_ASSERT(test_unwrappable<::DerivedFrom<array<int, 2>::reverse_iterator>, false>());
STATIC_ASSERT(test_unwrappable<::DerivedFrom<array<int, 2>::const_reverse_iterator>, false>());
STATIC_ASSERT(test_unwrappable<::DerivedFrom<deque<int>::iterator>, false>());
STATIC_ASSERT(test_unwrappable<::DerivedFrom<deque<int>::const_iterator>, false>());
STATIC_ASSERT(test_unwrappable<::DerivedFrom<deque<int>::reverse_iterator>, false>());
STATIC_ASSERT(test_unwrappable<::DerivedFrom<deque<int>::const_reverse_iterator>, false>());
STATIC_ASSERT(test_unwrappable<::DerivedFrom<forward_list<int>::iterator>, false>());
STATIC_ASSERT(test_unwrappable<::DerivedFrom<forward_list<int>::const_iterator>, false>());
STATIC_ASSERT(test_unwrappable<::DerivedFrom<list<int>::iterator>, false>());
STATIC_ASSERT(test_unwrappable<::DerivedFrom<list<int>::const_iterator>, false>());
STATIC_ASSERT(test_unwrappable<::DerivedFrom<list<int>::reverse_iterator>, false>());
STATIC_ASSERT(test_unwrappable<::DerivedFrom<list<int>::const_reverse_iterator>, false>());
STATIC_ASSERT(test_unwrappable<::DerivedFrom<vector<int>::iterator>, false>());
STATIC_ASSERT(test_unwrappable<::DerivedFrom<vector<int>::const_iterator>, false>());
STATIC_ASSERT(test_unwrappable<::DerivedFrom<vector<int>::reverse_iterator>, false>());
STATIC_ASSERT(test_unwrappable<::DerivedFrom<vector<int>::const_reverse_iterator>, false>());
STATIC_ASSERT(test_unwrappable<::DerivedFrom<vector<bool>::iterator>, false>());
STATIC_ASSERT(test_unwrappable<::DerivedFrom<vector<bool>::const_iterator>, false>());
STATIC_ASSERT(test_unwrappable<::DerivedFrom<vector<bool>::reverse_iterator>, false>());
STATIC_ASSERT(test_unwrappable<::DerivedFrom<vector<bool>::const_reverse_iterator>, false>());

STATIC_ASSERT(test_unwrappable<::DerivedFrom<map<int, int>::iterator>, false>());
STATIC_ASSERT(test_unwrappable<::DerivedFrom<map<int, int>::const_iterator>, false>());
STATIC_ASSERT(test_unwrappable<::DerivedFrom<map<int, int>::reverse_iterator>, false>());
STATIC_ASSERT(test_unwrappable<::DerivedFrom<map<int, int>::const_reverse_iterator>, false>());
STATIC_ASSERT(test_unwrappable<::DerivedFrom<multimap<int, int>::iterator>, false>());
STATIC_ASSERT(test_unwrappable<::DerivedFrom<multimap<int, int>::const_iterator>, false>());
STATIC_ASSERT(test_unwrappable<::DerivedFrom<multimap<int, int>::reverse_iterator>, false>());
STATIC_ASSERT(test_unwrappable<::DerivedFrom<multimap<int, int>::const_reverse_iterator>, false>());
STATIC_ASSERT(test_unwrappable<::DerivedFrom<set<int>::iterator>, false>());
STATIC_ASSERT(test_unwrappable<::DerivedFrom<set<int>::const_iterator>, false>());
STATIC_ASSERT(test_unwrappable<::DerivedFrom<set<int>::reverse_iterator>, false>());
STATIC_ASSERT(test_unwrappable<::DerivedFrom<set<int>::const_reverse_iterator>, false>());
STATIC_ASSERT(test_unwrappable<::DerivedFrom<multiset<int>::iterator>, false>());
STATIC_ASSERT(test_unwrappable<::DerivedFrom<multiset<int>::const_iterator>, false>());
STATIC_ASSERT(test_unwrappable<::DerivedFrom<multiset<int>::reverse_iterator>, false>());
STATIC_ASSERT(test_unwrappable<::DerivedFrom<multiset<int>::const_reverse_iterator>, false>());

STATIC_ASSERT(test_unwrappable<::DerivedFrom<unordered_map<int, int>::iterator>, false>());
STATIC_ASSERT(test_unwrappable<::DerivedFrom<unordered_map<int, int>::const_iterator>, false>());
STATIC_ASSERT(test_unwrappable<::DerivedFrom<unordered_multimap<int, int>::iterator>, false>());
STATIC_ASSERT(test_unwrappable<::DerivedFrom<unordered_multimap<int, int>::const_iterator>, false>());
STATIC_ASSERT(test_unwrappable<::DerivedFrom<unordered_set<int>::iterator>, false>());
STATIC_ASSERT(test_unwrappable<::DerivedFrom<unordered_set<int>::const_iterator>, false>());
STATIC_ASSERT(test_unwrappable<::DerivedFrom<unordered_multiset<int>::iterator>, false>());
STATIC_ASSERT(test_unwrappable<::DerivedFrom<unordered_multiset<int>::const_iterator>, false>());

STATIC_ASSERT(test_unwrappable<::DerivedFrom<move_iterator<int*>>, false>());
STATIC_ASSERT(test_unwrappable<::DerivedFrom<reverse_iterator<int*>>, false>());

#if _HAS_CXX17
STATIC_ASSERT(test_unwrappable<::DerivedFrom<string_view::iterator>, false>());
STATIC_ASSERT(test_unwrappable<::DerivedFrom<string_view::const_iterator>, false>());
STATIC_ASSERT(test_unwrappable<::DerivedFrom<string_view::reverse_iterator>, false>());
STATIC_ASSERT(test_unwrappable<::DerivedFrom<string_view::const_reverse_iterator>, false>());

STATIC_ASSERT(test_unwrappable<::DerivedFrom<std::filesystem::path::iterator>, false>());
STATIC_ASSERT(test_unwrappable<::DerivedFrom<std::filesystem::path::const_iterator>, false>());
#endif // _HAS_CXX17

#if _HAS_CXX20
STATIC_ASSERT(test_unwrappable<::DerivedFrom<span<int>::iterator>, false>());
STATIC_ASSERT(test_unwrappable<::DerivedFrom<span<int>::reverse_iterator>, false>());
#endif // _HAS_CXX20

STATIC_ASSERT(test_unwrappable<::DerivedFrom<stdext::checked_array_iterator<int*>>, false>());
STATIC_ASSERT(test_unwrappable<::DerivedFrom<stdext::unchecked_array_iterator<int*>>, false>());

STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<string::iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<string::const_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<string::reverse_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<string::const_reverse_iterator>, false>());

STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<array<int, 2>::iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<array<int, 2>::const_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<array<int, 2>::reverse_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<array<int, 2>::const_reverse_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<deque<int>::iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<deque<int>::const_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<deque<int>::reverse_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<deque<int>::const_reverse_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<forward_list<int>::iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<forward_list<int>::const_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<list<int>::iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<list<int>::const_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<list<int>::reverse_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<list<int>::const_reverse_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<vector<int>::iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<vector<int>::const_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<vector<int>::reverse_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<vector<int>::const_reverse_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<vector<bool>::iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<vector<bool>::const_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<vector<bool>::reverse_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<vector<bool>::const_reverse_iterator>, false>());

STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<map<int, int>::iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<map<int, int>::const_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<map<int, int>::reverse_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<map<int, int>::const_reverse_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<multimap<int, int>::iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<multimap<int, int>::const_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<multimap<int, int>::reverse_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<multimap<int, int>::const_reverse_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<set<int>::iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<set<int>::const_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<set<int>::reverse_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<set<int>::const_reverse_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<multiset<int>::iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<multiset<int>::const_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<multiset<int>::reverse_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<multiset<int>::const_reverse_iterator>, false>());

STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<unordered_map<int, int>::iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<unordered_map<int, int>::const_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<unordered_multimap<int, int>::iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<unordered_multimap<int, int>::const_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<unordered_set<int>::iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<unordered_set<int>::const_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<unordered_multiset<int>::iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<unordered_multiset<int>::const_iterator>, false>());

STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<move_iterator<int*>>, false>());
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<reverse_iterator<int*>>, false>());

#if _HAS_CXX17
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<string_view::iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<string_view::const_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<string_view::reverse_iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<string_view::const_reverse_iterator>, false>());

STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<std::filesystem::path::iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<std::filesystem::path::const_iterator>, false>());
#endif // _HAS_CXX17

#if _HAS_CXX20
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<span<int>::iterator>, false>());
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<span<int>::reverse_iterator>, false>());
#endif // _HAS_CXX20

STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<stdext::checked_array_iterator<int*>>, false>());
STATIC_ASSERT(test_unwrappable_for_offset<::DerivedFrom<stdext::unchecked_array_iterator<int*>>, false>());
