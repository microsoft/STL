// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _HAS_DEPRECATED_IS_LITERAL_TYPE 1
#define _HAS_DEPRECATED_RESULT_OF       1
#define _SILENCE_CXX17_IS_LITERAL_TYPE_DEPRECATION_WARNING
#define _SILENCE_CXX17_RESULT_OF_DEPRECATION_WARNING
#define _SILENCE_CXX20_IS_POD_DEPRECATION_WARNING
#define _SILENCE_CXX23_ALIGNED_UNION_DEPRECATION_WARNING
#define _USE_NAMED_IDL_NAMESPACE 1

#include <array>
#include <cstddef>
#include <cstdio>
#include <functional>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

// lets INSTANTIATE macro be used for types that aren't default constructible.
// Doesn't actually instantiate the type if not default constructible,
// but it will inspect the type, which is the true purpose.
template <typename T, bool = std::is_default_constructible_v<T>>
struct instantiate_helper;

template <typename T>
struct instantiate_helper<T, true> {
    using type = T;
};

template <typename T>
struct instantiate_helper<T, false> {
    using type = int;
};

#if _HAS_CXX17
// TRANSITION, Evil Extension
namespace detail {
    struct S {};
    void evil(S&);

    template <typename = S, typename = void>
    constexpr bool test = false;
    template <typename T>
    constexpr bool test<T, std::void_t<decltype(evil(T{}))>> = true;
} // namespace detail

constexpr bool has_evil_extension = detail::test<>;
#endif // _HAS_CXX17

#define USE_VALUE(...)          \
    do {                        \
        auto val = __VA_ARGS__; \
        (void) val;             \
    } while (0)
#define INSTANTIATE(...)                                      \
    do {                                                      \
        typename instantiate_helper<__VA_ARGS__>::type val{}; \
        (void) val;                                           \
    } while (0)
#define TRAIT_V(TRAIT_NAME, ...) USE_VALUE(TRAIT_NAME##_v<__VA_ARGS__>)

using namespace std;

template <typename T>
void enum_traits_test_impl(true_type) {
    INSTANTIATE(underlying_type_t<T>);
}

template <typename>
void enum_traits_test_impl(false_type) {}

template <typename T>
void integral_traits_test_impl(true_type) {
    INSTANTIATE(make_signed_t<T>);
    INSTANTIATE(make_unsigned_t<T>);
}

template <typename>
void integral_traits_test_impl(false_type) {}

template <typename T, typename U>
void constructible_traits_test_impl(true_type) {
    INSTANTIATE(aligned_union_t<4, T, U>);
    TRAIT_V(alignment_of, T);
}

template <typename, typename>
void constructible_traits_test_impl(false_type) {}

template <typename T>
void common_type_test_impl(true_type) {
    INSTANTIATE(common_type_t<T, T>);
}

template <typename>
void common_type_test_impl(false_type) {}

