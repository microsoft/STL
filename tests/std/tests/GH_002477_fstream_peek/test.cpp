// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <fstream>
#include <iostream>
using namespace std;

int main() {
    const char* const filename = "example_file.txt";

    {
        ofstream o(filename, ios_base::binary);
        assert(o.good());
        o << "cute fluffy kittens\n";
    }

    ifstream fin(filename);
    assert(fin.good());
    assert(fin.tellg() == 0);
    assert(fin.good());
    assert(static_cast<char>(fin.peek()) == 'c');
    assert(fin.good());
    assert(fin.tellg() == 0);
}
