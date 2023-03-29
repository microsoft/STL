// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <array>
#include <cassert>
#include <cstddef>
#include <memory>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

template <typename T>
void assert_empty_null(const shared_ptr<T>& sp) {
    assert(sp.use_count() == 0);
    assert(sp.get() == nullptr);
}

struct NullDeleter {
    void operator()(nullptr_t) const noexcept {}
};


void test_shared_ptr_typedefs() {
    STATIC_ASSERT(is_same_v<shared_ptr<string[]>::element_type, string>);
    STATIC_ASSERT(is_same_v<shared_ptr<string[4]>::element_type, string>);
    STATIC_ASSERT(is_same_v<shared_ptr<const string[]>::element_type, const string>);
    STATIC_ASSERT(is_same_v<shared_ptr<const string[4]>::element_type, const string>);

#if _HAS_CXX17
    STATIC_ASSERT(is_same_v<shared_ptr<string[]>::weak_type, weak_ptr<string[]>>);
    STATIC_ASSERT(is_same_v<shared_ptr<string[4]>::weak_type, weak_ptr<string[4]>>);
    STATIC_ASSERT(is_same_v<shared_ptr<const string[]>::weak_type, weak_ptr<const string[]>>);
    STATIC_ASSERT(is_same_v<shared_ptr<const string[4]>::weak_type, weak_ptr<const string[4]>>);
#endif // _HAS_CXX17
}

void test_shared_ptr_default_ctor() {
    STATIC_ASSERT(is_nothrow_default_constructible_v<shared_ptr<string[]>>);
    STATIC_ASSERT(is_nothrow_default_constructible_v<shared_ptr<string[4]>>);

    shared_ptr<string[]> sp0;
    shared_ptr<string[4]> sp1;

    assert_empty_null(sp0);
    assert_empty_null(sp1);
}

void test_shared_ptr_nullptr_ctor() {
    STATIC_ASSERT(is_nothrow_constructible_v<shared_ptr<string[]>, nullptr_t>);
    STATIC_ASSERT(is_nothrow_constructible_v<shared_ptr<string[4]>, nullptr_t>);

    shared_ptr<string[]> sp0  = nullptr;
    shared_ptr<string[4]> sp1 = nullptr;

    assert_empty_null(sp0);
    assert_empty_null(sp1);
}

template <typename T>
void impl_shared_ptr_rawptr_ctor() {
    string* p = new string[4];
    shared_ptr<T> sp(p);
    assert(sp.use_count() == 1);
    assert(sp.get() == p);
}

void test_shared_ptr_rawptr_ctor() {
    STATIC_ASSERT(is_constructible_v<shared_ptr<string[]>, string*>);
    STATIC_ASSERT(is_constructible_v<shared_ptr<string[4]>, string*>);
    STATIC_ASSERT(!is_constructible_v<shared_ptr<string[]>, const string*>);
    STATIC_ASSERT(!is_constructible_v<shared_ptr<string[4]>, const string*>);
    STATIC_ASSERT(is_constructible_v<shared_ptr<const string[]>, string*>);
    STATIC_ASSERT(is_constructible_v<shared_ptr<const string[4]>, string*>);
    STATIC_ASSERT(is_constructible_v<shared_ptr<const string[]>, const string*>);
    STATIC_ASSERT(is_constructible_v<shared_ptr<const string[4]>, const string*>);

    impl_shared_ptr_rawptr_ctor<string[]>();
    impl_shared_ptr_rawptr_ctor<string[4]>();
    impl_shared_ptr_rawptr_ctor<const string[]>();
    impl_shared_ptr_rawptr_ctor<const string[4]>();
}

template <typename T, typename Del>
void impl_shared_ptr_rawptr_del_ctor() {
    string* p = new string[4];
    shared_ptr<T> sp(p, Del{});
    assert(sp.use_count() == 1);
    assert(sp.get() == p);
}

void test_shared_ptr_rawptr_del_ctor() {
    using Del = default_delete<const string[]>;

    STATIC_ASSERT(is_constructible_v<shared_ptr<string[]>, string*, Del>);
    STATIC_ASSERT(is_constructible_v<shared_ptr<string[4]>, string*, Del>);
    STATIC_ASSERT(!is_constructible_v<shared_ptr<string[]>, const string*, Del>);
    STATIC_ASSERT(!is_constructible_v<shared_ptr<string[4]>, const string*, Del>);
    STATIC_ASSERT(is_constructible_v<shared_ptr<const string[]>, string*, Del>);
    STATIC_ASSERT(is_constructible_v<shared_ptr<const string[4]>, string*, Del>);
    STATIC_ASSERT(is_constructible_v<shared_ptr<const string[]>, const string*, Del>);
    STATIC_ASSERT(is_constructible_v<shared_ptr<const string[4]>, const string*, Del>);

    impl_shared_ptr_rawptr_del_ctor<string[], Del>();
    impl_shared_ptr_rawptr_del_ctor<string[4], Del>();
    impl_shared_ptr_rawptr_del_ctor<const string[], Del>();
    impl_shared_ptr_rawptr_del_ctor<const string[4], Del>();
}

template <typename T, typename Del, typename Al>
void impl_shared_ptr_rawptr_del_al_ctor() {
    string* p = new string[4];
    shared_ptr<T> sp(p, Del{}, Al{});
    assert(sp.use_count() == 1);
    assert(sp.get() == p);
}

void test_shared_ptr_rawptr_del_al_ctor() {
    using Del = default_delete<const string[]>;
    using Al  = allocator<int>;

    STATIC_ASSERT(is_constructible_v<shared_ptr<string[]>, string*, Del, Al>);
    STATIC_ASSERT(is_constructible_v<shared_ptr<string[4]>, string*, Del, Al>);
    STATIC_ASSERT(!is_constructible_v<shared_ptr<string[]>, const string*, Del, Al>);
    STATIC_ASSERT(!is_constructible_v<shared_ptr<string[4]>, const string*, Del, Al>);
    STATIC_ASSERT(is_constructible_v<shared_ptr<const string[]>, string*, Del, Al>);
    STATIC_ASSERT(is_constructible_v<shared_ptr<const string[4]>, string*, Del, Al>);
    STATIC_ASSERT(is_constructible_v<shared_ptr<const string[]>, const string*, Del, Al>);
    STATIC_ASSERT(is_constructible_v<shared_ptr<const string[4]>, const string*, Del, Al>);

    impl_shared_ptr_rawptr_del_al_ctor<string[], Del, Al>();
    impl_shared_ptr_rawptr_del_al_ctor<string[4], Del, Al>();
    impl_shared_ptr_rawptr_del_al_ctor<const string[], Del, Al>();
    impl_shared_ptr_rawptr_del_al_ctor<const string[4], Del, Al>();
}

