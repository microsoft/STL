// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <memory>
#include <ranges>
#include <utility>
#include <vector>

using namespace std;

void test() {
    auto ints                  = views::iota(0, 5);
    auto project_to_unique_ptr = []<movable T>(T v) { return make_unique<T>(move(v)); };

    using It          = ranges::iterator_t<decltype(ints)>;
    using Proj        = decltype(project_to_unique_ptr);
    using ProjectedIt = projected<It, Proj>;

    { // Check indirectly_unary_invocable
        auto consume = [](auto) {};
        static_assert(indirectly_unary_invocable<decltype(consume), ProjectedIt>);

        ranges::for_each(ints, consume, project_to_unique_ptr);
        ranges::for_each(ints.begin(), ints.end(), consume, project_to_unique_ptr);
    }

    { // Check indirectly_regular_unary_invocable
        static_assert(indirectly_regular_unary_invocable<decltype([](auto) {}), ProjectedIt>);
        using Check [[maybe_unused]] = projected<ProjectedIt, Proj>;
    }

    { // Check indirect_unary_predicate
        auto unary_pred = [](auto) { return false; };
        static_assert(indirect_unary_predicate<decltype(unary_pred), ProjectedIt>);

        (void) ranges::find_if(ints, unary_pred, project_to_unique_ptr);
        (void) ranges::find_if(ints.begin(), ints.end(), unary_pred, project_to_unique_ptr);
        (void) ranges::count_if(ints, unary_pred, project_to_unique_ptr);
        (void) ranges::count_if(ints.begin(), ints.end(), unary_pred, project_to_unique_ptr);
    }

    { // Check indirect_binary_predicate
        auto binary_pred = [](auto, auto) { return false; };
        static_assert(indirect_binary_predicate<decltype(binary_pred), ProjectedIt, ProjectedIt>);

        (void) ranges::adjacent_find(ints, binary_pred, project_to_unique_ptr);
        (void) ranges::adjacent_find(ints.begin(), ints.end(), binary_pred, project_to_unique_ptr);
    }

    { // Check indirect_equivalence_relation
        auto rel = [](auto, auto) { return false; };
        static_assert(indirect_equivalence_relation<decltype(rel), ProjectedIt>);

        vector<int> out;
        (void) ranges::unique_copy(ints, back_inserter(out), rel, project_to_unique_ptr);
        (void) ranges::unique_copy(ints.begin(), ints.end(), back_inserter(out), rel, project_to_unique_ptr);
    }

    { // Check indirect_strict_weak_order
        auto rel = [](auto x, auto y) { return x < y; };
        static_assert(indirect_strict_weak_order<decltype(rel), ProjectedIt>);

        (void) ranges::is_sorted_until(ints, rel, project_to_unique_ptr);
        (void) ranges::is_sorted_until(ints.begin(), ints.end(), rel, project_to_unique_ptr);
    }
}
