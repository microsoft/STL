// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstring>
#include <exception>
#include <stdexcept>

using namespace std;

struct A {
    ~A() noexcept {
        assert(uncaught_exceptions() == 2);
        try {
            assert(uncaught_exceptions() == 2);
            throw runtime_error("say what?");
        } catch (const exception&) {
            assert(uncaught_exceptions() == 2);
            auto c = current_exception();
            try {
                assert(uncaught_exceptions() == 2);
                rethrow_exception(c);
            } catch (const runtime_error& e) {
                assert(uncaught_exceptions() == 2);
                assert(strcmp("say what?", e.what()) == 0);
            }

            assert(uncaught_exceptions() == 2);

            try {
                assert(uncaught_exceptions() == 2);
                throw;
            } catch (const runtime_error& e) {
                assert(uncaught_exceptions() == 2);
                assert(strcmp("say what?", e.what()) == 0);
            }
        }
    }
};

struct B {
    ~B() noexcept {
        assert(uncaught_exceptions() == 1);
        try {
            assert(uncaught_exceptions() == 1);
            A aa;
            throw runtime_error("oh no!");
        } catch (exception&) {
            assert(uncaught_exceptions() == 1);
            auto c = current_exception();
            try {
                assert(uncaught_exceptions() == 1);
                rethrow_exception(c);
            } catch (const runtime_error& e) {
                assert(uncaught_exceptions() == 1);
                assert(strcmp("oh no!", e.what()) == 0);
            }

            assert(uncaught_exceptions() == 1);

            try {
                assert(uncaught_exceptions() == 1);
                throw;
            } catch (const runtime_error& e) {
                assert(uncaught_exceptions() == 1);
                assert(strcmp("oh no!", e.what()) == 0);
            }
        }
    }
};

void meow() {
    B bb;
    throw runtime_error("oh no");
}

int main() {
    try {
        meow();
    } catch (...) {
        assert(uncaught_exceptions() == 0);
        assert(current_exception());
    }
}
