// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// common header for functional?.cpp

#pragma warning(push)
#pragma warning(disable : 5215) // '%s' a function parameter with volatile qualified type is deprecated in C++20

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-volatile"
#endif // __clang__

struct funobj;
static int f0();
static int f1(const volatile funobj);
static int f2(const volatile funobj, int);
static int f3(const volatile funobj, int, int);
static int f4(const volatile funobj, int, int, int);
static int f5(const volatile funobj, int, int, int, int);
static int f6(const volatile funobj, int, int, int, int, int);
static int f7(const volatile funobj, int, int, int, int, int, int);
static int f8(const volatile funobj, int, int, int, int, int, int, int);
static int f9(const volatile funobj, int, int, int, int, int, int, int, int);
static int f10(const volatile funobj, int, int, int, int, int, int, int, int, int);
typedef int (*tf0)();
typedef int (*tf1)(const volatile funobj);
typedef int (*tf2)(const volatile funobj, int);
typedef int (*tf3)(const volatile funobj, int, int);
typedef int (*tf4)(const volatile funobj, int, int, int);
typedef int (*tf5)(const volatile funobj, int, int, int, int);
typedef int (*tf6)(const volatile funobj, int, int, int, int, int);
typedef int (*tf7)(const volatile funobj, int, int, int, int, int, int);
typedef int (*tf8)(const volatile funobj, int, int, int, int, int, int, int);
typedef int (*tf9)(const volatile funobj, int, int, int, int, int, int, int, int);
typedef int (*tf10)(const volatile funobj, int, int, int, int, int, int, int, int, int);

#pragma warning(push)
#pragma warning(disable : 4521) // multiple copy constructors specified
struct funobj { // general purpose function object
    typedef int result_type;

    funobj(int i = 0) : i0(i) {}

    funobj(const volatile funobj& f) : i0(f.i0) {}

    funobj(const funobj& f) : i0(f.i0) {}

    funobj(funobj&& f) : i0(f.i0) {}

    int get() const volatile { // return stored data value
        return i0;
    }
    int operator()() const { // function call operator
        return ::f0();
    }
    int operator()(funobj fn) const { // function call operator
        return ::f1(fn);
    }
    int operator()(funobj fn, int i1) const { // function call operator
        return ::f2(fn, i1);
    }
    int operator()(funobj fn, int i1, int i2) const { // function call operator
        return ::f3(fn, i1, i2);
    }
    int operator()(funobj fn, int i1, int i2, int i3) const { // function call operator
        return ::f4(fn, i1, i2, i3);
    }
    int operator()(funobj fn, int i1, int i2, int i3, int i4) const { // function call operator
        return ::f5(fn, i1, i2, i3, i4);
    }
    int operator()(funobj fn, int i1, int i2, int i3, int i4, int i5) const { // function call operator
        return ::f6(fn, i1, i2, i3, i4, i5);
    }
    int operator()(funobj fn, int i1, int i2, int i3, int i4, int i5,
        int i6) const { // function call operator
        return ::f7(fn, i1, i2, i3, i4, i5, i6);
    }
    int operator()(funobj fn, int i1, int i2, int i3, int i4, int i5, int i6, int i7) const { // function call operator
        return ::f8(fn, i1, i2, i3, i4, i5, i6, i7);
    }
    int operator()(
        funobj fn, int i1, int i2, int i3, int i4, int i5, int i6, int i7, int i8) const { // function call operator
        return ::f9(fn, i1, i2, i3, i4, i5, i6, i7, i8);
    }
    int operator()(funobj fn, int i1, int i2, int i3, int i4, int i5, int i6, int i7, int i8,
        int i9) const { // function call operator
        return ::f10(fn, i1, i2, i3, i4, i5, i6, i7, i8, i9);
    }

