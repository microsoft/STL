// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <functional>
#include <new>

using namespace std;

#pragma warning(disable : 4324) // 'large_callable': structure was padded due to alignment specifier
#pragma warning(disable : 28251) // Inconsistent annotation for 'new': this instance has no annotations.

int alloc_count   = 0;
int dealloc_count = 0;

void* operator new(size_t size) {
    if (size == 0) {
        size = 1;
    }

    ++alloc_count;
    void* result = malloc(size);
    if (!result) {
        throw std::bad_alloc{};
    }
    return result;
}

void operator delete(void* mem) noexcept {
    ++dealloc_count;
    free(mem);
}

void* operator new(size_t size, align_val_t al) {
    if (size == 0) {
        size = 1;
    }

    ++alloc_count;
    void* result = _aligned_malloc(size, static_cast<size_t>(al));
    if (!result) {
        throw std::bad_alloc{};
    }
    return result;
}

void operator delete(void* mem, align_val_t) noexcept {
    ++dealloc_count;
    _aligned_free(mem);
}


struct alloc_checker {
    explicit alloc_checker(int expected_delta_) : expected_delta(expected_delta_) {}
    alloc_checker(const alloc_checker&)            = delete;
    alloc_checker& operator=(const alloc_checker&) = delete;

    ~alloc_checker() {
        assert(alloc_count - before == expected_delta);
        assert(alloc_count == dealloc_count);
    }

    int expected_delta;
    int before = alloc_count;
};

struct copy_counter {
    copy_counter() = default;
    copy_counter(const copy_counter& other) : count(other.count + 1) {}

    int count = 0;
};

using fn_type = int(copy_counter);

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
        assert((reinterpret_cast<uintptr_t>(this) & 0x7f) == 0);
        assert(context == 1729);
        return counter.count;
    }
};

template <class Wrapper, class Callable>
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
        } catch (bad_function_call&) {
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
    alloc_checker(0), test_plain_call<function<fn_type>, small_callable>(0);
    alloc_checker(1), test_plain_call<function<fn_type>, large_callable>(0);
    alloc_checker(0), test_plain_call<move_only_function<fn_type>, small_callable>(0);
    alloc_checker(1), test_plain_call<move_only_function<fn_type>, large_callable>(0);

    // Moves to the same
    alloc_checker(0), test_wrapped_call<function<fn_type>, function<fn_type>, small_callable>(0);
    alloc_checker(1), test_wrapped_call<function<fn_type>, function<fn_type>, large_callable>(0);
    alloc_checker(0), test_wrapped_call<move_only_function<fn_type>, move_only_function<fn_type>, small_callable>(0);
    alloc_checker(1), test_wrapped_call<move_only_function<fn_type>, move_only_function<fn_type>, large_callable>(0);

    // Moves from function to move_only_function
#ifdef _WIN64
    alloc_checker(0),
#else
    alloc_checker(1),
#endif
        test_wrapped_call<move_only_function<fn_type>, function<fn_type>, small_callable>(0);
    alloc_checker(1), test_wrapped_call<move_only_function<fn_type>, function<fn_type>, large_callable>(0);

    // nulls
    alloc_checker(0), test_plain_null<function<fn_type>>(true);
    alloc_checker(0), test_plain_null<move_only_function<fn_type>>(false);

    // wrapped nulls
    alloc_checker(0), test_wrapped_call<function<fn_type>, function<fn_type>>(true, true);
    alloc_checker(0), test_wrapped_call<move_only_function<fn_type>, move_only_function<fn_type>>(true, false);
    alloc_checker(0), test_wrapped_call<move_only_function<fn_type>, function<fn_type>>(false, true);
}
