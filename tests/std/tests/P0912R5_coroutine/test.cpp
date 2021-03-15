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

void test_noop_handle() { // Validate noop_coroutine_handle
    const noop_coroutine_handle noop = noop_coroutine();
    static_assert(noexcept(noop_coroutine()));

    const coroutine_handle<> as_void = noop;
    static_assert(noexcept(static_cast<coroutine_handle<>>(noop_coroutine())));

    assert(noop);
    assert(as_void);
    static_assert(noexcept(static_cast<bool>(noop)));
    static_assert(noexcept(static_cast<bool>(as_void)));

    assert(!noop.done());
    assert(!as_void.done());
    static_assert(noexcept(noop.done()));
    static_assert(noexcept(as_void.done()));

    assert(noop);
    assert(as_void);
    noop();
    as_void();
    static_assert(noexcept(noop()));

    assert(noop);
    assert(as_void);
    noop.resume();
    as_void.resume();
    static_assert(noexcept(noop.resume()));

    assert(noop);
    assert(as_void);
    noop.destroy();
    as_void.destroy();
    static_assert(noexcept(noop.destroy()));

    assert(noop);
    assert(as_void);
    assert(&noop.promise() != nullptr);
    static_assert(noexcept(noop.promise()));

    assert(noop);
    assert(as_void);
    assert(noop.address() != nullptr);
    assert(noop.address() == as_void.address());
    static_assert(noexcept(noop.address()));
    static_assert(noexcept(as_void.address()));
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

    {
        // Also test GH-1422: hash<coroutine_handle<>>::operator() must be const
        const hash<coroutine_handle<>> h;
        (void) h(coroutine_handle<>{});
    }

    test_noop_handle();
}

#else // ^^^ test <coroutine> ^^^ / vvv don't test <coroutine> vvv
int main() {}
#endif // TRANSITION, P0912R5 Library Support For Coroutines
