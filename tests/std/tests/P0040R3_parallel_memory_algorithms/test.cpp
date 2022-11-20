// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <atomic>
#include <cassert>
#include <execution>
#include <iterator>
#include <memory>
#include <numeric>

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

struct test_case_uninitialized_default_construct_parallel {
    template <typename ExecutionPolicy>
    void operator()(const size_t testSize, const ExecutionPolicy& exec) {
        auto buffer         = make_unique<unsigned char[]>(testSize);
        const auto begin_it = reinterpret_cast<wrap_uchar*>(buffer.get());
        const auto end_it   = begin_it + testSize;
        uninitialized_default_construct(exec, begin_it, end_it);
        assert(all_of(begin_it, end_it, expectation));
    }
};

struct test_case_uninitialized_default_construct_n_parallel {
    template <typename ExecutionPolicy>
    void operator()(const size_t testSize, const ExecutionPolicy& exec) {
        auto buffer          = make_unique<unsigned char[]>(testSize);
        const auto begin_it  = reinterpret_cast<wrap_uchar*>(buffer.get());
        const auto end_it    = begin_it + testSize;
        const auto result_it = uninitialized_default_construct_n(exec, begin_it, testSize);
        assert(all_of(begin_it, end_it, expectation));
        assert(end_it == result_it);
    }
};

struct test_case_uninitialized_value_construct_parallel {
    template <typename ExecutionPolicy>
    void operator()(const size_t testSize, const ExecutionPolicy& exec) {
        auto buffer         = make_unique<unsigned char[]>(testSize);
        const auto begin_it = reinterpret_cast<wrap_uchar*>(buffer.get());
        const auto end_it   = begin_it + testSize;
        uninitialized_value_construct(exec, begin_it, end_it);
        assert(all_of(begin_it, end_it, expectation));
    }
};

struct test_case_uninitialized_value_construct_n_parallel {
    template <typename ExecutionPolicy>
    void operator()(const size_t testSize, const ExecutionPolicy& exec) {
        auto buffer          = make_unique<unsigned char[]>(testSize);
        const auto begin_it  = reinterpret_cast<wrap_uchar*>(buffer.get());
        const auto end_it    = begin_it + testSize;
        const auto result_it = uninitialized_value_construct_n(exec, begin_it, testSize);
        assert(all_of(begin_it, end_it, expectation));
        assert(end_it == result_it);
    }
};

struct test_case_uninitialized_value_construct_memset_parallel {
    template <typename ExecutionPolicy>
    void operator()(const size_t testSize, const ExecutionPolicy& exec) {
        auto buffer         = unique_ptr<int>(new int[testSize]);
        const auto begin_it = buffer.get();
        const auto end_it   = begin_it + testSize;
        uninitialized_value_construct(exec, begin_it, end_it);
        assert(all_of(begin_it, end_it, expectation_zero));
    }
};

struct test_case_uninitialized_value_construct_n_memset_parallel {
    template <typename ExecutionPolicy>
    void operator()(const size_t testSize, const ExecutionPolicy& exec) {
        auto buffer          = unique_ptr<int>(new int[testSize]);
        const auto begin_it  = buffer.get();
        const auto end_it    = begin_it + testSize;
        const auto result_it = uninitialized_value_construct_n(exec, begin_it, testSize);
        assert(all_of(begin_it, end_it, expectation_zero));
        assert(end_it == result_it);
    }
};

struct test_case_destroy_parallel {
    template <typename ExecutionPolicy>
    void operator()(const size_t testSize, const ExecutionPolicy& exec) {
        auto buffer         = make_unique<unsigned char[]>(testSize);
        const auto begin_it = reinterpret_cast<wrap_uchar*>(buffer.get());
        const auto end_it   = begin_it + testSize;

        uninitialized_default_construct(exec, begin_it, end_it);
        destroy(exec, begin_it, end_it);
    }
};

struct test_case_destroy_n_parallel {
    template <typename ExecutionPolicy>
    void operator()(const size_t testSize, const ExecutionPolicy& exec) {
        auto buffer         = make_unique<unsigned char[]>(testSize);
        const auto begin_it = reinterpret_cast<wrap_uchar*>(buffer.get());
        const auto end_it   = begin_it + testSize;

        uninitialized_default_construct_n(begin_it, testSize);
        const auto result_it = destroy_n(exec, begin_it, testSize);
        assert(end_it == result_it);
    }
};

struct test_case_uninitialized_copy_parallel {
    template <typename ExecutionPolicy>
    void operator()(const size_t testSize, const ExecutionPolicy& exec) {
        auto buffer         = make_unique<int[]>(testSize);
        auto buffer2        = make_unique<int[]>(testSize);
        const auto begin_it = buffer.get();
        const auto end_it   = begin_it + testSize;

        iota(begin_it, end_it, 42);

        const auto begin_it2 = buffer2.get();
        const auto end_it2   = begin_it2 + testSize;

        uninitialized_copy(exec, begin_it, end_it, begin_it2);
        assert(equal(begin_it, end_it, begin_it2, end_it2));
    }
};

