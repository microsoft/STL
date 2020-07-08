// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <functional>, part 7
#define TEST_NAME "<functional>, part 7"

#define _HAS_DEPRECATED_ADAPTOR_TYPEDEFS 1
#define _SILENCE_CXX17_ADAPTOR_TYPEDEFS_DEPRECATION_WARNING

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

static void t_bind() { // test STD bind
    // POINTER TO MEMBER FUNCTION
    CALL1m(STD bind(&funobj::f1, PH1));
    CALL1m(STD bind(&funobj::cf1, PH1));
    CALL1m(STD bind(&funobj::vf1, PH1));
    CALL1m(STD bind(&funobj::cvf1, PH1));
    CALL2m(STD bind(&funobj::f2, PH1, PH2));
    CALL2m(STD bind(&funobj::cf2, PH1, PH2));
    CALL2m(STD bind(&funobj::vf2, PH1, PH2));
    CALL2m(STD bind(&funobj::cvf2, PH1, PH2));
    CALL3m(STD bind(&funobj::f3, PH1, PH2, PH3));
    CALL3m(STD bind(&funobj::cf3, PH1, PH2, PH3));
    CALL3m(STD bind(&funobj::vf3, PH1, PH2, PH3));
    CALL3m(STD bind(&funobj::cvf3, PH1, PH2, PH3));
    CALL4m(STD bind(&funobj::f4, PH1, PH2, PH3, PH4));
    CALL4m(STD bind(&funobj::cf4, PH1, PH2, PH3, PH4));
    CALL4m(STD bind(&funobj::vf4, PH1, PH2, PH3, PH4));
    CALL4m(STD bind(&funobj::cvf4, PH1, PH2, PH3, PH4));
    CALL5m(STD bind(&funobj::f5, PH1, PH2, PH3, PH4, PH5));
    CALL5m(STD bind(&funobj::cf5, PH1, PH2, PH3, PH4, PH5));
    CALL5m(STD bind(&funobj::vf5, PH1, PH2, PH3, PH4, PH5));
    CALL5m(STD bind(&funobj::cvf5, PH1, PH2, PH3, PH4, PH5));
    CALL6m(STD bind(&funobj::f6, PH1, PH2, PH3, PH4, PH5, PH6));
    CALL6m(STD bind(&funobj::cf6, PH1, PH2, PH3, PH4, PH5, PH6));
    CALL6m(STD bind(&funobj::vf6, PH1, PH2, PH3, PH4, PH5, PH6));
    CALL6m(STD bind(&funobj::cvf6, PH1, PH2, PH3, PH4, PH5, PH6));
    CALL7m(STD bind(&funobj::f7, PH1, PH2, PH3, PH4, PH5, PH6, PH7));
    CALL7m(STD bind(&funobj::cf7, PH1, PH2, PH3, PH4, PH5, PH6, PH7));
    CALL7m(STD bind(&funobj::vf7, PH1, PH2, PH3, PH4, PH5, PH6, PH7));
    CALL7m(STD bind(&funobj::cvf7, PH1, PH2, PH3, PH4, PH5, PH6, PH7));
    CALL8m(STD bind(&funobj::f8, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8));
    CALL8m(STD bind(&funobj::cf8, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8));
    CALL8m(STD bind(&funobj::vf8, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8));
    CALL8m(STD bind(&funobj::cvf8, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8));
    CALL9m(STD bind(&funobj::f9, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9));
    CALL9m(STD bind(&funobj::cf9, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9));
    CALL9m(STD bind(&funobj::vf9, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9));
    CALL9m(STD bind(&funobj::cvf9, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9));
    CALL10m(STD bind(&funobj::f10, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9, PH10));
    CALL10m(STD bind(&funobj::cf10, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9, PH10));
    CALL10m(STD bind(&funobj::vf10, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9, PH10));
    CALL10m(STD bind(&funobj::cvf10, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9, PH10));

    match_result_type<int>(STD bind(&funobj::f1, PH1));
    match_result_type<int>(STD bind(&funobj::cf1, PH1));
    match_result_type<int>(STD bind(&funobj::vf1, PH1));
    match_result_type<int>(STD bind(&funobj::cvf1, PH1));
    match_result_type<int>(STD bind(&funobj::f2, PH1, PH2));
    match_result_type<int>(STD bind(&funobj::cf2, PH1, PH2));
    match_result_type<int>(STD bind(&funobj::vf2, PH1, PH2));
    match_result_type<int>(STD bind(&funobj::cvf2, PH1, PH2));
    match_result_type<int>(STD bind(&funobj::f3, PH1, PH2, PH3));
    match_result_type<int>(STD bind(&funobj::cf3, PH1, PH2, PH3));
    match_result_type<int>(STD bind(&funobj::vf3, PH1, PH2, PH3));
    match_result_type<int>(STD bind(&funobj::cvf3, PH1, PH2, PH3));
    match_result_type<int>(STD bind(&funobj::f4, PH1, PH2, PH3, PH4));
    match_result_type<int>(STD bind(&funobj::cf4, PH1, PH2, PH3, PH4));
    match_result_type<int>(STD bind(&funobj::vf4, PH1, PH2, PH3, PH4));
    match_result_type<int>(STD bind(&funobj::cvf4, PH1, PH2, PH3, PH4));
    match_result_type<int>(STD bind(&funobj::f5, PH1, PH2, PH3, PH4, PH5));
    match_result_type<int>(STD bind(&funobj::cf5, PH1, PH2, PH3, PH4, PH5));
    match_result_type<int>(STD bind(&funobj::vf5, PH1, PH2, PH3, PH4, PH5));
    match_result_type<int>(STD bind(&funobj::cvf5, PH1, PH2, PH3, PH4, PH5));
    match_result_type<int>(STD bind(&funobj::f6, PH1, PH2, PH3, PH4, PH5, PH6));
    match_result_type<int>(STD bind(&funobj::cf6, PH1, PH2, PH3, PH4, PH5, PH6));
    match_result_type<int>(STD bind(&funobj::vf6, PH1, PH2, PH3, PH4, PH5, PH6));
    match_result_type<int>(STD bind(&funobj::cvf6, PH1, PH2, PH3, PH4, PH5, PH6));
    match_result_type<int>(STD bind(&funobj::f7, PH1, PH2, PH3, PH4, PH5, PH6, PH7));
    match_result_type<int>(STD bind(&funobj::cf7, PH1, PH2, PH3, PH4, PH5, PH6, PH7));
    match_result_type<int>(STD bind(&funobj::vf7, PH1, PH2, PH3, PH4, PH5, PH6, PH7));
    match_result_type<int>(STD bind(&funobj::cvf7, PH1, PH2, PH3, PH4, PH5, PH6, PH7));
    match_result_type<int>(STD bind(&funobj::f8, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8));
    match_result_type<int>(STD bind(&funobj::cf8, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8));
    match_result_type<int>(STD bind(&funobj::vf8, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8));
    match_result_type<int>(STD bind(&funobj::cvf8, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8));
    match_result_type<int>(STD bind(&funobj::f9, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9));
    match_result_type<int>(STD bind(&funobj::cf9, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9));
    match_result_type<int>(STD bind(&funobj::vf9, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9));
    match_result_type<int>(STD bind(&funobj::cvf9, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9));
    match_result_type<int>(STD bind(&funobj::f10, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9, PH10));
    match_result_type<int>(STD bind(&funobj::cf10, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9, PH10));
    match_result_type<int>(STD bind(&funobj::vf10, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9, PH10));
    match_result_type<int>(STD bind(&funobj::cvf10, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9, PH10));

    // POINTER TO MEMBER DATA
    CALL1(STD bind(&funobj::i0, PH1));
    CHECK_INT(has_result_type(STD bind(&funobj::i0, PH1)), false);
}

void test_main() { // test header <functional>
    t_bind();
}
