// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <functional>, part 8
#define TEST_NAME "<functional>, part 8"

#include "tdefs.h"
#include <functional>
#include <iostream>

template <class T>
const T& fake_lvalue(const T&& t) { // C++11 12.2 [class.temporary]/5: "A temporary bound to a reference
                                    // parameter in a function call (5.2.2) persists until the completion
                                    // of the full-expression containing the call."
    return t;
}

void returns_void() {}

char f0() {
    return 0;
}

int f1(int) {
    return 1;
}

double f2(int, int) {
    return 2.0;
}

char f3(int, int, int) {
    return 3;
}

int f4(int, int, int, int) {
    return 4;
}

double f5(int, int, int, int, int) {
    return 5.0;
}

char f6(int, int, int, int, int, int) {
    return 6;
}

int f7(int, int, int, int, int, int, int) {
    return 7;
}

double f8(int, int, int, int, int, int, int, int) {
    return 8.0;
}

char f9(int, int, int, int, int, int, int, int, int) {
    return 9;
}

int f10(int, int, int, int, int, int, int, int, int, int) {
    return 10;
}

struct S {
    S() : di(1), df(2.0), cdd(3.0), vdd(4.0), cvdd(5.0) { // default constructor
    }

    S(const S& x) : di(x.di), df(x.df), cdd(x.cdd), vdd(x.vdd), cvdd(x.cvdd) { // copy constructor
    }

    S(const volatile S& x) : di(x.di), df(x.df), cdd(x.cdd), vdd(x.vdd), cvdd(x.cvdd) { // volatile copy constructor
    }

    S& operator=(const S&); // to quiet diagnostics

    int di;
    float df;
    const double cdd;
    volatile double vdd;
    const volatile double cvdd;

    int& gdi() {
        return di;
    }

    const int& gdi() const {
        return di;
    }

    volatile int& gdi() volatile {
        return di;
    }

    const volatile int& gdi() const volatile {
        return di;
    }

    void returns_void() {}

    char f0() {
        return 0;
    }

    int f1(int) {
        return 1;
    }

    double f2(int, int) {
        return 2.0;
    }

    char f3(int, int, int) {
        return 3;
    }

    int f4(int, int, int, int) {
        return 4;
    }

    double f5(int, int, int, int, int) {
        return 5.0;
    }

    char f6(int, int, int, int, int, int) {
        return 6;
    }

    int f7(int, int, int, int, int, int, int) {
        return 7;
    }

    double f8(int, int, int, int, int, int, int, int) {
        return 8.0;
    }

    char f9(int, int, int, int, int, int, int, int, int) {
        return 9;
    }

    char cf0() const {
        return 10;
    }

    int cf1(int) const {
        return 11;
    }

    double cf2(int, int) const {
        return 12.0;
    }

    char cf3(int, int, int) const {
        return 13;
    }

    int cf4(int, int, int, int) const {
        return 14;
    }

    double cf5(int, int, int, int, int) const {
        return 15.0;
    }

    char cf6(int, int, int, int, int, int) const {
        return 16;
    }

    int cf7(int, int, int, int, int, int, int) const {
        return 17;
    }

    double cf8(int, int, int, int, int, int, int, int) const {
        return 18.0;
    }

    char cf9(int, int, int, int, int, int, int, int, int) const {
        return 19;
    }

    char vf0() volatile {
        return 20;
    }

    int vf1(int) volatile {
        return 21;
    }

    double vf2(int, int) volatile {
        return 22.0;
    }

    char vf3(int, int, int) volatile {
        return 23;
    }

    int vf4(int, int, int, int) volatile {
        return 24;
    }

    double vf5(int, int, int, int, int) volatile {
        return 25.0;
    }

    char vf6(int, int, int, int, int, int) volatile {
        return 26;
    }

    int vf7(int, int, int, int, int, int, int) volatile {
        return 27;
    }

    double vf8(int, int, int, int, int, int, int, int) volatile {
        return 28.0;
    }

    char vf9(int, int, int, int, int, int, int, int, int) volatile {
        return 29;
    }

