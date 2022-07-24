// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <atomic>
#include <cassert>
#include <chrono>
#include <cstdio>
#include <optional>
#include <stop_token>
#include <thread>
#include <utility>

#include <new_counter.hpp>

using namespace std;
using namespace std_testing;

#ifdef __clang__
#pragma clang diagnostic ignored "-Wself-move"
#endif // __clang__

struct throwing_construction_functor {
    throwing_construction_functor(int x) {
        throw x;
    }

    void operator()() const {
        assert(false);
    }
};

struct call_counting_functor {
    atomic<int>* state;

    call_counting_functor(atomic<int>* state_) : state(state_) {}

    call_counting_functor(const call_counting_functor&)            = delete;
    call_counting_functor& operator=(const call_counting_functor&) = delete;

    void operator()() && {
        ++*state;
    }
};

struct cb_destroying_functor {
    optional<stop_callback<cb_destroying_functor>>& owner;
    cb_destroying_functor(optional<stop_callback<cb_destroying_functor>>& owner_) : owner(owner_) {}

    cb_destroying_functor(const cb_destroying_functor&)            = delete;
    cb_destroying_functor& operator=(const cb_destroying_functor&) = delete;

    void operator()() && {
        owner.reset();
    }
};

int main() noexcept {
    reset_new_counters(0);
    { // all the following must not allocate, and must work with a nostopstate source; in rough synopsis order
        stop_token token;
        stop_token token_copy{token};
        stop_token token_moved{move(token)};
        token_copy  = token;
        token_moved = move(token);
        token.swap(token_copy);
        assert(!token.stop_requested());
        assert(!token.stop_possible());
        assert(token == token_copy);
        swap(token, token_copy);

        stop_source source{nostopstate};
        stop_source copied_source{source};
        stop_source moved_source{move(source)};
        copied_source = source;
        moved_source  = move(source);
        copied_source.swap(source);

        assert(!source.get_token().stop_possible());
        assert(!source.get_token().stop_requested());
        assert(!source.stop_possible());
        assert(!source.stop_requested());
        assert(!source.request_stop());

        assert(source == copied_source);
        assert(source == moved_source);

        swap(source, copied_source);

        stop_callback cb{token, [] { assert(false); }};
        stop_callback cb_moved{move(token), [] { assert(false); }};
    }

    // normal reference counted things state management; in rough synopsis order
    reset_new_counters(2);
    { // stop_source
        stop_source empty{nostopstate};

        // default ctor
        stop_source source_a;
        assert(source_a.stop_possible());
        assert(!source_a.stop_requested());
        stop_source source_b;
        assert(source_b.stop_possible());
        assert(!source_b.stop_requested());
        assert(source_a != empty);
        assert(source_a != source_b);

        // copy ctor
        stop_source copied_source{source_a};
        assert(copied_source == source_a);
        source_a.swap(source_b);
        assert(copied_source == source_b);
        swap(source_a, source_b);
        assert(copied_source == source_a);

        // move ctor
        stop_source moved_source{move(source_a)};
        assert(!source_a.stop_possible());
        assert(empty == source_a);
        assert(moved_source != source_a);
        moved_source = move(moved_source);
        swap(moved_source, source_a);

        // copy assignment
        copied_source = source_b;
        assert(copied_source == source_b);

        // move assignment
        moved_source = move(source_a);
        assert(!source_a.stop_possible());
        assert(moved_source.stop_possible());

        // swap member
        moved_source.swap(source_a);
        assert(source_a.stop_possible());
        assert(!moved_source.stop_possible());

        // get_token tested with tokens below
        // stop_possible tested above
        // stop_requested tested below
        assert(!empty.request_stop());
        assert(source_a.request_stop());
        assert(source_a.stop_requested());
        assert(!source_a.request_stop());
        assert(source_a.stop_requested());

        assert(!source_b.stop_requested());
        assert(!copied_source.stop_requested());
        assert(copied_source.request_stop());
        assert(source_b.stop_requested());
        assert(copied_source.stop_requested());
        assert(!source_b.request_stop());
    }

    reset_new_counters(2);
    { // stop_token
        stop_source source_a;
        stop_token token_a = source_a.get_token();
        assert(token_a.stop_possible());
        assert(!token_a.stop_requested());

        stop_source source_b;
        stop_token token_b = source_b.get_token();
        assert(token_a != token_b);

        stop_token empty;

        // default ctor tested above in the no-alloc block

        // copy ctor
        stop_token copied_token{token_a};
        assert(copied_token == token_a);

        // move ctor
        stop_token moved_token{move(token_a)};
        assert(moved_token == copied_token);
        assert(moved_token != token_a);
        assert(!token_a.stop_possible());
        assert(!token_a.stop_requested());
        moved_token.swap(token_a);

        // copy assign
        copied_token = token_b;
        assert(copied_token == token_b);

        // move assign
        moved_token = move(token_b);
        assert(moved_token == copied_token);
        moved_token = move(moved_token);
        assert(moved_token == copied_token);
        assert(moved_token != token_a);
        assert(!token_b.stop_possible());
        assert(!token_b.stop_requested());
        swap(token_b, moved_token);

        // stop_possible tested above and 1 special case below

        // stop_requested
        assert(!copied_token.stop_requested());
        assert(source_b.request_stop());
        assert(!token_a.stop_requested());
        assert(token_b.stop_requested());
        assert(copied_token.stop_requested());

        // equals and swap tested above
    }

    // the stop_possible special cases
    reset_new_counters(1);
    { // all sources are gone
        stop_token token;
        {
            stop_source source;
            token = source.get_token();
            assert(token.stop_possible());
            assert(!token.stop_requested());
        } // destroy source

        assert(!token.stop_possible());
        assert(!token.stop_requested());
        stop_callback cb{token, [] { assert(false); }};
        (void) cb;
    }

    reset_new_counters(1);
    { // all sources are gone but stop happened first
        stop_token token;
        {
            stop_source source;
            token = source.get_token();
            assert(token.stop_possible());
            assert(!token.stop_requested());
            assert(source.request_stop());
            assert(token.stop_possible());
            assert(token.stop_requested());
        } // destroy source

        assert(token.stop_possible());
        assert(token.stop_requested());
    }

    // empty assign special cases
    reset_new_counters(1);
    {
        stop_source source;
        stop_source empty{nostopstate};
        source = empty; // lvalue
        assert(!source.stop_possible());
    }

    reset_new_counters(1);
    {
        stop_source source;
        source = stop_source{nostopstate}; // rvalue
        assert(!source.stop_possible());
    }

    reset_new_counters(1);
    {
        stop_source source;
        auto token = source.get_token();
        stop_token empty;
        token = empty; // lvalue
        assert(!token.stop_possible());
    }

    reset_new_counters(1);
    {
        stop_source source;
        auto token = source.get_token();
        token      = stop_token{}; // rvalue
        assert(!token.stop_possible());
    }

    // callback calling in the ctor
    reset_new_counters(1);
    {
        atomic<int> calls{0};
        stop_source source;
        source.request_stop();
        assert(calls.load() == 0);
        stop_callback<call_counting_functor> cb{source.get_token(), &calls};
        (void) cb;
        assert(calls.load() == 1);
    }

    reset_new_counters(1);
    {
        atomic<int> calls{0};
        stop_token token;

        {
            stop_source source;
            token = source.get_token();
            source.request_stop();
        } // destroy source

        assert(calls.load() == 0);
        stop_callback<call_counting_functor> cb{token, &calls};
        (void) cb;
        assert(calls.load() == 1);
    }

    // callback calling on cancel
    reset_new_counters(1);
    {
        atomic<int> calls{0};
        stop_source source;
        assert(calls.load() == 0);
        stop_callback<call_counting_functor> cb{source.get_token(), &calls};
        assert(calls.load() == 0);
        source.request_stop();
        assert(calls.load() == 1);
    }

    // if the callback is executing on the current thread it does not block for the callback to finish executing
    reset_new_counters(1);
    {
        stop_source source;
        auto token = source.get_token();
        optional<stop_callback<cb_destroying_functor>> cb;
        cb.emplace(token, cb);
        source.request_stop(); // if we don't do what the standard says, this will deadlock
    }

    // if the callback is executing on another thread it blocks for the callback to finish executing
    reset_new_counters(2); // nonstandard assumption that our std::thread allocates exactly once
    {
        static constexpr chrono::milliseconds callback_wait_length = 5s;
        static constexpr chrono::milliseconds request_wait_length  = 500ms;
        stop_source source;
        atomic<bool> block_request_stop{false};
        // block_destroy makes it more likely that the timing assumption above is correct because the timer doesn't
        // start until we know the worker thread is actively running trying to request_stop
        atomic<bool> block_destroy{false};
        thread worker{[&] {
            // run the callbacks in the worker thread
            block_request_stop.wait(false);
            block_destroy.store(true);
            block_destroy.notify_one();
            assert("request_wait_length TIMING ASSUMPTION" && source.request_stop());
        }};


        auto worker_id = worker.get_id();
        chrono::steady_clock::time_point started_at;
        {
            // timing assumption that the main thread will try to destroy cb within request_wait_length
            stop_callback cb{source.get_token(), [&] {
                                 this_thread::sleep_for(callback_wait_length);
                                 assert("request_wait_length TIMING ASSUMPTION" && this_thread::get_id() == worker_id);
                             }};
            started_at = chrono::steady_clock::now();
            block_request_stop.store(true);
            block_request_stop.notify_one();
            block_destroy.wait(false); // wait for the other thread to start stopping
            // timing assumption that worker enters request_stop before we try to destroy cb here;
            // if that assumption is wrong then we merely don't test the case in which we're interested (because cb will
            // run on this thread so we won't have to block for destruction)
            this_thread::sleep_for(request_wait_length);
            assert("request_wait_length TIMING ASSUMPTION" && !source.request_stop());
        } // destroy cb

        worker.join();

        // not a timing assumption: we must have waited at least as long as the sleep_for in the cancellation callback
        // (that's the point of this test)
        auto stopped_at = chrono::steady_clock::now();
        assert(started_at + callback_wait_length <= stopped_at);
    }

    // more than one callback in the list and the first callback unregisters one of the others
    // (this tests edge cases in the callback linked list management)
    for (int idx = 0; idx < 5; ++idx) {
        reset_new_counters(1);
        stop_source source;
        auto token = source.get_token();
        optional<stop_callback<cb_destroying_functor>> cbs[5];
        cbs[0].emplace(token, cbs[idx]);
        cbs[1].emplace(token, cbs[1]);
        cbs[2].emplace(token, cbs[2]);
        cbs[3].emplace(token, cbs[3]);
        cbs[4].emplace(token, cbs[4]);
        cbs[2].reset();
        source.request_stop();
    }

    // exception safety cases
    reset_new_counters(0);
    try {
        stop_source source;
        (void) source;
        assert(false);
    } catch (const bad_alloc&) {
        // expected
    }

    reset_new_counters(1);
    try {
        stop_source source;
        stop_callback<throwing_construction_functor> cb{source.get_token(), 42};
    } catch (int i) {
        assert(i == 42);
    }

    reset_new_counters(1);
    try {
        stop_source source;
        auto token_lvalue = source.get_token();
        stop_callback<throwing_construction_functor> cb{token_lvalue, 43};
    } catch (int i) {
        assert(i == 43);
    }

    reset_new_counters(0);

    puts("pass");
}
