// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// DevDiv-1131212:  std::uncaught_exceptions is not implemented
//
// This test validates the implementation of std::uncaught_exceptions by recursively
// throwing exceptions from a destructor.

#include <cassert>
#include <exception>

class awesome_exception {};

class check_uncaught_exceptions_count {
public:
    explicit check_uncaught_exceptions_count(int const remaining, int const processing = 0)
        : _remaining(remaining), _processing(processing) {}

    ~check_uncaught_exceptions_count() {
        if (_remaining != 0) {
            try {
                check_uncaught_exceptions_count const test(_remaining - 1, _processing + 1);
                throw awesome_exception();
            } catch (awesome_exception const&) {
            }
        }

        assert(_processing == std::uncaught_exceptions());
    }

private:
    check_uncaught_exceptions_count(check_uncaught_exceptions_count const&)            = delete;
    check_uncaught_exceptions_count& operator=(check_uncaught_exceptions_count const&) = delete;

    int _remaining;
    int _processing;
};

int main() {
    // Verify the return value of std::uncaught_exceptions() when between 0 and
    // 100 exceptions are being unwound:
    check_uncaught_exceptions_count(10);
}
