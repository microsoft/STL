// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <functional>
#include <intrin.h>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>


using namespace std;
using namespace std::placeholders;

long g_last_successful_line(0);

void verify(const bool b, const long line) {
    if (b) {
        _InterlockedExchange(&g_last_successful_line, line);
    } else {
        printf("Last successful line was %ld.\n", _InterlockedCompareExchange(&g_last_successful_line, 0, 0));
        printf("Failed on line %ld.\n", line);
        abort();
    }
}

#define VERIFY(B) verify(B, __LINE__)

struct WritePlaceholder {
    template <typename T>
    void operator()(const T&, int* const p) const {
        *p = is_placeholder_v<T>;
    }
};

void ThrowingFunc(int* const p) {
    ++*p;

    switch (*p) {
    case 1:
        throw overflow_error("one");
    case 2:
        throw underflow_error("two");
    default:
        break;
    }
}

void run_tests() {
    // Basic check.
    {
        once_flag flag;
        auto inc = [](int* const p) { ++*p; };
        int i    = 0;

        VERIFY(i == 0);
        call_once(flag, inc, &i);
        VERIFY(i == 1);
        call_once(flag, inc, &i);
        VERIFY(i == 1);
    }

    // call_once() should use INVOKE().
    {
        struct Meow {
            int x;
            int y;

            void write_sum(int* const p) const {
                *p = x + y;
            }
        };

        once_flag flag;
        Meow m = {1700, 29};
        int i  = 0;

        call_once(flag, &Meow::write_sum, &m, &i);
        VERIFY(i == 1729);
    }

    // call_once() should NOT use bind().
    {
        once_flag flag1;
        int i1 = -1;

        call_once(flag1, WritePlaceholder(), 3.14, &i1);
        VERIFY(i1 == 0);


        once_flag flag2;
        int i2 = -1;

        call_once(flag2, WritePlaceholder(), _5, &i2);
        VERIFY(i2 == 5);
    }

    // Test DevDiv-1086953 "Throwing exception from std::call_once does not allow other threads to enter".
    // EH, single-threaded.
    {
        once_flag flag;

        int i = 0;

        try {
            call_once(flag, ThrowingFunc, &i);
            VERIFY(false);
        } catch (const overflow_error& e) {
            VERIFY(i == 1);
            VERIFY(e.what() == "one"s);
        } catch (...) {
            VERIFY(false);
        }

        try {
            call_once(flag, ThrowingFunc, &i);
            VERIFY(false);
        } catch (const underflow_error& e) {
            VERIFY(i == 2);
            VERIFY(e.what() == "two"s);
        } catch (...) {
            VERIFY(false);
        }

        try {
            call_once(flag, ThrowingFunc, &i);
            VERIFY(i == 3);

            call_once(flag, ThrowingFunc, &i);
            VERIFY(i == 3);
        } catch (...) {
            VERIFY(false);
        }
    }

    // Test DevDiv-1086953 "Throwing exception from std::call_once does not allow other threads to enter".
    // EH, multi-threaded.
    {
        long ready = 0;
        once_flag flag;
        int i = 0;

        auto lambda = [&ready, &flag, &i] {
            while (!_InterlockedCompareExchange(&ready, 0, 0)) {
            }

            try {
                call_once(flag, ThrowingFunc, &i);
                VERIFY(i == 3);
            } catch (const overflow_error& e) {
                VERIFY(e.what() == "one"s);
            } catch (const underflow_error& e) {
                VERIFY(e.what() == "two"s);
            } catch (...) {
                VERIFY(false);
            }
        };

        vector<thread> v;

        // Want 5 threads: 2 exceptional, 1 returning, 2 passive.
        for (int n = 0; n < 5; ++n) {
            v.emplace_back(lambda);
        }

        _InterlockedExchange(&ready, 1);

        for (auto& t : v) {
            t.join();
        }

        VERIFY(i == 3);
    }

    // Test DevDiv-1092852 "concurrent std::call_once calls seem to be blocking somewhere on a shared variable".
    // Also test stateful callable objects.
    {
        long atom = 0;

        thread t1([&atom] {
            once_flag flag1;

            auto lambda1 = [&atom] {
                _InterlockedExchange(&atom, 100);
                while (_InterlockedCompareExchange(&atom, 111, 200) != 200) {
                }
            };

            call_once(flag1, lambda1);
        });

        thread t2([&atom] {
            once_flag flag2;

            auto lambda2 = [&atom] {
                while (_InterlockedCompareExchange(&atom, 200, 100) != 100) {
                }
                while (_InterlockedCompareExchange(&atom, 222, 111) != 111) {
                }
            };

            call_once(flag2, lambda2);
        });

        t1.join();
        t2.join();

        VERIFY(atom == 222);
    }

    // Verify that active executions don't occur simultaneously.
    {
        once_flag flag;
        long atom = 0;

        auto thread_lambda = [&flag, &atom] {
            try {
                call_once(flag, [&atom] {
                    VERIFY(_InterlockedIncrement(&atom) == 1);
                    this_thread::sleep_for(50ms);
                    VERIFY(_InterlockedDecrement(&atom) == 0);
                    throw runtime_error("woof");
                });
                VERIFY(false);
            } catch (const runtime_error& e) {
                VERIFY(e.what() == "woof"s);
            } catch (...) {
                VERIFY(false);
            }
        };

        vector<thread> v;

        for (int n = 0; n < 5; ++n) {
            v.emplace_back(thread_lambda);
        }

        for (auto& t : v) {
            t.join();
        }
    }

    // Test LWG-2442 "call_once() shouldn't DECAY_COPY()".
    {
        once_flag flag;
        int n       = 0;
        auto lambda = [](int& r) { r = 1729; };

        VERIFY(n == 0);
        call_once(flag, lambda, n);
        VERIFY(n == 1729);
    }
}

int main() {
    try {
        run_tests();
    } catch (const exception& e) {
        puts(e.what());
        VERIFY(false);
    }

    puts("Passed.");
}
