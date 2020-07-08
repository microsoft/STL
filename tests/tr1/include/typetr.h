// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// common header for type_traits?.cpp

// check that X is one of the boolean wrapper types
template <class X>
struct is_bool_wrapper { // most types are not boolean wrappers
    static const bool value = false;
};

template <>
struct is_bool_wrapper<STD false_type> { // false_type is a boolean wrapper
    static const bool value = true;
};

template <>
struct is_bool_wrapper<STD true_type> { // true_type is a boolean wrapper
    static const bool value = true;
};

#define T_INTEGRAL_CONSTANT(Ty, NTy, val) \
    CHECK_TYPE(Ty::type, Ty);             \
    CHECK_TYPE(Ty::value_type, NTy);      \
    CHECK_INT(Ty::value, val);            \
    CHECK_INT((NTy) Ty(), val)

#define CHECK_BASE(X)                                                                                       \
    CHECK_INT((STD is_base_of<STD true_type, X>::value || STD is_base_of<STD false_type, X>::value), true); \
    CHECK_INT(is_bool_wrapper<X::type>::value, true);

#define T_BASIC_TRAITS(X, b) \
    CHECK_BASE(X);           \
    CHECK_INT(X::value, b)

// MACROS FOR TESTING TYPE TRAITS
#define T_TEST(trait, X, b) T_BASIC_TRAITS(STD trait<X>, b)
#define T_TEST_CV(trait, X, b)               \
    do {                                     \
        typedef X XX;                        \
        T_TEST(trait, XX, b);                \
        T_TEST(trait, const XX, b);          \
        T_TEST(trait, volatile XX, b);       \
        T_TEST(trait, const volatile XX, b); \
    } while (0)
#define T_TEST_CONST(trait, X)                  \
    do {                                        \
        typedef X XX;                           \
        T_TEST(trait, XX, false);               \
        T_TEST(trait, const XX, true);          \
        T_TEST(trait, volatile XX, false);      \
        T_TEST(trait, const volatile XX, true); \
    } while (0)
#define T_TEST_VOLATILE(trait, X)               \
    do {                                        \
        typedef X XX;                           \
        T_TEST(trait, XX, false);               \
        T_TEST(trait, const XX, false);         \
        T_TEST(trait, volatile XX, true);       \
        T_TEST(trait, const volatile XX, true); \
    } while (0)

// MACROS FOR TESTING is_pod W/O COMPILER SUPPORT
#define T_POD(X, res) CHECK_INT(STD is_pod<X>::value, res)

#define T_POD_CV(X, res)               \
    do {                               \
        typedef X XX;                  \
        T_POD(XX, res);                \
        T_POD(const XX, res);          \
        T_POD(volatile XX, res);       \
        T_POD(const volatile XX, res); \
    } while (0)

// MACROS FOR TESTING has_trivial_* TRAITS W/O COMPILER SUPPORT
#define T_TRIVIAL(trait, X, res) CHECK_INT(STD trait<X>::value, res)

#define T_TRIVIAL_C(trait, X, b)       \
    do {                               \
        typedef X XX;                  \
        T_TRIVIAL(trait, XX, b);       \
        T_TRIVIAL(trait, const XX, b); \
    } while (0)

#define T_TRIVIAL_V(trait, X, b)          \
    do {                                  \
        typedef X XX;                     \
        T_TRIVIAL(trait, XX, b);          \
        T_TRIVIAL(trait, volatile XX, b); \
    } while (0)

#define T_TRIVIAL_CV(trait, X, b)               \
    do {                                        \
        typedef X XX;                           \
        T_TRIVIAL(trait, XX, b);                \
        T_TRIVIAL(trait, const XX, b);          \
        T_TRIVIAL(trait, volatile XX, b);       \
        T_TRIVIAL(trait, const volatile XX, b); \
    } while (0)

// MACROS FOR TESTING TRAITS WITH UNSPECIFIED RESULT W/O COMPILER SUPPORT
#define T_UNSPEC(trait, X, res) CHECK_INT(STD trait<X>::value, res)

#define T_UNSPEC_C(trait, X, b)       \
    do {                              \
        typedef X XX;                 \
        T_UNSPEC(trait, XX, b);       \
        T_UNSPEC(trait, const XX, b); \
    } while (0)

#define T_UNSPEC_V(trait, X, b)          \
    do {                                 \
        typedef X XX;                    \
        T_UNSPEC(trait, XX, b);          \
        T_UNSPEC(trait, volatile XX, b); \
    } while (0)

#define T_UNSPEC_CV(trait, X, b)               \
    do {                                       \
        typedef X XX;                          \
        T_UNSPEC(trait, XX, b);                \
        T_UNSPEC(trait, const XX, b);          \
        T_UNSPEC(trait, volatile XX, b);       \
        T_UNSPEC(trait, const volatile XX, b); \
    } while (0)

// MACROS FOR TESTING TRAITS WITH NO BASE W/O COMPILER SUPPORT
#define T_NOBASE(trait, X, res) CHECK_INT(STD trait<X>::value, res)

#define T_NOBASE_CV(trait, X, b)               \
    do {                                       \
        typedef X XX;                          \
        T_NOBASE(trait, XX, b);                \
        T_NOBASE(trait, const XX, b);          \
        T_NOBASE(trait, volatile XX, b);       \
        T_NOBASE(trait, const volatile XX, b); \
    } while (0)

// TYPES
struct A { // abstract class
    virtual void f() = 0;
};

struct B { // dummy class for member pointers
    virtual ~B() noexcept {}
};

struct C : B { // derived dummy class
};

struct D { // empty class
};

struct M { // movable class
    M();
    M(M&&);
    M& operator=(M&&);
};

struct T { // throwing class
    T() noexcept(false);
    T(const T&) noexcept(false);
    ~T() noexcept(false);
    T& operator=(const T&) noexcept(false);
};

union U { // dummy union
    int i;
    double d;
};

typedef int B::*pmo;
typedef const int B::*pmoc;
typedef volatile int B::*pmov;
typedef const volatile int B::*pmocv;
typedef void (B::*pmf)();
typedef void (B::*pmfc)() const;
typedef void (B::*pmfv)() volatile;
typedef void (B::*pmfcv)() const volatile;

typedef int arr_t[20];
enum enum_t { one, two, three };
typedef void (*pf)();
typedef void(func)();

typedef int ARR0;
typedef ARR0 ARR1[2];
typedef ARR1 ARR2[3];
typedef ARR2 ARR3[4];
typedef ARR3 ARR4[5];
typedef ARR4 ARR5[6];
typedef ARR5 ARRN[];

typedef int I;
typedef const int CI;
typedef volatile int VI;
typedef const volatile int CVI;

typedef I* IP;
typedef I* const ICP;
typedef I* volatile IVP;
typedef I* const volatile ICVP;

typedef CI* CIP;
typedef CI* const CICP;
typedef CI* volatile CIVP;
typedef CI* const volatile CICVP;

typedef VI* VIP;
typedef VI* const VICP;
typedef VI* volatile VIVP;
typedef VI* const volatile VICVP;

typedef CVI* CVIP;
typedef CVI* const CVICP;
typedef CVI* volatile CVIVP;
typedef CVI* const volatile CVICVP;
