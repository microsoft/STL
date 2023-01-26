// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <concepts>
#include <iterator>
#include <span>
#include <type_traits>
#include <utility>

using namespace std;

enum class Const : bool { no, yes };
enum class Dynamic : bool { no, yes };

static_assert(!to_underlying(Const::no) && to_underlying(Const::yes));
static_assert(!to_underlying(Dynamic::no) && to_underlying(Dynamic::yes));

template <Const IsConst, Dynamic IsDynamic>
constexpr bool test() {
    using T      = conditional_t<to_underlying(IsConst), const int, int>;
    using Span   = span<T, to_underlying(IsDynamic) ? dynamic_extent : 3>;
    using CIt    = typename Span::const_iterator;
    using CRevIt = typename Span::const_reverse_iterator;

    // Validate iterator properties
    static_assert(contiguous_iterator<CIt>);
    static_assert(random_access_iterator<CRevIt>);
    static_assert(same_as<CIt, const_iterator<typename Span::iterator>>);
    static_assert(same_as<CRevIt, const_iterator<typename Span::reverse_iterator>>);
    static_assert(same_as<CIt, const_iterator<CIt>>);
    static_assert(same_as<CRevIt, const_iterator<CRevIt>>);
    static_assert(same_as<iter_value_t<CIt>, remove_cv_t<T>>);
    static_assert(same_as<iter_value_t<CRevIt>, remove_cv_t<T>>);
    static_assert(same_as<iter_reference_t<CIt>, const T&>);
    static_assert(same_as<iter_reference_t<CRevIt>, const T&>);

    T elems[3] = {1, 22, 333};
    const Span s{elems};

    { // Validate span::cbegin
        same_as<CIt> auto it = s.cbegin();
        assert(it == s.begin());
        assert(*it == 1);
        static_assert(noexcept(s.cbegin()));
    }

    { // Validate span::cend
        same_as<CIt> auto it = s.cend();
        assert(it == s.end());
        assert(it[-1] == 333);
        static_assert(noexcept(s.cend()));
    }

    { // Validate span::crbegin
        same_as<CRevIt> auto it = s.crbegin();
        assert(it == s.rbegin());
        assert(*it == 333);
        static_assert(noexcept(s.crbegin()));
    }

    { // Validate span::crend
        same_as<CRevIt> auto it = s.crend();
        assert(it == s.rend());
        assert(it[-1] == 1);
        static_assert(noexcept(s.crend()));
    }

    return true;
}

int main() {
    static_assert(test<Const::no, Dynamic::no>());
    static_assert(test<Const::yes, Dynamic::no>());
    static_assert(test<Const::no, Dynamic::yes>());
    static_assert(test<Const::yes, Dynamic::yes>());

    test<Const::no, Dynamic::no>();
    test<Const::yes, Dynamic::no>();
    test<Const::no, Dynamic::yes>();
    test<Const::yes, Dynamic::yes>();
}
