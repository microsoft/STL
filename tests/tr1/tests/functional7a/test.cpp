// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <functional>, part 7a
#define TEST_NAME "<functional>, part 7a"

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

static void t_bind() { // test STD bind with forced return type
    // POINTER TO FUNCTION
    match_result_type<double>(STD bind<double>(&f0));
    match_result_type<double>(STD bind<double>(&f1, PH1));
    match_result_type<double>(STD bind<double>(&f2, PH1, PH2));
    match_result_type<double>(STD bind<double>(&f3, PH1, PH2, PH3));
    match_result_type<double>(STD bind<double>(&f4, PH1, PH2, PH3, PH4));
    match_result_type<double>(STD bind<double>(&f5, PH1, PH2, PH3, PH4, PH5));
    match_result_type<double>(STD bind<double>(&f6, PH1, PH2, PH3, PH4, PH5, PH6));
    match_result_type<double>(STD bind<double>(&f7, PH1, PH2, PH3, PH4, PH5, PH6, PH7));
    match_result_type<double>(STD bind<double>(&f8, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8));
    match_result_type<double>(STD bind<double>(&f9, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9));
    match_result_type<double>(STD bind<double>(&f10, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9, PH10));

    match_result_type<int>(STD bind<int>(&f0));
    match_result_type<int>(STD bind<int>(&f1, PH1));
    match_result_type<int>(STD bind<int>(&f2, PH1, PH2));
    match_result_type<int>(STD bind<int>(&f3, PH1, PH2, PH3));
    match_result_type<int>(STD bind<int>(&f4, PH1, PH2, PH3, PH4));
    match_result_type<int>(STD bind<int>(&f5, PH1, PH2, PH3, PH4, PH5));
    match_result_type<int>(STD bind<int>(&f6, PH1, PH2, PH3, PH4, PH5, PH6));
    match_result_type<int>(STD bind<int>(&f7, PH1, PH2, PH3, PH4, PH5, PH6, PH7));
    match_result_type<int>(STD bind<int>(&f8, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8));
    match_result_type<int>(STD bind<int>(&f9, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9));
    match_result_type<int>(STD bind<int>(&f10, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9, PH10));

    // FUNCTION OBJECT
    funobj fo;
    match_result_type<double>(STD bind<double>(fo));
    match_result_type<double>(STD bind<double>(fo, PH1));
    match_result_type<double>(STD bind<double>(fo, PH1, PH2));
    match_result_type<double>(STD bind<double>(fo, PH1, PH2, PH3));
    match_result_type<double>(STD bind<double>(fo, PH1, PH2, PH3, PH4));
    match_result_type<double>(STD bind<double>(fo, PH1, PH2, PH3, PH4, PH5));
    match_result_type<double>(STD bind<double>(fo, PH1, PH2, PH3, PH4, PH5, PH6));
    match_result_type<double>(STD bind<double>(fo, PH1, PH2, PH3, PH4, PH5, PH6, PH7));
    match_result_type<double>(STD bind<double>(fo, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8));
    match_result_type<double>(STD bind<double>(fo, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9));
    match_result_type<double>(STD bind<double>(fo, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9, PH10));

    match_result_type<int>(STD bind<int>(fo));
    match_result_type<int>(STD bind<int>(fo, PH1));
    match_result_type<int>(STD bind<int>(fo, PH1, PH2));
    match_result_type<int>(STD bind<int>(fo, PH1, PH2, PH3));
    match_result_type<int>(STD bind<int>(fo, PH1, PH2, PH3, PH4));
    match_result_type<int>(STD bind<int>(fo, PH1, PH2, PH3, PH4, PH5));
    match_result_type<int>(STD bind<int>(fo, PH1, PH2, PH3, PH4, PH5, PH6));
    match_result_type<int>(STD bind<int>(fo, PH1, PH2, PH3, PH4, PH5, PH6, PH7));
    match_result_type<int>(STD bind<int>(fo, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8));
    match_result_type<int>(STD bind<int>(fo, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9));
    match_result_type<int>(STD bind<int>(fo, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9, PH10));

    // POINTER TO MEMBER FUNCTION
    match_result_type<double>(STD bind<double>(&funobj::f1, PH1));
    match_result_type<double>(STD bind<double>(&funobj::cf1, PH1));
    match_result_type<double>(STD bind<double>(&funobj::vf1, PH1));
    match_result_type<double>(STD bind<double>(&funobj::cvf1, PH1));
    match_result_type<double>(STD bind<double>(&funobj::f2, PH1, PH2));
    match_result_type<double>(STD bind<double>(&funobj::cf2, PH1, PH2));
    match_result_type<double>(STD bind<double>(&funobj::vf2, PH1, PH2));
    match_result_type<double>(STD bind<double>(&funobj::cvf2, PH1, PH2));
    match_result_type<double>(STD bind<double>(&funobj::f3, PH1, PH2, PH3));
    match_result_type<double>(STD bind<double>(&funobj::cf3, PH1, PH2, PH3));
    match_result_type<double>(STD bind<double>(&funobj::vf3, PH1, PH2, PH3));
    match_result_type<double>(STD bind<double>(&funobj::cvf3, PH1, PH2, PH3));
    match_result_type<double>(STD bind<double>(&funobj::f4, PH1, PH2, PH3, PH4));
    match_result_type<double>(STD bind<double>(&funobj::cf4, PH1, PH2, PH3, PH4));
    match_result_type<double>(STD bind<double>(&funobj::vf4, PH1, PH2, PH3, PH4));
    match_result_type<double>(STD bind<double>(&funobj::cvf4, PH1, PH2, PH3, PH4));
    match_result_type<double>(STD bind<double>(&funobj::f5, PH1, PH2, PH3, PH4, PH5));
    match_result_type<double>(STD bind<double>(&funobj::cf5, PH1, PH2, PH3, PH4, PH5));
    match_result_type<double>(STD bind<double>(&funobj::vf5, PH1, PH2, PH3, PH4, PH5));
    match_result_type<double>(STD bind<double>(&funobj::cvf5, PH1, PH2, PH3, PH4, PH5));
    match_result_type<double>(STD bind<double>(&funobj::f6, PH1, PH2, PH3, PH4, PH5, PH6));
    match_result_type<double>(STD bind<double>(&funobj::cf6, PH1, PH2, PH3, PH4, PH5, PH6));
    match_result_type<double>(STD bind<double>(&funobj::vf6, PH1, PH2, PH3, PH4, PH5, PH6));
    match_result_type<double>(STD bind<double>(&funobj::cvf6, PH1, PH2, PH3, PH4, PH5, PH6));
    match_result_type<double>(STD bind<double>(&funobj::f7, PH1, PH2, PH3, PH4, PH5, PH6, PH7));
    match_result_type<double>(STD bind<double>(&funobj::cf7, PH1, PH2, PH3, PH4, PH5, PH6, PH7));
    match_result_type<double>(STD bind<double>(&funobj::vf7, PH1, PH2, PH3, PH4, PH5, PH6, PH7));
    match_result_type<double>(STD bind<double>(&funobj::cvf7, PH1, PH2, PH3, PH4, PH5, PH6, PH7));
    match_result_type<double>(STD bind<double>(&funobj::f8, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8));
    match_result_type<double>(STD bind<double>(&funobj::cf8, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8));
    match_result_type<double>(STD bind<double>(&funobj::vf8, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8));
    match_result_type<double>(STD bind<double>(&funobj::cvf8, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8));
    match_result_type<double>(STD bind<double>(&funobj::f9, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9));
    match_result_type<double>(STD bind<double>(&funobj::cf9, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9));
    match_result_type<double>(STD bind<double>(&funobj::vf9, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9));
    match_result_type<double>(STD bind<double>(&funobj::cvf9, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9));
    match_result_type<double>(STD bind<double>(&funobj::f10, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9, PH10));
    match_result_type<double>(STD bind<double>(&funobj::cf10, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9, PH10));
    match_result_type<double>(STD bind<double>(&funobj::vf10, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9, PH10));
    match_result_type<double>(STD bind<double>(&funobj::cvf10, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9, PH10));

    match_result_type<int>(STD bind<int>(&funobj::f1, PH1));
    match_result_type<int>(STD bind<int>(&funobj::cf1, PH1));
    match_result_type<int>(STD bind<int>(&funobj::vf1, PH1));
    match_result_type<int>(STD bind<int>(&funobj::cvf1, PH1));
    match_result_type<int>(STD bind<int>(&funobj::f2, PH1, PH2));
    match_result_type<int>(STD bind<int>(&funobj::cf2, PH1, PH2));
    match_result_type<int>(STD bind<int>(&funobj::vf2, PH1, PH2));
    match_result_type<int>(STD bind<int>(&funobj::cvf2, PH1, PH2));
    match_result_type<int>(STD bind<int>(&funobj::f3, PH1, PH2, PH3));
    match_result_type<int>(STD bind<int>(&funobj::cf3, PH1, PH2, PH3));
    match_result_type<int>(STD bind<int>(&funobj::vf3, PH1, PH2, PH3));
    match_result_type<int>(STD bind<int>(&funobj::cvf3, PH1, PH2, PH3));
    match_result_type<int>(STD bind<int>(&funobj::f4, PH1, PH2, PH3, PH4));
    match_result_type<int>(STD bind<int>(&funobj::cf4, PH1, PH2, PH3, PH4));
    match_result_type<int>(STD bind<int>(&funobj::vf4, PH1, PH2, PH3, PH4));
    match_result_type<int>(STD bind<int>(&funobj::cvf4, PH1, PH2, PH3, PH4));
    match_result_type<int>(STD bind<int>(&funobj::f5, PH1, PH2, PH3, PH4, PH5));
    match_result_type<int>(STD bind<int>(&funobj::cf5, PH1, PH2, PH3, PH4, PH5));
    match_result_type<int>(STD bind<int>(&funobj::vf5, PH1, PH2, PH3, PH4, PH5));
    match_result_type<int>(STD bind<int>(&funobj::cvf5, PH1, PH2, PH3, PH4, PH5));
    match_result_type<int>(STD bind<int>(&funobj::f6, PH1, PH2, PH3, PH4, PH5, PH6));
    match_result_type<int>(STD bind<int>(&funobj::cf6, PH1, PH2, PH3, PH4, PH5, PH6));
    match_result_type<int>(STD bind<int>(&funobj::vf6, PH1, PH2, PH3, PH4, PH5, PH6));
    match_result_type<int>(STD bind<int>(&funobj::cvf6, PH1, PH2, PH3, PH4, PH5, PH6));
    match_result_type<int>(STD bind<int>(&funobj::f7, PH1, PH2, PH3, PH4, PH5, PH6, PH7));
    match_result_type<int>(STD bind<int>(&funobj::cf7, PH1, PH2, PH3, PH4, PH5, PH6, PH7));
    match_result_type<int>(STD bind<int>(&funobj::vf7, PH1, PH2, PH3, PH4, PH5, PH6, PH7));
    match_result_type<int>(STD bind<int>(&funobj::cvf7, PH1, PH2, PH3, PH4, PH5, PH6, PH7));
    match_result_type<int>(STD bind<int>(&funobj::f8, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8));
    match_result_type<int>(STD bind<int>(&funobj::cf8, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8));
    match_result_type<int>(STD bind<int>(&funobj::vf8, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8));
    match_result_type<int>(STD bind<int>(&funobj::cvf8, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8));
    match_result_type<int>(STD bind<int>(&funobj::f9, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9));
    match_result_type<int>(STD bind<int>(&funobj::cf9, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9));
    match_result_type<int>(STD bind<int>(&funobj::vf9, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9));
    match_result_type<int>(STD bind<int>(&funobj::cvf9, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9));
    match_result_type<int>(STD bind<int>(&funobj::f10, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9, PH10));
    match_result_type<int>(STD bind<int>(&funobj::cf10, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9, PH10));
    match_result_type<int>(STD bind<int>(&funobj::vf10, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9, PH10));
    match_result_type<int>(STD bind<int>(&funobj::cvf10, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9, PH10));

    // POINTER TO MEMBER DATA
    CALL1(STD bind<double>(&funobj::i0, PH1));
    match_result_type<double>(STD bind<double>(&funobj::i0, PH1));
}

void test_main() { // test header <functional>
    t_bind();
}
