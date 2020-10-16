// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <assert.h>
#include <ctype.h>
#include <ios>
#include <system_error>

#ifndef _M_CEE
#include <future>
#endif // _M_CEE

#include <test_windows.hpp>

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
}


// Also test DevDiv-833886 "<system_error>: comparisons should be free functions".
bool test_code(const io_errc l, const error_code& r) {
    return l == r && l != r && l < r;
}

bool test_condition(const errc l, const error_condition& r) {
    return l == r && l != r && l < r;
}
