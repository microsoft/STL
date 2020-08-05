// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <atomic> C++11 header with multiple threads
#define TEST_NAME "<atomic>, part 1"
#undef NDEBUG

#include "tdefs.h"
#include <assert.h>
#include <atomic>
#include <limits>
#include <memory>
#include <thread>

// These tests look for synchronization failures on an atomic type. They
// test only the unsigned arithmetic types, assuming that the signed types
// rely on the same underlying code; they also take advantage of the
// standard wraparound behavior of unsigned types.

// The tests exercise the following functions:
//         load()
//         store()
//         exchange()
//         compare_exchange_weak()
//         compare_exchange_strong()
//         fetch_add()
//         fetch_sub()
//         fetch_xor()
// fetch_and and fetch_or are omitted -- it's hard to come up with a useful
// invariant that remains true over multiple asynchronous applications of these
// operations. These functions will, in general, rely on the same
// synchronization mechanism as fetch_xor. If there is an error
// in the implementation it should show up in the semantic tests.

// All of the tests use the assert() macro for internal checks. They can't
// use the CHECK_XXX macros because the CHECK_XXX macros
// aren't synchronized, and adding synchronization to the macros would
// introduce external synchronization into these tests, defeating their
// purpose.

// SUPPORT AND CONFIGURATION

// adjust to get reasonable testing times for the platform:
#if ATOMIC_LLONG_LOCK_FREE
static const unsigned long iterations = 1000000;
#else // ATOMIC_LLONG_LOCK_FREE
static const unsigned long iterations = 100000;
#endif // ATOMIC_LLONG_LOCK_FREE

inline unsigned long select(unsigned long first, unsigned long second) { // silence warnings
    return first < second ? first + 1 : second;
}

template <class Ty>
static unsigned long test_iterations() { // limit test iterations to
                                         // numeric_limits<Ty>::max() + 1 for small types
    return select((unsigned long) STD numeric_limits<Ty>::max(), iterations);
}

// CLASS test_runner
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

// TESTERS
struct flag_tester { // class for testing atomic_flag
public:
    static void run() { // run 4 threads competing for flag
        test_runner runner;
        runner.run_threads(&flag_tester::do_it, &flag_tester::do_it, &flag_tester::do_it, &flag_tester::do_it);
        CHECK_INT(val0, 0);
    }

private:
    static void do_it() { // use flag as lock surrounding non-optimizable
                          // manipulations of shared variable
        for (unsigned long i = 0; i < iterations; ++i) { // lock, manipulate, unlock
            while (flag.test_and_set(STD memory_order_acquire)) {
                // wait for lock
            }
            ++val0;
            assert(val0 == 1);
            val0 += 3;
            val0 /= 2;
            val0 *= 3;
            val0 -= 5;
            assert(val0 == 1);
            --val0;
            flag.clear(STD memory_order_release);
            STD this_thread::yield();
        }
    }
    static STD atomic_flag flag;
    static unsigned long val0;
};
STD atomic_flag flag_tester::flag;
unsigned long flag_tester::val0;

// CLASS TEMPLATE read_write_tester
template <class Ty>
struct read_write_tester { // class for testing acquire/release semantics for load/store ops
public:
    static void run() { // run 2 threads, one writing and one reading
        test_runner runner;
        runner.run_threads(&read_write_tester::write, &read_write_tester::read);
        CHECK_INT(val0 == test_iterations<Ty>() - 1, true);
        CHECK_INT(val1 == test_iterations<Ty>() - 1, true);
    }

private:
    static void write() { // do atomic writes with relaxed/release memory order
        unsigned long limit = test_iterations<Ty>() - 1;
        for (unsigned long i = 0; i < limit; ++i) { // do atomic stores
            Ty my_val = val0.load(STD memory_order_relaxed);
            val0.store(my_val + 1, STD memory_order_relaxed);
            val1.store(my_val + 1, STD memory_order_release);
        }
    }
    static void read() { // do atomic reads with relaxed/acquire memory order
        unsigned long limit = test_iterations<Ty>() - 1;
        for (unsigned long i = 0; i < limit; ++i) { // read values and check for proper update visibility
            Ty my_val1a = val1.load(STD memory_order_acquire);
            Ty my_val0a = val0.load(STD memory_order_relaxed);
            Ty my_val1b = STD atomic_load_explicit(&val1, STD memory_order_relaxed);
            Ty my_val0b = STD atomic_load_explicit(&val0, STD memory_order_relaxed);
            assert(my_val1a <= my_val0a);
            assert(my_val1b <= my_val0b);
        }
    }
    static STD atomic<Ty> val0;
    static STD atomic<Ty> val1;
};
template <class Ty>
STD atomic<Ty> read_write_tester<Ty>::val0;
template <class Ty>
STD atomic<Ty> read_write_tester<Ty>::val1;

// CLASS TEMPLATE exchange_tester
template <class Ty>
struct exchange_tester { // class for testing atomicity of exchange operations
public:
    static void run() { // run 4 threads, all exchanging shared value
        test_runner runner;
        runner.run_threads(
            &exchange_tester::do_it, &exchange_tester::do_it, &exchange_tester::do_it, &exchange_tester::do_it);
        CHECK_INT(val0 == (iterations & STD numeric_limits<Ty>::max()), true);
    }

private:
    static void do_it() { // repeatedly exchange values
        for (unsigned long i = 0; i < iterations; ++i)
            val0.exchange((Ty)(i + 1));
    }
    static STD atomic<Ty> val0;
};
template <class Ty>
STD atomic<Ty> exchange_tester<Ty>::val0;

