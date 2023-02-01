// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <execution>
#include <memory>
#include <numeric>
#include <vector>

#include <parallel_algorithms_utilities.hpp>

using namespace std;
using namespace std::execution;

class wrap_uchar {
private:
    unsigned char ch_ = 42;

public:
    bool is_expected() const noexcept {
        return ch_ == 42;
    }
};

static_assert(sizeof(wrap_uchar) == 1);

const auto expectation = [](const wrap_uchar& x) { return x.is_expected(); };

const auto expectation_zero = [](int n) { return n == 0; };

constexpr auto bad_uchar = static_cast<unsigned char>(0xcd);
constexpr auto bad_int   = static_cast<int>(0xdeadbeef);

struct resetting_guard {
    int* ptr_ = nullptr;

    resetting_guard() = default;

    ~resetting_guard() {
        if (ptr_) {
            *ptr_ = 0;
        }
    }

    resetting_guard(const resetting_guard&)            = delete;
    resetting_guard& operator=(const resetting_guard&) = delete;
};

template <class T>
struct deallocating_only_deleter {
    size_t count_ = 0;

    void operator()(T* ptr) const noexcept {
        allocator<T>{}.deallocate(ptr, count_);
    }
};

template <class T>
unique_ptr<T, deallocating_only_deleter<T>> make_constructed_nondestroying_buffer(size_t n) {
    if (n == 0) {
        return unique_ptr<T, deallocating_only_deleter<T>>{};
    }

    allocator<T> al;
    unique_ptr<T, deallocating_only_deleter<T>> up{al.allocate(n), deallocating_only_deleter<T>{n}};
    for (size_t i = 0; i != n; ++i) {
        allocator_traits<allocator<T>>::construct(al, up.get() + i);
    }

    return up;
}

template <class T>
unique_ptr<T, deallocating_only_deleter<T>> make_unconstructed_nondestroying_buffer(size_t n) {
    if (n == 0) {
        return unique_ptr<T, deallocating_only_deleter<T>>{};
    } else {
        return unique_ptr<T, deallocating_only_deleter<T>>{allocator<T>{}.allocate(n), deallocating_only_deleter<T>{n}};
    }
}

struct test_case_uninitialized_default_construct_parallel {
    template <class ExecutionPolicy>
    void operator()(const size_t testSize, const ExecutionPolicy& exec) {
        auto buffer         = make_unique<unsigned char[]>(testSize);
        const auto begin_it = reinterpret_cast<wrap_uchar*>(buffer.get());
        const auto end_it   = begin_it + testSize;

        fill_n(buffer.get(), testSize, bad_uchar);

        uninitialized_default_construct(exec, begin_it, end_it);
        assert(all_of(begin_it, end_it, expectation));
    }
};

struct test_case_uninitialized_default_construct_n_parallel {
    template <class ExecutionPolicy>
    void operator()(const size_t testSize, const ExecutionPolicy& exec) {
        auto buffer         = make_unique<unsigned char[]>(testSize);
        const auto begin_it = reinterpret_cast<wrap_uchar*>(buffer.get());
        const auto end_it   = begin_it + testSize;

        fill_n(buffer.get(), testSize, bad_uchar);

        const auto result_it = uninitialized_default_construct_n(exec, begin_it, testSize);
        assert(all_of(begin_it, end_it, expectation));
        assert(end_it == result_it);
    }
};

struct test_case_uninitialized_default_construct_trivial_parallel {
    template <class ExecutionPolicy>
    void operator()(const size_t testSize, const ExecutionPolicy& exec) {
        auto buffer         = make_unconstructed_nondestroying_buffer<unsigned char>(testSize);
        const auto begin_it = buffer.get();
        const auto end_it   = begin_it + testSize;

        uninitialized_default_construct(exec, begin_it, end_it);
    }
};

struct test_case_uninitialized_default_construct_n_trivial_parallel {
    template <class ExecutionPolicy>
    void operator()(const size_t testSize, const ExecutionPolicy& exec) {
        auto buffer         = make_unconstructed_nondestroying_buffer<unsigned char>(testSize);
        const auto begin_it = buffer.get();
        const auto end_it   = begin_it + testSize;

        const auto result_it = uninitialized_default_construct_n(exec, begin_it, testSize);
        assert(end_it == result_it);
    }
};

struct test_case_uninitialized_value_construct_parallel {
    template <class ExecutionPolicy>
    void operator()(const size_t testSize, const ExecutionPolicy& exec) {
        auto buffer         = make_unique<unsigned char[]>(testSize);
        const auto begin_it = reinterpret_cast<wrap_uchar*>(buffer.get());
        const auto end_it   = begin_it + testSize;

        fill_n(buffer.get(), testSize, bad_uchar);

        uninitialized_value_construct(exec, begin_it, end_it);
        assert(all_of(begin_it, end_it, expectation));
    }
};

