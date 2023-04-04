// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <chrono>
#include <cstdlib>
#include <functional>
#include <future>
#include <memory>
#include <string>
#include <system_error>
#include <thread>
#include <type_traits>
#include <utility>

using namespace std;
using namespace std::placeholders;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

// DevDiv-235721 "<future>: async() and packaged_task don't compile for void and T& return types"

void test_DevDiv_235721() {
    auto void_lambda = []() {};

    int i           = 1729;
    auto ref_lambda = [&]() -> int& { return i; };

    {
        future<void> f = async(launch::deferred, void_lambda);
        f.get();
    }

    {
        future<int&> f = async(launch::deferred, ref_lambda);
        f.get();
    }

    {
        packaged_task<void()> pt(void_lambda);
        future<void> f = pt.get_future();
        pt();
        f.get();
    }

    {
        packaged_task<int&()> pt(ref_lambda);
        future<int&> f = pt.get_future();
        pt();
        f.get();
    }
}


// DevDiv-586551 "<future>: future_errc message() and what() don't work"

void test_message(const future_errc fe, const string& s) {
    assert(make_error_code(fe).message() == s);
    assert(future_error(make_error_code(fe)).what() == s); // nonstandard

    // also P0517R0 "Constructing future_error From future_errc"
    assert(future_error(fe).what() == s); // C++17
}

void test_DevDiv_586551() {
    test_message(future_errc::broken_promise, "broken promise");
    test_message(future_errc::future_already_retrieved, "future already retrieved");
    test_message(future_errc::promise_already_satisfied, "promise already satisfied");
    test_message(future_errc::no_state, "no state");
}


// DevDiv-725337 "<future>: std::packaged_task<T> where T is void or a reference class are not movable"

void test_DevDiv_725337() {
    auto void_lambda = []() {};

    int i           = 1729;
    auto ref_lambda = [&]() -> int& { return i; };

    {
        packaged_task<int()> pt1([] { return 19937; });
        future<int> f = pt1.get_future();
        packaged_task<int()> pt2(move(pt1));
        packaged_task<int()> pt3;
        pt3 = move(pt2);
        assert(f.wait_for(0s) == future_status::timeout);
        pt3();
        assert(f.wait_for(0s) == future_status::ready);
        assert(f.get() == 19937);
    }

    {
        packaged_task<int&()> pt1(ref_lambda);
        future<int&> f = pt1.get_future();
        packaged_task<int&()> pt2(move(pt1));
        packaged_task<int&()> pt3;
        pt3 = move(pt2);
        assert(f.wait_for(0s) == future_status::timeout);
        pt3();
        assert(f.wait_for(0s) == future_status::ready);
        assert(&f.get() == &i);
    }

    {
        packaged_task<void()> pt1(void_lambda);
        future<void> f = pt1.get_future();
        packaged_task<void()> pt2(move(pt1));
        packaged_task<void()> pt3;
        pt3 = move(pt2);
        assert(f.wait_for(0s) == future_status::timeout);
        pt3();
        assert(f.wait_for(0s) == future_status::ready);
        f.get();
    }
}


// VSO-152487 "<future>: Bug in std::promise destructor for void and ref specializations"
template <typename T>
void assert_promise_broken(future<T>& f) noexcept {
    try {
        (void) f.get();
        abort();
    } catch (const future_error& fe) {
        assert(fe.code() == make_error_condition(future_errc::broken_promise));
    }
}

template <typename T>
void test_broken_promise() {
    future<T> f;

    { // promise destructor
        promise<T> p;
        f = p.get_future();
    }

    assert_promise_broken(f);

    { // promise move assignment operator
        promise<T> p;
        f = p.get_future();
        p = promise<T>{};
        assert_promise_broken(f);
    }
}

void test_VSO_152487() {
    test_broken_promise<int>();
    test_broken_promise<int&>();
    test_broken_promise<void>();
}

// VSO-97632 "<future>: async signature [libs-conformance]"
void void_function() {}
int int_function(double) {
    return 0;
}
STATIC_ASSERT(is_same_v<decltype(async(void_function)), future<void>>);
STATIC_ASSERT(is_same_v<decltype(async(int_function, 3)), future<int>>);

