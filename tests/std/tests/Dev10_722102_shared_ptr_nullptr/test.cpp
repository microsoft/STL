// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

//  Dev10-722102 "STL: Get nullptr overloads"
// DevDiv-520681 "Faulty implementation of shared_ptr(nullptr_t) constructor"

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <functional>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

#pragma warning(disable : 28251) // Inconsistent annotation for 'new': this instance has no annotations.

void Del(int* p) {
    delete p;
}

namespace {
    bool g_throw_on_alloc = false;
}

void* operator new(size_t const size) {
    if (!g_throw_on_alloc) {
        if (void* const vp = malloc(size)) {
            return vp;
        }
    }

    throw bad_alloc{};
}

void operator delete(void* const vp, size_t) noexcept {
    free(vp);
}

struct NullptrDeleter {
    template <class T>
    void operator()(T*) const {
        abort();
    }

    void operator()(nullptr_t) const {}
};

struct ImmobileDeleter {
    ImmobileDeleter()                             = default;
    ImmobileDeleter(ImmobileDeleter&&)            = delete;
    ImmobileDeleter& operator=(ImmobileDeleter&&) = delete;

    void operator()(void*) const {}
};

class NonDeleter {};

void test_deleter() {
    // VSO-387662: deleter should be called with a nullptr_t object,
    // not with (int *)nullptr.
    shared_ptr<int>{nullptr, NullptrDeleter{}};
    shared_ptr<int>{nullptr, NullptrDeleter{}, allocator<int>{}};
}

void test_exception() {
    // VSO-387662: deleter should be called with a nullptr_t object,
    // not with (int *)nullptr.
    g_throw_on_alloc = true;
    try {
        shared_ptr<int>{nullptr, NullptrDeleter{}};
        abort();
    } catch (bad_alloc&) {
    }
    try {
        shared_ptr<int>{nullptr, NullptrDeleter{}, allocator<int>{}};
        abort();
    } catch (bad_alloc&) {
    }
    g_throw_on_alloc = false;
}


void test_sfinae() {
    // per LWG-2875
    using SP = shared_ptr<int>;
    using A  = allocator<int>;

    // deleter must be move constructible
    STATIC_ASSERT(is_constructible_v<SP, nullptr_t, default_delete<int>>);
    STATIC_ASSERT(!is_constructible_v<SP, nullptr_t, ImmobileDeleter>);

    STATIC_ASSERT(is_constructible_v<SP, nullptr_t, default_delete<int>, A>);
    STATIC_ASSERT(!is_constructible_v<SP, nullptr_t, ImmobileDeleter, A>);

    // d(p) must be well-formed
    STATIC_ASSERT(!is_constructible_v<SP, nullptr_t, NonDeleter>);
    STATIC_ASSERT(!is_constructible_v<SP, nullptr_t, NonDeleter, A>);
}

namespace pointer {
    struct Base {};
    struct Derived : Base {};

