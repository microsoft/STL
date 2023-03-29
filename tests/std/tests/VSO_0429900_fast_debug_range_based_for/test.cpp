// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS
#include <array>
#include <cassert>
#include <deque>
#include <forward_list>
#include <hash_map>
#include <hash_set>
#include <list>
#include <map>
#include <memory>
#include <regex>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

template <typename T>
void assert_same_address(const T& x, const T& y) {
    assert(addressof(x) == addressof(y));
}

template <typename Range>
void assert_range_for_impl(Range& c) {
    auto b = c.begin();
    for (auto&& val : c) {
        STATIC_ASSERT(is_same_v<remove_reference_t<decltype(val)>, remove_reference_t<decltype(*b)>>);
        assert(b != c.end());
        if (is_same_v<remove_const_t<Range>, vector<bool>>) {
            assert(*b == val);
        } else {
            assert_same_address(*b, val);
        }

        ++b;
    }

    assert(b == c.end());
}

template <typename Range>
void assert_range_for(Range& c) {
    assert_range_for_impl(c);
    assert_range_for_impl(static_cast<const Range&>(c));
}

array<char, 0> emptyArray;
array<char, 2> elementsArray{{42, 67}};

template <typename Container>
struct DerivedEvilContainer : Container {
    mutable bool beginCalled = false;
    mutable bool endCalled   = false;

    auto begin() const {
        beginCalled = true;
        return Container::begin();
    }

    auto end() const {
        endCalled = true;
        return Container::end();
    }
};

template <typename Container>
struct StlLikeContainer : Container {
    mutable bool beginCalled = false;
    mutable bool endCalled   = false;

    using Container::_Unchecked_begin;
    using Container::_Unchecked_end;

    auto begin() const {
        beginCalled = true;
        return Container::begin();
    }

    auto end() const {
        endCalled = true;
        return Container::end();
    }
};

template <template <class...> class Container>
void test_case_sequence_container() {
    Container<char> testContainer;
    assert_range_for(testContainer);
    testContainer.assign(elementsArray.begin(), elementsArray.end());
    assert_range_for(testContainer);


    DerivedEvilContainer<Container<char>> dec;
    dec.assign(elementsArray.begin(), elementsArray.end());
    for (auto&& x : dec) {
        (void) x;
    }
    assert(dec.beginCalled);
    assert(dec.endCalled);

#if !defined(__EDG__) && !defined(__clang__)
    StlLikeContainer<Container<char>> stlLike;
    stlLike.assign(elementsArray.begin(), elementsArray.end());
    for (auto&& x : stlLike) {
        (void) x;
    }
    assert(!stlLike.beginCalled);
    assert(!stlLike.endCalled);
#endif // !defined(__EDG__) && !defined(__clang__)
}

template <template <class...> class Container>
void test_case_set_container() {
    Container<char> testContainer;
    assert_range_for(testContainer);
    testContainer.insert(elementsArray[0]);
    testContainer.insert(elementsArray[1]);
    assert_range_for(testContainer);
}

template <template <class...> class Container>
void test_case_map_container() {
    Container<int, int> testContainer;
    assert_range_for(testContainer);
    testContainer.emplace(42, 1729);
    testContainer.emplace(2048, 1000);
    assert_range_for(testContainer);
}

int main() {
    assert_range_for(emptyArray);
    assert_range_for(elementsArray);

    test_case_sequence_container<basic_string>();
    test_case_sequence_container<deque>();
    test_case_sequence_container<forward_list>();
    test_case_sequence_container<list>();
    test_case_sequence_container<vector>();

    vector<bool> vb;
    assert_range_for(vb);
    vb.push_back(true);
    vb.push_back(false);
    assert_range_for(vb);

    test_case_set_container<hash_multiset>();
    test_case_set_container<hash_set>();
    test_case_set_container<multiset>();
    test_case_set_container<set>();
    test_case_set_container<unordered_multiset>();
    test_case_set_container<unordered_set>();

    test_case_map_container<hash_map>();
    test_case_map_container<hash_multimap>();
    test_case_map_container<map>();
    test_case_map_container<multimap>();
    test_case_map_container<unordered_map>();
    test_case_map_container<unordered_multimap>();

    cmatch matchResults;
    assert_range_for(matchResults);
    const regex reg("(abc)(abc)");
    assert(regex_match("abcabc", matchResults, reg));
    assert(matchResults.size() == 3);
    assert_range_for(matchResults);

#if _HAS_CXX17
    string_view emptySv;
    assert_range_for(emptySv);
    string_view contentSv("hungry EVIL zombies");
    assert_range_for(contentSv);
#endif // _HAS_CXX17
}
