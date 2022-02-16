// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <assert.h>
#include <cstddef>
#include <iterator>
#include <memory>
#include <numeric>
#include <type_traits>
#include <utility>
#include <vector>

using namespace std;

static constexpr int input[] = {0, 1, 2, 3, 4, 5};

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

using vec = vector<int, soccc_allocator<int>>;

class input_iterator_tester {
private:
    static constexpr int size = 6;
    int data[size]            = {};

public:
    constexpr input_iterator_tester() noexcept {
        iota(data, data + size, 0);
    }

    class iterator {
    private:
        int* curr;

    public:
        using iterator_category = input_iterator_tag;
        using value_type        = int;
        using difference_type   = ptrdiff_t;
        using pointer           = void;
        using reference         = int&;

        constexpr explicit iterator(int* start) : curr(start) {}

        constexpr reference operator*() const {
            return *curr;
        }

        constexpr iterator& operator++() {
            ++curr;
            return *this;
        }

        constexpr iterator operator++(int) {
            auto tmp = *this;
            ++curr;
            return tmp;
        }

        constexpr bool operator==(const iterator& that) const {
            return curr == that.curr;
        }

        constexpr bool operator!=(const iterator& that) const {
            return !(*this == that);
        }
    };

    constexpr iterator begin() {
        return iterator(data);
    }

    constexpr iterator mid(const int off) {
        return iterator(data + off);
    }

    constexpr iterator end() {
        return iterator(data + size);
    }
};

template <class Range1, class Range2>
[[nodiscard]] constexpr bool Equal(Range1&& r1, Range2&& r2) noexcept {
#if !defined(__EDG__) || _ITERATOR_DEBUG_LEVEL != 2 // TRANSITION, VSO-1273365, VSO-1273386, VSO-1274387
    return equal(begin(r1), end(r1), begin(r2), end(r2));
#else //
    return true;
#endif
}

