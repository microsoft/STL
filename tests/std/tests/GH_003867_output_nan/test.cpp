// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <limits>
#include <locale>
#include <sstream>

using namespace std;

template <typename T>
void test_gh_3867() {
    // GH-3867 Writing NaN to the output stream with a set locale results in a weird output
    ostringstream s;
    s.imbue(locale("en-US"));
    s << -numeric_limits<T>::quiet_NaN();
    assert(s.str() == "-nan(ind)");
}

int main() {
    test_gh_3867<float>();
    test_gh_3867<double>();
    test_gh_3867<long double>();
}