    int f1() { // member function with zero arguments
        return ::f1(*this);
    }
    int f2(int i1) { // member function with one argument
        return ::f2(*this, i1);
    }
    int f3(int i1, int i2) { // member function with two arguments
        return ::f3(*this, i1, i2);
    }
    int f4(int i1, int i2, int i3) { // member function with three arguments
        return ::f4(*this, i1, i2, i3);
    }
    int f5(int i1, int i2, int i3, int i4) { // member function with four arguments
        return ::f5(*this, i1, i2, i3, i4);
    }
    int f6(int i1, int i2, int i3, int i4, int i5) { // member function with five arguments
        return ::f6(*this, i1, i2, i3, i4, i5);
    }
    int f7(int i1, int i2, int i3, int i4, int i5,
        int i6) { // member function with six arguments
        return ::f7(*this, i1, i2, i3, i4, i5, i6);
    }
    int f8(int i1, int i2, int i3, int i4, int i5, int i6, int i7) { // member function with seven arguments
        return ::f8(*this, i1, i2, i3, i4, i5, i6, i7);
    }
    int f9(int i1, int i2, int i3, int i4, int i5, int i6, int i7, int i8) { // member function with eight arguments
        return ::f9(*this, i1, i2, i3, i4, i5, i6, i7, i8);
    }
    int f10(
        int i1, int i2, int i3, int i4, int i5, int i6, int i7, int i8, int i9) { // member function with nine arguments
        return ::f10(*this, i1, i2, i3, i4, i5, i6, i7, i8, i9);
    }

    int cf1() const { // const member function with no arguments
        return ::f1(*this);
    }
    int cf2(int i1) const { // const member function with one argument
        return ::f2(*this, i1);
    }
    int cf3(int i1, int i2) const { // const member function with two arguments
        return ::f3(*this, i1, i2);
    }
    int cf4(int i1, int i2, int i3) const { // const member function with three arguments
        return ::f4(*this, i1, i2, i3);
    }
    int cf5(int i1, int i2, int i3, int i4) const { // const member function with four arguments
        return ::f5(*this, i1, i2, i3, i4);
    }
    int cf6(int i1, int i2, int i3, int i4, int i5) const { // const member function with five arguments
        return ::f6(*this, i1, i2, i3, i4, i5);
    }
    int cf7(int i1, int i2, int i3, int i4, int i5, int i6) const { // const member function with six arguments
        return ::f7(*this, i1, i2, i3, i4, i5, i6);
    }
    int cf8(int i1, int i2, int i3, int i4, int i5, int i6, int i7) const {
        // const member function with seven arguments
        return ::f8(*this, i1, i2, i3, i4, i5, i6, i7);
    }
    int cf9(int i1, int i2, int i3, int i4, int i5, int i6, int i7, int i8) const {
        // const member function with eight arguments
        return ::f9(*this, i1, i2, i3, i4, i5, i6, i7, i8);
    }
    int cf10(int i1, int i2, int i3, int i4, int i5, int i6, int i7, int i8, int i9) const {
        // const member function with nine arguments
        return ::f10(*this, i1, i2, i3, i4, i5, i6, i7, i8, i9);
    }

    int vf1() volatile { // volatile member function with no arguments
        return ::f1(*this);
    }
    int vf2(int i1) volatile { // volatile member function with one argument
        return ::f2(*this, i1);
    }
    int vf3(int i1, int i2) volatile { // volatile member function with two arguments
        return ::f3(*this, i1, i2);
    }
    int vf4(int i1, int i2, int i3) volatile { // volatile member function with three arguments
        return ::f4(*this, i1, i2, i3);
    }
    int vf5(int i1, int i2, int i3, int i4) volatile { // volatile member function with four arguments
        return ::f5(*this, i1, i2, i3, i4);
    }
    int vf6(int i1, int i2, int i3, int i4, int i5) volatile { // volatile member function with five arguments
        return ::f6(*this, i1, i2, i3, i4, i5);
    }
    int vf7(int i1, int i2, int i3, int i4, int i5, int i6) volatile { // volatile member function with six arguments
        return ::f7(*this, i1, i2, i3, i4, i5, i6);
    }
    int vf8(int i1, int i2, int i3, int i4, int i5, int i6, int i7) volatile {
        // volatile member function with seven arguments
        return ::f8(*this, i1, i2, i3, i4, i5, i6, i7);
    }
    int vf9(int i1, int i2, int i3, int i4, int i5, int i6, int i7, int i8) volatile {
        // volatile member function with eight arguments
        return ::f9(*this, i1, i2, i3, i4, i5, i6, i7, i8);
    }
    int vf10(int i1, int i2, int i3, int i4, int i5, int i6, int i7, int i8, int i9) volatile {
        // volatile member function with nine arguments
        return ::f10(*this, i1, i2, i3, i4, i5, i6, i7, i8, i9);
    }