constexpr bool test_interface() {
    input_iterator_tester input_iter_data;

    { // special member functions
        vec range_constructed(begin(input), end(input));
        assert(Equal(range_constructed, input));

        vec default_constructed;
        assert(default_constructed.empty());

        vec copy_constructed(range_constructed);
        assert(Equal(copy_constructed, range_constructed));

        vec move_constructed(move(copy_constructed));
        assert(Equal(move_constructed, range_constructed));
        assert(copy_constructed.empty()); // implementation-specific assumption that moved-from is empty

        vec copy_assigned = range_constructed;
        assert(Equal(copy_assigned, range_constructed));

        vec move_assigned = move(copy_assigned);
        assert(Equal(move_assigned, range_constructed));
        assert(copy_assigned.empty()); // implementation-specific assumption that moved-from is empty
    }

    { // Non allocator constructor
        vec size_default_constructed(5);
        assert(size_default_constructed.size() == 5);
        assert(all_of(
            size_default_constructed.begin(), size_default_constructed.end(), [](const int val) { return val == 0; }));

        vec size_default_constructed_empty(0);
        assert(size_default_constructed_empty.empty());

        vec size_value_constructed(5, 7);
        assert(size_value_constructed.size() == 5);
        assert(all_of(
            size_value_constructed.begin(), size_value_constructed.end(), [](const int val) { return val == 7; }));

        vec size_value_constructed_empty(0, 7);
        assert(size_value_constructed_empty.empty());

        vec range_constructed(begin(input), end(input));
        assert(Equal(range_constructed, input));

        vec range_constructed_empty(begin(input), begin(input));
        assert(range_constructed_empty.empty());

        vec input_range_constructed(begin(input_iter_data), end(input_iter_data));
        assert(Equal(range_constructed, input));

        vec input_range_constructed_empty(begin(input_iter_data), begin(input_iter_data));
        assert(input_range_constructed_empty.empty());

        vec initializer_list_constructed({0, 1, 2, 3, 4, 5});
        assert(Equal(initializer_list_constructed, input));

        vec initializer_list_constructed_empty({});
        assert(initializer_list_constructed_empty.empty());
    }

    { // allocator constructors
        soccc_allocator<int> alloc(2, 3);
        assert(alloc.id == 2);
        assert(alloc.soccc_generation == 3);

        vec al_range_constructed(begin(input), end(input), alloc);
        assert(Equal(al_range_constructed, input));
        assert(al_range_constructed.get_allocator().id == 4);
        assert(al_range_constructed.get_allocator().soccc_generation == 3);

        vec al_default_constructed(alloc);
        assert(al_default_constructed.empty());
        assert(al_default_constructed.get_allocator().id == 4);
        assert(al_default_constructed.get_allocator().soccc_generation == 3);

        vec al_copy_constructed(al_range_constructed, alloc);
        assert(Equal(al_copy_constructed, input));
        assert(al_copy_constructed.get_allocator().id == 4);
        assert(al_copy_constructed.get_allocator().soccc_generation == 3);

        vec al_move_constructed(move(al_copy_constructed), alloc);
        assert(Equal(al_move_constructed, input));
        assert(al_copy_constructed.empty()); // implementation-specific assumption that moved-from is empty
        assert(al_move_constructed.get_allocator().id == 4);
        assert(al_move_constructed.get_allocator().soccc_generation == 3);

        vec al_size_default_constructed(5, alloc);
        assert(al_size_default_constructed.size() == 5);
        assert(all_of(al_size_default_constructed.begin(), al_size_default_constructed.end(),
            [](const int val) { return val == 0; }));
        assert(al_size_default_constructed.get_allocator().id == 4);
        assert(al_size_default_constructed.get_allocator().soccc_generation == 3);

        vec al_size_default_constructed_empty(0, alloc);
        assert(al_size_default_constructed_empty.empty());
        assert(al_size_default_constructed_empty.get_allocator().id == 4);
        assert(al_size_default_constructed_empty.get_allocator().soccc_generation == 3);

        vec al_size_value_constructed(5, 7, alloc);
        assert(al_size_value_constructed.size() == 5);
        assert(all_of(al_size_value_constructed.begin(), al_size_value_constructed.end(),
            [](const int val) { return val == 7; }));
        assert(al_size_value_constructed.get_allocator().id == 4);
        assert(al_size_value_constructed.get_allocator().soccc_generation == 3);

        vec al_size_value_constructed_empty(0, 7, alloc);
        assert(al_size_value_constructed_empty.empty());
        assert(al_size_value_constructed_empty.get_allocator().id == 4);
        assert(al_size_value_constructed_empty.get_allocator().soccc_generation == 3);

        vec al_input_range_constructed(begin(input_iter_data), end(input_iter_data), alloc);
        assert(Equal(al_input_range_constructed, input_iter_data));
        assert(al_input_range_constructed.get_allocator().id == 4);
        assert(al_input_range_constructed.get_allocator().soccc_generation == 3);

        vec al_input_range_constructed_empty(begin(input_iter_data), begin(input_iter_data), alloc);
        assert(al_input_range_constructed_empty.empty());
        assert(al_input_range_constructed_empty.get_allocator().id == 4);
        assert(al_input_range_constructed_empty.get_allocator().soccc_generation == 3);

        vec al_initializer_list_constructed({0, 1, 2, 3, 4, 5}, alloc);
        assert(Equal(al_initializer_list_constructed, input));
        assert(al_initializer_list_constructed.get_allocator().id == 4);
        assert(al_initializer_list_constructed.get_allocator().soccc_generation == 3);

        vec al_initializer_list_constructed_empty({}, alloc);
        assert(al_initializer_list_constructed_empty.empty());
        assert(al_initializer_list_constructed_empty.get_allocator().id == 4);
        assert(al_initializer_list_constructed_empty.get_allocator().soccc_generation == 3);
    }

    { // assignment
        vec default_constructed;
        vec range_constructed(begin(input), end(input));

        vec copy_assigned;
        copy_assigned = range_constructed;
        assert(Equal(copy_assigned, range_constructed));

        vec copy_assigned_empty;
#if !defined(__EDG__) || _ITERATOR_DEBUG_LEVEL != 2 // TRANSITION, VSO-1273365, VSO-1273386, VSO-1274387
        copy_assigned_empty = default_constructed;
        assert(copy_assigned_empty.empty());
#endif

        vec move_assigned;
#if !defined(__EDG__) || _ITERATOR_DEBUG_LEVEL != 2 // TRANSITION, VSO-1273365, VSO-1273386, VSO-1274387
        move_assigned = move(copy_assigned);
        assert(Equal(move_assigned, range_constructed));
        assert(copy_assigned.empty()); // implementation-specific assumption that moved-from is empty
#endif

        vec move_assigned_empty;
        move_assigned_empty = move(copy_assigned_empty);
        assert(move_assigned_empty.empty());
        assert(copy_assigned_empty.empty()); // implementation-specific assumption that moved-from is empty

        vec initializer_list_assigned;
        initializer_list_assigned = {0, 1, 2, 3, 4, 5};
        assert(Equal(initializer_list_assigned, input));

        vec initializer_list_assigned_empty;
        initializer_list_assigned_empty = {};
        assert(initializer_list_assigned_empty.empty());
    }

    { // assign
        vec assign_value_size_grow;
        constexpr int expected_assign_value[] = {4, 4, 4, 4, 4};
        assign_value_size_grow.assign(5, 4);
        assert(Equal(assign_value_size_grow, expected_assign_value));

        vec assign_value_size_shrink(8, 3);
        assign_value_size_shrink.assign(5, 4);
        assert(Equal(assign_value_size_shrink, expected_assign_value));

        vec assign_value_size_empty(8, 3);
        assign_value_size_empty.assign(0, 4);
        assert(assign_value_size_empty.empty());

        vec assign_range_grow(1, 2);
        assign_range_grow.assign(begin(input), end(input));
        assert(Equal(assign_range_grow, input));

        vec assign_range_shrink(10, 2);
        assign_range_shrink.assign(begin(input), end(input));
        assert(Equal(assign_range_shrink, input));

        vec assign_range_empty(10, 2);
        assign_range_empty.assign(begin(input), begin(input));
        assert(assign_range_empty.empty());

        vec assign_input_range_grow(1, 2);
        assign_input_range_grow.assign(begin(input_iter_data), end(input_iter_data));
        assert(Equal(assign_input_range_grow, input_iter_data));

        vec assign_input_range_shrink(15, 2);
        assign_input_range_shrink.assign(begin(input_iter_data), end(input_iter_data));
        assert(Equal(assign_input_range_shrink, input_iter_data));

        vec assign_input_range_empty(15, 2);
        assign_input_range_empty.assign(begin(input_iter_data), begin(input_iter_data));
        assert(assign_input_range_empty.empty());

        vec assign_initializer_grow(2, 3);
        assign_initializer_grow.assign({0, 1, 2, 3, 4, 5});
        assert(Equal(assign_initializer_grow, input));

        vec assign_initializer_shrink(12, 3);
        assign_initializer_shrink.assign({0, 1, 2, 3, 4, 5});
        assert(Equal(assign_initializer_shrink, input));

        vec assign_initializer_empty(12, 3);
        assign_initializer_shrink.assign({});
        assert(assign_initializer_shrink.empty());
    }

    { // allocator
        vec default_constructed;
        const auto alloc = default_constructed.get_allocator();
        static_assert(is_same_v<remove_const_t<decltype(alloc)>, soccc_allocator<int>>);
        assert(alloc.id == 1);
        assert(alloc.soccc_generation == 0);
    }

#if !defined(__EDG__) || _ITERATOR_DEBUG_LEVEL != 2 // TRANSITION, VSO-1273365, VSO-1273386, VSO-1274387
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
#endif // !defined(__EDG__) || _ITERATOR_DEBUG_LEVEL != 2 // TRANSITION, VSO-1273365, VSO-1273386, VSO-1274387

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
        assert(ms == static_cast<size_t>(-1) / sizeof(int));

        range_constructed.reserve(20);

        const auto c = range_constructed.capacity();
        static_assert(is_same_v<remove_const_t<decltype(c)>, size_t>);
        assert(c == 20);

        range_constructed.shrink_to_fit();

        const auto c2 = range_constructed.capacity();
        static_assert(is_same_v<remove_const_t<decltype(c2)>, size_t>);
        assert(c2 == 6);
    }

    { // clear
        vec cleared = {1, 2, 3, 4, 5};
        cleared.clear();
        assert(cleared.empty());

        vec cleared_empty;
        cleared_empty.clear();
        assert(cleared_empty.empty());
    }

