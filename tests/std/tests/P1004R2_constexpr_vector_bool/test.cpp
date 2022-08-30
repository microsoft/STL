// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <limits>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

using namespace std;

static constexpr bool input[]         = {true, false, true, true, false, true};
static constexpr bool input_flipped[] = {false, true, false, false, true, false};

struct demoterator { // demote pointer to input iterator
    using iterator_category = input_iterator_tag;
    using value_type        = int;
    using difference_type   = ptrdiff_t;
    using reference         = const int&;
    using pointer           = void;

    constexpr bool operator==(const demoterator& that) const {
        return ptr == that.ptr;
    }

    constexpr reference operator*() const {
        return *ptr;
    }

    constexpr demoterator& operator++() {
        ++ptr;
        return *this;
    }
    constexpr void operator++(int) {
        ++*this;
    }

    const int* ptr;
};

// Just long enough to force a reallocation when inserting
static constexpr int num_arr[33] = { //
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, //
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, //
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, //
    0, 1, 2};

template <typename T>
struct soccc_allocator {
    using value_type = T;

    constexpr soccc_allocator() noexcept = default;
    constexpr explicit soccc_allocator(const int id_) noexcept : id(id_), soccc_generation(0) {}
    constexpr explicit soccc_allocator(const int id_, const int soccc_generation_) noexcept
        : id(id_), soccc_generation(soccc_generation_) {}
    template <typename U>
    constexpr soccc_allocator(const soccc_allocator<U>& other) noexcept
        : id(other.id), soccc_generation(other.soccc_generation) {}
    constexpr soccc_allocator(const soccc_allocator& other) noexcept
        : id(other.id + 1), soccc_generation(other.soccc_generation) {}

    constexpr soccc_allocator& operator=(const soccc_allocator&) noexcept {
        return *this;
    }

    constexpr soccc_allocator select_on_container_copy_construction() const noexcept {
        return soccc_allocator(id, soccc_generation + 1);
    }

    template <typename U>
    constexpr bool operator==(const soccc_allocator<U>&) const noexcept {
        return true;
    }

    constexpr T* allocate(const size_t n) {
        return allocator<T>{}.allocate(n);
    }

    constexpr void deallocate(T* const p, const size_t n) noexcept {
        allocator<T>{}.deallocate(p, n);
    }

    template <class... Args>
    constexpr void construct(T* const p, Args&&... args) {
        construct_at(p, forward<Args>(args)...);
    }

    int id               = 0;
    int soccc_generation = 0;
};

using vec = vector<bool, soccc_allocator<bool>>;

