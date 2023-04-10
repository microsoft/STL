// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <initializer_list>
#include <optional>
#include <type_traits>
#include <utility>
#include <variant>

using namespace std;

struct With_trivial_destructor {
    int _val = 0;
    constexpr With_trivial_destructor(const int val) noexcept : _val(val) {}
    constexpr With_trivial_destructor(initializer_list<int> vals) noexcept : _val(*vals.begin()) {}

    constexpr bool operator==(const int right) const noexcept {
        return _val == right;
    }
};

struct With_nontrivial_destructor {
    int _val = 0;
    constexpr With_nontrivial_destructor(const int val) noexcept : _val(val) {}
    constexpr With_nontrivial_destructor(initializer_list<int> vals) noexcept : _val(*vals.begin()) {}
    With_nontrivial_destructor(const With_nontrivial_destructor&)            = default;
    With_nontrivial_destructor& operator=(const With_nontrivial_destructor&) = default;
    constexpr ~With_nontrivial_destructor() {}

    constexpr bool operator==(const int right) const noexcept {
        return _val == right;
    }
};

struct Dummy {
    int _val = 0;
};

template <class T>
constexpr bool test_optional() {
    { // empty construction
        optional<T> default_constructed;
        assert(!default_constructed.has_value());

        optional<T> nullopt_constructed{nullopt};
        assert(!nullopt_constructed.has_value());
    }

    { // construction from underlying type
        const T input{42};
        optional<T> construct_from_type{input};
        assert(construct_from_type.has_value());
        assert(*construct_from_type == 42);

        optional<T> construct_from_type_rvalue{T{42}};
        assert(construct_from_type_rvalue.has_value());
        assert(*construct_from_type_rvalue == 42);

        optional<T> assign_from_type;
        assert(!assign_from_type.has_value());
        assign_from_type = input;
        assert(assign_from_type.has_value());
        assert(*assign_from_type == 42);

        optional<T> assign_from_type_rvalue;
        assert(!assign_from_type_rvalue.has_value());
        assign_from_type_rvalue = T{42};
        assert(assign_from_type_rvalue.has_value());
        assert(*assign_from_type_rvalue == 42);
    }

    { // construction from convertible type
        const int input{42};
        optional<T> construct_from_convertible_type{input};
        assert(construct_from_convertible_type.has_value());
        assert(*construct_from_convertible_type == 42);

        optional<T> construct_from_convertible_type_rvalue{42};
        assert(construct_from_convertible_type_rvalue.has_value());
        assert(*construct_from_convertible_type_rvalue == 42);

        optional<T> assign_from_convertible_type;
        assert(!assign_from_convertible_type.has_value());
        assign_from_convertible_type = input;
        assert(assign_from_convertible_type.has_value());
        assert(*assign_from_convertible_type == 42);

        optional<T> assign_from_convertible_type_rvalue;
        assert(!assign_from_convertible_type_rvalue.has_value());
        assign_from_convertible_type_rvalue = 42;
        assert(assign_from_convertible_type_rvalue.has_value());
        assert(*assign_from_convertible_type_rvalue == 42);
    }

    { // construction from optional with same type
        optional<T> constructed{42};
        assert(constructed.has_value());
        assert(*constructed == 42);

        optional<T> copy_constructed{constructed};
        assert(copy_constructed.has_value());
        assert(*copy_constructed == 42);

        optional<T> move_constructed{move(constructed)};
        assert(move_constructed.has_value());
        assert(*move_constructed == 42);

        optional<T> copy_assigned;
        assert(!copy_assigned.has_value());
        copy_assigned = move_constructed;
        assert(copy_assigned.has_value());
        assert(*copy_assigned == 42);

        optional<T> move_assigned;
        assert(!move_assigned.has_value());
        move_assigned = move(copy_assigned);
        assert(move_assigned.has_value());
        assert(*move_assigned == 42);
    }

    { // construction from optional with convertible types
        optional<int> input{42};

        optional<T> construct_from_convertible_optional{input};
        assert(construct_from_convertible_optional.has_value());
        assert(*construct_from_convertible_optional == 42);

        optional<T> construct_from_convertible_optional_rvalue{optional<int>{3}};
        assert(construct_from_convertible_optional_rvalue.has_value());
        assert(*construct_from_convertible_optional_rvalue == 3);

        optional<T> assign_from_convertible_optional;
        assert(!assign_from_convertible_optional.has_value());
        assign_from_convertible_optional = input;
        assert(assign_from_convertible_optional.has_value());
        assert(*assign_from_convertible_optional == 42);

        optional<T> assign_from_convertible_optional_rvalue;
        assert(!assign_from_convertible_optional_rvalue.has_value());
        assign_from_convertible_optional_rvalue = optional<int>{3};
        assert(assign_from_convertible_optional_rvalue.has_value());
        assert(*assign_from_convertible_optional_rvalue == 3);
    }

    { // emplace
        T input{42};
        optional<T> emplace_copy;
        assert(!emplace_copy.has_value());
        emplace_copy.emplace(input);
        assert(emplace_copy.has_value());
        assert(*emplace_copy == 42);

        optional<T> emplace_move;
        assert(!emplace_move.has_value());
        emplace_move.emplace(T{42});
        assert(emplace_move.has_value());
        assert(*emplace_move == 42);

        optional<T> emplace_conversion;
        assert(!emplace_conversion.has_value());
        emplace_conversion.emplace(42);
        assert(emplace_conversion.has_value());
        assert(*emplace_conversion == 42);

        optional<T> emplace_initializer_list;
        assert(!emplace_initializer_list.has_value());
        emplace_initializer_list.emplace({42, 43});
        assert(emplace_initializer_list.has_value());
        assert(*emplace_initializer_list == 42);
    }

    { // reset
        optional<T> resetted{42};
        resetted.reset();
        assert(!resetted.has_value());
    }

    { // swap
        optional<T> left{42};
        optional<T> right{3};
        assert(*left == 42);
        assert(*right == 3);

        left.swap(right);
        assert(*left == 3);
        assert(*right == 42);

        swap(left, right);
        assert(*left == 42);
        assert(*right == 3);
    }
    return true;
}