    void test_sfinae() {
        {
            // per LWG-2874

            STATIC_ASSERT(is_constructible_v<shared_ptr<int>, int*>);
            STATIC_ASSERT(is_constructible_v<shared_ptr<const int>, int*>);
            STATIC_ASSERT(is_constructible_v<shared_ptr<void>, int*>);
            STATIC_ASSERT(is_constructible_v<shared_ptr<Base>, Derived*>);
            STATIC_ASSERT(!is_constructible_v<shared_ptr<int>, const int*>);
            STATIC_ASSERT(!is_constructible_v<shared_ptr<int>, void*>);
            STATIC_ASSERT(!is_constructible_v<shared_ptr<Derived>, Base*>);

            STATIC_ASSERT(is_constructible_v<shared_ptr<int[]>, int*>);
            STATIC_ASSERT(is_constructible_v<shared_ptr<int[42]>, int*>);
            STATIC_ASSERT(is_constructible_v<shared_ptr<const int[]>, int*>);
            STATIC_ASSERT(is_constructible_v<shared_ptr<const int[42]>, int*>);
            STATIC_ASSERT(!is_constructible_v<shared_ptr<Base[]>, Derived*>);
            STATIC_ASSERT(!is_constructible_v<shared_ptr<Base[42]>, Derived*>);
            STATIC_ASSERT(!is_constructible_v<shared_ptr<int[]>, const int*>);
            STATIC_ASSERT(!is_constructible_v<shared_ptr<int[42]>, const int*>);
        }
        {
            // per LWG-2875

            // deleter must be move constructible
            STATIC_ASSERT(is_constructible_v<shared_ptr<int>, int*, default_delete<int>>);
            STATIC_ASSERT(!is_constructible_v<shared_ptr<int>, int*, ImmobileDeleter>);

            STATIC_ASSERT(is_constructible_v<shared_ptr<int>, int*, default_delete<int>, allocator<int>>);
            STATIC_ASSERT(!is_constructible_v<shared_ptr<int>, int*, ImmobileDeleter, allocator<int>>);

            // d(p) must be well-formed
            STATIC_ASSERT(!is_constructible_v<shared_ptr<int>, int*, NonDeleter>);
            STATIC_ASSERT(!is_constructible_v<shared_ptr<int>, int*, NonDeleter, allocator<int>>);

            // Y * must be convertible to T *
            STATIC_ASSERT(is_constructible_v<shared_ptr<Base>, Derived*, default_delete<Derived>>);
            STATIC_ASSERT(!is_constructible_v<shared_ptr<int>, const int*, default_delete<const int>>);
            STATIC_ASSERT(!is_constructible_v<shared_ptr<int>, void*, void (*)(void*)>);
            STATIC_ASSERT(!is_constructible_v<shared_ptr<Derived>, Base*, default_delete<Base>>);
        }
    }

    void test() {
        test_sfinae();
    }
} // namespace pointer

namespace unique_ptr_ {
    struct secret_tag {};

    template <class T>
    struct fancy_pointer {
        T* ptr_{nullptr};

        fancy_pointer() = default;

        explicit fancy_pointer(secret_tag, T* ptr) : ptr_{ptr} {}

        fancy_pointer(nullptr_t) {}

        fancy_pointer& operator=(nullptr_t) {
            ptr_ = nullptr;
            return *this;
        }

        friend bool operator==(const fancy_pointer& left, const fancy_pointer& right) {
            return left.ptr_ == right.ptr_;
        }

        friend bool operator!=(const fancy_pointer& left, const fancy_pointer& right) {
            return left.ptr_ != right.ptr_;
        }

        friend bool operator==(const fancy_pointer& left, nullptr_t) {
            return left.ptr_ == nullptr;
        }

        friend bool operator!=(const fancy_pointer& left, nullptr_t) {
            return left.ptr_ != nullptr;
        }

        friend bool operator==(nullptr_t, const fancy_pointer& right) {
            return nullptr == right.ptr_;
        }

        friend bool operator!=(nullptr_t, const fancy_pointer& right) {
            return nullptr != right.ptr_;
        }

        operator T*() const {
            return ptr_;
        }
    };

    template <class T>
    struct fancy_deleter {
        using pointer = fancy_pointer<T>;

        void operator()(pointer ptr) const noexcept {
            assert(ptr.ptr_ != nullptr);
            _Analysis_assume_(ptr.ptr_);
            assert(*ptr.ptr_ == 42);
            *ptr.ptr_ = 13;
        }

        // Also test LWG-3548 "shared_ptr construction from unique_ptr should move (not copy) the deleter".
        fancy_deleter()                = default;
        fancy_deleter(fancy_deleter&&) = default;

        fancy_deleter(const fancy_deleter&)            = delete;
        fancy_deleter& operator=(fancy_deleter&&)      = delete;
        fancy_deleter& operator=(const fancy_deleter&) = delete;
    };

