// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <inplace_vector>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>

using namespace std;

static constexpr int input[] = {0, 1, 2, 3, 4, 5};

template <typename T>
using vector = inplace_vector<T, 42>;

using vec = vector<int>;

constexpr bool test_interface() {
    { // constructors

        // Non allocator constructors
        vec size_default_constructed(5);
        assert(size_default_constructed.size() == 5);
        assert(all_of(
            size_default_constructed.begin(), size_default_constructed.end(), [](const int val) { return val == 0; }));

        vec size_value_constructed(5, 7);
        assert(size_value_constructed.size() == 5);
        assert(all_of(
            size_value_constructed.begin(), size_value_constructed.end(), [](const int val) { return val == 7; }));

        vec range_constructed(begin(input), end(input));
        assert(equal(range_constructed.begin(), range_constructed.end(), begin(input), end(input)));

        vec initializer_list_constructed({2, 3, 4, 5});
        assert(equal(
            initializer_list_constructed.begin(), initializer_list_constructed.end(), begin(input) + 2, end(input)));

        // special member functions
        vec default_constructed;
        assert(default_constructed.empty());
        vec copy_constructed(size_default_constructed);
        assert(equal(copy_constructed.begin(), copy_constructed.end(), size_default_constructed.begin(),
            size_default_constructed.end()));

        vec move_constructed(move(copy_constructed));
        assert(equal(move_constructed.begin(), move_constructed.end(), size_default_constructed.begin(),
            size_default_constructed.end()));

        vec copy_assigned = range_constructed;
        assert(equal(copy_assigned.begin(), copy_assigned.end(), range_constructed.begin(), range_constructed.end()));

        vec move_assigned = move(copy_assigned);
        assert(equal(move_assigned.begin(), move_assigned.end(), range_constructed.begin(), range_constructed.end()));
    }

    { // assignment
        vec range_constructed(begin(input), end(input));

        vec copy_constructed;
        copy_constructed = range_constructed;
        assert(equal(
            copy_constructed.begin(), copy_constructed.end(), range_constructed.begin(), range_constructed.end()));

        vec move_constructed;
        move_constructed = move(copy_constructed);
        assert(equal(
            move_constructed.begin(), move_constructed.end(), range_constructed.begin(), range_constructed.end()));

        vec initializer_list_constructed;
        initializer_list_constructed = {0, 1, 2, 3, 4, 5};
        assert(
            equal(initializer_list_constructed.begin(), initializer_list_constructed.end(), begin(input), end(input)));

        vec assigned;
        constexpr int expected_assign_value[] = {4, 4, 4, 4, 4};
        assigned.assign(5, 4);
        assert(equal(assigned.begin(), assigned.end(), begin(expected_assign_value), end(expected_assign_value)));

        assigned.assign(begin(input), end(input));
        assert(equal(assigned.begin(), assigned.end(), begin(input), end(input)));

        constexpr int expected_assign_initializer[] = {2, 3, 4, 5};
        assigned.assign({2, 3, 4, 5});
        assert(equal(
            assigned.begin(), assigned.end(), begin(expected_assign_initializer), end(expected_assign_initializer)));
    }

    { // iterators
        vec range_constructed(begin(input), end(input));
        const vec const_range_constructed(begin(input), end(input));

        const auto b = range_constructed.begin();
        static_assert(is_same_v<remove_const_t<decltype(b)>, vec::iterator>);
        assert(*b == 0);

        const auto cb = range_constructed.cbegin();
        static_assert(is_same_v<remove_const_t<decltype(cb)>, vec::const_iterator>);
        assert(*cb == 0);

        const auto cb2 = const_range_constructed.begin();
        static_assert(is_same_v<remove_const_t<decltype(cb2)>, vec::const_iterator>);
        assert(*cb2 == 0);

        const auto e = range_constructed.end();
        static_assert(is_same_v<remove_const_t<decltype(e)>, vec::iterator>);
        assert(*prev(e) == 5);

        const auto ce = range_constructed.cend();
        static_assert(is_same_v<remove_const_t<decltype(ce)>, vec::const_iterator>);
        assert(*prev(ce) == 5);

        const auto ce2 = const_range_constructed.end();
        static_assert(is_same_v<remove_const_t<decltype(ce2)>, vec::const_iterator>);
        assert(*prev(ce2) == 5);

        const auto rb = range_constructed.rbegin();
        static_assert(is_same_v<remove_const_t<decltype(rb)>, reverse_iterator<vec::iterator>>);
        assert(*rb == 5);

        const auto crb = range_constructed.crbegin();
        static_assert(is_same_v<remove_const_t<decltype(crb)>, reverse_iterator<vec::const_iterator>>);
        assert(*crb == 5);

        const auto crb2 = const_range_constructed.rbegin();
        static_assert(is_same_v<remove_const_t<decltype(crb2)>, reverse_iterator<vec::const_iterator>>);
        assert(*crb2 == 5);

        const auto re = range_constructed.rend();
        static_assert(is_same_v<remove_const_t<decltype(rb)>, reverse_iterator<vec::iterator>>);
        assert(*prev(re) == 0);

        const auto cre = range_constructed.crend();
        static_assert(is_same_v<remove_const_t<decltype(cre)>, reverse_iterator<vec::const_iterator>>);
        assert(*prev(cre) == 0);

        const auto cre2 = const_range_constructed.rend();
        static_assert(is_same_v<remove_const_t<decltype(cre2)>, reverse_iterator<vec::const_iterator>>);
        assert(*prev(cre2) == 0);
    }

    { // access
        vec range_constructed(begin(input), end(input));
        const vec const_range_constructed(begin(input), end(input));

        const auto at = range_constructed.at(2);
        static_assert(is_same_v<remove_const_t<decltype(at)>, int>);
        assert(at == 2);

        range_constructed.at(2) = 3;

        const auto at2 = range_constructed.at(2);
        static_assert(is_same_v<remove_const_t<decltype(at2)>, int>);
        assert(at2 == 3);

        const auto cat = const_range_constructed.at(2);
        static_assert(is_same_v<remove_const_t<decltype(cat)>, int>);
        assert(cat == 2);

        const auto op = range_constructed[3];
        static_assert(is_same_v<remove_const_t<decltype(op)>, int>);
        assert(op == 3);

        range_constructed[3] = 4;
        const auto op2       = range_constructed[3];
        static_assert(is_same_v<remove_const_t<decltype(op2)>, int>);
        assert(op2 == 4);

        const auto cop = const_range_constructed[3];
        static_assert(is_same_v<remove_const_t<decltype(cop)>, int>);
        assert(cop == 3);

        const auto f = range_constructed.front();
        static_assert(is_same_v<remove_const_t<decltype(f)>, int>);
        assert(f == 0);

        const auto cf = const_range_constructed.front();
        static_assert(is_same_v<remove_const_t<decltype(cf)>, int>);
        assert(cf == 0);

        const auto b = range_constructed.back();
        static_assert(is_same_v<remove_const_t<decltype(b)>, int>);
        assert(b == 5);

        const auto cb = const_range_constructed.back();
        static_assert(is_same_v<remove_const_t<decltype(cb)>, int>);
        assert(cb == 5);

        const auto d = range_constructed.data();
        static_assert(is_same_v<remove_const_t<decltype(d)>, int*>);
        assert(*d == 0);

        const auto cd = const_range_constructed.data();
        static_assert(is_same_v<remove_const_t<decltype(cd)>, const int*>);
        assert(*cd == 0);
    }

    { // capacity
        vec range_constructed(begin(input), end(input));

        const auto e = range_constructed.empty();
        static_assert(is_same_v<remove_const_t<decltype(e)>, bool>);
        assert(!e);

        const auto s = range_constructed.size();
        static_assert(is_same_v<remove_const_t<decltype(s)>, size_t>);
        assert(s == size(input));

        const auto ms = range_constructed.max_size();
        static_assert(is_same_v<remove_const_t<decltype(ms)>, size_t>);

        range_constructed.reserve(20);

        const auto c = range_constructed.capacity();
        static_assert(is_same_v<remove_const_t<decltype(c)>, size_t>);
        assert(c >= 20);

        range_constructed.shrink_to_fit();

        const auto c2 = range_constructed.capacity();
        static_assert(is_same_v<remove_const_t<decltype(c2)>, size_t>);
        assert(c2 >= 6);
    }

    { // modifiers
        vec range_constructed(begin(input), end(input));

        vec cleared = range_constructed;
        cleared.clear();
        assert(cleared.empty());
        assert(cleared.capacity() == range_constructed.capacity());

        vec inserted;

        const int to_be_inserted = 3;
        inserted.insert(inserted.begin(), to_be_inserted);
        assert(inserted.size() == 1);
        assert(inserted.front() == 3);

        const int to_be_inserted2 = 4;
        inserted.insert(inserted.cbegin(), to_be_inserted2);
        assert(inserted.size() == 2);
        assert(inserted.front() == 4);

        inserted.insert(inserted.begin(), 1);
        assert(inserted.size() == 3);
        assert(inserted.front() == 1);

        inserted.insert(inserted.cbegin(), 2);
        assert(inserted.size() == 4);
        assert(inserted.front() == 2);

        const auto it = inserted.insert(inserted.begin(), begin(input), end(input));
        assert(inserted.size() == 10);
        assert(it == inserted.begin());

        const auto it2 = inserted.insert(inserted.cbegin(), begin(input), end(input));
        assert(inserted.size() == 16);
        assert(it2 == inserted.begin());

        const auto it3 = inserted.insert(inserted.begin(), {2, 3, 4});
        assert(inserted.size() == 19);
        assert(it3 == inserted.begin());

        inserted.insert(inserted.cbegin(), {2, 3, 4});
        assert(inserted.size() == 22);

        inserted.insert(inserted.begin(), 4, 11);
        assert(inserted.size() == 26);

        vec emplaced;
        emplaced.emplace(emplaced.cbegin(), 42);
        assert(emplaced.size() == 1);
        assert(emplaced.front() == 42);

        emplaced.emplace_back(43);
        assert(emplaced.size() == 2);
        assert(emplaced.back() == 43);

        emplaced.push_back(44);
        assert(emplaced.size() == 3);
        assert(emplaced.back() == 44);

        const int to_be_pushed = 45;
        emplaced.push_back(to_be_pushed);
        assert(emplaced.size() == 4);
        assert(emplaced.back() == 45);

        emplaced.pop_back();
        assert(emplaced.size() == 3);
        assert(emplaced.back() == 44);

        emplaced.resize(1);
        assert(emplaced.size() == 1);
        assert(emplaced.front() == 42);

        emplaced.swap(inserted);
        assert(inserted.size() == 1);
        assert(inserted.front() == 42);
        assert(emplaced.size() == 26);

        emplaced.erase(emplaced.end() - 1);
        assert(emplaced.size() == 25);

        emplaced.erase(emplaced.begin(), emplaced.begin() + 2);
        assert(emplaced.size() == 23);

        emplaced.emplace(emplaced.cbegin(), 42);
        assert(emplaced.size() == 24);
        assert(emplaced.front() == 42);
    }

    { // swap
        vec first{2, 3, 4};
        vec second{5, 6, 7, 8};
        swap(first, second);

        constexpr int expected_first[]  = {5, 6, 7, 8};
        constexpr int expected_second[] = {2, 3, 4};
        assert(equal(first.begin(), first.end(), begin(expected_first), end(expected_first)));
        assert(equal(second.begin(), second.end(), begin(expected_second), end(expected_second)));
    }

    { // erase
        vec erased{1, 2, 3, 4, 2, 3, 2};
        erase(erased, 2);
        constexpr int expected_erased[] = {1, 3, 4, 3};
        assert(equal(erased.begin(), erased.end(), begin(expected_erased), end(expected_erased)));

        erase_if(erased, [](const int val) { return val < 4; });
        constexpr int expected_erase_if[] = {4};
        assert(equal(erased.begin(), erased.end(), begin(expected_erase_if), end(expected_erase_if)));
    }

    { // comparison
        vec first(begin(input), end(input));
        vec second(begin(input), end(input));
        vec third{2, 3, 4};

        const auto e = first == second;
        static_assert(is_same_v<remove_const_t<decltype(e)>, bool>);
        assert(e);

        const auto ne = first != third;
        static_assert(is_same_v<remove_const_t<decltype(ne)>, bool>);
        assert(ne);

        const auto l = first < third;
        static_assert(is_same_v<remove_const_t<decltype(l)>, bool>);
        assert(l);

        const auto le = first <= third;
        static_assert(is_same_v<remove_const_t<decltype(le)>, bool>);
        assert(le);

        const auto g = first > third;
        static_assert(is_same_v<remove_const_t<decltype(g)>, bool>);
        assert(!g);

        const auto ge = first >= third;
        static_assert(is_same_v<remove_const_t<decltype(ge)>, bool>);
        assert(!ge);
    }

    return true;
}

