// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstddef>
#include <ranges>
#include <span>

#include <test_death.hpp>
using namespace std;

struct test_predicate {
    struct tag {};

    test_predicate() = default;
    test_predicate(const test_predicate&) {
        throw tag{};
    }
    test_predicate& operator=(const test_predicate&) = delete;

    constexpr bool operator()(int i) const {
        return i == 42;
    }
};

auto with_no_predicate() {
    using DWV = decltype(ranges::drop_while_view{span<int, 0>{}, test_predicate{}});
    DWV r;
    try {
        r = DWV{};
    } catch (const test_predicate::tag&) {
    }
    return r;
}

void test_view_predicate() {
    auto r = with_no_predicate();
    (void) r.pred(); // drop_while_view has no predicate
}

void test_view_begin() {
    auto r = with_no_predicate();
    (void) r.begin(); // N4885 [range.drop.while.view] forbids calling begin on a drop_while_view with no predicate
}

int main(int argc, char* argv[]) {
    std_testing::death_test_executive exec;

#if _ITERATOR_DEBUG_LEVEL != 0
    exec.add_death_tests({
        test_view_predicate,
        test_view_begin,
    });
#endif // _ITERATOR_DEBUG_LEVEL != 0

    return exec.run(argc, argv);
}
