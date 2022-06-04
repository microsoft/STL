// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <future> C++0x header
#define TEST_NAME "<future>"

#include "tdefs.h"
#include <future>

static int counted_allocator_instances = 0;
static bool counted_allocator_used     = false;

template <class Ty>
class counted_allocator { // allocator with instrumented constructor, destructor
    typedef STD allocator<Ty> MyAllocTy;

public:
    typedef Ty value_type;

    counted_allocator() throw() { // construct
        ++counted_allocator_instances;
    }
    counted_allocator(const counted_allocator&) throw() { // construct copy
        ++counted_allocator_instances;
    }
    template <class Other>
    counted_allocator(const counted_allocator<Other>&) throw() { // construct copy
        ++counted_allocator_instances;
    }
    ~counted_allocator() throw() { // destroy
        --counted_allocator_instances;
    }

    Ty* allocate(size_t sz) { // allocate
        counted_allocator_used = true;
        return MyAllocTy{}.allocate(sz);
    }
    void deallocate(Ty* p, size_t n) { // deallocate
        MyAllocTy{}.deallocate(p, n);
    }

    template <class Other>
    bool operator==(const counted_allocator<Other>&) const {
        return true;
    }

    template <class Other>
    bool operator!=(const counted_allocator<Other>&) const {
        return false;
    }
};

static STD error_code brk(STD future_errc::broken_promise);
static STD error_code rtr(STD future_errc::future_already_retrieved);
static STD error_code sat(STD future_errc::promise_already_satisfied);
static STD error_code nst(STD future_errc::no_state);

static bool check_future_errc(STD future_errc) { // bind to future_errc values
    return 1;
}

static bool check_launch(STD launch) { // bind to launch values
    return 1;
}

static bool check_future_status(STD future_status) { // bind to future_status values
    return 1;
}

static void test_enums() { // test future_errc, launch, and future_status
    CHECK_MSG("future_errc", check_future_errc(STD future_errc::broken_promise));
    CHECK_MSG("future_errc", check_future_errc(STD future_errc::future_already_retrieved));
    CHECK_MSG("future_errc", check_future_errc(STD future_errc::promise_already_satisfied));
    CHECK_MSG("future_errc", check_future_errc(STD future_errc::no_state));
    CHECK_INT(STD is_error_code_enum<STD future_errc>::value, true);

    CHECK_MSG("launch", check_launch(STD launch::async));
    CHECK_MSG("launch", check_launch(STD launch::deferred));
    CHECK_MSG("launch", check_launch(STD launch::async | STD launch::deferred));

    CHECK_MSG("future_status", check_future_status(STD future_status::ready));
    CHECK_MSG("future_status", check_future_status(STD future_status::timeout));
    CHECK_MSG("future_status", check_future_status(STD future_status::deferred));
}

static void test_error_code(STD future_errc code) { // test error_code and error_condition
    STD error_code eco = STD make_error_code(code);
    CHECK_INT((int) eco.value() == (int) code, true);
    CHECK_INT(eco.category() == STD future_category(), true);

    STD error_condition ecn = STD make_error_condition(code);
    CHECK_INT((int) ecn.value() == (int) code, true);
    CHECK_INT(ecn.category() == STD future_category(), true);
}

bool is_error_category(const STD error_category*) { // return true for pointer to error_category
    return true;
}
bool is_error_category(const void*) { // return false for other pointers
    return false;
}

static void test_error_handling() { // test future_category and error codes
    CHECK_STR(STD future_category().name(), "future");
    CHECK_INT(is_error_category(&STD future_category()), true);

    test_error_code(STD future_errc::broken_promise);
    test_error_code(STD future_errc::future_already_retrieved);
    test_error_code(STD future_errc::promise_already_satisfied);
    test_error_code(STD future_errc::no_state);
}

bool is_logic_error(const STD logic_error*) { // return true for pointer to logic_error
    return true;
}
bool is_logic_error(const void*) { // return false for other pointers
    return false;
}

