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
    ;

    counter() {
        inst++;
    }

    counter(const counter&) {
        inst++;
    }

    counter(counter&&) noexcept {
        inst++;
    }

    ~counter() {
        inst--;
    }
};

int counter::inst = 0;

struct small_callable : counter {
    int operator()(int a, pass_this_by_ref& b) {
        assert(a == 23);
        assert(b.v == 63);
        return 38;
    }

    small_callable() = default;

    small_callable(const small_callable&) {
        abort();
    }

    small_callable(small_callable&&) noexcept = default;
};

struct large_callable : small_callable {
    char data[large_function_size];
};

using test_function_t = move_only_function<int(int, pass_this_by_ref&)>;

template <class F>
void test_impl() {
    {
        pass_this_by_ref x{63};

        test_function_t f1(F{});
        assert(f1(23, x) == 38);

        test_function_t f2 = std::move(f1);

        assert(f2(23, x) == 38);

        try {
            f1(24, x);
            abort();
        } catch (std::bad_function_call&) {
        }
    }
    assert(counter::inst == 0);
}


int main() {
    test_impl<small_callable>();
    test_impl<large_callable>();
}
