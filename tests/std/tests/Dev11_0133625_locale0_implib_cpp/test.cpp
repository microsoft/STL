// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <locale>
#include <sstream>
#include <string>

using namespace std;

// The problem in this bug was that basic_string was getting dragged into locale0_implib.cpp,
// which is injected into the msvcprt.lib and msvcprtd.lib import libraries.
// This triggered ODR violations with non-default _ITERATOR_DEBUG_LEVEL settings
// (and more obscurely, with /J).
// In native code, the usual symptoms were debugger visualization problems.
// In managed code, /clr's metadata merge triggered linker errors.
// This test uses a facet to drag in locale0_implib.cpp, then uses a string.
// It's then compiled with lots of different options.
// This will fail if basic_string sneaks its way into msvcprt[d].lib again.

int main() {
    use_facet<ctype<char>>(locale::classic());

    const string s("cute fluffy kittens");

    ostringstream oss;

    oss << "I have " << 9 * 9 * 9 + 10 * 10 * 10 << " " << s << ".";

    assert(oss.str() == "I have 1729 cute fluffy kittens.");
}
