// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

//////////////////////////////////////////////////////////////////////////////////////////

// Bug : VSW563705
// When _CRTDBG_MAP_ALLOC is defined, cpp files that use std::malloc(),
// std::free(), std::calloc(), std::realloc() don't compile.
//

// Problem :
// When _CRTDBG_MAP_ALLOC is defined, malloc() is defined as _malloc_dbg() etc., but
// _malloc_dbg() isn't defined in std namespace.

// Test :
// This is a compilation test making sure that std::malloc() etc. can be compiled
// successfully when _CRTDBG_MAP_ALLOC is defined.


//////////////////////////////////////////////////////////////////////////////////////////

#include <crtdbg.h>
#include <cstdlib>

int main() {
    // this is a test to test whether
    // malloc/free/realloc and its _dbg variant
    // compiles under std namespace
    std::free(std::malloc(1));
    std::free(std::calloc(1, 1));
    std::free(std::realloc(std::malloc(1), 1));
}
