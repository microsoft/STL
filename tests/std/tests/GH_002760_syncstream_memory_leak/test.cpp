// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// REQUIRES: debug_CRT

#include <cassert>
#include <cstdlib>
#include <sstream>
#include <syncstream>
using namespace std;

// GH-2760, <syncstream>: std::osyncstream memory leak
int main() {
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    [[maybe_unused]] _CrtMemState start;
    [[maybe_unused]] _CrtMemState end;
    [[maybe_unused]] _CrtMemState diff;
    _CrtMemCheckpoint(&start);
    {
        stringstream s;
        osyncstream bout(s);
        bout << "Hello, this line is long, which previously leaked memory" << '\n';
    }
    _CrtMemCheckpoint(&end);
    assert(_CrtMemDifference(&diff, &start, &end) == 0);
}
