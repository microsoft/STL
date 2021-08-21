// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef TDEFS_H
#define TDEFS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
#include <cstdio> // pick up C++ library defines
#include <exception>
#endif // __cplusplus

// When compiling /clr:pure we need to include <new> to ensure that we get the
// __clrcall declarations of operator new and delete.
#if defined(__cplusplus) && defined(_M_CEE_PURE)
#include <new>
#endif // defined(__cplusplus) && defined(_M_CEE_PURE)

// Define the following macros on the command line:

// AFMT -- defined to print floating-point values in %a instead of %f format
// TERSE -- defined to quiet informative messages (default is OFF)
// VERBOSE -- defined to turn on additional messages (default is OFF)

// For the following macros, this header will auto adapt if no definition
// occurs on the compile command line:

// NO_EXCEPTIONS -- defined to disable all uses of exceptions

// EPS_ULPD -- permissible ULP error for double (default is 4)
// EPS_ULPF -- permissible ULP error for float (default is 4)
// EPS_ULPL -- permissible ULP error for long double (default is 4)

// MACROS
#if !defined(__cplusplus)
#define CSTD
#define STD
#else // C headers in global, C++ headers in std
#define CSTD ::
#define STD  std::
#endif // namespaces

#if !_HAS_EXCEPTIONS
#define NO_EXCEPTIONS 1
#else // !_HAS_EXCEPTIONS
#define NO_EXCEPTIONS 0
#endif // !_HAS_EXCEPTIONS

#define CHECK0(ok, ch, test) check_one(ok, ch, #test, __FILE__, __LINE__, test)
#define CHECK(test)          CHK_RESULT(#test, test)