constexpr bool test_interface() {
    { // constructors

// Non allocator constructors
#ifndef __EDG__ // TRANSITION, VSO-1274387
        vec size_default_constructed(5);
        assert(size_default_constructed.size() == 5);
#if !defined(__EDG__) || _ITERATOR_DEBUG_LEVEL != 2 // TRANSITION, VSO-1273296
        assert(all_of(
            size_default_constructed.begin(), size_default_constructed.end(), [](const bool val) { return !val; }));
#endif // !defined(__EDG__) || _ITERATOR_DEBUG_LEVEL != 2
#endif // __EDG__

#ifndef __EDG__ // TRANSITION, VSO-1274387
        vec size_value_constructed(5, true);
        assert(size_value_constructed.size() == 5);
#if !defined(__EDG__) || _ITERATOR_DEBUG_LEVEL != 2 // TRANSITION, VSO-1273296
        assert(
            all_of(size_value_constructed.begin(), size_value_constructed.end(), [](const bool val) { return val; }));
#endif // !defined(__EDG__) || _ITERATOR_DEBUG_LEVEL != 2
#endif // __EDG__

#if !defined(__EDG__) || _ITERATOR_DEBUG_LEVEL != 2 // TRANSITION, VSO-1273365
#ifndef __EDG__ // TRANSITION, VSO-1274387
        vec range_constructed(begin(input), end(input));
        assert(equal(range_constructed.begin(), range_constructed.end(), begin(input), end(input)));

        vec initializer_list_constructed({true, true, false, true});
        assert(equal(
            initializer_list_constructed.begin(), initializer_list_constructed.end(), begin(input) + 2, end(input)));
#endif // __EDG__
#endif // !defined(__EDG__) || _ITERATOR_DEBUG_LEVEL != 2

        // special member functions
#ifndef __EDG__ // TRANSITION, VSO-1274387
        vec default_constructed;
        assert(default_constructed.empty());
        vec copy_constructed(size_default_constructed);
#if !defined(__EDG__) || _ITERATOR_DEBUG_LEVEL != 2 // TRANSITION, VSO-1273296
        assert(equal(copy_constructed.begin(), copy_constructed.end(), size_default_constructed.begin(),
            size_default_constructed.end()));
#endif // !defined(__EDG__) || _ITERATOR_DEBUG_LEVEL != 2

        vec move_constructed(move(copy_constructed));
#if !defined(__EDG__) || _ITERATOR_DEBUG_LEVEL != 2 // TRANSITION, VSO-1273296
        assert(equal(move_constructed.begin(), move_constructed.end(), size_default_constructed.begin(),
            size_default_constructed.end()));
#endif // !defined(__EDG__) || _ITERATOR_DEBUG_LEVEL != 2
        assert(copy_constructed.empty()); // implementation-specific assumption that moved-from is empty
#endif // __EDG__

#if !defined(__EDG__) || _ITERATOR_DEBUG_LEVEL != 2 // TRANSITION, VSO-1273365
#ifndef __EDG__ // TRANSITION, VSO-1274387, VSO-1273296
        vec copy_assigned = range_constructed;
        assert(equal(copy_assigned.begin(), copy_assigned.end(), range_constructed.begin(), range_constructed.end()));

        vec move_assigned = move(copy_assigned);
        assert(equal(move_assigned.begin(), move_assigned.end(), range_constructed.begin(), range_constructed.end()));
        assert(copy_assigned.empty()); // implementation-specific assumption that moved-from is empty
#endif // __EDG__
#endif // !defined(__EDG__) || _ITERATOR_DEBUG_LEVEL != 2

        // allocator constructors
        soccc_allocator<int> alloc(2, 3);
        assert(alloc.id == 2);
        assert(alloc.soccc_generation == 3);

#ifndef __EDG__ // TRANSITION, VSO-1274387
        vec al_default_constructed(alloc);
        assert(al_default_constructed.empty());
        assert(al_default_constructed.get_allocator().id == 4);
        assert(al_default_constructed.get_allocator().soccc_generation == 3);

        vec al_copy_constructed(size_value_constructed, alloc);
#if !defined(__EDG__) || _ITERATOR_DEBUG_LEVEL != 2 // TRANSITION, VSO-1273296
        assert(all_of(al_copy_constructed.begin(), al_copy_constructed.end(), [](const bool val) { return val; }));
#endif // !defined(__EDG__) || _ITERATOR_DEBUG_LEVEL != 2
        assert(al_copy_constructed.get_allocator().id == 4);
        assert(al_copy_constructed.get_allocator().soccc_generation == 3);

        vec al_move_constructed(move(al_copy_constructed), alloc);
#if !defined(__EDG__) || _ITERATOR_DEBUG_LEVEL != 2 // TRANSITION, VSO-1273296
        assert(all_of(al_move_constructed.begin(), al_move_constructed.end(), [](const bool val) { return val; }));
#endif // !defined(__EDG__) || _ITERATOR_DEBUG_LEVEL != 2
        assert(al_copy_constructed.empty()); // implementation-specific assumption that moved-from is empty
        assert(al_move_constructed.get_allocator().id == 4);
        assert(al_move_constructed.get_allocator().soccc_generation == 3);

        vec al_size_default_constructed(5, alloc);
        assert(al_size_default_constructed.size() == 5);
#if !defined(__EDG__) || _ITERATOR_DEBUG_LEVEL != 2 // TRANSITION, VSO-1273296
        assert(all_of(al_size_default_constructed.begin(), al_size_default_constructed.end(),
            [](const bool val) { return !val; }));
#endif // !defined(__EDG__) || _ITERATOR_DEBUG_LEVEL != 2
        assert(al_size_default_constructed.get_allocator().id == 4);
        assert(al_size_default_constructed.get_allocator().soccc_generation == 3);

        vec al_size_value_constructed(5, true, alloc);
        assert(al_size_value_constructed.size() == 5);
#if !defined(__EDG__) || _ITERATOR_DEBUG_LEVEL != 2 // TRANSITION, VSO-1273296
        assert(all_of(
            al_size_value_constructed.begin(), al_size_value_constructed.end(), [](const bool val) { return val; }));
#endif // !defined(__EDG__) || _ITERATOR_DEBUG_LEVEL != 2
        assert(al_size_value_constructed.get_allocator().id == 4);
        assert(al_size_value_constructed.get_allocator().soccc_generation == 3);
#endif // __EDG__

#if !defined(__EDG__) || _ITERATOR_DEBUG_LEVEL != 2 // TRANSITION, VSO-1273365
#ifndef __EDG__ // TRANSITION, VSO-1274387
        vec al_range_constructed(begin(input), end(input), alloc);
        assert(equal(al_range_constructed.begin(), al_range_constructed.end(), begin(input), end(input)));
        assert(al_range_constructed.get_allocator().id == 4);
        assert(al_range_constructed.get_allocator().soccc_generation == 3);
#endif // __EDG__

#ifndef __EDG__ // TRANSITION, VSO-1274387
        vec al_initializer_list_constructed({true, true, false, true}, alloc);
        assert(equal(al_initializer_list_constructed.begin(), al_initializer_list_constructed.end(), begin(input) + 2,
            end(input)));
        assert(al_initializer_list_constructed.get_allocator().id == 4);
        assert(al_initializer_list_constructed.get_allocator().soccc_generation == 3);
#endif // __EDG__
#endif // !defined(__EDG__) || _ITERATOR_DEBUG_LEVEL != 2
    }

    { // assignment
#ifndef __EDG__ // TRANSITION, VSO-1274387
        vec range_constructed(begin(input), end(input));

        vec copy_constructed;
        copy_constructed = range_constructed;
        assert(equal(
            copy_constructed.begin(), copy_constructed.end(), range_constructed.begin(), range_constructed.end()));

        vec move_constructed;
        move_constructed = move(copy_constructed);
        assert(equal(
            move_constructed.begin(), move_constructed.end(), range_constructed.begin(), range_constructed.end()));
        assert(copy_constructed.empty()); // implementation-specific assumption that moved-from is empty

        vec initializer_list_constructed;
        initializer_list_constructed = {true, false, true, true, false, true};
        assert(
            equal(initializer_list_constructed.begin(), initializer_list_constructed.end(), begin(input), end(input)));

        vec assigned;
        constexpr bool expected_assign_value[] = {true, true, true, true, true};
        assigned.assign(5, true);
        assert(equal(assigned.begin(), assigned.end(), begin(expected_assign_value), end(expected_assign_value)));

        assigned.assign(begin(input), end(input));
        assert(equal(assigned.begin(), assigned.end(), begin(input), end(input)));

        constexpr bool expected_assign_initializer[] = {true, false, true, true, false, true};
        assigned.assign({true, false, true, true, false, true});
        assert(equal(
            assigned.begin(), assigned.end(), begin(expected_assign_initializer), end(expected_assign_initializer)));
#endif // __EDG__
    }

    { // allocator
#ifndef __EDG__ // TRANSITION, VSO-1274387
        vec default_constructed;
        const auto alloc = default_constructed.get_allocator();
        static_assert(is_same_v<remove_const_t<decltype(alloc)>, soccc_allocator<bool>>);
        assert(alloc.id == 1);
        assert(alloc.soccc_generation == 0);
#endif // __EDG__
    }

    { // iterators
#ifndef __EDG__ // TRANSITION, VSO-1274387
        vec range_constructed(begin(input), end(input));
        const vec const_range_constructed(begin(input), end(input));

#if !defined(__EDG__) || _ITERATOR_DEBUG_LEVEL != 2 // TRANSITION, VSO-1273381
        const auto b = range_constructed.begin();
        static_assert(is_same_v<remove_const_t<decltype(b)>, vec::iterator>);
        assert(*b);

        const auto cb = range_constructed.cbegin();
        static_assert(is_same_v<remove_const_t<decltype(cb)>, vec::const_iterator>);
        assert(*cb);

        const auto cb2 = const_range_constructed.begin();
        static_assert(is_same_v<remove_const_t<decltype(cb2)>, vec::const_iterator>);
        assert(*cb2);

        const auto e = range_constructed.end();
        static_assert(is_same_v<remove_const_t<decltype(e)>, vec::iterator>);
        assert(*prev(e));

        const auto ce = range_constructed.cend();
        static_assert(is_same_v<remove_const_t<decltype(ce)>, vec::const_iterator>);
        assert(*prev(ce));

        const auto ce2 = const_range_constructed.end();
        static_assert(is_same_v<remove_const_t<decltype(ce2)>, vec::const_iterator>);
        assert(*prev(ce2));

        const auto rb = range_constructed.rbegin();
        static_assert(is_same_v<remove_const_t<decltype(rb)>, reverse_iterator<vec::iterator>>);
        assert(*rb);

        const auto crb = range_constructed.crbegin();
        static_assert(is_same_v<remove_const_t<decltype(crb)>, reverse_iterator<vec::const_iterator>>);
        assert(*crb);

        const auto crb2 = const_range_constructed.rbegin();
        static_assert(is_same_v<remove_const_t<decltype(crb2)>, reverse_iterator<vec::const_iterator>>);
        assert(*crb2);

        const auto re = range_constructed.rend();
        static_assert(is_same_v<remove_const_t<decltype(rb)>, reverse_iterator<vec::iterator>>);
        assert(*prev(re));

        const auto cre = range_constructed.crend();
        static_assert(is_same_v<remove_const_t<decltype(cre)>, reverse_iterator<vec::const_iterator>>);
        assert(*prev(cre));

        const auto cre2 = const_range_constructed.rend();
        static_assert(is_same_v<remove_const_t<decltype(cre2)>, reverse_iterator<vec::const_iterator>>);
        assert(*prev(cre2));
#endif // defined(__EDG__) || _ITERATOR_DEBUG_LEVEL != 2
#endif // __EDG__
    }

    { // access
#ifndef __EDG__ // TRANSITION, VSO-1274387
        vec range_constructed(begin(input), end(input));
        const vec const_range_constructed(begin(input), end(input));

#if !defined(__EDG__) || _ITERATOR_DEBUG_LEVEL != 2 // TRANSITION, VSO-1273381
        const auto at = range_constructed.at(2);
        static_assert(is_same_v<remove_const_t<decltype(at)>, _Iter_ref_t<vec::iterator>>);
        assert(at);

        range_constructed.at(2) = false;

        const auto at2 = range_constructed.at(2);
        static_assert(is_same_v<remove_const_t<decltype(at2)>, _Iter_ref_t<vec::iterator>>);
        assert(at2 == false);

        const auto cat = const_range_constructed.at(2);
        static_assert(is_same_v<remove_const_t<decltype(cat)>, _Iter_ref_t<vec::const_iterator>>);
        assert(cat);

        const auto op = range_constructed[3];
        static_assert(is_same_v<remove_const_t<decltype(op)>, _Iter_ref_t<vec::iterator>>);
        assert(op);

        range_constructed[3] = true;
        const auto op2       = range_constructed[3];
        static_assert(is_same_v<remove_const_t<decltype(op2)>, _Iter_ref_t<vec::iterator>>);
        assert(op2);

        const auto cop = const_range_constructed[3];
        static_assert(is_same_v<remove_const_t<decltype(cop)>, _Iter_ref_t<vec::const_iterator>>);
        assert(cop);

        const auto f = range_constructed.front();
        static_assert(is_same_v<remove_const_t<decltype(f)>, _Iter_ref_t<vec::iterator>>);
        assert(f);

        const auto cf = const_range_constructed.front();
        static_assert(is_same_v<remove_const_t<decltype(cf)>, _Iter_ref_t<vec::const_iterator>>);
        assert(cf);

        const auto b = range_constructed.back();
        static_assert(is_same_v<remove_const_t<decltype(b)>, _Iter_ref_t<vec::iterator>>);
        assert(b);

        const auto cb = const_range_constructed.back();
        static_assert(is_same_v<remove_const_t<decltype(cb)>, _Iter_ref_t<vec::const_iterator>>);
        assert(cb);
#endif // !defined(__EDG__) || _ITERATOR_DEBUG_LEVEL != 2
#endif // __EDG__
    }

    { // capacity
#ifndef __EDG__ // TRANSITION, VSO-1274387
        vec range_constructed(begin(input), end(input));

        const auto e = range_constructed.empty();
        static_assert(is_same_v<remove_const_t<decltype(e)>, bool>);
        assert(e == false);

        const auto s = range_constructed.size();
        static_assert(is_same_v<remove_const_t<decltype(s)>, size_t>);
        assert(s == size(input));

        const auto ms = range_constructed.max_size();
        static_assert(is_same_v<remove_const_t<decltype(ms)>, size_t>);
        assert(ms == static_cast<size_t>(numeric_limits<ptrdiff_t>::max()));

        range_constructed.reserve(20);

        const auto c = range_constructed.capacity();
        static_assert(is_same_v<remove_const_t<decltype(c)>, size_t>);
        assert(c == 32);

        range_constructed.shrink_to_fit();

        const auto c2 = range_constructed.capacity();
        static_assert(is_same_v<remove_const_t<decltype(c2)>, size_t>);
        assert(c2 == 32);
#endif // __EDG__
    }

    { // modifiers
#ifndef __EDG__ // TRANSITION, VSO-1274387
        vec range_constructed(begin(input), end(input));

        vec flipped = range_constructed;
        flipped.flip();
        assert(flipped.size() == 6);
        assert(equal(flipped.begin(), flipped.end(), begin(input_flipped), end(input_flipped)));
        // {true, false, true, true, false, true};

        vec cleared = range_constructed;
        cleared.clear();
        assert(cleared.empty());
        assert(cleared.capacity() == range_constructed.capacity());

        vec inserted;

        const bool to_be_inserted = true;
        inserted.insert(inserted.begin(), to_be_inserted);
        assert(inserted.size() == 1);
        assert(inserted.front());

        const bool to_be_inserted2 = false;
        inserted.insert(inserted.cbegin(), to_be_inserted2);
        assert(inserted.size() == 2);
        assert(inserted.front() == false);

        inserted.insert(inserted.begin(), true);
        assert(inserted.size() == 3);
        assert(inserted.front());

        inserted.insert(inserted.cbegin(), false);
        assert(inserted.size() == 4);
        assert(inserted.front() == false);

#if !defined(__EDG__) || _ITERATOR_DEBUG_LEVEL != 2 // TRANSITION, VSO-1273381
        const auto it = inserted.insert(inserted.begin(), begin(input), end(input));
        assert(inserted.size() == 10);
        assert(it == inserted.begin());

        const auto it2 = inserted.insert(inserted.cbegin(), begin(input), end(input));
        assert(inserted.size() == 16);
        assert(it2 == inserted.begin());

        const auto it3 = inserted.insert(inserted.begin(), {true, false, true});
        assert(inserted.size() == 19);
        assert(it3 == inserted.begin());
#endif // !defined(__EDG__) || _ITERATOR_DEBUG_LEVEL != 2

        inserted.insert(inserted.cbegin(), {false, true, false});
        assert(inserted.size() == 22);

        inserted.insert(inserted.cbegin(), 3, true);
        assert(inserted.size() == 25);

        vec emplaced;
        emplaced.emplace(emplaced.cbegin(), false);
        assert(emplaced.size() == 1);
        assert(emplaced.front() == false);

        emplaced.emplace_back(true);
        assert(emplaced.size() == 2);
        assert(emplaced.back());

        emplaced.push_back(false);
        assert(emplaced.size() == 3);
        assert(emplaced.back() == false);

        const bool to_be_pushed = true;
        emplaced.push_back(to_be_pushed);
        assert(emplaced.size() == 4);
        assert(emplaced.back());

        emplaced.pop_back();
        assert(emplaced.size() == 3);
        assert(emplaced.back() == false);

        emplaced.resize(1);
        assert(emplaced.size() == 1);
        assert(emplaced.front() == false);

        emplaced.swap(inserted);
        assert(inserted.size() == 1);
        assert(inserted.front() == false);
        assert(emplaced.size() == 25);

        emplaced.erase(emplaced.end() - 1);
        assert(emplaced.size() == 24);

        emplaced.erase(emplaced.begin(), emplaced.begin() + 2);
        assert(emplaced.size() == 22);

        {
            // GH-2440: we were incorrectly reallocating _before_ orphaning iterators
            // (resulting in UB) while inserting ranges of unknown length

            vector<bool> input_inserted;
            const auto result =
                input_inserted.insert(input_inserted.end(), demoterator{begin(num_arr)}, demoterator{end(num_arr)});
            static_assert(is_same_v<decltype(result), const vector<bool>::iterator>);
            assert(result == input_inserted.begin());
            assert(input_inserted.size() == size(num_arr));
        }
#endif // __EDG__
    }

    { // swap
#ifndef __EDG__ // TRANSITION, VSO-1274387
        vec first{true, false, true};
        vec second{false, false, true, false};
        swap(first, second);

        constexpr bool expected_first[]  = {false, false, true, false};
        constexpr bool expected_second[] = {true, false, true};
        assert(equal(first.begin(), first.end(), begin(expected_first), end(expected_first)));
        assert(equal(second.begin(), second.end(), begin(expected_second), end(expected_second)));
#endif // __EDG__
    }

    { // erase
#ifndef __EDG__ // TRANSITION, VSO-1274387
        vec erased{false, false, true, false, true};
        erase(erased, false);
        constexpr bool expected_erased[] = {true, true};
        assert(equal(erased.begin(), erased.end(), begin(expected_erased), end(expected_erased)));

        vec erased_if{false, false, true, false, true};
        erase_if(erased_if, [](const bool val) { return val; });
        constexpr bool expected_erase_if[] = {false, false, false};
        assert(equal(erased_if.begin(), erased_if.end(), begin(expected_erase_if), end(expected_erase_if)));
#endif // __EDG__
    }

    { // comparison
#ifndef __EDG__ // TRANSITION, VSO-1274387
        vec first(begin(input), end(input));
        vec second(begin(input), end(input));
        vec third{true, false, true};

        const auto e = first == second;
        static_assert(is_same_v<remove_const_t<decltype(e)>, bool>);
        assert(e);

        const auto ne = first != third;
        static_assert(is_same_v<remove_const_t<decltype(ne)>, bool>);
        assert(ne);

        const auto l = first < third;
        static_assert(is_same_v<remove_const_t<decltype(l)>, bool>);
        assert(!l);

        const auto le = first <= third;
        static_assert(is_same_v<remove_const_t<decltype(le)>, bool>);
        assert(!le);

        const auto g = first > third;
        static_assert(is_same_v<remove_const_t<decltype(g)>, bool>);
        assert(g);

        const auto ge = first >= third;
        static_assert(is_same_v<remove_const_t<decltype(ge)>, bool>);
        assert(ge);
#endif // __EDG__
    }

    return true;
}

