// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <type_traits> header, part 6
#define TEST_NAME "<type_traits>, part 6"

#define _HAS_DEPRECATED_RESULT_OF 1
#define _SILENCE_CXX17_RESULT_OF_DEPRECATION_WARNING

#include "tdefs.h"
#include "tfuns.h"
#include <type_traits>

struct no_result_type { // struct without nested 'result_type' but
                        // with nested template 'result'
    int operator()() const;
    int operator()(int) const;
    template <class T1>
    struct result { // simple result template
        typedef int type;
    };
};

struct overloaded { // struct with operator() overloaded on argument types
    int operator()(int);
    double operator()(double);
    float operator()(int, float);
    long double operator()(long double, int);
};

void test_main() { // test header <functional>
    // pointer to function
    CHECK_TYPE(STD result_of<tf0()>::type, int);
    CHECK_TYPE(STD result_of<tf1(funobj)>::type, int);
    CHECK_TYPE(STD result_of<tf2(funobj, int)>::type, int);
    CHECK_TYPE(STD result_of<tf3(funobj, int, int)>::type, int);
    CHECK_TYPE(STD result_of<tf4(funobj, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<tf5(funobj, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<tf6(funobj, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<tf7(funobj, int, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<tf8(funobj, int, int, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<tf9(funobj, int, int, int, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<tf10(funobj, int, int, int, int, int, int, int, int, int)>::type, int);

    // pointer to member function applied to object
    CHECK_TYPE(STD result_of<mf1(funobj)>::type, int);
    CHECK_TYPE(STD result_of<mf2(funobj, int)>::type, int);
    CHECK_TYPE(STD result_of<mf3(funobj, int, int)>::type, int);
    CHECK_TYPE(STD result_of<mf4(funobj, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<mf5(funobj, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<mf6(funobj, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<mf7(funobj, int, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<mf8(funobj, int, int, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<mf9(funobj, int, int, int, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<mf10(funobj, int, int, int, int, int, int, int, int, int)>::type, int);

    CHECK_TYPE(STD result_of<cmf1(funobj)>::type, int);
    CHECK_TYPE(STD result_of<cmf2(funobj, int)>::type, int);
    CHECK_TYPE(STD result_of<cmf3(funobj, int, int)>::type, int);
    CHECK_TYPE(STD result_of<cmf4(funobj, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<cmf5(funobj, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<cmf6(funobj, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<cmf7(funobj, int, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<cmf8(funobj, int, int, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<cmf9(funobj, int, int, int, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<cmf10(funobj, int, int, int, int, int, int, int, int, int)>::type, int);

    CHECK_TYPE(STD result_of<vmf1(funobj)>::type, int);
    CHECK_TYPE(STD result_of<vmf2(funobj, int)>::type, int);
    CHECK_TYPE(STD result_of<vmf3(funobj, int, int)>::type, int);
    CHECK_TYPE(STD result_of<vmf4(funobj, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<vmf5(funobj, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<vmf6(funobj, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<vmf7(funobj, int, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<vmf8(funobj, int, int, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<vmf9(funobj, int, int, int, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<vmf10(funobj, int, int, int, int, int, int, int, int, int)>::type, int);

    CHECK_TYPE(STD result_of<cvmf1(funobj)>::type, int);
    CHECK_TYPE(STD result_of<cvmf2(funobj, int)>::type, int);
    CHECK_TYPE(STD result_of<cvmf3(funobj, int, int)>::type, int);
    CHECK_TYPE(STD result_of<cvmf4(funobj, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<cvmf5(funobj, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<cvmf6(funobj, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<cvmf7(funobj, int, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<cvmf8(funobj, int, int, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<cvmf9(funobj, int, int, int, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<cvmf10(funobj, int, int, int, int, int, int, int, int, int)>::type, int);

    // pointer to member function applied to object pointer
    CHECK_TYPE(STD result_of<mf1(funobj*)>::type, int);
    CHECK_TYPE(STD result_of<mf2(funobj*, int)>::type, int);
    CHECK_TYPE(STD result_of<mf3(funobj*, int, int)>::type, int);
    CHECK_TYPE(STD result_of<mf4(funobj*, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<mf5(funobj*, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<mf6(funobj*, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<mf7(funobj*, int, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<mf8(funobj*, int, int, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<mf9(funobj*, int, int, int, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<mf10(funobj*, int, int, int, int, int, int, int, int, int)>::type, int);

    CHECK_TYPE(STD result_of<cmf1(funobj*)>::type, int);
    CHECK_TYPE(STD result_of<cmf2(funobj*, int)>::type, int);
    CHECK_TYPE(STD result_of<cmf3(funobj*, int, int)>::type, int);
    CHECK_TYPE(STD result_of<cmf4(funobj*, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<cmf5(funobj*, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<cmf6(funobj*, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<cmf7(funobj*, int, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<cmf8(funobj*, int, int, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<cmf9(funobj*, int, int, int, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<cmf10(funobj*, int, int, int, int, int, int, int, int, int)>::type, int);

    CHECK_TYPE(STD result_of<vmf1(funobj*)>::type, int);
    CHECK_TYPE(STD result_of<vmf2(funobj*, int)>::type, int);
    CHECK_TYPE(STD result_of<vmf3(funobj*, int, int)>::type, int);
    CHECK_TYPE(STD result_of<vmf4(funobj*, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<vmf5(funobj*, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<vmf6(funobj*, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<vmf7(funobj*, int, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<vmf8(funobj*, int, int, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<vmf9(funobj*, int, int, int, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<vmf10(funobj*, int, int, int, int, int, int, int, int, int)>::type, int);

    CHECK_TYPE(STD result_of<cvmf1(funobj*)>::type, int);
    CHECK_TYPE(STD result_of<cvmf2(funobj*, int)>::type, int);
    CHECK_TYPE(STD result_of<cvmf3(funobj*, int, int)>::type, int);
    CHECK_TYPE(STD result_of<cvmf4(funobj*, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<cvmf5(funobj*, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<cvmf6(funobj*, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<cvmf7(funobj*, int, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<cvmf8(funobj*, int, int, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<cvmf9(funobj*, int, int, int, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<cvmf10(funobj*, int, int, int, int, int, int, int, int, int)>::type, int);

    // pointer to member data
    CHECK_TYPE(STD result_of<md0(funobj)>::type, int);
    CHECK_TYPE(STD result_of<md0(funobj*)>::type, int);
    CHECK_TYPE(STD result_of<md0(const funobj*)>::type, int);
    CHECK_TYPE(STD result_of<md0(volatile funobj*)>::type, int);
    CHECK_TYPE(STD result_of<md0(const volatile funobj*)>::type, int);

    // callable type
    CHECK_TYPE(STD result_of<funobj()>::type, int);
    CHECK_TYPE(STD result_of<funobj(int)>::type, int);
    CHECK_TYPE(STD result_of<funobj(int, int)>::type, int);
    CHECK_TYPE(STD result_of<funobj(int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<funobj(int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<funobj(int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<funobj(int, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<funobj(int, int, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<funobj(int, int, int, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<funobj(int, int, int, int, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<funobj(int, int, int, int, int, int, int, int, int, int)>::type, int);

    CHECK_TYPE(STD result_of<overloaded(int)>::type, int);
    CHECK_TYPE(STD result_of<overloaded(double)>::type, double);
    CHECK_TYPE(STD result_of<overloaded(long double, int)>::type, long double);
    CHECK_TYPE(STD result_of<overloaded(int, float)>::type, float);
}