template <typename T>
void impl_shared_ptr_nullptr_del_ctor() {
    using Del = NullDeleter;
    STATIC_ASSERT(is_constructible_v<shared_ptr<T>, nullptr_t, Del>);
    shared_ptr<T> sp(nullptr, Del{});
    assert(sp.use_count() == 1);
    assert(sp.get() == nullptr);
}

void test_shared_ptr_nullptr_del_ctor() {
    impl_shared_ptr_nullptr_del_ctor<string[]>();
    impl_shared_ptr_nullptr_del_ctor<string[4]>();
    impl_shared_ptr_nullptr_del_ctor<const string[]>();
    impl_shared_ptr_nullptr_del_ctor<const string[4]>();
}

template <typename T>
void impl_shared_ptr_nullptr_del_al_ctor() {
    using Del = NullDeleter;
    using Al  = allocator<int>;
    STATIC_ASSERT(is_constructible_v<shared_ptr<T>, nullptr_t, Del, Al>);
    shared_ptr<T> sp(nullptr, Del{}, Al{});
    assert(sp.use_count() == 1);
    assert(sp.get() == nullptr);
}

void test_shared_ptr_nullptr_del_al_ctor() {
    impl_shared_ptr_nullptr_del_al_ctor<string[]>();
    impl_shared_ptr_nullptr_del_al_ctor<string[4]>();
    impl_shared_ptr_nullptr_del_al_ctor<const string[]>();
    impl_shared_ptr_nullptr_del_al_ctor<const string[4]>();
}

void test_shared_ptr_aliasing_ctor() {
    STATIC_ASSERT(is_nothrow_constructible_v<shared_ptr<string[]>, const shared_ptr<array<string, 4>>&, string*>);
    STATIC_ASSERT(is_nothrow_constructible_v<shared_ptr<string[4]>, const shared_ptr<array<string, 4>>&, string*>);
    STATIC_ASSERT(
        is_nothrow_constructible_v<shared_ptr<const string[]>, const shared_ptr<array<string, 4>>&, const string*>);
    STATIC_ASSERT(
        is_nothrow_constructible_v<shared_ptr<const string[4]>, const shared_ptr<array<string, 4>>&, const string*>);

    const auto sp_arr = make_shared<array<string, 4>>();
    string* p         = sp_arr->data();
    const string* c   = p;

    shared_ptr<string[]> sp0(sp_arr, p);
    shared_ptr<string[4]> sp1(sp_arr, p);
    shared_ptr<const string[]> sp2(sp_arr, c);
    shared_ptr<const string[4]> sp3(sp_arr, c);

    assert(sp0.use_count() == 5);
    assert(sp1.use_count() == 5);
    assert(sp2.use_count() == 5);
    assert(sp3.use_count() == 5);

    assert(sp0.get() == p);
    assert(sp1.get() == p);
    assert(sp2.get() == c);
    assert(sp3.get() == c);
}

template <typename Dest, typename Src>
void impl_shared_ptr_all_copy_ctors() {
    const shared_ptr<Src> orig(new string[4]);
    shared_ptr<Dest> cpy = orig;
    assert(cpy.use_count() == 2);
    assert(cpy.get() == orig.get());

    const shared_ptr<Src> empty;
    shared_ptr<Dest> emp2 = empty;
    assert_empty_null(emp2);
}

void test_shared_ptr_all_copy_ctors() {
    STATIC_ASSERT(is_nothrow_constructible_v<shared_ptr<string[]>,
        const shared_ptr<string[]>&>); // GOOD: same (plain, not converting)
    STATIC_ASSERT(is_nothrow_constructible_v<shared_ptr<string[]>,
        const shared_ptr<string[4]>&>); // GOOD: known-to-unknown is compatible
    STATIC_ASSERT(!is_constructible_v<shared_ptr<string[]>, const shared_ptr<const string[]>&>); // BAD: drops const
    STATIC_ASSERT(!is_constructible_v<shared_ptr<string[]>, const shared_ptr<const string[4]>&>); // BAD: drops const

    STATIC_ASSERT(
        !is_constructible_v<shared_ptr<string[4]>, const shared_ptr<string[]>&>); // BAD: unknown-to-known isn't allowed
    STATIC_ASSERT(is_nothrow_constructible_v<shared_ptr<string[4]>,
        const shared_ptr<string[4]>&>); // GOOD: same (plain, not converting)
    STATIC_ASSERT(!is_constructible_v<shared_ptr<string[4]>, const shared_ptr<const string[]>&>); // BAD: drops const
    STATIC_ASSERT(!is_constructible_v<shared_ptr<string[4]>, const shared_ptr<const string[4]>&>); // BAD: drops const

    STATIC_ASSERT(
        is_nothrow_constructible_v<shared_ptr<const string[]>, const shared_ptr<string[]>&>); // GOOD: adds const
    STATIC_ASSERT(is_nothrow_constructible_v<shared_ptr<const string[]>,
        const shared_ptr<string[4]>&>); // GOOD: adds const, known-to-unknown is compatible
    STATIC_ASSERT(is_nothrow_constructible_v<shared_ptr<const string[]>,
        const shared_ptr<const string[]>&>); // GOOD: same (plain, not converting)
    STATIC_ASSERT(is_nothrow_constructible_v<shared_ptr<const string[]>,
        const shared_ptr<const string[4]>&>); // GOOD: known-to-unknown is compatible

    STATIC_ASSERT(!is_constructible_v<shared_ptr<const string[4]>,
                  const shared_ptr<string[]>&>); // BAD: unknown-to-known isn't allowed
    STATIC_ASSERT(
        is_nothrow_constructible_v<shared_ptr<const string[4]>, const shared_ptr<string[4]>&>); // GOOD: adds const
    STATIC_ASSERT(!is_constructible_v<shared_ptr<const string[4]>,
                  const shared_ptr<const string[]>&>); // BAD: unknown-to-known isn't allowed
    STATIC_ASSERT(is_nothrow_constructible_v<shared_ptr<const string[4]>,
        const shared_ptr<const string[4]>&>); // GOOD: same (plain, not converting)

    impl_shared_ptr_all_copy_ctors<string[], string[]>();
    impl_shared_ptr_all_copy_ctors<string[], string[4]>();
    impl_shared_ptr_all_copy_ctors<string[4], string[4]>();
    impl_shared_ptr_all_copy_ctors<const string[], string[]>();
    impl_shared_ptr_all_copy_ctors<const string[], string[4]>();
    impl_shared_ptr_all_copy_ctors<const string[], const string[]>();
    impl_shared_ptr_all_copy_ctors<const string[], const string[4]>();
    impl_shared_ptr_all_copy_ctors<const string[4], string[4]>();
    impl_shared_ptr_all_copy_ctors<const string[4], const string[4]>();
}

