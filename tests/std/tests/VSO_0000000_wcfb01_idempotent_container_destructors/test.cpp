// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <new>
#include <string>
#include <vector>

using namespace std;

int main() {
    // this is nonstandard behavior, but historically our containers
    // have allowed it so we want to preserve it in the current ABI
    //
    // TRANSITION, ABI
    // in the next ABI breaking release we will change this behavior to terminate programs that do nonstandard things
    // like this

    { // small string
        char buff[sizeof(string)];
        string& s = *::new (buff) string;
        s.~string();
        s.~string();
    }

    { // big string
        char buff[sizeof(string)];
        string& s = *::new (buff) string("a really long string that is going to trigger separate allocation");
        s.~string();
        s.~string();
    }

    using vecT = vector<int>;

    { // empty vector
        char buff[sizeof(vecT)];
        vecT& v = *::new (buff) vecT;
        v.~vecT();
        v.~vecT();
    }

    { // data vector
        char buff[sizeof(vecT)];
        vecT& v = *::new (buff) vecT;
        v.push_back(42);
        v.~vecT();
        v.~vecT();
    }
}
