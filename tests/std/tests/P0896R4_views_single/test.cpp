// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <memory>
#include <new>
#include <ranges>
#include <string>
#include <utility>

using namespace std;

template <class>
constexpr bool always_false = false;

struct only_copy_constructible {
    int val;

    constexpr explicit only_copy_constructible(int i) noexcept : val{i} {}
    only_copy_constructible(const only_copy_constructible&) = default;
    only_copy_constructible(only_copy_constructible&& that) : val{exchange(that.val, -1)} {}

    bool operator==(const only_copy_constructible&) const = default;

    template <class T = void>
    auto operator&() const {
        static_assert(always_false<T>);
    }

    template <class U>
    auto operator,(U&&) const {
        static_assert(always_false<U>);
    }
};

struct literal_class {
    int value;

    literal_class() = default;
    constexpr explicit literal_class(int i) noexcept : value{i} {}

    bool operator==(const literal_class&) const = default;
};

template <class T, class... Args>
constexpr bool test_one_type(T value, Args&&... args) {
    // validate type properties
    using R = ranges::single_view<T>;
    static_assert(ranges::view<R> && ranges::contiguous_range<R> && ranges::sized_range<R> && ranges::common_range<R>);

    // validate CTAD and const T& constructor
    same_as<R> auto r0 = ranges::single_view{value};
    const R& cr0       = r0;

    // validate member size
    static_assert(same_as<decltype(R::size()), size_t>);
    static_assert(R::size() == 1);
    static_assert(noexcept(R::size()));
    static_assert(noexcept(ranges::size(r0)));
    static_assert(noexcept(ranges::size(cr0)));

    // validate member data
    const same_as<T*> auto ptr = r0.data();
    assert(ptr != nullptr);
    assert(ptr != addressof(value));
    static_assert(noexcept(r0.data()));
    static_assert(noexcept(ranges::data(r0)));

    const same_as<const T*> auto cptr = cr0.data();
    assert(cptr == ptr);
    static_assert(noexcept(cr0.data()));
    static_assert(noexcept(ranges::data(cr0)));

    // validate members begin and end
    static_assert(same_as<decltype(r0.begin()), T*>);
    assert(*r0.begin() == value);
    assert(r0.begin() == ptr);
    static_assert(noexcept(r0.begin()));
    static_assert(noexcept(ranges::begin(r0)));

    static_assert(same_as<decltype(r0.end()), T*>);
    assert(r0.end() == ptr + 1);
    static_assert(noexcept(r0.end()));
    static_assert(noexcept(ranges::end(r0)));

    static_assert(same_as<decltype(cr0.begin()), const T*>);
    assert(*cr0.begin() == value);
    assert(cr0.begin() == cptr);
    static_assert(noexcept(cr0.begin()));
    static_assert(noexcept(ranges::begin(cr0)));

    static_assert(same_as<decltype(cr0.end()), const T*>);
    assert(cr0.end() == cptr + 1);
    static_assert(noexcept(cr0.end()));
    static_assert(noexcept(ranges::end(cr0)));

    // validate CTAD and T&& constructor
    const same_as<R> auto cr1 = ranges::single_view{move(value)};
    assert(cr1.data() != nullptr);
    assert(cr1.data() != addressof(value));
    assert(cr1.data() != cr0.data());
    assert(*cr1.data() == *cr0.data());

    // validate in_place constructor
    const same_as<R> auto cr2 = ranges::single_view<T>{in_place, forward<Args>(args)...};
    assert(cr2.data() != nullptr);
    assert(*cr2.data() == *cr1.data());
    static_assert(
        noexcept(R{in_place, forward<Args>(args)...}) == is_nothrow_constructible_v<T, Args...>); // strengthened

    // validate CPO [lvalue]
    auto value2               = *cr2.data();
    const same_as<R> auto cr3 = views::single(value2);
    assert(cr3.data() != nullptr);
    assert(cr3.data() != addressof(value2));
    assert(*cr3.data() == *cr2.data());
    static_assert(noexcept(views::single(value)) == is_nothrow_copy_constructible_v<T>); // strengthened

    // validate CPO [rvalue]
    const same_as<R> auto cr4 = views::single(move(value2));
    assert(cr4.data() != nullptr);
    assert(cr4.data() != addressof(value2));
    assert(*cr4.data() == *cr3.data());
    static_assert(noexcept(views::single(move(value))) == is_nothrow_move_constructible_v<T>); // strengthened

    // validate members inherited from view_interface
    assert(!cr0.empty());
    assert(cr0);
    assert(addressof(cr0.front()) == ptr);
    assert(addressof(cr0.back()) == ptr);

    return true;
}

// Validate that single-view-of-single-view nests correctly per P2367R0
static_assert(same_as<ranges::single_view<ranges::single_view<int>>, decltype(views::single(views::single(42)))>);
// Validate that views::single decays its argument type correctly per P2367R0
static_assert(same_as<ranges::single_view<const char*>, decltype(views::single("Hello, World!"))>);
void double_function(double);
static_assert(same_as<ranges::single_view<void (*)(double)>, decltype(views::single(double_function))>);

// Validate that the _Copyable_box primary template works when fed with a non-trivially-destructible type
void test_non_trivially_destructible_type() { // COMPILE-ONLY
    struct non_trivially_destructible {
        non_trivially_destructible() = default;
        ~non_trivially_destructible() {}

        // To test the correct specialization of _Copyable_box, this type must not be copy assignable.
        non_trivially_destructible(const non_trivially_destructible&)            = default;
        non_trivially_destructible& operator=(const non_trivially_destructible&) = delete;
    };

    (void) views::single(non_trivially_destructible{});
}

int main() {
    static_assert(test_one_type(42, 42));
    test_one_type(42, 42);

    static_assert(test_one_type(literal_class{42}, 42));
    test_one_type(literal_class{42}, 42);

    test_one_type(only_copy_constructible{42}, 42);
    test_one_type(string{"Hello, World!"}, "Hello, World!");
}
