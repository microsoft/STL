// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstddef>
#include <exception>

using namespace std;

struct BasicLifetimeTracker {
    static size_t allAlive;
};

size_t BasicLifetimeTracker::allAlive = 0;

template <class T>
struct LifetimeTracker : BasicLifetimeTracker {
    static size_t thisAlive;

    static void AssertAlive(const size_t expected) {
        assert(thisAlive == expected);
    }

    LifetimeTracker() {
        ++thisAlive;
        ++allAlive;
    }

    LifetimeTracker(const LifetimeTracker&) {
        ++thisAlive;
        ++allAlive;
    }

    LifetimeTracker& operator=(const LifetimeTracker&) = default;

    ~LifetimeTracker() {
        --thisAlive;
        --allAlive;
    }
};

template <class T>
size_t LifetimeTracker<T>::thisAlive = 0;

template <size_t NestLevel>
struct EvilException : LifetimeTracker<EvilException<NestLevel>> {
    EvilException() = default;
    EvilException(const EvilException&) {
        throw EvilException<NestLevel - 1>();
    }
};

template <>
struct EvilException<0> : LifetimeTracker<EvilException<0>> {};

// The asserts about number of exceptions alive in this test are MSVC ABI assumptions:
// We copy the exception object in throw (as mandated by the standard), current_exception (allowed by the standard),
// and rethrow_exception (unclear compliance with the standard; P1675 looks to make it allowed)

int main() {
    try {
        throw EvilException<0>(); // copy 1
    } catch (...) {
        try {
            rethrow_exception( // copy 3
                current_exception() // copy 2
            );
        } catch (const EvilException<0>&) { // unwind destroys copy 2
            LifetimeTracker<EvilException<0>>::AssertAlive(2);
        }
    }
    assert(BasicLifetimeTracker::allAlive == 0);

    {
        exception_ptr ptr;
        try {
            throw EvilException<0>(); // copy 1
        } catch (...) {
            try {
                ptr = current_exception(); // copy 2
                rethrow_exception(ptr); // copy 3
            } catch (const EvilException<0>&) {
                LifetimeTracker<EvilException<0>>::AssertAlive(3);
            }
        }
        LifetimeTracker<EvilException<0>>::AssertAlive(1); // *ptr
    }

    assert(BasicLifetimeTracker::allAlive == 0);

    try {
        throw EvilException<1>(); // copy 1
    } catch (...) {
        try {
            rethrow_exception( // EvilException<0> copy 2
                current_exception() // attempt copy 2, fails, making EvilException<0> copy 1
            );
        } catch (const EvilException<0>&) { // unwind destroys EvilException<0> copy 2
            LifetimeTracker<EvilException<0>>::AssertAlive(1);
            LifetimeTracker<EvilException<1>>::AssertAlive(1);
        }
    }
    assert(BasicLifetimeTracker::allAlive == 0);

    try {
        throw EvilException<2>(); // copy 1
    } catch (...) {
        try {
            rethrow_exception(current_exception() // attempt copy 2, fails,
                                                  // making EvilException<1> copy 1,
                                                  // copying that fails, making bad_exception
            );
        } catch (const bad_exception&) {
            LifetimeTracker<EvilException<0>>::AssertAlive(0);
            LifetimeTracker<EvilException<1>>::AssertAlive(0);
            LifetimeTracker<EvilException<2>>::AssertAlive(1);
        }
    }
    assert(BasicLifetimeTracker::allAlive == 0);
}