template <class Ty>
struct cas_weak_tester { // class for testing atomicity of
                         // compare-and-exchange-weak operations
public:
    static void run() { // run 4 threads, all comparing and exchanging shared value
        test_runner runner;
        runner.run_threads(
            &cas_weak_tester::do_it, &cas_weak_tester::do_it, &cas_weak_tester::do_it, &cas_weak_tester::do_it);
        CHECK_INT(val0 == (Ty) test_iterations<Ty>(), true);
    }

private:
    static void do_it() { // repeatedly compare-and-exchange values
        Ty my_val           = 0;
        Ty my_old_val       = my_val;
        unsigned long limit = test_iterations<Ty>() / 4;
        for (unsigned long i = 0; i < limit; ++i) { // compare-and-exchange incremented value
            while (!val0.compare_exchange_weak(my_val, my_val + 1))
                assert(my_old_val < my_val);
            my_old_val = my_val;
            STD this_thread::yield();
        }
    }
    static STD atomic<Ty> val0;
};
template <class Ty>
STD atomic<Ty> cas_weak_tester<Ty>::val0;

template <class Ty>
struct cas_strong_tester { // class for testing atomicity of
                           // compare-and-exchange-strong operations
public:
    static void run() { // run 4 threads, all comparing and exchanging shared value
        test_runner runner;
        runner.run_threads(
            &cas_strong_tester::do_it, &cas_strong_tester::do_it, &cas_strong_tester::do_it, &cas_strong_tester::do_it);
        CHECK_INT(val0 == (Ty) test_iterations<Ty>(), true);
    }

private:
    static void do_it() { // repeatedly compare-and-exchange values
        Ty my_val           = 0;
        Ty my_old_val       = my_val;
        unsigned long limit = test_iterations<Ty>() / 4;
        for (unsigned long i = 0; i < limit; ++i) { // compare-and-exchange incremented value
            while (!val0.compare_exchange_strong(my_val, my_val + 1))
                assert(my_old_val < my_val);
            my_old_val = my_val;
            STD this_thread::yield();
        }
    }
    static STD atomic<Ty> val0;
};
template <class Ty>
STD atomic<Ty> cas_strong_tester<Ty>::val0;

template <class Ty>
struct arithmetic_tester { // class for testing atomicity of arithmetic operations
public:
    static void run() { // run 4 threads, all updating shared value
        test_runner runner;
        runner.run_threads(&arithmetic_tester::do_it0, &arithmetic_tester::do_it1, &arithmetic_tester::do_it2,
            &arithmetic_tester::do_it3);
        CHECK_INT(val0 == 0, true);
    }

private:
    static void do_it0() { // increment/decrement by 2
        inc_dec(2);
    }
    static void do_it1() { // increment/decrement by 3
        inc_dec(3);
    }
    static void do_it2() { // increment/decrement by max() - 5
        inc_dec(STD numeric_limits<Ty>::max() - 5);
    }
    static void do_it3() { // increment/decrement by max() - 7
        inc_dec(STD numeric_limits<Ty>::max() - 7);
    }
    static void inc_dec(Ty value) { // increment/decrement by value
        unsigned long limit = iterations;
        for (unsigned long i = 0; i < limit; ++i)
            val0.fetch_add(value, STD memory_order_acq_rel);
        for (unsigned long i = 0; i < limit; ++i)
            val0.fetch_sub(value, STD memory_order_acq_rel);
    }
    static STD atomic<Ty> val0;
};
template <class Ty>
STD atomic<Ty> arithmetic_tester<Ty>::val0;

template <class Ty>
struct xor_tester { // class for testing atomicity of xor operations
public:
    static void run() { // run 4 threads, all updating shared value
        test_runner runner;
        runner.run_threads(&xor_tester::do_it0, &xor_tester::do_it1, &xor_tester::do_it2, &xor_tester::do_it3);
        CHECK_INT(val0 == 0, true);
    }

private:
    static void do_it0() { // xor with 0011...
        xor_it(0x3333333333333333ULL & STD numeric_limits<Ty>::max());
    }
    static void do_it1() { // xor with 0101...
        xor_it(0x5555555555555555ULL & STD numeric_limits<Ty>::max());
    }
    static void do_it2() { // xor with 1010...
        xor_it(0xAAAAAAAAAAAAAAAAULL & STD numeric_limits<Ty>::max());
    }
    static void do_it3() { // xor with 1100...
        xor_it(0xCCCCCCCCCCCCCCCCULL & STD numeric_limits<Ty>::max());
    }
    static void xor_it(Ty value) { // xor with value
        for (unsigned long i = 0; i < iterations; ++i)
            val0.fetch_xor(value, STD memory_order_acq_rel);
    }
    static STD atomic<Ty> val0;
};
template <class Ty>
STD atomic<Ty> xor_tester<Ty>::val0;

// RUN TESTS
template <class Ty>
void test_for_type() { // run tests for type Ty
    read_write_tester<Ty>::run();
    exchange_tester<Ty>::run();
    cas_weak_tester<Ty>::run();
    cas_strong_tester<Ty>::run();
    arithmetic_tester<Ty>::run();
    xor_tester<Ty>::run();
}

void display(const char* mesg) { // optionally display progress message
    if (!terse)
        printf("%s", mesg);
}

void test_main() { // test header <atomic>
    display("testing atomic_flag:\n");
    flag_tester::run();
    display("testing type unsigned char:\n");
    test_for_type<unsigned char>();
    display("testing type unsigned short:\n");
    test_for_type<unsigned short>();
    display("testing type unsigned int:\n");
    test_for_type<unsigned int>();
    display("testing type unsigned long:\n");
    test_for_type<unsigned long>();
    display("testing type unsigned long long:\n");
    test_for_type<unsigned long long>();
}
