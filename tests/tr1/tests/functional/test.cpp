// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <functional>
#define TEST_NAME "<functional>"

#define _HAS_AUTO_PTR_ETC                1
#define _HAS_DEPRECATED_ADAPTOR_TYPEDEFS 1
#define _HAS_DEPRECATED_NEGATORS         1
#define _SILENCE_CXX17_NEGATORS_DEPRECATION_WARNING

#include "tdefs.h"
#include <algorithm>
#include <functional>
#include <string.h>
#include <string>

void test_simple(char* first, char* last, char* dest) { // test simple function objects
    typedef STD unary_function<char, int> Uf;
    Uf::argument_type* pa0 = (char*) nullptr;
    Uf::result_type* pr0   = (int*) nullptr;

    typedef STD binary_function<char, int, float> Bf;
    Bf::first_argument_type* pa1  = (char*) nullptr;
    Bf::second_argument_type* pa2 = (int*) nullptr;
    Bf::result_type* pr1          = (float*) nullptr;

    char* mid = first + 2;

    pa0 = pa0; // to quiet diagnostics
    pr0 = pr0;
    pa1 = pa1;
    pa2 = pa2;
    pr1 = pr1;

    CSTD strcpy(first, "\4\3\2\1");
    CSTD strcpy(dest, "abcd");
    STD transform(first, mid, mid, dest, STD plus<char>());
    CHECK_MEM(dest, "\6\4cd", 4);
    STD transform(first, mid, mid, dest, STD minus<char>());
    CHECK_MEM(dest, "\2\2cd", 4);
    STD transform(first, mid, mid, dest, STD multiplies<char>());
    CHECK_MEM(dest, "\10\3cd", 4);
    STD transform(first, mid, mid, dest, STD divides<char>());
    CHECK_MEM(dest, "\2\3cd", 4);
    STD transform(first, mid, first + 1, dest, STD modulus<char>());
    CHECK_MEM(dest, "\1\1cd", 4);
    STD transform(first, mid, dest, STD negate<char>());
    CHECK_INT(((signed char*) dest)[0], -4);
    CHECK_INT(((signed char*) dest)[1], -3);

    STD transform(first, mid, "\4\4", dest, STD equal_to<char>());
    CHECK_MEM(dest, "\1\0cd", 4);
    STD transform(first, mid, "\4\4", dest, STD not_equal_to<char>());
    CHECK_MEM(dest, "\0\1cd", 4);
    STD transform(first, mid, "\3\3", dest, STD greater<char>());
    CHECK_MEM(dest, "\1\0cd", 4);
    STD transform(first, mid, "\4\4", dest, STD less<char>());
    CHECK_MEM(dest, "\0\1cd", 4);
    STD transform(first, mid, "\4\4", dest, STD greater_equal<char>());
    CHECK_MEM(dest, "\1\0cd", 4);
    STD transform(first, mid, "\3\3", dest, STD less_equal<char>());
    CHECK_MEM(dest, "\0\1cd", 4);

    STD transform(last - 1, last + 1, "\1\0", dest, STD logical_and<char>());
    CHECK_MEM(dest, "\1\0cd", 4);
    STD transform(last - 1, last + 1, dest, STD logical_not<char>());
    CHECK_MEM(dest, "\0\1cd", 4);
    STD transform(last - 1, last + 1, "\0\0", dest, STD logical_or<char>());
    CHECK_MEM(dest, "\1\0cd", 4);

    STD transform(first, mid, mid, dest, STD bit_and<char>());
    CHECK_MEM(dest, "\0\1cd", 4);
    STD transform(first, mid, mid, dest, STD bit_or<char>());
    CHECK_MEM(dest, "\6\3cd", 4);
    STD transform(first, mid, mid, dest, STD bit_xor<char>());
    CHECK_MEM(dest, "\6\2cd", 4);
}

class Mybind1st : public STD binder1st<STD minus<char>> {
public:
    typedef STD binder1st<STD minus<char>> Mybase;
    typedef STD minus<char> Myop;

    Mybind1st(const STD minus<char>& func, char left) : Mybase(func, left) { // construct from functor and left operand
    }

    Myop getop() const { // return protected op
        return op;
    }

    char getval() const { // return protected value
        return value;
    }
};

class Mybind2nd : public STD binder2nd<STD minus<char>> {
public:
    typedef STD binder2nd<STD minus<char>> Mybase;
    typedef STD minus<char> Myop;

    Mybind2nd(const STD minus<char>& func, char right)
        : Mybase(func, right) { // construct from functor and right operand
    }

    Myop getop() const { // return protected op
        return op;
    }

    char getval() const { // return protected value
        return value;
    }
};

