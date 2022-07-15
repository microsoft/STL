// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// DevDiv Bugs.52961 - "VC8 SP1 Regression: Crash With /MDd /D_HAS_ITERATOR_DEBUGGING=0"
//
// This regression from VC8 RTM to VC8 SP1 affects code compiled in debug mode
// with iterator debugging disabled. Whenever an iterator is set to point into
// a container and then the container is destroyed before the iterator, the code
// will crash.
//
// This was caused by the fix for the VC8 RTM bug VSWhidbey.579198, which also
// affected debug mode with iterator debugging disabled. In that bug, we begin
// with a local std::string, which has iterator debugging disabled. We then call
// std::string::substr(), which runs inside the separately compiled msvcp100d.dll
// with iterator debugging ENABLED and returns a temporary std::string with iterator
// debugging ENABLED. We then call std::string::end() on the temporary, which also
// runs inside the DLL. When this method constructs its return value, the iterator
// is registered in the temporary std::string's iterator list. However, the iterator
// will be destroyed in the user's code, which has iterator debugging disabled.
// Therefore, the temporary std::string's iterator list is left with an entry for
// a dead iterator. When the temporary std::string itself dies, its destructor runs
// in the DLL and detects that it was created with iterator debugging ENABLED, so
// it attempts to orphan all iterators in its iterator list, and tries to write to
// wherever the dead iterator lived.
//
// The fix introduced code into the base of all iterators (not just string iterators)
// which would run in user code compiled in debug mode with iterator debugging disabled
// and detect if the dying iterator was created inside the DLL with iterator debugging
// ENABLED, in which case the iterator would remove itself from the iterator list before
// dying.
//
// Unfortunately, fixing this problem triggered by a std::string outliving its iterator
// introduced a problem triggered by an iterator outliving its container (of any type).
// When the long-lived iterator dies, it attempts to remove itself from its dead parent's
// iterator list.
//
// The fix for the VC8 RTM bug VSWhidbey.583388 (yet another debug mode, iterator debugging
// disabled bug) introduced a separate regression, DevDiv Bugs.36183, which was fixed
// before the final version of VC8 SP1 was released. This last fix is the interesting one.
// Now, the DLL (with iterator debugging ENABLED) is smart. In std::string::substr(), it
// detects that the original string, living in user code, has iterator debugging disabled.
// Accordingly, it disables iterator debugging on the temporary std::string it returns.
// (std::string::substr() is the only method that returns a temporary std::string like this.)
// Now, calling std::string::end() on the temporary doesn't register any iterators in any
// iterator lists.
//
// As the fix for DevDiv Bugs.36183 thoroughly solves the problem that the broken fix
// for VSWhidbey.579198 attempted to solve, we should remove the broken fix. This restores
// correctness to all known cases, and also increases efficiency (as iterator destructors
// in debug mode with iterator debugging disabled now perform no extra work).

#include <cassert>
#include <cstdio>
#include <cstring>
#include <vector>

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

#pragma warning(push)
#pragma warning(disable : 4582) // 'uninit_vector::storage': constructor is not implicitly called
#pragma warning(disable : 4583) // 'uninit_vector::storage': destructor is not implicitly called
union uninit_vector {
    unsigned char bytes[sizeof(vector<int>)];
    vector<int> storage;

    constexpr uninit_vector() : bytes{} {}
    uninit_vector(const uninit_vector&)            = delete;
    uninit_vector& operator=(const uninit_vector&) = delete;
    ~uninit_vector() {}
};
#pragma warning(pop)

int main() {
    puts("Beginning of test for DevDiv Bugs.52961 ...");

    {
        uninit_vector storage;

        puts("Constructing vector.");

        vector<int>* const p = new (&storage.storage) vector<int>;

        _Analysis_assume_(p);

        puts("Vector constructed. Pushing back values.");

        p->push_back(20);
        p->push_back(30);

        puts("Values pushed back. Constructing iterator.");

        const vector<int>::const_iterator i = p->begin();

        puts("Iterator constructed. Using iterator.");

        assert(i[0] == 20);
        assert(i[1] == 30);

        puts("Iterator use successful. Destroying container.");

        p->~vector<int>();

        puts("Container destroyed. Scribbling over memory.");

        memset(&storage.bytes, 0xC4, sizeof(storage.bytes));

        puts("Memory scribbled. Destroying iterator (this should not crash).");
    }

    puts("Iterator destroyed successfully.");
}
