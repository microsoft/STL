// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <memory>
#include <utility>

using namespace std;

void test_raw_ptr() {
    int i        = 5;
    int* int_ptr = &i;

    {
        const auto f = [](int** ptr) {
            assert(**ptr == 5);
            static int i = 7;
            *ptr         = &i;
        };

        f(inout_ptr(int_ptr));

        assert(*int_ptr == 7);
    }
    {
        const auto f = [](void** ptr) {
            assert(*static_cast<int*>(*ptr) == 7);
            static int i = 15;
            *ptr         = &i;
        };

        f(inout_ptr(int_ptr));

        assert(*int_ptr == 15);
    }
}

void test_shared_ptr() {
    shared_ptr<int> int_ptr = make_shared<int>(5);

    {
        const auto f = [](int** ptr) { *ptr = new int(7); };

        f(out_ptr(int_ptr, default_delete<int>{}));

        assert(*int_ptr == 7);
    }
    {
        const auto f = [](void** ptr) { *ptr = new int(15); };

        f(out_ptr(int_ptr, default_delete<int>{}));

        assert(*int_ptr == 15);
    }

    int count          = 0;
    const auto deleter = [&count](int* ptr) {
        ++count;
        delete ptr;
    };

    {
        const auto f = [](int** ptr) { *ptr = new int(23); };

        f(out_ptr(int_ptr, deleter));

        assert(count == 0);
        assert(*int_ptr == 23);
    }
    {
        const auto f = [](void** ptr) { *ptr = new int(32); };

        f(out_ptr(int_ptr, deleter));

        assert(count == 1);
        assert(*int_ptr == 32);
    }

    // LWG-3734 Inconsistency in inout_ptr and out_ptr for empty case
    {
        const auto f = [](void** ptr) { *ptr = new int(42); };

        {
            auto temp_adaptor = out_ptr(int_ptr, deleter);
            assert(int_ptr.get() == nullptr);
            f(temp_adaptor);
        }

        assert(count == 2);
        assert(*int_ptr == 42);
    }

    int_ptr.reset();
    assert(count == 3);
}

template <class Ptr, class Pointer = void, class... Args>
void test_smart_ptr(Args&&... args) {
    Ptr int_ptr{new int(5)};

    {
        const auto f = [](int** ptr) {
            assert(**ptr == 5);
            *ptr = new int(7);
        };

        f(inout_ptr<Pointer>(int_ptr, forward<Args>(args)...));

        assert(*int_ptr == 7);
    }
    {
        const auto f = [](int** ptr) { *ptr = new int(12); };

        f(out_ptr<Pointer>(int_ptr, forward<Args>(args)...));

        assert(*int_ptr == 12);
    }
    {
        const auto f = [](void** ptr) {
            assert(*static_cast<int*>(*ptr) == 12);
            *ptr = new int(15);
        };

        f(inout_ptr<int*>(int_ptr, forward<Args>(args)...));

        assert(*int_ptr == 15);
    }
    {
        const auto f = [](void** ptr) { *ptr = new int(19); };

        f(out_ptr<int*>(int_ptr, forward<Args>(args)...));

        assert(*int_ptr == 19);
    }

    // LWG-3734 Inconsistency in inout_ptr and out_ptr for empty case
    {
        const auto f = [](void** ptr) { *ptr = new int(42); };

        {
            auto temp_adaptor = out_ptr<int*>(int_ptr);
            assert(int_ptr.get() == nullptr);
            f(temp_adaptor);
        }

        assert(*int_ptr == 42);
    }

    // LWG-3594 inout_ptr - inconsistent release() in destructor
    {
        const auto f = [](int** ptr) {
            delete *ptr;
            *ptr = nullptr;
        };

        f(inout_ptr<int*>(int_ptr, forward<Args>(args)...));

        assert(int_ptr.get() == nullptr);
    }
}

struct reset_tag {};

struct resettable_ptr {
    using element_type = int; // test having element_type only

    unique_ptr<int> ptr;

    explicit resettable_ptr(int* p) : ptr(p) {}

    void reset() {
        ptr.reset();
    }

    void reset(int* p, reset_tag) {
        ptr.reset(p);
    }

    auto operator*() const {
        return *ptr;
    }

    auto get() const {
        return ptr.get();
    }

    void release() {
        ptr.release();
    }
};

struct constructible_ptr {
    using pointer = int*; // test having pointer only

    unique_ptr<int> ptr;

    constructible_ptr() = default;
    explicit constructible_ptr(int* p) : ptr(p) {}
    explicit constructible_ptr(int* p, reset_tag) : ptr(p) {}

    auto operator*() const {
        return *ptr;
    }

    auto get() const {
        return ptr.get();
    }

    void release() {
        ptr.release();
    }
};

struct resettable_ptr2 : resettable_ptr {
    using resettable_ptr::resettable_ptr;

private:
    using resettable_ptr::element_type;
};

template <>
struct pointer_traits<resettable_ptr2> {
    using element_type = int; // test having only pointer_traits::element_type
};

struct constructible_ptr2 : constructible_ptr {
    using constructible_ptr::constructible_ptr;

private:
    using constructible_ptr::pointer;
};

template <>
struct pointer_traits<constructible_ptr2> {
    // test having nothing
};

int main() {
    test_raw_ptr();
    test_shared_ptr();
    test_smart_ptr<unique_ptr<int>>();
    test_smart_ptr<resettable_ptr>(reset_tag{});
    test_smart_ptr<constructible_ptr>(reset_tag{});
    test_smart_ptr<resettable_ptr2>(reset_tag{});
    test_smart_ptr<constructible_ptr2, int*>(reset_tag{});
}
