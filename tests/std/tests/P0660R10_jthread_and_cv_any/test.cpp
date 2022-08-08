// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <mutex>
#include <thread>
#include <type_traits>
#include <utility>

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

#ifdef __clang__
#pragma clang diagnostic ignored "-Wself-move"
#endif // __clang__

using namespace std;

STATIC_ASSERT(is_same_v<thread::id, jthread::id>);
STATIC_ASSERT(is_same_v<thread::native_handle_type, jthread::native_handle_type>);

int main() {
    // dtor tested in lots of places here so no explicit tests for it

    { // default ctor
        jthread default_constructed;
        assert(default_constructed.get_id() == thread::id{});
        assert(!default_constructed.get_stop_source().stop_possible());
    }

    { // initializing ctor, traditional functor
        jthread worker{[] {}};
        assert(worker.get_id() != thread::id{});
        assert(worker.joinable());
        assert(worker.get_stop_source().stop_possible());
    }

    { // also make sure that we don't delegate to std::thread's constructor which would try to move assign over the
      // std::thread inside jthread rather than passing it to the functor
        jthread worker{[](thread t) { t.join(); }, thread{[] {}}};
        assert(worker.get_id() != thread::id{});
        assert(worker.joinable());
        assert(worker.get_stop_source().stop_possible());
    }

    { // initializing ctor, token functor
        bool called = false;
        struct overload_detector {
            bool* p_called;
            void operator()(stop_token, int i) const {
                assert(i == 1729);
                *p_called = true;
            }
            void operator()(int) const {
                assert(false);
            }
        };

        {
            jthread worker{overload_detector{&called}, 1729};
            (void) worker;
        }

        assert(called);
    }

    { // move ctor
        jthread worker{[] {}};
        auto worker_source = worker.get_stop_source();
        {
            jthread moved{move(worker)};
            assert(moved.get_stop_source() == worker_source);
            assert(moved.joinable());
            assert(worker.get_stop_source() != worker_source);
            assert(!worker.joinable());
        }
    }

    { // move assign
        jthread worker_a{[] {}};
        auto source_a = worker_a.get_stop_source();
        jthread worker_b{[] {}};
        auto id_b     = worker_b.get_id();
        auto source_b = worker_b.get_stop_source();
        worker_a      = move(worker_b);
        assert(source_a.stop_requested());
        assert(id_b == worker_a.get_id());
        assert(!source_b.stop_requested());
        assert(worker_a.get_stop_source() == source_b);
        assert(!worker_b.joinable());
    }

    { // self move assign, as of N4861 specified to try to cancel and join [thread.jthread.cons]/13
        jthread worker{[] {}};
        auto source = worker.get_stop_source();
        worker      = move(worker);
        assert(!worker.joinable());
        assert(source.stop_requested());
    }

    { // swaps
        jthread worker_a{[] {}};
        auto id_a     = worker_a.get_id();
        auto source_a = worker_a.get_stop_source();
        auto token_a  = worker_a.get_stop_token();
        jthread worker_b{[] {}};
        auto id_b     = worker_b.get_id();
        auto source_b = worker_b.get_stop_source();
        auto token_b  = worker_b.get_stop_token();

        assert(id_a != id_b);
        assert(source_a != source_b);
        assert(token_a != token_b);

        worker_a.swap(worker_b);
        assert(worker_a.get_id() == id_b);
        assert(worker_a.get_stop_source() == source_b);
        assert(worker_b.get_id() == id_a);
        assert(worker_b.get_stop_source() == source_a);
        swap(worker_a, worker_b);
        assert(worker_a.get_id() == id_a);
        assert(worker_a.get_stop_source() == source_a);
        assert(worker_b.get_id() == id_b);
        assert(worker_b.get_stop_source() == source_b);
    }

    { // join
        jthread worker{[] {}};
        auto source = worker.get_stop_source();
        worker.join();
        assert(!worker.joinable());
        assert(worker.get_stop_source() == source);
        assert(!source.stop_requested());
        assert(source.stop_possible());
    }

    // TRANSITION, OS-11107628 "_Exit allows cleanup in other DLLs"
    // detach() is intentionally not tested

    // get_id, get_stop_source, get_stop_token tested above

    assert(jthread::hardware_concurrency() == thread::hardware_concurrency());

    { // first wait_until overload; without the cancellation this would deadlock
        jthread worker([](stop_token token) {
            mutex m;
            condition_variable_any cv;
            unique_lock lck{m};
            assert(cv.wait(lck, move(token), [] { return false; }) == false);
        });
    }

    static constexpr auto forever  = chrono::steady_clock::duration::max();
    static constexpr auto infinity = chrono::steady_clock::time_point::max();

    { // ditto without the cancellation this would deadlock
        jthread worker([](stop_token token) {
            mutex m;
            condition_variable_any cv;
            unique_lock lck{m};
            assert(cv.wait_until(lck, move(token), infinity, [] { return false; }) == false);
        });
    }

    { // ditto without the cancellation this would deadlock
        jthread worker([](stop_token token) {
            mutex m;
            condition_variable_any cv;
            unique_lock lck{m};
            assert(cv.wait_for(lck, move(token), forever, [] { return false; }) == false);
        });
    }

    // smoke test true-returning versions of the above
    {
        mutex m;
        condition_variable_any cv;
        bool b = false;
        jthread worker([&](stop_token token) {
            unique_lock lck{m};
            assert(cv.wait(lck, move(token), [] { return true; }) == true);
            assert(cv.wait(lck, move(token), [&] { return b; }) == true); // Intentionally uses moved-from token
        });

        {
            lock_guard lck{m};
            b = true;
        }

        cv.notify_all();
    }

    {
        mutex m;
        condition_variable_any cv;
        bool b = false;
        jthread worker([&](stop_token token) {
            unique_lock lck{m};
            assert(cv.wait_until(lck, move(token), infinity, [] { return true; }) == true);
            assert(cv.wait_until(lck, move(token), infinity, [&] { return b; })
                   == true); // Intentionally uses moved-from token
        });

        {
            lock_guard lck{m};
            b = true;
        }

        cv.notify_all();
    }

    {
        mutex m;
        condition_variable_any cv;
        bool b = false;
        jthread worker([&](stop_token token) {
            unique_lock lck{m};
            assert(cv.wait_for(lck, move(token), forever, [] { return true; }) == true);
            assert(cv.wait_for(lck, move(token), forever, [&] { return b; })
                   == true); // Intentionally uses moved-from token
        });

        {
            lock_guard lck{m};
            b = true;
        }

        cv.notify_all();
    }

    // smoke test a timeout case:
    {
        jthread worker([] {
            stop_source never_stopped;
            mutex m;
            condition_variable_any cv;
            unique_lock lck{m};
            auto started_at = chrono::steady_clock::now();
            assert(cv.wait_for(lck, never_stopped.get_token(), 100ms, [] { return false; }) == false);
            // not a timing assumption: the wait_for must wait at least that long
            assert(started_at + 100ms <= chrono::steady_clock::now());
        });
    }

    puts("pass");
}