static void test_future_error() { // test class future_error
    STD error_code code = STD make_error_code(STD future_errc::broken_promise);
    STD future_error fe(code);
    CHECK_INT(is_logic_error(&fe), true);
    CHECK_INT(fe.code() == code, true);
    STD string what_msg = fe.what();
    CHECK_INT(what_msg.find(code.message()) != STD string::npos, true);
}

static void test_promise_core() { // test construction, copying, moving, assigning, swapping
    STD promise<int> pr0;
    bool thrown = false;
    try { // set value on default-constructed object
        pr0.set_value(3);
    } catch (...) { // shouldn't get here
        thrown = true;
    }
    CHECK_INT(thrown, false);
    thrown = false;
    try { // set value a second time
        pr0.set_value(3);
    } catch (const STD future_error& err) { // should get here, promise already satisfied
        CHECK_INT(err.code().value() == sat.value(), true);
        thrown = true;
    } catch (...) { // shouldn't get here
    }
    CHECK_INT(thrown, true);

    STD promise<int> pr1(STD move(pr0));
    thrown = false;
    try { // set value on moved-from object
        pr0.set_value(3);
    } catch (const STD future_error& err) { // should get here, no state
        CHECK_INT(err.code().value() == nst.value(), true);
        thrown = true;
    } catch (...) { // shouldn't get here
    }
    CHECK_INT(thrown, true);
    thrown = false;
    try { // set value on moved-to object
        pr1.set_value(3);
    } catch (const STD future_error& err) { // should get here, promise already satisfied
        CHECK_INT(err.code().value() == sat.value(), true);
        thrown = true;
    } catch (...) { // shouldn't get here
    }
    CHECK_INT(thrown, true);

    pr0    = STD move(pr1);
    thrown = false;
    try { // set value on moved-to object
        pr0.set_value(3);
    } catch (const STD future_error& err) { // should get here, promise already satisfied
        CHECK_INT(err.code().value() == sat.value(), true);
        thrown = true;
    } catch (...) { // shouldn't get here
    }
    CHECK_INT(thrown, true);
    thrown = false;
    try { // set value on moved-from object
        pr1.set_value(3);
    } catch (const STD future_error& err) { // should get here, no state
        CHECK_INT(err.code().value() == nst.value(), true);
        thrown = true;
    } catch (...) { // shouldn't get here
    }
    CHECK_INT(thrown, true);

    STD swap(pr0, pr1);
    thrown = false;
    try { // set value on swapped object
        pr0.set_value(3);
    } catch (const STD future_error& err) { // should get here, no state
        CHECK_INT(err.code().value() == nst.value(), true);
        thrown = true;
    } catch (...) { // shouldn't get here
    }
    CHECK_INT(thrown, true);
    thrown = false;
    try { // set value on swapped object
        pr1.set_value(3);
    } catch (const STD future_error& err) { // should get here, promise already satisfied
        CHECK_INT(err.code().value() == sat.value(), true);
        thrown = true;
    } catch (...) { // shouldn't get here
    }
    CHECK_INT(thrown, true);

    pr0.swap(pr1);
    thrown = false;
    try { // set value on swapped object
        pr0.set_value(3);
    } catch (const STD future_error& err) { // should get here, promise already satisfied
        CHECK_INT(err.code().value() == sat.value(), true);
        thrown = true;
    } catch (...) { // shouldn't get here
    }
    CHECK_INT(thrown, true);
    thrown = false;
    try { // set value on swapped object
        pr1.set_value(3);
    } catch (const STD future_error& err) { // should get here, no state
        CHECK_INT(err.code().value() == nst.value(), true);
        thrown = true;
    } catch (...) { // shouldn't get here
    }
    CHECK_INT(thrown, true);

    thrown = false;
    try { // try to get future
        (void) pr1.get_future();
    } catch (const STD future_error& err) { // should get here, no state
        CHECK_INT(err.code().value() == nst.value(), true);
        thrown = true;
    } catch (...) { // shouldn't get here
    }
    CHECK_INT(thrown, true);

    int i = 0;
    try { // try to get future
        (void) pr0.get_future();
        ++i;
        (void) pr0.get_future();
        ++i;
    } catch (const STD future_error& err) { // should get here after second attempt: future already retrieved
        CHECK_INT(err.code().value() == rtr.value(), true);
    } catch (...) { // shouldn't get here
    }
    CHECK_INT(i, 1);

    thrown = false;
    STD future<int> f;
    {
        STD promise<int> pr2;
        f = pr2.get_future();
    }
    try {
        f.get();
    } catch (...) { // should get here, shared state abandoned
        thrown = true;
    }
    CHECK_INT(thrown, true);
}