    char cvf0() const volatile {
        return 30;
    }

    int cvf1(int) const volatile {
        return 31;
    }

    double cvf2(int, int) const volatile {
        return 32.0;
    }

    char cvf3(int, int, int) const volatile {
        return 33;
    }

    int cvf4(int, int, int, int) const volatile {
        return 34;
    }

    double cvf5(int, int, int, int, int) const volatile {
        return 35.0;
    }

    char cvf6(int, int, int, int, int, int) const volatile {
        return 36;
    }

    int cvf7(int, int, int, int, int, int, int) const volatile {
        return 37;
    }

    double cvf8(int, int, int, int, int, int, int, int) const volatile {
        return 38.0;
    }

    char cvf9(int, int, int, int, int, int, int, int, int) const volatile {
        return 39;
    }
};

struct Fn0 {
    char operator()() const {
        return 0;
    }
};

struct Fn1 {
    int operator()(int) const {
        return 1;
    }
};

struct Fn2 {
    double operator()(int, int) const {
        return 2.0;
    }
};

struct Fn3 {
    char operator()(int, int, int) const {
        return 3;
    }
};

struct Fn4 {
    int operator()(int, int, int, int) const {
        return 4;
    }
};

struct Fn5 {
    double operator()(int, int, int, int, int) const {
        return 5.0;
    }
};

struct Fn6 {
    char operator()(int, int, int, int, int, int) const {
        return 6;
    }
};

struct Fn7 {
    int operator()(int, int, int, int, int, int, int) const {
        return 7;
    }
};

struct Fn8 {
    double operator()(int, int, int, int, int, int, int, int) const {
        return 8.0;
    }
};

struct Fn9 {
    char operator()(int, int, int, int, int, int, int, int, int) const {
        return 9;
    }
};

struct Fn10 {
    int operator()(int, int, int, int, int, int, int, int, int, int) const {
        return 10;
    }
};

struct S0 {};