    template <class>
    struct Counted {
        static int count;
        Counted() {
            ++count;
        }
        Counted(Counted const&) {
            ++count;
        }
        Counted& operator=(Counted const&) = delete;
        ~Counted() noexcept {
            --count;
        }
    };
    template <class T>
    int Counted<T>::count = 0;

    struct CountedDeleter : Counted<CountedDeleter> {
        template <class T>
        void operator()(T* ptr) const {
            delete ptr;
        }
    };

    struct AssertDeleter : Counted<AssertDeleter> {
        void operator()(void*) const {
            abort();
        }
    };

    void test_fancy() {
        int i = 42;
        unique_ptr<int, fancy_deleter<int>> up{fancy_pointer<int>{secret_tag{}, &i}};
        {
            shared_ptr<int> sp{move(up)};
            assert(sp.get() == &i);
        }
        assert(i == 13);
    }

    void test_ref_deleter() {
        CountedDeleter cd;
        assert(CountedDeleter::count == 1);
        {
            shared_ptr<int> sp{unique_ptr<int, CountedDeleter&>{new int{42}, cd}};
            assert(CountedDeleter::count == 1);
            assert(get_deleter<reference_wrapper<CountedDeleter>>(sp) != nullptr);
            assert(&get_deleter<reference_wrapper<CountedDeleter>>(sp)->get() == &cd);
        }
        {
            reference_wrapper<CountedDeleter> rw{cd};
            shared_ptr<int> sp{unique_ptr<int, reference_wrapper<CountedDeleter>&>{new int{42}, rw}};
            assert(CountedDeleter::count == 1);
            assert(get_deleter<reference_wrapper<CountedDeleter>>(sp) != nullptr);
            assert(&get_deleter<reference_wrapper<CountedDeleter>>(sp)->get() == &cd);
        }
    }

    void test_exception() {
        auto up          = make_unique<int>(42);
        auto rawptr      = up.get();
        g_throw_on_alloc = true;
        try {
            shared_ptr<int>{move(up)};
            abort();
        } catch (bad_alloc&) {
        }
        g_throw_on_alloc = false;
        assert(up.get() == rawptr);
    }

    void test_lwg_2415() {
        // per LWG-2415: "Inconsistency between unique_ptr and shared_ptr"
        assert(AssertDeleter::count == 0);
        unique_ptr<int, AssertDeleter> up{nullptr};
        assert(AssertDeleter::count == 1);
        shared_ptr<int> sp{move(up)};
        assert(AssertDeleter::count == 1);
        assert(sp.use_count() == 0);
        assert(sp.get() == nullptr);
    }

    void test() {
        test_fancy();
        test_ref_deleter();
        test_exception();
        test_lwg_2415();
    }
} // namespace unique_ptr_

namespace weak_ptr_ {
    struct Base {};
    struct Derived : Base {};