template <class Ty>
static void call_promise_setter(STD promise<Ty>* pr, int which) { // try to set a result
    switch (which) { // result determined by value of which
    case 0:
        pr->set_value(3);
        break;
    case 1:
        pr->set_exception(STD make_exception_ptr(0));
        break;
    case 2:
        pr->set_value_at_thread_exit(3);
        break;
    case 3:
        pr->set_exception_at_thread_exit(STD make_exception_ptr(0));
        break;
    }
}

template <class Ty>
static void call_promise_setter_from_thread(
    STD promise<Ty>* pr, int which, bool should_throw) { // try to set a result, should succeed if should_throw is false
    bool thrown = false;
    try { // try to set a result
        call_promise_setter(pr, which);
    } catch (const STD future_error& err) { // if we get here, the promise has already been satisfied
        CHECK_INT(err.code().value() == sat.value(), true);
        thrown = true;
    } catch (...) { // shouldn't get here
    }
    CHECK_INT(thrown, should_throw);
}

template <class Ty>
static void test_promise_satisfied(
    STD promise<Ty>& pr, int which) { // set result according to which, then try to set again four ways
    STD thread thr0(&call_promise_setter_from_thread<Ty>, &pr, which, false);
    thr0.join();
    for (int i = 0; i < 4; ++i) { // try to set again, should throw exception
        STD thread thr(&call_promise_setter_from_thread<Ty>, &pr, i, true);
        thr.join();
    }
}

static void test_promise_setters() { // set result four ways, try to set again four ways
    for (int i = 0; i < 4; ++i) { // set result, check
        STD promise<int> pr;
        test_promise_satisfied(pr, i);
    }
}

template <class Ty>
static void do_create_promise_with_allocator(
    counted_allocator<int> alloc) { // create a promise object with an allocator
    STD promise<Ty> pr(STD allocator_arg, alloc);
    STD future<Ty> fut = pr.get_future();
}

template <class Ty>
static void do_test_promise_allocator() { // check that allocator is managed correctly with promise
    counted_allocator_instances = 0;
    counted_allocator_used      = false;
    counted_allocator<int> alloc;
    do_create_promise_with_allocator<Ty>(alloc);
    CHECK_INT(counted_allocator_instances, 1);
    CHECK_INT(counted_allocator_used, true);
}

static void test_promise_allocator() { // test promise with allocator
    typedef STD uses_allocator<STD promise<int>, STD allocator<int>> promise_uses_allocator;
    CHECK_INT(promise_uses_allocator::value, true);

    do_test_promise_allocator<int>();
    do_test_promise_allocator<int&>();
    do_test_promise_allocator<void>();
}

static void test_promise() { // do basic tests for promise
    test_promise_core();
    test_promise_setters();
    test_promise_allocator();
}

template <class Arg>
struct future_factory { // template to generate future<Arg> object and test its get() member
    static STD future<Arg> make_future() { // generate future<Arg> object
        STD promise<Arg> pr;
        pr.set_value(val);
        return pr.get_future();
    }
    template <class Future>
    static bool get_and_check(Future& f) { // test its get() member
        return f.get() == val;
    }
    static Arg val;
};
template <class Arg>
Arg future_factory<Arg>::val = Arg();

