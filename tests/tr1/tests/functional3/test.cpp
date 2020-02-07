// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <functional>, part 3
#define TEST_NAME "<functional>, part 3"

#define _HAS_DEPRECATED_RESULT_OF 1
#define _SILENCE_CXX17_RESULT_OF_DEPRECATION_WARNING

#include "tdefs.h"
#include "tfuns.h"
#include <functional>

void t_result_of() { // test result_of for reference_wrapper objects
    // POINTER TO FUNCTION
    CHECK_TYPE(STD result_of<STD reference_wrapper<tf0>()>::type, int);
    CHECK_TYPE(STD result_of<STD reference_wrapper<tf1>(funobj)>::type, int);
    CHECK_TYPE(STD result_of<STD reference_wrapper<tf2>(funobj, int)>::type, int);
    CHECK_TYPE(STD result_of<STD reference_wrapper<tf3>(funobj, int, int)>::type, int);
    CHECK_TYPE(STD result_of<STD reference_wrapper<tf4>(funobj, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<STD reference_wrapper<tf5>(funobj, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<STD reference_wrapper<tf6>(funobj, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<STD reference_wrapper<tf7>(funobj, int, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<STD reference_wrapper<tf8>(funobj, int, int, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<STD reference_wrapper<tf9>(funobj, int, int, int, int, int, int, int, int)>::type, int);
    CHECK_TYPE(
        STD result_of<STD reference_wrapper<tf10>(funobj, int, int, int, int, int, int, int, int, int)>::type, int);

    // FUNCTION OBJECT
    CHECK_TYPE(STD result_of<STD reference_wrapper<funobj>()>::type, int);
    CHECK_TYPE(STD result_of<STD reference_wrapper<funobj>(int)>::type, int);
    CHECK_TYPE(STD result_of<STD reference_wrapper<funobj>(int, int)>::type, int);
    CHECK_TYPE(STD result_of<STD reference_wrapper<funobj>(int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<STD reference_wrapper<funobj>(int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<STD reference_wrapper<funobj>(int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<STD reference_wrapper<funobj>(int, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<STD reference_wrapper<funobj>(int, int, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<STD reference_wrapper<funobj>(int, int, int, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<STD reference_wrapper<funobj>(int, int, int, int, int, int, int, int, int)>::type, int);
    CHECK_TYPE(
        STD result_of<STD reference_wrapper<funobj>(int, int, int, int, int, int, int, int, int, int)>::type, int);

    // POINTER TO MEMBER FUNCTION
    CHECK_TYPE(STD result_of<STD reference_wrapper<mf1>(funobj)>::type, int);
    CHECK_TYPE(STD result_of<STD reference_wrapper<mf2>(funobj, int)>::type, int);
    CHECK_TYPE(STD result_of<STD reference_wrapper<mf3>(funobj, int, int)>::type, int);
    CHECK_TYPE(STD result_of<STD reference_wrapper<mf4>(funobj, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<STD reference_wrapper<mf5>(funobj, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<STD reference_wrapper<mf6>(funobj, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<STD reference_wrapper<mf7>(funobj, int, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<STD reference_wrapper<mf8>(funobj, int, int, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<STD reference_wrapper<mf9>(funobj, int, int, int, int, int, int, int, int)>::type, int);
    CHECK_TYPE(
        STD result_of<STD reference_wrapper<mf10>(funobj, int, int, int, int, int, int, int, int, int)>::type, int);

    CHECK_TYPE(STD result_of<STD reference_wrapper<cmf1>(funobj)>::type, int);
    CHECK_TYPE(STD result_of<STD reference_wrapper<cmf2>(funobj, int)>::type, int);
    CHECK_TYPE(STD result_of<STD reference_wrapper<cmf3>(funobj, int, int)>::type, int);
    CHECK_TYPE(STD result_of<STD reference_wrapper<cmf4>(funobj, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<STD reference_wrapper<cmf5>(funobj, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<STD reference_wrapper<cmf6>(funobj, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<STD reference_wrapper<cmf7>(funobj, int, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<STD reference_wrapper<cmf8>(funobj, int, int, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<STD reference_wrapper<cmf9>(funobj, int, int, int, int, int, int, int, int)>::type, int);
    CHECK_TYPE(
        STD result_of<STD reference_wrapper<cmf10>(funobj, int, int, int, int, int, int, int, int, int)>::type, int);

    CHECK_TYPE(STD result_of<STD reference_wrapper<vmf1>(funobj)>::type, int);
    CHECK_TYPE(STD result_of<STD reference_wrapper<vmf2>(funobj, int)>::type, int);
    CHECK_TYPE(STD result_of<STD reference_wrapper<vmf3>(funobj, int, int)>::type, int);
    CHECK_TYPE(STD result_of<STD reference_wrapper<vmf4>(funobj, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<STD reference_wrapper<vmf5>(funobj, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<STD reference_wrapper<vmf6>(funobj, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<STD reference_wrapper<vmf7>(funobj, int, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<STD reference_wrapper<vmf8>(funobj, int, int, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<STD reference_wrapper<vmf9>(funobj, int, int, int, int, int, int, int, int)>::type, int);
    CHECK_TYPE(
        STD result_of<STD reference_wrapper<vmf10>(funobj, int, int, int, int, int, int, int, int, int)>::type, int);

    CHECK_TYPE(STD result_of<STD reference_wrapper<cvmf1>(funobj)>::type, int);
    CHECK_TYPE(STD result_of<STD reference_wrapper<cvmf2>(funobj, int)>::type, int);
    CHECK_TYPE(STD result_of<STD reference_wrapper<cvmf3>(funobj, int, int)>::type, int);
    CHECK_TYPE(STD result_of<STD reference_wrapper<cvmf4>(funobj, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<STD reference_wrapper<cvmf5>(funobj, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<STD reference_wrapper<cvmf6>(funobj, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<STD reference_wrapper<cvmf7>(funobj, int, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<STD reference_wrapper<cvmf8>(funobj, int, int, int, int, int, int, int)>::type, int);
    CHECK_TYPE(STD result_of<STD reference_wrapper<cvmf9>(funobj, int, int, int, int, int, int, int, int)>::type, int);
    CHECK_TYPE(
        STD result_of<STD reference_wrapper<cvmf10>(funobj, int, int, int, int, int, int, int, int, int)>::type, int);

    // POINTER TO MEMBER DATA
    CHECK_TYPE(STD result_of<STD reference_wrapper<md1>(funobj&)>::type, int*);
    CHECK_TYPE(STD result_of<STD reference_wrapper<md1>(const funobj&)>::type, int*);
    CHECK_TYPE(STD result_of<STD reference_wrapper<md1>(volatile funobj&)>::type, int*);
    CHECK_TYPE(STD result_of<STD reference_wrapper<md1>(const volatile funobj&)>::type, int*);
}

void test_main() { // test header <functional>
    t_result_of();
}
