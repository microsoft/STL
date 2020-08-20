#include <assert.h>
#include <coroutine>
#include <exception>
using namespace std;

struct Task {
    struct Promise {
        int result;
        coroutine_handle<> previous;

        Task get_return_object() {
            return {*this};
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

                coroutine_handle<> await_suspend(coroutine_handle<Promise> h) {
                    // If there is no previous coroutine to resume we've reached the outermost coroutine.
                    // Return a noop coroutine to allow control to return back to the caller.
                    if (!h.promise().previous) {
                        return noop_coroutine();
                    }

                    return h.promise().previous; // resume awaiting coroutine
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
        return coro; // resume ourselves.
    }

    Task(Task&& rhs) noexcept : coro(rhs.coro) {
        rhs.coro = nullptr;
    }

    Task(Promise& p) : coro(coroutine_handle<Promise>::from_promise(p)) {}

    ~Task() {
        if (coro) {
            coro.destroy();
        }
    }

    coroutine_handle<Promise> coro;
};

Task f(int n) {
    if (n == 0) {
        co_return 0;
    }

    co_return 1 + co_await f(n - 1);
}

int main() {
    Task t               = f(10);
    coroutine_handle<> h = t.coro;

    assert(h == t.coro);
    assert(h);
    assert(!h.done());

    h();

    int val = t.coro.promise().result;

    assert(val == 10);
}
