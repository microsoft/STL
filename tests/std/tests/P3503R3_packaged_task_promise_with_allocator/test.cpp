// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstddef>
#include <future>
#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

using namespace std;

ptrdiff_t global_allocation_count   = 0;
ptrdiff_t global_deallocation_count = 0;

template <class T>
struct global_counting_allocator {
    using value_type = T;

    global_counting_allocator() = default;
    template <class U>
    constexpr global_counting_allocator(const global_counting_allocator<U>&) noexcept {}

    T* allocate(size_t n) {
        auto p = allocator<T>{}.allocate(n);
        ++global_allocation_count;
        return p;
    }

    void deallocate(T* p, size_t n) {
        allocator<T>{}.deallocate(p, n);
        ++global_deallocation_count;
    }

    template <class U>
    friend constexpr bool operator==(global_counting_allocator, global_counting_allocator<U>) noexcept {
        return true;
    }
#if !_HAS_CXX20
    template <class U>
    friend constexpr bool operator!=(global_counting_allocator, global_counting_allocator<U>) noexcept {
        return false;
    }
#endif // !_HAS_CXX20
};

template <class T>
void test_implicit_conversion(T) noexcept;

template <class V, class T, class... Args>
constexpr bool is_copy_list_initializable_impl = false;
template <class T, class... Args>
constexpr bool
    is_copy_list_initializable_impl<decltype(test_implicit_conversion<T>({declval<Args>()...})), T, Args...> = true;

template <class T, class... Args>
constexpr bool is_copy_list_initializable = is_copy_list_initializable_impl<void, T, Args...>;

template <class R>
struct large_functor {
    unsigned char dummy_[64]{};

    template <class... Args>
    R operator()(Args&&...) const {
        return R{};
    }
};
template <class R>
struct large_functor<R&> {
    unsigned char dummy_[64]{};

    template <class... Args>
    R& operator()(Args&&...) const {
        static R r{};
        return r;
    }
};
template <>
struct large_functor<void> {
    unsigned char dummy_[64]{};

    template <class... Args>
    void operator()(Args&&...) const {}
};

STATIC_ASSERT(is_constructible_v<promise<int>>);
STATIC_ASSERT(is_constructible_v<promise<int>, const allocator_arg_t&, const allocator<int>&>);
STATIC_ASSERT(is_constructible_v<promise<int>, const allocator_arg_t&, const global_counting_allocator<int>&>);

STATIC_ASSERT(is_copy_list_initializable<promise<int>>);
STATIC_ASSERT(is_copy_list_initializable<promise<int>, const allocator_arg_t&, const allocator<int>&>);
STATIC_ASSERT(is_copy_list_initializable<promise<int>, const allocator_arg_t&, const global_counting_allocator<int>&>);

STATIC_ASSERT(is_constructible_v<packaged_task<int()>>);
STATIC_ASSERT(
    is_constructible_v<packaged_task<int()>, const allocator_arg_t&, const allocator<int>&, large_functor<int>>);
STATIC_ASSERT(is_constructible_v<packaged_task<int()>, const allocator_arg_t&, const global_counting_allocator<int>&,
    large_functor<int>>);

STATIC_ASSERT(is_copy_list_initializable<packaged_task<int()>>);
STATIC_ASSERT(!is_copy_list_initializable<packaged_task<int()>, const allocator_arg_t&, const allocator<int>&,
    large_functor<int>>);
STATIC_ASSERT(!is_copy_list_initializable<packaged_task<int()>, const allocator_arg_t&,
    const global_counting_allocator<int>&, large_functor<int>>);

STATIC_ASSERT(!uses_allocator_v<promise<int>, allocator<int>>);
STATIC_ASSERT(!uses_allocator_v<promise<int&>, allocator<int>>);
STATIC_ASSERT(!uses_allocator_v<promise<void>, allocator<int>>);
STATIC_ASSERT(!uses_allocator_v<promise<int>, global_counting_allocator<int>>);
STATIC_ASSERT(!uses_allocator_v<promise<int&>, global_counting_allocator<int>>);
STATIC_ASSERT(!uses_allocator_v<promise<void>, global_counting_allocator<int>>);

STATIC_ASSERT(!uses_allocator_v<packaged_task<int()>, allocator<int>>);
STATIC_ASSERT(!uses_allocator_v<packaged_task<int&()>, allocator<int>>);
STATIC_ASSERT(!uses_allocator_v<packaged_task<void()>, allocator<int>>);
STATIC_ASSERT(!uses_allocator_v<packaged_task<int()>, global_counting_allocator<int>>);
STATIC_ASSERT(!uses_allocator_v<packaged_task<int&()>, global_counting_allocator<int>>);
STATIC_ASSERT(!uses_allocator_v<packaged_task<void()>, global_counting_allocator<int>>);

