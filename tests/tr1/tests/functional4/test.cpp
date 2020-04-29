// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <functional>, part 4
#define TEST_NAME "<functional>, part 4"

#pragma warning(disable : 5215) // '%s' a function parameter with a volatile qualified type is deprecated in C++20

#include "tdefs.h"
#include "tfuns.h"
#include <functional>

#define CHECK_BASE_OPS(fn, res)     \
    CHECK_INT(fn == nullptr, res);  \
    CHECK_INT(nullptr == fn, res);  \
    CHECK_INT(fn != nullptr, !res); \
    CHECK_INT(nullptr != fn, !res)

#define CHECK_OPS(fn, tgt, tgt_t)                     \
    CHECK_BASE_OPS(fn, false);                        \
    CHECK_INT(fn.target_type() == typeid(tgt), true); \
    CHECK_INT(fn.target<tgt_t>() == nullptr, false)

#define CHECK_NULL_OPS(fn)    \
    CHECK_BASE_OPS(fn, true); \
    CHECK_INT(fn.target_type() == typeid(void), true)

void t_function() { // test STD function
    typedef STD function<int()> Fty0;
    typedef STD function<int(funobj)> Fty1;
    typedef STD function<int(funobj, int)> Fty2;
    typedef STD function<int(funobj, int, int)> Fty3;
    typedef STD function<int(funobj, int, int, int)> Fty4;

    typedef STD function<int(funobj, int, int, int, int)> Fty5;
    typedef STD function<int(funobj, int, int, int, int, int)> Fty6;
    typedef STD function<int(funobj, int, int, int, int, int, int)> Fty7;
    typedef STD function<int(funobj, int, int, int, int, int, int, int)> Fty8;
    typedef STD function<int(funobj, int, int, int, int, int, int, int, int)> Fty9;
    typedef STD function<int(funobj, int, int, int, int, int, int, int, int, int)> Fty10;

    // CONSTRUCT FROM NULL POINTER CONSTANT
    Fty0 fobj0p(nullptr);
    CHECK_NULL_OPS(fobj0p);
    Fty1 fobj1p(nullptr);
    CHECK_NULL_OPS(fobj1p);
    Fty2 fobj2p(nullptr);
    CHECK_NULL_OPS(fobj2p);
    Fty3 fobj3p(nullptr);
    CHECK_NULL_OPS(fobj3p);
    Fty4 fobj4p(nullptr);
    CHECK_NULL_OPS(fobj4p);

    Fty5 fobj5p(nullptr);
    CHECK_NULL_OPS(fobj5p);
    Fty6 fobj6p(nullptr);
    CHECK_NULL_OPS(fobj6p);
    Fty7 fobj7p(nullptr);
    CHECK_NULL_OPS(fobj7p);
    Fty8 fobj8p(nullptr);
    CHECK_NULL_OPS(fobj8p);
    Fty9 fobj9p(nullptr);
    CHECK_NULL_OPS(fobj9p);
    Fty10 fobj10p(nullptr);
    CHECK_NULL_OPS(fobj10p);

    Fty0 fobj0;
    CHECK_NULL_OPS(fobj0);
    Fty1 fobj1;
    CHECK_NULL_OPS(fobj1);
    Fty2 fobj2;
    CHECK_NULL_OPS(fobj2);
    Fty3 fobj3;
    CHECK_NULL_OPS(fobj3);
    Fty4 fobj4;
    CHECK_NULL_OPS(fobj4);

    Fty5 fobj5;
    CHECK_NULL_OPS(fobj5);
    Fty6 fobj6;
    CHECK_NULL_OPS(fobj6);
    Fty7 fobj7;
    CHECK_NULL_OPS(fobj7);
    Fty8 fobj8;
    CHECK_NULL_OPS(fobj8);
    Fty9 fobj9;
    CHECK_NULL_OPS(fobj9);
    Fty10 fobj10;
    CHECK_NULL_OPS(fobj10);

    // POINTER TO FUNCTION
#if _HAS_FUNCTION_ALLOCATOR_SUPPORT
    STD allocator<int> al;
    Fty0 fobj0x(STD allocator_arg, al, fobj0);

    Fty0 fobj0y(STD move(fobj0x));
    fobj0x = STD move(fobj0y);

    fobj0y = &f0;

    Fty0 fobj0z(STD allocator_arg, al, STD move(fobj0y));

    Fty0 fobj0a1(STD allocator_arg, al);
    Fty0 fobj0a2(STD allocator_arg, al, nullptr);
    Fty0 fobj0a3(STD allocator_arg, al, fobj0a1);
    Fty0 fobj0a4(STD allocator_arg, al, &f0);

    typedef STD uses_allocator<Fty0, STD allocator<int>> function_uses_allocator;
    CHECK_INT(function_uses_allocator::value, true);
#endif // _HAS_FUNCTION_ALLOCATOR_SUPPORT

    fobj0 = &f0;
    CHECK_OPS(fobj0, &f0, tf0);
    CALL0(fobj0);
    fobj0 = nullptr;
    CHECK_NULL_OPS(fobj0);
    fobj1 = &f1;
    CHECK_OPS(fobj1, &f1, tf1);
    CALL1(fobj1);
    fobj1 = nullptr;
    CHECK_NULL_OPS(fobj1);
    fobj2 = &f2;
    CHECK_OPS(fobj2, &f2, tf2);
    CALL2(fobj2);
    fobj2 = nullptr;
    CHECK_NULL_OPS(fobj2);
    fobj3 = &f3;
    CHECK_OPS(fobj3, &f3, tf3);
    CALL3(fobj3);
    fobj3 = nullptr;
    CHECK_NULL_OPS(fobj3);
    fobj4 = &f4;
    CHECK_OPS(fobj4, &f4, tf4);
    CALL4(fobj4);
    fobj4 = nullptr;
    CHECK_NULL_OPS(fobj4);

    fobj5 = &f5;
    CHECK_OPS(fobj5, &f5, tf5);
    CALL5(fobj5);
    fobj5 = nullptr;
    CHECK_NULL_OPS(fobj5);
    fobj6 = &f6;
    CHECK_OPS(fobj6, &f6, tf6);
    CALL6(fobj6);
    fobj6 = nullptr;
    CHECK_NULL_OPS(fobj6);
    fobj7 = &f7;
    CHECK_OPS(fobj7, &f7, tf7);
    CALL7(fobj7);
    fobj7 = nullptr;
    CHECK_NULL_OPS(fobj7);
    fobj8 = &f8;
    CHECK_OPS(fobj8, &f8, tf8);
    CALL8(fobj8);
    fobj8 = nullptr;
    CHECK_NULL_OPS(fobj8);
    fobj9 = &f9;
    CHECK_OPS(fobj9, &f9, tf9);
    CALL9(fobj9);
    fobj9 = nullptr;
    CHECK_NULL_OPS(fobj9);
    fobj10 = &f10;
    CHECK_OPS(fobj10, &f10, tf10);
    CALL10(fobj10);
    fobj10 = nullptr;
    CHECK_NULL_OPS(fobj10);

    // FUNCTION OBJECT
    funobj fo;
    fobj0 = fo;
    CHECK_OPS(fobj0, fo, funobj);
    CALL0(fobj0);
    fobj1 = fo;
    CHECK_OPS(fobj1, fo, funobj);
    CALL1(fobj1);
    fobj2 = fo;
    CHECK_OPS(fobj2, fo, funobj);
    CALL2(fobj2);
    fobj3 = fo;
    CHECK_OPS(fobj3, fo, funobj);
    CALL3(fobj3);
    fobj4 = fo;
    CHECK_OPS(fobj4, fo, funobj);
    CALL4(fobj4);

    fobj5 = fo;
    CHECK_OPS(fobj5, fo, funobj);
    CALL5(fobj5);
    fobj6 = fo;
    CHECK_OPS(fobj6, fo, funobj);
    CALL6(fobj6);
    fobj7 = fo;
    CHECK_OPS(fobj7, fo, funobj);
    CALL7(fobj7);
    fobj8 = fo;
    CHECK_OPS(fobj8, fo, funobj);
    CALL8(fobj8);
    fobj9 = fo;
    CHECK_OPS(fobj9, fo, funobj);
    CALL9(fobj9);
    fobj10 = fo;
    CHECK_OPS(fobj10, fo, funobj);
    CALL10(fobj10);

    // POINTER TO MEMBER FUNCTION
    fobj1 = &funobj::f1;
    CHECK_OPS(fobj1, &funobj::f1, mf1);
    CALL1(fobj1);
    fobj1 = &funobj::cf1;
    CHECK_OPS(fobj1, &funobj::cf1, cmf1);
    CALL1(fobj1);
    fobj1 = &funobj::vf1;
    CHECK_OPS(fobj1, &funobj::vf1, vmf1);
    CALL1(fobj1);
    fobj1 = &funobj::cvf1;
    CHECK_OPS(fobj1, &funobj::cvf1, cvmf1);
    CALL1(fobj1);
    fobj1 = nullptr;

    fobj2 = &funobj::f2;
    CHECK_OPS(fobj2, &funobj::f2, mf2);
    CALL2(fobj2);
    fobj2 = &funobj::cf2;
    CHECK_OPS(fobj2, &funobj::cf2, cmf2);
    CALL2(fobj2);
    fobj2 = &funobj::vf2;
    CHECK_OPS(fobj2, &funobj::vf2, vmf2);
    CALL2(fobj2);
    fobj2 = &funobj::cvf2;
    CHECK_OPS(fobj2, &funobj::cvf2, cvmf2);
    CALL2(fobj2);
    fobj2 = nullptr;

    fobj3 = &funobj::f3;
    CHECK_OPS(fobj3, &funobj::f3, mf3);
    CALL3(fobj3);
    fobj3 = &funobj::cf3;
    CHECK_OPS(fobj3, &funobj::cf3, cmf3);
    CALL3(fobj3);
    fobj3 = &funobj::vf3;
    CHECK_OPS(fobj3, &funobj::vf3, vmf3);
    CALL3(fobj3);
    fobj3 = &funobj::cvf3;
    CHECK_OPS(fobj3, &funobj::cvf3, cvmf3);
    CALL3(fobj3);
    fobj3 = nullptr;

    fobj4 = &funobj::f4;
    CHECK_OPS(fobj4, &funobj::f4, mf4);
    CALL4(fobj4);
    fobj4 = &funobj::cf4;
    CHECK_OPS(fobj4, &funobj::cf4, cmf4);
    CALL4(fobj4);
    fobj4 = &funobj::vf4;
    CHECK_OPS(fobj4, &funobj::vf4, vmf4);
    CALL4(fobj4);
    fobj4 = &funobj::cvf4;
    CHECK_OPS(fobj4, &funobj::cvf4, cvmf4);
    CALL4(fobj4);
    fobj4 = nullptr;

    fobj5 = &funobj::f5;
    CHECK_OPS(fobj5, &funobj::f5, mf5);
    CALL5(fobj5);
    fobj5 = &funobj::cf5;
    CHECK_OPS(fobj5, &funobj::cf5, cmf5);
    CALL5(fobj5);
    fobj5 = &funobj::vf5;
    CHECK_OPS(fobj5, &funobj::vf5, vmf5);
    CALL5(fobj5);
    fobj5 = &funobj::cvf5;
    CHECK_OPS(fobj5, &funobj::cvf5, cvmf5);
    CALL5(fobj5);
    fobj5 = nullptr;

    fobj6 = &funobj::f6;
    CHECK_OPS(fobj6, &funobj::f6, mf6);
    CALL6(fobj6);
    fobj6 = &funobj::cf6;
    CHECK_OPS(fobj6, &funobj::cf6, cmf6);
    CALL6(fobj6);
    fobj6 = &funobj::vf6;
    CHECK_OPS(fobj6, &funobj::vf6, vmf6);
    CALL6(fobj6);
    fobj6 = &funobj::cvf6;
    CHECK_OPS(fobj6, &funobj::cvf6, cvmf6);
    CALL6(fobj6);
    fobj6 = nullptr;

    fobj7 = &funobj::f7;
    CHECK_OPS(fobj7, &funobj::f7, mf7);
    CALL7(fobj7);
    fobj7 = &funobj::cf7;
    CHECK_OPS(fobj7, &funobj::cf7, cmf7);
    CALL7(fobj7);
    fobj7 = &funobj::vf7;
    CHECK_OPS(fobj7, &funobj::vf7, vmf7);
    CALL7(fobj7);
    fobj7 = &funobj::cvf7;
    CHECK_OPS(fobj7, &funobj::cvf7, cvmf7);
    CALL7(fobj7);
    fobj7 = nullptr;

    fobj8 = &funobj::f8;
    CHECK_OPS(fobj8, &funobj::f8, mf8);
    CALL8(fobj8);
    fobj8 = &funobj::cf8;
    CHECK_OPS(fobj8, &funobj::cf8, cmf8);
    CALL8(fobj8);
    fobj8 = &funobj::vf8;
    CHECK_OPS(fobj8, &funobj::vf8, vmf8);
    CALL8(fobj8);
    fobj8 = &funobj::cvf8;
    CHECK_OPS(fobj8, &funobj::cvf8, cvmf8);
    CALL8(fobj8);
    fobj8 = nullptr;

    fobj9 = &funobj::f9;
    CHECK_OPS(fobj9, &funobj::f9, mf9);
    CALL9(fobj9);
    fobj9 = &funobj::cf9;
    CHECK_OPS(fobj9, &funobj::cf9, cmf9);
    CALL9(fobj9);
    fobj9 = &funobj::vf9;
    CHECK_OPS(fobj9, &funobj::vf9, vmf9);
    CALL9(fobj9);
    fobj9 = &funobj::cvf9;
    CHECK_OPS(fobj9, &funobj::cvf9, cvmf9);
    CALL9(fobj9);
    fobj9 = nullptr;

    fobj10 = &funobj::f10;
    CHECK_OPS(fobj10, &funobj::f10, mf10);
    CALL10(fobj10);
    fobj10 = &funobj::cf10;
    CHECK_OPS(fobj10, &funobj::cf10, cmf10);
    CALL10(fobj10);
    fobj10 = &funobj::vf10;
    CHECK_OPS(fobj10, &funobj::vf10, vmf10);
    CALL10(fobj10);
    fobj10 = &funobj::cvf10;
    CHECK_OPS(fobj10, &funobj::cvf10, cvmf10);
    CALL10(fobj10);
    fobj10 = nullptr;

    // POINTER TO MEMBER DATA
    fobj1 = &funobj::i0;
    CHECK_OPS(fobj1, &funobj::i0, md0);
    CALL1(fobj1);
}

