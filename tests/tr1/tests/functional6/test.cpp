// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <functional>, part 6
#define TEST_NAME "<functional>, part 6"

#define _HAS_DEPRECATED_ADAPTOR_TYPEDEFS 1
#define _SILENCE_CXX17_ADAPTOR_TYPEDEFS_DEPRECATION_WARNING

#include "tdefs.h"
#include "tfuns.h"
#include <functional>
#include <iostream>

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

static void t_bad() { // test bad_function_call
#if NO_EXCEPTIONS
#else // NO_EXCEPTIONS
    typedef double(Fd)(double);
    typedef STD function<Fd> Myfunc;
    bool ok = false;

    try { // test exception for empty function object
        Myfunc fd1;
        STD cout << fd1(3) << STD endl;
    } catch (const STD bad_function_call&) { // handle bad call
        ok = true;
    }
    CHECK_MSG("empty function throws bad_function_call", ok);
#endif // NO_EXCEPTIONS
}

static void t_bind() { // test STD bind
    bool ans;
    typedef decltype(STD bind(&f0)) bind0;
    CHECK(STD is_bind_expression<bind0>::value);
    ans = STD is_convertible<STD is_bind_expression<bind0>, STD true_type>::value;
    CHECK(ans);

    typedef decltype(PH3) ph3;
    CHECK_INT(STD is_placeholder<ph3>::value, 3);
    ans = STD is_convertible<STD is_placeholder<ph3>, STD integral_constant<int, 3>>::value;
    CHECK(ans);

    CHECK(!STD is_bind_expression<int>::value);
    ans = STD is_convertible<STD is_bind_expression<int>, STD false_type>::value;
    CHECK(ans);

    CHECK_INT(STD is_placeholder<int>::value, 0);
    ans = STD is_convertible<STD is_placeholder<int>, STD integral_constant<int, 0>>::value;
    CHECK(ans);

    // POINTER TO FUNCTION
    CALL0(STD bind(&f0));
    CALL1(STD bind(&f1, PH1));
    CALL2(STD bind(&f2, PH1, PH2));
    CALL3(STD bind(&f3, PH1, PH2, PH3));
    CALL4(STD bind(&f4, PH1, PH2, PH3, PH4));
    CALL5(STD bind(&f5, PH1, PH2, PH3, PH4, PH5));
    CALL6(STD bind(&f6, PH1, PH2, PH3, PH4, PH5, PH6));
    CALL7(STD bind(&f7, PH1, PH2, PH3, PH4, PH5, PH6, PH7));
    CALL8(STD bind(&f8, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8));
    CALL9(STD bind(&f9, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9));
    CALL10(STD bind(&f10, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9, PH10));

    match_result_type<int>(STD bind(&f0));
    match_result_type<int>(STD bind(&f1, PH1));
    match_result_type<int>(STD bind(&f2, PH1, PH2));
    match_result_type<int>(STD bind(&f3, PH1, PH2, PH3));
    match_result_type<int>(STD bind(&f4, PH1, PH2, PH3, PH4));
    match_result_type<int>(STD bind(&f5, PH1, PH2, PH3, PH4, PH5));
    match_result_type<int>(STD bind(&f6, PH1, PH2, PH3, PH4, PH5, PH6));
    match_result_type<int>(STD bind(&f7, PH1, PH2, PH3, PH4, PH5, PH6, PH7));
    match_result_type<int>(STD bind(&f8, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8));
    match_result_type<int>(STD bind(&f9, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9));
    match_result_type<int>(STD bind(&f10, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9, PH10));

    // FUNCTION OBJECT
    funobj fo;
    CALL0(STD bind(fo));
    CALL1(STD bind(fo, PH1));
    CALL2(STD bind(fo, PH1, PH2));
    CALL3(STD bind(fo, PH1, PH2, PH3));
    CALL4(STD bind(fo, PH1, PH2, PH3, PH4));
    CALL5(STD bind(fo, PH1, PH2, PH3, PH4, PH5));
    CALL6(STD bind(fo, PH1, PH2, PH3, PH4, PH5, PH6));
    CALL7(STD bind(fo, PH1, PH2, PH3, PH4, PH5, PH6, PH7));
    CALL8(STD bind(fo, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8));
    CALL9(STD bind(fo, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9));
    CALL10(STD bind(fo, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9, PH10));

    match_result_type<int>(STD bind(fo));
    match_result_type<int>(STD bind(fo, PH1));
    match_result_type<int>(STD bind(fo, PH1, PH2));
    match_result_type<int>(STD bind(fo, PH1, PH2, PH3));
    match_result_type<int>(STD bind(fo, PH1, PH2, PH3, PH4));
    match_result_type<int>(STD bind(fo, PH1, PH2, PH3, PH4, PH5));
    match_result_type<int>(STD bind(fo, PH1, PH2, PH3, PH4, PH5, PH6));
    match_result_type<int>(STD bind(fo, PH1, PH2, PH3, PH4, PH5, PH6, PH7));
    match_result_type<int>(STD bind(fo, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8));
    match_result_type<int>(STD bind(fo, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9));
    match_result_type<int>(STD bind(fo, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9, PH10));
}

struct funobj_no_nested { // callable type with no nested result_type
    template <class Fty>
    struct result { // make non-decltype version of result_of happy
        typedef int type;
    };
    int operator()();
    template <class T1>
    int operator()(T1);
    template <class T1, class T2>
    int operator()(T1, T2);
    template <class T1, class T2, class T3>
    int operator()(T1, T2, T3);
    template <class T1, class T2, class T3, class T4>
    int operator()(T1, T2, T3, T4);
};

static void t_bind_no_nested() {
    funobj_no_nested fn;
    CHECK_INT(has_result_type(STD bind(fn)), false);
    CHECK_INT(has_result_type(STD bind(fn, PH1)), false);
    CHECK_INT(has_result_type(STD bind(fn, PH1, PH2)), false);
    CHECK_INT(has_result_type(STD bind(fn, PH1, PH2, PH3)), false);
    CHECK_INT(has_result_type(STD bind(fn, PH1, PH2, PH3, PH4)), false);
}

void test_main() { // test header <functional>
    t_bad();
    t_bind();
    t_bind_no_nested();
}
