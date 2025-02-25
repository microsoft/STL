// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <new>
#include <string>
#include <vector>

using namespace std;

#if _MSVC_STL_DESTRUCTOR_TOMBSTONES
#error This test is fundamentally incompatible with destructor tombstones.
#endif

int main() {
    // Idempotent destruction is an abomination that our `string` and `vector` have historically allowed.
    // We're preserving it for the time being, except when destructor tombstones are enabled.

    // TRANSITION, ABI: In the next ABI-breaking release, we'll stop supporting this nonstandard usage.

    { // small string
        alignas(string) char buff[sizeof(string)];
        string& s = *::new (buff) string;
        s.~string();
        s.~string();
    }

    { // big string
        alignas(string) char buff[sizeof(string)];
        string& s = *::new (buff) string("a really long string that is going to trigger separate allocation");
        s.~string();
        s.~string();
    }

    using vecT = vector<int>;

    { // empty vector
        alignas(vecT) char buff[sizeof(vecT)];
        vecT& v = *::new (buff) vecT;
        v.~vecT();
        v.~vecT();
    }

    { // data vector
        alignas(vecT) char buff[sizeof(vecT)];
        vecT& v = *::new (buff) vecT;
        v.push_back(42);
        v.~vecT();
        v.~vecT();
    }
}