    void test_sfinae() {
        // per LWG-2876

        STATIC_ASSERT(is_constructible_v<shared_ptr<int>, weak_ptr<int>>);
        STATIC_ASSERT(!is_constructible_v<shared_ptr<int>, weak_ptr<const int>>);
        STATIC_ASSERT(is_constructible_v<shared_ptr<void>, weak_ptr<int>>);
        STATIC_ASSERT(!is_constructible_v<shared_ptr<void>, weak_ptr<const int>>);
        STATIC_ASSERT(is_constructible_v<shared_ptr<const void>, weak_ptr<const int>>);

        STATIC_ASSERT(is_constructible_v<shared_ptr<int[]>, weak_ptr<int[]>>);
        STATIC_ASSERT(is_constructible_v<shared_ptr<int[]>, weak_ptr<int[42]>>);
        STATIC_ASSERT(!is_constructible_v<shared_ptr<int[42]>, weak_ptr<int[]>>);
        STATIC_ASSERT(is_constructible_v<shared_ptr<int[42]>, weak_ptr<int[42]>>);

        STATIC_ASSERT(!is_constructible_v<shared_ptr<int[]>, weak_ptr<const int[]>>);
        STATIC_ASSERT(!is_constructible_v<shared_ptr<int[]>, weak_ptr<const int[42]>>);
        STATIC_ASSERT(!is_constructible_v<shared_ptr<int[42]>, weak_ptr<const int[]>>);
        STATIC_ASSERT(!is_constructible_v<shared_ptr<int[42]>, weak_ptr<const int[42]>>);

        STATIC_ASSERT(is_constructible_v<shared_ptr<const int[]>, weak_ptr<int[]>>);
        STATIC_ASSERT(is_constructible_v<shared_ptr<const int[]>, weak_ptr<int[42]>>);
        STATIC_ASSERT(!is_constructible_v<shared_ptr<const int[42]>, weak_ptr<int[]>>);
        STATIC_ASSERT(is_constructible_v<shared_ptr<const int[42]>, weak_ptr<int[42]>>);

        STATIC_ASSERT(!is_constructible_v<shared_ptr<Base[]>, weak_ptr<Derived[]>>);
        STATIC_ASSERT(!is_constructible_v<shared_ptr<Base[42]>, weak_ptr<Derived[]>>);
        STATIC_ASSERT(!is_constructible_v<shared_ptr<Base[]>, weak_ptr<Derived[42]>>);
        STATIC_ASSERT(!is_constructible_v<shared_ptr<Base[42]>, weak_ptr<Derived[42]>>);
    }

    void test() {
        test_sfinae();
    }
} // namespace weak_ptr_

