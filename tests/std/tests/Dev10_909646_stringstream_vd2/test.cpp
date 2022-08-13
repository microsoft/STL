// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <sstream>

using namespace std;

struct Object {
    virtual ~Object() {}
};

struct Base : virtual public Object {
    Base() : Object() {
        Object* o = static_cast<Object*>(this);
        Base* b   = dynamic_cast<Base*>(o);

        assert(this == b);
    }
};

struct Derived : public Base {
    int i;
};

int main() {
    Derived d;

    stringstream* ss = new stringstream;

    *ss << "I have " << 9 * 9 * 9 + 10 * 10 * 10 << " cute fluffy kittens.";

    assert(ss->str() == "I have 1729 cute fluffy kittens.");

    delete ss;
}
