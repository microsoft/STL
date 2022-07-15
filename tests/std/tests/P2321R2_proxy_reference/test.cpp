// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>
using namespace std;

enum class State {
    none,
    copy_construct,
    copy_construct_from_const,
    move_construct,
    move_construct_from_const,
    copy_assign,
    copy_assign_to_const,
    move_assign,
    move_assign_to_const,
    swap,
    swap_const,
};

struct Meow {
    Meow() = default;
    constexpr Meow(Meow&) : state(State::copy_construct) {}
    constexpr Meow(const Meow&) : state(State::copy_construct_from_const) {}
    constexpr Meow(Meow&&) : state(State::move_construct) {}
    constexpr Meow(const Meow&&) : state(State::move_construct_from_const) {}

    constexpr Meow& operator=(Meow&)             = delete;
    constexpr const Meow& operator=(Meow&) const = delete;

    constexpr Meow& operator=(const Meow&) {
        state = State::copy_assign;
        return *this;
    }
    constexpr const Meow& operator=(const Meow&) const {
        state = State::copy_assign_to_const;
        return *this;
    }
    constexpr Meow& operator=(Meow&&) {
        state = State::move_assign;
        return *this;
    }
    constexpr const Meow& operator=(Meow&&) const {
        state = State::move_assign_to_const;
        return *this;
    }

    constexpr Meow& operator=(const Meow&&)             = delete;
    constexpr const Meow& operator=(const Meow&&) const = delete;

    friend constexpr void swap(Meow& left, Meow&) {
        left.state = State::swap;
    }
    friend constexpr void swap(const Meow& left, const Meow&) {
        left.state = State::swap_const;
    }

    mutable State state;
};

