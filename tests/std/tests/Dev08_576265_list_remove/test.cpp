// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <list>

struct Val {
    int value;
    unsigned int canary;
    Val() : value(0), canary(0xDEADBEEF) {}
    Val(int val) : value(val), canary(0x600DF00D) {}
    Val(const Val&)            = default;
    Val& operator=(const Val&) = default;
    ~Val() {
        canary = 0xDEADBEEF;
    }
};

bool operator==(const Val& val1, const Val& val2) {
    assert(val1.canary == 0x600DF00D && val2.canary == 0x600DF00D);
    return val1.value == val2.value;
}

int main() {
    // When passed a reference to an element of the list, std::list::remove
    // will defer destruction of that element's node until it returns.
    std::list<Val> l;

    l.push_back(Val(1));
    l.push_back(Val(2));
    l.remove(l.front());
}
