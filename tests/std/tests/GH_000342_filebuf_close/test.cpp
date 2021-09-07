// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <fstream>

using namespace std;

int main() {
    {
        ofstream f("test.txt");
        f << "ab";
    }
    fstream f("test.txt");
    f.seekg(1);
    f.get(); // b
    f.putback('b');
    f.putback('a');
    f.close();
    return 0;
}
