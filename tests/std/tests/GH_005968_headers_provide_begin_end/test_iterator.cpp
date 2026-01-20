// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <iterator>

#include "shared_test.hpp"

struct minimal_container {
    static constexpr size_t magic_value = 3376942;

    constexpr void begin() const {}
    constexpr void end() const {}
    constexpr void cbegin() const {}
    constexpr void cend() const {}
    constexpr void crbegin() const {}
    constexpr void crend() const {}
    constexpr void rbegin() const {}
    constexpr void rend() const {}
    constexpr size_t size() const {
        return magic_value;
    }
    constexpr void empty() const {}
    constexpr void data() const {}
};


// Self-test the template machinery to check it properly detects member functions
namespace detail {
    template <class C>
    constexpr bool minimal_container_test(C& c) {
        // when the CONDITIONALLY_CALL expression fails to detect the member, it has type void, i.e. clearly
        // incompatible with operator==. If the detection mechanism did not work properly, this would fail to compile.
        return CONDITIONALLY_CALL(c, size) == minimal_container::magic_value;
    }

    constexpr minimal_container min_cont;
    static_assert(minimal_container_test(min_cont), "The member detection utility is broken");
} // namespace detail

void test_iterator() {
    minimal_container container;

    shared_test(container);
}
