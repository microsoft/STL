// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <fstream>

using namespace std;

int main() {
    {
        ofstream prepareTestFile("test.txt");
        prepareTestFile << "ab";
    }
    fstream f("test.txt");
    f.seekg(1);
    const int res = f.get();
    assert(res == 'b');
    f.putback('b');
    f.putback('a');
    f.close();
    return 0;
}