template <typename T, typename U = T>
void type_traits_test_impl() {
    TRAIT_V(is_void, T);
    INSTANTIATE(add_const_t<T>);
    INSTANTIATE(add_volatile_t<T>);
    INSTANTIATE(add_cv_t<T>);
    INSTANTIATE(add_lvalue_reference_t<T>);
    INSTANTIATE(add_rvalue_reference_t<T>);
    INSTANTIATE(remove_const_t<T>); // from xtr1common
    INSTANTIATE(remove_volatile_t<T>); // from xtr1common
    INSTANTIATE(remove_cv_t<T>); // from xtr1common
    INSTANTIATE(remove_reference_t<T>); // from xtr1common
    INSTANTIATE(remove_extent_t<T>);
    INSTANTIATE(remove_all_extents_t<T>);
    INSTANTIATE(remove_pointer_t<T>);
    INSTANTIATE(add_pointer_t<T>);
    TRAIT_V(is_array, T);
    TRAIT_V(is_lvalue_reference, T);
    TRAIT_V(is_rvalue_reference, T);
    TRAIT_V(is_reference, T);
    TRAIT_V(is_member_object_pointer, T);
    TRAIT_V(is_member_function_pointer, T);
    TRAIT_V(is_pointer, T);
    TRAIT_V(is_null_pointer, T);
    TRAIT_V(is_union, T);
    TRAIT_V(is_class, T);
    TRAIT_V(is_function, T); // from xstddef
    TRAIT_V(is_fundamental, T);
    TRAIT_V(is_arithmetic, T); // from xtr1common
    TRAIT_V(is_object, T);
    TRAIT_V(is_convertible, T, U);
    TRAIT_V(is_enum, T);
    TRAIT_V(is_compound, T);
    TRAIT_V(is_member_pointer, T);
    TRAIT_V(is_scalar, T);
    TRAIT_V(is_const, T);
    TRAIT_V(is_volatile, T);
    TRAIT_V(is_pod, T);
    TRAIT_V(is_abstract, T);
    TRAIT_V(is_final, T);
    TRAIT_V(is_standard_layout, T);
    TRAIT_V(is_literal_type, T);
    TRAIT_V(is_trivial, T);
    TRAIT_V(is_trivially_copyable, T);
    TRAIT_V(has_virtual_destructor, T);
    TRAIT_V(is_constructible, T, U);
    TRAIT_V(is_copy_constructible, T);
    TRAIT_V(is_default_constructible, T);
    TRAIT_V(is_move_constructible, T);
    TRAIT_V(is_assignable, T, U);
    TRAIT_V(is_copy_assignable, T);
    TRAIT_V(is_move_assignable, T);
    TRAIT_V(is_destructible, T);
    TRAIT_V(is_trivially_constructible, T, U);
    TRAIT_V(is_trivially_copy_constructible, T);
    TRAIT_V(is_trivially_default_constructible, T);
    TRAIT_V(is_trivially_move_constructible, T);
    TRAIT_V(is_trivially_assignable, T, U);
    TRAIT_V(is_trivially_destructible, T);
    TRAIT_V(is_nothrow_constructible, T, U);
    TRAIT_V(is_nothrow_copy_constructible, T);
    TRAIT_V(is_nothrow_default_constructible, T);
    TRAIT_V(is_nothrow_move_constructible, T);
    TRAIT_V(is_nothrow_assignable, T, U);
    TRAIT_V(is_nothrow_copy_assignable, T);
    TRAIT_V(is_nothrow_move_assignable, T);
    TRAIT_V(is_nothrow_destructible, T);
    TRAIT_V(is_integral, T); // from xtr1common
    TRAIT_V(is_floating_point, T); // from xtr1common
    TRAIT_V(is_signed, T);
    TRAIT_V(is_unsigned, T);
#if _HAS_CXX20
    TRAIT_V(is_bounded_array, T);
    TRAIT_V(is_unbounded_array, T);
#endif // _HAS_CXX20
#if _HAS_CXX23
    TRAIT_V(is_scoped_enum, T);
#endif // _HAS_CXX23
    TRAIT_V(rank, T);
    TRAIT_V(extent, T);
    TRAIT_V(is_same, T, U); // from xtr1common
    TRAIT_V(is_base_of, T, U);
    INSTANTIATE(decay_t<T>);
    TRAIT_V(conjunction, is_reference<T>, is_lvalue_reference<T>);
    TRAIT_V(disjunction, is_lvalue_reference<T>, is_reference<T>);
    TRAIT_V(negation, is_reference<T>);
#if _HAS_CXX17
    TRAIT_V(is_swappable, T);
    TRAIT_V(is_nothrow_swappable, T);
    TRAIT_V(is_swappable_with, T, U);
    constexpr bool avoid_evil_extension = // TRANSITION, Evil Extension
        has_evil_extension && !conjunction_v<is_lvalue_reference<T>, is_lvalue_reference<U>>;
    if constexpr (!avoid_evil_extension) { // avoid triggering Evil Extension warnings
        TRAIT_V(is_nothrow_swappable_with, T, U);
    }
#endif // _HAS_CXX17
#if _HAS_CXX20
    INSTANTIATE(type_identity_t<T>);
    INSTANTIATE(remove_cvref_t<T>); // from xtr1common
#endif // _HAS_CXX20

    enum_traits_test_impl<T>(is_enum<T>());
    integral_traits_test_impl<T>(is_integral<T>());
    constructible_traits_test_impl<T, U>(conjunction<is_constructible<T>, is_constructible<U>>());
    common_type_test_impl<T>(conjunction<negation<is_volatile<T>>, is_constructible<T>>());
}

template <typename T, typename U>
void type_traits_cv2_test_impl() {
    type_traits_test_impl<T, U>();
    type_traits_test_impl<T, add_const_t<U>>();
    type_traits_test_impl<T, add_volatile_t<U>>();
    type_traits_test_impl<T, add_cv_t<U>>();
}

template <typename T, typename U = T>
void type_traits_cv_test_impl() {
    type_traits_cv2_test_impl<T, U>();
    type_traits_cv2_test_impl<add_const_t<T>, U>();
    type_traits_cv2_test_impl<add_volatile_t<T>, U>();
    type_traits_cv2_test_impl<add_cv_t<T>, U>();
}

