// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <iomanip>
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

// Also test GH-4210: With setprecision(0) showpoint fixed, a bogus '.' is emitted for infinity and NaN

template <class FloatingPoint>
void test_output_nonfinite_value(const FloatingPoint x) {
    const auto s1 = [x] {
        ostringstream os;
        os << setprecision(0) << showpoint << fixed;
        os << x;
        return os.str();
    }();
    const auto s2 = [x] {
        ostringstream os;
        os << setprecision(0) << noshowpoint << fixed;
        os << x;
        return os.str();
    }();
    assert(s1 == s2);

    const auto s3 = [x] {
        ostringstream os;
        os << setprecision(0) << showpoint << fixed << showpos;
        os << x;
        return os.str();
    }();
    const auto s4 = [x] {
        ostringstream os;
        os << setprecision(0) << noshowpoint << fixed << showpos;
        os << x;
        return os.str();
    }();
    assert(s3 == s4);
}

template <class FloatingPoint>
void test_gh_4210() {
    constexpr auto inf_val = numeric_limits<FloatingPoint>::infinity();
    constexpr auto nan_val = numeric_limits<FloatingPoint>::quiet_NaN();

    test_output_nonfinite_value(inf_val);
    test_output_nonfinite_value(-inf_val);
    test_output_nonfinite_value(nan_val);
    test_output_nonfinite_value(-nan_val);
}

int main() {
    test_gh_3867<float>();
    test_gh_3867<double>();
    test_gh_3867<long double>();

    test_gh_4210<float>();
    test_gh_4210<double>();
    test_gh_4210<long double>();
}
