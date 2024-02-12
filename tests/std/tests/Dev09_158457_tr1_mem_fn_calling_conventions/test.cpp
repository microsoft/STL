// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <functional>
#include <type_traits>

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

using namespace std;

#if defined(_M_IX86) && _M_IX86_FP < 2
#error On x86, this test assumes _M_IX86_FP >= 2
#endif

struct Cat {
    int a() {
        return 2000000;
    }

#if !defined(_M_CEE_PURE)
    int __thiscall b() {
        return 200000;
    }
#else
    int b() {
        return 100000;
    }
#endif

#if defined(_M_IX86) && !defined(_M_CEE_PURE)
    int __cdecl c() {
        return 20000;
    }
    int __stdcall d() {
        return 2000;
    }
#else
    int c() {
        return 10000;
    }
    int d() {
        return 1000;
    }
#endif

#if defined(_M_IX86) && !defined(_M_CEE)
    int __fastcall e() {
        return 200;
    }
#else
    int e() {
        return 100;
    }
#endif

#if defined(_M_CEE)
    int __clrcall f() {
        return 20;
    }
#else
    int f() {
        return 10;
    }
#endif

#if (defined(_M_IX86) || defined(_M_X64)) && !defined(_M_CEE)
    int __vectorcall g() {
        return 2;
    }
#else
    int g() {
        return 1;
    }
#endif
};

int u(int i) {
    return -400000 * i;
}

#if defined(_M_IX86) && !defined(_M_CEE_PURE)
int __cdecl v(int i) {
    return -40000 * i;
}
int __stdcall w(int i) {
    return -4000 * i;
}
#else
int v(int i) {
    return -30000 * i;
}
int w(int i) {
    return -3000 * i;
}
#endif

#if defined(_M_IX86) && !defined(_M_CEE)
int __fastcall x(int i) {
    return -400 * i;
}
#else
int x(int i) {
    return -300 * i;
}
#endif

#if defined(_M_CEE)
int __clrcall y(int i) {
    return -40 * i;
}
#else
int y(int i) {
    return -30 * i;
}
#endif

#if (defined(_M_IX86) || defined(_M_X64)) && !defined(_M_CEE)
int __vectorcall z(int i) {
    return -4 * i;
}
#else
int z(int i) {
    return -3 * i;
}
#endif

#if defined(_M_IX86)
#if !defined(_M_CEE)
const int member_correct = 2222212; // x86 native
const int free_correct   = 444434; // x86 native
#elif !defined(_M_CEE_PURE)
const int member_correct = 2222121; // x86 ijw
const int free_correct   = 444343; // x86 ijw
#else
const int member_correct = 2111121; // x86 pure
const int free_correct   = 433343; // x86 pure
#endif
#elif defined(_M_X64)
#if !defined(_M_CEE)
const int member_correct = 2211112; // x64 native
const int free_correct   = 433334; // x64 native
#elif !defined(_M_CEE_PURE)
const int member_correct = 2211121; // x64 ijw
const int free_correct   = 433343; // x64 ijw
#else
const int member_correct = 2111121; // x64 pure
const int free_correct   = 433343; // x64 pure
#endif
#else
#if !defined(_M_CEE)
const int member_correct = 2211111; // arm/arm64 native
const int free_correct   = 433333; // arm/arm64 native
#elif !defined(_M_CEE_PURE)
const int member_correct = 2211121; // arm/arm64 ijw
const int free_correct   = 433343; // arm/arm64 ijw
#else
const int member_correct = 2111121; // arm/arm64 pure
const int free_correct   = 433343; // arm/arm64 pure
#endif
#endif

STATIC_ASSERT(is_member_function_pointer_v<decltype(&Cat::a)>);
STATIC_ASSERT(is_member_function_pointer_v<decltype(&Cat::b)>);
STATIC_ASSERT(is_member_function_pointer_v<decltype(&Cat::c)>);
STATIC_ASSERT(is_member_function_pointer_v<decltype(&Cat::d)>);
STATIC_ASSERT(is_member_function_pointer_v<decltype(&Cat::e)>);
STATIC_ASSERT(is_member_function_pointer_v<decltype(&Cat::f)>);
STATIC_ASSERT(is_member_function_pointer_v<decltype(&Cat::g)>);

STATIC_ASSERT(is_member_pointer_v<decltype(&Cat::a)>);
STATIC_ASSERT(is_member_pointer_v<decltype(&Cat::b)>);
STATIC_ASSERT(is_member_pointer_v<decltype(&Cat::c)>);
STATIC_ASSERT(is_member_pointer_v<decltype(&Cat::d)>);
STATIC_ASSERT(is_member_pointer_v<decltype(&Cat::e)>);
STATIC_ASSERT(is_member_pointer_v<decltype(&Cat::f)>);
STATIC_ASSERT(is_member_pointer_v<decltype(&Cat::g)>);

STATIC_ASSERT(is_function_v<decltype(u)>);
STATIC_ASSERT(is_function_v<decltype(v)>);
STATIC_ASSERT(is_function_v<decltype(w)>);
STATIC_ASSERT(is_function_v<decltype(x)>);
STATIC_ASSERT(is_function_v<decltype(y)>);
STATIC_ASSERT(is_function_v<decltype(z)>);


