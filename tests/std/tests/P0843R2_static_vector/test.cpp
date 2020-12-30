// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <assert.h>
#include <memory>
#include <static_vector>
#include <utility>

#include <range_algorithm_support.hpp>
using namespace std;

static constexpr int input[]            = {0, 1, 2, 3, 4, 5};
static constexpr int short_input[]      = {1, 2, 3, 4};
static constexpr size_t vector_capacity = 6;
using vec                               = static_vector<int, vector_capacity>;

static_assert(sizeof(vec) == sizeof(size_t) + 6 * sizeof(int));

constexpr bool test_interface() {
    static_assert(ranges::contiguous_range<vec>);
    using ranges::all_of, ranges::begin, ranges::end, ranges::equal;
    { // constructors
        // default range construct
        vec construct_range(input);
        assert(equal(construct_range, input));

        // special member functions
        vec default_construct;
        assert(default_construct.empty());

        vec copy_construct(construct_range);
        assert(equal(copy_construct, input));

        vec move_construct(move(copy_construct));
        assert(equal(move_construct, input));
        assert(move_construct.size() == copy_construct.size());

        vec copy_assignment_grow{3, 4};
        copy_assignment_grow = construct_range;
        assert(equal(copy_assignment_grow, input));

        vec move_assignment_grow{3, 4};
        move_assignment_grow = move(copy_assignment_grow);
        assert(equal(move_assignment_grow, input));
        assert(move_assignment_grow.size() == copy_assignment_grow.size());

        vec copy_assignment_shrink{6, 4};
        vec short_range{short_input};
        copy_assignment_shrink = short_range;
        assert(equal(copy_assignment_shrink, short_input));

        vec move_assignment_shrink{6, 4};
        move_assignment_shrink = move(copy_assignment_shrink);
        assert(equal(move_assignment_shrink, short_input));
        assert(move_assignment_shrink.size() == copy_assignment_shrink.size());

        // other constructors
        vec construct_size(5);
        assert(construct_size.size() == 5);
        assert(all_of(construct_size, [](const int val) { return val == 0; }));

        vec construct_size_value(5, 7);
        assert(construct_size_value.size() == 5);
        assert(all_of(construct_size_value, [](const int val) { return val == 7; }));

        vec construct_iter_range(begin(input), end(input));
        assert(equal(construct_iter_range, input));

        vec contruct_initializer_list({0, 1, 2, 3, 4, 5});
        assert(equal(contruct_initializer_list, input));

        vec assignment_initializer_list_grow{3, 4};
        assignment_initializer_list_grow = {0, 1, 2, 3, 4, 5};
        assert(equal(assignment_initializer_list_grow, input));

        vec assignment_initializer_list_shrink{6, 4};
        assignment_initializer_list_shrink = {1, 2, 3, 4};
        assert(equal(assignment_initializer_list_shrink, short_input));
    }

    { // assign
        vec assign_count_val_grow{3, 4};
        assign_count_val_grow.assign(5, 1);
        assert(equal(assign_count_val_grow, vec{1, 1, 1, 1, 1}));

        vec assign_count_val_shrink{3, 4};
        assign_count_val_shrink.assign(2, 1);
        assert(equal(assign_count_val_shrink, vec{1, 1}));

        vec assign_range_grow{3, 4};
        assign_range_grow.assign(input);
        assert(equal(assign_range_grow, input));

        vec assign_range_shrink{6, 4};
        assign_range_shrink.assign(short_input);
        assert(equal(assign_range_shrink, short_input));

        vec assign_iter_range_grow{3, 4};
        assign_iter_range_grow.assign(begin(input), end(input));
        assert(equal(assign_iter_range_grow, input));

        vec assign_iter_range_shrink{6, 4};
        assign_iter_range_shrink.assign(begin(short_input), end(short_input));
        assert(equal(assign_iter_range_shrink, short_input));

        vec assign_initializer_list_grow{3, 4};
        assign_initializer_list_grow.assign({0, 1, 2, 3, 4, 5});
        assert(equal(assign_initializer_list_grow, input));

        vec assign_initializer_list_shrink{6, 4};
        assign_initializer_list_shrink.assign({1, 2, 3, 4});
        assert(equal(assign_initializer_list_shrink, short_input));
    }

    { // iterators
        vec range(input);
        const vec const_range(input);

        const same_as<vec::iterator> auto b = range.begin();
        assert(*b == 0);

        const same_as<vec::const_iterator> auto cb = range.cbegin();
        assert(*cb == 0);

        const same_as<vec::const_iterator> auto cb2 = const_range.begin();
        assert(*cb2 == 0);

        const same_as<vec::iterator> auto e = range.end();
        assert(*prev(e) == 5);

        const same_as<vec::const_iterator> auto ce = range.cend();
        assert(*prev(ce) == 5);

        const same_as<vec::const_iterator> auto ce2 = const_range.end();
        assert(*prev(ce2) == 5);

        const same_as<vec::reverse_iterator> auto rb = range.rbegin();
        assert(*rb == 5);

        const same_as<vec::const_reverse_iterator> auto crb = range.crbegin();
        assert(*crb == 5);

        const same_as<vec::const_reverse_iterator> auto crb2 = const_range.rbegin();
        assert(*crb2 == 5);

        const same_as<vec::reverse_iterator> auto re = range.rend();
        assert(*prev(re) == 0);

        const same_as<vec::const_reverse_iterator> auto cre = range.crend();
        assert(*prev(cre) == 0);

        const same_as<vec::const_reverse_iterator> auto cre2 = const_range.rend();
        assert(*prev(cre2) == 0);
    }

    { // access
        vec range(input);
        const vec const_range(input);

        same_as<int&> auto&& op = range[3];
        assert(op == 3);

        range[3]                 = 4;
        same_as<int&> auto&& op2 = range[3];
        assert(op2 == 4);

        same_as<const int&> auto&& cop = const_range[3];
        assert(cop == 3);

        same_as<int&> auto&& f = range.front();
        assert(f == 0);

        same_as<const int&> auto&& cf = const_range.front();
        assert(cf == 0);

        same_as<int&> auto&& b = range.back();
        assert(b == 5);

        same_as<const int&> auto&& cb = const_range.back();
        assert(cb == 5);

        same_as<int*> auto&& d = range.data();
        assert(*d == 0);

        same_as<const int*> auto&& cd = const_range.data();
        assert(*cd == 0);
    }

    { // capacity
        vec range(input);
        const vec const_range(input);

        const same_as<bool> auto e = range.empty();
        assert(!e);

        const same_as<bool> auto ec = const_range.empty();
        assert(!ec);

        const same_as<size_t> auto s = range.size();
        assert(s == size(input));

        const same_as<size_t> auto sc = const_range.size();
        assert(sc == size(input));

        const same_as<size_t> auto ms = range.max_size();
        assert(ms == vector_capacity);

        const same_as<size_t> auto msc = const_range.max_size();
        assert(msc == vector_capacity);

        const same_as<size_t> auto c = range.capacity();
        assert(c == vector_capacity);

        const same_as<size_t> auto cc = const_range.capacity();
        assert(cc == vector_capacity);
    }

    { // insert
        constexpr int expected_insert[] = {0, 3, 5};
        constexpr int to_be_inserted    = 3;

        vec insert_lvalue = {0, 5};
        const same_as<vec::iterator> auto res_insert_lvalue =
            insert_lvalue.insert(insert_lvalue.begin() + 1, to_be_inserted);
        assert(equal(insert_lvalue, expected_insert));
        assert(res_insert_lvalue == insert_lvalue.begin() + 1);

        vec insert_lvalue_const = {0, 5};
        const same_as<vec::iterator> auto res_insert_lvalue_const =
            insert_lvalue_const.insert(insert_lvalue_const.cbegin() + 1, to_be_inserted);
        assert(equal(insert_lvalue_const, expected_insert));
        assert(res_insert_lvalue_const == insert_lvalue_const.cbegin() + 1);

        vec insert_rvalue                                   = {0, 5};
        const same_as<vec::iterator> auto res_insert_rvalue = insert_rvalue.insert(insert_rvalue.begin() + 1, 3);
        assert(equal(insert_rvalue, expected_insert));
        assert(res_insert_rvalue == insert_rvalue.begin() + 1);

        vec insert_rvalue_const = {0, 5};
        const same_as<vec::iterator> auto res_insert_rvalue_const =
            insert_rvalue_const.insert(insert_rvalue_const.cbegin() + 1, 3);
        assert(equal(insert_rvalue_const, expected_insert));
        assert(res_insert_rvalue_const == insert_rvalue_const.cbegin() + 1);

        vec insert_range                                   = {0, 5};
        const same_as<vec::iterator> auto res_insert_range = insert_range.insert(insert_range.begin() + 1, short_input);
        assert(equal(insert_range, input));
        assert(res_insert_range == insert_range.begin() + 1);

        vec insert_range_const = {0, 5};
        const same_as<vec::iterator> auto res_insert_range_const =
            insert_range_const.insert(insert_range_const.cbegin() + 1, short_input);
        assert(equal(insert_range_const, input));
        assert(res_insert_range_const == insert_range_const.cbegin() + 1);

        vec insert_iter_range = {0, 5};
        const same_as<vec::iterator> auto res_insert_iter_range =
            insert_iter_range.insert(insert_iter_range.begin() + 1, begin(short_input), end(short_input));
        assert(equal(insert_iter_range, input));
        assert(res_insert_iter_range == insert_iter_range.begin() + 1);

        vec insert_iter_range_const = {0, 5};
        const same_as<vec::iterator> auto res_insert_iter_range_const =
            insert_iter_range_const.insert(insert_iter_range_const.cbegin() + 1, begin(short_input), end(short_input));
        assert(equal(insert_iter_range_const, input));
        assert(res_insert_iter_range_const == insert_iter_range_const.cbegin() + 1);

        vec insert_initializer = {0, 5};
        const same_as<vec::iterator> auto res_insert_initializer =
            insert_initializer.insert(insert_initializer.begin() + 1, {1, 2, 3, 4});
        assert(equal(insert_initializer, input));
        assert(res_insert_initializer == insert_initializer.begin() + 1);

        vec insert_initializer_const = {0, 5};
        const same_as<vec::iterator> auto res_insert_initializer_const =
            insert_initializer_const.insert(insert_initializer_const.cbegin() + 1, {1, 2, 3, 4});
        assert(equal(insert_initializer_const, input));
        assert(res_insert_initializer_const == insert_initializer_const.cbegin() + 1);
    }

    { // emplace
        vec emplace                                    = {0, 1, 2, 4, 5};
        const same_as<vec::iterator> auto res_emplaced = emplace.emplace(emplace.begin() + 3, 3);
        assert(equal(emplace, input));
        assert(res_emplaced == emplace.begin() + 3);

        vec emplace_const                                   = {0, 1, 2, 4, 5};
        const same_as<vec::iterator> auto res_emplace_const = emplace_const.emplace(emplace_const.cbegin() + 3, 3);
        assert(equal(emplace_const, input));
        assert(res_emplace_const == emplace_const.cbegin() + 3);

        vec emplace_back                      = {0, 1, 2, 3, 4};
        same_as<int&> auto&& res_emplace_back = emplace_back.emplace_back(5);
        assert(equal(emplace_back, input));
        assert(res_emplace_back++ == 5);
        assert(res_emplace_back == 6);

        vec push_back_lvalue                      = {0, 1, 2, 3, 4};
        const int to_be_pushed                    = 5;
        same_as<int&> auto&& res_push_back_lvalue = push_back_lvalue.push_back(to_be_pushed);
        assert(equal(push_back_lvalue, input));
        assert(res_push_back_lvalue++ == 5);
        assert(res_push_back_lvalue == 6);

        vec push_back_rvalue                      = {0, 1, 2, 3, 4};
        same_as<int&> auto&& res_push_back_rvalue = push_back_rvalue.push_back(5);
        assert(equal(push_back_rvalue, input));
        assert(res_push_back_rvalue++ == 5);
        assert(res_push_back_rvalue == 6);
    }

    { // resize
        vec resize_shrink{input};
        resize_shrink.resize(1);
        assert(equal(resize_shrink, vec{0}));

        vec resize_value_shrink{input};
        resize_value_shrink.resize(1, 5);
        assert(equal(resize_value_shrink, vec{0}));

        vec resize_grow(3, 4);
        resize_grow.resize(5);
        assert(equal(resize_grow, vec{4, 4, 4, 0, 0}));

        vec resize_value_grow(3, 4);
        resize_value_grow.resize(5, 2);
        assert(equal(resize_value_grow, vec{4, 4, 4, 2, 2}));
    }

    { // other modifiers
        vec clear{input};
        clear.clear();
        assert(clear.empty());

        vec pop_back{input};
        pop_back.pop_back();
        assert(equal(pop_back, vec{0, 1, 2, 3, 4}));

        vec swap_first{input};
        vec swap_second{short_input};

        swap_first.swap(swap_second);
        assert(equal(swap_first, short_input));
        assert(equal(swap_second, input));

        swap(swap_first, swap_second);
        assert(equal(swap_first, input));
        assert(equal(swap_second, short_input));
    }

    { // erase
        vec erased{1, 2, 3, 4, 2, 3};
        erase(erased, 2);
        assert(equal(erased, vec{1, 3, 4, 3}));

        vec erased_if{1, 2, 3, 4, 2, 3};
        erase_if(erased_if, [](const int val) { return val < 4; });
        assert(equal(erased_if, vec{4}));
    }

    { // comparison
        vec first(input);
        vec second(input);
        vec third{short_input};

        const same_as<bool> auto e = first == second;
        assert(e);

        const same_as<bool> auto ne = first != third;
        assert(ne);
    }
    return true;
}

