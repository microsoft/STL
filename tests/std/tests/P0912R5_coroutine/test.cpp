// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <version> // TRANSITION, P0912R5 Library Support For Coroutines
#if defined(__cpp_lib_coroutine) && __cpp_lib_coroutine >= 201902L // TRANSITION, P0912R5 Library Support For Coroutines

#include <assert.h>
#include <coroutine>
#include <exception>
using namespace std;

int g_tasks_destroyed{0};

struct Task {
    struct Promise {
        int result{-1000};
        coroutine_handle<> previous;

        Task get_return_object() {
            return Task{*this};
        }

        suspend_always initial_suspend() {
            return {};
        }

        auto final_suspend() noexcept {
            struct Awaiter {
                bool await_ready() noexcept {
                    return false;
                }

                void await_resume() noexcept {}

                coroutine_handle<> await_suspend(coroutine_handle<Promise> h) noexcept {
                    if (auto& pre = h.promise().previous; pre) {
                        return pre; // resume awaiting coroutine
                    }

                    // If there is no previous coroutine to resume, we've reached the outermost coroutine.
                    // Return a noop coroutine to allow control to return back to the caller.
                    return noop_coroutine();
                }
            };

            return Awaiter{};
        }

        void return_value(const int v) {
            result = v;
        }

        void unhandled_exception() noexcept {
            terminate();
        }
    };

    using promise_type = Promise;

    bool await_ready() const noexcept {
        return false;
    }

    int await_resume() {
        return coro.promise().result;
    }

    auto await_suspend(coroutine_handle<> enclosing) {
        coro.promise().previous = enclosing;
        return coro; // resume ourselves
    }

    Task(Task&& rhs) noexcept : coro(rhs.coro) {
        rhs.coro = nullptr;
    }

    explicit Task(Promise& p) : coro(coroutine_handle<Promise>::from_promise(p)) {}

    ~Task() {
        ++g_tasks_destroyed;

        if (coro) {
            coro.destroy();
        }
    }

    coroutine_handle<Promise> coro;
};

Task triangular_number(const int n) {
    if (n == 0) {
        co_return 0;
    }

    co_return n + co_await triangular_number(n - 1);
}

int main() {
    assert(g_tasks_destroyed == 0);

    {
        Task t               = triangular_number(10);
        coroutine_handle<> h = t.coro;

        assert(h == t.coro);
        assert(h);
        assert(!h.done());

        h();

        assert(h == t.coro);
        assert(h);
        assert(h.done());

        assert(g_tasks_destroyed == 10); // triangular_number() called for [0, 9]

        const int val = t.coro.promise().result;

        assert(val == 55);
    }

    assert(g_tasks_destroyed == 11); // triangular_number() called for [0, 10]
}

#else // ^^^ test <coroutine> ^^^ / vvv don't test <coroutine> vvv
int main() {}
#endif // TRANSITION, P0912R5 Library Support For Coroutines