#if !defined(__EDG__) || _ITERATOR_DEBUG_LEVEL != 2 // TRANSITION, VSO-1273365, VSO-1273386, VSO-1274387
    { // modifiers
        // insert value
        const int to_be_inserted = 0;
        vec insert_front_lvalue{1, 2, 3, 4, 5};
        insert_front_lvalue.reserve(10);
        insert_front_lvalue.insert(insert_front_lvalue.begin(), to_be_inserted);
        assert(Equal(insert_front_lvalue, input));

        vec insert_front_grow_lvalue{1, 2, 3, 4, 5};
        insert_front_grow_lvalue.insert(insert_front_grow_lvalue.begin(), to_be_inserted);
        assert(Equal(insert_front_grow_lvalue, input));

        vec insert_front_rvalue{1, 2, 3, 4, 5};
        insert_front_rvalue.reserve(10);
        insert_front_rvalue.insert(insert_front_rvalue.begin(), 0);
        assert(Equal(insert_front_rvalue, input));

        vec insert_front_grow_rvalue{1, 2, 3, 4, 5};
        insert_front_grow_rvalue.insert(insert_front_grow_rvalue.begin(), 0);
        assert(Equal(insert_front_grow_rvalue, input));

        const int to_be_inserted_mid = 2;
        vec insert_mid_lvalue{0, 1, 3, 4, 5};
        insert_mid_lvalue.reserve(10);
        insert_mid_lvalue.insert(insert_mid_lvalue.begin() + 2, to_be_inserted_mid);
        assert(Equal(insert_mid_lvalue, input));

        vec insert_mid_grow_lvalue{0, 1, 3, 4, 5};
        insert_mid_grow_lvalue.insert(insert_mid_grow_lvalue.begin() + 2, to_be_inserted_mid);
        assert(Equal(insert_mid_grow_lvalue, input));

        vec insert_mid_rvalue{0, 1, 3, 4, 5};
        insert_mid_rvalue.reserve(10);
        insert_mid_rvalue.insert(insert_mid_rvalue.begin() + 2, 2);
        assert(Equal(insert_mid_rvalue, input));

        vec insert_mid_grow_rvalue{0, 1, 3, 4, 5};
        insert_mid_grow_rvalue.insert(insert_mid_grow_rvalue.begin() + 2, 2);
        assert(Equal(insert_mid_grow_rvalue, input));

        const int to_be_inserted_end = 5;
        vec insert_end_lvalue{0, 1, 2, 3, 4};
        insert_end_lvalue.reserve(10);
        insert_end_lvalue.insert(insert_end_lvalue.end(), to_be_inserted_end);
        assert(Equal(insert_end_lvalue, input));

        vec insert_end_grow_lvalue{0, 1, 2, 3, 4};
        insert_end_grow_lvalue.insert(insert_end_grow_lvalue.end(), to_be_inserted_end);
        assert(Equal(insert_end_grow_lvalue, input));

        vec insert_end_rvalue{0, 1, 2, 3, 4};
        insert_end_rvalue.reserve(10);
        insert_end_rvalue.insert(insert_end_rvalue.end(), 5);
        assert(Equal(insert_end_rvalue, input));

        vec insert_end_grow_rvalue{0, 1, 2, 3, 4};
        insert_end_grow_rvalue.insert(insert_end_grow_rvalue.end(), 5);
        assert(Equal(insert_end_grow_rvalue, input));

        // insert range
        vec insert_front_range{4, 5};
        insert_front_range.reserve(10);
        insert_front_range.insert(insert_front_range.begin(), begin(input), begin(input) + 4);
        assert(Equal(insert_front_range, input));

        vec insert_front_grow_range{4, 5};
        insert_front_grow_range.insert(insert_front_grow_range.begin(), begin(input), begin(input) + 4);
        assert(Equal(insert_front_grow_range, input));

        vec insert_front_input_range{4, 5};
        insert_front_input_range.reserve(10);
        insert_front_input_range.insert(
            insert_front_input_range.begin(), begin(input_iter_data), input_iter_data.mid(4));
        assert(Equal(insert_front_input_range, input));

        vec insert_front_grow_input_range{4, 5};
        insert_front_grow_input_range.insert(
            insert_front_grow_input_range.begin(), begin(input_iter_data), input_iter_data.mid(4));
        assert(Equal(insert_front_grow_input_range, input));

        vec insert_mid_range{0, 1, 5};
        insert_mid_range.reserve(10);
        insert_mid_range.insert(insert_mid_range.begin() + 2, begin(input) + 2, begin(input) + 5);
        assert(Equal(insert_mid_range, input));

        vec insert_mid_grow_range{0, 1, 5};
        insert_mid_grow_range.insert(insert_mid_grow_range.begin() + 2, begin(input) + 2, begin(input) + 5);
        assert(Equal(insert_mid_grow_range, input));

        vec insert_mid_input_range{0, 1, 5};
        insert_mid_input_range.reserve(10);
        insert_mid_input_range.insert(
            insert_mid_input_range.begin() + 2, input_iter_data.mid(2), input_iter_data.mid(5));
        assert(Equal(insert_mid_input_range, input));

        vec insert_mid_grow_input_range{0, 1, 5};
        insert_mid_grow_input_range.insert(
            insert_mid_grow_input_range.begin() + 2, input_iter_data.mid(2), input_iter_data.mid(5));
        assert(Equal(insert_mid_grow_input_range, input));

        vec insert_end_range{0, 1};
        insert_end_range.reserve(10);
        insert_end_range.insert(insert_end_range.end(), begin(input) + 2, end(input));
        assert(Equal(insert_end_range, input));

        vec insert_end_grow_range{0, 1};
        insert_end_grow_range.insert(insert_end_grow_range.end(), begin(input) + 2, end(input));
        assert(Equal(insert_end_grow_range, input));

        vec insert_end_input_range{0, 1};
        insert_end_input_range.reserve(10);
        insert_end_input_range.insert(insert_end_input_range.end(), input_iter_data.mid(2), input_iter_data.end());
        assert(Equal(insert_end_input_range, input));

        vec insert_end_grow_input_range{0, 1};
        insert_end_grow_input_range.insert(
            insert_end_grow_input_range.end(), input_iter_data.mid(2), input_iter_data.end());
        assert(Equal(insert_end_grow_input_range, input));

        // insert initializer
        vec insert_front_initializer{4, 5};
        insert_front_initializer.reserve(10);
        insert_front_initializer.insert(insert_front_initializer.begin(), {0, 1, 2, 3});
        assert(Equal(insert_front_initializer, input));

        vec insert_front_grow_initializer{4, 5};
        insert_front_grow_initializer.insert(insert_front_grow_initializer.begin(), {0, 1, 2, 3});
        assert(Equal(insert_front_grow_initializer, input));

        vec insert_mid_initializer{0, 1, 5};
        insert_mid_initializer.reserve(10);
        insert_mid_initializer.insert(insert_mid_initializer.begin() + 2, {2, 3, 4});
        assert(Equal(insert_mid_initializer, input));

        vec insert_mid_grow_initializer{0, 1, 5};
        insert_mid_grow_initializer.insert(insert_mid_grow_initializer.begin() + 2, {2, 3, 4});
        assert(Equal(insert_mid_grow_initializer, input));

        vec insert_end_initializer{0, 1};
        insert_end_initializer.reserve(10);
        insert_end_initializer.insert(insert_end_initializer.end(), {2, 3, 4, 5});
        assert(Equal(insert_end_initializer, input));

        vec insert_end_grow_initializer{0, 1};
        insert_end_grow_initializer.insert(insert_end_grow_initializer.end(), {2, 3, 4, 5});
        assert(Equal(insert_end_grow_initializer, input));
    }
