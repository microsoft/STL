// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <sstream>
#include <typeinfo>

using namespace std;

int main() {
    ostringstream oss;

    ostream& r = (oss << 9 * 9 * 9 + 10 * 10 * 10);

    assert(oss.str() == "1729");

    assert(typeid(r) == typeid(ostringstream));
    assert(typeid(r) != typeid(istringstream));
    assert(typeid(r) != typeid(stringstream));
    assert(typeid(r) != typeid(ostream));
}
