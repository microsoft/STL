// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <limits>
#include <locale>
#include <sstream>
#include <string>

using namespace std;

// Because std::string crosses the DLL boundary via overridden virtual functions,
// we can test custom facets only when:
// * linking statically, or
// * linking dynamically with IDL set to its default value (so the user code and the DLL match).
#ifdef _DEBUG
#define DEFAULT_IDL 2
#else
#define DEFAULT_IDL 0
#endif

#if !defined(_DLL) || _ITERATOR_DEBUG_LEVEL == DEFAULT_IDL
#define TEST_CUSTOM_FACET 1
#else
#define TEST_CUSTOM_FACET 0
#endif

#if TEST_CUSTOM_FACET
struct groups_of_1 : numpunct<char> {
    // The standard char specialization of std::numpunct::do_thousands_sep returns ','
    string do_grouping() const override {
        return "\1";
    } // groups of 1 digit
};
#endif // TEST_CUSTOM_FACET

template <typename T>
void test_gh_3867() {
    // GH-3867 Writing NaN to the output stream with a set locale results in a weird output
    {
        ostringstream s;
        s.imbue(locale("en-US"));
        s << -numeric_limits<T>::quiet_NaN();
        assert(s.str() == "-nan(ind)");
    }

#if TEST_CUSTOM_FACET
    {
        ostringstream s;
        s.imbue(locale(s.getloc(), new groups_of_1));
        s << -numeric_limits<T>::infinity();
        assert(s.str() == "-inf");
    }
#endif // TEST_CUSTOM_FACET
}

int main() {
    test_gh_3867<float>();
    test_gh_3867<double>();
    test_gh_3867<long double>();
}
