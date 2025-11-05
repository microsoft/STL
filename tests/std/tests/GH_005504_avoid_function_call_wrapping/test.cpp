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

template<class Wrapper, class Callable>
void test_plain_call(int expected_copies) {
    Wrapper fn{Callable{}};
    assert(fn(copy_counter{}) == expected_copies);
}

template <class OuterWrapper, class InnerWrapper, class Callable>
void test_wrapped_call(int expected_copies) {
    InnerWrapper inner{Callable{}};
    OuterWrapper outer{std::move(inner)};
    assert(!inner);
    assert(outer(copy_counter{}) == expected_copies);
}

template <class Wrapper>
void test_plain_null(bool throws) {
    Wrapper fn{};
    assert(!fn);

    if (throws) {
        try {
            fn(copy_counter{});
            abort(); // should not reach
        } catch (bad_function_call&){
        }
    }
}

template <class OuterWrapper, class InnerWrapper>
void test_wrapped_call(bool outer_is_null, bool outer_throws) {
    InnerWrapper inner{};
    OuterWrapper outer{std::move(inner)};
    assert(!inner);
    assert(!outer == outer_is_null);
    
    if (outer_throws) {
        try {
            outer(copy_counter{});
            abort(); // should not reach
        } catch (bad_function_call&) {
        }
    } else {
        // UB that in our implementation tries to call doom function; we do not test that
    }
}

int main() {
    // Plain calls
    test_plain_call<function<function_type>, small_callable>(0);
    test_plain_call<function<function_type>, large_callable>(0);
    test_plain_call<move_only_function<function_type>, small_callable>(0);
    test_plain_call<move_only_function<function_type>, large_callable>(0);

    // Moves to the same
    test_wrapped_call<function<function_type>, function<function_type>, small_callable>(0);
    test_wrapped_call<function<function_type>, function<function_type>, large_callable>(0);
    test_wrapped_call<move_only_function<function_type>, move_only_function<function_type>, small_callable>(0);
    test_wrapped_call<move_only_function<function_type>, move_only_function<function_type>, large_callable>(0);

    // Moves from function to move_only_function
    test_wrapped_call<move_only_function<function_type>, function<function_type>, small_callable>(0);
    test_wrapped_call<move_only_function<function_type>, function<function_type>, large_callable>(0);

    // nulls
    test_plain_null<function<function_type>>(true);
    test_plain_null<move_only_function<function_type>>(false);

    // wrapped nulls
    test_wrapped_call<function<function_type>, function<function_type>>(true, true);
    test_wrapped_call<move_only_function<function_type>, move_only_function<function_type>>(true, false);
    test_wrapped_call<move_only_function<function_type>, function<function_type>>(false, true);
}
