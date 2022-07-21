// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <excpt.h>

#include <Windows.h>

ULONG_PTR data[] = {0xC0FFEE, 1729};

std::exception_ptr GetNoncontinuableException() {
    try {
        RaiseException(1234, EXCEPTION_NONCONTINUABLE, 2, data);
    } catch (...) {
        return std::current_exception(); // depends on /EHa
    }

    puts("Test failed; you probably didn't compile with /EHa");
    abort();
}

std::exception_ptr GetContinuableException() {
    try {
        RaiseException(1234, 0, 2, data);
    } catch (...) {
        return std::current_exception(); // depends on /EHa
    }

    puts("Test failed; you probably didn't compile with /EHa");
    abort();
}

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

DWORD RethrowContinuationFilter(int& phase, const EXCEPTION_POINTERS* const pointers) {
    switch (phase++) {
    case 0:
        AssertExceptionRecordOk(pointers);
        return static_cast<DWORD>(EXCEPTION_CONTINUE_EXECUTION);
    case 1:
        assert(pointers->ExceptionRecord->ExceptionCode == EXCEPTION_NONCONTINUABLE_EXCEPTION);
        return static_cast<DWORD>(EXCEPTION_EXECUTE_HANDLER);
    default:
        abort();
    }
}

void AssertRethrowContinuationFails(const std::exception_ptr& ex) {
    int phase = 0;
    __try {
        Rethrow(ex);
        abort();
    } __except (RethrowContinuationFilter(phase, GetExceptionInformation())) {
    }
}

int main() {
    AssertRethrowNoContinuationOK(GetNoncontinuableException());
    AssertRethrowNoContinuationOK(GetContinuableException());
    AssertRethrowContinuationFails(GetNoncontinuableException());
    AssertRethrowContinuationFails(GetContinuableException());
}
