// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <functional>, part 5
#define TEST_NAME "<functional>, part 5"

#define _HAS_DEPRECATED_RESULT_OF 1
#define _SILENCE_CXX17_RESULT_OF_DEPRECATION_WARNING

#include "tdefs.h"
#include "tfuns.h"
#include <functional>

static void t_mem_fn() { // test STD mem_fn
    // POINTER TO MEMBER FUNCTION
    CALL1m(STD mem_fn(&funobj::f1));
    CALL1m(STD mem_fn(&funobj::cf1));
    CALL1m(STD mem_fn(&funobj::vf1));
    CALL1m(STD mem_fn(&funobj::cvf1));
    CALL2m(STD mem_fn(&funobj::f2));
    CALL2m(STD mem_fn(&funobj::cf2));
    CALL2m(STD mem_fn(&funobj::vf2));
    CALL2m(STD mem_fn(&funobj::cvf2));
    CALL3m(STD mem_fn(&funobj::f3));
    CALL3m(STD mem_fn(&funobj::cf3));
    CALL3m(STD mem_fn(&funobj::vf3));
    CALL3m(STD mem_fn(&funobj::cvf3));
    CALL4m(STD mem_fn(&funobj::f4));
    CALL4m(STD mem_fn(&funobj::cf4));
    CALL4m(STD mem_fn(&funobj::vf4));
    CALL4m(STD mem_fn(&funobj::cvf4));
    CALL5m(STD mem_fn(&funobj::f5));
    CALL5m(STD mem_fn(&funobj::cf5));
    CALL5m(STD mem_fn(&funobj::vf5));
    CALL5m(STD mem_fn(&funobj::cvf5));
    CALL6m(STD mem_fn(&funobj::f6));
    CALL6m(STD mem_fn(&funobj::cf6));
    CALL6m(STD mem_fn(&funobj::vf6));
    CALL6m(STD mem_fn(&funobj::cvf6));
    CALL7m(STD mem_fn(&funobj::f7));
    CALL7m(STD mem_fn(&funobj::cf7));
    CALL7m(STD mem_fn(&funobj::vf7));
    CALL7m(STD mem_fn(&funobj::cvf7));
    CALL8m(STD mem_fn(&funobj::f8));
    CALL8m(STD mem_fn(&funobj::cf8));
    CALL8m(STD mem_fn(&funobj::vf8));
    CALL8m(STD mem_fn(&funobj::cvf8));
    CALL9m(STD mem_fn(&funobj::f9));
    CALL9m(STD mem_fn(&funobj::cf9));
    CALL9m(STD mem_fn(&funobj::vf9));
    CALL9m(STD mem_fn(&funobj::cvf9));
    CALL10m(STD mem_fn(&funobj::f10));
    CALL10m(STD mem_fn(&funobj::cf10));
    CALL10m(STD mem_fn(&funobj::vf10));
    CALL10m(STD mem_fn(&funobj::cvf10));

    // POINTER TO MEMBER DATA
    CALL1m(STD mem_fn(&funobj::i0));
}

#define CHECK_MEM_FN1(fn, r)  check_mem_fn(fn, r, __FILE__, __LINE__)
#define CHECK_MEM_FN2(fn, r)  check_mem_fn(fn, r, 1, __FILE__, __LINE__)
#define CHECK_MEM_FN3(fn, r)  check_mem_fn(fn, r, 1, 2, __FILE__, __LINE__)
#define CHECK_MEM_FN4(fn, r)  check_mem_fn(fn, r, 1, 2, 3, __FILE__, __LINE__)
#define CHECK_MEM_FN5(fn, r)  check_mem_fn(fn, r, 1, 2, 3, 4, __FILE__, __LINE__)
#define CHECK_MEM_FN6(fn, r)  check_mem_fn(fn, r, 1, 2, 3, 4, 5, __FILE__, __LINE__)
#define CHECK_MEM_FN7(fn, r)  check_mem_fn(fn, r, 1, 2, 3, 4, 5, 6, __FILE__, __LINE__)
#define CHECK_MEM_FN8(fn, r)  check_mem_fn(fn, r, 1, 2, 3, 4, 5, 6, 7, __FILE__, __LINE__)
#define CHECK_MEM_FN9(fn, r)  check_mem_fn(fn, r, 1, 2, 3, 4, 5, 6, 7, 8, __FILE__, __LINE__)
#define CHECK_MEM_FN10(fn, r) check_mem_fn(fn, r, 1, 2, 3, 4, 5, 6, 7, 8, 9, __FILE__, __LINE__)

template <class Fty, class Ret>
void check_mem_fn(Fty, Ret, const char* file_name, int line_number) {
    CHECK_TYPE_LONG(typename STD result_of<Fty(funobj)>::type, Ret, file_name, line_number);
}

template <class Fty, class Ret>
void check_mem_fn(Fty, Ret, int, const char* file_name, int line_number) {
    CHECK_TYPE_LONG(typename STD result_of<Fty(funobj, int)>::type, Ret, file_name, line_number);
}

template <class Fty, class Ret>
void check_mem_fn(Fty, Ret, int, int, const char* file_name, int line_number) {
    CHECK_TYPE_LONG(typename STD result_of<Fty(funobj, int, int)>::type, Ret, file_name, line_number);
}

