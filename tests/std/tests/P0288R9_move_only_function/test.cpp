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

struct large_callable : small_callable {
    char data[large_function_size];
};

struct odd_cc_callable : counter {
    int __fastcall operator()(int a, pass_this_by_ref& b) {
        assert(a == 23);
        assert(b.v == 63);
        return 38;
    }

    odd_cc_callable() = default;

    odd_cc_callable(const odd_cc_callable&)     = default;
    odd_cc_callable(odd_cc_callable&&) noexcept = default;
};

int __fastcall plain_callable(int a, pass_this_by_ref& b) {
    assert(a == 23);
    assert(b.v == 63);
    return 38;
}

using test_function_t = move_only_function<int(int, pass_this_by_ref&)>;

template <class F, class... Args>
void test_impl(Args... args) {
    {
        pass_this_by_ref x{63};

        test_function_t constructed_directly(F{args...});

        assert(constructed_directly(23, x) == 38);

        assert(constructed_directly);
        assert(constructed_directly != nullptr);

        test_function_t move_constructed = std::move(constructed_directly);

        assert(move_constructed(23, x) == 38);

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
        assert(constructed_in_place(23, x) == 38);
        assert(counter::copies == 0);
        assert(counter::moves == 0);
    }

    if constexpr (is_class_v<F>) {
        assert(counter::inst == 0);
    }
}

void test_empty() {
    test_function_t emtpty;
    assert(!emtpty);
    assert(emtpty == nullptr);

    test_function_t emtpty_moved = std::move(emtpty);
    assert(!emtpty_moved);
    assert(emtpty_moved == nullptr);
    assert(!emtpty_moved);
    assert(emtpty_moved == nullptr);
}

int main() {
    test_impl<small_callable>();
    test_impl<large_callable>();
    test_impl<odd_cc_callable>();
    test_impl<decltype(&plain_callable)>(plain_callable);
    test_empty();
}