struct test_case_uninitialized_value_construct_n_parallel {
    template <class ExecutionPolicy>
    void operator()(const size_t testSize, const ExecutionPolicy& exec) {
        auto buffer         = make_unique<unsigned char[]>(testSize);
        const auto begin_it = reinterpret_cast<wrap_uchar*>(buffer.get());
        const auto end_it   = begin_it + testSize;

        fill_n(buffer.get(), testSize, bad_uchar);

        const auto result_it = uninitialized_value_construct_n(exec, begin_it, testSize);
        assert(all_of(begin_it, end_it, expectation));
        assert(end_it == result_it);
    }
};

struct test_case_uninitialized_value_construct_memset_parallel {
    template <class ExecutionPolicy>
    void operator()(const size_t testSize, const ExecutionPolicy& exec) {
        auto buffer         = make_unique<int[]>(testSize);
        const auto begin_it = buffer.get();
        const auto end_it   = begin_it + testSize;

        fill_n(begin_it, testSize, bad_int);

        uninitialized_value_construct(exec, begin_it, end_it);
        assert(all_of(begin_it, end_it, expectation_zero));
    }
};

struct test_case_uninitialized_value_construct_n_memset_parallel {
    template <class ExecutionPolicy>
    void operator()(const size_t testSize, const ExecutionPolicy& exec) {
        auto buffer         = make_unique<int[]>(testSize);
        const auto begin_it = buffer.get();
        const auto end_it   = begin_it + testSize;

        fill_n(begin_it, testSize, bad_int);

        const auto result_it = uninitialized_value_construct_n(exec, begin_it, testSize);
        assert(all_of(begin_it, end_it, expectation_zero));
        assert(end_it == result_it);
    }
};

struct test_case_uninitialized_value_construct_unwrap_parallel {
    template <class ExecutionPolicy>
    void operator()(const size_t testSize, const ExecutionPolicy& exec) {
        auto vec            = vector<int>(testSize, bad_int);
        const auto begin_it = vec.begin();
        const auto end_it   = vec.end();

        uninitialized_value_construct(exec, begin_it, end_it);
        assert(all_of(begin_it, end_it, expectation_zero));
    }
};

struct test_case_uninitialized_value_construct_n_unwrap_parallel {
    template <class ExecutionPolicy>
    void operator()(const size_t testSize, const ExecutionPolicy& exec) {
        auto vec            = vector<int>(testSize, bad_int);
        const auto begin_it = vec.begin();
        const auto end_it   = vec.end();

        const auto result_it = uninitialized_value_construct_n(exec, begin_it, testSize);
        assert(all_of(begin_it, end_it, expectation_zero));
        assert(end_it == result_it);
    }
};

struct test_case_destroy_parallel {
    template <class ExecutionPolicy>
    void operator()(const size_t testSize, const ExecutionPolicy& exec) {
        auto buffer         = make_unique<unsigned char[]>(testSize);
        const auto begin_it = reinterpret_cast<wrap_uchar*>(buffer.get());
        const auto end_it   = begin_it + testSize;

        fill_n(buffer.get(), testSize, bad_uchar);

        uninitialized_default_construct(exec, begin_it, end_it);
        destroy(exec, begin_it, end_it);
    }
};

struct test_case_destroy_n_parallel {
    template <class ExecutionPolicy>
    void operator()(const size_t testSize, const ExecutionPolicy& exec) {
        auto buffer         = make_unique<unsigned char[]>(testSize);
        const auto begin_it = reinterpret_cast<wrap_uchar*>(buffer.get());
        const auto end_it   = begin_it + testSize;

        fill_n(buffer.get(), testSize, bad_uchar);

        uninitialized_default_construct_n(begin_it, testSize);
        const auto result_it = destroy_n(exec, begin_it, testSize);
        assert(end_it == result_it);
    }
};

struct test_case_destroy_nontrivial_parallel {
    template <class ExecutionPolicy>
    void operator()(const size_t testSize, const ExecutionPolicy& exec) {
        auto buffer_to_destroy = make_constructed_nondestroying_buffer<resetting_guard>(testSize);
        const auto begin_it    = buffer_to_destroy.get();
        const auto end_it      = begin_it + testSize;

        auto buffer_to_clear           = make_unique<int[]>(testSize);
        const auto begin_it_validation = buffer_to_clear.get();
        const auto end_it_validation   = begin_it_validation + testSize;

        fill(begin_it_validation, end_it_validation, bad_int);
        auto it_guard = begin_it;
        for (auto it_int = begin_it_validation; it_int != end_it_validation; ++it_int) {
            it_guard->ptr_ = it_int;
            ++it_guard;
        }

        destroy(exec, begin_it, end_it);
        assert(all_of(begin_it_validation, end_it_validation, expectation_zero));
    }
};

