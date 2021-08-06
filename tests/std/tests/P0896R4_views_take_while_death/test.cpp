// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _CONTAINER_DEBUG_LEVEL 1

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
    using TWV = decltype(ranges::take_while_view{span<int, 0>{}, test_predicate{}});
    TWV r;
    try {
        r = TWV{};
    } catch (const test_predicate::tag&) {
    }
    return r;
}

void test_view_predicate() {
    auto r = with_no_predicate();
    (void) r.pred(); // take_while_view has no predicate
}

void test_view_end() {
    auto r = with_no_predicate();
    (void) r.end(); // cannot call end on a take_while_view with no predicate
}

void test_view_const_end() {
    const auto r = with_no_predicate();
    (void) r.end(); // cannot call end on a take_while_view with no predicate
}

int main(int argc, char* argv[]) {
    std_testing::death_test_executive exec;

    exec.add_death_tests({
        test_view_predicate,
        test_view_end,
        test_view_const_end,
    });

    return exec.run(argc, argv);
}
