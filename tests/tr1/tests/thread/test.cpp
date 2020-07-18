// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <thread> header, C++
#define TEST_NAME "<thread>"

#define _HAS_AUTO_PTR_ETC 1

#include "tdefs.h"
#include <iostream>
#include <sstream>
#include <thread>

#define MOVE(x) STD move(x)

#define DELAY_VALUE 2 // milliseconds for delay()

namespace {

    int called = 0;

    void test_no_args() { // mark that function was called
        called = 1;
    }

    void test_one_arg(int val) { // mark that function was called
        called = val;
    }

    void test_two_args(int val0, int val1) { // mark that function was called
        called = val0 + val1;
    }

    struct fun_obj0 { // struct with function call operator
        void operator()() const { // mark that function was called
            called = 2;
        }
        typedef void result_type;
    };

    struct fun_obj1 : STD unary_function<int, void> { // struct with function call operator
        void operator()(int val) const { // mark that function was called
            called = val;
        }
    };

    struct fun_obj2 : STD binary_function<int, int, void> { // struct with function call operator
        void operator()(int val0, int val1) const { // mark that function was called
            called = val0 + val1;
        }
    };

    void t_ctors() { // test join, constructors except copy constructor
        STD thread thr0;

        CHECK_INT(thr0.get_id() == STD thread::id(), true);
        CHECK_INT(thr0.joinable(), false);

        called = 0;
        STD thread thr1(test_no_args);
        CHECK_INT(thr1.joinable(), true);
        thr1.join();
        CHECK_INT(thr1.joinable(), false);
        CHECK_INT(called, 1);
        CHECK_INT(thr1.get_id() == STD thread::id(), true);

        called = 0;
        fun_obj0 fun0;
        STD thread thr2(fun0);
        CHECK_INT(thr2.joinable(), true);
        thr2.join();
        CHECK_INT(thr2.joinable(), false);
        CHECK_INT(called, 2);
        CHECK_INT(thr2.get_id() == STD thread::id(), true);

        int first, second;
        first = 3;
        STD thread thr3(test_one_arg, first);
        CHECK_INT(thr3.joinable(), true);
        thr3.join();
        CHECK_INT(thr3.joinable(), false);
        CHECK_INT(called, 3);

        fun_obj1 fun1;
        first = 4;
        STD thread thr4(fun1, first);
        CHECK_INT(thr4.joinable(), true);
        thr4.join();
        CHECK_INT(thr4.joinable(), false);
        CHECK_INT(called, 4);

        first  = 4;
        second = 5;
        STD thread thr5(test_two_args, first, second);
        CHECK_INT(thr5.joinable(), true);
        thr5.join();
        CHECK_INT(thr5.joinable(), false);
        CHECK_INT(called, 9);

        first  = 5;
        second = 6;
        fun_obj2 fun2;
        STD thread thr6(fun2, first, second);
        CHECK_INT(thr6.joinable(), true);
        thr6.join();
        CHECK_INT(thr6.joinable(), false);
        CHECK_INT(called, 11);
    }

    void delay() { // pause, then mark that function was called
        STD this_thread::sleep_for(STD chrono::milliseconds(DELAY_VALUE));
        called = 1;
    }

    void t_copy() { // test copy[move] constructor, assignment, detach
        called = 0;
        STD thread thr0(delay);
        STD thread::id id0 = thr0.get_id();
        CHECK_INT(thr0.joinable(), true);
        CHECK_INT(id0 != STD this_thread::get_id(), true);
        STD thread thr1(MOVE(thr0));
        CHECK_INT(thr0.joinable(), false);
        CHECK_INT(thr0.get_id() == STD thread::id(), true);
        CHECK_INT(thr1.joinable(), true);
        CHECK_INT(thr1.get_id() == id0, true);

        STD thread thr2;
        thr2 = MOVE(thr1);
        CHECK_INT(thr1.joinable(), false);
        CHECK_INT(thr1.get_id() == STD thread::id(), true);
        CHECK_INT(thr2.joinable(), true);
        CHECK_INT(thr2.get_id() == id0, true);
        thr2.join();
        CHECK_INT(called, 1);

        called = 0;
        STD thread thr3(delay);
        CHECK_INT(thr3.joinable(), true);
        thr3.detach();
        CHECK_INT(called, 0);
        CHECK_INT(thr3.joinable(), false);
        CHECK_INT(thr3.get_id() == STD thread::id(), true);
    }