template <class T>
void test_construction_promise_in_tuple() {
    {
        tuple<promise<T>> t{allocator_arg, allocator<int>{}};
        (void) t;
    }
    {
        const auto old_alloc_cnt   = global_allocation_count;
        const auto old_dealloc_cnt = global_deallocation_count;
        {
            tuple<promise<T>> t{allocator_arg, global_counting_allocator<int>{}};
            (void) t;

            assert(global_allocation_count == old_alloc_cnt);
            assert(global_deallocation_count == old_dealloc_cnt);
        }
        assert(global_allocation_count == old_alloc_cnt);
        assert(global_deallocation_count == old_dealloc_cnt);
    }
    {
        tuple<promise<T>> t{allocator_arg, allocator<int>{}, promise<T>{}};
        (void) t;
    }
    {
        const auto old_alloc_cnt   = global_allocation_count;
        const auto old_dealloc_cnt = global_deallocation_count;
        {
            tuple<promise<T>> t{allocator_arg, global_counting_allocator<int>{}, promise<T>{}};
            (void) t;

            assert(global_allocation_count == old_alloc_cnt);
            assert(global_deallocation_count == old_dealloc_cnt);
        }
        assert(global_allocation_count == old_alloc_cnt);
        assert(global_deallocation_count == old_dealloc_cnt);
    }
}

template <class F>
void test_construction_packaged_task_in_tuple() {
    {
        tuple<packaged_task<F>> t{allocator_arg, allocator<int>{}};
        (void) t;
    }
    {
        const auto old_alloc_cnt   = global_allocation_count;
        const auto old_dealloc_cnt = global_deallocation_count;
        {
            tuple<packaged_task<F>> t{allocator_arg, global_counting_allocator<int>{}};
            (void) t;

            assert(global_allocation_count == old_alloc_cnt);
            assert(global_deallocation_count == old_dealloc_cnt);
        }
        assert(global_allocation_count == old_alloc_cnt);
        assert(global_deallocation_count == old_dealloc_cnt);
    }
    {
        tuple<packaged_task<F>> t{allocator_arg, allocator<int>{}, packaged_task<F>{}};
        (void) t;
    }
    {

        const auto old_alloc_cnt   = global_allocation_count;
        const auto old_dealloc_cnt = global_deallocation_count;
        {
            tuple<packaged_task<F>> t{allocator_arg, global_counting_allocator<int>{}, packaged_task<F>{}};
            (void) t;

            assert(global_allocation_count == old_alloc_cnt);
            assert(global_deallocation_count == old_dealloc_cnt);
        }
        assert(global_allocation_count == old_alloc_cnt);
        assert(global_deallocation_count == old_dealloc_cnt);
    }
}

struct alloc_dealloc_count_result {
    ptrdiff_t alloc_count;
    ptrdiff_t dealloc_count;
};

template <class F, class F2>
void test_construction_packaged_task_reset(F2&& functor) {
    assert(global_allocation_count == global_deallocation_count);

    const auto old_cnts = [&functor]() -> alloc_dealloc_count_result {
        packaged_task<F> pt{allocator_arg, global_counting_allocator<int>{}, forward<F2>(functor)};

        const auto alloc_cnt1   = global_allocation_count;
        const auto dealloc_cnt1 = global_deallocation_count;

        pt.reset();

        const auto alloc_cnt2   = global_allocation_count;
        const auto dealloc_cnt2 = global_deallocation_count;

#ifdef _CPPRTTI // TRANSITION, ABI, correct behavior should not rely on RTTI
        assert(alloc_cnt2 > alloc_cnt1);
#else // ^^^ defined(_CPPRTTI) / !defined(_CPPRTTI) vvv
        assert(alloc_cnt2 == alloc_cnt1);
#endif // ^^^ !defined(_CPPRTTI) ^^^
        assert(dealloc_cnt2 > dealloc_cnt1);

        return {alloc_cnt2, dealloc_cnt2};
    }();

    assert(global_allocation_count == old_cnts.alloc_count);
    assert(global_deallocation_count > old_cnts.dealloc_count);
    assert(global_allocation_count == global_deallocation_count);
}

int main() {
    test_construction_promise_in_tuple<int>();
    test_construction_promise_in_tuple<int&>();
    test_construction_promise_in_tuple<void>();

    test_construction_packaged_task_in_tuple<int()>();
    test_construction_packaged_task_in_tuple<int&()>();
    test_construction_packaged_task_in_tuple<void()>();

    test_construction_packaged_task_reset<int()>(large_functor<int>{});
    test_construction_packaged_task_reset<int&()>(large_functor<int&>{});
    test_construction_packaged_task_reset<void()>(large_functor<void>{});
}