template <class Arg>
struct future_factory<Arg&> { // template to generate future<Arg&> object and test its get() member
    static STD future<Arg&> make_future() { // generate future<Arg&> object
        STD promise<Arg&> pr;
        pr.set_value(val);
        return pr.get_future();
    }
    template <class Future>
    static bool get_and_check(Future& f) { // test its get() member
        return f.get() == val;
    }
    static Arg val;
};
template <class Arg>
Arg future_factory<Arg&>::val = Arg();

template <>
struct future_factory<void> { // template to generate future<void> object and test its get() member
    static STD future<void> make_future() { // generate future<void> object
        STD promise<void> pr;
        pr.set_value();
        return pr.get_future();
    }
    template <class Future>
    static bool get_and_check(Future& f) { // test its get() member
        f.get(); // do not remove
        return true;
    }
};

template <class Future, class Arg, bool get_only_once>
struct test_futures { // class for testing various future types
    static void test_core() { // test Future's core operations
        Future f0;
        CHECK_INT(f0.valid(), false);
        Future f1;
        CHECK_INT(f0.valid(), false);
        CHECK_INT(f1.valid(), false);

        Future f0x(future_factory<Arg>::make_future());
        f0 = STD move(f0x);
        CHECK_INT(f0.valid(), true);
        CHECK_INT(future_factory<Arg>::get_and_check(f0), true);
        CHECK_INT(f0.valid(), !get_only_once);
        f1 = STD move(f0);
        CHECK_INT(f1.valid(), !get_only_once);
        bool thrown = false;
        try { // try to get() a second tome
            f1.get();
            CHECK_MSG("shouldn't get here", !get_only_once);
        } catch (const STD future_error& err) { // should get here if get_only_once is true
            CHECK_INT(err.code().value() == nst.value(), true);
            thrown = true;
        }
        CHECK_INT(thrown, get_only_once);
    }

    static void test_copy(bool unique) { // test copying
        Future f0 = future_factory<Arg>::make_future();
        Future f1 = f0;
        CHECK_INT(f0.valid(), !unique);
        CHECK_INT(f1.valid(), true);
        Future f2;
        f2 = f1;
        CHECK_INT(f1.valid(), !unique);
        CHECK_INT(f2.valid(), true);
    }

    static void test_move(bool unique) { // test copying
        Future f0 = future_factory<Arg>::make_future();
        Future f1 = STD move(f0);
        CHECK_INT(f0.valid(), !unique);
        CHECK_INT(f1.valid(), true);
        Future f2;
        f2 = STD move(f1);
        CHECK_INT(f1.valid(), !unique);
        CHECK_INT(f2.valid(), true);
    }
};

static void test_future() { // test STD future
    test_futures<STD future<int>, int, true>::test_core();
    test_futures<STD future<int&>, int&, true>::test_core();
    test_futures<STD future<void>, void, true>::test_core();
    test_futures<STD future<int>, int, true>::test_move(true);
    test_futures<STD future<int&>, int&, true>::test_move(true);
    test_futures<STD future<void>, void, true>::test_move(true);
}

static void test_shared_future() { // test STD shared_future
    test_futures<STD shared_future<int>, int, false>::test_core();
    test_futures<STD shared_future<int&>, int&, false>::test_core();
    test_futures<STD shared_future<void>, void, false>::test_core();
    test_futures<STD shared_future<int>, int, true>::test_copy(false);
    test_futures<STD shared_future<int&>, int&, true>::test_copy(false);
    test_futures<STD shared_future<void>, void, true>::test_copy(false);

    STD future<int> f0(future_factory<int>::make_future());
    STD shared_future<int> sf0 = f0.share();
    CHECK(sf0.valid());

    STD future<int&> f1(future_factory<int&>::make_future());
    STD shared_future<int&> sf1 = f1.share();
    CHECK(sf1.valid());

    STD future<void> f2(future_factory<void>::make_future());
    STD shared_future<void> sf2 = f2.share();
    CHECK(sf2.valid());
}

