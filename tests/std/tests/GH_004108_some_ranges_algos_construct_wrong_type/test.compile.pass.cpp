// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cstddef>
#include <ranges>

using namespace std;

namespace gh4108 {
    struct RRef {
        RRef(RRef&&) = delete;
    };

    struct Val {
        Val(const RRef&);
        auto operator<=>(const Val&) const = default;
    };

    struct I {
        using value_type      = Val;
        using difference_type = ptrdiff_t;
        Val& operator*() const;
        I& operator++();
        I operator++(int);
        I& operator--();
        I operator--(int);
        bool operator==(const I&) const;
        friend RRef&& iter_move(const I&);
    };

    // GH-4108 <algorithm>: ranges::inplace_merge accidentally constructed remove_cvref_t<iter_rvalue_reference_t<I>>
    void test_gh_4108() {
        ranges::inplace_merge(I{}, I{}, I{});
    }
} // namespace gh4108

namespace gh4102 {
    struct Ref {
        Ref(const Ref&)                    = delete;
        auto operator<=>(const Ref&) const = default;
    };

    struct Val {
        Val() = default;
        Val(const Ref&);
        auto operator<=>(const Val&) const = default;
    };

    struct I {
        using value_type      = Val;
        using difference_type = ptrdiff_t;
        const Ref& operator*() const;
        I& operator++();
        void operator++(int);
        bool operator==(const I&) const;
    };

    // GH-4102 <algorithm>: ranges::minmax accidentally constructed remove_cvref_t<iter_reference_t<I>>
    void test_gh_4102() {
        [[maybe_unused]] auto result = ranges::minmax(ranges::subrange<I, I>{});
    }
} // namespace gh4102
