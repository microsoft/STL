// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <sstream>

using namespace std;

int main() {
    istringstream iss("42");
    int fortyTwo = 100;
    iss >> fortyTwo;
    assert(fortyTwo == 42);
    assert(iss.rdstate() == ios_base::eofbit);
    iss.seekg(0); // VSO-599804 caused by ignoring seek to 0 for non-nullptr buffers
    assert(iss.rdstate() == ios_base::goodbit);
    fortyTwo = 100;
    iss >> fortyTwo;
    assert(fortyTwo == 42);
    assert(iss.rdstate() == ios_base::eofbit);
}