template <class T>
constexpr bool test_variant() {
    { // construction from underlying type
        const T input{42};
        variant<Dummy, T> construct_from_type{input};
        assert(construct_from_type.index() == 1);
        assert(get<T>(construct_from_type) == 42);

        variant<Dummy, T> construct_from_type_rvalue{T{42}};
        assert(construct_from_type_rvalue.index() == 1);
        assert(get<T>(construct_from_type_rvalue) == 42);

        variant<Dummy, T> assign_from_type;
        assert(assign_from_type.index() == 0);
        assign_from_type = input;
        assert(assign_from_type.index() == 1);
        assert(get<T>(assign_from_type) == 42);

        variant<Dummy, T> assign_from_type_rvalue;
        assert(assign_from_type_rvalue.index() == 0);
        assign_from_type_rvalue = T{42};
        assert(assign_from_type_rvalue.index() == 1);
        assert(get<T>(assign_from_type_rvalue) == 42);
    }

    { // construction from variant with same type
        variant<Dummy, T> constructed{T{42}};
        assert(constructed.index() == 1);
        assert(get<T>(constructed) == 42);

        variant<Dummy, T> copy_constructed{constructed};
        assert(copy_constructed.index() == 1);
        assert(get<T>(copy_constructed) == 42);

        variant<Dummy, T> move_constructed{move(constructed)};
        assert(move_constructed.index() == 1);
        assert(get<T>(move_constructed) == 42);

        variant<Dummy, T> copy_assigned;
        assert(copy_assigned.index() == 0);
        copy_assigned = constructed;
        assert(copy_assigned.index() == 1);
        assert(get<T>(copy_assigned) == 42);

        variant<Dummy, T> move_assigned;
        assert(move_assigned.index() == 0);
        move_assigned = move(constructed);
        assert(move_assigned.index() == 1);
        assert(get<T>(move_assigned) == 42);
    }

    { // emplace type
        T input{42};
        variant<Dummy, T> emplace_copy;
        assert(emplace_copy.index() == 0);
        emplace_copy.template emplace<T>(input);
        assert(emplace_copy.index() == 1);
        assert(get<T>(emplace_copy) == 42);

        variant<Dummy, T> emplace_move;
        assert(emplace_move.index() == 0);
        emplace_move.template emplace<T>(T{42});
        assert(emplace_move.index() == 1);
        assert(get<T>(emplace_move) == 42);

        variant<Dummy, T> emplace_conversion;
        assert(emplace_conversion.index() == 0);
        emplace_conversion.template emplace<T>(42);
        assert(emplace_conversion.index() == 1);
        assert(get<T>(emplace_conversion) == 42);

        variant<Dummy, T> emplace_initializer_list;
        assert(emplace_initializer_list.index() == 0);
        emplace_initializer_list.template emplace<T>({42, 43});
        assert(emplace_initializer_list.index() == 1);
        assert(get<T>(emplace_initializer_list) == 42);
    }

    { // emplace index
        T input{42};
        variant<Dummy, T> emplace_copy;
        assert(emplace_copy.index() == 0);
        emplace_copy.template emplace<1>(input);
        assert(emplace_copy.index() == 1);
        assert(get<T>(emplace_copy) == 42);

        variant<Dummy, T> emplace_move;
        assert(emplace_move.index() == 0);
        emplace_move.template emplace<1>(T{42});
        assert(emplace_move.index() == 1);
        assert(get<T>(emplace_move) == 42);

        variant<Dummy, T> emplace_conversion;
        assert(emplace_conversion.index() == 0);
        emplace_conversion.template emplace<1>(42);
        assert(emplace_conversion.index() == 1);
        assert(get<T>(emplace_conversion) == 42);

        variant<Dummy, T> emplace_initializer_list;
        assert(emplace_initializer_list.index() == 0);
        emplace_initializer_list.template emplace<1>({42, 43});
        assert(emplace_initializer_list.index() == 1);
        assert(get<T>(emplace_initializer_list) == 42);
    }

    { // swap
        variant<Dummy, T> left{T{42}};
        variant<Dummy, T> right;
        assert(left.index() == 1);
        assert(get<T>(left) == 42);
        assert(right.index() == 0);

        left.swap(right);
        assert(right.index() == 1);
        assert(get<T>(right) == 42);
        assert(left.index() == 0);

        swap(left, right);
        assert(left.index() == 1);
        assert(get<T>(left) == 42);
        assert(right.index() == 0);
    }
    return true;
}

int main() {
    test_optional<With_trivial_destructor>();
    test_optional<With_nontrivial_destructor>();
    static_assert(test_optional<With_trivial_destructor>());
    static_assert(test_optional<With_nontrivial_destructor>());

    test_variant<With_trivial_destructor>();
    test_variant<With_nontrivial_destructor>();
    static_assert(test_variant<With_trivial_destructor>());
    static_assert(test_variant<With_nontrivial_destructor>());
}
