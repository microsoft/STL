// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstdlib>
#include <functional>
#include <new>
#include <stdexcept>
#include <type_traits>
#include <utility>

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
    char data[large_function_size] = {};

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
    char data[large_function_size] = {};

    using pfn = int (*)(int a, pass_this_by_ref& b);

    operator pfn() {
        return [](int a, pass_this_by_ref& b) {
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

        test_function_t move_constructed = move(constructed_directly);

        assert(move_constructed(23, x) == expect);

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
        if constexpr (is_class_v<F>) {
            assert(counter::copies == 0);
            assert(counter::moves == 0);
        }
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
        f2 = move(f1);
        assert(f2(23, x) == 38);
        f1 = large_callable{};
        assert(f1(23, x) == 39);
    }

    {
        test_function_t f1{large_callable{}};
        test_function_t f2{small_callable{}};
        f2 = move(f1);
        assert(f2(23, x) == 39);
        f1 = small_callable{};
        assert(f1(23, x) == 38);
    }

    {
        test_function_t f1{small_callable{}};
        test_function_t f2{odd_cc_callable{}};
        f2 = move(f1);
        assert(f2(23, x) == 38);
        f1 = odd_cc_callable{};
        assert(f1(23, x) == 40);
    }

    {
        test_function_t f1{large_callable{}};
        test_function_t f2{large_implicit_ptr_callable{}};
        f2 = move(f1);
        assert(f2(23, x) == 39);
        f1 = large_implicit_ptr_callable{};
        assert(f1(23, x) == 41);
    }

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-move"
#endif // __clang__
    {
        test_function_t f1{small_callable{}};
        test_function_t f2{large_callable{}};
        f1 = move(f1); // deliberate self-move as a test case
#pragma warning(suppress : 26800) // use a moved-from object
        assert(f1(23, x) == 38);
        f2 = move(f2); // deliberate self-move as a test case
#pragma warning(suppress : 26800) // use a moved-from object
        assert(f2(23, x) == 39);
    }
#ifdef __clang__
#pragma clang diagnostic pop
#endif // __clang__

    {
        test_function_t f1{small_callable{}};
        test_function_t f2{large_callable{}};
        test_function_t f3{small_callable{}};
        test_function_t f4{large_callable{}};
        test_function_t f5{nullptr};
        assert(f1);
        assert(f2);
        assert(f3);
        assert(f4);
        assert(!f5);
        f1 = nullptr;
        f2 = nullptr;
        f3 = test_function_t{nullptr};
        f4 = test_function_t{nullptr};
        assert(!f1);
        assert(!f2);
        assert(!f3);
        assert(!f4);
    }
}

void test_swap() {
    pass_this_by_ref x{63};

    {
        test_function_t f1{small_callable{}};
        test_function_t f2{large_callable{}};
        swap(f1, f2);
        assert(f2(23, x) == 38);
        assert(f1(23, x) == 39);
    }

    {
        test_function_t f1{small_callable{}};
        test_function_t f2{odd_cc_callable{}};
        f1.swap(f2);
        assert(f2(23, x) == 38);
        assert(f1(23, x) == 40);
    }

    {
        test_function_t f1{large_callable{}};
        test_function_t f2{large_implicit_ptr_callable{}};
        f2.swap(f1);
        assert(f2(23, x) == 39);
        assert(f1(23, x) == 41);
    }

    {
        test_function_t f1{small_callable{}};
        test_function_t f2{large_callable{}};
        swap(f1, f1);
        f2.swap(f2);
        assert(f1(23, x) == 38);
        assert(f2(23, x) == 39);
    }
}

void test_empty() {
    test_function_t no_callable;
    assert(!no_callable);
    assert(no_callable == nullptr);
    assert(nullptr == no_callable);

    test_function_t no_callable_moved = move(no_callable);
#pragma warning(suppress : 26800) // use a moved-from object
    assert(!no_callable);
    assert(no_callable == nullptr);
    assert(!no_callable_moved);
    assert(no_callable_moved == nullptr);
}

