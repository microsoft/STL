// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

import std.compat;

#include <assert.h> // intentionally not <cassert>

// INTENTIONALLY AVOIDED: using namespace std;

void test_export_import_std() {
    ::puts("std.compat: Testing `export import std;`.");

    // Verify that std::vector is available:
    const std::vector<int> v{10, 20, 30, 40, 50};
    assert(v[2] == 30);

    // Verify that `::operator new` is available:
    const int* const ptr = new (std::nothrow) int{1729};
    assert(ptr);
    assert(*ptr == 1729);
    delete ptr;

    // Verify that the Sufficient Additional Overloads are available for std::sqrt():
    assert(std::sqrt(25.0) == 5.0);
    assert(std::sqrt(25.0f) == 5.0f);
    assert(std::sqrt(25) == 5.0);
    static_assert(std::is_same_v<decltype(std::sqrt(25.0)), double>);
    static_assert(std::is_same_v<decltype(std::sqrt(25.0f)), float>);
    static_assert(std::is_same_v<decltype(std::sqrt(25)), double>);
}

template <typename Void, typename... Args>
struct IsGlobalHypotCallableImpl : std::false_type {};

template <typename... Args>
struct IsGlobalHypotCallableImpl<std::void_t<decltype(::hypot(std::declval<Args>()...))>, Args...> : std::true_type {};

template <typename... Args>
constexpr bool IsGlobalHypotCallable = IsGlobalHypotCallableImpl<void, Args...>::value;

void test_binary_hypot() {
    ::puts("std.compat: Testing binary hypot().");

    // hypot() is special. C's hypot(x, y) and C++'s Sufficient Additional Overloads
    // are defined in the global namespace, then dragged into namespace std.
    // C++'s hypot(x, y, z) (and its Sufficient Additional Overloads) are defined directly within namespace std.
    // Therefore, the std.compat module should provide only 2-arg hypot() in the global namespace, not 3-arg.

    static_assert(IsGlobalHypotCallable<double, double>);
    static_assert(IsGlobalHypotCallable<float, float>);
    static_assert(IsGlobalHypotCallable<int, int>);

    static_assert(!IsGlobalHypotCallable<double, double, double>);
    static_assert(!IsGlobalHypotCallable<float, float, float>);
    static_assert(!IsGlobalHypotCallable<int, int, int>);

    assert(::hypot(3.0, 4.0) == 5.0);
    assert(::hypot(3.0f, 4.0f) == 5.0f);
    assert(::hypot(3, 4) == 5.0);
    static_assert(std::is_same_v<decltype(::hypot(3.0, 4.0)), double>);
    static_assert(std::is_same_v<decltype(::hypot(3.0f, 4.0f)), float>);
    static_assert(std::is_same_v<decltype(::hypot(3, 4)), double>);
}

void test_global_cassert() {
    ::puts("std.compat: Nothing to test in <cassert>, only macros.");
}

void test_global_cctype() {
    ::puts("std.compat: Testing <cctype>.");
    assert(::isalpha('b'));
    assert(!::isalpha('2'));
}

void test_global_cerrno() {
    ::puts("std.compat: Nothing to test in <cerrno>, only macros.");
}

void test_global_cfenv() {
    ::puts("std.compat: Testing <cfenv>.");
    assert(::fegetround() >= 0);
}

void test_global_cfloat() {
    ::puts("std.compat: Nothing to test in <cfloat>, only macros.");
}

void test_global_cinttypes() {
    ::puts("std.compat: Testing <cinttypes>.");
    assert(::strtoumax("0xABCDEF0123456789", nullptr, 0) == 0xABCDEF0123456789ull);
}

void test_global_climits() {
    ::puts("std.compat: Nothing to test in <climits>, only macros.");
}

void test_global_clocale() {
    ::puts("std.compat: Testing <clocale>.");
    const ::lconv* const ptr = ::localeconv();
    assert(ptr != nullptr);
}

void test_global_cmath() {
    ::puts("std.compat: Testing <cmath>.");

    // Verify that the Sufficient Additional Overloads are available for ::sqrt():
    assert(::sqrt(25.0) == 5.0);
    assert(::sqrt(25.0f) == 5.0f);
    assert(::sqrt(25) == 5.0);
    static_assert(std::is_same_v<decltype(::sqrt(25.0)), double>);
    static_assert(std::is_same_v<decltype(::sqrt(25.0f)), float>);
    static_assert(std::is_same_v<decltype(::sqrt(25)), double>);
}

void test_global_csetjmp() {
    ::puts("std.compat: Testing <csetjmp>.");
    static_assert(sizeof(::jmp_buf) > 0);
}

void test_global_csignal() {
    ::puts("std.compat: Testing <csignal>.");
    static_assert(sizeof(::sig_atomic_t) > 0);
}

void test_global_cstdarg() {
    ::puts("std.compat: Testing <cstdarg>.");
    static_assert(sizeof(::va_list) > 0);
}

void test_global_cstddef() {
    ::puts("std.compat: Testing <cstddef>.");
    static_assert(std::is_signed_v<::ptrdiff_t>);
}

void test_global_cstdint() {
    ::puts("std.compat: Testing <cstdint>.");
    static_assert(std::is_unsigned_v<::uint16_t>);
}

#pragma warning(push)
#pragma warning(disable : 4996) // was declared deprecated
void test_global_cstdio() {
    ::puts("std.compat: Testing <cstdio>.");
    double dbl{0.0};
    assert(::sscanf("-3.25", "%lg", &dbl) == 1);
    assert(dbl == -3.25);
}
#pragma warning(pop)

void test_global_cstdlib() {
    ::puts("std.compat: Testing <cstdlib>.");
    assert(::strtod("7.125", nullptr) == 7.125);
}

void test_global_cstring() {
    ::puts("std.compat: Testing <cstring>.");
    assert(::strcmp("cats", "dogs") < 0);
}

void test_global_ctime() {
    ::puts("std.compat: Testing <ctime>.");
    const ::time_t now = ::time(nullptr);
    assert(now > 0);
}

void test_global_cuchar() {
    ::puts("std.compat: Testing <cuchar>.");
    char32_t c32{};
    ::mbstate_t state{};
    assert(::mbrtoc32(&c32, "x", 1, &state) == 1);
    assert(c32 == U'x');
}

void test_global_cwchar() {
    ::puts("std.compat: Testing <cwchar>.");
    assert(::wcslen(L"cute fluffy kittens") == 19);
}

void test_global_cwctype() {
    ::puts("std.compat: Testing <cwctype>.");
    assert(::iswalpha(L'b'));
    assert(!::iswalpha(L'2'));
}

void all_global_cmeow_tests() {
    test_global_cassert();
    test_global_cctype();
    test_global_cerrno();
    test_global_cfenv();
    test_global_cfloat();
    test_global_cinttypes();
    test_global_climits();
    test_global_clocale();
    test_global_cmath();
    test_global_csetjmp();
    test_global_csignal();
    test_global_cstdarg();
    test_global_cstddef();
    test_global_cstdint();
    test_global_cstdio();
    test_global_cstdlib();
    test_global_cstring();
    test_global_ctime();
    test_global_cuchar();
    test_global_cwchar();
    test_global_cwctype();
}

void test_module_std_compat() {
    test_export_import_std();
    test_binary_hypot();
    all_global_cmeow_tests();
}