#define CHECK_DOUBLE(left, right) \
    check_double(#left " == " #right, __FILE__, __LINE__, (double) (left), (double) (right))
#define CHECK_INT(left, right)    check_int(#left " == " #right, __FILE__, __LINE__, (int) (left), (int) (right))
#define CHECK_SIZE_T(left, right) check_size_t(#left " == " #right, __FILE__, __LINE__, (left), (right))
#define CHECK_PTR(left, right)    check_ptr(#left " == " #right, __FILE__, __LINE__, left, right)

#define CHECK_MEM(left, right, count)  check_mem(#left " == " #right, __FILE__, __LINE__, left, right, (int) (count))
#define CHECK_STR(left, right)         check_str(#left " == " #right, __FILE__, __LINE__, left, right)
#define CHECK_WMEM(left, right, count) check_wmem(#left " == " #right, __FILE__, __LINE__, left, right, (int) (count))
#define CHECK_WSTR(left, right)        check_wstr(#left " == " #right, __FILE__, __LINE__, left, right)
#define CHECK_STRING(left, right)      check_str(#left " == " #right, __FILE__, __LINE__, left.c_str(), right.c_str())
#define CHECK_WSTRING(left, right)     check_wstr(#left " == " #right, __FILE__, __LINE__, left.c_str(), right.c_str())

#define CHECK_TYPE_LONG(left, right, file_name, line_number) \
    check_type(#left " == " #right, file_name, line_number, typeid(left), typeid(right))
#define CHECK_TYPE(left, right) CHECK_TYPE_LONG(left, right, __FILE__, __LINE__)

#define CHECK_MSG(label, test)  CHK_RESULT(label, test)
#define CHK_RESULT(label, test) results(label, __FILE__, __LINE__, test)
#define SUMMARIZE               leave_chk(__FILE__)

#define EXIT_STATUS (0 < total_fail ? 1 : 0)

// OBJECTS
int total_pass = 0;
int total_fail = 0;

#ifdef AFMT
int afmt = 1;
#else // AFMT
int afmt    = 0;
#endif // AFMT

#ifdef TERSE
int terse = 1;
#else // TERSE
int terse   = 0;
#endif // TERSE

#ifdef VERBOSE
int verbose = 1;
#else // VERBOSE
int verbose = 0;
#endif // VERBOSE

// FUNCTIONS
int check_one(int ok, unsigned int ch, const char* label, const char* file_name, int line_number,
    int test) { // accumulate and maybe display failures
    if (test != 0) { // succeeded, display if verbose
        if (verbose)
            CSTD printf(" PASS test %.3d at line %.3d in %s for %#.2x: %s\n", total_pass + total_fail + 1, line_number,
                file_name, ch, label);
    } else { // failed, accumulate and maybe display
        ++total_fail;
        ok = 0;
        if (!terse)
            CSTD printf(" FAIL test %.3d at line %.3d in %s for %#.2x: %s\n", total_pass + total_fail + 1, line_number,
                file_name, ch, label);
    }
    return ok;
}

void results(const char* label, const char* file_name, int line_number, int test) { // display results
    if (test != 0) { // pass, count and maybe display
        ++total_pass;
        if (verbose)
            CSTD printf(
                " PASS test %.3d at line %.3d in %s: %s\n", total_pass + total_fail, line_number, file_name, label);
    } else { // fail, count and display
        ++total_fail;
        CSTD printf(" FAIL test %.3d at line %.3d in %s: %s\n", total_pass + total_fail, line_number, file_name, label);
    }
}

void check_int(const char* label, const char* file_name, int line_number, int left, int right) {
    int ans = left == right;

    if (!terse && !ans)
        CSTD printf(" GOT %d != %d\n", left, right);
    results(label, file_name, line_number, ans);
}

void check_size_t(const char* label, const char* file_name, int line_number, size_t left, size_t right) {
    int ans = left == right;

    if (!terse && !ans)
        CSTD printf(" GOT %zu != %zu\n", left, right);
    results(label, file_name, line_number, ans);
}

void check_mem(const char* label, const char* file_name, int line_number, const void* left, const void* right,
    int length) { // check for buffer equality
    int ans        = 0;
    int n          = length;
    const char* s1 = (const char*) left;
    const char* s2 = (const char*) right;

    for (; 0 <= --n && *s1 == *s2; ++s1, ++s2)
        ;
    if (n <= 0)
        ans = 1;
    if (!terse && !ans) { // put differing strings
        CSTD printf(" GOT \"");
        for (n = length, s1 = (const char*) left; 0 <= --n; ++s1)
            CSTD printf("%c", *s1);
        CSTD printf("\" != \"");
        for (n = length, s2 = (const char*) right; 0 <= --n; ++s2)
            CSTD printf("%c", *s2);
        CSTD printf("\"\n");
    }
    results(label, file_name, line_number, ans);
}

void check_ptr(const char* label, const char* file_name, int line_number, const void* left,
    const void* right) { // check for pointer equality
    int ans = left == right;

    if (!terse && !ans)
        CSTD printf(" GOT %p != %p\n", left, right);
    results(label, file_name, line_number, ans);
}

void check_str(const char* label, const char* file_name, int line_number, const char* left,
    const char* right) { // check for NTBS equality
    int ans        = 0;
    const char* s1 = left;
    const char* s2 = right;

    for (; *s1 == *s2; ++s1, ++s2)
        if (*s1 == '\0') { // equal through NUL terminator
            ans = 1;
            break;
        }
    if (!terse && !ans)
        CSTD printf(" GOT \"%s\" != \"%s\"\n", left, right);
    results(label, file_name, line_number, ans);
}

void check_wmem(const char* label, const char* file_name, int line_number, const wchar_t* left, const wchar_t* right,
    int length) { // check for wide buffer equality
    int ans           = 0;
    int n             = length;
    const wchar_t* s1 = left;
    const wchar_t* s2 = right;

    for (; 0 <= --n && *s1 == *s2; ++s1, ++s2)
        ;
    if (n <= 0)
        ans = 1;
    if (!terse && !ans) { // put differing strings
        CSTD printf(" GOT L\"");
        for (n = length; 0 <= --n; ++left)
            CSTD printf("%c", (char) *left);
        CSTD printf("\" != \"");
        for (n = length; 0 <= --n; ++right)
            CSTD printf("%c", (char) *right);
        CSTD printf("\"\n");
    }
    results(label, file_name, line_number, ans);
}

void check_wstr(const char* label, const char* file_name, int line_number, const wchar_t* left,
    const wchar_t* right) { // check for NTBS equality
    int ans           = 0;
    const wchar_t* s1 = left;
    const wchar_t* s2 = right;

    for (; *s1 == *s2; ++s1, ++s2)
        if (*s1 == L'\0') { // equal through NUL terminator
            ans = 1;
            break;
        }
    if (!terse && !ans) { // failure, print wide strings
        CSTD printf(" GOT L\"");
        for (; *left != L'\0'; ++left)
            CSTD printf("%c", (char) *left);
        CSTD printf("\" != L\"");
        for (; *right != L'\0'; ++right)
            CSTD printf("%c", (char) *right);
        CSTD printf("\"\n");
    }
    results(label, file_name, line_number, ans);
}

#ifdef __cplusplus
#include <typeinfo>

void check_type(const char* label, const char* file_name, int line_number, const STD type_info& left,
    const STD type_info& right) { // check for int equality
    int ans = left == right;

    if (!terse && !ans)
        CSTD printf(" GOT %s != %s\n", left.name(), right.name());
    results(label, file_name, line_number, ans);
}
#endif // __cplusplus

void check_double(const char* label, const char* file_name, int line_number, double left, double right) {
    int ans = left == right;

    if (!terse && !ans) { // print hex or decimal floating-point
        if (afmt)
            CSTD printf(" GOT %a != %a\n", left, right);
        else
            CSTD printf(" GOT %f != %f\n", left, right);
    }
    results(label, file_name, line_number, ans);
}

int leave_chk(const char* file_name) { // print summary on exit
    if (!terse || 0 < total_fail)
        CSTD printf("***** %d erroneous test cases in %s *****\n", total_fail, file_name);
    if (!terse)
        CSTD printf("***** %d successful test cases in %s *****\n", total_pass, file_name);

    if (0 < total_fail)
        CSTD printf("#FAILED: %s\n", file_name);
    else
        CSTD printf("#PASSED: %s\n", file_name);

    return EXIT_STATUS;
}

#ifdef COMPLEX_TYPE
#if CPP_COMPLEX
#include <complex>

typedef STD complex<float> Flt_complex;
typedef STD complex<double> Dbl_complex;
typedef STD complex<long double> Ldbl_complex;

#elif defined(_Fcomplex)
typedef _CSTD _Fcomplex Flt_complex;
typedef _CSTD _Dcomplex Dbl_complex;
typedef _CSTD _Lcomplex Ldbl_complex;

#else // language version
typedef float complex Flt_complex;
typedef double _Complex Dbl_complex;
typedef long double _Complex Ldbl_complex;
#endif // language version
#endif // COMPLEX_TYPE

#ifdef FLOAT_TYPE
#include <float.h>

#ifdef EPS
#define EPS_ULPD EPS // define all ulp from command line
#define EPS_ULPF EPS
#define EPS_ULPL EPS
#endif // EPS

#ifndef EPS_ULPD
#define EPS_ULPD 4 // default tolerance is 4 ulp
#endif // EPS_ULPD

#ifndef EPS_ULPF
#define EPS_ULPF 4
#endif // EPS_ULPF

#ifndef EPS_ULPL
#define EPS_ULPL 4
#endif // EPS_ULPL

#define IS_DOUBLE  1 // FLOAT_TYPE selects type (if defined)
#define IS_FLOAT   2
#define IS_LDOUBLE 3

#define epsd ((double) (EPS_ULPD * DBL_EPSILON))
#define epsf ((float) (EPS_ULPF * FLT_EPSILON))
#define epsl ((long double) (EPS_ULPL * LDBL_EPSILON))

#if FLOAT_TYPE == IS_DOUBLE
#define eps0 ((double) DBL_EPSILON)
typedef double Float_type;
int ulp = EPS_ULPD;

#elif FLOAT_TYPE == IS_FLOAT
#define eps0 ((float) FLT_EPSILON)
typedef float Float_type;
int ulp = EPS_ULPF;

#elif FLOAT_TYPE == IS_LDOUBLE
#define eps0 ((long double) LDBL_EPSILON)
typedef long double Float_type;
int ulp = EPS_ULPL;
#endif // value of FLOAT_TYPE

int approx2(Float_type d1, Float_type d2, Float_type sensitivity) { // test for approximate equality
#if 199901L <= __STDC_VERSION__
    if (isunordered(d1, d2)) { // at least one NaN
        if (!terse) { // report NaNs
            if (isnan(d1))
                CSTD printf("approx(x, y): x is a NaN\n");
            if (isnan(d2))
                CSTD printf("approx(x, y): y is a NaN\n");
        }
        return 0;
    } else
#endif // 199901L <= __STDC_VERSION__
    { // compare finite values
        Float_type err;

        if (d2 != (Float_type) 0)
            err = (d2 - d1) / d2;
        else
            err = d1;
        if (err < (Float_type) 0)
            err = -err;

        sensitivity += static_cast<Float_type>(ulp);
        if (err <= sensitivity * eps0) { // close enough, maybe display then succeed
            if (verbose)
                CSTD printf("difference is %.2g ulp (<= %.2g ulp)"
                            " for %.5Lg vs. %.5Lg\n",
                    (double) (err / eps0), (double) sensitivity, (long double) d1, (long double) d2);
            return 1;
        } else { // too different, maybe display then fail
            if (!terse)
                CSTD printf("difference is %.2g ulp (> %.2g ulp)"
                            " for %.5Lg vs. %.5Lg\n",
                    (double) (err / eps0), (double) sensitivity, (long double) d1, (long double) d2);
            return 0;
        }
    }
}

int approx(Float_type d1, Float_type d2) { // test for approximate equality
    return approx2(d1, d2, 0);
}

#ifdef COMPLEX_TYPE
#if COMPLEX_TYPE == IS_DOUBLE
#if CPP_COMPLEX
#define cmplx(re, im) Dbl_complex(re, im)

#else // __cplusplus
Dbl_complex cmplx(double re, double im) { // construct a double complex value from parts
    Dbl_complex ans;

    ((double*) &ans)[0] = re;
    ((double*) &ans)[1] = im;
    return ans;
}
#endif // __cplusplus

#elif COMPLEX_TYPE == IS_FLOAT
#if CPP_COMPLEX
#define cmplx(re, im) Flt_complex(re, im)

#else // __cplusplus
Flt_complex cmplx(float re, float im) { // construct a float complex value from parts
    Flt_complex ans;

    ((float*) &ans)[0] = re;
    ((float*) &ans)[1] = im;
    return ans;
}
#endif // __cplusplus

#elif COMPLEX_TYPE == IS_LDOUBLE
#if CPP_COMPLEX
#define cmplx(re, im) Ldbl_complex(re, im)

#else // __cplusplus
Ldbl_complex cmplx(long double re, long double im) { // construct a long double complex value from parts
    Ldbl_complex ans;

    ((long double*) &ans)[0] = re;
    ((long double*) &ans)[1] = im;
    return ans;
}
#endif // __cplusplus
#endif // value of COMPLEX_TYPE
#endif // COMPLEX_TYPE
#endif // FLOAT_TYPE

#ifdef __cplusplus
class Copyable_int { // wrap an integer, copyable
public:
    Copyable_int(int v = 0) : val(v) { // construct from value
    }

    Copyable_int(const Copyable_int& x) : val(x.val) { // construct from copied value
    }

    Copyable_int& operator=(const Copyable_int& x) { // copy value
        val = x.val;
        return *this;
    }

    operator int() const { // convert to int
        return val;
    }

    bool operator==(const Copyable_int& x) const { // compare for equality
        return val == x.val;
    }

    bool operator!=(const Copyable_int& x) const { // compare for equality
        return val != x.val;
    }

    bool operator<(const Copyable_int& x) const { // compare for order
        return val < x.val;
    }

    int val;

    Copyable_int(Copyable_int&& x) : val(x.val) { // construct from moved value
        x.val = -1;
    }

    Copyable_int& operator=(Copyable_int&& x) { // move value
        val   = x.val;
        x.val = -1;
        return *this;
    }
};

class Movable_int : public Copyable_int { // wrap a move-only integer
public:
    typedef Copyable_int Mybase;

    Movable_int(int v = 0) : Mybase(v) { // construct from value
    }

    Movable_int(int v1, int v2) : Mybase(v2 + (v1 << 4)) { // construct from two values
    }

    Movable_int(int v1, int v2, int v3) : Mybase(v3 + (v2 << 4) + (v1 << 8)) { // construct from three values
    }

    Movable_int(int v1, int v2, int v3, int v4)
        : Mybase(v4 + (v3 << 4) + (v2 << 8) + (v1 << 12)) { // construct from four values
    }

    Movable_int(int v1, int v2, int v3, int v4, int v5)
        : Mybase(v5 + (v4 << 4) + (v3 << 8) + (v2 << 12) + (v1 << 16)) { // construct from five values
    }

    Movable_int(Movable_int&& right) : Mybase(right.val) { // construct from moved value
        right.val = -1;
    }

    Movable_int& operator=(Movable_int&& right) { // assign from moved value
        if (this != &right) { // different, move it
            val       = right.val;
            right.val = -1;
        }
        return *this;
    }

    operator int() const { // convert to int
        return val;
    }

    bool operator==(const Movable_int& x) const { // compare for equality
        return val == x.val;
    }

    bool operator!=(const Movable_int& x) const { // compare for equality
        return val != x.val;
    }

    bool operator<(const Movable_int& x) const { // compare for order
        return val < x.val;
    }

    Movable_int(const Movable_int&) = delete;
    Movable_int& operator=(const Movable_int&) = delete;
};
#endif // __cplusplus

// FUNCTION main
#ifdef TEST_NAMEX
#define TEST_NAME TEST_NAMEX
#endif // TEST_NAMEX

#ifdef TEST_NAME
void test_main(void);

int main(int argc, char** argv) { // call test, print summary, return
    int nextarg;
    CSTD setvbuf(stdout, nullptr, _IONBF, 0); // disable output buffering

    for (nextarg = 1; nextarg < argc; ++nextarg)
        if (strcmp(argv[nextarg], "-v") == 0)
            verbose = 1;
        else if (strcmp(argv[nextarg], "-t") == 0)
            terse = 1;
        else
            printf("UNEXPECTED argument: %s\n", argv[nextarg]);

#ifndef __cplusplus
    test_main(); // call C test function

#else // __cplusplus
#if NO_EXCEPTIONS
    test_main(); // call C++ test function

#else // NO_EXCEPTIONS
    try { // run tests and catch exceptions
        test_main(); // call C++ test function
    } catch (const STD exception& ex) { // report library exception
        printf("****** LIBRARY EXCEPTION THROWN: %s\n\n", ex.what());
        ++total_fail;
    } catch (const char* msg) { // report string exception
        printf("****** STRING EXCEPTION THROWN: %s\n\n", msg);
        ++total_fail;
    } catch (...) { // report unknown exception
        printf("UNKNOWN EXCEPTION THROWN\n");
        ++total_fail;
    }
#endif // NO_EXCEPTIONS
#endif // __cplusplus

#ifndef TEST_NAMEX
    CSTD puts("FINISHED testing " TEST_NAME);
#endif // TEST_NAMEX

    return leave_chk(TEST_NAME);
}
#endif // TEST_NAME
#endif // TDEFS_H
