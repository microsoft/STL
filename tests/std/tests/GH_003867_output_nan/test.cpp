// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <limits>
#include <locale>
#include <sstream>
#include <string>

using namespace std;

struct groups_of_1 : numpunct<char> {
    // The standard char specialization of std::numpunct::do_thousands_sep returns ','
    string do_grouping() const override {
        return "\1";
    } // groups of 1 digit
};

template <typename T>
void test_gh_3867() {
    // GH-3867 Writing NaN to the output stream with a set locale results in a weird output
    {
        ostringstream s;
        s.imbue(locale("en-US"));
        s << -numeric_limits<T>::quiet_NaN();
        assert(s.str() == "-nan(ind)");
    }
    {
        ostringstream s;
        s.imbue(locale(s.getloc(), new groups_of_1));
        s << -numeric_limits<T>::infinity();
        assert(s.str() == "-inf");
    }
}

int main() {
    test_gh_3867<float>();
    test_gh_3867<double>();
    test_gh_3867<long double>();
}
