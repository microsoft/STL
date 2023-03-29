// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cstdlib>
#include <deque>
#include <forward_list>
#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// Regression test for DevDiv-483851 : [C++11] STL containers must use std::allocator_traits in debug mode

template <typename T, typename Base>
struct simple_allocator : Base {
    typedef T value_type;

    simple_allocator() {}

    template <typename U, typename UB>
    simple_allocator(simple_allocator<U, UB> const&) {}

    value_type* allocate(std::size_t n) {
        return static_cast<value_type*>(malloc(n * sizeof(value_type)));
    }

    void deallocate(value_type* p, std::size_t) {
        free(p);
    }
};

template <typename T, typename TB, typename U, typename UB>
bool operator==(simple_allocator<T, TB> const&, simple_allocator<U, UB> const&) {
    return true;
}

template <typename T, typename TB, typename U, typename UB>
bool operator!=(simple_allocator<T, TB> const&, simple_allocator<U, UB> const&) {
    return false;
}

struct empty_base {};
struct nonempty_base {
    int x;
};

// Ensure that we can instantiate all of the containers with this minimal allocator:
namespace std {
    // deque
    template class deque<int, simple_allocator<int, empty_base>>;
    template class deque<int, simple_allocator<int, nonempty_base>>;

    // forward_list
    template class forward_list<int, simple_allocator<int, empty_base>>;
    template class forward_list<int, simple_allocator<int, nonempty_base>>;

    // list
    template class list<int, simple_allocator<int, empty_base>>;
    template class list<int, simple_allocator<int, nonempty_base>>;

    // map
    template class map<int, int, less<int>, simple_allocator<pair<int const, int>, empty_base>>;
    template class map<int, int, less<int>, simple_allocator<pair<int const, int>, nonempty_base>>;

    // multimap
    template class multimap<int, int, less<int>, simple_allocator<pair<int const, int>, empty_base>>;
    template class multimap<int, int, less<int>, simple_allocator<pair<int const, int>, nonempty_base>>;

    // multiset
    template class multiset<int, less<int>, simple_allocator<int, empty_base>>;
    template class multiset<int, less<int>, simple_allocator<int, nonempty_base>>;

    // set
    template class set<int, less<int>, simple_allocator<int, empty_base>>;
    template class set<int, less<int>, simple_allocator<int, nonempty_base>>;

    // unordered_map
    template class unordered_map<int, int, hash<int>, equal_to<int>,
        simple_allocator<pair<int const, int>, empty_base>>;
    template class unordered_map<int, int, hash<int>, equal_to<int>,
        simple_allocator<pair<int const, int>, nonempty_base>>;

    // unordered_multimap
    template class unordered_multimap<int, int, hash<int>, equal_to<int>,
        simple_allocator<pair<int const, int>, empty_base>>;
    template class unordered_multimap<int, int, hash<int>, equal_to<int>,
        simple_allocator<pair<int const, int>, nonempty_base>>;

    // unordered_multiset
    template class unordered_multiset<int, hash<int>, equal_to<int>, simple_allocator<int, empty_base>>;
    template class unordered_multiset<int, hash<int>, equal_to<int>, simple_allocator<int, nonempty_base>>;

    // unordered_set
    template class unordered_set<int, hash<int>, equal_to<int>, simple_allocator<int, empty_base>>;
    template class unordered_set<int, hash<int>, equal_to<int>, simple_allocator<int, nonempty_base>>;

    // vector
    template class vector<int, simple_allocator<int, empty_base>>;
    template class vector<int, simple_allocator<int, nonempty_base>>;

    // vector<bool>
    template class vector<bool, simple_allocator<bool, empty_base>>;
    template class vector<bool, simple_allocator<bool, nonempty_base>>;
} // namespace std