void test_compound(char* first, char* last, char* dest) { // test compound function objects
    char* mid = first + 2;
    STD logical_not<char> not_op;
    STD unary_negate<STD logical_not<char>> unop(not_op);

    CSTD strcpy(first, "\4\3\2\1");
    CSTD strcpy(dest, "abcd");
    STD transform(last - 1, last + 1, dest, unop);
    STD transform(last - 1, last + 1, dest + 2, STD not1(STD logical_not<char>()));
    CHECK_MEM(dest, "\1\0\1\0", 4);

    STD less<char> less_op;
    STD binary_negate<STD less<char>> binop(less_op);
    STD transform(first, mid, "\5\2", dest, binop);
    STD transform(first, mid, "\5\2", dest + 2, STD not2(STD less<char>()));
    CHECK_MEM(dest, "\0\1\0\1", 4);

    STD plus<char> plus_op;
    STD binder1st<STD plus<char>> add1(plus_op, '\1');
    STD transform(first, mid, dest, add1);
    STD transform(mid, last, dest + 2, STD bind1st(STD plus<char>(), '\1'));
    CHECK_MEM(dest, "\5\4\3\2", 4);

    STD minus<char> minus_op;
    STD binder2nd<STD minus<char>> sub1(minus_op, '\1');
    STD transform(first, mid, dest, sub1);
    STD transform(mid, last, dest + 2, STD bind2nd(STD minus<char>(), '\1'));
    CHECK_MEM(dest, "\3\2\1\0", 4);
}

// functions to test pointer function objects
char ufn(char ch) { // unary increment function
    return (char) (ch + 1);
}

char bfn(char ch1, char ch2) { // binary add function
    return (char) (ch1 + ch2);
}

struct Myclass { // non-const functor
    char fn0() { // return 7
        return '\7';
    }

    char fn1(char ch) { // increment argument
        return (char) (ch + 1);
    }
} mycl;

struct Mycclass { // const functor
    char fn0() const { // return 7
        return '\7';
    }

    char fn1(char ch) const {
        return (char) (ch + 1);
    }
} myccl;

void test_pointer(char* first, char* last, char* dest) { // test pointer function objects
    char* mid = first + 2;

    CSTD strcpy(first, "\4\3\2\1");
    CSTD strcpy(dest, "abcd");
    STD pointer_to_unary_function<char, char> uf(ufn);

    STD transform(first, mid, dest, uf);
    STD transform(mid, last, dest + 2, STD ptr_fun(ufn));
    CHECK_MEM(dest, "\5\4\3\2", 4);
    STD pointer_to_binary_function<char, char, char> bf(bfn);
    STD transform(first, mid, "\2\2", dest, bf);
    STD transform(mid, last, "\2\2", dest + 2, STD ptr_fun(bfn));
    CHECK_MEM(dest, "\6\5\4\3", 4);

    STD mem_fun_t<char, Myclass> mf(&Myclass::fn0);
    CHECK_INT(mf(&mycl), '\7');
    CHECK_INT(STD mem_fun(&Myclass::fn0)(&mycl), '\7');
    STD mem_fun1_t<char, Myclass, char> mf1(&Myclass::fn1);
    CHECK_INT(mf1(&mycl, '\3'), '\4');
    CHECK_INT(STD mem_fun(&Myclass::fn1)(&mycl, '\3'), '\4');
    STD mem_fun_ref_t<char, Myclass> mfr(&Myclass::fn0);
    CHECK_INT(mfr(mycl), '\7');
    CHECK_INT(STD mem_fun_ref(&Myclass::fn0)(mycl), '\7');
    STD mem_fun1_ref_t<char, Myclass, char> mf1r(&Myclass::fn1);
    CHECK_INT(mf1r(mycl, '\3'), '\4');
    CHECK_INT(STD mem_fun_ref(&Myclass::fn1)(mycl, '\3'), '\4');

    STD const_mem_fun_t<char, Mycclass> cmf(&Mycclass::fn0);
    CHECK_INT(cmf(&myccl), '\7');
    CHECK_INT(STD mem_fun(&Mycclass::fn0)(&myccl), '\7');
    STD const_mem_fun1_t<char, Mycclass, char> cmf1(&Mycclass::fn1);
    CHECK_INT(cmf1(&myccl, '\3'), '\4');
    CHECK_INT(STD mem_fun(&Mycclass::fn1)(&myccl, '\3'), '\4');
    STD const_mem_fun_ref_t<char, Mycclass> cmfr(&Mycclass::fn0);
    CHECK_INT(cmfr(myccl), '\7');
    CHECK_INT(STD mem_fun_ref(&Mycclass::fn0)(myccl), '\7');
    STD const_mem_fun1_ref_t<char, Mycclass, char> cmf1r(&Mycclass::fn1);
    CHECK_INT(cmf1r(myccl, '\3'), '\4');
    CHECK_INT(STD mem_fun_ref(&Mycclass::fn1)(myccl, '\3'), '\4');
}

void test_main() { // test basic workings of functional definitions
    char buf[]  = "\4\3\2\1";
    char dest[] = "abcd";
    char *first = buf, *last = buf + 4;

    test_simple(first, last, dest);
    test_compound(first, last, dest);
    test_pointer(first, last, dest);

    CSTD size_t hash_val = STD hash<int>()(3);
    (void) hash_val;

    hash_val = STD hash<double>()(3.0);
    hash_val = STD hash<STD string>()(STD string("abc"));
    hash_val = STD hash<STD wstring>()(STD wstring(L"abc"));
    hash_val = STD hash<long long>()((long long) 3);
}
