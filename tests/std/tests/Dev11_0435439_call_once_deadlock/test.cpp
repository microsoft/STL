// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <atomic>
#include <thread>

using namespace std;

atomic<bool> Atom(true);

void Spin() {
    while (Atom) {
    }
}

struct Global {
    thread m_t;
    Global() : m_t(Spin) {}
    ~Global() {
        Atom = false;
        m_t.join();
    }
};

Global g;

int main() {
    // runtime tests are in constructors and destructors of variables with static storage duration
}