template <typename Dest, typename Src>
void impl_shared_ptr_all_move_ctors() {
    string* p = new string[4];
    shared_ptr<Src> orig(p);
    shared_ptr<Dest> mov = move(orig);
    assert_empty_null(orig);
    assert(mov.use_count() == 1);
    assert(mov.get() == p);

    shared_ptr<Src> empty;
    shared_ptr<Dest> emp2 = move(empty);
    assert_empty_null(empty);
    assert_empty_null(emp2);
}

void test_shared_ptr_all_move_ctors() {
    STATIC_ASSERT(
        is_nothrow_constructible_v<shared_ptr<string[]>, shared_ptr<string[]>>); // GOOD: same (plain, not converting)
    STATIC_ASSERT(is_nothrow_constructible_v<shared_ptr<string[]>,
        shared_ptr<string[4]>>); // GOOD: known-to-unknown is compatible
    STATIC_ASSERT(!is_constructible_v<shared_ptr<string[]>, shared_ptr<const string[]>>); // BAD: drops const
    STATIC_ASSERT(!is_constructible_v<shared_ptr<string[]>, shared_ptr<const string[4]>>); // BAD: drops const

    STATIC_ASSERT(
        !is_constructible_v<shared_ptr<string[4]>, shared_ptr<string[]>>); // BAD: unknown-to-known isn't allowed
    STATIC_ASSERT(
        is_nothrow_constructible_v<shared_ptr<string[4]>, shared_ptr<string[4]>>); // GOOD: same (plain, not converting)
    STATIC_ASSERT(!is_constructible_v<shared_ptr<string[4]>, shared_ptr<const string[]>>); // BAD: drops const
    STATIC_ASSERT(!is_constructible_v<shared_ptr<string[4]>, shared_ptr<const string[4]>>); // BAD: drops const

    STATIC_ASSERT(is_nothrow_constructible_v<shared_ptr<const string[]>, shared_ptr<string[]>>); // GOOD: adds const
    STATIC_ASSERT(is_nothrow_constructible_v<shared_ptr<const string[]>,
        shared_ptr<string[4]>>); // GOOD: adds const, known-to-unknown is compatible
    STATIC_ASSERT(is_nothrow_constructible_v<shared_ptr<const string[]>,
        shared_ptr<const string[]>>); // GOOD: same (plain, not converting)
    STATIC_ASSERT(is_nothrow_constructible_v<shared_ptr<const string[]>,
        shared_ptr<const string[4]>>); // GOOD: known-to-unknown is compatible

    STATIC_ASSERT(
        !is_constructible_v<shared_ptr<const string[4]>, shared_ptr<string[]>>); // BAD: unknown-to-known isn't allowed
    STATIC_ASSERT(is_nothrow_constructible_v<shared_ptr<const string[4]>, shared_ptr<string[4]>>); // GOOD: adds const
    STATIC_ASSERT(!is_constructible_v<shared_ptr<const string[4]>,
                  shared_ptr<const string[]>>); // BAD: unknown-to-known isn't allowed
    STATIC_ASSERT(is_nothrow_constructible_v<shared_ptr<const string[4]>,
        shared_ptr<const string[4]>>); // GOOD: same (plain, not converting)

    impl_shared_ptr_all_move_ctors<string[], string[]>();
    impl_shared_ptr_all_move_ctors<string[], string[4]>();
    impl_shared_ptr_all_move_ctors<string[4], string[4]>();
    impl_shared_ptr_all_move_ctors<const string[], string[]>();
    impl_shared_ptr_all_move_ctors<const string[], string[4]>();
    impl_shared_ptr_all_move_ctors<const string[], const string[]>();
    impl_shared_ptr_all_move_ctors<const string[], const string[4]>();
    impl_shared_ptr_all_move_ctors<const string[4], string[4]>();
    impl_shared_ptr_all_move_ctors<const string[4], const string[4]>();
}

template <typename Dest, typename Src>
void impl_shared_ptr_weak_ctor() {
    const shared_ptr<Src> orig(new string[4]);
    const weak_ptr<Src> weak = orig;
    shared_ptr<Dest> sp(weak);
    assert(sp.use_count() == 2);
    assert(sp.get() == orig.get());

    bool caught = false;
    try {
        const weak_ptr<Src> empty;
        shared_ptr<Dest> emp2(empty);
    } catch (const bad_weak_ptr&) {
        caught = true;
    }
    assert(caught);
}

