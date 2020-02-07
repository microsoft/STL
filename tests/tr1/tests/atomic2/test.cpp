// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <atomic> C++11 header with UDT and multiple threads
#define TEST_NAME "<atomic>, part 2"
#undef NDEBUG

#include "tdefs.h"
#include <assert.h>
#include <atomic>
#include <limits>
#include <memory>
#include <thread>

// These tests look for synchronization failures on an atomic type managing
// an object of a user-defined type.

// The tests exercise the following functions:
//         load()
//         store()
//         exchange()
//         compare_exchange_weak()
//         compare_exchange_strong()

// All of the tests use the assert() macro for internal checks. They can't
// use the CHECK_XXX macros because the CHECK_XXX macros aren't
// synchronized, and adding synchronization to the macros would introduce
// external synchronization into these tests, defeating their purpose.

// SUPPORT AND CONFIGURATION

// adjust to get reasonable testing times for the platform:
static const unsigned long iterations = 1000000;

class test_runner { // class to run multiple threads and wait for them all to finish
public:
    template <class Fn0>
    void run_threads(Fn0 fn0) { // run 1 thread
        thread_count = 1;
        STD thread thr0(fn0);
        threads[0].swap(thr0);
        wait();
    }
    template <class Fn0, class Fn1>
    void run_threads(Fn0 fn0, Fn1 fn1) { // run 2 threads
        thread_count = 2;
        STD thread thr0(fn0);
        threads[0].swap(thr0);
        STD thread thr1(fn1);
        threads[1].swap(thr1);
        wait();
    }
    template <class Fn0, class Fn1, class Fn2>
    void run_threads(Fn0 fn0, Fn1 fn1, Fn2 fn2) { // run 3 threads
        thread_count = 3;
        STD thread thr0(fn0);
        threads[0].swap(thr0);
        STD thread thr1(fn1);
        threads[1].swap(thr1);
        STD thread thr2(fn2);
        threads[2].swap(thr2);
        wait();
    }
    template <class Fn0, class Fn1, class Fn2, class Fn3>
    void run_threads(Fn0 fn0, Fn1 fn1, Fn2 fn2, Fn3 fn3) { // run 4 threads
        thread_count = 4;
        STD thread thr0(fn0);
        threads[0].swap(thr0);
        STD thread thr1(fn1);
        threads[1].swap(thr1);
        STD thread thr2(fn2);
        threads[2].swap(thr2);
        STD thread thr3(fn3);
        threads[3].swap(thr3);
        wait();
    }

private:
    void wait() { // wait for all threads to finish
        for (int i = 0; i < thread_count; ++i)
            threads[i].join();
    }
    int thread_count;
    STD thread threads[4];
};

struct UDT { // struct for testing atomic<UDT>
    unsigned long long counter0;
    unsigned char counter1;
    unsigned long long counter2;
};

inline void increment(UDT& udt) { // increment counters in UDT object
    ++udt.counter0;
    ++udt.counter1;
    ++udt.counter2;
}

inline bool operator==(const UDT& left, const UDT& right) { // return true if left and right are equal
    return left.counter0 == right.counter0 && left.counter1 == right.counter1 && left.counter2 == right.counter2;
}

inline bool operator!=(const UDT& left, const UDT& right) { // return true if left and right are not equal
    return !(left == right);
}

inline bool operator<(const UDT& left, const UDT& right) { // return true if left < right
    return left.counter0 < right.counter0;
}

inline bool operator<=(const UDT& left, const UDT& right) { // return true if left <= right
    return !(right < left);
}

struct read_write_tester { // class for testing acquire/release semantics for load/store ops
public:
    static void run() { // run 2 threads, one writing and one reading
        test_runner runner;
        runner.run_threads(&read_write_tester::write, &read_write_tester::read);
        CHECK_INT(val0.load().counter0 == iterations, true);
        CHECK_INT(val0.load().counter1 == (iterations & STD numeric_limits<unsigned char>::max()), true);
        CHECK_INT(val0.load().counter2 == iterations, true);
        CHECK_INT(val1.load().counter0 == iterations, true);
        CHECK_INT(val1.load().counter1 == (iterations & STD numeric_limits<unsigned char>::max()), true);
        CHECK_INT(val1.load().counter2 == iterations, true);
    }

private:
    static void write() { // do atomic writes with relaxed/release memory order
        for (unsigned long i = 0; i < iterations; ++i) { // do atomic stores
            UDT my_val = val0.load(STD memory_order_relaxed);
            increment(my_val);
            val0.store(my_val, STD memory_order_relaxed);
            val1.store(my_val, STD memory_order_release);
        }
    }
    static void read() { // do atomic reads with relaxed/acquire memory order
        for (unsigned long i = 0; i < iterations; ++i) { // read values and check for proper update visibility
            UDT my_val1a = val1.load(STD memory_order_acquire);
            UDT my_val0a = val0.load(STD memory_order_relaxed);
            UDT my_val1b = val1.load(STD memory_order_relaxed);
            UDT my_val0b = val0.load(STD memory_order_relaxed);
            assert(my_val1a <= my_val0a);
            assert(my_val1b <= my_val0b);
        }
    }
    static STD atomic<UDT> val0;
    static STD atomic<UDT> val1;
};
STD atomic<UDT> read_write_tester::val0;
STD atomic<UDT> read_write_tester::val1;