template <class Fty, class Ret>
void check_mem_fn(Fty, Ret, int, int, int, const char* file_name, int line_number) {
    CHECK_TYPE_LONG(typename STD result_of<Fty(funobj, int, int, int)>::type, Ret, file_name, line_number);
}

template <class Fty, class Ret>
void check_mem_fn(Fty, Ret, int, int, int, int, const char* file_name, int line_number) {
    CHECK_TYPE_LONG(typename STD result_of<Fty(funobj, int, int, int, int)>::type, Ret, file_name, line_number);
}

template <class Fty, class Ret>
void check_mem_fn(Fty, Ret, int, int, int, int, int, const char* file_name, int line_number) {
    CHECK_TYPE_LONG(typename STD result_of<Fty(funobj, int, int, int, int, int)>::type, Ret, file_name, line_number);
}

template <class Fty, class Ret>
void check_mem_fn(Fty, Ret, int, int, int, int, int, int, const char* file_name, int line_number) {
    CHECK_TYPE_LONG(
        typename STD result_of<Fty(funobj, int, int, int, int, int, int)>::type, Ret, file_name, line_number);
}

template <class Fty, class Ret>
void check_mem_fn(Fty, Ret, int, int, int, int, int, int, int, const char* file_name, int line_number) {
    CHECK_TYPE_LONG(
        typename STD result_of<Fty(funobj, int, int, int, int, int, int, int)>::type, Ret, file_name, line_number);
}

template <class Fty, class Ret>
void check_mem_fn(Fty, Ret, int, int, int, int, int, int, int, int, const char* file_name, int line_number) {
    CHECK_TYPE_LONG(
        typename STD result_of<Fty(funobj, int, int, int, int, int, int, int, int)>::type, Ret, file_name, line_number);
}

template <class Fty, class Ret>
void check_mem_fn(Fty, Ret, int, int, int, int, int, int, int, int, int, const char* file_name, int line_number) {
    CHECK_TYPE_LONG(typename STD result_of<Fty(funobj, int, int, int, int, int, int, int, int, int)>::type, Ret,
        file_name, line_number);
}

void t_result_of() { // test result_of for mem_fn objects
    // POINTER TO MEMBER FUNCTION
    CHECK_MEM_FN1(&funobj::f1, 0);
    CHECK_MEM_FN2(&funobj::f2, 0);
    CHECK_MEM_FN3(&funobj::f3, 0);
    CHECK_MEM_FN4(&funobj::f4, 0);
    CHECK_MEM_FN5(&funobj::f5, 0);
    CHECK_MEM_FN6(&funobj::f6, 0);
    CHECK_MEM_FN7(&funobj::f7, 0);
    CHECK_MEM_FN8(&funobj::f8, 0);
    CHECK_MEM_FN9(&funobj::f9, 0);
    CHECK_MEM_FN10(&funobj::f10, 0);

    CHECK_MEM_FN1(&funobj::cf1, 0);
    CHECK_MEM_FN2(&funobj::cf2, 0);
    CHECK_MEM_FN3(&funobj::cf3, 0);
    CHECK_MEM_FN4(&funobj::cf4, 0);
    CHECK_MEM_FN5(&funobj::cf5, 0);
    CHECK_MEM_FN6(&funobj::cf6, 0);
    CHECK_MEM_FN7(&funobj::cf7, 0);
    CHECK_MEM_FN8(&funobj::cf8, 0);
    CHECK_MEM_FN9(&funobj::cf9, 0);
    CHECK_MEM_FN10(&funobj::cf10, 0);

    CHECK_MEM_FN1(&funobj::vf1, 0);
    CHECK_MEM_FN2(&funobj::vf2, 0);
    CHECK_MEM_FN3(&funobj::vf3, 0);
    CHECK_MEM_FN4(&funobj::vf4, 0);
    CHECK_MEM_FN5(&funobj::vf5, 0);
    CHECK_MEM_FN6(&funobj::vf6, 0);
    CHECK_MEM_FN7(&funobj::vf7, 0);
    CHECK_MEM_FN8(&funobj::vf8, 0);
    CHECK_MEM_FN9(&funobj::vf9, 0);
    CHECK_MEM_FN10(&funobj::vf10, 0);

    CHECK_MEM_FN1(&funobj::cvf1, 0);
    CHECK_MEM_FN2(&funobj::cvf2, 0);
    CHECK_MEM_FN3(&funobj::cvf3, 0);
    CHECK_MEM_FN4(&funobj::cvf4, 0);
    CHECK_MEM_FN5(&funobj::cvf5, 0);
    CHECK_MEM_FN6(&funobj::cvf6, 0);
    CHECK_MEM_FN7(&funobj::cvf7, 0);
    CHECK_MEM_FN8(&funobj::cvf8, 0);
    CHECK_MEM_FN9(&funobj::cvf9, 0);
    CHECK_MEM_FN10(&funobj::cvf10, 0);

    CHECK_MEM_FN1(&funobj::i0, 0);
}

void test_main() { // test header <functional>
    t_mem_fn();
    t_result_of();
}
