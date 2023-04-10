// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

//////////////////////////////////////////////////////////////////////////////////////////

// Bug : VSW563686
//      iostream has memory leaks

// Problem :
//      istream and ostream virtually inherits from ios_base. Init function
//      was being called from both the derived classes. This resulted in
//      a memory leak since the memory allocated in the first initialization
//      is lost when the Init function is called the second time.
//
// Fix :
//      Modify ostream constructor to take a parameter that specifies whether
//      it should call base class. Modify the constructor to call Init based
//      on the parameter.

// Test :
//      Use CRT memory tracking functions to detect memory leaks.
//      Create iostream instances and destroy them.
//      Check for any memory leaks.

//////////////////////////////////////////////////////////////////////////////////////////

#define CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <cstdlib>
#include <sstream>

#include <Windows.h>

int main() {
    // Track CRT blocks
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_CRT_DF);

    {
        // Allocate long lived objects in the CRT.
        // We don't want these objects to show up as leaks.
        std::iostream s(nullptr);
    }

    // Get memory state before creating iostream
    _CrtMemState before;
    _CrtMemCheckpoint(&before);

    // Construct and destroy an iostream, which previously leaked.
    { std::iostream s(nullptr); }

    // Get memory state after iostream allocation/deallocation
    _CrtMemState after;
    _CrtMemCheckpoint(&after);

    // Diff memory state
    _CrtMemState diff;
    // return 1 if there are diffs
    int nRet = _CrtMemDifference(&diff, &before, &after);
    (void) diff;
    (void) before;
    (void) after;

    // Dump diff stats and the leaks if any.
    _CrtMemDumpStatistics(&diff);
    OutputDebugString("Start Memory Dump\n");
    _CrtMemDumpAllObjectsSince(&before);
    OutputDebugString("End Memory Dump\n");

    return nRet;
}
