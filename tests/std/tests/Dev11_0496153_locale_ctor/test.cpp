// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <clocale>
#include <cstdio>
#include <exception>
#include <locale>
#include <sstream>

using namespace std;

// The following monstrosity avoids activating the test code for _DLL compiles
// that use a non-default value of _ITERATOR_DEBUG_LEVEL (_ITERATOR_DEBUG_LEVEL
// defaults to "0" for release builds and "2" for _DEBUG).
#if defined(_DLL) && (_ITERATOR_DEBUG_LEVEL == 1 || (defined(_DEBUG) != (_ITERATOR_DEBUG_LEVEL == 2)))
#define ENABLE_TEST 0
#else
#define ENABLE_TEST 1
#endif // defined(_DLL) && (_ITERATOR_DEBUG_LEVEL == 1 || (defined(_DEBUG) != (_ITERATOR_DEBUG_LEVEL == 2)))

#if ENABLE_TEST
void test_Dev11_496153_locale_ctor_should_not_throw() noexcept {
    const locale loc(setlocale(LC_ALL, nullptr));

    (void) loc;
}

struct comma_separator : numpunct<char> {
    virtual char do_decimal_point() const override {
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
#endif // ENABLE_TEST

int main() {
#if ENABLE_TEST
    test_Dev11_496153_locale_ctor_should_not_throw();
    test_VSO_159700_locale_should_support_user_defined_facets();
#endif // ENABLE_TEST
}