void test_ptr() {
    struct s_t {
        int f(int p) {
            return p + 2;
        }

        int j = 6;

        static int g(int z) {
            return z - 3;
        }
    };

    move_only_function<int(s_t*, int)> mem_fun_ptr(&s_t::f);
    move_only_function<int(s_t*)> mem_ptr(&s_t::j);
    move_only_function<int(int)> fun_ptr(&s_t::g);

    s_t s;
    assert(mem_fun_ptr);
    assert(mem_fun_ptr(&s, 3) == 5);
    assert(mem_ptr);
    assert(mem_ptr(&s) == 6);
    assert(fun_ptr);
    assert(fun_ptr(34) == 31);

    move_only_function<int(s_t*, int)> mem_fun_ptr_n(static_cast<decltype(&s_t::f)>(nullptr));
    move_only_function<int(s_t*)> mem_ptr_n(static_cast<decltype(&s_t::j)>(nullptr));
    move_only_function<int(int)> fun_ptr_n(static_cast<decltype(&s_t::g)>(nullptr));

    assert(!mem_fun_ptr_n);
    assert(!mem_ptr_n);
    assert(!fun_ptr_n);
}

void test_inner() {
    move_only_function<short(long, long)> f1(nullptr);
    move_only_function<int(int, int)> f2 = move(f1);
    assert(!f2);
#pragma warning(suppress : 26800) // use a moved-from object
    f2 = move(f1);
    assert(!f1);
}


void test_inplace_list() {
    struct in_place_list_constructible {
        in_place_list_constructible(initializer_list<int> li) {
            int x = 0;
            for (int i : li) {
                ++x;
                assert(x == i);
            }
        }

        in_place_list_constructible(initializer_list<int> li, const char*) {
            int x = 0;
            for (int i : li) {
                --x;
                assert(x == i);
            }
        }

        in_place_list_constructible(const in_place_list_constructible&)            = delete;
        in_place_list_constructible& operator=(const in_place_list_constructible&) = delete;

        int operator()(int i) {
            return i - 1;
        }
    };

    move_only_function<int(int)> f1(in_place_type<in_place_list_constructible>, {1, 2, 3, 4, 5});
    assert(f1(5) == 4);

    move_only_function<int(int)> f2(in_place_type<in_place_list_constructible>, {-1, -2, -3, -4, -5}, "fox");
    assert(f2(8) == 7);
}


template <bool Nx>
struct test_noexcept_t {
    int operator()() noexcept(Nx) {
        return 888;
    }
};

void test_noexcept() {
    using f_x  = move_only_function<int()>;
    using f_nx = move_only_function<int() noexcept>;

    static_assert(!noexcept(declval<f_x>()()));
#ifdef __cpp_noexcept_function_type
    static_assert(noexcept(declval<f_nx>()()));
#else // ^^^ defined(__cpp_noexcept_function_type) ^^^ / vvv !defined(__cpp_noexcept_function_type) vvv
    static_assert(!noexcept(declval<f_nx>()()));
#endif // ^^^ !defined(__cpp_noexcept_function_type) ^^^

    static_assert(is_constructible_v<f_x, test_noexcept_t<false>>);
    assert(f_x(test_noexcept_t<false>{})() == 888);

    static_assert(is_constructible_v<f_x, test_noexcept_t<true>>);
    assert(f_x(test_noexcept_t<true>{})() == 888);

#ifdef __cpp_noexcept_function_type
    static_assert(!is_constructible_v<f_nx, test_noexcept_t<false>>);
#else // ^^^ defined(__cpp_noexcept_function_type) ^^^ / vvv !defined(__cpp_noexcept_function_type) vvv
    static_assert(is_constructible_v<f_nx, test_noexcept_t<false>>);
    assert(f_nx(test_noexcept_t<false>{})() == 888);
#endif // ^^^ !defined(__cpp_noexcept_function_type) ^^^

    static_assert(is_constructible_v<f_nx, test_noexcept_t<true>>);
    assert(f_nx(test_noexcept_t<true>{})() == 888);
}

template <bool>
struct test_const_t {
    int operator()() {
        return 456;
    }
};

template <>
struct test_const_t<true> {
    int operator()() const {
        return 456;
    }
};

void test_const() {
    using f_c  = move_only_function<int() const>;
    using f_nc = move_only_function<int()>;

    static_assert(is_constructible_v<f_nc, test_const_t<false>>);
    f_nc f1(test_const_t<false>{});
    assert(f1() == 456);

    static_assert(is_constructible_v<f_nc, test_const_t<true>>);
    f_nc f2(test_const_t<true>{});
    assert(f2() == 456);

    static_assert(!is_constructible_v<f_c, test_const_t<false>>);

    static_assert(is_constructible_v<f_c, test_const_t<true>>);
    f_c f3(test_const_t<true>{});
    assert(f3() == 456);
    const f_c f4(test_const_t<true>{});
    assert(f4() == 456);
}