    int cvf1() const volatile { // const volatile member function with no arguments
        return ::f1(*this);
    }
    int cvf2(int i1) const volatile { // const volatile member function with one argument
        return ::f2(*this, i1);
    }
    int cvf3(int i1, int i2) const volatile { // const volatile member function with two arguments
        return ::f3(*this, i1, i2);
    }
    int cvf4(int i1, int i2, int i3) const volatile { // const volatile member function with three arguments
        return ::f4(*this, i1, i2, i3);
    }
    int cvf5(int i1, int i2, int i3, int i4) const volatile { // const volatile member function with four arguments
        return ::f5(*this, i1, i2, i3, i4);
    }
    int cvf6(int i1, int i2, int i3, int i4, int i5) const volatile {
        // const volatile member function with five arguments
        return ::f6(*this, i1, i2, i3, i4, i5);
    }
    int cvf7(int i1, int i2, int i3, int i4, int i5, int i6) const volatile {
        // const volatile member function with six arguments
        return ::f7(*this, i1, i2, i3, i4, i5, i6);
    }
    int cvf8(int i1, int i2, int i3, int i4, int i5, int i6, int i7) const volatile {
        // const volatile member function with seven arguments
        return ::f8(*this, i1, i2, i3, i4, i5, i6, i7);
    }
    int cvf9(int i1, int i2, int i3, int i4, int i5, int i6, int i7, int i8) const volatile {
        // const volatile member function with eight arguments
        return ::f9(*this, i1, i2, i3, i4, i5, i6, i7, i8);
    }
    int cvf10(int i1, int i2, int i3, int i4, int i5, int i6, int i7, int i8, int i9) const volatile {
        // const volatile member function with nine arguments
        return ::f10(*this, i1, i2, i3, i4, i5, i6, i7, i8, i9);
    }
    int i0;
};
#pragma warning(pop)

typedef int (funobj::*mf1)();
typedef int (funobj::*mf2)(int);
typedef int (funobj::*mf3)(int, int);
typedef int (funobj::*mf4)(int, int, int);
typedef int (funobj::*mf5)(int, int, int, int);
typedef int (funobj::*mf6)(int, int, int, int, int);
typedef int (funobj::*mf7)(int, int, int, int, int, int);
typedef int (funobj::*mf8)(int, int, int, int, int, int, int);
typedef int (funobj::*mf9)(int, int, int, int, int, int, int, int);
typedef int (funobj::*mf10)(int, int, int, int, int, int, int, int, int);

typedef int (funobj::*cmf1)() const;
typedef int (funobj::*cmf2)(int) const;
typedef int (funobj::*cmf3)(int, int) const;
typedef int (funobj::*cmf4)(int, int, int) const;
typedef int (funobj::*cmf5)(int, int, int, int) const;
typedef int (funobj::*cmf6)(int, int, int, int, int) const;
typedef int (funobj::*cmf7)(int, int, int, int, int, int) const;
typedef int (funobj::*cmf8)(int, int, int, int, int, int, int) const;
typedef int (funobj::*cmf9)(int, int, int, int, int, int, int, int) const;
typedef int (funobj::*cmf10)(int, int, int, int, int, int, int, int, int) const;

