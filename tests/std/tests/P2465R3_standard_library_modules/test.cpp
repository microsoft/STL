// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

import std;

#include <assert.h> // intentionally not <cassert>

// SPECIAL: This is order-dependent and must be included after all other Standard imports/includes.
#include <test_header_units_and_modules.hpp>

// INTENTIONALLY AVOIDED: using namespace std;

void prepare_test_environment();
void all_std_cmeow_tests();
void test_module_std_compat();

int main() {
    prepare_test_environment(); // defined in classic.cpp
    all_cpp_header_tests(); // defined in test_header_units_and_modules.hpp
    all_std_cmeow_tests(); // defined below
    test_module_std_compat(); // defined in test2.cpp
}

void test_std_cassert() {
    std::puts("Nothing to test in <cassert>, only macros.");
}

void test_std_cctype() {
    std::puts("Testing <cctype>.");
    assert(std::isalpha('b'));
    assert(!std::isalpha('2'));
}

void test_std_cerrno() {
    std::puts("Nothing to test in <cerrno>, only macros.");
}

void test_std_cfenv() {
    std::puts("Testing <cfenv>.");
    assert(std::fegetround() >= 0);
}

void test_std_cfloat() {
    std::puts("Nothing to test in <cfloat>, only macros.");
}

void test_std_cinttypes() {
    std::puts("Testing <cinttypes>.");
    assert(std::strtoumax("0xABCDEF0123456789", nullptr, 0) == 0xABCDEF0123456789ull);
}

void test_std_climits() {
    std::puts("Nothing to test in <climits>, only macros.");
}

void test_std_clocale() {
    std::puts("Testing <clocale>.");
    const std::lconv* const ptr = std::localeconv();
    assert(ptr != nullptr);
}

void test_std_cmath() {
    std::puts("Testing <cmath>.");

    // Test C++'s Sufficient Additional Overloads
    assert(std::sqrt(25.0) == 5.0);
    assert(std::sqrt(25.0f) == 5.0f);
    assert(std::sqrt(25) == 5.0);
    static_assert(std::is_same_v<decltype(std::sqrt(25.0)), double>);
    static_assert(std::is_same_v<decltype(std::sqrt(25.0f)), float>);
    static_assert(std::is_same_v<decltype(std::sqrt(25)), double>);

    // Test C's hypot(x, y), C++'s hypot(x, y, z), and Sufficient Additional Overloads
    assert(std::hypot(3.0, 4.0) == 5.0);
    assert(std::hypot(3.0f, 4.0f) == 5.0f);
    assert(std::hypot(3, 4) == 5.0);
    static_assert(std::is_same_v<decltype(std::hypot(3.0, 4.0)), double>);
    static_assert(std::is_same_v<decltype(std::hypot(3.0f, 4.0f)), float>);
    static_assert(std::is_same_v<decltype(std::hypot(3, 4)), double>);

    assert(std::hypot(2.0, 3.0, 6.0) == 7.0);
    assert(std::round(std::hypot(2.0f, 3.0f, 6.0f)) == 7.0f); // compensate for rounding error
    assert(std::hypot(2, 3, 6) == 7.0);
    static_assert(std::is_same_v<decltype(std::hypot(2.0, 3.0, 6.0)), double>);
    static_assert(std::is_same_v<decltype(std::hypot(2.0f, 3.0f, 6.0f)), float>);
    static_assert(std::is_same_v<decltype(std::hypot(2, 3, 6)), double>);

    // Test C++'s lerp()
    assert(std::lerp(100.0, 110.0, 0.25) == 102.5);

    // Test C++'s Special Math
    const double rz3 = std::riemann_zeta(3.0);
    assert(rz3 > 1.2020569031);
    assert(rz3 < 1.2020569032);
}

void test_std_csetjmp() {
    std::puts("Testing <csetjmp>.");
    static_assert(sizeof(std::jmp_buf) > 0);
}

void test_std_csignal() {
    std::puts("Testing <csignal>.");
    static_assert(sizeof(std::sig_atomic_t) > 0);
}

void test_std_cstdarg() {
    std::puts("Testing <cstdarg>.");
    static_assert(sizeof(std::va_list) > 0);
}

void test_std_cstddef() {
    std::puts("Testing <cstddef>.");
    static_assert(std::is_signed_v<std::ptrdiff_t>);

    // Test C++'s byte
    std::byte b{0x33};
    b |= std::byte{0x85};
    assert(std::to_integer<unsigned char>(b) == 0xB7);

    // Test C++'s nullptr_t
    static_assert(std::nullptr_t{} == nullptr);
}

void test_std_cstdint() {
    std::puts("Testing <cstdint>.");
    static_assert(std::is_unsigned_v<std::uint16_t>);
}

#pragma warning(push)
#pragma warning(disable : 4996) // was declared deprecated
void test_std_cstdio() {
    std::puts("Testing <cstdio>.");
    double dbl{0.0};
    assert(std::sscanf("-3.25", "%lg", &dbl) == 1);
    assert(dbl == -3.25);
}
#pragma warning(pop)

void test_std_cstdlib() {
    std::puts("Testing <cstdlib>.");
    assert(std::strtod("7.125", nullptr) == 7.125);
}

void test_std_cstring() {
    std::puts("Testing <cstring>.");
    assert(std::strcmp("cats", "dogs") < 0);
}

void test_std_ctime() {
    std::puts("Testing <ctime>.");
    const std::time_t now = std::time(nullptr);
    assert(now > 0);
}

void test_std_cuchar() {
    std::puts("Testing <cuchar>.");
    char32_t c32{};
    std::mbstate_t state{};
    assert(std::mbrtoc32(&c32, "x", 1, &state) == 1);
    assert(c32 == U'x');
}

void test_std_cwchar() {
    std::puts("Testing <cwchar>.");
    assert(std::wcslen(L"cute fluffy kittens") == 19);
}

void test_std_cwctype() {
    std::puts("Testing <cwctype>.");
    assert(std::iswalpha(L'b'));
    assert(!std::iswalpha(L'2'));
}

void all_std_cmeow_tests() {
    test_std_cassert();
    test_std_cctype();
    test_std_cerrno();
    test_std_cfenv();
    test_std_cfloat();
    test_std_cinttypes();
    test_std_climits();
    test_std_clocale();
    test_std_cmath();
    test_std_csetjmp();
    test_std_csignal();
    test_std_cstdarg();
    test_std_cstddef();
    test_std_cstdint();
    test_std_cstdio();
    test_std_cstdlib();
    test_std_cstring();
    test_std_ctime();
    test_std_cuchar();
    test_std_cwchar();
    test_std_cwctype();
}