#endif // !defined(__EDG__) || _ITERATOR_DEBUG_LEVEL != 2 // TRANSITION, VSO-1273365, VSO-1273386, VSO-1274387

#if !defined(__EDG__) || _ITERATOR_DEBUG_LEVEL != 2 // TRANSITION, VSO-1273365, VSO-1273386, VSO-1274387
    { // emplace
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

        emplaced.resize(5);
        assert(emplaced.size() == 5);
        assert(emplaced.front() == 42);

        emplaced.erase(emplaced.end() - 1);
        assert(emplaced.size() == 4);

        emplaced.erase(emplaced.begin(), emplaced.begin() + 2);
        assert(emplaced.size() == 2);

        emplaced.emplace(emplaced.cbegin(), 42);
        assert(emplaced.size() == 3);
        assert(emplaced.front() == 42);
    }
#endif // !defined(__EDG__) || _ITERATOR_DEBUG_LEVEL != 2 // TRANSITION, VSO-1273365, VSO-1273386, VSO-1274387

    { // swap
        vec first{2, 3, 4};
        vec second{5, 6, 7, 8};
        swap(first, second);

        constexpr int expected_first[]  = {5, 6, 7, 8};
        constexpr int expected_second[] = {2, 3, 4};
        assert(Equal(first, expected_first));
        assert(Equal(second, expected_second));
    }