constexpr bool test_iterators() {
#ifndef __EDG__ // TRANSITION, VSO-1274387
    vec range_constructed(begin(input), end(input));

#if !defined(__EDG__) || _ITERATOR_DEBUG_LEVEL != 2 // TRANSITION, VSO-1273381
    { // increment
        auto it = range_constructed.begin();
        assert(*++it == false);
        assert(*it++ == false);
        assert(*it);

        auto cit = range_constructed.cbegin();
        assert(*++cit == false);
        assert(*cit++ == false);
        assert(*cit);
    }

    { // advance
        auto it = range_constructed.begin() + 2;
        assert(*it);
        it += 2;
        assert(*it == false);

        auto cit = range_constructed.cbegin() + 2;
        assert(*cit);
        cit += 2;
        assert(*cit == false);
    }

    { // decrement
        auto it = range_constructed.end();
        assert(*--it);
        assert(*it--);
        assert(*it == false);

        auto cit = range_constructed.cend();
        assert(*--cit);
        assert(*cit--);
        assert(*cit == false);
    }

    { // advance back
        auto it = range_constructed.end() - 2;
        assert(*it == false);
        it -= 2;
        assert(*it);

        auto cit = range_constructed.cend() - 2;
        assert(*cit == false);
        cit -= 2;
        assert(*cit);
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
        it[2]         = false;
        assert(range_constructed[4] == false);

        const auto cit = range_constructed.cbegin() + 2;
        assert(cit[2] == false);
    }
#endif // __EDG__
#endif // !defined(__EDG__) || _ITERATOR_DEBUG_LEVEL != 2

    return true;
}

int main() {
    test_interface();
    test_iterators();
    static_assert(test_interface());
    static_assert(test_iterators());
}
