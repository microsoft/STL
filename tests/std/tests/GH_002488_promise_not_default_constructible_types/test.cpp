// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <atomic>
#include <cassert>
#include <exception>
#include <future>
#include <system_error>
#include <thread>

static std::atomic<int> has_default_objects{0};
static std::atomic<int> no_default_objects{0};
static std::atomic<int> no_default_or_assign_objects{0};

struct has_default {
    has_default() : x(0xbad) {
        ++has_default_objects;
    }
    explicit has_default(int n) : x(n) {
        ++has_default_objects;
    }
    has_default(const has_default& v) : x(v.x) {
        ++has_default_objects;
    }

    ~has_default() {
        --has_default_objects;
    }

    int x;
};

struct no_default {
    no_default() = delete;
    explicit no_default(int n) : x(n) {
        ++no_default_objects;
    }
    no_default(const no_default& v) : x(v.x) {
        ++no_default_objects;
    }

    ~no_default() {
        --no_default_objects;
    }

    int x;
};

struct no_default_or_assign {
    no_default_or_assign() = delete;
    explicit no_default_or_assign(int n) : x(n) {
        ++no_default_or_assign_objects;
    }
    no_default_or_assign(const no_default_or_assign& v) : x(v.x) {
        ++no_default_or_assign_objects;
    }

    void operator=(const no_default_or_assign&) = delete;

    ~no_default_or_assign() {
        --no_default_or_assign_objects;
    }

    int x;
};

template <class F>
void assert_throws_future_error(F f, std::error_code expected_code) {
    try {
        f();
    } catch (const std::future_error& e) {
        assert(e.code() == expected_code);
        return;
    } catch (...) {
    }
    assert(false);
}

template <class T>
void run_tests() {
    using Promise = std::promise<T>;
    using Future  = std::future<T>;

    {
        Promise p;
        p.set_value(T(4));
        assert(p.get_future().get().x == 4);
    }

    {
        Promise p;
        Future f = p.get_future();
        T v(10);
        p.set_value(v);
        assert(f.get().x == 10);
        assert_throws_future_error([&] { p.set_value(v); }, std::future_errc::promise_already_satisfied);
        assert_throws_future_error([&] { f.get(); }, std::future_errc::no_state);
        assert_throws_future_error([&] { p.get_future().get(); }, std::future_errc::future_already_retrieved);
    }

    {
        Promise p;
        Future f = p.get_future();
        p.set_exception(std::make_exception_ptr(5));
        try {
            f.get();
            assert(false);
        } catch (int i) {
            assert(i == 5);
        } catch (...) {
            assert(false);
        }
    }

    {
        Promise p;
        Future f = p.get_future();
        p.set_exception(std::make_exception_ptr(3));
        assert_throws_future_error([&] { p.set_value(T(2)); }, std::future_errc::promise_already_satisfied);
        try {
            f.get();
            assert(false);
        } catch (int i) {
            assert(i == 3);
        } catch (...) {
            assert(false);
        }
    }

    {
        Promise p;
        Future f = p.get_future();
        std::atomic<int> failures{0};
        int succeeded    = -1;
        auto make_thread = [&](int n) {
            return std::thread([&, n] {
                try {
                    p.set_value(T(n));
                } catch (std::future_error) {
                    ++failures;
                    return;
                }
                succeeded = n;
            });
        };
        std::thread threads[]{make_thread(0), make_thread(1), make_thread(2), make_thread(3), make_thread(4),
            make_thread(5), make_thread(6), make_thread(7)};

        for (auto& t : threads) {
            t.join();
        }

        assert(failures == 7);
        assert(succeeded != -1 && f.get().x == succeeded);
    }

    {
        (void) std::async(std::launch::async, [] { return T(16); });
        (void) std::async(std::launch::async, [] {
            const T x(40);
            return x;
        });

        Future f = std::async(std::launch::async, [] { return T(23); });
        assert(f.get().x == 23);
    }

    {
        std::packaged_task<T()> pt([] { return T(7); });
        Future f = pt.get_future();
        pt();

        assert(f.get().x == 7);
    }
}

int main() {
    run_tests<has_default>();
    run_tests<no_default>();
    run_tests<no_default_or_assign>();
    assert(has_default_objects == 0);
    assert(no_default_objects == 0);
    assert(no_default_or_assign_objects == 0);
}
