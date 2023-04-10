// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <string>

using namespace std;

string meow() {
    return "MEOWMEOWMEOWMEOWMEOW";
}

string purr() {
    return "PURRPURRPURRPURRPURR";
}

int main() {
    const string s = "lvalue";

    const string& r1 = s + meow();
    const string& r2 = meow() + s;
    const string& r3 = meow() + purr();
    const string& r4 = "cat" + meow();
    const string& r5 = 'x' + meow();
    const string& r6 = meow() + "dog";
    const string& r7 = meow() + 'y';

    assert(r1 == "lvalueMEOWMEOWMEOWMEOWMEOW");
    assert(r2 == "MEOWMEOWMEOWMEOWMEOWlvalue");
    assert(r3 == "MEOWMEOWMEOWMEOWMEOWPURRPURRPURRPURRPURR");
    assert(r4 == "catMEOWMEOWMEOWMEOWMEOW");
    assert(r5 == "xMEOWMEOWMEOWMEOWMEOW");
    assert(r6 == "MEOWMEOWMEOWMEOWMEOWdog");
    assert(r7 == "MEOWMEOWMEOWMEOWMEOWy");
}