    void t_swap0() {
        STD thread thr0;
        STD thread thr1;

        thr0.swap(thr1);
        CHECK_INT(thr0.get_id() == STD thread::id(), true);
        CHECK_INT(thr0.joinable(), false);
        CHECK_INT(thr1.get_id() == STD thread::id(), true);
        CHECK_INT(thr1.joinable(), false);

        thr1.swap(thr0);
        CHECK_INT(thr0.get_id() == STD thread::id(), true);
        CHECK_INT(thr0.joinable(), false);
        CHECK_INT(thr1.get_id() == STD thread::id(), true);
        CHECK_INT(thr1.joinable(), false);

        STD swap(thr0, thr1);
        CHECK_INT(thr0.get_id() == STD thread::id(), true);
        CHECK_INT(thr0.joinable(), false);
        CHECK_INT(thr1.get_id() == STD thread::id(), true);
        CHECK_INT(thr1.joinable(), false);

        STD swap(thr1, thr0);
        CHECK_INT(thr0.get_id() == STD thread::id(), true);
        CHECK_INT(thr0.joinable(), false);
        CHECK_INT(thr1.get_id() == STD thread::id(), true);
        CHECK_INT(thr1.joinable(), false);
    }

    void t_swap1() {
        called = 0;
        STD thread thr0(delay);
        STD thread::id id0 = thr0.get_id();
        STD thread thr1;

        thr0.swap(thr1);
        CHECK_INT(thr0.get_id() == STD thread::id(), true);
        CHECK_INT(thr0.joinable(), false);
        CHECK_INT(thr1.get_id() == id0, true);
        CHECK_INT(thr1.joinable(), true);

        thr1.swap(thr0);
        CHECK_INT(thr0.get_id() == id0, true);
        CHECK_INT(thr0.joinable(), true);
        CHECK_INT(thr1.get_id() == STD thread::id(), true);
        CHECK_INT(thr1.joinable(), false);

        STD swap(thr0, thr1);
        CHECK_INT(thr0.get_id() == STD thread::id(), true);
        CHECK_INT(thr0.joinable(), false);
        CHECK_INT(thr1.get_id() == id0, true);
        CHECK_INT(thr1.joinable(), true);

        STD swap(thr1, thr0);
        CHECK_INT(thr0.get_id() == id0, true);
        CHECK_INT(thr0.joinable(), true);
        CHECK_INT(thr1.get_id() == STD thread::id(), true);
        CHECK_INT(thr1.joinable(), false);

        thr0.join();
        CHECK_INT(called, 1);
    }

    void t_swap2() {
        called = 0;
        STD thread thr0(delay);
        STD thread::id id0 = thr0.get_id();
        STD thread thr1(delay);
        STD thread::id id1 = thr1.get_id();

        thr0.swap(thr1);
        CHECK_INT(thr0.get_id() == id1, true);
        CHECK_INT(thr0.joinable(), true);
        CHECK_INT(thr1.get_id() == id0, true);
        CHECK_INT(thr1.joinable(), true);

        thr1.swap(thr0);
        CHECK_INT(thr0.get_id() == id0, true);
        CHECK_INT(thr0.joinable(), true);
        CHECK_INT(thr1.get_id() == id1, true);
        CHECK_INT(thr1.joinable(), true);

        STD swap(thr0, thr1);
        CHECK_INT(thr0.get_id() == id1, true);
        CHECK_INT(thr0.joinable(), true);
        CHECK_INT(thr1.get_id() == id0, true);
        CHECK_INT(thr1.joinable(), true);

        STD swap(thr1, thr0);
        CHECK_INT(thr0.get_id() == id0, true);
        CHECK_INT(thr0.joinable(), true);
        CHECK_INT(thr1.get_id() == id1, true);
        CHECK_INT(thr1.joinable(), true);

        thr0.join();
        thr1.join();
        CHECK_INT(called, 1);
    }

