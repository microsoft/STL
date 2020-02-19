// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _HAS_DEPRECATED_ADAPTOR_TYPEDEFS 1
#define _SILENCE_CXX17_ADAPTOR_TYPEDEFS_DEPRECATION_WARNING

#include <functional>
#include <type_traits>

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

template <typename Correct, typename F>
void test(const F&) {
    STATIC_ASSERT(is_same_v<Correct, typename F::result_type>);
}

int cat(int) {
    return 5;
}

struct X {
    int dog() {
        return 6;
    }
};

struct Y {
    typedef int result_type;

    int operator()(int) const {
        return 7;
    }
};

int main() {
    X x;

    test<int>(bind(cat, 10));

    test<int>(bind(&cat, 10));

    test<int>(bind(&X::dog, x));

    test<int>(bind(Y(), 20));

    test<double>(bind<double>(cat, 10));

    test<double>(bind<double>(&cat, 10));

    test<double>(bind<double>(&X::dog, x));

    test<double>(bind<double>(Y(), 20));
}