void test_shared_ptr_weak_ctor() {
    STATIC_ASSERT(is_constructible_v<shared_ptr<string[]>, const weak_ptr<string[]>&>); // GOOD: same
    STATIC_ASSERT(
        is_constructible_v<shared_ptr<string[]>, const weak_ptr<string[4]>&>); // GOOD: known-to-unknown is compatible
    STATIC_ASSERT(!is_constructible_v<shared_ptr<string[]>, const weak_ptr<const string[]>&>); // BAD: drops const
    STATIC_ASSERT(!is_constructible_v<shared_ptr<string[]>, const weak_ptr<const string[4]>&>); // BAD: drops const

    STATIC_ASSERT(
        !is_constructible_v<shared_ptr<string[4]>, const weak_ptr<string[]>&>); // BAD: unknown-to-known isn't allowed
    STATIC_ASSERT(is_constructible_v<shared_ptr<string[4]>, const weak_ptr<string[4]>&>); // GOOD: same
    STATIC_ASSERT(!is_constructible_v<shared_ptr<string[4]>, const weak_ptr<const string[]>&>); // BAD: drops const
    STATIC_ASSERT(!is_constructible_v<shared_ptr<string[4]>, const weak_ptr<const string[4]>&>); // BAD: drops const

    STATIC_ASSERT(is_constructible_v<shared_ptr<const string[]>, const weak_ptr<string[]>&>); // GOOD: adds const
    STATIC_ASSERT(is_constructible_v<shared_ptr<const string[]>,
        const weak_ptr<string[4]>&>); // GOOD: adds const, known-to-unknown is compatible
    STATIC_ASSERT(is_constructible_v<shared_ptr<const string[]>, const weak_ptr<const string[]>&>); // GOOD: same
    STATIC_ASSERT(is_constructible_v<shared_ptr<const string[]>,
        const weak_ptr<const string[4]>&>); // GOOD: known-to-unknown is compatible

    STATIC_ASSERT(!is_constructible_v<shared_ptr<const string[4]>,
                  const weak_ptr<string[]>&>); // BAD: unknown-to-known isn't allowed
    STATIC_ASSERT(is_constructible_v<shared_ptr<const string[4]>, const weak_ptr<string[4]>&>); // GOOD: adds const
    STATIC_ASSERT(!is_constructible_v<shared_ptr<const string[4]>,
                  const weak_ptr<const string[]>&>); // BAD: unknown-to-known isn't allowed
    STATIC_ASSERT(is_constructible_v<shared_ptr<const string[4]>, const weak_ptr<const string[4]>&>); // GOOD: same

    impl_shared_ptr_weak_ctor<string[], string[]>();
    impl_shared_ptr_weak_ctor<string[], string[4]>();
    impl_shared_ptr_weak_ctor<string[4], string[4]>();
    impl_shared_ptr_weak_ctor<const string[], string[]>();
    impl_shared_ptr_weak_ctor<const string[], string[4]>();
    impl_shared_ptr_weak_ctor<const string[], const string[]>();
    impl_shared_ptr_weak_ctor<const string[], const string[4]>();
    impl_shared_ptr_weak_ctor<const string[4], string[4]>();
    impl_shared_ptr_weak_ctor<const string[4], const string[4]>();
}

template <typename Dest, typename Src, typename DelType>
void impl2_shared_ptr_unique_ctor() {
    default_delete<Src> deleter;

    string* p = new string[4];
    unique_ptr<Src, DelType> uniq(p, deleter);
    shared_ptr<Dest> sp = move(uniq);
    assert(!uniq);
    assert(sp.use_count() == 1);
    assert(sp.get() == p);

    unique_ptr<Src, DelType> empty(nullptr, deleter);
    shared_ptr<Dest> emp2 = move(empty);
    assert(!empty);
    assert_empty_null(emp2);
}

template <typename Dest, typename Src>
void impl_shared_ptr_unique_ctor() {
    impl2_shared_ptr_unique_ctor<Dest, Src, default_delete<Src>>();
    impl2_shared_ptr_unique_ctor<Dest, Src, default_delete<Src>&>();
    impl2_shared_ptr_unique_ctor<Dest, Src, const default_delete<Src>&>();
}

void test_shared_ptr_unique_ctor() {
    STATIC_ASSERT(is_constructible_v<shared_ptr<string[]>, unique_ptr<string[]>>); // GOOD: same
    STATIC_ASSERT(!is_constructible_v<shared_ptr<string[]>, unique_ptr<const string[]>>); // BAD: drops const

    STATIC_ASSERT(
        !is_constructible_v<shared_ptr<string[4]>, unique_ptr<string[]>>); // BAD: unknown-to-known isn't allowed
    STATIC_ASSERT(!is_constructible_v<shared_ptr<string[4]>, unique_ptr<const string[]>>); // BAD: drops const

    STATIC_ASSERT(is_constructible_v<shared_ptr<const string[]>, unique_ptr<string[]>>); // GOOD: adds const
    STATIC_ASSERT(is_constructible_v<shared_ptr<const string[]>, unique_ptr<const string[]>>); // GOOD: same

    STATIC_ASSERT(
        !is_constructible_v<shared_ptr<const string[4]>, unique_ptr<string[]>>); // BAD: unknown-to-known isn't allowed
    STATIC_ASSERT(!is_constructible_v<shared_ptr<const string[4]>,
                  unique_ptr<const string[]>>); // BAD: unknown-to-known isn't allowed

    impl_shared_ptr_unique_ctor<string[], string[]>();
    impl_shared_ptr_unique_ctor<const string[], string[]>();
    impl_shared_ptr_unique_ctor<const string[], const string[]>();
}

template <typename T>
void impl_shared_ptr_swap() {
#if _HAS_CXX17
    STATIC_ASSERT(is_nothrow_swappable_v<shared_ptr<T>>);
#endif // _HAS_CXX17

    string* p1 = new string[4];
    shared_ptr<T> left(p1);

    string* p2 = new string[4];
    shared_ptr<T> right(p2);

    assert(left.get() == p1);
    assert(right.get() == p2);

    left.swap(right);

    assert(left.get() == p2);
    assert(right.get() == p1);

    swap(left, right);

    assert(left.get() == p1);
    assert(right.get() == p2);
}

void test_shared_ptr_swap() {
    impl_shared_ptr_swap<string[]>();
    impl_shared_ptr_swap<string[4]>();
    impl_shared_ptr_swap<const string[]>();
    impl_shared_ptr_swap<const string[4]>();
}