static int fun1(int i) { // static function with one argument
    return i;
}

static int fun2(int i) { // static function with one argument
    return 2 * i;
}

struct Fat {
    Fat(int barg = 0) : base(barg) { // construct
    }

    int operator()(int i) { // member operator with one argument
        return i + base;
    }

    int base;
    char pad[200];
};

void t_swap() { // test function swap
    typedef STD function<int(int)> F1;
    F1 f1 = &fun1;
    CHECK_INT(f1(3), 3);
    F1 f2 = &fun2;
    CHECK_INT(f2(3), 6);
    STD swap(f1, f1);
    CHECK_INT(f1(3), 3);

    STD swap(f1, f2);
    CHECK_INT(f1(3), 6);
    CHECK_INT(f2(3), 3);
    STD swap(f1, f2);
    CHECK_INT(f1(3), 3);
    CHECK_INT(f2(3), 6);

    Fat fun3(1);
    F1 f3 = fun3;
    CHECK_INT(f3(5), 6);
    STD swap(f2, f3);
    CHECK_INT(f2(5), 6);
    CHECK_INT(f3(5), 10);
    STD swap(f2, f3);
    CHECK_INT(f2(5), 10);
    CHECK_INT(f3(5), 6);

    Fat fun4(2);
    F1 f4 = fun4;
    CHECK_INT(f4(5), 7);
    STD swap(f3, f4);
    CHECK_INT(f3(5), 7);
    CHECK_INT(f4(5), 6);
}

void test_main() { // test header <functional>
    t_swap();
    t_function();
}
