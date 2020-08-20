
#include <coroutine>
#include <exception>

struct task
{
    struct promise_type
    {
        int result;
        std::coroutine_handle<> prev;

        task get_return_object()
        {
            return {*this};
        }

        std::suspend_always initial_suspend() { return {}; }

        auto final_suspend() noexcept
        {
            struct Awaiter
            {
                bool await_ready() noexcept { return false; }
                void await_resume() noexcept {}
                std::coroutine_handle<> await_suspend(std::coroutine_handle<promise_type> h)
                {
                    // If there is no previous coroutine to resume we've reached the outermost coroutine.
                    // Return a noop coroutine to allow control to return back to the caller.
                    if (!h.promise().prev)
                        return std::noop_coroutine();
                    return h.promise().prev; // resume awaiting coroutine
                }
            };
            return Awaiter{};
        }
        void return_value(const int v) { result = v; }
        void unhandled_exception() noexcept { std::terminate(); }
    };

    bool await_ready() const noexcept { return false; }
    int await_resume() { return coro.promise().result; }
    auto await_suspend(std::coroutine_handle<> enclosing)
    {
        coro.promise().prev = enclosing;
        return coro; // resume ourselves.
    }

    task(task&& rhs) noexcept : coro(rhs.coro) { rhs.coro = nullptr; }
    task(task const&) = delete;
    task(promise_type& p) : coro(std::coroutine_handle<promise_type>::from_promise(p)) {}

    ~task()
    {
        if (coro)
            coro.destroy();
    }

    std::coroutine_handle<promise_type> coro;
};

task f(int n)
{
    if (n == 0)
        co_return 0;

    co_return 1 + co_await f(n - 1);
}

int main()
{
    task t = f(10);
    std::coroutine_handle<> h = t.coro;
    if (h != t.coro)
    {
        return 1;
    }
    if (!h || h.done())
    {
        return 1;
    }

    h();

    int val = t.coro.promise().result;

    if (val != 10)
    {
        return 1;
    }
}
