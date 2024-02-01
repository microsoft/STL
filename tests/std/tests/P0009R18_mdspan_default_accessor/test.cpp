// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <array>
#include <cassert>
#include <concepts>
#include <cstdint>
#include <mdspan>
#include <string>
#include <type_traits>

#include <test_mdspan_support.hpp>

using namespace std;

template <class ElementType>
constexpr void test_one(array<ElementType, 3> elems) {
    using Accessor = default_accessor<ElementType>;

    // default_accessor meets the accessor policy requirements
    static_assert(check_accessor_policy_requirements<Accessor>());

    // Check modeled concepts
    static_assert(is_nothrow_move_constructible_v<Accessor>);
    static_assert(is_nothrow_move_assignable_v<Accessor>);
    static_assert(is_nothrow_swappable_v<Accessor>);
    static_assert(is_trivially_copyable_v<Accessor>);
    static_assert(semiregular<Accessor>);

    // Check if default_accessor is empty
    static_assert(is_empty_v<Accessor>);

    // Check nested types
    static_assert(same_as<typename Accessor::offset_policy, Accessor>);
    static_assert(same_as<typename Accessor::element_type, ElementType>);
    static_assert(same_as<typename Accessor::reference, ElementType&>);
    static_assert(same_as<typename Accessor::data_handle_type, ElementType*>);

    // Check default constructor
    Accessor accessor;
    static_assert(is_nothrow_default_constructible_v<Accessor>);

    { // Check converting constructor from other accessor
        [[maybe_unused]] default_accessor<const ElementType> const_accessor = accessor;
        static_assert(is_nothrow_constructible_v<default_accessor<const ElementType>, Accessor>);
        static_assert(!is_constructible_v<Accessor, default_accessor<const ElementType>>);
    }

    { // Check 'access' member function
        same_as<ElementType&> decltype(auto) accessed_elem = accessor.access(elems.data(), 1);
        assert(accessed_elem == elems[1]);
        static_assert(noexcept(accessor.access(elems.data(), 0)));
    }

    { // Check 'offset' member function
        same_as<ElementType*> auto ptr = accessor.offset(elems.data(), 1);
        assert(ptr == elems.data() + 1);
        static_assert(noexcept(accessor.offset(elems.data(), 0)));
    }
}

constexpr bool test() {
    test_one<char>({'a', 'b', 'c'});
    test_one<long>({1, 2, 3});
    test_one<double>({1.1, 2.2, 3.3});
    test_one<wstring>({L"1", L"2", L"3"});
    test_one<intmax_t>({3, 2, 1});
    return true;
}

int main() {
#if !(defined(_DEBUG) && defined(__EDG__)) // TRANSITION, VSO-1898962
    static_assert(test());
#endif // ^^^ no workaround ^^^
    test();
}