// VSO-112570 "<future>: std::async (still?) does not accept move only types"
// VSO-111640 "<future>: std::async decaying(losing) rvalue reference [...]"
struct move_only_functor_type {
    int operator()(unique_ptr<int>&& ptr) && {
        return *ptr;
    }

    move_only_functor_type() = delete;
    move_only_functor_type(int, int) {}
    move_only_functor_type(const move_only_functor_type&)            = delete;
    move_only_functor_type(move_only_functor_type&&)                 = default;
    move_only_functor_type& operator=(const move_only_functor_type&) = delete;
    move_only_functor_type& operator=(move_only_functor_type&&)      = delete;
};

void test_VSO_112570() {
    auto defaultPolicy = async(move_only_functor_type{0, 0}, make_unique<int>(42));
    assert(defaultPolicy.get() == 42);

    auto asyncPolicy = async(launch::async, move_only_functor_type{0, 0}, make_unique<int>(1729));
    assert(asyncPolicy.get() == 1729);

    auto deferredPolicy = async(launch::deferred, move_only_functor_type{0, 0}, make_unique<int>(0xC0FFEE));
    assert(deferredPolicy.get() == 0xC0FFEE);
}

// VSO-115515 "<future>: Compilation errors within std::async when using placeholders as arguments"
struct placeholder_detect {
    int lastPlaceholder = 0;

    void detect_placeholder(decltype(_1)) {
        lastPlaceholder = 1;
    }

    void detect_placeholder(decltype(_2)) {
        lastPlaceholder = 2;
    }
};

void test_VSO_115515() {
    placeholder_detect pd;
    async([&](auto val) { pd.detect_placeholder(val); }, _1).wait();
    assert(pd.lastPlaceholder == 1);

    async([&](auto val) { pd.detect_placeholder(val); }, _2).wait();
    assert(pd.lastPlaceholder == 2);
}

// VSO-272761 "[Feedback] std::promise + move = incorrect broken_promise (VS2015 Update 2)"
void test_VSO_272761_void() {
    promise<void> p;
    future<void> f = p.get_future();

    thread t([p = move(p)]() mutable { p.set_value_at_thread_exit(); });

    f.wait();
    t.join();
}

void test_VSO_272761_int() {
    promise<int> p;
    future<int> f = p.get_future();

    thread t([p = move(p)]() mutable { p.set_value_at_thread_exit(42); });

    assert(f.get() == 42);
    t.join();
}

void test_VSO_272761_ref() {
    int a;
    promise<int&> p;
    future<int&> f = p.get_future();

    thread t([p = move(p), &a]() mutable { p.set_value_at_thread_exit(a); });

    assert(&f.get() == &a);
    t.join();
}

void test_VSO_272761() {
    test_VSO_272761_void();
    test_VSO_272761_int();
    test_VSO_272761_ref();
}

template <typename T>
void test_future_shared_future_noexcept_impl() {
    STATIC_ASSERT(is_nothrow_default_constructible_v<future<T>>);
    STATIC_ASSERT(is_nothrow_move_constructible_v<future<T>>);
    STATIC_ASSERT(is_nothrow_move_assignable_v<future<T>>);
    STATIC_ASSERT(is_nothrow_destructible_v<future<T>>);

    STATIC_ASSERT(is_nothrow_default_constructible_v<shared_future<T>>);
    STATIC_ASSERT(is_nothrow_move_constructible_v<shared_future<T>>);
    STATIC_ASSERT(is_nothrow_move_assignable_v<shared_future<T>>);
    STATIC_ASSERT(is_nothrow_destructible_v<shared_future<T>>);
}

// P0516R0 "Marking shared_future Copying As noexcept"
template <typename T>
void test_shared_future_noexcept_copy_impl() {
    STATIC_ASSERT(noexcept(declval<future<T>>().share()));
    STATIC_ASSERT(is_nothrow_copy_constructible_v<shared_future<T>>);
    STATIC_ASSERT(is_nothrow_copy_assignable_v<shared_future<T>>);
    shared_future<T> prototype;
    assert(!prototype.valid());
    auto copyCtord = prototype;
    assert(!copyCtord.valid());
    copyCtord = prototype;
    assert(!copyCtord.valid());
}