// use_mem_fn() works around the fact that we can't always decompose and recompose PMF types. Consider
// x86 /Gd (the default), where non-members default to __cdecl and members default to __thiscall. When
// a member has been marked as __cdecl, like &Cat::c, we can't give it to mem_fn(). We're required to
// provide the signature mem_fn(R T::*). Template argument deduction decomposes the PMF type into an
// ordinary function type, then substitution recomposes the PMF type. This works for default calling
// conventions (__thiscall PMFs become __cdecl non-members and back). It also works for fancy calling
// conventions (e.g. __stdcall PMFs become __stdcall non-members and back). However, it fails when the
// PMF's calling convention is what non-members have by default. A __cdecl PMF becomes a __cdecl
// non-member, then recomposition produces a __thiscall PMF. use_mem_fn() allows us to ignore this
// problem. When recomposition succeeds (as it usually does), both use_mem_fn(R Cat::*) and
// use_mem_fn(PMF) are viable, and use_mem_fn(R Cat::*) is chosen because it's more specialized. When
// recomposition fails, use_mem_fn(R Cat::*) is non-viable, so use_mem_fn(PMF) is chosen. (In Standard
// code, it's entirely possible for template argument deduction to succeed, but for the overload to be
// non-viable.)

template <typename R>
int use_mem_fn(R Cat::*pmf) {
    Cat cat;
    return mem_fn(pmf)(cat);
}

template <typename PMF>
int use_mem_fn(PMF pmf) {
    Cat cat;
    return (cat.*pmf)();
}

#ifdef _M_CEE_PURE
int main() {
#else
int __cdecl main() {
#endif
    Cat cat;
    int n = 0;

    n = 0;
    n += use_mem_fn(&Cat::a);
    n += use_mem_fn(&Cat::b);
    n += use_mem_fn(&Cat::c);
    n += use_mem_fn(&Cat::d);
    n += use_mem_fn(&Cat::e);
    n += use_mem_fn(&Cat::f);
    n += use_mem_fn(&Cat::g);
    assert(n == member_correct);

    n = 0;
    n += bind(&Cat::a, cat)();
    n += bind(&Cat::b, cat)();
    n += bind(&Cat::c, cat)();
    n += bind(&Cat::d, cat)();
    n += bind(&Cat::e, cat)();
    n += bind(&Cat::f, cat)();
    n += bind(&Cat::g, cat)();
    assert(n == member_correct);

    n = 0;
    n += bind(u, -1)();
    n += bind(v, -1)();
    n += bind(w, -1)();
    n += bind(x, -1)();
    n += bind(y, -1)();
    n += bind(z, -1)();
    assert(n == free_correct);

    n = 0;
    n += bind(&u, -1)();
    n += bind(&v, -1)();
    n += bind(&w, -1)();
    n += bind(&x, -1)();
    n += bind(&y, -1)();
    n += bind(&z, -1)();
    assert(n == free_correct);

    n = 0;
    n += function<int(Cat&)>(&Cat::a)(cat);
    n += function<int(Cat&)>(&Cat::b)(cat);
    n += function<int(Cat&)>(&Cat::c)(cat);
    n += function<int(Cat&)>(&Cat::d)(cat);
    n += function<int(Cat&)>(&Cat::e)(cat);
    n += function<int(Cat&)>(&Cat::f)(cat);
    n += function<int(Cat&)>(&Cat::g)(cat);
    assert(n == member_correct);

    n = 0;
    n += function<int(int)>(u)(-1);
    n += function<int(int)>(v)(-1);
    n += function<int(int)>(w)(-1);
    n += function<int(int)>(x)(-1);
    n += function<int(int)>(y)(-1);
    n += function<int(int)>(z)(-1);
    assert(n == free_correct);

    n = 0;
    n += function<int(int)>(&u)(-1);
    n += function<int(int)>(&v)(-1);
    n += function<int(int)>(&w)(-1);
    n += function<int(int)>(&x)(-1);
    n += function<int(int)>(&y)(-1);
    n += function<int(int)>(&z)(-1);
    assert(n == free_correct);

    auto pmf_a = &Cat::a;
    auto pmf_b = &Cat::b;
    auto pmf_c = &Cat::c;
    auto pmf_d = &Cat::d;
    auto pmf_e = &Cat::e;
    auto pmf_f = &Cat::f;
    auto pmf_g = &Cat::g;

    n = 0;
    n += ref(pmf_a)(cat);
    n += ref(pmf_b)(cat);
    n += ref(pmf_c)(cat);
    n += ref(pmf_d)(cat);
    n += ref(pmf_e)(cat);
    n += ref(pmf_f)(cat);
    n += ref(pmf_g)(cat);
    assert(n == member_correct);

    n = 0;
    n += cref(pmf_a)(cat);
    n += cref(pmf_b)(cat);
    n += cref(pmf_c)(cat);
    n += cref(pmf_d)(cat);
    n += cref(pmf_e)(cat);
    n += cref(pmf_f)(cat);
    n += cref(pmf_g)(cat);
    assert(n == member_correct);

    n = 0;
    n += ref(u)(-1);
    n += ref(v)(-1);
    n += ref(w)(-1);
    n += ref(x)(-1);
    n += ref(y)(-1);
    n += ref(z)(-1);
    assert(n == free_correct);

    n = 0;
    n += cref(u)(-1);
    n += cref(v)(-1);
    n += cref(w)(-1);
    n += cref(x)(-1);
    n += cref(y)(-1);
    n += cref(z)(-1);
    assert(n == free_correct);

    auto pf_u = &u;
    auto pf_v = &v;
    auto pf_w = &w;
    auto pf_x = &x;
    auto pf_y = &y;
    auto pf_z = &z;

    n = 0;
    n += ref(pf_u)(-1);
    n += ref(pf_v)(-1);
    n += ref(pf_w)(-1);
    n += ref(pf_x)(-1);
    n += ref(pf_y)(-1);
    n += ref(pf_z)(-1);
    assert(n == free_correct);

    n = 0;
    n += cref(pf_u)(-1);
    n += cref(pf_v)(-1);
    n += cref(pf_w)(-1);
    n += cref(pf_x)(-1);
    n += cref(pf_y)(-1);
    n += cref(pf_z)(-1);
    assert(n == free_correct);
}
