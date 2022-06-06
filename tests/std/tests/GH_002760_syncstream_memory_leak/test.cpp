// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
//
#include <crtdbg.h>
//
#include <cassert>
#include <sstream>
#include <syncstream>

using namespace std;
// GH-2760, <syncstream>: std::osyncstream memory leak
int main() {
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    _CrtMemState start, end, diff;
    _CrtMemCheckpoint(&start);
    {
        stringstream s;
        osyncstream bout(s);
        bout << "Hello, this line is long, LEAK incoming" << '\n';
    }
    _CrtMemCheckpoint(&end);
    assert(_CrtMemDifference(&diff, &start, &end) == 0);
}
