// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <utility>

using namespace std;

// Functors for comparison and hashing
// based on modular arithmetic equivalence classes
template <int N>
struct ModLess {
    bool operator()(int lhs, int rhs) const {
        return (lhs % N) < (rhs % N);
    }
};

template <int N>
struct ModEqual {
    bool operator()(int lhs, int rhs) const {
        return (lhs % N) == (rhs % N);
    }
};

template <int N>
struct ModHash {
    size_t operator()(int value) const {
        return static_cast<size_t>(value % N);
    }
};

// Overloaded function to get a key from a set / map's value_type
const int& get_key(const int& value) {
    return value;
}
const int& get_key(const pair<const int, int>& p) {
    return p.first;
}

// Equality comparison for unordered sets and maps as per the C++ standard
// It also gives the correct result for associative containers (though is sub-optimal)
template <typename Container>
bool std_equal(const Container& lhs, const Container& rhs) {
    if (lhs.size() != rhs.size()) {
        return false;
    }

    for (auto it = lhs.cbegin(); it != lhs.cend();) {
        const auto& key    = get_key(*it);
        const auto l_range = lhs.equal_range(key);
        const auto r_range = rhs.equal_range(key);

        if (!is_permutation(l_range.first, l_range.second, r_range.first, r_range.second)) {
            return false;
        }
        it = l_range.second;
    }
    return true;
}

template <typename Map>
void test_maps() {
    // These maps should compare not equal, even though their elements are equivalent
    Map map1 = {{1, 1}, {2, 2}};
    Map map2 = {{21, 1}, {12, 2}};

    assert(map1 != map2);
    assert(!std_equal(map1, map2));

    // Test a case that should compare equal
    Map map3 = {{12, 2}, {21, 1}};
    assert(map2 == map3);
    assert(std_equal(map2, map3));
}

template <typename Set>
void test_sets() {
    // These sets should compare not equal, even though their elements are equivalent
    Set set1 = {1, 2};
    Set set2 = {21, 12};

    assert(set1 != set2);
    assert(!std_equal(set1, set2));

    // Test a case that should compare equal
    Set set3 = {12, 21};
    assert(set2 == set3);
    assert(std_equal(set2, set3));
}

// GH-4388: <unordered_set>, <unordered_map>: operator== is incorrect with custom equivalence functor
void test_gh_4388() {
    using Set               = set<int, ModLess<10>>;
    using MultiSet          = multiset<int, ModLess<10>>;
    using UnorderedSet      = unordered_set<int, ModHash<10>, ModEqual<10>>;
    using UnorderedMultiSet = unordered_multiset<int, ModHash<10>, ModEqual<10>>;

    test_sets<Set>();
    test_sets<MultiSet>();
    test_sets<UnorderedSet>();
    test_sets<UnorderedMultiSet>();

    using Map               = map<int, int, ModLess<10>>;
    using MultiMap          = multimap<int, int, ModLess<10>>;
    using UnorderedMap      = unordered_map<int, int, ModHash<10>, ModEqual<10>>;
    using UnorderedMultiMap = unordered_multimap<int, int, ModHash<10>, ModEqual<10>>;

    test_maps<Map>();
    test_maps<MultiMap>();
    test_maps<UnorderedMap>();
    test_maps<UnorderedMultiMap>();
}

int main() {
    test_gh_4388();
}
