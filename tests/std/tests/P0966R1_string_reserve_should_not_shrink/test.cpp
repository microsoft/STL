// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _SILENCE_CXX20_STRING_RESERVE_WITHOUT_ARGUMENT_DEPRECATION_WARNING

#include <cassert>
#include <string>

using namespace std;

int main() {
    // N4830 [string.capacity]/9:
    // "constexpr void reserve(size_type res_arg);
    // Effects: A directive that informs a basic_string of a planned change in size, so that the storage
    // allocation can be managed accordingly. After reserve(), capacity() is greater or equal to the
    // argument of reserve if reallocation happens; and equal to the previous value of capacity() otherwise.
    // Reallocation happens at this point if and only if the current capacity is less than the argument of reserve()."

    // [depr.string.capacity]/2:
    // "void reserve();
    // Effects: After this call, capacity() has an unspecified value greater than or equal to size().
    // [Note: This is a non-binding shrink to fit request. -end note]"

    // We've preserved our implementation's pre-C++20 behavior for reserve() without an argument,
    // performing a shrink to fit for empty strings in large mode (i.e. owning dynamically allocated memory).

    // Note that the exact values of capacity() are implementation-specific.

    const char* const large = "This is a very large string. Cute fluffy kittens. Meow! Purr!";

    string s(large);
    s.append(1000, '-');
    s.erase(55);

    assert(s.size() == 55);
    assert(s.capacity() == 1071);
    s.reserve(55); // reserve(res_arg) won't shrink from large to less-large
    assert(s.size() == 55);
    assert(s.capacity() == 1071);
    s.reserve(); // reserve() won't shrink from large to less-large
    assert(s.size() == 55);
    assert(s.capacity() == 1071);

    s.erase(4);

    assert(s.size() == 4);
    assert(s.capacity() == 1071);
#if _HAS_CXX20
    s.reserve(4); // reserve(res_arg) won't shrink from large to small in C++20
    assert(s.size() == 4);
    assert(s.capacity() == 1071);
#endif // _HAS_CXX20
    s.reserve(); // reserve() won't shrink from large to small
    assert(s.size() == 4);
    assert(s.capacity() == 1071);

    s.clear();

    assert(s.size() == 0);
    assert(s.capacity() == 1071);
#if _HAS_CXX20
    s.reserve(0); // reserve(res_arg) won't shrink from large to empty in C++20
    assert(s.size() == 0);
    assert(s.capacity() == 1071);
#endif // _HAS_CXX20
    s.reserve(); // reserve() WILL shrink from large to empty
    assert(s.size() == 0);
    assert(s.capacity() == 15);

#if !_HAS_CXX20
    s.assign(large);
    s.append(1000, '-');
    s.erase(4);

    assert(s.size() == 4);
    assert(s.capacity() == 1071);
    s.reserve(4); // reserve(res_arg) WILL shrink from large to small in our pre-C++20 mode
    assert(s.size() == 4);
    assert(s.capacity() == 15);

    s.assign(large);
    s.append(1000, '-');
    s.clear();

    assert(s.size() == 0);
    assert(s.capacity() == 1071);
    s.reserve(0); // reserve(res_arg) WILL shrink from large to empty in our pre-C++20 mode
    assert(s.size() == 0);
    assert(s.capacity() == 15);
#endif // !_HAS_CXX20

    s.assign("small");

    assert(s.size() == 5);
    assert(s.capacity() == 15);
    s.reserve(5); // reserve(res_arg) won't shrink when small
    assert(s.size() == 5);
    assert(s.capacity() == 15);
    s.reserve(); // reserve() won't shrink when small
    assert(s.size() == 5);
    assert(s.capacity() == 15);

    s.clear();

    assert(s.size() == 0);
    assert(s.capacity() == 15);
    s.reserve(0); // reserve(res_arg) won't shrink when small and empty
    assert(s.size() == 0);
    assert(s.capacity() == 15);
    s.reserve(); // reserve() won't shrink when small and empty
    assert(s.size() == 0);
    assert(s.capacity() == 15);

    // Test reserve(res_arg) with res_arg >= capacity():
    s.assign("cat");
    assert(s.size() == 3);
    assert(s.capacity() == 15);
    s.reserve(15); // same capacity when small, no change
    assert(s.size() == 3);
    assert(s.capacity() == 15);
    s.reserve(16); // increase capacity from small to large
    assert(s.size() == 3);
    assert(s.capacity() == 31);
    s.reserve(31); // same capacity when large, no change
    assert(s.size() == 3);
    assert(s.capacity() == 31);
    s.reserve(1000); // increase capacity "exactly" (with a bit of rounding)
    assert(s.size() == 3);
    assert(s.capacity() == 1007);
    s.reserve(1008); // increase capacity geometrically
    assert(s.size() == 3);
    assert(s.capacity() == 1510);
}
