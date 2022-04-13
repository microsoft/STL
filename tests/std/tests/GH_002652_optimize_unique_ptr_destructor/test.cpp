// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Test the optimization required by GH-2652, which avoids redundantly
// checking unique_ptr's stored pointer is non-null before passing it to
// `delete`.

#include <cassert>
#include <memory>

template <int>
struct S {
    static bool called;
};

template <int I>
bool S<I>::called = false;

template <>
struct std::default_delete<S<0>> {
    using _Library_defined = default_delete; // Lies!

    void operator()(S<0>* ptr) const noexcept {
        assert(ptr == nullptr);
        S<0>::called = true;
    }
};

template <>
struct std::default_delete<S<1>> {
    void operator()(S<1>* ptr) const noexcept {
        assert(ptr != nullptr);
        S<1>::called = true;
    }
};

template <>
struct std::default_delete<S<0>[]> {
    using _Library_defined = default_delete; // Lies!

    void operator()(S<0>* ptr) const noexcept {
        assert(ptr == nullptr);
        S<0>::called = true;
    }
};

template <>
struct std::default_delete<S<1>[]> {
    void operator()(S<1>* ptr) const noexcept {
        assert(ptr != nullptr);
        S<1>::called = true;
    }
};

int main() {
    std::unique_ptr<S<0>>{};
    assert(S<0>::called);
    std::unique_ptr<S<1>>{};
    assert(!S<1>::called);

    S<0>::called = S<1>::called = false;

    std::unique_ptr<S<0>[]> {};
    assert(S<0>::called);
    std::unique_ptr<S<1>[]> {};
    assert(!S<1>::called);
}