void test_shared_ptr_assignment_and_reset() {
    // These operations are specified and implemented with construct-and-swap,
    // so they don't need to be exhaustively tested.

    string* p1 = new string[4];
    shared_ptr<const string[]> sp1(p1);
    string* p2 = new string[4];
    shared_ptr<string[]> sp2(p2);

    shared_ptr<const string[]> x;
    assert_empty_null(x);

    x = sp1;
    assert(x.use_count() == 2);
    assert(x.get() == p1);

    x = sp2;
    assert(x.use_count() == 2);
    assert(x.get() == p2);

    x = move(sp1);
    assert_empty_null(sp1);
    assert(x.use_count() == 1);
    assert(x.get() == p1);

    x = move(sp2);
    assert_empty_null(sp2);
    assert(x.use_count() == 1);
    assert(x.get() == p2);

    string* p3 = new string[4];
    unique_ptr<string[]> uniq(p3);
    x = move(uniq);
    assert(!uniq);
    assert(x.use_count() == 1);
    assert(x.get() == p3);

    x.reset();
    assert_empty_null(x);

    string* p4 = new string[4];
    x.reset(p4);
    assert(x.use_count() == 1);
    assert(x.get() == p4);

    using Del = default_delete<const string[]>;
    using Al  = allocator<int>;

    string* p5 = new string[4];
    x.reset(p5, Del{});
    assert(x.use_count() == 1);
    assert(x.get() == p5);

    string* p6 = new string[4];
    x.reset(p6, Del{}, Al{});
    assert(x.use_count() == 1);
    assert(x.get() == p6);
}

void test_shared_ptr_indexing() {
    // use_count() and get() have already been used extensively.

    const shared_ptr<string[]> sp(new string[4]);
    sp[0] = "cute";
    sp[1] = "fluffy";
    sp[2] = "kittens";
    sp[3] = "say MEOW!";
    assert(sp[2].size() == 7);
    assert(sp && sp[3].size() == 9);
}

void test_shared_ptr_owner_before() {
    const shared_ptr<string[]> sp(new string[4]);

    const shared_ptr<int> shared_other(new int(1729));
    const weak_ptr<int> weak_other = shared_other;
    assert(sp.owner_before(shared_other) || shared_other.owner_before(sp));
    assert(sp.owner_before(weak_other) || weak_other.owner_before(sp));

    const shared_ptr<string[]> empty;
    const shared_ptr<int> e1;
    const weak_ptr<int> e2;
    assert(!empty.owner_before(e1) && !e1.owner_before(empty));
    assert(!empty.owner_before(e2) && !e2.owner_before(empty));
}

void test_shared_ptr_comparisons() {
    const shared_ptr<string[]> arr(new string[4]);

    const shared_ptr<string[]> x(arr, arr.get() + 1);
    const shared_ptr<const string[2]> y(arr, arr.get() + 2);

    assert(!(x == y));
    assert(x != y);
    assert(x < y);
    assert(!(x > y));
    assert(x <= y);
    assert(!(x >= y));

    assert(!(x == nullptr));
    assert(!(nullptr == x));
    assert(x != nullptr);
    assert(nullptr != x);

    const shared_ptr<string[]> e;
    assert(e == nullptr);
    assert(nullptr == e);
    assert(!(e != nullptr));
    assert(!(nullptr != e));
    assert(!(e < nullptr));
    assert(!(nullptr < e));
    assert(!(e > nullptr));
    assert(!(nullptr > e));
    assert(e <= nullptr);
    assert(nullptr <= e);
    assert(e >= nullptr);
    assert(nullptr >= e);
}

void test_shared_ptr_casts() {
    // dynamic_pointer_cast() isn't really applicable to arrays.

    const shared_ptr<const string[]> orig(new string[4]);

    const shared_ptr<const void> spvoid  = orig;
    const shared_ptr<const string[]> sp1 = static_pointer_cast<const string[]>(spvoid);

    const shared_ptr<string[]> mod = const_pointer_cast<string[]>(orig);

    const shared_ptr<const char> spbytes = reinterpret_pointer_cast<const char>(orig);
    const shared_ptr<const string[]> sp2 = reinterpret_pointer_cast<const string[]>(spbytes);

    assert(orig.use_count() == 6);
    assert(spvoid.use_count() == 6);
    assert(sp1.use_count() == 6);
    assert(mod.use_count() == 6);
    assert(spbytes.use_count() == 6);
    assert(sp2.use_count() == 6);

    assert(spvoid.get() == orig.get());
    assert(sp1.get() == orig.get());
    assert(mod.get() == orig.get());
    assert(spbytes.get() == spvoid.get());
    assert(sp2.get() == orig.get());
}

void test_shared_ptr_get_deleter() {
    const shared_ptr<string[]> sp(new string[4], default_delete<string[]>{});

    assert(get_deleter<default_delete<string[]>>(sp) != nullptr);
    assert(get_deleter<default_delete<const string[]>>(sp) == nullptr);
}

void test_shared_ptr_streaming_and_hashing() {
    int* p = new int[4];
    const shared_ptr<int[]> sp(p);

    ostringstream oss1;
    oss1 << p;

    ostringstream oss2;
    oss2 << sp;

    assert(oss1.str() == oss2.str());

    assert(hash<shared_ptr<int[]>>{}(sp) == hash<int*>{}(p));
}

void test_weak_ptr_typedefs() {
    STATIC_ASSERT(is_same_v<weak_ptr<string[]>::element_type, string>);
    STATIC_ASSERT(is_same_v<weak_ptr<string[4]>::element_type, string>);
    STATIC_ASSERT(is_same_v<weak_ptr<const string[]>::element_type, const string>);
    STATIC_ASSERT(is_same_v<weak_ptr<const string[4]>::element_type, const string>);
}

void test_weak_ptr_default_ctor() {
    STATIC_ASSERT(is_nothrow_default_constructible_v<weak_ptr<string[]>>);
    STATIC_ASSERT(is_nothrow_default_constructible_v<weak_ptr<string[4]>>);

    weak_ptr<string[]> wp0;
    weak_ptr<string[4]> wp1;

    assert(wp0.use_count() == 0);
    assert(wp1.use_count() == 0);
}

template <typename Dest, typename Src>
void impl_weak_ptr_all_copy_ctors_and_shared_ctor() {
    const shared_ptr<Src> orig(new string[4]);
    const weak_ptr<Src> weak = orig;
    const weak_ptr<Dest> wp1 = orig;
    const weak_ptr<Dest> wp2 = weak;
    assert(orig.use_count() == 1);
    assert(weak.use_count() == 1);
    assert(wp1.use_count() == 1);
    assert(wp2.use_count() == 1);
    assert(weak.lock().get() == orig.get());
    assert(wp1.lock().get() == orig.get());
    assert(wp2.lock().get() == orig.get());

    const shared_ptr<Src> shared_empty;
    const weak_ptr<Src> weak_empty;
    const weak_ptr<Dest> emp1 = shared_empty;
    const weak_ptr<Dest> emp2 = weak_empty;
    assert(emp1.use_count() == 0);
    assert(emp2.use_count() == 0);
}

