// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <exception>
#include <excpt.h>

#include <Windows.h>

ULONG_PTR data[] = {0xC0FFEE, 1729};

struct LifetimeTracker {
    static size_t thisAlive;

    static void AssertAlive(const size_t expected) {
        assert(thisAlive == expected);
    }

    LifetimeTracker() {
        ++thisAlive;
    }

    LifetimeTracker(const LifetimeTracker&) {
        ++thisAlive;
    }

    LifetimeTracker& operator=(const LifetimeTracker&) = default;

    ~LifetimeTracker() {
        --thisAlive;
    }
};

size_t LifetimeTracker::thisAlive = 0;

struct EvilException : LifetimeTracker {
    EvilException() = default;
    EvilException(const EvilException&) {
        RaiseException(1234, EXCEPTION_NONCONTINUABLE, 2, data);
    }
};

void AssertExceptionRecordOk(const EXCEPTION_POINTERS* const pointers) {
    assert(pointers->ExceptionRecord->ExceptionCode == 1234);
    assert((pointers->ExceptionRecord->ExceptionFlags & EXCEPTION_NONCONTINUABLE) == EXCEPTION_NONCONTINUABLE);
    assert(pointers->ExceptionRecord->ExceptionRecord == nullptr);
    assert(pointers->ExceptionRecord->ExceptionAddress != nullptr);
    assert(pointers->ExceptionRecord->NumberParameters == 2);
    assert(pointers->ExceptionRecord->ExceptionInformation[0] == data[0]);
    assert(pointers->ExceptionRecord->ExceptionInformation[1] == data[1]);
}

void Rethrow(const std::exception_ptr& ptr) {
    // separate function to allow use in functions with __try/__except
    std::rethrow_exception(ptr);
}

void AssertRethrowNoContinuationOK(const std::exception_ptr& ex) {
    __try {
        Rethrow(ex);
    } __except (AssertExceptionRecordOk(GetExceptionInformation()), EXCEPTION_EXECUTE_HANDLER) {
    }
}

int main() {
    try {
        throw EvilException();
    } catch (...) {
        AssertRethrowNoContinuationOK(std::current_exception());
    }

    LifetimeTracker::AssertAlive(0); // if this assert fails, double check that you built with /EHa
}
