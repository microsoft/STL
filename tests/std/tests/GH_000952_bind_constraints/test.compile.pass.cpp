// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <functional>
#include <type_traits>

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

using namespace std;

void test() { // COMPILE-ONLY
    {
        auto lambda = [](int) {};
        auto f      = bind(lambda, placeholders::_1);
        STATIC_ASSERT(!is_convertible_v<decltype(f), function<void()>>);
        STATIC_ASSERT(!is_convertible_v<decltype(f), function<void*(int)>>);
        STATIC_ASSERT(is_convertible_v<decltype(f), function<void(int)>>);
        STATIC_ASSERT(!is_convertible_v<decltype(f), function<char(int)>>);
    }
    {
        auto lambda = [](int) { return 42; };
        auto f      = bind<void>(lambda, placeholders::_1);
        STATIC_ASSERT(!is_convertible_v<decltype(f), function<void()>>);
        STATIC_ASSERT(!is_convertible_v<decltype(f), function<void*(int)>>);
        STATIC_ASSERT(is_convertible_v<decltype(f), function<void(int)>>);
        STATIC_ASSERT(!is_convertible_v<decltype(f), function<char(int)>>);
    }
    {
        auto lambda = [](int) { return 42; };
        auto f      = bind<long>(lambda, placeholders::_1);
        STATIC_ASSERT(!is_convertible_v<decltype(f), function<void()>>);
        STATIC_ASSERT(!is_convertible_v<decltype(f), function<void*(int)>>);
        STATIC_ASSERT(is_convertible_v<decltype(f), function<void(int)>>);
        STATIC_ASSERT(is_convertible_v<decltype(f), function<char(int)>>);
    }
    {
        auto lambda0 = [](int, int) { return true; };
        auto lambda1 = [](const char*) { return 0; };
        auto f       = bind(lambda0, placeholders::_1, bind(lambda1, placeholders::_2));
        STATIC_ASSERT(!is_convertible_v<decltype(f), function<void()>>);
        STATIC_ASSERT(!is_convertible_v<decltype(f), function<void*(int, const char*)>>);
        STATIC_ASSERT(is_convertible_v<decltype(f), function<void(int, const char*)>>);
        STATIC_ASSERT(is_convertible_v<decltype(f), function<bool(int, const char*)>>);
    }
}