int func(int i) { // simple function for call from packaged_task
    STD this_thread::sleep_for(STD chrono::microseconds(10));
    return i - 1;
}

typedef decltype(func) Func_type;

static int func_res = 0;
int& funcr(int i) { // simple function for call from packaged_task
    STD this_thread::sleep_for(STD chrono::microseconds(10));
    func_res = i - 1;
    return func_res;
}
typedef decltype(funcr) Funcr_type;

void funcv(int i) { // simple function for call from packaged_task
    STD this_thread::sleep_for(STD chrono::microseconds(10));
    func_res = i - 1;
    return;
}
typedef decltype(funcv) Funcv_type;

struct fnobj { // simple function object for call from packaged_task
    int operator()(int i) { // function call operator
        STD this_thread::sleep_for(STD chrono::microseconds(10));
        return i + 1;
    }
    typedef int result_type;
};

struct fnobjr { // simple function object for call from packaged_task
    int& operator()(int i) { // function call operator
        STD this_thread::sleep_for(STD chrono::microseconds(10));
        func_res = i + 1;
        return func_res;
    }
    typedef int& result_type;
};

struct fnobjv { // simple function object for call from packaged_task
    void operator()(int i) { // function call operator
        STD this_thread::sleep_for(STD chrono::microseconds(10));
        func_res = i + 1;
        return;
    }
    typedef void result_type;
};

static void test_packaged_task_core() { // test core of packaged_task
    STD packaged_task<Func_type> pt0;
    CHECK_INT(pt0.valid(), false);
    bool caught = false;
    try { // try to get future from default-constructed object
        (void) pt0.get_future();
    } catch (const STD future_error& exc) { // should get here, no state
        CHECK_INT(exc.code().value() == nst.value(), true);
        caught = true;
    } catch (...) { // shouldn't get here
    }
    CHECK_INT(caught, true);

    caught = false;
    try { // try to call function on default-constructed object
        pt0(3);
    } catch (const STD future_error& exc) { // should get here, no state
        CHECK_INT(exc.code().value() == nst.value(), true);
        caught = true;
    } catch (...) { // shouldn't get here
    }
    CHECK_INT(caught, true);

    STD packaged_task<Func_type> pt1(func);
    CHECK_INT(pt1.valid(), true);
    caught = false;
    try { // call function object
        STD future<int> f1 = pt1.get_future();
        pt1(3);
        CHECK_INT(f1.get(), 2);
    } catch (...) { // shouldn't get here
        caught = true;
    }
    CHECK_INT(caught, false);

    caught = false;
    try { // try to get future a second time
        (void) pt1.get_future();
    } catch (const STD future_error& exc) { // should get here, future already retrieved
        CHECK_INT(exc.code().value() == rtr.value(), true);
        caught = true;
    } catch (...) { // shouldn't get here
    }
    CHECK_INT(caught, true);

    caught = false;
    try { // try to call function again
        pt1(3);
    } catch (const STD future_error& exc) { // should get here, future already satisfied
        CHECK_INT(exc.code().value() == sat.value(), true);
        caught = true;
    } catch (...) { // shouldn't get here
    }
    CHECK_INT(caught, true);

    pt1.reset();
    CHECK_INT(pt1.valid(), true);

    caught = false;
    try { // try to call after reset
        STD future<int> f1 = pt1.get_future();
        CHECK_INT(f1.valid(), true);
        pt1(3);
        CHECK_INT(f1.get(), 2);
    } catch (...) { // shouldn't get here
        caught = true;
    }
    CHECK_INT(caught, false);

    fnobj fn;
    STD packaged_task<Func_type> pt2(fn);
    CHECK_INT(pt2.valid(), true);
    caught = false;
    try { // call function object
        STD future<int> f2 = pt2.get_future();
        pt2(3);
        CHECK_INT(f2.get(), 4);
    } catch (...) { // shouldn't get here
        caught = true;
    }
    CHECK_INT(caught, false);

    caught = false;
    try {
        STD packaged_task<Func_type> pt3;
        pt3(3);
    } catch (...) { // should get here, shared state abandoned
        caught = true;
    }
    CHECK_INT(caught, true);
}

