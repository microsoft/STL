// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <memory>
#include <type_traits>

using namespace std;

int val = 0;

struct decrement_val {
    void operator()(void (*)(int)) const {
        --val;
    }
};

void add(const int i) {
    val += i;
}

void add_noexcept(const int i) noexcept {
    val += i;
}

int add_return(const int i) {
    return val += i;
}

void subtract(const int i) {
    val -= i;
}

int main() {
    static_assert(!is_constructible_v<shared_ptr<void(int)>, void (*)(int)>,
        "shared_ptr of function type should not be constructible without deleter");
    {
        shared_ptr<void(int)> s1(add, decrement_val{});
        assert(s1.use_count() == 1);
        assert(s1.get() == add);

        s1.get()(2);
        assert(val == 2);

        shared_ptr<void(int)> s2 = s1;
        assert(s1.use_count() == 2);
        assert(s2.use_count() == 2);
        assert(s2.get() == add);

        s2.reset(subtract, decrement_val{});
        assert(s1.use_count() == 1);
        assert(s2.use_count() == 1);
        assert(s2.get() == subtract);

        (*s2)(1);
        assert(val == 1);

        s2.reset();
        assert(s2 == nullptr);
        assert(val == 0);

        s2.swap(s1);
        assert(s1 == nullptr);
        assert(s2.get() == add);

        shared_ptr<int(int)> s3(s2, add_return);
        assert((*s3)(1) == 1);

        weak_ptr<void(int)> w1 = s2;
        (*w1.lock())(2);
        assert(val == 3);

#ifdef __cpp_noexcept_function_type
        shared_ptr<void(int)> s4(add_noexcept, decrement_val{});
        assert(s4.get() == add_noexcept);
        (*s4)(1);
        assert(val == 4);

        shared_ptr<void(int) noexcept> s5(add_noexcept, decrement_val{});
        assert(s5.get() == add_noexcept);
        (*s5)(1);
        assert(val == 5);

        s4 = s5;
        assert(s4.get() == add_noexcept);

        static_assert(!is_constructible_v<shared_ptr<void(int) noexcept>, void (*)(int), decrement_val>,
            "shared_ptr of noexcept function type should not be constructible from non-noexcept function pointer");
        static_assert(!is_convertible_v<shared_ptr<void(int)>, shared_ptr<void(int) noexcept>>,
            "shared_ptr of non-noexcept function type should not be convertible to shared_ptr of noexcept function "
            "type");
#endif // __cpp_noexcept_function_type
    }
    assert(val == 2);
}
