// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <functional>

#pragma warning(disable : 4324) // 'large_callable': structure was padded due to alignment specifier

using namespace std;

struct copy_counter {
    copy_counter() = default;
    copy_counter(const copy_counter& other) : count(other.count + 1) {}

    int count = 0;
};

using function_type = int(copy_counter);

struct small_callable {
    int context = 42;

    int operator()(const copy_counter& counter) {
        assert(context == 42);
        return counter.count;
    }
};

struct alignas(128) large_callable {
    int context = 1729;

    int operator()(const copy_counter& counter) {
        assert(context == 1729);
        return counter.count;
    }
};

int main() {
    // Plain calls
    {
        function<function_type> fn{small_callable{}};
        assert(fn(copy_counter{}) == 0);
    }
    {
        function<function_type> fn{large_callable{}};
        assert(fn(copy_counter{}) == 0);
    }
    {
        move_only_function<function_type> fn{small_callable{}};
        assert(fn(copy_counter{}) == 0);
    }
    {
        move_only_function<function_type> fn{large_callable{}};
        assert(fn(copy_counter{}) == 0);
    }

    // Moves to the same
    {
        function<function_type> fn{function<function_type>{small_callable{}}};
        assert(fn(copy_counter{}) == 0);
    }
    {
        function<function_type> fn{function<function_type>{large_callable{}}};
        assert(fn(copy_counter{}) == 0);
    }
    {
        move_only_function<function_type> fn{move_only_function<function_type>{small_callable{}}};
        assert(fn(copy_counter{}) == 0);
    }
    {
        move_only_function<function_type> fn{move_only_function<function_type>{large_callable{}}};
        assert(fn(copy_counter{}) == 0);
    }

    // Moves from function to move_only_function
    {
        move_only_function<function_type> fn{function<function_type>{small_callable{}}};
        assert(fn(copy_counter{}) == 0);
    }
    {
        move_only_function<function_type> fn{function<function_type>{large_callable{}}};
        assert(fn(copy_counter{}) == 0);
    }
}