constexpr bool test_iterators() {
    static_assert(contiguous_iterator<vec::iterator>);
    static_assert(contiguous_iterator<vec::const_iterator>);

    vec range(input);

    { // increment
        auto it = range.begin();
        assert(*++it == 1);
        assert(*it++ == 1);
        assert(*it == 2);

        auto cit = range.cbegin();
        assert(*++cit == 1);
        assert(*cit++ == 1);
        assert(*cit == 2);
    }

    { // advance
        auto it = range.begin() + 2;
        assert(*it == 2);
        it += 2;
        assert(*it == 4);

        auto cit = range.cbegin() + 2;
        assert(*cit == 2);
        cit += 2;
        assert(*cit == 4);
    }

    { // decrement
        auto it = range.end();
        assert(*--it == 5);
        assert(*it-- == 5);
        assert(*it == 4);

        auto cit = range.cend();
        assert(*--cit == 5);
        assert(*cit-- == 5);
        assert(*cit == 4);
    }

    { // advance back
        auto it = range.end() - 2;
        assert(*it == 4);
        it -= 2;
        assert(*it == 2);

        auto cit = range.cend() - 2;
        assert(*cit == 4);
        cit -= 2;
        assert(*cit == 2);
    }

    { // difference
        const auto it1 = range.begin();
        const auto it2 = range.end();
        assert(it2 - it1 == ssize(input));

        const auto cit1 = range.cbegin();
        const auto cit2 = range.cend();
        assert(cit2 - cit1 == ssize(input));

        assert(it2 - cit1 == ssize(input));
        assert(cit2 - it1 == ssize(input));
    }

    { // comparison
        const auto it1 = range.begin();
        const auto it2 = range.begin();
        const auto it3 = range.end();

        assert(it1 == it2);
        assert(it1 != it3);
        assert(it1 < it3);
        assert(it1 <= it3);
        assert(it3 > it1);
        assert(it3 >= it1);
    }

    { // access
        const auto it = range.begin() + 2;
        it[2]         = 3;
        assert(range[4] == 3);

        const auto cit = range.cbegin() + 2;
        assert(cit[2] == 3);

        static_vector<pair<int, int>, 2> vec2 = {{1, 2}, {2, 3}};
        const auto it2                        = vec2.begin();
        assert(it2->second == 2);

        const auto cit2 = vec2.cbegin();
        assert(cit2->first == 1);
    }

    return true;
}