    void t_id() {
        STD thread::id my_id = STD this_thread::get_id();
        CHECK_INT(my_id != STD thread::id(), true);
        STD thread thr0(delay);
        STD thread::id id0 = thr0.get_id();
        STD thread thr1(delay);
        STD thread::id id1 = thr1.get_id();

        CHECK_INT(id0 == id0, true);
        CHECK_INT(!(id0 != id0), true);
        CHECK_INT(!(id0 < id0), true);
        CHECK_INT(id0 <= id0, true);
        CHECK_INT(!(id0 > id0), true);
        CHECK_INT(id0 >= id0, true);

        CHECK_INT(!(id0 == id1), true);
        CHECK_INT(id0 != id1, true);
        CHECK_INT(id0 < id1 && !(id1 < id0) || id1 < id0 && !(id0 < id1), true);
        CHECK_INT(id0 <= id1 && !(id1 <= id0) || id1 <= id0 && !(id0 <= id1), true);
        CHECK_INT(id0 > id1 && !(id1 > id0) || id1 > id0 && !(id0 > id1), true);
        CHECK_INT(id0 >= id1 && !(id1 >= id0) || id1 >= id0 && !(id0 >= id1), true);

        if (id0 < id1 && id1 < my_id)
            CHECK_INT(id0 < my_id, true);
        else if (id0 < my_id && my_id < id1)
            CHECK_INT(id0 < id1, true);
        else if (id1 < id0 && id0 < my_id)
            CHECK_INT(id1 < my_id, true);
        else if (id1 < my_id && my_id < id0)
            CHECK_INT(id1 < id0, true);
        else if (my_id < id0 && id0 < id1)
            CHECK_INT(my_id < id1, true);
        else if (my_id < id1 && id1 < id0)
            CHECK_INT(my_id < id0, true);
        else
            CHECK_MSG("Non-transitive id <", false);

        STD ostringstream str0;
        str0 << id0;
        STD ostringstream str1;
        str1 << id1;
        STD ostringstream str2;
        str2 << my_id;
        STD ostringstream str3;
        str3 << STD thread::id();
        CHECK_INT(str0.str() != str1.str(), true);
        CHECK_INT(str0.str() != str2.str(), true);
        CHECK_INT(str0.str() != str3.str(), true);
        CHECK_INT(str1.str() != str2.str(), true);
        CHECK_INT(str1.str() != str3.str(), true);
        CHECK_INT(str2.str() != str3.str(), true);

        STD hash<STD thread::id> hasher;
        CHECK_INT(hasher(id1), hasher(id1));

        thr1.join();
        thr0.join();
    }

    void t_misc() {
        STD this_thread::yield(); // nothing meaningful to test
        CHECK_INT(STD thread::hardware_concurrency() >= 1, true);

        {
            xtime tgt;
            xtime_get(&tgt, TIME_UTC);
            tgt.nsec += 30000000;
            if (1000000000 <= tgt.nsec) {
                tgt.nsec -= 1000000000;
                ++tgt.sec;
            }
            STD this_thread::sleep_until(&tgt);
            xtime now;
            xtime_get(&now, TIME_UTC);
            CHECK_INT(tgt.sec < now.sec || tgt.sec == now.sec && tgt.nsec <= now.nsec, true);
        }

        {
            STD chrono::system_clock::time_point tgt =
                STD chrono::system_clock::now() + STD chrono::system_clock::duration(100);
            STD this_thread::sleep_until(tgt);
            CHECK_INT(tgt <= STD chrono::system_clock::now(), true);

            tgt = STD chrono::system_clock::now() - STD chrono::system_clock::duration(100);
            STD this_thread::sleep_until(tgt);
            CHECK_INT(tgt <= STD chrono::system_clock::now(), true);
        }

        {
            STD chrono::system_clock::duration dur   = STD chrono::system_clock::duration(100);
            STD chrono::system_clock::time_point tgt = STD chrono::system_clock::now() + dur;
            STD this_thread::sleep_for(dur);
            CHECK_INT(tgt <= STD chrono::system_clock::now(), true);

            dur = -dur;
            tgt = STD chrono::system_clock::now() + dur;
            STD this_thread::sleep_for(dur);
            CHECK_INT(tgt <= STD chrono::system_clock::now(), true);
        }
    }
} // unnamed namespace

void test_main() { // test header <thread>
    if (!terse) { // display value of __STDCPP_THREADS__
#ifdef __STDCPP_THREADS__
#define STRING2(x) #x
#define STRING(x)  STRING2(<x>)
#define MACRO_NAME STRING(__STDCPP_THREADS__)
        _CSTD printf("__STDCPP_THREADS__ = %s\n", MACRO_NAME);
#else // __STDCPP_THREADS__
        _CSTD printf("__STDCPP_THREADS__ not defined\n");
#endif // __STDCPP_THREADS__
    }

    t_ctors();
    t_copy();
    t_swap0();
    t_swap1();
    t_swap2();
    t_id();
    t_misc();
}
