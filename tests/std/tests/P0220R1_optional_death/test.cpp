// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <optional>
#include <utility>

#include <test_death.hpp>

using namespace std;

struct S {
    int value;
};

void test_nullopt_operator_arrow() {
    optional<S> o;
    (void) o->value;
}

void test_nullopt_operator_arrow_const() {
    const optional<S> o;
    (void) o->value;
}

void test_nullopt_operator_star_lvalue() {
    optional<S> o;
    (void) *o;
}

void test_nullopt_operator_star_const_lvalue() {
    const optional<S> o;
    (void) *o;
}

void test_nullopt_operator_star_rvalue() {
    optional<S> o;
    (void) *move(o);
}

void test_nullopt_operator_star_const_rvalue() {
    const optional<S> o;
    (void) *move(o);
}

int main(int argc, char* argv[]) {
    std_testing::death_test_executive exec;

#if _ITERATOR_DEBUG_LEVEL != 0
    exec.add_death_tests({
        test_nullopt_operator_arrow,
        test_nullopt_operator_arrow_const,
        test_nullopt_operator_star_lvalue,
        test_nullopt_operator_star_const_lvalue,
        test_nullopt_operator_star_rvalue,
        test_nullopt_operator_star_const_rvalue,
    });
#endif // _ITERATOR_DEBUG_LEVEL != 0

    return exec.run(argc, argv);
}