struct test_ranges {
    template <ranges::input_range Read>
    static constexpr void call() {
        using ranges::begin, ranges::end, ranges::equal;

        { // constructors
            Read wrapped_input1{input};
            vec construct_range(wrapped_input1);
            assert(equal(construct_range, input));

            Read wrapped_input2{input};
            vec construct_iter_range(begin(wrapped_input2), end(wrapped_input2));
            assert(equal(construct_iter_range, input));
        }

        { // assign range
            Read wrapped_input{input};
            vec assign_range_grow(3, 4);
            assign_range_grow.assign(wrapped_input);
            assert(equal(assign_range_grow, input));

            Read wrapped_short_input{short_input};
            vec assign_range_shrink(6, 4);
            assign_range_shrink.assign(wrapped_short_input);
            assert(equal(assign_range_shrink, short_input));
        }

        { // assign iter range
            Read wrapped_input{input};
            vec assign_iter_range_grow(3, 4);
            assign_iter_range_grow.assign(begin(wrapped_input), end(wrapped_input));
            assert(equal(assign_iter_range_grow, input));

            Read wrapped_short_input{short_input};
            vec assign_iter_range_shrink(3, 4);
            assign_iter_range_shrink.assign(begin(wrapped_short_input), end(wrapped_short_input));
            assert(equal(assign_iter_range_shrink, short_input));
        }

        { // insert
            Read wrapped_input1{short_input};
            vec insert_range = {0, 5};
            const same_as<vec::iterator> auto res_insert_range =
                insert_range.insert(insert_range.begin() + 1, wrapped_input1);
            assert(equal(insert_range, input));
            assert(res_insert_range == insert_range.begin() + 1);

            Read wrapped_input2{short_input};
            vec insert_iter_range = {0, 5};
            const same_as<vec::iterator> auto res_insert_iter_range =
                insert_iter_range.insert(insert_iter_range.begin() + 1, begin(wrapped_input2), end(wrapped_input2));
            assert(equal(insert_iter_range, input));
            assert(res_insert_iter_range == insert_iter_range.begin() + 1);
        }
    }
};

int main() {
    test_interface();
    test_iterators();
    test_in<test_ranges, const int>();

    static_assert(test_interface());
    static_assert(test_iterators());
    static_assert((test_in<test_ranges, const int>(), true));
}