struct test_case_uninitialized_copy_n_parallel {
    template <typename ExecutionPolicy>
    void operator()(const size_t testSize, const ExecutionPolicy& exec) {
        auto buffer         = make_unique<int[]>(testSize);
        auto buffer2        = make_unique<int[]>(testSize);
        const auto begin_it = buffer.get();
        const auto end_it   = begin_it + testSize;

        iota(begin_it, end_it, 42);
        
        const auto begin_it2 = buffer2.get();
        const auto end_it2   = begin_it2 + testSize;

        const auto result_it = uninitialized_copy_n(exec, begin_it, testSize, begin_it2);
        assert(equal(begin_it, end_it, begin_it2, end_it2));
        assert(end_it2 == result_it);
    }
};

struct test_case_uninitialized_move_parallel {
    template <typename ExecutionPolicy>
    void operator()(const size_t testSize, const ExecutionPolicy& exec) {
        auto buffer         = make_unique<int[]>(testSize);
        auto buffer2        = make_unique<int[]>(testSize);
        const auto begin_it = buffer.get();
        const auto end_it   = begin_it + testSize;

        iota(begin_it, end_it, 42);

        const auto begin_it2 = buffer2.get();
        const auto end_it2   = begin_it2 + testSize;

        uninitialized_move(exec, begin_it, end_it, begin_it2);
        assert(equal(begin_it, end_it, begin_it2, end_it2));
    }
};

struct test_case_uninitialized_move_n_parallel {
    template <typename ExecutionPolicy>
    void operator()(const size_t testSize, const ExecutionPolicy& exec) {
        auto buffer         = make_unique<int[]>(testSize);
        auto buffer2        = make_unique<int[]>(testSize);
        const auto begin_it = buffer.get();
        const auto end_it   = begin_it + testSize;

        iota(begin_it, end_it, 42);

        const auto begin_it2 = buffer2.get();
        const auto end_it2   = begin_it2 + testSize;

        const auto result_pair = uninitialized_move_n(exec, begin_it, testSize, begin_it2);
        assert(equal(begin_it, end_it, begin_it2, end_it2));
        assert(end_it == result_pair.first && end_it2 == result_pair.second);
    }
};

struct test_case_uninitialized_fill_parallel {
    template <typename ExecutionPolicy>
    void operator()(const size_t testSize, const ExecutionPolicy& exec) {
        auto buffer         = make_unique<int[]>(testSize);
        const auto begin_it = buffer.get();
        const auto end_it   = begin_it + testSize;

        uninitialized_fill(exec, begin_it, end_it, 42);
        assert(all_of(begin_it, end_it, [](int n) { return n == 42; }));
    }
};

struct test_case_uninitialized_fill_n_parallel {
    template <typename ExecutionPolicy>
    void operator()(const size_t testSize, const ExecutionPolicy& exec) {
        auto buffer         = make_unique<int[]>(testSize);
        const auto begin_it = buffer.get();
        const auto end_it   = begin_it + testSize;

        const auto result_it = uninitialized_fill_n(exec, begin_it, testSize, 42);
        assert(all_of(begin_it, end_it, [](int n) { return n == 42; }));
        assert(end_it == result_it);
    }
};

int main() {
    parallel_test_case(test_case_uninitialized_default_construct_parallel{}, par);
    parallel_test_case(test_case_uninitialized_default_construct_n_parallel{}, par);
    parallel_test_case(test_case_uninitialized_value_construct_parallel{}, par);
    parallel_test_case(test_case_uninitialized_value_construct_n_parallel{}, par);
    parallel_test_case(test_case_uninitialized_value_construct_memset_parallel{}, par);
    parallel_test_case(test_case_uninitialized_value_construct_n_memset_parallel{}, par);
    parallel_test_case(test_case_destroy_parallel{}, par);
    parallel_test_case(test_case_destroy_n_parallel{}, par);

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
    parallel_test_case(test_case_uninitialized_value_construct_parallel{}, unseq);
    parallel_test_case(test_case_uninitialized_value_construct_n_parallel{}, unseq);
    parallel_test_case(test_case_uninitialized_value_construct_memset_parallel{}, unseq);
    parallel_test_case(test_case_uninitialized_value_construct_n_memset_parallel{}, unseq);
    parallel_test_case(test_case_destroy_parallel{}, unseq);
    parallel_test_case(test_case_destroy_n_parallel{}, unseq);

    // currently not parallelized
    parallel_test_case(test_case_uninitialized_copy_parallel{}, unseq);
    parallel_test_case(test_case_uninitialized_copy_n_parallel{}, unseq);
    parallel_test_case(test_case_uninitialized_move_parallel{}, unseq);
    parallel_test_case(test_case_uninitialized_move_n_parallel{}, unseq);
    parallel_test_case(test_case_uninitialized_fill_parallel{}, unseq);
    parallel_test_case(test_case_uninitialized_fill_n_parallel{}, unseq);
#endif // _HAS_CXX20
}