static void test_packaged_task_core_r() { // test core of packaged_task for function returning reference
    STD packaged_task<Funcr_type> pt0;
    CHECK_INT(pt0.valid(), false);
    bool caught = false;
    try { // try to get future from default-constructed object
        (void) pt0.get_future();
    } catch (const STD future_error& exc) { // should get here, no state
        CHECK_INT(exc.code().value() == nst.value(), true);
        caught = true;
    } catch (...) { // shouldn't get here
    }
    CHECK_INT(caught, true);

    caught = false;
    try { // try to call function on default-constructed object
        pt0(3);
    } catch (const STD future_error& exc) { // should get here, no state
        CHECK_INT(exc.code().value() == nst.value(), true);
        caught = true;
    } catch (...) { // shouldn't get here
    }
    CHECK_INT(caught, true);

    STD packaged_task<Funcr_type> pt1(funcr);
    CHECK_INT(pt1.valid(), true);
    caught = false;
    try { // call function object
        STD future<int&> f1 = pt1.get_future();
        func_res            = 0;
        pt1(3);
        CHECK_INT(f1.get(), 2);
        CHECK_INT(func_res, 2);
    } catch (...) { // shouldn't get here
        caught = true;
    }
    CHECK_INT(caught, false);

    caught = false;
    try { // try to get future a second time
        (void) pt1.get_future();
    } catch (const STD future_error& exc) { // should get here, future already retrieved
        CHECK_INT(exc.code().value() == rtr.value(), true);
        caught = true;
    } catch (...) { // shouldn't get here
    }
    CHECK_INT(caught, true);

    caught = false;
    try { // try to call function again
        pt1(3);
    } catch (const STD future_error& exc) { // should get here, future already satisfied
        CHECK_INT(exc.code().value() == sat.value(), true);
        caught = true;
    } catch (...) { // shouldn't get here
    }
    CHECK_INT(caught, true);

    pt1.reset();
    CHECK_INT(pt1.valid(), true);

    caught = false;
    try { // try to call after reset
        STD future<int&> f1 = pt1.get_future();
        CHECK_INT(f1.valid(), true);
        func_res = 0;
        pt1(3);
        CHECK_INT(f1.get(), 2);
        CHECK_INT(func_res, 2);
    } catch (...) { // shouldn't get here
        caught = true;
    }
    CHECK_INT(caught, false);

    fnobjr fn;
    STD packaged_task<Funcr_type> pt2(fn);
    CHECK_INT(pt2.valid(), true);
    caught = false;
    try { // call function object
        STD future<int&> f2 = pt2.get_future();
        func_res            = 0;
        pt2(3);
        CHECK_INT(f2.get(), 4);
        CHECK_INT(func_res, 4);
    } catch (...) { // shouldn't get here
        caught = true;
    }
    CHECK_INT(caught, false);

    caught = false;
    try {
        STD packaged_task<Funcr_type> pt3;
        pt3(3);
    } catch (...) { // should get here, shared state abandoned
        caught = true;
    }
    CHECK_INT(caught, true);
}

