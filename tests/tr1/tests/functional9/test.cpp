// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <functional>, part 9
#define TEST_NAME "<functional>, part 9"

#include "tdefs.h"
#include "tfuns.h"
#include <functional>

#define PH1  STD placeholders::_1
#define PH2  STD placeholders::_2
#define PH3  STD placeholders::_3
#define PH4  STD placeholders::_4
#define PH5  STD placeholders::_5
#define PH6  STD placeholders::_6
#define PH7  STD placeholders::_7
#define PH8  STD placeholders::_8
#define PH9  STD placeholders::_9
#define PH10 STD placeholders::_10

template <class Wrapper>
void call1(Wrapper wrapper) {
    CHECK_INT(wrapper(funobj(2)), 2);
}

template <class Wrapper>
void call2(Wrapper wrapper) {
    CHECK_INT(wrapper(funobj(2), 3), 5);
}

template <class Wrapper>
void call3(Wrapper wrapper) {
    CHECK_INT(wrapper(funobj(2), 3, 4), 9);
}

template <class Wrapper>
void call4(Wrapper wrapper) {
    CHECK_INT(wrapper(funobj(2), 3, 4, 5), 14);
}

template <class Wrapper>
void call5(Wrapper wrapper) {
    CHECK_INT(wrapper(funobj(2), 3, 4, 5, 6), 20);
}

int sum0() {
    return 0;
}
int sum1(int v1) {
    return v1;
}
int sum4(int v1, int v2, int v3, int v4) {
    return v1 + v2 + v3 + v4;
}

static void t_invoke() { // test invoke
    CHECK_INT(STD invoke(sum0), 0);
    CHECK_INT(STD invoke(sum1, 5), 5);
    CHECK_INT(STD invoke(sum4, 1, 3, 5, 7), 16);
}

void test_main() { // test header <functional>
    // reference_wrapper::operator() with rvalues
    call1(STD cref(fake_lvalue(&f1)));
    call2(STD cref(fake_lvalue(&f2)));
    call3(STD cref(fake_lvalue(&f3)));
    call4(STD cref(fake_lvalue(&f4)));
    call5(STD cref(fake_lvalue(&f5)));

    call1(STD cref(fake_lvalue(&funobj::cf1)));
    call2(STD cref(fake_lvalue(&funobj::cf2)));
    call3(STD cref(fake_lvalue(&funobj::cf3)));
    call4(STD cref(fake_lvalue(&funobj::cf4)));
    call5(STD cref(fake_lvalue(&funobj::cf5)));

    call1(funobj());
    call2(funobj());
    call3(funobj());
    call4(funobj());
    call5(funobj());

    call1(STD mem_fn(&funobj::cf1));
    call2(STD mem_fn(&funobj::cf2));
    call3(STD mem_fn(&funobj::cf3));
    call4(STD mem_fn(&funobj::cf4));
    call5(STD mem_fn(&funobj::cf5));

    call1(STD bind(&funobj::cf1, PH1));
    call2(STD bind(&funobj::cf2, PH1, PH2));
    call3(STD bind(&funobj::cf3, PH1, PH2, PH3));
    call4(STD bind(&funobj::cf4, PH1, PH2, PH3, PH4));
    call5(STD bind(&funobj::cf5, PH1, PH2, PH3, PH4, PH5));

    t_invoke();
}
