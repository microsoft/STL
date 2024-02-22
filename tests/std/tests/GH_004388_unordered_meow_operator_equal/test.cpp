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

namespace modulo {
    // Functors for comparison and hashing
    // based on modular arithmetic equivalence classes
    template <int N>
    struct compare {
        bool operator()(int lhs, int rhs) const {
            return (lhs % N) < (rhs % N);
        }
    };

    template <int N>
    struct equals {
        bool operator()(int lhs, int rhs) const {
            return (lhs % N) == (rhs % N);
        }
    };

    template <int N>
    struct hash {
        size_t operator()(int value) const {
            return static_cast<size_t>(value % N);
        }
    };
} // namespace modulo

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
        const auto& key = get_key(*it);
        auto l_range    = lhs.equal_range(key);
        auto r_range    = rhs.equal_range(key);

        if (!is_permutation(l_range.first, l_range.second, r_range.first, r_range.second)) {
            return false;
        }
        it = l_range.second;
    }
    return true;
}

template <typename Map>
bool test_maps() {
    // In all cases, these sets should compare not equal, even though their elements are equivalent
    Map map1 = {{1, 1}, {2, 2}};
    Map map2 = {{21, 1}, {12, 2}};

    return (map1 == map2) == std_equal(map1, map2);
}

template <typename Set>
bool test_sets() {
    // In all cases, these sets should compare not equal, even though their elements are equivalent
    Set set1 = {1, 2};
    Set set2 = {21, 12};

    return (set1 == set2) == std_equal(set1, set2);
}

// GH-4388: <unordered_set>, <unordered_map>: operator== is incorrect with custom equivalence functor
void test_gh_4388() {
    using Set               = set<int, modulo::compare<10>>;
    using MultiSet          = multiset<int, modulo::compare<10>>;
    using UnorderedSet      = unordered_set<int, modulo::hash<10>, modulo::equals<10>>;
    using UnorderedMultiSet = unordered_multiset<int, modulo::hash<10>, modulo::equals<10>>;

    assert(test_sets<Set>());
    assert(test_sets<MultiSet>());
    assert(test_sets<UnorderedSet>());
    assert(test_sets<UnorderedMultiSet>());

    using Map               = map<int, int, modulo::compare<10>>;
    using MultiMap          = multimap<int, int, modulo::compare<10>>;
    using UnorderedMap      = unordered_map<int, int, modulo::hash<10>, modulo::equals<10>>;
    using UnorderedMultiMap = unordered_multimap<int, int, modulo::hash<10>, modulo::equals<10>>;

    assert(test_maps<Map>());
    assert(test_maps<MultiMap>());
    assert(test_maps<UnorderedMap>());
    assert(test_maps<UnorderedMultiMap>());
}

int main() {
    test_gh_4388();
}