typedef int (funobj::*vmf1)() volatile;
typedef int (funobj::*vmf2)(int) volatile;
typedef int (funobj::*vmf3)(int, int) volatile;
typedef int (funobj::*vmf4)(int, int, int) volatile;
typedef int (funobj::*vmf5)(int, int, int, int) volatile;
typedef int (funobj::*vmf6)(int, int, int, int, int) volatile;
typedef int (funobj::*vmf7)(int, int, int, int, int, int) volatile;
typedef int (funobj::*vmf8)(int, int, int, int, int, int, int) volatile;
typedef int (funobj::*vmf9)(int, int, int, int, int, int, int, int) volatile;
typedef int (funobj::*vmf10)(int, int, int, int, int, int, int, int, int) volatile;

typedef int (funobj::*cvmf1)() const volatile;
typedef int (funobj::*cvmf2)(int) const volatile;
typedef int (funobj::*cvmf3)(int, int) const volatile;
typedef int (funobj::*cvmf4)(int, int, int) const volatile;
typedef int (funobj::*cvmf5)(int, int, int, int) const volatile;
typedef int (funobj::*cvmf6)(int, int, int, int, int) const volatile;
typedef int (funobj::*cvmf7)(int, int, int, int, int, int) const volatile;
typedef int (funobj::*cvmf8)(int, int, int, int, int, int, int) const volatile;
typedef int (funobj::*cvmf9)(int, int, int, int, int, int, int, int) const volatile;
typedef int (funobj::*cvmf10)(int, int, int, int, int, int, int, int, int) const volatile;

typedef int funobj::*md0;
typedef int* funobj::*md1;

struct sp { // simplistic smart pointer
    sp(funobj& f) : ptr(&f) {}
    funobj& operator*() { // return reference to funobj
        return *ptr;
    }

    const funobj& operator*() const { // return reference to const funobj
        return *ptr;
    }

private:
    funobj* ptr;
};

static int f0() { // static function with no arguments
    return 1;
}
static int f1(const volatile funobj fn) { // static function with one argument
    return fn.get();
}
static int f2(const volatile funobj fn, int i1) { // static function with two arguments
    return fn.get() + i1;
}
static int f3(const volatile funobj fn, int i1, int i2) { // static function with three arguments
    return fn.get() + i1 + i2;
}
static int f4(const volatile funobj fn, int i1, int i2, int i3) { // static function with four arguments
    return fn.get() + i1 + i2 + i3;
}
static int f5(const volatile funobj fn, int i1, int i2, int i3, int i4) { // static function with five arguments
    return fn.get() + i1 + i2 + i3 + i4;
}
static int f6(const volatile funobj fn, int i1, int i2, int i3, int i4,
    int i5) { // static function with six arguments
    return fn.get() + i1 + i2 + i3 + i4 + i5;
}
static int f7(
    const volatile funobj fn, int i1, int i2, int i3, int i4, int i5, int i6) { // static function with seven arguments
    return fn.get() + i1 + i2 + i3 + i4 + i5 + i6;
}
static int f8(const volatile funobj fn, int i1, int i2, int i3, int i4, int i5, int i6,
    int i7) { // static function with eight arguments
    return fn.get() + i1 + i2 + i3 + i4 + i5 + i6 + i7;
}
static int f9(const volatile funobj fn, int i1, int i2, int i3, int i4, int i5, int i6, int i7,
    int i8) { // static function with nine arguments
    return fn.get() + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8;
}
static int f10(const volatile funobj fn, int i1, int i2, int i3, int i4, int i5, int i6, int i7, int i8,
    int i9) { // static function with ten arguments
    return fn.get() + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9;
}