static void test_packaged_task_core_v() { // test core of packaged_task for function returning void
    STD packaged_task<Funcv_type> pt0;
    CHECK_INT(pt0.valid(), false);
    bool caught = false;
    try { // try to get future from default-constructed object
        (void) pt0.get_future();
    } catch (const STD future_error& exc) { // should get here, no state
        CHECK_INT(exc.code().value() == nst.value(), true);
        caught = true;
    } catch (...) { // shouldn't get here
    }
    CHECK_INT(caught, true);

    caught = false;
    try { // try to call function on default-constructed object
        pt0(3);
    } catch (const STD future_error& exc) { // should get here, no state
        CHECK_INT(exc.code().value() == nst.value(), true);
        caught = true;
    } catch (...) { // shouldn't get here
    }
    CHECK_INT(caught, true);

    STD packaged_task<Funcv_type> pt1(funcv);
    CHECK_INT(pt1.valid(), true);
    caught = false;
    try { // call function object
        STD future<void> f1 = pt1.get_future();
        func_res            = 0;
        pt1(3);
        f1.get();
        CHECK_INT(func_res, 2);
    } catch (...) { // shouldn't get here
        caught = true;
    }
    CHECK_INT(caught, false);

    caught = false;
    try { // try to get future a second time
        (void) pt1.get_future();
    } catch (const STD future_error& exc) { // should get here, future already retrieved
        CHECK_INT(exc.code().value() == rtr.value(), true);
        caught = true;
    } catch (...) { // shouldn't get here
    }
    CHECK_INT(caught, true);

    caught = false;
    try { // try to call function again
        pt1(3);
    } catch (const STD future_error& exc) { // should get here, future already satisfied
        CHECK_INT(exc.code().value() == sat.value(), true);
        caught = true;
    } catch (...) { // shouldn't get here
    }
    CHECK_INT(caught, true);

    pt1.reset();
    CHECK_INT(pt1.valid(), true);

    caught = false;
    try { // try to call after reset
        STD future<void> f1 = pt1.get_future();
        CHECK_INT(f1.valid(), true);
        func_res = 0;
        pt1(3);
        f1.get();
        CHECK_INT(func_res, 2);
    } catch (...) { // shouldn't get here
        caught = true;
    }
    CHECK_INT(caught, false);

    fnobjv fn;
    STD packaged_task<Funcv_type> pt2(fn);
    CHECK_INT(pt2.valid(), true);
    caught = false;
    try { // call function object
        STD future<void> f2 = pt2.get_future();
        func_res            = 0;
        pt2(3);
        f2.get();
        CHECK_INT(func_res, 4);
    } catch (...) { // shouldn't get here
        caught = true;
    }
    CHECK_INT(caught, false);

    caught = false;
    try {
        STD packaged_task<Funcv_type> pt3;
        pt3(3);
    } catch (...) { // should get here, shared state abandoned
        caught = true;
    }
    CHECK_INT(caught, true);

    caught = false;
    STD packaged_task<Funcv_type> pt3(fn);
    try { // try to call after reset, shared state must be abandoned
          // by reset
        STD future<void> f1 = pt3.get_future();
        CHECK_INT(f1.valid(), true);
        pt3.reset();
        f1.get();
    } catch (const STD future_error& exc) { // should get here, shared state abandoned
        CHECK_INT(exc.code().value() == brk.value(), true);
        caught = true;
    }
    CHECK_INT(caught, true);

    caught = false;
    try { // try to call after reset, shared state must be abandoned
          // by packaged_task destructor
        STD future<void> f1;
        {
            STD packaged_task<Funcv_type> pt4(fn);
            f1 = pt4.get_future();
            CHECK_INT(f1.valid(), true);
        }
        f1.get();
    } catch (const STD future_error& exc) { // should get here, shared state abandoned
        CHECK_INT(exc.code().value() == brk.value(), true);
        caught = true;
    }
    CHECK_INT(caught, true);
}

