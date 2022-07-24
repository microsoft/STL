// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _SILENCE_CXX23_ALIGNED_UNION_DEPRECATION_WARNING
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

#include <array>
#include <cassert>
#include <cstring>
#include <deque>
#include <experimental/filesystem>
#include <filesystem>
#include <forward_list>
#include <iterator>
#include <list>
#include <map>
#include <new>
#include <regex>
#include <set>
#include <span>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

// N3797 24.2.5 [forward.iterators]/2:
// "The domain of == for forward iterators is that of iterators over the same
// underlying sequence. However, value-initialized iterators may be compared and
// shall compare equal to other value-initialized iterators of the same type."

template <typename Depend>
struct always_false : false_type {};

// Note we take the iterators under test by const& to avoid invoking copy constructors
// which may invalidate the "placement new on top of garbage" test case.
template <typename FwdIt>
void test_iterator_impl(const FwdIt& lhs, const FwdIt& rhs, input_iterator_tag) {
    // while most iterators in this test need to be forward iterators, path::iterator meets most
    // of the forward iterator requirements (including those that we test here), but not all of them,
    // so it's marked with input_iterator_tag
    assert(lhs == rhs);
    assert(!(lhs != rhs));
}

template <typename RanIt>
void test_iterator_impl(const RanIt& lhs, const RanIt& rhs, random_access_iterator_tag) {

    assert(lhs == rhs);
    assert(!(lhs != rhs));

    // See N4567 24.2.7 [random.access.iterators]/1 (table 110)
    RanIt lhsCopy(lhs);
    assert(lhsCopy == lhs);
    assert(lhs == lhsCopy);
    // r += n
    lhsCopy += 0;
    assert(lhsCopy == lhs);
    assert(lhs == lhsCopy);
    // n + a
    assert(0 + lhs == lhs);
    // a + n
    assert(lhs + 0 == lhs);
    // r -= n
    lhsCopy -= 0;
    assert(lhsCopy == lhs);
    assert(lhs == lhsCopy);
    // a - n
    assert(lhs - 0 == lhs);
    // b - a
    assert(lhs - rhs == 0);
    // a < b
    assert(!(lhs < rhs));
    // a > b
    assert(!(lhs > rhs));
    // a <= b
    assert(lhs <= rhs);
    // a >= b
    assert(lhs >= rhs);
}

template <typename FwdIt>
void test_iterator() {
    typedef typename iterator_traits<FwdIt>::iterator_category Cat;

    {
        const FwdIt i1{};
        const FwdIt i2{};

        test_iterator_impl(i1, i2, Cat{});
    }

    {
        aligned_union_t<0, FwdIt> au3;
        aligned_union_t<0, FwdIt> au4;

        FwdIt* p3 = reinterpret_cast<FwdIt*>(&au3);
        FwdIt* p4 = reinterpret_cast<FwdIt*>(&au4);

        memset(p3, 0xCC, sizeof(FwdIt));
        memset(p4, 0xDD, sizeof(FwdIt));

        new (p3) FwdIt{};
        new (p4) FwdIt{};

        test_iterator_impl(*p3, *p4, Cat{});

        p3->~FwdIt();
        p4->~FwdIt();
    }
}

template <typename Container>
void test_container() {
    using Iter      = typename Container::iterator;
    using ConstIter = typename Container::const_iterator;

    test_iterator<Iter>();
    test_iterator<ConstIter>();

    // Also test how container destruction orphans iterators.
    // VSO-845592 "Debug crash when destroying iterator to deque.end()
    // gotten through deque.erase(it) after deque has been destroyed"

    Iter i;
    ConstIter ci;

    {
        Container container;
        i  = container.end();
        ci = container.cend();
    }

    // i and ci have been orphaned, but can be re-assigned.

    i  = {};
    ci = {};

    assert(i == Iter{});
    assert(ci == ConstIter{});
}

int main() {
    test_container<array<int, 5>>();
    test_container<deque<int>>();
    test_container<forward_list<int>>();
    test_container<list<int>>();
    test_container<vector<int>>();
    test_container<vector<bool>>();

    test_container<map<int, int>>();
    test_container<multimap<int, int>>();
    test_container<set<int>>();
    test_container<multiset<int>>();

    test_container<unordered_map<int, int>>();
    test_container<unordered_multimap<int, int>>();
    test_container<unordered_set<int>>();
    test_container<unordered_multiset<int>>();

    test_container<string>();
#if _HAS_CXX17
    test_iterator<string_view::iterator>();
#endif // _HAS_CXX17

#if _HAS_CXX20
    test_iterator<span<int>::iterator>();
#endif // _HAS_CXX20

    test_iterator<sregex_iterator>();
    test_iterator<sregex_token_iterator>();

    // N3797 24.5.1.3.1 [reverse.iter.cons]/1:
    // "reverse_iterator(); Effects: Value initializes current."
    test_iterator<reverse_iterator<int*>>();

    // N3797 24.5.3.3.1 [move.iter.op.const]/1:
    // "move_iterator(); Effects: Constructs a move_iterator, value initializing current."
    test_iterator<move_iterator<int*>>();

    test_iterator<std::experimental::filesystem::path::iterator>();
#if _HAS_CXX17
    test_iterator<std::filesystem::path::iterator>();
#endif // _HAS_CXX17

    test_iterator<stdext::checked_array_iterator<int*>>();
    test_iterator<stdext::unchecked_array_iterator<int*>>();

    {
        // Also test string iterator subtraction.

        string s("cute fluffy kittens");

        const string::iterator i = s.begin() + 5;
        const string::iterator j = s.begin() + 12;

        assert(i - j == -7);
        assert(j - i == 7);

        const string::const_iterator ci = s.cbegin();
        const string::const_iterator cj = s.cbegin() + 8;

        assert(ci - cj == -8);
        assert(cj - ci == 8);

        assert(i - ci == 5);
        assert(ci - i == -5);
    }
}