struct test_case_destroy_n_nontrivial_parallel {
    template <class ExecutionPolicy>
    void operator()(const size_t testSize, const ExecutionPolicy& exec) {
        auto buffer_to_destroy = make_constructed_nondestroying_buffer<resetting_guard>(testSize);
        const auto begin_it    = buffer_to_destroy.get();
        const auto end_it      = begin_it + testSize;

        auto buffer_to_clear           = make_unique<int[]>(testSize);
        const auto begin_it_validation = buffer_to_clear.get();
        const auto end_it_validation   = begin_it_validation + testSize;

        fill_n(begin_it_validation, testSize, bad_int);
        auto it_guard = begin_it;
        for (auto it_int = begin_it_validation; it_int != end_it_validation; ++it_int) {
            it_guard->ptr_ = it_int;
            ++it_guard;
        }

        const auto result_it = destroy_n(exec, begin_it, testSize);
        assert(end_it == result_it);
        assert(all_of(begin_it_validation, end_it_validation, expectation_zero));
    }
};

struct test_case_uninitialized_copy_parallel {
    template <class ExecutionPolicy>
    void operator()(const size_t testSize, const ExecutionPolicy& exec) {
        auto buffer         = make_unique<int[]>(testSize);
        auto buffer2        = make_unique<int[]>(testSize);
        const auto begin_it = buffer.get();
        const auto end_it   = begin_it + testSize;

        fill_n(buffer2.get(), testSize, bad_int);
        iota(begin_it, end_it, 42);

        const auto begin_it2 = buffer2.get();
        const auto end_it2   = begin_it2 + testSize;

        uninitialized_copy(exec, begin_it, end_it, begin_it2);
        assert(equal(begin_it, end_it, begin_it2, end_it2));
    }
};

struct test_case_uninitialized_copy_n_parallel {
    template <class ExecutionPolicy>
    void operator()(const size_t testSize, const ExecutionPolicy& exec) {
        auto buffer         = make_unique<int[]>(testSize);
        auto buffer2        = make_unique<int[]>(testSize);
        const auto begin_it = buffer.get();
        const auto end_it   = begin_it + testSize;

        fill_n(buffer2.get(), testSize, bad_int);
        iota(begin_it, end_it, 42);

        const auto begin_it2 = buffer2.get();
        const auto end_it2   = begin_it2 + testSize;

        const auto result_it = uninitialized_copy_n(exec, begin_it, testSize, begin_it2);
        assert(equal(begin_it, end_it, begin_it2, end_it2));
        assert(end_it2 == result_it);
    }
};

struct test_case_uninitialized_move_parallel {
    template <class ExecutionPolicy>
    void operator()(const size_t testSize, const ExecutionPolicy& exec) {
        auto buffer         = make_unique<int[]>(testSize);
        auto buffer2        = make_unique<int[]>(testSize);
        const auto begin_it = buffer.get();
        const auto end_it   = begin_it + testSize;

        fill_n(buffer2.get(), testSize, bad_int);
        iota(begin_it, end_it, 42);

        const auto begin_it2 = buffer2.get();
        const auto end_it2   = begin_it2 + testSize;

        uninitialized_move(exec, begin_it, end_it, begin_it2);
        assert(equal(begin_it, end_it, begin_it2, end_it2));
    }
};

struct test_case_uninitialized_move_n_parallel {
    template <class ExecutionPolicy>
    void operator()(const size_t testSize, const ExecutionPolicy& exec) {
        auto buffer         = make_unique<int[]>(testSize);
        auto buffer2        = make_unique<int[]>(testSize);
        const auto begin_it = buffer.get();
        const auto end_it   = begin_it + testSize;

        fill_n(buffer2.get(), testSize, bad_int);
        iota(begin_it, end_it, 42);

        const auto begin_it2 = buffer2.get();
        const auto end_it2   = begin_it2 + testSize;

        const auto result_pair = uninitialized_move_n(exec, begin_it, testSize, begin_it2);
        assert(equal(begin_it, end_it, begin_it2, end_it2));
        assert(end_it == result_pair.first && end_it2 == result_pair.second);
    }
};