#if !defined(__EDG__) || _ITERATOR_DEBUG_LEVEL != 2 // TRANSITION, VSO-1273365, VSO-1273386, VSO-1274387
    { // erase
        vec erased{1, 2, 3, 4, 2, 3, 2};
        erase(erased, 2);
        constexpr int expected_erased[] = {1, 3, 4, 3};
        assert(Equal(erased, expected_erased));

        erase_if(erased, [](const int val) { return val < 4; });
        constexpr int expected_erase_if[] = {4};
        assert(Equal(erased, expected_erase_if));
    }
#endif // !defined(__EDG__) || _ITERATOR_DEBUG_LEVEL != 2 // TRANSITION, VSO-1273365, VSO-1273386, VSO-1274387

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

constexpr bool test_growth() {
    {
        vector<int> v(1000, 1729);

        assert(v.size() == 1000);
        assert(v.capacity() == 1000);

        v.resize(1003);

        assert(v.size() == 1003);
        assert(v.capacity() == 1500);
    }

    {
        vector<int> v(1000, 1729);

        assert(v.size() == 1000);
        assert(v.capacity() == 1000);

        v.resize(8000);

        assert(v.size() == 8000);
        assert(v.capacity() == 8000);
    }

    {
        vector<int> v(1000, 1729);

        assert(v.size() == 1000);
        assert(v.capacity() == 1000);

        v.push_back(47);

        assert(v.size() == 1001);
        assert(v.capacity() == 1500);
    }

    {
        vector<int> v(1000, 1729);

        assert(v.size() == 1000);
        assert(v.capacity() == 1000);

#if !defined(__EDG__) || _ITERATOR_DEBUG_LEVEL != 2 // TRANSITION, VSO-1273365, VSO-1273386, VSO-1274387
        vector<int> l(3, 47);
        v.insert(v.end(), l.begin(), l.end());

        assert(v.size() == 1003);
        assert(v.capacity() == 1500);
#endif // !defined(__EDG__) || _ITERATOR_DEBUG_LEVEL != 2
    }

    {
        vector<int> v(1000, 1729);

        assert(v.size() == 1000);
        assert(v.capacity() == 1000);

#if !defined(__EDG__) || _ITERATOR_DEBUG_LEVEL != 2 // TRANSITION, VSO-1273365, VSO-1273386, VSO-1274387
        vector<int> l(7000, 47);
        v.insert(v.end(), l.begin(), l.end());

        assert(v.size() == 8000);
        assert(v.capacity() == 8000);
#endif // !defined(__EDG__) || _ITERATOR_DEBUG_LEVEL != 2
    }

    {
        vector<int> v(1000, 1729);

        assert(v.size() == 1000);
        assert(v.capacity() == 1000);

#if !defined(__EDG__) || _ITERATOR_DEBUG_LEVEL != 2 // TRANSITION, VSO-1273365, VSO-1273386, VSO-1274387
        v.insert(v.end(), 3, 47);

        assert(v.size() == 1003);
        assert(v.capacity() == 1500);
#endif // !defined(__EDG__) || _ITERATOR_DEBUG_LEVEL != 2
    }

    {
        vector<int> v(1000, 1729);

        assert(v.size() == 1000);
        assert(v.capacity() == 1000);

#if !defined(__EDG__) || _ITERATOR_DEBUG_LEVEL != 2 // TRANSITION, VSO-1273365, VSO-1273386, VSO-1274387
        v.insert(v.end(), 7000, 47);

        assert(v.size() == 8000);
        assert(v.capacity() == 8000);
#endif // !defined(__EDG__) || _ITERATOR_DEBUG_LEVEL != 2
    }

    return true;
}

int main() {
    test_interface();
    test_iterators();
    test_growth();

    static_assert(test_interface());
#if !defined(__EDG__) || _ITERATOR_DEBUG_LEVEL != 2 // TRANSITION, VSO-1273365, VSO-1273386, VSO-1274387
    static_assert(test_iterators());
#endif
    static_assert(test_growth());
}
