// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// DevDiv Bugs.12361 - "VS2005 Checked STL vector::swap causes incorrect iterator assertion in Release builds"

// C++03 23.1/10 requires that "all container types defined in this clause meet
// the following additional requirements: [...] no swap() function invalidates
// any references, pointers, or iterators referring to the elements of the
// containers being swapped."

// This was broken when _HAS_ITERATOR_DEBUGGING was disabled and _SECURE_SCL
// was enabled (the default in release mode, obtainable in debug mode).
// In this case, every iterator held a pointer to its parent container (in
// addition to the element being referred to) so that it could perform bounds
// checking. However, swapping containers would not update these parent
// pointers, so using iterators into those containers would perform bounds
// checks against the wrong containers. (Vector iterators perform bounds checks
// based on pointers, not indices, so they were broken even when the containers
// were the same size.) Also, one container could be destroyed before the
// other, wreaking havoc even more reliably.

// This afflicted all Standard containers (vector/deque/list, set/multiset,
// map/multimap, and string). When both _HAS_ITERATOR_DEBUGGING and _SECURE_SCL
// were enabled, swap() worked properly because the iterator debugging
// machinery walked through all iterators to update their parent container
// pointers. When both _HAS_ITERATOR_DEBUGGING and _SECURE_SCL were disabled,
// there were no parent container pointers to get mangled, with one exception:

// deque was even more broken. Due to its map-of-blocks structure, deque
// iterators maintained parent container pointers even when both
// _HAS_ITERATOR_DEBUGGING and _SECURE_SCL were disabled. These were also
// broken by swap().

// The fix for both of these problems was to introduce a dynamically allocated
// object, called the aux object, owned by every container when
// _HAS_ITERATOR_DEBUGGING is disabled and _SECURE_SCL is enabled (and
// additionally by deque when both _HAS_ITERATOR_DEBUGGING and _SECURE_SCL are
// disabled). An iterator now holds a pointer to an aux object, which holds a
// pointer to a container. This extra level of indirection solves the swap()
// problem; the containers and the aux objects do not change addresses, but
// the containers swap ownership of their aux objects, and the aux objects are
// made to point to their new owners, allowing iterators to find their parents
// correctly.

// Note, however, that this fix has NOT been applied to strings, because of the
// complexity of how they are passed between the separately compiled runtime
// and the client code.

// The aux object is allocated by the container's allocator due to a subtle
// issue. When creating a precompiled header, the compiler snapshots its
// internal data structures to disk. These internal data structures include
// STL containers, which use a special allocator to support this snapshotting.
// If the aux object is simply allocated by "new", then it will be missing
// when the PCH is reloaded.

// This fix has the following effects on container and iterator sizes
// (in 32-bit mode):
// 1. Non-string non-deque containers are 4 bytes larger in release mode with
// _SECURE_SCL enabled (as they must own an aux object now).
// 2. Non-string non-deque containers are the same size in debug mode with
// _HAS_ITERATOR_DEBUGGING disabled. Although they own an aux object now,
// an unnecessary data member was removed, keeping them the same size.
// 3. Non-string non-deque iterators are the same size in release mode with
// _SECURE_SCL enabled (their parent container pointer was replaced with an aux
// object pointer).
// 4. Non-string non-deque iterators are 4 bytes smaller in debug mode with
// _HAS_ITERATOR_DEBUGGING disabled. They gained an aux object pointer, but
// lost two unnecessary data members.

// 5. Deque iterators are 4 bytes smaller in release mode with _SECURE_SCL
// enabled (a duplicate data member was removed).
// 6. Deque iterators are 8 bytes smaller in debug mode with
// _HAS_ITERATOR_DEBUGGING disabled (three data members were removed, and an
// aux object pointer was added).

// 7. Deques are 8 bytes larger in release mode (regardless of _SECURE_SCL).
// This is because they must own an aux object now, although I don't know why
// this costs 8 bytes instead of 4. There seems to be padding involved.

// To summarize object sizes in the order,
// Debug HID/SCL, Debug SCL, Debug None, Release SCL, Release None:
// vector<int>: Previously 20, 20, 20, 16, 16. Now 20, 20, 20, 20, 16.
// vector<int>::iterator: Previously 12, 12, 12, 8, 4. Now 12, 8, 8, 8, 4.
// deque<int>: Previously 28, 28, 28, 20, 20. Now 28, 28, 28, 28, 28.
// deque<int>::iterator: Previously 12, 16, 16, 12, 8. Now 12, 8, 8, 8, 8.

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <deque>
#include <iterator>
#include <list>
#include <set>
#include <vector>

using namespace std;

template <typename C>
void test_swap(const char* const name) {
    const size_t N = sizeof(C);

    unsigned char array[N] = {0};

    C c;
    c.insert(c.end(), 10);
    c.insert(c.end(), 20);
    c.insert(c.end(), 30);

    C* const p = new (array) C;
    _Analysis_assume_(p);
    p->insert(p->end(), 11);
    p->insert(p->end(), 22);
    p->insert(p->end(), 33);
    p->insert(p->end(), 44);
    p->insert(p->end(), 55);

    typename C::const_iterator i = p->begin();

    advance(i, 3);

    swap(c, *p);

    p->~C();
    memset(array, 0xC4, N);

    assert(*i == 44);

    printf("    * %s test succeeded.\n", name);
}

int main() {
#ifdef _DEBUG
#if _HAS_ITERATOR_DEBUGGING
    puts("Running tests for Debug HID/SCL.");
#elif _SECURE_SCL
    puts("Running tests for Debug SCL.");
#else
    puts("Running tests for Debug None.");
#endif
#else
#if _SECURE_SCL
    puts("Running tests for Retail SCL.");
#else
    puts("Running tests for Retail None.");
#endif
#endif

    test_swap<vector<int>>("vector");
    test_swap<deque<int>>("deque");
    test_swap<list<int>>("list");
    test_swap<set<int>>("set");

    puts("All tests succeeded.");
}
