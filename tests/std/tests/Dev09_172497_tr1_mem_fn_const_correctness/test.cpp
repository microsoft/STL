// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _HAS_DEPRECATED_ADAPTOR_TYPEDEFS 1
#define _SILENCE_CXX17_ADAPTOR_TYPEDEFS_DEPRECATION_WARNING

#include <functional>
#include <type_traits>

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

struct Cat {
    float meow() {
        return 11.1f;
    }
    float meow_c() const {
        return 11.1f;
    }
    float meow_v() volatile {
        return 11.1f;
    }
    float meow_cv() const volatile {
        return 11.1f;
    }

    double purr(int) {
        return 22.2;
    }
    double purr_c(int) const {
        return 22.2;
    }
    double purr_v(int) volatile {
        return 22.2;
    }
    double purr_cv(int) const volatile {
        return 22.2;
    }

    long double hiss(int, int) {
        return 33.3l;
    }
    long double hiss_c(int, int) const {
        return 33.3l;
    }
    long double hiss_v(int, int) volatile {
        return 33.3l;
    }
    long double hiss_cv(int, int) const volatile {
        return 33.3l;
    }
};

// N4659 [depr.func.adaptor.typedefs]/10: "The simple call wrapper returned from a call to mem_fn(pm) shall define two
// nested types named argument_type and result_type as synonyms for cv T* and Ret, respectively, when pm is a pointer to
// member function with cv-qualifier cv and taking no arguments, where Ret is pm's return type."
template <typename Ptr, typename F>
void test_unary(F) {
    STATIC_ASSERT(is_same_v<typename F::argument_type, Ptr>);
    STATIC_ASSERT(is_same_v<typename F::result_type, float>);
}

// N4659 [depr.func.adaptor.typedefs]/11: "The simple call wrapper returned from a call to mem_fn(pm) shall define three
// nested types named first_argument_type, second_argument_type, and result_type as synonyms for cv T*, T1, and Ret,
// respectively, when pm is a pointer to member function with cv-qualifier cv and taking one argument
// of type T1, where Ret is pm's return type."
template <typename Ptr, typename F>
void test_binary(F) {
    STATIC_ASSERT(is_same_v<typename F::first_argument_type, Ptr>);
    STATIC_ASSERT(is_same_v<typename F::second_argument_type, int>);
    STATIC_ASSERT(is_same_v<typename F::result_type, double>);
}

// N4659 [depr.func.adaptor.typedefs]/9: "The simple call wrapper returned from a call to mem_fn(pm) shall have a
// nested type result_type that is a synonym for the return type of pm when pm is a pointer to member function."
template <typename F>
void test_ternary(F) {
    STATIC_ASSERT(is_same_v<typename F::result_type, long double>);
}

int main() {
    test_unary<Cat*>(mem_fn(&Cat::meow));
    test_unary<const Cat*>(mem_fn(&Cat::meow_c));
    test_unary<volatile Cat*>(mem_fn(&Cat::meow_v));
    test_unary<const volatile Cat*>(mem_fn(&Cat::meow_cv));

    test_binary<Cat*>(mem_fn(&Cat::purr));
    test_binary<const Cat*>(mem_fn(&Cat::purr_c));
    test_binary<volatile Cat*>(mem_fn(&Cat::purr_v));
    test_binary<const volatile Cat*>(mem_fn(&Cat::purr_cv));

    test_ternary(mem_fn(&Cat::hiss));
    test_ternary(mem_fn(&Cat::hiss_c));
    test_ternary(mem_fn(&Cat::hiss_v));
    test_ternary(mem_fn(&Cat::hiss_cv));
}