void test_weak_ptr_all_copy_ctors_and_shared_ctor() {
    STATIC_ASSERT(is_nothrow_constructible_v<weak_ptr<string[]>,
        const weak_ptr<string[]>&>); // GOOD: same (plain, not converting)
    STATIC_ASSERT(is_nothrow_constructible_v<weak_ptr<string[]>,
        const weak_ptr<string[4]>&>); // GOOD: known-to-unknown is compatible
    STATIC_ASSERT(!is_constructible_v<weak_ptr<string[]>, const weak_ptr<const string[]>&>); // BAD: drops const
    STATIC_ASSERT(!is_constructible_v<weak_ptr<string[]>, const weak_ptr<const string[4]>&>); // BAD: drops const

    STATIC_ASSERT(
        !is_constructible_v<weak_ptr<string[4]>, const weak_ptr<string[]>&>); // BAD: unknown-to-known isn't allowed
    STATIC_ASSERT(is_nothrow_constructible_v<weak_ptr<string[4]>,
        const weak_ptr<string[4]>&>); // GOOD: same (plain, not converting)
    STATIC_ASSERT(!is_constructible_v<weak_ptr<string[4]>, const weak_ptr<const string[]>&>); // BAD: drops const
    STATIC_ASSERT(!is_constructible_v<weak_ptr<string[4]>, const weak_ptr<const string[4]>&>); // BAD: drops const

    STATIC_ASSERT(is_nothrow_constructible_v<weak_ptr<const string[]>, const weak_ptr<string[]>&>); // GOOD: adds const
    STATIC_ASSERT(is_nothrow_constructible_v<weak_ptr<const string[]>,
        const weak_ptr<string[4]>&>); // GOOD: adds const, known-to-unknown is compatible
    STATIC_ASSERT(is_nothrow_constructible_v<weak_ptr<const string[]>,
        const weak_ptr<const string[]>&>); // GOOD: same (plain, not converting)
    STATIC_ASSERT(is_nothrow_constructible_v<weak_ptr<const string[]>,
        const weak_ptr<const string[4]>&>); // GOOD: known-to-unknown is compatible

    STATIC_ASSERT(!is_constructible_v<weak_ptr<const string[4]>,
                  const weak_ptr<string[]>&>); // BAD: unknown-to-known isn't allowed
    STATIC_ASSERT(
        is_nothrow_constructible_v<weak_ptr<const string[4]>, const weak_ptr<string[4]>&>); // GOOD: adds const
    STATIC_ASSERT(!is_constructible_v<weak_ptr<const string[4]>,
                  const weak_ptr<const string[]>&>); // BAD: unknown-to-known isn't allowed
    STATIC_ASSERT(is_nothrow_constructible_v<weak_ptr<const string[4]>,
        const weak_ptr<const string[4]>&>); // GOOD: same (plain, not converting)

    STATIC_ASSERT(is_nothrow_constructible_v<weak_ptr<string[]>, const shared_ptr<string[]>&>); // GOOD: same
    STATIC_ASSERT(is_nothrow_constructible_v<weak_ptr<string[]>,
        const shared_ptr<string[4]>&>); // GOOD: known-to-unknown is compatible
    STATIC_ASSERT(!is_constructible_v<weak_ptr<string[]>, const shared_ptr<const string[]>&>); // BAD: drops const
    STATIC_ASSERT(!is_constructible_v<weak_ptr<string[]>, const shared_ptr<const string[4]>&>); // BAD: drops const

    STATIC_ASSERT(
        !is_constructible_v<weak_ptr<string[4]>, const shared_ptr<string[]>&>); // BAD: unknown-to-known isn't allowed
    STATIC_ASSERT(is_nothrow_constructible_v<weak_ptr<string[4]>, const shared_ptr<string[4]>&>); // GOOD: same
    STATIC_ASSERT(!is_constructible_v<weak_ptr<string[4]>, const shared_ptr<const string[]>&>); // BAD: drops const
    STATIC_ASSERT(!is_constructible_v<weak_ptr<string[4]>, const shared_ptr<const string[4]>&>); // BAD: drops const

    STATIC_ASSERT(
        is_nothrow_constructible_v<weak_ptr<const string[]>, const shared_ptr<string[]>&>); // GOOD: adds const
    STATIC_ASSERT(is_nothrow_constructible_v<weak_ptr<const string[]>,
        const shared_ptr<string[4]>&>); // GOOD: adds const, known-to-unknown is compatible
    STATIC_ASSERT(
        is_nothrow_constructible_v<weak_ptr<const string[]>, const shared_ptr<const string[]>&>); // GOOD: same
    STATIC_ASSERT(is_nothrow_constructible_v<weak_ptr<const string[]>,
        const shared_ptr<const string[4]>&>); // GOOD: known-to-unknown is compatible

    STATIC_ASSERT(!is_constructible_v<weak_ptr<const string[4]>,
                  const shared_ptr<string[]>&>); // BAD: unknown-to-known isn't allowed
    STATIC_ASSERT(
        is_nothrow_constructible_v<weak_ptr<const string[4]>, const shared_ptr<string[4]>&>); // GOOD: adds const
    STATIC_ASSERT(!is_constructible_v<weak_ptr<const string[4]>,
                  const shared_ptr<const string[]>&>); // BAD: unknown-to-known isn't allowed
    STATIC_ASSERT(
        is_nothrow_constructible_v<weak_ptr<const string[4]>, const shared_ptr<const string[4]>&>); // GOOD: same

    impl_weak_ptr_all_copy_ctors_and_shared_ctor<string[], string[]>();
    impl_weak_ptr_all_copy_ctors_and_shared_ctor<string[], string[4]>();
    impl_weak_ptr_all_copy_ctors_and_shared_ctor<string[4], string[4]>();
    impl_weak_ptr_all_copy_ctors_and_shared_ctor<const string[], string[]>();
    impl_weak_ptr_all_copy_ctors_and_shared_ctor<const string[], string[4]>();
    impl_weak_ptr_all_copy_ctors_and_shared_ctor<const string[], const string[]>();
    impl_weak_ptr_all_copy_ctors_and_shared_ctor<const string[], const string[4]>();
    impl_weak_ptr_all_copy_ctors_and_shared_ctor<const string[4], string[4]>();
    impl_weak_ptr_all_copy_ctors_and_shared_ctor<const string[4], const string[4]>();
}

