// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <functional>

using namespace std;

constexpr auto large_function_size = 100;

struct pass_this_by_ref {
    int v;

    pass_this_by_ref(int v_) : v(v_) {}

    pass_this_by_ref(const pass_this_by_ref&) {
        abort();
    }
};

struct counter {
    static int inst;
    static int copies;
    static int moves;

    counter() {
        ++inst;
    }

    counter(const counter&) {
        ++inst;
        ++copies;
    }

    counter(counter&&) noexcept {
        ++inst;
        ++moves;
    }

    ~counter() {
        --inst;
    }
};

int counter::inst   = 0;
int counter::copies = 0;
int counter::moves  = 0;

struct small_callable : counter {
    int operator()(int a, pass_this_by_ref& b) {
        assert(a == 23);
        assert(b.v == 63);
        return 38;
    }

    void* operator new(size_t)  = delete;
    void operator delete(void*) = delete;

    small_callable() = default;

    small_callable(const small_callable&) = default;

    small_callable(small_callable&&) noexcept = default;
};

struct large_callable : counter {
    char data[large_function_size];

    int operator()(int a, pass_this_by_ref& b) {
        assert(a == 23);
        assert(b.v == 63);
        return 39;
    }

    void* operator new(size_t)  = delete;
    void operator delete(void*) = delete;

    large_callable() = default;

    large_callable(const large_callable&) = default;

    large_callable(large_callable&&) noexcept = default;
};

struct odd_cc_callable : counter {
    int __fastcall operator()(int a, pass_this_by_ref& b) {
        assert(a == 23);
        assert(b.v == 63);
        return 40;
    }

    odd_cc_callable() = default;

    odd_cc_callable(const odd_cc_callable&)     = default;
    odd_cc_callable(odd_cc_callable&&) noexcept = default;
};

struct large_implicit_ptr_callable : counter {
    char data[large_function_size];

    using pfn = int (*)(int a, pass_this_by_ref& b);

    operator pfn() {
        return +[](int a, pass_this_by_ref& b) {
            assert(a == 23);
            assert(b.v == 63);
            return 41;
        };
    }

    large_implicit_ptr_callable() = default;

    large_implicit_ptr_callable(const large_implicit_ptr_callable&)     = default;
    large_implicit_ptr_callable(large_implicit_ptr_callable&&) noexcept = default;
};

int __fastcall plain_callable(int a, pass_this_by_ref& b) {
    assert(a == 23);
    assert(b.v == 63);
    return 42;
}

using test_function_t = move_only_function<int(int, pass_this_by_ref&)>;

template <class F, class... Args>
void test_construct_impl(int expect, Args... args) {
    {
        pass_this_by_ref x{63};

        test_function_t constructed_directly(F{args...});

        assert(constructed_directly(23, x) == expect);

        assert(constructed_directly);
        assert(constructed_directly != nullptr);

        test_function_t move_constructed = std::move(constructed_directly);

        assert(move_constructed(23, x) == expect);

        assert(!constructed_directly);
        assert(constructed_directly == nullptr);

        if constexpr (is_class_v<F>) {
            assert(counter::copies == 0);
        }

        F v{args...};
        test_function_t constructed_lvalue(v);
        if constexpr (is_class_v<F>) {
            assert(counter::copies == 1);
            counter::copies = 0;
        }

        if constexpr (is_class_v<F>) {
            counter::copies = 0;
            counter::moves  = 0;
        }
        test_function_t constructed_in_place(in_place_type<F>, args...);
        assert(constructed_in_place(23, x) == expect);
        assert(counter::copies == 0);
        assert(counter::moves == 0);
    }

    if constexpr (is_class_v<F>) {
        assert(counter::inst == 0);
    }
}

void test_assign() {
    pass_this_by_ref x{63};

    {
        test_function_t f1{small_callable{}};
        test_function_t f2{large_callable{}};
        f2 = std::move(f1);
        assert(f2(23, x) == 38);
        f1 = large_callable{};
        assert(f1(23, x) == 39);
        f1 = std::move(f1);
        assert(f1(23, x) == 39);
    }

    {
        test_function_t f1{large_callable{}};
        test_function_t f2{small_callable{}};
        f2 = std::move(f1);
        assert(f2(23, x) == 39);
        f1 = small_callable{};
        assert(f1(23, x) == 38);
        f1 = std::move(f1);
        assert(f1(23, x) == 38);
    }

    {
        test_function_t f1{small_callable{}};
        test_function_t f2{odd_cc_callable{}};
        f2 = std::move(f1);
        assert(f2(23, x) == 38);
        f1 = odd_cc_callable{};
        assert(f1(23, x) == 40);
    }

    {
        test_function_t f1{large_callable{}};
        test_function_t f2{large_implicit_ptr_callable{}};
        f2 = std::move(f1);
        assert(f2(23, x) == 39);
        f1 = large_implicit_ptr_callable{};
        assert(f1(23, x) == 41);
    }
}

void test_swap() {
    pass_this_by_ref x{63};

    {
        test_function_t f1{small_callable{}};
        test_function_t f2{large_callable{}};
        std::swap(f1, f2);
        assert(f2(23, x) == 38);
        assert(f1(23, x) == 39);
    }

    {
        test_function_t f1{large_callable{}};
        test_function_t f2{small_callable{}};
        f2.swap(f1);
        assert(f2(23, x) == 39);
        assert(f1(23, x) == 38);
    }

    {
        test_function_t f1{small_callable{}};
        test_function_t f2{odd_cc_callable{}};
        swap(f1, f2);
        assert(f2(23, x) == 38);
        assert(f1(23, x) == 40);
    }

    {
        test_function_t f1{large_callable{}};
        test_function_t f2{large_implicit_ptr_callable{}};
        swap(f1, f2);
        assert(f2(23, x) == 39);
        assert(f1(23, x) == 41);
    }
}


void test_empty() {
    test_function_t emtpty;
    assert(!emtpty);
    assert(emtpty == nullptr);
    assert(nullptr == emtpty);

    test_function_t emtpty_moved = std::move(emtpty);
    assert(!emtpty_moved);
    assert(emtpty_moved == nullptr);
    assert(!emtpty_moved);
    assert(emtpty_moved == nullptr);
}

int main() {
    test_construct_impl<small_callable>(38);
    test_construct_impl<large_callable>(39);
    test_construct_impl<odd_cc_callable>(40);
    test_construct_impl<large_implicit_ptr_callable>(41);
    test_construct_impl<decltype(&plain_callable)>(42, plain_callable);
    test_assign();
    test_swap();
    test_empty();
}
