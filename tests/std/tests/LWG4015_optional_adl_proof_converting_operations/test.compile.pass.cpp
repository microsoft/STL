// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// LWG-4015: "LWG-3973 broke const overloads of std::optional monadic operations"
// This test verifies that optional's converting constructors and assignments
// don't use unqualified operator* which would be vulnerable to ADL hijacking.

#include <optional>
#include <utility>

namespace bad {
    template <class T>
    void operator*(const std::optional<T>&) = delete;

    struct S {
        int val;
        operator int() const {
            return val;
        }
    };
} // namespace bad

void test_converting_construction() {
    const std::optional<bad::S> src_copy{bad::S{42}};
    std::optional<int> d1{src_copy};
    (void) d1;

    std::optional<bad::S> src_move{bad::S{42}};
    std::optional<int> d2{std::move(src_move)};
    (void) d2;
}

void test_converting_assignment() {
    std::optional<int> d;

    const std::optional<bad::S> src_copy{bad::S{42}};
    d = src_copy;

    std::optional<bad::S> src_move{bad::S{42}};
    d = std::move(src_move);
}