template <typename Dest, typename Src>
void impl_weak_ptr_all_move_ctors() {
    shared_ptr<Src> orig(new string[4]);
    weak_ptr<Src> weak = orig;
    weak_ptr<Dest> wp2 = move(weak);
    assert(weak.use_count() == 0);
    assert(wp2.use_count() == 1);
    assert(wp2.lock().get() == orig.get());

    weak_ptr<Src> empty;
    weak_ptr<Dest> emp2 = move(empty);
    assert(empty.use_count() == 0);
    assert(emp2.use_count() == 0);
}

void test_weak_ptr_all_move_ctors() {
    STATIC_ASSERT(
        is_nothrow_constructible_v<weak_ptr<string[]>, weak_ptr<string[]>>); // GOOD: same (plain, not converting)
    STATIC_ASSERT(
        is_nothrow_constructible_v<weak_ptr<string[]>, weak_ptr<string[4]>>); // GOOD: known-to-unknown is compatible
    STATIC_ASSERT(!is_constructible_v<weak_ptr<string[]>, weak_ptr<const string[]>>); // BAD: drops const
    STATIC_ASSERT(!is_constructible_v<weak_ptr<string[]>, weak_ptr<const string[4]>>); // BAD: drops const

    STATIC_ASSERT(!is_constructible_v<weak_ptr<string[4]>, weak_ptr<string[]>>); // BAD: unknown-to-known isn't allowed
    STATIC_ASSERT(
        is_nothrow_constructible_v<weak_ptr<string[4]>, weak_ptr<string[4]>>); // GOOD: same (plain, not converting)
    STATIC_ASSERT(!is_constructible_v<weak_ptr<string[4]>, weak_ptr<const string[]>>); // BAD: drops const
    STATIC_ASSERT(!is_constructible_v<weak_ptr<string[4]>, weak_ptr<const string[4]>>); // BAD: drops const

    STATIC_ASSERT(is_nothrow_constructible_v<weak_ptr<const string[]>, weak_ptr<string[]>>); // GOOD: adds const
    STATIC_ASSERT(is_nothrow_constructible_v<weak_ptr<const string[]>,
        weak_ptr<string[4]>>); // GOOD: adds const, known-to-unknown is compatible
    STATIC_ASSERT(is_nothrow_constructible_v<weak_ptr<const string[]>,
        weak_ptr<const string[]>>); // GOOD: same (plain, not converting)
    STATIC_ASSERT(is_nothrow_constructible_v<weak_ptr<const string[]>,
        weak_ptr<const string[4]>>); // GOOD: known-to-unknown is compatible

    STATIC_ASSERT(
        !is_constructible_v<weak_ptr<const string[4]>, weak_ptr<string[]>>); // BAD: unknown-to-known isn't allowed
    STATIC_ASSERT(is_nothrow_constructible_v<weak_ptr<const string[4]>, weak_ptr<string[4]>>); // GOOD: adds const
    STATIC_ASSERT(!is_constructible_v<weak_ptr<const string[4]>,
                  weak_ptr<const string[]>>); // BAD: unknown-to-known isn't allowed
    STATIC_ASSERT(is_nothrow_constructible_v<weak_ptr<const string[4]>,
        weak_ptr<const string[4]>>); // GOOD: same (plain, not converting)

    impl_weak_ptr_all_move_ctors<string[], string[]>();
    impl_weak_ptr_all_move_ctors<string[], string[4]>();
    impl_weak_ptr_all_move_ctors<string[4], string[4]>();
    impl_weak_ptr_all_move_ctors<const string[], string[]>();
    impl_weak_ptr_all_move_ctors<const string[], string[4]>();
    impl_weak_ptr_all_move_ctors<const string[], const string[]>();
    impl_weak_ptr_all_move_ctors<const string[], const string[4]>();
    impl_weak_ptr_all_move_ctors<const string[4], string[4]>();
    impl_weak_ptr_all_move_ctors<const string[4], const string[4]>();
}

template <typename T>
void impl_weak_ptr_swap() {
#if _HAS_CXX17
    STATIC_ASSERT(is_nothrow_swappable_v<weak_ptr<T>>);
#endif // _HAS_CXX17

    const shared_ptr<T> sp1(new string[4]);
    weak_ptr<T> left = sp1;

    const shared_ptr<T> sp2(new string[4]);
    weak_ptr<T> right = sp2;

    assert(left.lock().get() == sp1.get());
    assert(right.lock().get() == sp2.get());

    left.swap(right);

    assert(left.lock().get() == sp2.get());
    assert(right.lock().get() == sp1.get());

    swap(left, right);

    assert(left.lock().get() == sp1.get());
    assert(right.lock().get() == sp2.get());
}

void test_weak_ptr_swap() {
    impl_weak_ptr_swap<string[]>();
    impl_weak_ptr_swap<string[4]>();
    impl_weak_ptr_swap<const string[]>();
    impl_weak_ptr_swap<const string[4]>();
}

void test_weak_ptr_assignment_and_reset() {
    // These operations are specified and implemented with construct-and-swap,
    // so they don't need to be exhaustively tested.

    const shared_ptr<string[]> sp1(new string[4]);
    weak_ptr<string[]> wp1 = sp1;

    const shared_ptr<const string[]> sp2(new string[4]);
    weak_ptr<const string[]> wp2 = sp2;

    weak_ptr<const string[]> weak;

    weak = wp2;
    assert(weak.lock().get() == sp2.get());

    weak = wp1;
    assert(weak.lock().get() == sp1.get());

    weak = move(wp2);
    assert(wp2.use_count() == 0);
    assert(weak.lock().get() == sp2.get());

    weak = move(wp1);
    assert(wp1.use_count() == 0);
    assert(weak.lock().get() == sp1.get());

    weak = sp2;
    assert(weak.lock().get() == sp2.get());

    weak.reset();
    assert(weak.use_count() == 0);
}