struct test_case_uninitialized_fill_parallel {
    template <class ExecutionPolicy>
    void operator()(const size_t testSize, const ExecutionPolicy& exec) {
        auto buffer         = make_unique<int[]>(testSize);
        const auto begin_it = buffer.get();
        const auto end_it   = begin_it + testSize;

        fill(begin_it, end_it, bad_int);

        uninitialized_fill(exec, begin_it, end_it, 42);
        assert(all_of(begin_it, end_it, [](int n) { return n == 42; }));
    }
};

struct test_case_uninitialized_fill_n_parallel {
    template <class ExecutionPolicy>
    void operator()(const size_t testSize, const ExecutionPolicy& exec) {
        auto buffer         = make_unique<int[]>(testSize);
        const auto begin_it = buffer.get();
        const auto end_it   = begin_it + testSize;

        fill_n(begin_it, testSize, bad_int);

        const auto result_it = uninitialized_fill_n(exec, begin_it, testSize, 42);
        assert(all_of(begin_it, end_it, [](int n) { return n == 42; }));
        assert(end_it == result_it);
    }
};

int main() {
#ifndef _M_CEE // TRANSITION, VSO-1664463
    parallel_test_case(test_case_uninitialized_default_construct_parallel{}, par);
    parallel_test_case(test_case_uninitialized_default_construct_n_parallel{}, par);
    parallel_test_case(test_case_uninitialized_default_construct_trivial_parallel{}, par);
    parallel_test_case(test_case_uninitialized_default_construct_n_trivial_parallel{}, par);
    parallel_test_case(test_case_uninitialized_value_construct_parallel{}, par);
    parallel_test_case(test_case_uninitialized_value_construct_n_parallel{}, par);
    parallel_test_case(test_case_uninitialized_value_construct_memset_parallel{}, par);
    parallel_test_case(test_case_uninitialized_value_construct_n_memset_parallel{}, par);
    parallel_test_case(test_case_uninitialized_value_construct_unwrap_parallel{}, par);
    parallel_test_case(test_case_uninitialized_value_construct_n_unwrap_parallel{}, par);
    parallel_test_case(test_case_destroy_parallel{}, par);
    parallel_test_case(test_case_destroy_n_parallel{}, par);
    parallel_test_case(test_case_destroy_nontrivial_parallel{}, par);
    parallel_test_case(test_case_destroy_n_nontrivial_parallel{}, par);

    // currently not parallelized
    parallel_test_case(test_case_uninitialized_copy_parallel{}, par);
    parallel_test_case(test_case_uninitialized_copy_n_parallel{}, par);
    parallel_test_case(test_case_uninitialized_move_parallel{}, par);
    parallel_test_case(test_case_uninitialized_move_n_parallel{}, par);
    parallel_test_case(test_case_uninitialized_fill_parallel{}, par);
    parallel_test_case(test_case_uninitialized_fill_n_parallel{}, par);
#if _HAS_CXX20
    parallel_test_case(test_case_uninitialized_default_construct_parallel{}, unseq);
    parallel_test_case(test_case_uninitialized_default_construct_n_parallel{}, unseq);
    parallel_test_case(test_case_uninitialized_default_construct_trivial_parallel{}, unseq);
    parallel_test_case(test_case_uninitialized_default_construct_n_trivial_parallel{}, unseq);
    parallel_test_case(test_case_uninitialized_value_construct_parallel{}, unseq);
    parallel_test_case(test_case_uninitialized_value_construct_n_parallel{}, unseq);
    parallel_test_case(test_case_uninitialized_value_construct_memset_parallel{}, unseq);
    parallel_test_case(test_case_uninitialized_value_construct_n_memset_parallel{}, unseq);
    parallel_test_case(test_case_uninitialized_value_construct_unwrap_parallel{}, unseq);
    parallel_test_case(test_case_uninitialized_value_construct_n_unwrap_parallel{}, unseq);
    parallel_test_case(test_case_destroy_parallel{}, unseq);
    parallel_test_case(test_case_destroy_n_parallel{}, unseq);
    parallel_test_case(test_case_destroy_nontrivial_parallel{}, unseq);
    parallel_test_case(test_case_destroy_n_nontrivial_parallel{}, unseq);

    // currently not parallelized
    parallel_test_case(test_case_uninitialized_copy_parallel{}, unseq);
    parallel_test_case(test_case_uninitialized_copy_n_parallel{}, unseq);
    parallel_test_case(test_case_uninitialized_move_parallel{}, unseq);
    parallel_test_case(test_case_uninitialized_move_n_parallel{}, unseq);
    parallel_test_case(test_case_uninitialized_fill_parallel{}, unseq);
    parallel_test_case(test_case_uninitialized_fill_n_parallel{}, unseq);
#endif // _HAS_CXX20
#endif // _M_CEE
}
