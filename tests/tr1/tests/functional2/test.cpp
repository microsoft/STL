// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <functional>, part 2
#define TEST_NAME "<functional>, part 2"

#include "tdefs.h"
#include "tfuns.h"
#include <functional>

void t_reference_wrapper() { // test reference_wrapper's operator()
    // POINTER TO FUNCTION
    CALL0(STD cref(fake_lvalue(&f0)));
    CALL1(STD cref(fake_lvalue(&f1)));
    CALL2x(STD cref(fake_lvalue(&f2)), funobj);
    CALL3x(STD cref(fake_lvalue(&f3)), funobj);
    CALL4x(STD cref(fake_lvalue(&f4)), funobj);
    CALL5x(STD cref(fake_lvalue(&f5)), funobj);
    CALL6x(STD cref(fake_lvalue(&f6)), funobj);
    CALL7x(STD cref(fake_lvalue(&f7)), funobj);
    CALL8x(STD cref(fake_lvalue(&f8)), funobj);
    CALL9x(STD cref(fake_lvalue(&f9)), funobj);
    CALL10x(STD cref(fake_lvalue(&f10)), funobj);

    // FUNCTION OBJECT
    funobj fo;
    CALL0(STD cref(fo));
    CALL1(STD cref(fo));
    CALL2(STD cref(fo));
    CALL3(STD cref(fo));
    CALL4(STD cref(fo));
    CALL5(STD cref(fo));
    CALL6(STD cref(fo));
    CALL7(STD cref(fo));
    CALL8(STD cref(fo));
    CALL9(STD cref(fo));
    CALL10(STD cref(fo));

    CALL0(STD ref(fo));
    CALL1(STD ref(fo));
    CALL2(STD ref(fo));
    CALL3(STD ref(fo));
    CALL4(STD ref(fo));
    CALL5(STD ref(fo));
    CALL6(STD ref(fo));
    CALL7(STD ref(fo));
    CALL8(STD ref(fo));
    CALL9(STD ref(fo));
    CALL10(STD ref(fo));

    // POINTER TO MEMBER FUNCTION
    CALL1m(STD cref(fake_lvalue(&funobj::f1)));
    CALL1m(STD cref(fake_lvalue(&funobj::cf1)));
    CALL1m(STD cref(fake_lvalue(&funobj::vf1)));
    CALL1m(STD cref(fake_lvalue(&funobj::cvf1)));
    CALL2m(STD cref(fake_lvalue(&funobj::f2)));
    CALL2m(STD cref(fake_lvalue(&funobj::cf2)));
    CALL2m(STD cref(fake_lvalue(&funobj::vf2)));
    CALL2m(STD cref(fake_lvalue(&funobj::cvf2)));
    CALL3m(STD cref(fake_lvalue(&funobj::f3)));
    CALL3m(STD cref(fake_lvalue(&funobj::cf3)));
    CALL3m(STD cref(fake_lvalue(&funobj::vf3)));
    CALL3m(STD cref(fake_lvalue(&funobj::cvf3)));
    CALL4m(STD cref(fake_lvalue(&funobj::f4)));
    CALL4m(STD cref(fake_lvalue(&funobj::cf4)));
    CALL4m(STD cref(fake_lvalue(&funobj::vf4)));
    CALL4m(STD cref(fake_lvalue(&funobj::cvf4)));
    CALL5m(STD cref(fake_lvalue(&funobj::f5)));
    CALL5m(STD cref(fake_lvalue(&funobj::cf5)));
    CALL5m(STD cref(fake_lvalue(&funobj::vf5)));
    CALL5m(STD cref(fake_lvalue(&funobj::cvf5)));
    CALL6m(STD cref(fake_lvalue(&funobj::f6)));
    CALL6m(STD cref(fake_lvalue(&funobj::cf6)));
    CALL6m(STD cref(fake_lvalue(&funobj::vf6)));
    CALL6m(STD cref(fake_lvalue(&funobj::cvf6)));
    CALL7m(STD cref(fake_lvalue(&funobj::f7)));
    CALL7m(STD cref(fake_lvalue(&funobj::cf7)));
    CALL7m(STD cref(fake_lvalue(&funobj::vf7)));
    CALL7m(STD cref(fake_lvalue(&funobj::cvf7)));
    CALL8m(STD cref(fake_lvalue(&funobj::f8)));
    CALL8m(STD cref(fake_lvalue(&funobj::cf8)));
    CALL8m(STD cref(fake_lvalue(&funobj::vf8)));
    CALL8m(STD cref(fake_lvalue(&funobj::cvf8)));
    CALL9m(STD cref(fake_lvalue(&funobj::f9)));
    CALL9m(STD cref(fake_lvalue(&funobj::cf9)));
    CALL9m(STD cref(fake_lvalue(&funobj::vf9)));
    CALL9m(STD cref(fake_lvalue(&funobj::cvf9)));
    CALL10m(STD cref(fake_lvalue(&funobj::f10)));
    CALL10m(STD cref(fake_lvalue(&funobj::cf10)));
    CALL10m(STD cref(fake_lvalue(&funobj::vf10)));
    CALL10m(STD cref(fake_lvalue(&funobj::cvf10)));

    // POINTER TO MEMBER DATA
    CALL1m(STD cref(fake_lvalue(&funobj::i0)));

    // ref(reference_wrapper<T>)
    int i = 3;

    STD reference_wrapper<int> ri = STD ref(STD ref(i));
    CHECK_INT(ri.get(), 3);
    ri.get() = 4;
    CHECK_INT(i, 4);
    STD reference_wrapper<const int> cri = STD cref(STD cref(i));
    CHECK_INT(cri.get(), 4);
}

void test_main() { // test header <functional>
    t_reference_wrapper();
}