void test_weak_ptr_observers() {
    shared_ptr<string[]> sp1(new string[4]);
    shared_ptr<string[]> sp2 = sp1;
    sp1[3]                   = "Peppermint";

    weak_ptr<string[]> weak = sp1;
    assert(weak.use_count() == 2);
    assert(!weak.expired());
    assert(weak.lock()[3].size() == 10);

    sp1.reset();
    assert(weak.use_count() == 1);
    assert(!weak.expired());
    assert(weak.lock()[3].size() == 10);

    sp2.reset();
    assert(weak.use_count() == 0);
    assert(weak.expired());
    assert_empty_null(weak.lock());

    weak_ptr<string[]> weak2 = weak;
    assert(weak.owner_before(sp1) || sp1.owner_before(weak));
    assert(!weak.owner_before(weak2) && !weak2.owner_before(weak));
}

struct ESFT : enable_shared_from_this<ESFT> {
    ESFT() : m_n(-1) {}
    explicit ESFT(const int n) : m_n(n) {}
    int m_n;
};

void test_enable_shared_from_this() {
    shared_ptr<ESFT> sp1(new ESFT(1729));
    ESFT* ptr            = sp1.get();
    weak_ptr<ESFT> weak  = ptr->weak_from_this();
    shared_ptr<ESFT> sp2 = weak.lock();
    shared_ptr<ESFT> sp3 = ptr->shared_from_this();
    assert(sp1.use_count() == 3);
    assert(sp2.use_count() == 3);
    assert(sp3.use_count() == 3);
    assert(sp1->m_n == 1729);
    assert(sp2->m_n == 1729);
    assert(sp3->m_n == 1729);

    shared_ptr<ESFT[]> arr(new ESFT[4]);
    ESFT* first = arr.get();
    for (int i = 0; i < 4; ++i) {
        assert(first[i].m_n == -1);
        assert(first[i].weak_from_this().expired());
    }
}

struct BaseX {
    int x = 11;

    BaseX()          = default;
    virtual ~BaseX() = default;

    BaseX(const BaseX&)            = delete;
    BaseX& operator=(const BaseX&) = delete;
};

struct BaseY {
    int y = 22;

    BaseY()          = default;
    virtual ~BaseY() = default;

    BaseY(const BaseY&)            = delete;
    BaseY& operator=(const BaseY&) = delete;
};

struct DerivedZ : BaseX, BaseY {
    int z = 33;
};

// LWG-2996 "Missing rvalue overloads for shared_ptr operations"
void test_LWG_2996() {
    shared_ptr<DerivedZ> sp1 = make_shared<DerivedZ>();

    BaseX* const px    = sp1.get();
    BaseY* const py    = sp1.get();
    DerivedZ* const pz = sp1.get();
    assert(px->x == 11);
    assert(py->y == 22);
    assert(pz->z == 33);

    assert(sp1.use_count() == 1);
    assert(sp1.get() == pz);

    shared_ptr<const BaseX> sp2(move(sp1)); // move converting ctor, old
    assert(sp1.use_count() == 0);
    assert(sp1.get() == nullptr);
    assert(sp2.use_count() == 1);
    assert(sp2.get() == px);

    shared_ptr<BaseX> sp3 = const_pointer_cast<BaseX>(move(sp2)); // added by LWG-2996
    assert(sp2.use_count() == 0);
    assert(sp2.get() == nullptr);
    assert(sp3.use_count() == 1);
    assert(sp3.get() == px);

    shared_ptr<BaseY> sp4 = dynamic_pointer_cast<BaseY>(move(sp3)); // added by LWG-2996
    assert(sp3.use_count() == 0);
    assert(sp3.get() == nullptr);
    assert(sp4.use_count() == 1);
    assert(sp4.get() == py);

    shared_ptr<DerivedZ> sp5 = static_pointer_cast<DerivedZ>(move(sp4)); // added by LWG-2996
    assert(sp4.use_count() == 0);
    assert(sp4.get() == nullptr);
    assert(sp5.use_count() == 1);
    assert(sp5.get() == pz);

    shared_ptr<char> sp6 = reinterpret_pointer_cast<char>(move(sp5)); // added by LWG-2996
    assert(sp5.use_count() == 0);
    assert(sp5.get() == nullptr);
    assert(sp6.use_count() == 1);
    assert(sp6.get() == reinterpret_cast<char*>(pz));

    shared_ptr<int> sp7(move(sp6), &py->y); // aliasing move ctor, added by LWG-2996
    assert(sp6.use_count() == 0);
    assert(sp6.get() == nullptr);
    assert(sp7.use_count() == 1);
    assert(sp7.get() == &py->y);

    assert(px->x == 11);
    assert(py->y == 22);
    assert(pz->z == 33);
}

int main() {
    test_shared_ptr_typedefs();
    test_shared_ptr_default_ctor();
    test_shared_ptr_nullptr_ctor();
    test_shared_ptr_rawptr_ctor();
    test_shared_ptr_rawptr_del_ctor();
    test_shared_ptr_rawptr_del_al_ctor();
    test_shared_ptr_nullptr_del_ctor();
    test_shared_ptr_nullptr_del_al_ctor();
    test_shared_ptr_aliasing_ctor();
    test_shared_ptr_all_copy_ctors();
    test_shared_ptr_all_move_ctors();
    test_shared_ptr_weak_ctor();
    test_shared_ptr_unique_ctor();
    test_shared_ptr_swap();
    test_shared_ptr_assignment_and_reset();
    test_shared_ptr_indexing();
    test_shared_ptr_owner_before();
    test_shared_ptr_comparisons();
    test_shared_ptr_casts();
    test_shared_ptr_get_deleter();
    test_shared_ptr_streaming_and_hashing();
    test_weak_ptr_typedefs();
    test_weak_ptr_default_ctor();
    test_weak_ptr_all_copy_ctors_and_shared_ctor();
    test_weak_ptr_all_move_ctors();
    test_weak_ptr_swap();
    test_weak_ptr_assignment_and_reset();
    test_weak_ptr_observers();
    test_enable_shared_from_this();
    test_LWG_2996();
}
