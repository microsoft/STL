// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <assert.h>
#include <sstream>
#include <string>

using namespace std;

int main() {
#if _HAS_CXX17
    // LWG-2221 "No formatted output operator for nullptr"
    ostringstream os;
    os << nullptr;
    assert(os.str() == "nullptr");
    wostringstream wos;
    wos << nullptr;
    assert(wos.str() == L"nullptr");
#endif // _HAS_CXX17

    // LWG-1203 "More useful rvalue stream insertion"
    assert((ostringstream{} << 42).str() == "42");
    int x;
    assert((istringstream("42 1729") >> x).str() == "42 1729");
    assert(x == 42);
}