void test_future_shared_future_noexcept() {
    test_future_shared_future_noexcept_impl<int>();
    test_future_shared_future_noexcept_impl<int&>();
    test_future_shared_future_noexcept_impl<void>();

    test_shared_future_noexcept_copy_impl<int>();
    test_shared_future_noexcept_copy_impl<int&>();
    test_shared_future_noexcept_copy_impl<void>();
}

// Also test the exception specifications of move functions of promise and packaged_task
template <typename T>
void test_promise_noexcept_impl() {
    STATIC_ASSERT(is_nothrow_move_constructible_v<promise<T>>);
    STATIC_ASSERT(is_nothrow_move_assignable_v<promise<T>>);
    STATIC_ASSERT(is_nothrow_destructible_v<promise<T>>);
}

void test_promise_noexcept() {
    test_promise_noexcept_impl<int>();
    test_promise_noexcept_impl<int&>();
    test_promise_noexcept_impl<void>();
}

template <typename F>
void test_packaged_task_noexcept_impl() {
    STATIC_ASSERT(is_nothrow_default_constructible_v<packaged_task<F>>);
    STATIC_ASSERT(is_nothrow_move_constructible_v<packaged_task<F>>);
    STATIC_ASSERT(is_nothrow_move_assignable_v<packaged_task<F>>);
    STATIC_ASSERT(is_nothrow_destructible_v<packaged_task<F>>);
}

void test_packaged_task_noexcept() {
    test_packaged_task_noexcept_impl<int()>();
    test_packaged_task_noexcept_impl<int&()>();
    test_packaged_task_noexcept_impl<void()>();

    test_packaged_task_noexcept_impl<int(int)>();
    test_packaged_task_noexcept_impl<int&(int)>();
    test_packaged_task_noexcept_impl<void(int)>();
}

// Also test the non-constructibility of future from (future, {}) and (shared_future, {})
template <typename Void, typename T, typename... Args>
constexpr bool is_constructible_with_trailing_empty_brace_impl = false;

template <typename T, typename... Args>
constexpr bool
    is_constructible_with_trailing_empty_brace_impl<void_t<decltype(T(declval<Args>()..., {}))>, T, Args...> = true;

template <typename T, typename... Args>
constexpr bool is_constructible_with_trailing_empty_brace =
    is_constructible_with_trailing_empty_brace_impl<void, T, Args...>;

STATIC_ASSERT(is_constructible_with_trailing_empty_brace<pair<double*, int>, double*>); // verify a true case

template <typename T>
void test_no_implicit_brace_construction_impl() {
    STATIC_ASSERT(!is_constructible_with_trailing_empty_brace<future<T>, future<T>>);
    STATIC_ASSERT(!is_constructible_with_trailing_empty_brace<future<T>, const future<T>&>);
    STATIC_ASSERT(!is_constructible_with_trailing_empty_brace<future<T>, shared_future<T>>);
    STATIC_ASSERT(!is_constructible_with_trailing_empty_brace<future<T>, const shared_future<T>&>);
}

void test_no_implicit_brace_construction() {
    test_no_implicit_brace_construction_impl<int>();
    test_no_implicit_brace_construction_impl<int&>();
    test_no_implicit_brace_construction_impl<void>();
}

#ifndef _M_CEE // TRANSITION, VSO-1659511
struct use_async_in_a_global_tester {
    use_async_in_a_global_tester() {
        assert(async([] { return 42; }).get() == 42);
    }
    ~use_async_in_a_global_tester() {
        (void) async([] { return 1729; }).get();
    }
};

use_async_in_a_global_tester use_async_in_a_global_instance;
#endif // _M_CEE

int main() {
    test_DevDiv_235721();
    test_DevDiv_586551();
    test_DevDiv_725337();
    test_VSO_152487();
    test_VSO_112570();
    test_VSO_115515();
    test_VSO_272761();
    test_future_shared_future_noexcept();
    test_promise_noexcept();
    test_packaged_task_noexcept();
    test_no_implicit_brace_construction();
}