void test_qual() {
    move_only_function<int(int)> f1([](auto i) { return i + 1; });
    assert(f1(1) == 2);
    move_only_function<int(int)&> f2([](auto i) { return i + 1; });
    assert(f2(2) == 3);
    move_only_function<int(int) &&> f3([](auto i) { return i + 1; });
    assert(move(f3)(3) == 4);

    move_only_function<int(int) const> f1c([](auto i) { return i + 1; });
    assert(f1c(4) == 5);
    move_only_function<int(int) const&> f2c([](auto i) { return i + 1; });
    assert(f2c(5) == 6);
    move_only_function<int(int) const&&> f3c([](auto i) { return i + 1; });
    assert(move(f3c)(6) == 7);

    move_only_function<int(int) noexcept> f1_nx([](auto i) noexcept { return i + 1; });
    assert(f1_nx(1) == 2);
    move_only_function<int(int)& noexcept> f2_nx([](auto i) noexcept { return i + 1; });
    assert(f2_nx(2) == 3);
    move_only_function<int(int)&& noexcept> f3_nx([](auto i) noexcept { return i + 1; });
    assert(move(f3_nx)(3) == 4);

    move_only_function<int(int) const noexcept> f1c_nx([](auto i) noexcept { return i + 1; });
    assert(f1c_nx(4) == 5);
    move_only_function<int(int) const& noexcept> f2c_nx([](auto i) noexcept { return i + 1; });
    assert(f2c_nx(5) == 6);
    move_only_function<int(int) const&& noexcept> f3c_nx([](auto i) noexcept { return i + 1; });
    assert(move(f3c_nx)(6) == 7);
}

static_assert(is_same_v<move_only_function<void()>::result_type, void>);
static_assert(is_same_v<move_only_function<short(long&) &>::result_type, short>);
static_assert(is_same_v<move_only_function<int(char*) &&>::result_type, int>);
static_assert(is_same_v<move_only_function<void() const>::result_type, void>);
static_assert(is_same_v<move_only_function<short(long&) const&>::result_type, short>);
static_assert(is_same_v<move_only_function<int(char*) const&&>::result_type, int>);

#ifdef __cpp_noexcept_function_type
static_assert(is_same_v<move_only_function<void() noexcept>::result_type, void>);
static_assert(is_same_v<move_only_function<short(long&) & noexcept>::result_type, short>);
static_assert(is_same_v<move_only_function<int(char*) && noexcept>::result_type, int>);
static_assert(is_same_v<move_only_function<void() const noexcept>::result_type, void>);
static_assert(is_same_v<move_only_function<short(long&) const& noexcept>::result_type, short>);
static_assert(is_same_v<move_only_function<int(char*) const&& noexcept>::result_type, int>);
#endif // ^^^ defined(__cpp_noexcept_function_type) ^^^

bool fail_allocations = false;

#pragma warning(suppress : 28251) // Inconsistent annotation for 'new': this instance has no annotations.
void* operator new(size_t size) {
    if (fail_allocations) {
        throw bad_alloc{};
    }
    void* result = size > 0 ? malloc(size) : malloc(1);
    if (!result) {
        throw bad_alloc{};
    }
    return result;
}

void operator delete(void* p) noexcept {
    free(p);
}

void test_except() {
    struct throwing {
        throwing() = default;
        throwing(throwing&&) { // not noexcept to avoid small functor optimization
            throw runtime_error{"boo"};
        }
        void operator()() {}
    };

    struct not_throwing {
        not_throwing() = default;
        not_throwing(not_throwing&&) {} // not noexcept to avoid small functor optimization
        void operator()() {}
    };

    try {
        move_only_function<void()> f{throwing{}};
        assert(false); // unreachable
    } catch (runtime_error&) {
    }

    try {
        fail_allocations = true;
        move_only_function<void()> f{not_throwing{}};
        assert(false); // unreachable
    } catch (bad_alloc&) {
        fail_allocations = false;
    }
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
    test_ptr();
    test_inner();
    test_inplace_list();
    test_noexcept();
    test_const();
    test_qual();
    test_except();
}
