// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <functional>

using namespace std;

class Base {
public:
    Base() {}

    virtual ~Base() {}

    void meow() {
        purr();
    }

private:
    Base(const Base&);
    Base& operator=(const Base&);

    virtual void purr() = 0;
};

class Derived : public Base {
private:
    void purr() override {}
};

int main() {
    Derived d;
    mem_fn (&Derived::meow)(d);
}
