// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstdlib>
#include <exception>
#include <new>

using namespace std;

exception_ptr rvalue() {
    return exception_ptr();
}

void observe(const exception_ptr&) {}

int main() {
    {
        exception_ptr ep1; // DefaultConstructible
        observe(exception_ptr()); // DefaultConstructible

        exception_ptr ep2 = rvalue(); // MoveConstructible
        observe(exception_ptr(rvalue())); // MoveConstructible

        exception_ptr ep3 = ep1; // CopyConstructible
        observe(exception_ptr(ep1)); // CopyConstructible

        ep1 = rvalue(); // MoveAssignable

        ep1 = ep2; // CopyAssignable

        swap(ep1, ep2); // Swappable

        exception_ptr ep4(nullptr); // NullablePointer
        exception_ptr ep5 = nullptr; // NullablePointer
        observe(exception_ptr(nullptr)); // NullablePointer
        ep1 = nullptr; // NullablePointer
        if (ep1 == ep2) { // EqualityComparable
        }
        if (ep1 != ep2) { // NullablePointer
        }
        if (ep1 == nullptr) { // NullablePointer
        }
        if (ep1 != nullptr) { // NullablePointer
        }
        if (nullptr == ep1) { // NullablePointer
        }
        if (nullptr != ep1) { // NullablePointer
        }
        if (ep1) { // NullablePointer
        }
    } // Destructible

    {
        exception_ptr empty;

        exception_ptr full = make_exception_ptr(1729);

        assert(!static_cast<bool>(empty));
        assert(static_cast<bool>(full));

        assert(empty == empty);
        assert(!(empty != empty));

        assert(full == full);
        assert(!(full != full));

        assert(empty == nullptr);
        assert(nullptr == empty);
        assert(!(empty != nullptr));
        assert(!(nullptr != empty));

        assert(!(full == nullptr));
        assert(!(nullptr == full));
        assert(full != nullptr);
        assert(nullptr != full);

        assert(!(empty == full));
        assert(!(full == empty));
        assert(empty != full);
        assert(full != empty);

        exception_ptr empty2 = nullptr;

        assert(empty == empty2);
        assert(empty2 == empty);
        assert(!(empty != empty2));
        assert(!(empty2 != empty));

        exception_ptr full2 = full;

        assert(full == full2);
        assert(full2 == full);
        assert(!(full != full2));
        assert(!(full2 != full));

        exception_ptr different = make_exception_ptr(1729);

        assert(!(full == different));
        assert(!(different == full));
        assert(full != different);
        assert(different != full);


        full = nullptr;

        assert(!static_cast<bool>(full));

        assert(full == empty);
        assert(full != full2);


        full = full2;

        assert(full);
        assert(full != empty);
        assert(full == full2);


        exception_ptr full3 = static_cast<exception_ptr&&>(full);

        assert(full3 != empty);
        assert(full3 == full2);
        assert(full3 != different);


        full = static_cast<exception_ptr&&>(full3);

        assert(full != empty);
        assert(full == full2);
        assert(full != different);
    }

    {
        exception_ptr a = make_exception_ptr(1729);
        exception_ptr b = make_exception_ptr("meow");

        exception_ptr AA = a;
        exception_ptr BB = b;

        assert(a == AA);
        assert(a != BB);

        assert(b != AA);
        assert(b == BB);

        swap(a, b);

        assert(a != AA);
        assert(a == BB);

        assert(b == AA);
        assert(b != BB);

        swap(a, b);

        assert(a == AA);
        assert(a != BB);

        assert(b != AA);
        assert(b == BB);

        swap(a, a);

        assert(a == AA);
        assert(a != BB);

        assert(b != AA);
        assert(b == BB);
    }

    {
        // Also test DevDiv-1210471 "std::rethrow_exception is not [[noreturn]]".

        auto lambda = []() -> double { rethrow_exception(make_exception_ptr(1729)); };

        try {
            const double d = lambda();
            (void) d;
            abort();
        } catch (const int i) {
            assert(i == 1729);
        }
    }
}