constexpr bool test_iterators() {
    vec range_constructed(begin(input), end(input));

    { // increment
        auto it = range_constructed.begin();
        assert(*++it == 1);
        assert(*it++ == 1);
        assert(*it == 2);

        auto cit = range_constructed.cbegin();
        assert(*++cit == 1);
        assert(*cit++ == 1);
        assert(*cit == 2);
    }

    { // advance
        auto it = range_constructed.begin() + 2;
        assert(*it == 2);
        it += 2;
        assert(*it == 4);

        auto cit = range_constructed.cbegin() + 2;
        assert(*cit == 2);
        cit += 2;
        assert(*cit == 4);
    }

    { // decrement
        auto it = range_constructed.end();
        assert(*--it == 5);
        assert(*it-- == 5);
        assert(*it == 4);

        auto cit = range_constructed.cend();
        assert(*--cit == 5);
        assert(*cit-- == 5);
        assert(*cit == 4);
    }

    { // advance back
        auto it = range_constructed.end() - 2;
        assert(*it == 4);
        it -= 2;
        assert(*it == 2);

        auto cit = range_constructed.cend() - 2;
        assert(*cit == 4);
        cit -= 2;
        assert(*cit == 2);
    }

    { // difference
        const auto it1 = range_constructed.begin();
        const auto it2 = range_constructed.end();
        assert(it2 - it1 == ssize(input));

        const auto cit1 = range_constructed.cbegin();
        const auto cit2 = range_constructed.cend();
        assert(cit2 - cit1 == ssize(input));

        assert(it2 - cit1 == ssize(input));
        assert(cit2 - it1 == ssize(input));
    }

    { // comparison
        const auto it1 = range_constructed.begin();
        const auto it2 = range_constructed.begin();
        const auto it3 = range_constructed.end();

        assert(it1 == it2);
        assert(it1 != it3);
        assert(it1 < it3);
        assert(it1 <= it3);
        assert(it3 > it1);
        assert(it3 >= it1);
    }

    { // access
        const auto it = range_constructed.begin() + 2;
        it[2]         = 3;
        assert(range_constructed[4] == 3);

        const auto cit = range_constructed.cbegin() + 2;
        assert(cit[2] == 3);

        vector<pair<int, int>> vec2 = {{1, 2}, {2, 3}};
        const auto it2              = vec2.begin();
        assert(it2->second == 2);

        const auto cit2 = vec2.cbegin();
        assert(cit2->first == 1);
    }

    return true;
}

int main() {
    test_interface();
    test_iterators();
    // static_assert(test_interface());
    // static_assert(test_iterators());
}