int main() {
    allocator<int> Alloc;

    {
        shared_ptr<int> sp1;
        assert(!sp1 && !sp1.get() && sp1.use_count() == 0);
    }

    {
        shared_ptr<int> sp2(nullptr);
        assert(!sp2 && !sp2.get() && sp2.use_count() == 0);
    }

    {
        shared_ptr<int> sp3(static_cast<int*>(nullptr));
        assert(!sp3 && !sp3.get() && sp3.use_count() == 1);
    }

    {
        shared_ptr<int> sp4(static_cast<int*>(nullptr), Del);
        assert(!sp4 && !sp4.get() && sp4.use_count() == 1);
    }

    {
        shared_ptr<int> sp5(static_cast<int*>(nullptr), Del, Alloc);
        assert(!sp5 && !sp5.get() && sp5.use_count() == 1);
    }

    {
        shared_ptr<int> sp6(new int(11));
        assert(sp6 && sp6.get() && sp6.use_count() == 1);
    }

    {
        shared_ptr<int> sp7(new int(22), Del);
        assert(sp7 && sp7.get() && sp7.use_count() == 1);
    }

    {
        shared_ptr<int> sp8(new int(33), Del, Alloc);
        assert(sp8 && sp8.get() && sp8.use_count() == 1);
    }

    {
        shared_ptr<int> sp9(nullptr, Del);
        assert(!sp9 && !sp9.get() && sp9.use_count() == 1);
    }

    {
        shared_ptr<int> sp10(nullptr, Del, Alloc);
        assert(!sp10 && !sp10.get() && sp10.use_count() == 1);
    }

    {
        shared_ptr<int> empty;
        shared_ptr<int> sp11(empty, static_cast<int*>(nullptr));
        assert(!empty && !empty.get() && empty.use_count() == 0);
        assert(!sp11 && !sp11.get() && sp11.use_count() == 0);
    }

    {
        shared_ptr<int> empty;
        int n = 1729;
        shared_ptr<int> sp12(empty, &n);
        assert(!empty && !empty.get() && empty.use_count() == 0);
        assert(sp12 && sp12.get() == &n && sp12.use_count() == 0);
    }

    {
        shared_ptr<pair<int, int>> full(new pair<int, int>(11, 22));
        shared_ptr<int> sp13(full, static_cast<int*>(nullptr));
        assert(full && full.get() && full.use_count() == 2);
        assert(!sp13 && !sp13.get() && sp13.use_count() == 2);
    }

    {
        shared_ptr<pair<int, int>> full(new pair<int, int>(11, 22));
        shared_ptr<int> sp14(full, &full->second);
        assert(full && full.get() && full.use_count() == 2);
        assert(sp14 && sp14.get() == &full->second && sp14.use_count() == 2);
    }

    {
        shared_ptr<int> empty;
        shared_ptr<int> sp15(empty);
        assert(!empty && !empty.get() && empty.use_count() == 0);
        assert(!sp15 && !sp15.get() && sp15.use_count() == 0);
    }

    {
        shared_ptr<int> empty;
        shared_ptr<const int> sp16(empty);
        assert(!empty && !empty.get() && empty.use_count() == 0);
        assert(!sp16 && !sp16.get() && sp16.use_count() == 0);
    }

    {
        shared_ptr<int> empty;
        shared_ptr<int> sp17(move(empty));
        assert(!empty && !empty.get() && empty.use_count() == 0);
        assert(!sp17 && !sp17.get() && sp17.use_count() == 0);
    }

    {
        shared_ptr<int> empty;
        shared_ptr<const int> sp18(move(empty));
        assert(!empty && !empty.get() && empty.use_count() == 0);
        assert(!sp18 && !sp18.get() && sp18.use_count() == 0);
    }

    {
        shared_ptr<int> full(new int(1729));
        shared_ptr<int> sp19(full);
        assert(full && full.get() && full.use_count() == 2);
        assert(sp19 && sp19.get() && sp19.use_count() == 2);
    }

    {
        shared_ptr<int> full(new int(1729));
        shared_ptr<const int> sp20(full);
        assert(full && full.get() && full.use_count() == 2);
        assert(sp20 && sp20.get() && sp20.use_count() == 2);
    }

    {
        shared_ptr<int> full(new int(1729));
        shared_ptr<int> sp21(move(full));
        assert(!full && !full.get() && full.use_count() == 0);
        assert(sp21 && sp21.get() && sp21.use_count() == 1);
    }

    {
        shared_ptr<int> full(new int(1729));
        shared_ptr<const int> sp22(move(full));
        assert(!full && !full.get() && full.use_count() == 0);
        assert(sp22 && sp22.get() && sp22.use_count() == 1);
    }

    {
        shared_ptr<int> orig(new int(1729));
        weak_ptr<int> weak(orig);
        shared_ptr<int> sp23(weak);
        assert(orig && orig.get() && orig.use_count() == 2);
        assert(sp23 && sp23.get() && sp23.use_count() == 2);
    }

#if _HAS_AUTO_PTR_ETC
    {
        auto_ptr<int> ap;
        shared_ptr<int> sp24(move(ap));
        assert(!ap.get());
        assert(!sp24 && !sp24.get() && sp24.use_count() == 1);
    }

    {
        auto_ptr<int> ap(new int(1729));
        shared_ptr<int> sp25(move(ap));
        assert(!ap.get());
        assert(sp25 && sp25.get() && sp25.use_count() == 1);
    }
#endif // _HAS_AUTO_PTR_ETC

    {
        unique_ptr<int> up;
        shared_ptr<int> sp26(move(up));
        assert(!up && !up.get());
        assert(!sp26 && !sp26.get() && sp26.use_count() == 0);
    }

    {
        unique_ptr<int> up(new int(1729));
        shared_ptr<int> sp27(move(up));
        assert(!up && !up.get());
        assert(sp27 && sp27.get() && sp27.use_count() == 1);
    }

    test_deleter();
    test_exception();
    test_sfinae();

    pointer::test();
    unique_ptr_::test();
    weak_ptr_::test();
}
