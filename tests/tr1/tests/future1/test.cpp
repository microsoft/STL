// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <future> C++0x header
#define TEST_NAME "<future> with multiple threads"

#include "tdefs.h"
#include <condition_variable>
#include <future>
#include <mutex>
#include <thread>

template <template <class T> class Future, class Ty>
class future_tester_base { // base class for testing future types with set and get in
                           // separate threads
public:
    future_tester_base() = default;

    future_tester_base(const future_tester_base&)            = delete;
    future_tester_base& operator=(const future_tester_base&) = delete;
    virtual ~future_tester_base() {}

    typedef STD promise<Ty> MyPromise;
    typedef Future<Ty> MyFuture;
    void run_tests() { // run four test variants
        run_test(set_before_get, set_a_value);
        run_test(set_after_get, set_a_value);
        run_test(set_before_get, set_an_exception);
        run_test(set_after_get, set_an_exception);
    }

protected:
    MyPromise pr;
    MyFuture fut;

private:
    void run_test(int when, int what) { // run test; when determines order of set and get,
                                        // what determines whether result is a value or an exception
        status = ready;
        STD unique_lock<STD mutex> lock(mtx);

        pr = MyPromise();

        if (when == set_after_get) {
            fut = pr.get_future();
        }

        if (what == set_a_value) {
            STD thread thr0(&future_tester_base::set_value, this);
            thr = STD move(thr0);
        } else if (what == set_an_exception) {
            STD thread thr0(&future_tester_base::set_exception, this);
            thr = STD move(thr0);
        }

        while (status == ready) {
            cnd.wait(lock);
        }

        if (when == set_before_get) {
            fut = pr.get_future();
        }

        if (what == set_a_value) {
            check_value();
        } else if (what == set_an_exception) { // get() should throw exception
            bool thrown = false;
            try { // try to get()
                fut.get();
            } catch (int i) { // catch the exception
                CHECK_INT(i, 3);
                thrown = true;
            }
            CHECK_INT(thrown, true);
        }
        status = done;
        cnd.notify_all();
        thr.join();
    }
    void set_value() { // set a value as the result
        STD unique_lock<STD mutex> lock(mtx);
        do_set_value();
        status = set;
        cnd.notify_all();
    }
    void check_value() { // check the value of the result
        do_check_value();
    }
    virtual void do_set_value()   = 0;
    virtual void do_check_value() = 0;
    void set_exception() { // set an exception as the result
        STD unique_lock<STD mutex> lock(mtx);
        STD exception_ptr ptr = STD make_exception_ptr(3);
        pr.set_exception(ptr);
        status = set;
        cnd.notify_all();
    }
    STD mutex mtx;
    STD condition_variable cnd;
    STD thread thr;
    enum { ready, set, done } status;
    enum { set_before_get, set_after_get };
    enum { set_a_value, set_an_exception };
};

template <template <class T> class Future>
class future_value_tester : public future_tester_base<Future, int> { // class for testing Future<T>
    virtual void do_set_value() override { // set the value
        this->pr.set_value(3);
    }
    virtual void do_check_value() override { // check the value
        CHECK_INT(this->fut.get(), 3);
    }
};

template <template <class T> class Future>
class future_reference_tester : public future_tester_base<Future, int&> { // class for testing Future<T&>
public:
    future_reference_tester() : value(0) { // construct
    }

private:
    virtual void do_set_value() override { // set the value
        value = 3;
        this->pr.set_value(value);
    }
    virtual void do_check_value() override { // check the value
        CHECK_INT(this->fut.get(), 3);
    }
    int value;
};

template <template <class T> class Future>
class future_void_tester : public future_tester_base<Future, void> { // class for testing Future<void>
    virtual void do_set_value() override { // set the value
        this->pr.set_value();
    }
    virtual void do_check_value() override { // get the value
        this->fut.get(); // do not remove
    }
};

static void test_future() { // run tests on STD future
    future_value_tester<STD future> fval;
    fval.run_tests();
    future_reference_tester<STD future> fref;
    fref.run_tests();
    future_void_tester<STD future> fvoid;
    fvoid.run_tests();
}

void test_main() { // test header <future> with multiple threads
    test_future();
}
