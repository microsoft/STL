// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <clocale>
#include <cstdio>
#include <exception>
#include <locale>
#include <sstream>

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
void test_Dev11_496153_locale_ctor_should_not_throw() noexcept {
    const locale loc(setlocale(LC_ALL, nullptr));

    (void) loc;
}

struct comma_separator : numpunct<char> {
    char do_decimal_point() const override {
        return ',';
    }
};

void test_VSO_159700_locale_should_support_user_defined_facets() {
    stringstream str;
    locale loc(str.getloc(), new comma_separator);
    str.imbue(loc);
    str << 1.5f;
    assert("locale didn't support user-defined facets" && str.str() == "1,5");
}
#endif // TEST_CUSTOM_FACET

int main() {
#if TEST_CUSTOM_FACET
    test_Dev11_496153_locale_ctor_should_not_throw();
    test_VSO_159700_locale_should_support_user_defined_facets();
#endif // TEST_CUSTOM_FACET
}
