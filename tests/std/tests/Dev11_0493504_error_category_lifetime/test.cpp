// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cctype>
#include <ios>
#include <system_error>

#ifndef _M_CEE
#include <future>
#endif // _M_CEE

#include <Windows.h>

using namespace std;

struct Global {
    ~Global() {
        (void) generic_category().name();
        (void) iostream_category().name();
        (void) system_category().name();

#ifndef _M_CEE
        (void) future_category().name();
#endif // _M_CEE
    }
};

Global global;

// Also test LWG-3598: system_category().default_error_condition(0) has a generic category.
void test_lwg_3598() {
    error_condition cond = system_category().default_error_condition(0);

    assert(cond.category() == generic_category());
    assert(cond.message() == "success");
    assert(cond.value() == 0);

    assert(error_code() == error_condition());
}

// Also test GH-2572: WAIT_TIMEOUT is not matched against by std::errc::timed_out
void test_gh_2572() {
    assert((errc::timed_out == error_code{WAIT_TIMEOUT, system_category()}));
    assert((make_error_condition(errc::timed_out) == error_code{WAIT_TIMEOUT, system_category()}));

    assert((errc::timed_out == error_code{ERROR_TIMEOUT, system_category()}));
    assert((make_error_condition(errc::timed_out) == error_code{ERROR_TIMEOUT, system_category()}));

    assert((errc::timed_out == error_code{ERROR_SEM_TIMEOUT, system_category()}));
    assert((make_error_condition(errc::timed_out) == error_code{ERROR_SEM_TIMEOUT, system_category()}));
}

// Also test GH-2893 <system_error>: Several Windows system errors are not mapped
void test_gh_2893() {
    assert((errc::filename_too_long == error_code{ERROR_FILENAME_EXCED_RANGE, system_category()}));
    assert(
        (make_error_condition(errc::filename_too_long) == error_code{ERROR_FILENAME_EXCED_RANGE, system_category()}));

    assert((errc::no_such_file_or_directory == error_code{ERROR_BAD_NET_NAME, system_category()}));
    assert(
        (make_error_condition(errc::no_such_file_or_directory) == error_code{ERROR_BAD_NET_NAME, system_category()}));
}

int main() {
    // Also test DevDiv-781294 "<system_error>: Visual C++ 2013 RC system_category().equivalent function does not work".
    const error_code code(ERROR_NOT_ENOUGH_MEMORY, system_category());

    const error_condition cond = make_error_condition(errc::not_enough_memory);

    assert(code == cond);

    assert(system_category().equivalent(ERROR_NOT_ENOUGH_MEMORY, cond));

    assert(system_category().default_error_condition(ERROR_DIR_NOT_EMPTY)
           == make_error_condition(errc::directory_not_empty));

    // Also test VSO-649432 <system_error> system_category().message() ends with extra newlines
    const auto msg = code.message();
    assert(!msg.empty() && msg.back() != '\0' && !isspace(static_cast<unsigned char>(msg.back())));

    test_lwg_3598();

    test_gh_2572();
    test_gh_2893();
}


// Also test DevDiv-833886 "<system_error>: comparisons should be free functions".
bool test_code(const io_errc l, const error_code& r) {
    return l == r && l != r && l < r;
}

bool test_condition(const errc l, const error_condition& r) {
    return l == r && l != r && l < r;
}
