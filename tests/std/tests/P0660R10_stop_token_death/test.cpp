// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <stop_token>

#include <test_death.hpp>

using namespace std;

struct throwing_functor {
    void operator()() {
        throw 42;
    }
};

void test_case_throw_during_callback_ctor() {
    stop_source source;
    source.request_stop();
    stop_callback cb{source.get_token(), throwing_functor{}};
    (void) cb;
}

void test_case_throw_during_callback_lvalue_ctor() {
    stop_source source;
    source.request_stop();
    auto lvalue_token = source.get_token();
    stop_callback cb{lvalue_token, throwing_functor{}};
    (void) cb;
}

void test_case_throw_during_request_stop() {
    stop_source source;
    stop_callback cb{source.get_token(), throwing_functor{}};
    (void) cb;
    source.request_stop();
}

int main(int argc, char* argv[]) {
    std_testing::death_test_executive exec([] {});

    exec.add_death_tests({
        test_case_throw_during_request_stop,
        test_case_throw_during_callback_lvalue_ctor,
        test_case_throw_during_request_stop,
    });

    return exec.run(argc, argv);
}