template <typename T, typename U>
void type_traits_ref2_test_impl() {
    type_traits_cv_test_impl<T, U>();
    type_traits_cv_test_impl<T, add_lvalue_reference_t<U>>();
    type_traits_cv_test_impl<T, add_rvalue_reference_t<U>>();
}

template <typename T, typename U = T>
void type_traits_ref_test_impl() {
    type_traits_ref2_test_impl<T, U>();
    type_traits_ref2_test_impl<add_lvalue_reference_t<T>, U>();
    type_traits_ref2_test_impl<add_rvalue_reference_t<T>, U>();
}

template <typename Callable, typename... Args>
void type_traits_func_test_impl() {
    INSTANTIATE(result_of<Callable(Args...)>);
#if _HAS_CXX17
    INSTANTIATE(invoke_result<Callable, Args...>);
    INSTANTIATE(is_invocable<Callable, Args...>);
    INSTANTIATE(is_nothrow_invocable<Callable, Args...>);
    INSTANTIATE(is_invocable_r<int, Callable, Args...>);
    INSTANTIATE(is_nothrow_invocable_r<int, Callable, Args...>);
#endif // _HAS_CXX17
}

void type_traits_test() {
    type_traits_ref_test_impl<void>();
    type_traits_ref_test_impl<void*, int*>();
    type_traits_ref_test_impl<int[]>();
    type_traits_ref_test_impl<int[5]>();
    type_traits_ref_test_impl<int[5][5]>();

    type_traits_ref_test_impl<array<int, 5>>();
    type_traits_ref_test_impl<string, const char*>();
    type_traits_ref_test_impl<vector<int>, int>();
    type_traits_ref_test_impl<decltype(printf)>();
    type_traits_ref_test_impl<int(int)>();
    type_traits_ref_test_impl<int(int, int)>();
    type_traits_ref_test_impl<int(int, int, int)>();

    struct A final {
        void fn() {}
        int x;
        void (A::*fn_ptr)();
    };
    type_traits_ref_test_impl<A>();
    type_traits_ref_test_impl<int A::*>();
    type_traits_ref_test_impl<decltype(&A::fn)>();
    type_traits_ref_test_impl<nullptr_t>();
    union B {
        int a;
        double b;
    };
    type_traits_ref_test_impl<B>();
    class C {
    public:
        virtual ~C() {}
        virtual void fn() = 0;
    };

    type_traits_ref_test_impl<C>();
    enum class D {};
    type_traits_ref_test_impl<D>();
    enum E {};
    type_traits_ref_test_impl<E>();
    type_traits_ref_test_impl<unsigned long>();
    type_traits_ref_test_impl<double>();
    type_traits_ref_test_impl<int>();
    type_traits_ref_test_impl<short>();
    type_traits_ref_test_impl<char>();
    type_traits_ref_test_impl<unsigned int>();
    class F : public C {
    public:
        virtual ~F() {}
    };
    type_traits_ref_test_impl<F, C>();
    type_traits_ref_test_impl<pair<int, double>>();
    type_traits_ref_test_impl<tuple<int, int, int>>();

    INSTANTIATE(make_integer_sequence<int, 0>);
    INSTANTIATE(index_sequence<1, 1, 2, 3, 5, 8, 13>);
    INSTANTIATE(make_index_sequence<5>);
    INSTANTIATE(index_sequence_for<int, int, int, int>);

    auto e = forward<string&&>(string{});
    string str{};
    auto f               = move(str);
    auto g               = move_if_noexcept(str);
    auto lambda          = []() { return 5; };
    auto noexcept_lambda = []() noexcept { return 5; };

    A my_a{};
    invoke(lambda);
    invoke(noexcept_lambda);
    invoke(&A::fn, my_a);
    invoke(my_a.fn_ptr, my_a);

    type_traits_func_test_impl<decltype(lambda)>();
    type_traits_func_test_impl<decltype(lambda), int>();
    type_traits_func_test_impl<decltype(noexcept_lambda)>();
    type_traits_func_test_impl<decltype(noexcept_lambda), int>();
    type_traits_func_test_impl<decltype(&A::fn), A>();
    type_traits_func_test_impl<void, int>();
    type_traits_func_test_impl<int, int>();
    using fp = void (*)(int);
    type_traits_func_test_impl<fp, int>();

    USE_VALUE(ref(my_a));
    auto ref_lambda = ref(lambda);
    ref_lambda();
    auto cref_lambda = cref(lambda);
    cref_lambda();
    const A const_a{};
    USE_VALUE(cref(const_a));
}