int arg[] = { // arguments
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
int res[] = { // results
    0, 1, 3, 6, 10, 15, 21, 28, 36, 45, 55, 66, 78, 91, 105, 120, 136, 153, 171, 190};

#define RESULT(n, o) (res[n + o] - res[o])

#define CALL0(fobj) \
    { CHECK_INT(fobj(), 1); }
#define CALL1x(fobj, funobj)                   \
    {                                          \
        for (int i = 0; i < 10; ++i) {         \
            funobj f0(arg[i]);                 \
            CHECK_INT(fobj(f0), RESULT(1, i)); \
        }                                      \
    }
#define CALL1(fobj)                          \
    {                                        \
        CALL1x(fobj, funobj);                \
        CALL1x(fobj, const funobj);          \
        CALL1x(fobj, volatile funobj);       \
        CALL1x(fobj, const volatile funobj); \
    }
#define CALL2x(fobj, funobj)                               \
    {                                                      \
        for (int i = 0; i < 10; ++i) {                     \
            funobj f0(arg[i]);                             \
            CHECK_INT(fobj(f0, arg[i + 1]), RESULT(2, i)); \
        }                                                  \
    }
#define CALL2(fobj)                          \
    {                                        \
        CALL2x(fobj, funobj);                \
        CALL2x(fobj, const funobj);          \
        CALL2x(fobj, volatile funobj);       \
        CALL2x(fobj, const volatile funobj); \
    }
#define CALL3x(fobj, funobj)                                           \
    {                                                                  \
        for (int i = 0; i < 10; ++i) {                                 \
            funobj f0(arg[i]);                                         \
            CHECK_INT(fobj(f0, arg[i + 1], arg[i + 2]), RESULT(3, i)); \
        }                                                              \
    }
#define CALL3(fobj)                          \
    {                                        \
        CALL3x(fobj, funobj);                \
        CALL3x(fobj, const funobj);          \
        CALL3x(fobj, volatile funobj);       \
        CALL3x(fobj, const volatile funobj); \
    }
#define CALL4x(fobj, funobj)                                                       \
    {                                                                              \
        for (int i = 0; i < 10; ++i) {                                             \
            funobj f0(arg[i]);                                                     \
            CHECK_INT(fobj(f0, arg[i + 1], arg[i + 2], arg[i + 3]), RESULT(4, i)); \
        }                                                                          \
    }
#define CALL4(fobj)                          \
    {                                        \
        CALL4x(fobj, funobj);                \
        CALL4x(fobj, const funobj);          \
        CALL4x(fobj, volatile funobj);       \
        CALL4x(fobj, const volatile funobj); \
    }
#define CALL5x(fobj, funobj)                                                                   \
    {                                                                                          \
        for (int i = 0; i < 10; ++i) {                                                         \
            funobj f0(arg[i]);                                                                 \
            CHECK_INT(fobj(f0, arg[i + 1], arg[i + 2], arg[i + 3], arg[i + 4]), RESULT(5, i)); \
        }                                                                                      \
    }
#define CALL5(fobj)                          \
    {                                        \
        CALL5x(fobj, funobj);                \
        CALL5x(fobj, const funobj);          \
        CALL5x(fobj, volatile funobj);       \
        CALL5x(fobj, const volatile funobj); \
    }
#define CALL6x(fobj, funobj)                                                                               \
    {                                                                                                      \
        for (int i = 0; i < 10; ++i) {                                                                     \
            funobj f0(arg[i]);                                                                             \
            CHECK_INT(fobj(f0, arg[i + 1], arg[i + 2], arg[i + 3], arg[i + 4], arg[i + 5]), RESULT(6, i)); \
        }                                                                                                  \
    }
#define CALL6(fobj)                          \
    {                                        \
        CALL6x(fobj, funobj);                \
        CALL6x(fobj, const funobj);          \
        CALL6x(fobj, volatile funobj);       \
        CALL6x(fobj, const volatile funobj); \
    }
#define CALL7x(fobj, funobj)                                                                                           \
    {                                                                                                                  \
        for (int i = 0; i < 10; ++i) {                                                                                 \
            funobj f0(arg[i]);                                                                                         \
            CHECK_INT(fobj(f0, arg[i + 1], arg[i + 2], arg[i + 3], arg[i + 4], arg[i + 5], arg[i + 6]), RESULT(7, i)); \
        }                                                                                                              \
    }
#define CALL7(fobj)                          \
    {                                        \
        CALL7x(fobj, funobj);                \
        CALL7x(fobj, const funobj);          \
        CALL7x(fobj, volatile funobj);       \
        CALL7x(fobj, const volatile funobj); \
    }
#define CALL8x(fobj, funobj)                                                                                        \
    {                                                                                                               \
        for (int i = 0; i < 10; ++i) {                                                                              \
            funobj f0(arg[i]);                                                                                      \
            CHECK_INT(fobj(f0, arg[i + 1], arg[i + 2], arg[i + 3], arg[i + 4], arg[i + 5], arg[i + 6], arg[i + 7]), \
                RESULT(8, i));                                                                                      \
        }                                                                                                           \
    }
#define CALL8(fobj)                          \
    {                                        \
        CALL8x(fobj, funobj);                \
        CALL8x(fobj, const funobj);          \
        CALL8x(fobj, volatile funobj);       \
        CALL8x(fobj, const volatile funobj); \
    }
#define CALL9x(fobj, funobj)                                                                                       \
    {                                                                                                              \
        for (int i = 0; i < 10; ++i) {                                                                             \
            funobj f0(arg[i]);                                                                                     \
            CHECK_INT(fobj(f0, arg[i + 1], arg[i + 2], arg[i + 3], arg[i + 4], arg[i + 5], arg[i + 6], arg[i + 7], \
                          arg[i + 8]),                                                                             \
                RESULT(9, i));                                                                                     \
        }                                                                                                          \
    }
#define CALL9(fobj)                          \
    {                                        \
        CALL9x(fobj, funobj);                \
        CALL9x(fobj, const funobj);          \
        CALL9x(fobj, volatile funobj);       \
        CALL9x(fobj, const volatile funobj); \
    }
#define CALL10x(fobj, funobj)                                                                                      \
    {                                                                                                              \
        for (int i = 0; i < 10; ++i) {                                                                             \
            funobj f0(arg[i]);                                                                                     \
            CHECK_INT(fobj(f0, arg[i + 1], arg[i + 2], arg[i + 3], arg[i + 4], arg[i + 5], arg[i + 6], arg[i + 7], \
                          arg[i + 8], arg[i + 9]),                                                                 \
                RESULT(10, i));                                                                                    \
        }                                                                                                          \
    }
#define CALL10(fobj)                          \
    {                                         \
        CALL10x(fobj, funobj);                \
        CALL10x(fobj, const funobj);          \
        CALL10x(fobj, volatile funobj);       \
        CALL10x(fobj, const volatile funobj); \
    }

#define CALL1m(fobj)                            \
    {                                           \
        for (int i = 0; i < 10; ++i) {          \
            funobj f0(arg[i]);                  \
            funobj* fp = &f0;                   \
            sp sp0(f0);                         \
            CHECK_INT(fobj(f0), RESULT(1, i));  \
            CHECK_INT(fobj(fp), RESULT(1, i));  \
            CHECK_INT(fobj(sp0), RESULT(1, i)); \
        }                                       \
    }
#define CALL2m(fobj)                                        \
    {                                                       \
        for (int i = 0; i < 10; ++i) {                      \
            funobj f0(arg[i]);                              \
            funobj* fp = &f0;                               \
            sp sp0(f0);                                     \
            CHECK_INT(fobj(f0, arg[i + 1]), RESULT(2, i));  \
            CHECK_INT(fobj(fp, arg[i + 1]), RESULT(2, i));  \
            CHECK_INT(fobj(sp0, arg[i + 1]), RESULT(2, i)); \
        }                                                   \
    }
#define CALL3m(fobj)                                                    \
    {                                                                   \
        for (int i = 0; i < 10; ++i) {                                  \
            funobj f0(arg[i]);                                          \
            funobj* fp = &f0;                                           \
            sp sp0(f0);                                                 \
            CHECK_INT(fobj(f0, arg[i + 1], arg[i + 2]), RESULT(3, i));  \
            CHECK_INT(fobj(fp, arg[i + 1], arg[i + 2]), RESULT(3, i));  \
            CHECK_INT(fobj(sp0, arg[i + 1], arg[i + 2]), RESULT(3, i)); \
        }                                                               \
    }
#define CALL4m(fobj)                                                                \
    {                                                                               \
        for (int i = 0; i < 10; ++i) {                                              \
            funobj f0(arg[i]);                                                      \
            funobj* fp = &f0;                                                       \
            sp sp0(f0);                                                             \
            CHECK_INT(fobj(f0, arg[i + 1], arg[i + 2], arg[i + 3]), RESULT(4, i));  \
            CHECK_INT(fobj(fp, arg[i + 1], arg[i + 2], arg[i + 3]), RESULT(4, i));  \
            CHECK_INT(fobj(sp0, arg[i + 1], arg[i + 2], arg[i + 3]), RESULT(4, i)); \
        }                                                                           \
    }
#define CALL5m(fobj)                                                                            \
    {                                                                                           \
        for (int i = 0; i < 10; ++i) {                                                          \
            funobj f0(arg[i]);                                                                  \
            funobj* fp = &f0;                                                                   \
            sp sp0(f0);                                                                         \
            CHECK_INT(fobj(f0, arg[i + 1], arg[i + 2], arg[i + 3], arg[i + 4]), RESULT(5, i));  \
            CHECK_INT(fobj(fp, arg[i + 1], arg[i + 2], arg[i + 3], arg[i + 4]), RESULT(5, i));  \
            CHECK_INT(fobj(sp0, arg[i + 1], arg[i + 2], arg[i + 3], arg[i + 4]), RESULT(5, i)); \
        }                                                                                       \
    }
#define CALL6m(fobj)                                                                                        \
    {                                                                                                       \
        for (int i = 0; i < 10; ++i) {                                                                      \
            funobj f0(arg[i]);                                                                              \
            funobj* fp = &f0;                                                                               \
            sp sp0(f0);                                                                                     \
            CHECK_INT(fobj(f0, arg[i + 1], arg[i + 2], arg[i + 3], arg[i + 4], arg[i + 5]), RESULT(6, i));  \
            CHECK_INT(fobj(fp, arg[i + 1], arg[i + 2], arg[i + 3], arg[i + 4], arg[i + 5]), RESULT(6, i));  \
            CHECK_INT(fobj(sp0, arg[i + 1], arg[i + 2], arg[i + 3], arg[i + 4], arg[i + 5]), RESULT(6, i)); \
        }                                                                                                   \
    }
#define CALL7m(fobj)                                                                                                   \
    {                                                                                                                  \
        for (int i = 0; i < 10; ++i) {                                                                                 \
            funobj f0(arg[i]);                                                                                         \
            funobj* fp = &f0;                                                                                          \
            sp sp0(f0);                                                                                                \
            CHECK_INT(fobj(f0, arg[i + 1], arg[i + 2], arg[i + 3], arg[i + 4], arg[i + 5], arg[i + 6]), RESULT(7, i)); \
            CHECK_INT(fobj(fp, arg[i + 1], arg[i + 2], arg[i + 3], arg[i + 4], arg[i + 5], arg[i + 6]), RESULT(7, i)); \
            CHECK_INT(                                                                                                 \
                fobj(sp0, arg[i + 1], arg[i + 2], arg[i + 3], arg[i + 4], arg[i + 5], arg[i + 6]), RESULT(7, i));      \
        }                                                                                                              \
    }
#define CALL8m(fobj)                                                                                                 \
    {                                                                                                                \
        for (int i = 0; i < 10; ++i) {                                                                               \
            funobj f0(arg[i]);                                                                                       \
            funobj* fp = &f0;                                                                                        \
            sp sp0(f0);                                                                                              \
            CHECK_INT(fobj(f0, arg[i + 1], arg[i + 2], arg[i + 3], arg[i + 4], arg[i + 5], arg[i + 6], arg[i + 7]),  \
                RESULT(8, i));                                                                                       \
            CHECK_INT(fobj(fp, arg[i + 1], arg[i + 2], arg[i + 3], arg[i + 4], arg[i + 5], arg[i + 6], arg[i + 7]),  \
                RESULT(8, i));                                                                                       \
            CHECK_INT(fobj(sp0, arg[i + 1], arg[i + 2], arg[i + 3], arg[i + 4], arg[i + 5], arg[i + 6], arg[i + 7]), \
                RESULT(8, i));                                                                                       \
        }                                                                                                            \
    }
#define CALL9m(fobj)                                                                                                \
    {                                                                                                               \
        for (int i = 0; i < 10; ++i) {                                                                              \
            funobj f0(arg[i]);                                                                                      \
            funobj* fp = &f0;                                                                                       \
            sp sp0(f0);                                                                                             \
            CHECK_INT(fobj(f0, arg[i + 1], arg[i + 2], arg[i + 3], arg[i + 4], arg[i + 5], arg[i + 6], arg[i + 7],  \
                          arg[i + 8]),                                                                              \
                RESULT(9, i));                                                                                      \
            CHECK_INT(fobj(fp, arg[i + 1], arg[i + 2], arg[i + 3], arg[i + 4], arg[i + 5], arg[i + 6], arg[i + 7],  \
                          arg[i + 8]),                                                                              \
                RESULT(9, i));                                                                                      \
            CHECK_INT(fobj(sp0, arg[i + 1], arg[i + 2], arg[i + 3], arg[i + 4], arg[i + 5], arg[i + 6], arg[i + 7], \
                          arg[i + 8]),                                                                              \
                RESULT(9, i));                                                                                      \
        }                                                                                                           \
    }
#define CALL10m(fobj)                                                                                               \
    {                                                                                                               \
        for (int i = 0; i < 10; ++i) {                                                                              \
            funobj f0(arg[i]);                                                                                      \
            funobj* fp = &f0;                                                                                       \
            sp sp0(f0);                                                                                             \
            CHECK_INT(fobj(f0, arg[i + 1], arg[i + 2], arg[i + 3], arg[i + 4], arg[i + 5], arg[i + 6], arg[i + 7],  \
                          arg[i + 8], arg[i + 9]),                                                                  \
                RESULT(10, i));                                                                                     \
            CHECK_INT(fobj(fp, arg[i + 1], arg[i + 2], arg[i + 3], arg[i + 4], arg[i + 5], arg[i + 6], arg[i + 7],  \
                          arg[i + 8], arg[i + 9]),                                                                  \
                RESULT(10, i));                                                                                     \
            CHECK_INT(fobj(sp0, arg[i + 1], arg[i + 2], arg[i + 3], arg[i + 4], arg[i + 5], arg[i + 6], arg[i + 7], \
                          arg[i + 8], arg[i + 9]),                                                                  \
                RESULT(10, i));                                                                                     \
        }                                                                                                           \
    }

template <class Res, class Fty>
void match_result_type(Fty) {
    CHECK_TYPE(typename Fty::result_type, Res);
}

template <class Ty, class = void>
struct has_result_type_helper : STD false_type {};

template <class Ty>
struct has_result_type_helper<Ty, STD void_t<typename Ty::result_type>> : STD true_type {};

template <class Fty>
bool has_result_type(Fty) {
    return has_result_type_helper<Fty>::value;
}

template <class T>
const T& fake_lvalue(const T&& t) { // C++11 12.2 [class.temporary]/5: "A temporary bound to a reference
                                    // parameter in a function call (5.2.2) persists until the completion
                                    // of the full-expression containing the call."
    return t;
}

#ifdef __clang__
#pragma clang diagnostic pop
#endif // __clang__

#pragma warning(pop)