struct exchange_tester { // class for testing atomicity of exchange operations
public:
    static void run() { // run 4 threads, all exchanging shared value
        test_runner runner;
        runner.run_threads(
            &exchange_tester::do_it, &exchange_tester::do_it, &exchange_tester::do_it, &exchange_tester::do_it);
        CHECK_INT(val0.load().counter0 == iterations, true);
        CHECK_INT(val0.load().counter1 == (iterations & STD numeric_limits<unsigned char>::max()), true);
        CHECK_INT(val0.load().counter2 == iterations, true);
    }

private:
    static void do_it() { // repeatedly exchange values
        UDT my_val{};
        for (unsigned long i = 0; i < iterations; ++i) { // repeatedly exchange values
            increment(my_val);
            val0.exchange(my_val);
        }
    }
    static STD atomic<UDT> val0;
};
STD atomic<UDT> exchange_tester::val0;

struct cas_weak_tester { // class for testing atomicity of
                         // compare-and-exchange-weak operations
public:
    static void run() { // run 4 threads, all comparing and exchanging shared value
        test_runner runner;
        runner.run_threads(
            &cas_weak_tester::do_it, &cas_weak_tester::do_it, &cas_weak_tester::do_it, &cas_weak_tester::do_it);
        CHECK_INT(val0.load().counter0 == iterations, true);
        CHECK_INT(val0.load().counter1 == (iterations & STD numeric_limits<unsigned char>::max()), true);
        CHECK_INT(val0.load().counter2 == iterations, true);
    }

private:
    static void do_it() { // repeatedly compare-and-exchange values
        UDT my_val{};
        UDT my_old_val = my_val;
        for (unsigned long i = 0; i < iterations / 4; ++i) { // compare-and-exchange incremented value
            UDT temp_val = my_val;
            increment(temp_val);
            while (!val0.compare_exchange_weak(my_val, temp_val)) { // compare-and-exchange incremented value
                assert(my_old_val < my_val);
                temp_val = my_val;
                increment(temp_val);
            }
            my_old_val = my_val;
            STD this_thread::yield();
        }
    }
    static STD atomic<UDT> val0;
};
STD atomic<UDT> cas_weak_tester::val0;

struct cas_strong_tester { // class for testing atomicity of
                           // compare-and-exchange-strong operations
public:
    static void run() { // run 4 threads, all comparing and exchanging shared value
        test_runner runner;
        runner.run_threads(
            &cas_strong_tester::do_it, &cas_strong_tester::do_it, &cas_strong_tester::do_it, &cas_strong_tester::do_it);
        CHECK_INT(val0.load().counter0 == iterations, true);
        CHECK_INT(val0.load().counter1 == (iterations & STD numeric_limits<unsigned char>::max()), true);
        CHECK_INT(val0.load().counter2 == iterations, true);
    }

private:
    static void do_it() { // repeatedly compare-and-exchange values
        UDT my_val{};
        UDT my_old_val = my_val;
        for (unsigned long i = 0; i < iterations / 4; ++i) { // compare-and-exchange incremented value
            UDT temp_val = my_val;
            increment(temp_val);
            while (!val0.compare_exchange_strong(my_val, temp_val)) { // compare-and-exchange incremented value
                assert(my_old_val < my_val);
                temp_val = my_val;
                increment(temp_val);
            }
            my_old_val = my_val;
            STD this_thread::yield();
        }
    }
    static STD atomic<UDT> val0;
};
STD atomic<UDT> cas_strong_tester::val0;

// RUN TESTS
void test_main() { // test header <atomic>
    read_write_tester::run();
    exchange_tester::run();
    cas_weak_tester::run();
    cas_strong_tester::run();
}
