// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Intentially declare variables of these names before the inclusion of standard headers. See LWG-3629.
struct InvalidFunctor {
    template <class T>
    void operator()(T&&) = delete;
};

constexpr InvalidFunctor make_error_code{};
constexpr InvalidFunctor make_error_condition{};

#include <cassert>
#include <cctype>
#include <ios>
#include <system_error>

#ifndef _M_CEE_PURE
#include <future>
#endif // _M_CEE_PURE

#include <Windows.h>

using namespace std;

struct Global {
    ~Global() {
        (void) generic_category().name();
        (void) iostream_category().name();
        (void) system_category().name();

#ifndef _M_CEE_PURE
        (void) future_category().name();
#endif // _M_CEE_PURE
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

// Also test LWG-3629 make_error_code and make_error_condition are customization points
namespace test_ns {
    struct friendly_error {
        friend std::error_code make_error_code(friendly_error) {
            return std::error_code{};
        }

        friend std::error_condition make_error_condition(friendly_error) {
            return std::error_condition{};
        }
    };

    struct converted_errc : std::enable_if<false> {
        operator std::errc() const {
            return std::errc{};
        }
    };

    struct converted_io_errc : std::enable_if<false> {
        operator std::io_errc() const {
            return std::io_errc{};
        }
    };
} // namespace test_ns

template <>
struct std::is_error_code_enum<test_ns::friendly_error> : std::true_type {};

template <>
struct std::is_error_code_enum<test_ns::converted_io_errc> : std::true_type {};

template <>
struct std::is_error_condition_enum<test_ns::friendly_error> : std::true_type {};

template <>
struct std::is_error_condition_enum<test_ns::converted_errc> : std::true_type {};

void test_lwg_3629() {
#ifndef _M_CEE_PURE
    std::error_code err_future{std::future_errc{}};
    (void) err_future;
#endif // _M_CEE_PURE
    std::error_code err_io{std::io_errc{}};
    (void) err_io;
    std::error_condition errcond{std::errc{}};
    (void) errcond;

    std::error_code ec_friendly{test_ns::friendly_error{}};
    (void) ec_friendly;
    std::error_code ec_converted_io{test_ns::converted_io_errc{}};
    (void) ec_converted_io;
    std::error_condition econd_friendly{test_ns::friendly_error{}};
    (void) econd_friendly;
    std::error_condition econd_converted{test_ns::converted_errc{}};
    (void) econd_converted;
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
    test_lwg_3629();

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