static void test_packaged_task_allocator() { // test packaged_task with allocator
#if _HAS_FUNCTION_ALLOCATOR_SUPPORT
    typedef STD uses_allocator<STD packaged_task<Func_type>, STD allocator<Func_type>> packaged_task_uses_allocator;
    CHECK_INT(packaged_task_uses_allocator::value, true);
#endif // _HAS_FUNCTION_ALLOCATOR_SUPPORT
}

static void test_packaged_task() { // do basic tests for packaged_task
    test_packaged_task_core();
    test_packaged_task_core_r();
    test_packaged_task_core_v();
    test_packaged_task_allocator();
}

static void test_async0() { // test function template async with function returning value
    fnobj fn;
    STD future<int> f0 = STD async(STD launch::async, fn, 1);
    f0.wait();
    CHECK_INT(f0.get(), 2);
    STD future<int> f1 = STD async(STD launch::deferred, fn, 3);
    f1.wait();
    CHECK_INT(f1.get(), 4);
    STD future<int> f2 = STD async(STD launch::async | STD launch::deferred, fn, 5);
    f2.wait();
    CHECK_INT(f2.get(), 6);
    STD future<int> f3 = STD async(STD launch::deferred, fn, 7);
    f3.wait();
    CHECK_INT(f3.get(), 8);
    STD future<int> f4 = STD async(fn, 9);
    f4.wait();
    CHECK_INT(f4.get(), 10);
}

static void test_asyncr() { // test function template async with function returning reference
    fnobjr fn;
    func_res            = 0;
    STD future<int&> f0 = STD async(STD launch::async, fn, 1);
    f0.wait();
    CHECK_INT(f0.get(), 2);
    CHECK_INT(func_res, 2);
    STD future<int&> f1 = STD async(STD launch::deferred, fn, 3);
    f1.wait();
    CHECK_INT(f1.get(), 4);
    CHECK_INT(func_res, 4);
    STD future<int&> f2 = STD async(STD launch::async | STD launch::deferred, fn, 5);
    f2.wait();
    CHECK_INT(f2.get(), 6);
    CHECK_INT(func_res, 6);
    STD future<int&> f3 = STD async(STD launch::deferred, fn, 7);
    f3.wait();
    CHECK_INT(f3.get(), 8);
    CHECK_INT(func_res, 8);
    STD future<int&> f4 = STD async(fn, 9);
    f4.wait();
    CHECK_INT(f4.get(), 10);
    CHECK_INT(func_res, 10);
}

static void test_asyncv() { // test function template async with function returning void
    fnobjv fn;
    func_res            = 0;
    STD future<void> f0 = STD async(STD launch::async, fn, 1);
    f0.wait();
    f0.get();
    CHECK_INT(func_res, 2);
    STD future<void> f1 = STD async(STD launch::deferred, fn, 3);
    f1.wait();
    f1.get();
    CHECK_INT(func_res, 4);
    STD future<void> f2 = STD async(STD launch::async | STD launch::deferred, fn, 5);
    f2.wait();
    f2.get();
    CHECK_INT(func_res, 6);
    STD future<void> f3 = STD async(STD launch::deferred, fn, 7);
    f3.wait();
    f3.get();
    CHECK_INT(func_res, 8);
    STD future<void> f4 = STD async(fn, 9);
    f4.wait();
    f4.get();
    CHECK_INT(func_res, 10);
}

static void test_asyncdtor() { // test that futures created with std::async
                               // and std::launch::async policy block in the dtor
    int n = 0;
    {
        STD future<void> f1 = STD async(STD launch::async, [&n] {
            STD this_thread::sleep_for(STD chrono::milliseconds(10));
            n = 1;
        });
    }
    CHECK_INT(n, 1);
}

static void test_async() { // test function async
    test_async0();
    test_asyncr();
    test_asyncv();
    test_asyncdtor();
}

void test_main() { // test header <future>
    test_enums();
    test_error_handling();
    test_future_error();
    test_promise();
    test_future();
    test_shared_future();
    test_packaged_task();
    test_async();
}
