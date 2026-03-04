// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// LWG-4015: "LWG-3973 broke const overloads of std::optional monadic operations"
// This test verifies that optional's operations
// don't use unqualified operator* which would be vulnerable to ADL hijacking.

#include <cstddef>
#include <functional>
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

namespace std {
    template <>
    struct hash<bad::S> {
        [[nodiscard]] std::size_t operator()(const bad::S& obj) const noexcept {
            return std::hash<int>{}(obj);
        }
    };
} // namespace std

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

void test_swap() {
    std::optional<bad::S> left{bad::S{17}};
    std::optional<bad::S> right{bad::S{29}};
    left.swap(right);
}

void test_compare_optional_with_optional() {
    const std::optional<bad::S> left{bad::S{17}};
    const std::optional<bad::S> right{bad::S{29}};

    (void) (left == right);
    (void) (left != right);
    (void) (left < right);
    (void) (left > right);
    (void) (left <= right);
    (void) (left >= right);

#if _HAS_CXX20
    (void) (left <=> right);
#endif
}

void test_compare_optional_with_value() {
    const std::optional<bad::S> left{bad::S{17}};

    (void) (left == 29);
    (void) (left != 29);
    (void) (left < 29);
    (void) (left > 29);
    (void) (left <= 29);
    (void) (left >= 29);

#if _HAS_CXX20
    (void) (left <=> 29);
#endif
}

void test_hash() {
    const std::optional<bad::S> opt{bad::S{1729}};
    std::hash<std::optional<bad::S>> hasher;
    (void) hasher(opt);
}
