// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <array>
#include <cassert>
#include <concepts>
#include <cstdint>
#include <mdspan>
#include <string>
#include <type_traits>

using namespace std;

template <class ElementType>
constexpr void test_one(array<ElementType, 3> elems) {
    using DefaultAccessor = default_accessor<ElementType>;

    // Check modeled concepts
    static_assert(is_nothrow_move_constructible_v<DefaultAccessor>);
    static_assert(is_nothrow_move_assignable_v<DefaultAccessor>);
    static_assert(is_nothrow_swappable_v<DefaultAccessor>);
    static_assert(is_trivially_copyable_v<DefaultAccessor>);
    static_assert(semiregular<DefaultAccessor>);

    // Check nested types
    static_assert(same_as<typename DefaultAccessor::offset_policy, DefaultAccessor>);
    static_assert(same_as<typename DefaultAccessor::element_type, ElementType>);
    static_assert(same_as<typename DefaultAccessor::reference, ElementType&>);
    static_assert(same_as<typename DefaultAccessor::data_handle_type, ElementType*>);

    // Check default constructor
    DefaultAccessor accessor;
    static_assert(is_nothrow_default_constructible_v<DefaultAccessor>);

    { // Check converting constructor from other accessor
        [[maybe_unused]] default_accessor<const ElementType> const_accessor = accessor;
        static_assert(is_nothrow_constructible_v<default_accessor<const ElementType>, DefaultAccessor>);
        static_assert(!is_constructible_v<DefaultAccessor, default_accessor<const ElementType>>);
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
    static_assert(test());
    test();
}