constexpr bool test() {
    { // Test pair and tuple
        int val = 0;

        pair<int&, Meow> pris{val, Meow{}};

        // Test pair construction
        pair<int&, Meow> pris2 = pris;
        pair<int&, Meow> pris3 = as_const(pris);
        pair<int&, Meow> pris4 = move(pris);
        pair<int&, Meow> pris5 = move(as_const(pris));

        pair<const int&, Meow> prcis1 = pris;
        pair<const int&, Meow> prcis2 = as_const(pris);
        pair<const int&, Meow> prcis3 = move(pris);
        pair<const int&, Meow> prcis4 = move(as_const(pris));

        assert(pris2.second.state == State::copy_construct);
        assert(pris3.second.state == State::copy_construct_from_const);
        assert(pris4.second.state == State::move_construct);
        assert(pris5.second.state == State::move_construct_from_const);
        assert(prcis1.second.state == State::copy_construct);
        assert(prcis2.second.state == State::copy_construct_from_const);
        assert(prcis3.second.state == State::move_construct);
        assert(prcis4.second.state == State::move_construct_from_const);

        // Test pair assignment from lvalue
        pris2           = pris4;
        as_const(pris3) = pris5;
        pris4           = prcis1;
        as_const(pris5) = prcis2;

        assert(pris2.second.state == State::copy_assign);
        assert(pris3.second.state == State::copy_assign_to_const);
        assert(pris4.second.state == State::copy_assign);
        assert(pris5.second.state == State::copy_assign_to_const);

        // Test pair assignment from rvalue
        pris2           = move(pris4);
        as_const(pris3) = move(pris5);
        pris4           = move(prcis1);
        as_const(pris5) = move(prcis2);

        assert(pris2.second.state == State::move_assign);
        assert(pris3.second.state == State::move_assign_to_const);
        assert(pris4.second.state == State::move_assign);
        assert(pris5.second.state == State::move_assign_to_const);

        // Test pair member swap
        pris.swap(pris2);
        as_const(pris2).swap(as_const(pris3));

        assert(pris.second.state == State::swap);
        assert(pris2.second.state == State::swap_const);

        // Test pair non-member swap
        swap(pris3, pris5);
        swap(as_const(pris4), as_const(pris5));
        assert(pris3.second.state == State::swap);
        assert(pris4.second.state == State::swap_const);
        static_assert(is_nothrow_swappable_v<pair<int&, int&>>);
        static_assert(is_nothrow_swappable_v<const pair<int&, int&>>);
        static_assert(!is_nothrow_swappable_v<pair<int&, Meow>>);
        static_assert(!is_nothrow_swappable_v<const pair<int&, Meow>>);

#ifdef __cpp_lib_concepts
        // Test basic_common_reference and common_type specializations for pair
        static_assert(is_same_v<common_reference_t<pair<int&, Meow>, pair<const int&, Meow>>, pair<const int&, Meow>>);
        static_assert(is_same_v<common_reference_t<const pair<int&, Meow>, pair<const int&, Meow>>,
            pair<const int&, const Meow>>);
        static_assert(is_same_v<common_type_t<pair<int&, Meow>, pair<const int&, Meow>>, pair<int, Meow>>);
#endif // __cpp_lib_concepts

        tuple<int&, Meow> tris{val, Meow{}};

        // Test tuple construction (from tuple or pair)
        tuple<int&, Meow> tris2 = tris;
        tuple<int&, Meow> tris3 = as_const(tris);
        tuple<int&, Meow> tris4 = move(tris);
        tuple<int&, Meow> tris5 = move(as_const(tris));
        tuple<int&, Meow> tris6 = pris;
        tuple<int&, Meow> tris7 = as_const(pris);
        tuple<int&, Meow> tris8 = move(pris);
        tuple<int&, Meow> tris9 = move(as_const(pris));

        tuple<const int&, Meow> trcis1 = tris;
        tuple<const int&, Meow> trcis2 = as_const(tris);
        tuple<const int&, Meow> trcis3 = move(tris);
        tuple<const int&, Meow> trcis4 = move(as_const(tris));
        tuple<const int&, Meow> trcis5 = pris;
        tuple<const int&, Meow> trcis6 = as_const(pris);
        tuple<const int&, Meow> trcis7 = move(pris);
        tuple<const int&, Meow> trcis8 = move(as_const(pris));

        assert(get<1>(tris2).state == State::copy_construct);
        assert(get<1>(tris3).state == State::copy_construct_from_const);
        assert(get<1>(tris4).state == State::move_construct);
        assert(get<1>(tris5).state == State::move_construct_from_const);
        assert(get<1>(tris6).state == State::copy_construct);
        assert(get<1>(tris7).state == State::copy_construct_from_const);
        assert(get<1>(tris8).state == State::move_construct);
        assert(get<1>(tris9).state == State::move_construct_from_const);
        assert(get<1>(trcis1).state == State::copy_construct);
        assert(get<1>(trcis2).state == State::copy_construct_from_const);
        assert(get<1>(trcis3).state == State::move_construct);
        assert(get<1>(trcis4).state == State::move_construct_from_const);
        assert(get<1>(trcis5).state == State::copy_construct);
        assert(get<1>(trcis6).state == State::copy_construct_from_const);
        assert(get<1>(trcis7).state == State::move_construct);
        assert(get<1>(trcis8).state == State::move_construct_from_const);

        // Test tuple uses-allocator construction
        tuple<int&, Meow> tris10{allocator_arg, allocator<int>{}, tris};
        tuple<int&, Meow> tris11{allocator_arg, allocator<int>{}, as_const(tris)};
        tuple<int&, Meow> tris12{allocator_arg, allocator<int>{}, move(tris)};
        tuple<int&, Meow> tris13{allocator_arg, allocator<int>{}, move(as_const(tris))};
        tuple<int&, Meow> tris14{allocator_arg, allocator<int>{}, pris};
        tuple<int&, Meow> tris15{allocator_arg, allocator<int>{}, as_const(pris)};
        tuple<int&, Meow> tris16{allocator_arg, allocator<int>{}, move(pris)};
        tuple<int&, Meow> tris17{allocator_arg, allocator<int>{}, move(as_const(pris))};

        tuple<const int&, Meow> trcis9{allocator_arg, allocator<int>{}, tris};
        tuple<const int&, Meow> trcis10{allocator_arg, allocator<int>{}, as_const(tris)};
        tuple<const int&, Meow> trcis11{allocator_arg, allocator<int>{}, move(tris)};
        tuple<const int&, Meow> trcis12{allocator_arg, allocator<int>{}, move(as_const(tris))};
        tuple<const int&, Meow> trcis13{allocator_arg, allocator<int>{}, pris};
        tuple<const int&, Meow> trcis14{allocator_arg, allocator<int>{}, as_const(pris)};
        tuple<const int&, Meow> trcis15{allocator_arg, allocator<int>{}, move(pris)};
        tuple<const int&, Meow> trcis16{allocator_arg, allocator<int>{}, move(as_const(pris))};

        assert(get<1>(tris10).state == State::copy_construct);
        assert(get<1>(tris11).state == State::copy_construct_from_const);
        assert(get<1>(tris12).state == State::move_construct);
        assert(get<1>(tris13).state == State::move_construct_from_const);
        assert(get<1>(tris14).state == State::copy_construct);
        assert(get<1>(tris15).state == State::copy_construct_from_const);
        assert(get<1>(tris16).state == State::move_construct);
        assert(get<1>(tris17).state == State::move_construct_from_const);
        assert(get<1>(trcis9).state == State::copy_construct);
        assert(get<1>(trcis10).state == State::copy_construct_from_const);
        assert(get<1>(trcis11).state == State::move_construct);
        assert(get<1>(trcis12).state == State::move_construct_from_const);
        assert(get<1>(trcis13).state == State::copy_construct);
        assert(get<1>(trcis14).state == State::copy_construct_from_const);
        assert(get<1>(trcis15).state == State::move_construct);
        assert(get<1>(trcis16).state == State::move_construct_from_const);

        // Test tuple assignment from tuple lvalue
        tris2           = tris;
        as_const(tris3) = tris;
        tris4           = trcis1;
        as_const(tris5) = trcis2;

        assert(get<1>(tris2).state == State::copy_assign);
        assert(get<1>(tris3).state == State::copy_assign_to_const);
        assert(get<1>(tris4).state == State::copy_assign);
        assert(get<1>(tris5).state == State::copy_assign_to_const);

        // Test tuple assignment from tuple rvalue
        tris2           = move(tris);
        as_const(tris3) = move(tris);
        tris4           = move(trcis1);
        as_const(tris5) = move(trcis2);

        assert(get<1>(tris2).state == State::move_assign);
        assert(get<1>(tris3).state == State::move_assign_to_const);
        assert(get<1>(tris4).state == State::move_assign);
        assert(get<1>(tris5).state == State::move_assign_to_const);

        // Test tuple assignment from pair
        tris2           = pris;
        as_const(tris3) = pris;
        tris4           = move(pris);
        as_const(tris5) = move(pris);

        assert(get<1>(tris2).state == State::copy_assign);
        assert(get<1>(tris3).state == State::copy_assign_to_const);
        assert(get<1>(tris4).state == State::move_assign);
        assert(get<1>(tris5).state == State::move_assign_to_const);

        // Test tuple member swap
        tris2.swap(tris);
        as_const(tris3).swap(tris);

        assert(get<1>(tris2).state == State::swap);
        assert(get<1>(tris3).state == State::swap_const);

        // Test tuple non-member swap
        swap(tris4, tris);
        swap(as_const(tris5), as_const(tris));

        assert(get<1>(tris4).state == State::swap);
        assert(get<1>(tris5).state == State::swap_const);

        static_assert(is_nothrow_swappable_v<tuple<int&, int&>>);
        static_assert(is_nothrow_swappable_v<const tuple<int&, int&>>);
        static_assert(!is_nothrow_swappable_v<tuple<int&, Meow>>);
        static_assert(!is_nothrow_swappable_v<const tuple<int&, Meow>>);

#ifdef __cpp_lib_concepts
        // Test basic_common_reference and common_type specializations for tuple
        static_assert(
            is_same_v<common_reference_t<tuple<int&, Meow>, tuple<const int&, Meow>>, tuple<const int&, Meow>>);
        static_assert(is_same_v<common_reference_t<const tuple<int&, Meow>, tuple<const int&, Meow>>,
            tuple<const int&, const Meow>>);
        static_assert(is_same_v<common_type_t<tuple<int&, Meow>, tuple<const int&, Meow>>, tuple<int, Meow>>);
#endif // __cpp_lib_concepts
    }

    { // Test vector<bool>::reference
        static_assert(is_assignable_v<const vector<bool>::reference, bool>);
#if defined(__EDG__) && _ITERATOR_DEBUG_LEVEL != 0 // TRANSITION, VSO-1274387, VSO-1273296
        if (!is_constant_evaluated())
#endif // defined(__EDG__) && _ITERATOR_DEBUG_LEVEL != 0
        {
            vector<bool> vb{false};
            const vector<bool>::reference r = vb[0];

            r = true;
            assert(vb.front());
        }
    }

    return true;
}

int main() {
    test();
    static_assert(test());
}