static void tref0() {
    S s;
    const S cs;
    volatile S vs;
    const volatile S cvs;
    S* sp                  = &s;
    const S* csp           = &cs;
    volatile S* vsp        = &vs;
    const volatile S* cvsp = &cvs;
    int one                = 1;

    // reference_wrapper FOR DATA MEMBERS
    // CALL WITH OBJECTS
    CHECK_TYPE(int, STD cref(fake_lvalue(&S::di))(s));
    CHECK_INT(s.di, STD cref(fake_lvalue(&S::di))(s));
    CHECK_TYPE(float, STD cref(fake_lvalue(&S::df))(s));
    CHECK_DOUBLE(s.df, STD cref(fake_lvalue(&S::df))(s));
    CHECK_TYPE(double, STD cref(fake_lvalue(&S::cdd))(s));
    CHECK_DOUBLE(s.cdd, STD cref(fake_lvalue(&S::cdd))(s));

    CHECK_TYPE(int, STD cref(fake_lvalue(&S::di))(cs));
    CHECK_INT(cs.di, STD cref(fake_lvalue(&S::di))(cs));
    CHECK_TYPE(float, STD cref(fake_lvalue(&S::df))(cs));
    CHECK_DOUBLE(cs.df, STD cref(fake_lvalue(&S::df))(cs));
    CHECK_TYPE(double, STD cref(fake_lvalue(&S::cdd))(cs));
    CHECK_DOUBLE(cs.cdd, STD cref(fake_lvalue(&S::cdd))(cs));

    CHECK_TYPE(int, STD cref(fake_lvalue(&S::di))(vs));
    CHECK_INT(vs.di, STD cref(fake_lvalue(&S::di))(vs));
    CHECK_TYPE(float, STD cref(fake_lvalue(&S::df))(vs));
    CHECK_DOUBLE(vs.df, STD cref(fake_lvalue(&S::df))(vs));
    CHECK_TYPE(double, STD cref(fake_lvalue(&S::cdd))(vs));
    CHECK_DOUBLE(vs.cdd, STD cref(fake_lvalue(&S::cdd))(vs));

    CHECK_TYPE(int, STD cref(fake_lvalue(&S::di))(cvs));
    CHECK_INT(cvs.di, STD cref(fake_lvalue(&S::di))(cvs));
    CHECK_TYPE(float, STD cref(fake_lvalue(&S::df))(cvs));
    CHECK_DOUBLE(cvs.df, STD cref(fake_lvalue(&S::df))(cvs));
    CHECK_TYPE(double, STD cref(fake_lvalue(&S::cdd))(cvs));
    CHECK_DOUBLE(cvs.cdd, STD cref(fake_lvalue(&S::cdd))(cvs));

    // call with pointers
    CHECK_TYPE(int, STD cref(fake_lvalue(&S::di))(sp));
    CHECK_INT(s.di, STD cref(fake_lvalue(&S::di))(sp));
    CHECK_TYPE(float, STD cref(fake_lvalue(&S::df))(sp));
    CHECK_DOUBLE(s.df, STD cref(fake_lvalue(&S::df))(sp));
    CHECK_TYPE(double, STD cref(fake_lvalue(&S::cdd))(sp));
    CHECK_DOUBLE(s.cdd, STD cref(fake_lvalue(&S::cdd))(sp));

    CHECK_TYPE(int, STD cref(fake_lvalue(&S::di))(csp));
    CHECK_INT(cs.di, STD cref(fake_lvalue(&S::di))(csp));
    CHECK_TYPE(float, STD cref(fake_lvalue(&S::df))(csp));
    CHECK_DOUBLE(cs.df, STD cref(fake_lvalue(&S::df))(csp));
    CHECK_TYPE(double, STD cref(fake_lvalue(&S::cdd))(csp));
    CHECK_DOUBLE(cs.cdd, STD cref(fake_lvalue(&S::cdd))(csp));

    CHECK_TYPE(int, STD cref(fake_lvalue(&S::di))(vsp));
    CHECK_INT(vs.di, STD cref(fake_lvalue(&S::di))(vsp));
    CHECK_TYPE(float, STD cref(fake_lvalue(&S::df))(vsp));
    CHECK_DOUBLE(vs.df, STD cref(fake_lvalue(&S::df))(vsp));
    CHECK_TYPE(double, STD cref(fake_lvalue(&S::cdd))(vsp));
    CHECK_DOUBLE(vs.cdd, STD cref(fake_lvalue(&S::cdd))(vsp));

    CHECK_TYPE(int, STD cref(fake_lvalue(&S::di))(cvsp));
    CHECK_INT(cvs.di, STD cref(fake_lvalue(&S::di))(cvsp));
    CHECK_TYPE(float, STD cref(fake_lvalue(&S::df))(cvsp));
    CHECK_DOUBLE(cvs.df, STD cref(fake_lvalue(&S::df))(cvsp));
    CHECK_TYPE(double, STD cref(fake_lvalue(&S::cdd))(cvsp));
    CHECK_DOUBLE(cvs.cdd, STD cref(fake_lvalue(&S::cdd))(cvsp));

    // MODIFY DATA MEMBER
    STD cref(fake_lvalue(&S::di))(s)++;
    CHECK_INT(s.di, 2);
    STD cref(fake_lvalue(&S::df))(vs) = 4.0;
    CHECK_DOUBLE(vs.df, 4.0);

    // reference_wrapper FOR FUNCTIONS
    // CALL 0-ARGUMENT FUNCTION RETURNING VOID
    CHECK_MSG("incorrect return type for member function returning void",
        typeid(STD cref(fake_lvalue(&returns_void))()) == typeid(void));
    // CALL 0-ARGUMENT FUNCTION
    CHECK_TYPE(char, STD cref(fake_lvalue(&f0))());
    CHECK_INT(0, STD cref(fake_lvalue(&f0))());

    // CALL 1-ARGUMENT FUNCTION
    CHECK_TYPE(int, STD cref(fake_lvalue(&f1))(one));
    CHECK_INT(1, STD cref(fake_lvalue(&f1))(one));

    // CALL 2-ARGUMENT FUNCTION
    CHECK_TYPE(double, STD cref(fake_lvalue(&f2))(one, one));
    CHECK_DOUBLE(2.0, STD cref(fake_lvalue(&f2))(one, one));

    // CALL 3-ARGUMENT FUNCTION
    CHECK_TYPE(char, STD cref(fake_lvalue(&f3))(one, one, one));
    CHECK_INT(3, STD cref(fake_lvalue(&f3))(one, one, one));

    // CALL 4-ARGUMENT FUNCTION
    CHECK_TYPE(int, STD cref(fake_lvalue(&f4))(one, one, one, one));
    CHECK_INT(4, STD cref(fake_lvalue(&f4))(one, one, one, one));

    // CALL 5-ARGUMENT FUNCTION
    CHECK_TYPE(double, STD cref(fake_lvalue(&f5))(one, one, one, one, one));
    CHECK_DOUBLE(5.0, STD cref(fake_lvalue(&f5))(one, one, one, one, one));

    // CALL 6-ARGUMENT FUNCTION
    CHECK_TYPE(char, STD cref(fake_lvalue(&f6))(one, one, one, one, one, one));
    CHECK_INT(6, STD cref(fake_lvalue(&f6))(one, one, one, one, one, one));

    // CALL 7-ARGUMENT FUNCTION
    CHECK_TYPE(int, STD cref(fake_lvalue(&f7))(one, one, one, one, one, one, one));
    CHECK_INT(7, STD cref(fake_lvalue(&f7))(one, one, one, one, one, one, one));

    // CALL 8-ARGUMENT FUNCTION
    CHECK_TYPE(double, STD cref(fake_lvalue(&f8))(one, one, one, one, one, one, one, one));
    CHECK_DOUBLE(8.0, STD cref(fake_lvalue(&f8))(one, one, one, one, one, one, one, one));

    // CALL 9-ARGUMENT FUNCTION
    CHECK_TYPE(char, STD cref(fake_lvalue(&f9))(one, one, one, one, one, one, one, one, one));
    CHECK_INT(9, STD cref(fake_lvalue(&f9))(one, one, one, one, one, one, one, one, one));

    // CALL 10-ARGUMENT FUNCTION
    CHECK_TYPE(int, STD cref(fake_lvalue(&f10))(one, one, one, one, one, one, one, one, one, one));
    CHECK_INT(10, STD cref(fake_lvalue(&f10))(one, one, one, one, one, one, one, one, one, one));

    // reference_wrapper FOR MEMBER FUNCTIONS
    // CALL 0-ARGUMENT MEMBER FUNCTION RETURNING VOID
    CHECK_MSG("incorrect return type for member function returning void",
        typeid(STD cref(fake_lvalue(&S::returns_void))(sp)) == typeid(void));
    // CALL 0-ARGUMENT MEMBER FUNCTIONS
    CHECK_TYPE(char, STD cref(fake_lvalue(&S::f0))(sp));
    CHECK_INT(0, STD cref(fake_lvalue(&S::f0))(sp));

    CHECK_TYPE(char, STD cref(fake_lvalue(&S::cf0))(csp));
    CHECK_INT(10, STD cref(fake_lvalue(&S::cf0))(sp));
    CHECK_INT(10, STD cref(fake_lvalue(&S::cf0))(csp));

    CHECK_TYPE(char, STD cref(fake_lvalue(&S::vf0))(vsp));
    CHECK_INT(20, STD cref(fake_lvalue(&S::vf0))(sp));
    CHECK_INT(20, STD cref(fake_lvalue(&S::vf0))(vsp));

    CHECK_TYPE(char, STD cref(fake_lvalue(&S::cvf0))(cvsp));
    CHECK_INT(30, STD cref(fake_lvalue(&S::cvf0))(sp));
    CHECK_INT(30, STD cref(fake_lvalue(&S::cvf0))(csp));
    CHECK_INT(30, STD cref(fake_lvalue(&S::cvf0))(vsp));
    CHECK_INT(30, STD cref(fake_lvalue(&S::cvf0))(cvsp));

    // CALL 1-ARGUMENT MEMBER FUNCTIONS
    CHECK_TYPE(int, STD cref(fake_lvalue(&S::f1))(sp, one));
    CHECK_INT(1, STD cref(fake_lvalue(&S::f1))(sp, one));

    CHECK_TYPE(int, STD cref(fake_lvalue(&S::cf1))(csp, one));
    CHECK_INT(11, STD cref(fake_lvalue(&S::cf1))(sp, one));
    CHECK_INT(11, STD cref(fake_lvalue(&S::cf1))(csp, one));

    CHECK_TYPE(int, STD cref(fake_lvalue(&S::vf1))(vsp, one));
    CHECK_INT(21, STD cref(fake_lvalue(&S::vf1))(sp, one));
    CHECK_INT(21, STD cref(fake_lvalue(&S::vf1))(vsp, one));

    CHECK_TYPE(int, STD cref(fake_lvalue(&S::cvf1))(cvsp, one));
    CHECK_INT(31, STD cref(fake_lvalue(&S::cvf1))(sp, one));
    CHECK_INT(31, STD cref(fake_lvalue(&S::cvf1))(csp, one));
    CHECK_INT(31, STD cref(fake_lvalue(&S::cvf1))(vsp, one));
    CHECK_INT(31, STD cref(fake_lvalue(&S::cvf1))(cvsp, one));

    // CALL 2-ARGUMENT MEMBER FUNCTIONS
    CHECK_TYPE(double, STD cref(fake_lvalue(&S::f2))(sp, one, one));
    CHECK_DOUBLE(2.0, STD cref(fake_lvalue(&S::f2))(sp, one, one));

    CHECK_TYPE(double, STD cref(fake_lvalue(&S::cf2))(csp, one, one));
    CHECK_DOUBLE(12.0, STD cref(fake_lvalue(&S::cf2))(sp, one, one));
    CHECK_DOUBLE(12.0, STD cref(fake_lvalue(&S::cf2))(csp, one, one));

    CHECK_TYPE(double, STD cref(fake_lvalue(&S::vf2))(vsp, one, one));
    CHECK_DOUBLE(22.0, STD cref(fake_lvalue(&S::vf2))(sp, one, one));
    CHECK_DOUBLE(22.0, STD cref(fake_lvalue(&S::vf2))(vsp, one, one));

    CHECK_TYPE(double, STD cref(fake_lvalue(&S::cvf2))(cvsp, one, one));
    CHECK_DOUBLE(32.0, STD cref(fake_lvalue(&S::cvf2))(sp, one, one));
    CHECK_DOUBLE(32.0, STD cref(fake_lvalue(&S::cvf2))(csp, one, one));
    CHECK_DOUBLE(32.0, STD cref(fake_lvalue(&S::cvf2))(vsp, one, one));
    CHECK_DOUBLE(32.0, STD cref(fake_lvalue(&S::cvf2))(cvsp, one, one));

    // CALL 3-ARGUMENT MEMBER FUNCTIONS
    CHECK_TYPE(char, STD cref(fake_lvalue(&S::f3))(sp, one, one, one));
    CHECK_INT(3, STD cref(fake_lvalue(&S::f3))(sp, one, one, one));

    CHECK_TYPE(char, STD cref(fake_lvalue(&S::cf3))(csp, one, one, one));
    CHECK_INT(13, STD cref(fake_lvalue(&S::cf3))(sp, one, one, one));
    CHECK_INT(13, STD cref(fake_lvalue(&S::cf3))(csp, one, one, one));

    CHECK_TYPE(char, STD cref(fake_lvalue(&S::vf3))(vsp, one, one, one));
    CHECK_INT(23, STD cref(fake_lvalue(&S::vf3))(sp, one, one, one));
    CHECK_INT(23, STD cref(fake_lvalue(&S::vf3))(vsp, one, one, one));

    CHECK_TYPE(char, STD cref(fake_lvalue(&S::cvf3))(cvsp, one, one, one));
    CHECK_INT(33, STD cref(fake_lvalue(&S::cvf3))(sp, one, one, one));
    CHECK_INT(33, STD cref(fake_lvalue(&S::cvf3))(csp, one, one, one));
    CHECK_INT(33, STD cref(fake_lvalue(&S::cvf3))(vsp, one, one, one));
    CHECK_INT(33, STD cref(fake_lvalue(&S::cvf3))(cvsp, one, one, one));

    // CALL 4-ARGUMENT MEMBER FUNCTIONS
    CHECK_TYPE(int, STD cref(fake_lvalue(&S::f4))(sp, one, one, one, one));
    CHECK_INT(4, STD cref(fake_lvalue(&S::f4))(sp, one, one, one, one));

    CHECK_TYPE(int, STD cref(fake_lvalue(&S::cf4))(csp, one, one, one, one));
    CHECK_INT(14, STD cref(fake_lvalue(&S::cf4))(sp, one, one, one, one));
    CHECK_INT(14, STD cref(fake_lvalue(&S::cf4))(csp, one, one, one, one));

    CHECK_TYPE(int, STD cref(fake_lvalue(&S::vf4))(vsp, one, one, one, one));
    CHECK_INT(24, STD cref(fake_lvalue(&S::vf4))(sp, one, one, one, one));
    CHECK_INT(24, STD cref(fake_lvalue(&S::vf4))(vsp, one, one, one, one));

    CHECK_TYPE(int, STD cref(fake_lvalue(&S::cvf4))(cvsp, one, one, one, one));
    CHECK_INT(34, STD cref(fake_lvalue(&S::cvf4))(sp, one, one, one, one));
    CHECK_INT(34, STD cref(fake_lvalue(&S::cvf4))(csp, one, one, one, one));
    CHECK_INT(34, STD cref(fake_lvalue(&S::cvf4))(vsp, one, one, one, one));
    CHECK_INT(34, STD cref(fake_lvalue(&S::cvf4))(cvsp, one, one, one, one));

    // CALL 5-ARGUMENT MEMBER FUNCTIONS
    CHECK_TYPE(double, STD cref(fake_lvalue(&S::f5))(sp, one, one, one, one, one));
    CHECK_DOUBLE(5.0, STD cref(fake_lvalue(&S::f5))(sp, one, one, one, one, one));

    CHECK_TYPE(double, STD cref(fake_lvalue(&S::cf5))(csp, one, one, one, one, one));
    CHECK_DOUBLE(15.0, STD cref(fake_lvalue(&S::cf5))(sp, one, one, one, one, one));
    CHECK_DOUBLE(15.0, STD cref(fake_lvalue(&S::cf5))(csp, one, one, one, one, one));

    CHECK_TYPE(double, STD cref(fake_lvalue(&S::vf5))(vsp, one, one, one, one, one));
    CHECK_DOUBLE(25.0, STD cref(fake_lvalue(&S::vf5))(sp, one, one, one, one, one));
    CHECK_DOUBLE(25.0, STD cref(fake_lvalue(&S::vf5))(vsp, one, one, one, one, one));

    CHECK_TYPE(double, STD cref(fake_lvalue(&S::cvf5))(cvsp, one, one, one, one, one));
    CHECK_DOUBLE(35.0, STD cref(fake_lvalue(&S::cvf5))(sp, one, one, one, one, one));
    CHECK_DOUBLE(35.0, STD cref(fake_lvalue(&S::cvf5))(csp, one, one, one, one, one));
    CHECK_DOUBLE(35.0, STD cref(fake_lvalue(&S::cvf5))(vsp, one, one, one, one, one));
    CHECK_DOUBLE(35.0, STD cref(fake_lvalue(&S::cvf5))(cvsp, one, one, one, one, one));

    // CALL 6-ARGUMENT MEMBER FUNCTIONS
    CHECK_TYPE(char, STD cref(fake_lvalue(&S::f6))(sp, one, one, one, one, one, one));
    CHECK_INT(6, STD cref(fake_lvalue(&S::f6))(sp, one, one, one, one, one, one));

    CHECK_TYPE(char, STD cref(fake_lvalue(&S::cf6))(csp, one, one, one, one, one, one));
    CHECK_INT(16, STD cref(fake_lvalue(&S::cf6))(sp, one, one, one, one, one, one));
    CHECK_INT(16, STD cref(fake_lvalue(&S::cf6))(csp, one, one, one, one, one, one));

    CHECK_TYPE(char, STD cref(fake_lvalue(&S::vf6))(vsp, one, one, one, one, one, one));
    CHECK_INT(26, STD cref(fake_lvalue(&S::vf6))(sp, one, one, one, one, one, one));
    CHECK_INT(26, STD cref(fake_lvalue(&S::vf6))(vsp, one, one, one, one, one, one));

    CHECK_TYPE(char, STD cref(fake_lvalue(&S::cvf6))(cvsp, one, one, one, one, one, one));
    CHECK_INT(36, STD cref(fake_lvalue(&S::cvf6))(sp, one, one, one, one, one, one));
    CHECK_INT(36, STD cref(fake_lvalue(&S::cvf6))(csp, one, one, one, one, one, one));
    CHECK_INT(36, STD cref(fake_lvalue(&S::cvf6))(vsp, one, one, one, one, one, one));
    CHECK_INT(36, STD cref(fake_lvalue(&S::cvf6))(cvsp, one, one, one, one, one, one));

    // CALL 7-ARGUMENT MEMBER FUNCTIONS
    CHECK_TYPE(int, STD cref(fake_lvalue(&S::f7))(sp, one, one, one, one, one, one, one));
    CHECK_INT(7, STD cref(fake_lvalue(&S::f7))(sp, one, one, one, one, one, one, one));

    CHECK_TYPE(int, STD cref(fake_lvalue(&S::cf7))(csp, one, one, one, one, one, one, one));
    CHECK_INT(17, STD cref(fake_lvalue(&S::cf7))(sp, one, one, one, one, one, one, one));
    CHECK_INT(17, STD cref(fake_lvalue(&S::cf7))(csp, one, one, one, one, one, one, one));

    CHECK_TYPE(int, STD cref(fake_lvalue(&S::vf7))(vsp, one, one, one, one, one, one, one));
    CHECK_INT(27, STD cref(fake_lvalue(&S::vf7))(sp, one, one, one, one, one, one, one));
    CHECK_INT(27, STD cref(fake_lvalue(&S::vf7))(vsp, one, one, one, one, one, one, one));

    CHECK_TYPE(int, STD cref(fake_lvalue(&S::cvf7))(cvsp, one, one, one, one, one, one, one));
    CHECK_INT(37, STD cref(fake_lvalue(&S::cvf7))(sp, one, one, one, one, one, one, one));
    CHECK_INT(37, STD cref(fake_lvalue(&S::cvf7))(csp, one, one, one, one, one, one, one));
    CHECK_INT(37, STD cref(fake_lvalue(&S::cvf7))(vsp, one, one, one, one, one, one, one));
    CHECK_INT(37, STD cref(fake_lvalue(&S::cvf7))(cvsp, one, one, one, one, one, one, one));

    // CALL 8-ARGUMENT MEMBER FUNCTIONS
    CHECK_TYPE(double, STD cref(fake_lvalue(&S::f8))(sp, one, one, one, one, one, one, one, one));
    CHECK_DOUBLE(8.0, STD cref(fake_lvalue(&S::f8))(sp, one, one, one, one, one, one, one, one));

    CHECK_TYPE(double, STD cref(fake_lvalue(&S::cf8))(csp, one, one, one, one, one, one, one, one));
    CHECK_DOUBLE(18.0, STD cref(fake_lvalue(&S::cf8))(sp, one, one, one, one, one, one, one, one));
    CHECK_DOUBLE(18.0, STD cref(fake_lvalue(&S::cf8))(csp, one, one, one, one, one, one, one, one));

    CHECK_TYPE(double, STD cref(fake_lvalue(&S::vf8))(vsp, one, one, one, one, one, one, one, one));
    CHECK_DOUBLE(28.0, STD cref(fake_lvalue(&S::vf8))(sp, one, one, one, one, one, one, one, one));
    CHECK_DOUBLE(28.0, STD cref(fake_lvalue(&S::vf8))(vsp, one, one, one, one, one, one, one, one));

    CHECK_TYPE(double, STD cref(fake_lvalue(&S::cvf8))(cvsp, one, one, one, one, one, one, one, one));
    CHECK_DOUBLE(38.0, STD cref(fake_lvalue(&S::cvf8))(sp, one, one, one, one, one, one, one, one));
    CHECK_DOUBLE(38.0, STD cref(fake_lvalue(&S::cvf8))(csp, one, one, one, one, one, one, one, one));
    CHECK_DOUBLE(38.0, STD cref(fake_lvalue(&S::cvf8))(vsp, one, one, one, one, one, one, one, one));
    CHECK_DOUBLE(38.0, STD cref(fake_lvalue(&S::cvf8))(cvsp, one, one, one, one, one, one, one, one));

    // CALL 9-ARGUMENT MEMBER FUNCTIONS
    CHECK_TYPE(char, STD cref(fake_lvalue(&S::f9))(sp, one, one, one, one, one, one, one, one, one));
    CHECK_INT(9, STD cref(fake_lvalue(&S::f9))(sp, one, one, one, one, one, one, one, one, one));

    CHECK_TYPE(char, STD cref(fake_lvalue(&S::cf9))(csp, one, one, one, one, one, one, one, one, one));
    CHECK_INT(19, STD cref(fake_lvalue(&S::cf9))(sp, one, one, one, one, one, one, one, one, one));
    CHECK_INT(19, STD cref(fake_lvalue(&S::cf9))(csp, one, one, one, one, one, one, one, one, one));

    CHECK_TYPE(char, STD cref(fake_lvalue(&S::vf9))(vsp, one, one, one, one, one, one, one, one, one));
    CHECK_INT(29, STD cref(fake_lvalue(&S::vf9))(sp, one, one, one, one, one, one, one, one, one));
    CHECK_INT(29, STD cref(fake_lvalue(&S::vf9))(vsp, one, one, one, one, one, one, one, one, one));

    CHECK_TYPE(char, STD cref(fake_lvalue(&S::cvf9))(cvsp, one, one, one, one, one, one, one, one, one));
    CHECK_INT(39, STD cref(fake_lvalue(&S::cvf9))(sp, one, one, one, one, one, one, one, one, one));
    CHECK_INT(39, STD cref(fake_lvalue(&S::cvf9))(csp, one, one, one, one, one, one, one, one, one));
    CHECK_INT(39, STD cref(fake_lvalue(&S::cvf9))(vsp, one, one, one, one, one, one, one, one, one));
    CHECK_INT(39, STD cref(fake_lvalue(&S::cvf9))(cvsp, one, one, one, one, one, one, one, one, one));
}

void test_main() { // test header <functional>
    typedef STD reference_wrapper<int> rwi;
    typedef STD reference_wrapper<const int> rwci;
    int i = 3, j = 4;

    rwi rw0 = rwi(i);
    CHECK_INT(rw0, 3);
    rwi rw1(j);
    CHECK_INT(rw1, 4);
    rw1 = rw0;
    CHECK_INT(rw0, 3);

    int& ir0 = rw1;
    CHECK_INT(ir0, i);
    ir0 = 5;
    CHECK_INT(ir0, 5);
    CHECK_INT(i, 5);
    CHECK_INT(rw1, 5);
    const int& cir0 = rwci(j);
    CHECK_INT(cir0, j);

    int& ir1 = rw1.get();
    CHECK_INT(ir1, i);
    const int& cir1 = rwci(j).get();
    CHECK_INT(cir1, j);

    tref0();

    rwi rw2 = STD ref(i);
    CHECK_INT(rw2, i);
    rwci rw3 = STD cref(i);
    CHECK_INT(rw3, i);
}
