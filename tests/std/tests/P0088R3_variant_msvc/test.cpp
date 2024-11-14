// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _SILENCE_CXX23_ALIGNED_UNION_DEPRECATION_WARNING
#define _LIBCXX_IN_DEVCRT
#include <msvc_stdlib_force_include.h> // Must precede any other libc++ headers

// Include Standard headers:
#include <cassert>
#include <cstddef>
#include <functional>
#include <limits>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>

// Include llvm-project/libcxx/test/support headers:
#include <test_macros.h>
#include <type_id.h>
#include <variant_test_helpers.h>

namespace msvc {
    namespace size {
        template <class T>
        using element =
            std::conditional<std::is_reference<T>::value, std::reference_wrapper<std::remove_reference_t<T>>, T>;

        template <std::size_t N>
        using index_t =
            std::conditional_t<(N < static_cast<std::size_t>(std::numeric_limits<signed char>::max())), signed char,
                std::conditional_t<(N < static_cast<std::size_t>(std::numeric_limits<short>::max())), short, int>>;

        template <class... Ts>
        struct fake_variant {
            std::aligned_union_t<0, typename element<Ts>::type...> data_;
            index_t<sizeof...(Ts)> index_;
        };

        template <class... Ts>
        constexpr bool check_size = sizeof(std::variant<Ts...>) == sizeof(fake_variant<Ts...>);

        template <int>
        struct empty {};

        struct not_empty {
            int i;
        };
        struct __declspec(empty_bases) many_bases : empty<0>, empty<1>, empty<2>, empty<3> {};

        static_assert(check_size<bool>);
        static_assert(check_size<char>);
        static_assert(check_size<unsigned char>);
        static_assert(check_size<int>);
        static_assert(check_size<unsigned int>);
        static_assert(check_size<long>);
        static_assert(check_size<long long>);
        static_assert(check_size<float>);
        static_assert(check_size<double>);
        static_assert(check_size<void*>);
        static_assert(check_size<empty<0>>);
        static_assert(check_size<not_empty>);
        static_assert(check_size<many_bases>);

        static_assert(check_size<bool, char, short, int, long, long long, float, double, long double, void*, empty<0>,
            empty<1>, not_empty, many_bases>);
    } // namespace size

    namespace big_variant {
#ifdef __EDG__
        constexpr std::size_t big = 20;
#else // C1XX and Clang
        constexpr std::size_t big = 64;
#endif // tune value of "big" to a bit less than the largest variant the front-end can handle
        constexpr std::size_t n = 16;

        template <std::size_t Size, std::size_t I, std::size_t N>
        void test_gets() {
            using V = std::_Meta_repeat_n_c<Size, std::string, std::variant>;
            V v{std::in_place_index<I>, "Hello, world!"};
            assert(std::get<I>(v) == "Hello, world!");
            if constexpr (N != 0) {
                test_gets<Size, I + (Size - I - 1) / N, N - 1>();
            }
        }

        template <std::size_t Size>
        void test_size() {
            if constexpr (Size <= big) {
                using V = std::_Meta_repeat_n_c<Size, std::string, std::variant>;
                // test 0, and n equally spaced indices including big - 1
                test_gets < Size, 0, n<Size ? n : Size>();
                constexpr std::size_t i = Size / 2;
                V v1{std::in_place_index<i>, "Hello, world!"};
                V v2 = v1;
                assert(std::get<i>(v2) == "Hello, world!");
                V v3 = std::move(v1);
                assert(std::get<i>(v3) == "Hello, world!");
                constexpr std::size_t j = Size <= 2 ? 0 : i + 1;
                v2.template emplace<j>("Goodbye, world!");
                v1 = v2;
                assert(std::get<j>(v1) == "Goodbye, world!");
                v2 = std::move(v3);
                assert(std::get<i>(v2) == "Hello, world!");
                v1.swap(v2);
                assert(std::get<j>(v2) == "Goodbye, world!");
                assert(std::get<i>(v1) == "Hello, world!");
                auto visitor = [](const std::string& s) { return s; };
                assert(std::visit(visitor, v1) == "Hello, world!");
            }
        }

        void run_test() {
            // test with all of the power-of-four sizes used for the switch blocks in visit and _Variant_raw_visit
            test_size<1>();
            test_size<3>();
            test_size<15>();
            test_size<63>();
            test_size<255>();
            test_size<big>();
        }
    } // namespace big_variant

    namespace derived_variant {
        void run_test() {
            // Extension: std::visit accepts types derived from a specialization of variant.
            {
                struct my_variant : std::variant<int, char, double> {
                    using std::variant<int, char, double>::variant;
                };

                my_variant v1{42};
                my_variant v2{3.14};
                auto visitor1 = [](auto&& x) { return static_cast<double>(x); };
                assert(std::visit(visitor1, v1) == 42.0);
                assert(std::visit(visitor1, v2) == 3.14);
                auto visitor2 = [](auto&& x, auto&& y) { return static_cast<double>(x + y); };
                assert(std::visit(visitor2, v1, v2) == 45.14);
            }
            {
                struct MakeEmptyT {
                    MakeEmptyT() = default;
                    MakeEmptyT(MakeEmptyT&&) {
                        throw 42;
                    }
                    MakeEmptyT& operator=(MakeEmptyT&&) {
                        throw 42;
                    }
                };
                struct my_variant : std::variant<int, MakeEmptyT> {
                    using std::variant<int, MakeEmptyT>::variant;
                };

                my_variant v{42};
                try {
                    v = my_variant{std::in_place_type<MakeEmptyT>};
                    abort();
                } catch (int) {
                    assert(v.valueless_by_exception());
                }

                auto very_useful_visitor = [](auto&&...) { abort(); };
                try {
                    std::visit(very_useful_visitor, v);
                    abort();
                } catch (const std::bad_variant_access&) {
                }
                try {
                    std::visit(very_useful_visitor, my_variant{42}, v);
                    abort();
                } catch (const std::bad_variant_access&) {
                }
                try {
                    std::visit(very_useful_visitor, v, my_variant{42});
                    abort();
                } catch (const std::bad_variant_access&) {
                }
            }
        }
    } // namespace derived_variant

    template <class Fn>
    struct mobile_visitor {
        mobile_visitor() = default;

        mobile_visitor(const mobile_visitor&) {
            abort();
        }
        mobile_visitor(mobile_visitor&&) {
            abort();
        }
        mobile_visitor& operator=(const mobile_visitor&) {
            abort();
            return *this;
        }
        mobile_visitor& operator=(mobile_visitor&&) {
            abort();
            return *this;
        }

        template <class... Args>
        constexpr decltype(auto) operator()(Args&&... args) const {
            return Fn{}(std::forward<Args>(args)...);
        }
    };

    template <class Fn>
    struct immobile_visitor : mobile_visitor<Fn> {
        immobile_visitor()                                   = default;
        immobile_visitor(const immobile_visitor&)            = delete;
        immobile_visitor& operator=(const immobile_visitor&) = delete;
    };

    template <class T>
    struct convert_to {
        template <class U, std::enable_if_t<std::is_convertible_v<U, T>, int> = 0>
        constexpr T operator()(U&& u) const {
            return std::forward<U>(u);
        }
    };

    namespace visit {
        void test_immobile_function() {
            {
                // Validate that visit need not copy or move the visitor
                using V = std::variant<int, double>;
                immobile_visitor<convert_to<double>> visitor{};
                assert(std::visit(visitor, V{42}) == 42.0);
                assert(std::visit(std::as_const(visitor), V{3.14}) == 3.14);
                assert(std::visit(std::move(visitor), V{1729}) == 1729.0);
                assert(std::visit(std::move(std::as_const(visitor)), V{1.414}) == 1.414);
            }
            {
                // Validate that visit does not copy or move the visitor
                using V = std::variant<int, double>;
                mobile_visitor<convert_to<double>> visitor{};
                assert(std::visit(visitor, V{42}) == 42.0);
                assert(std::visit(std::as_const(visitor), V{3.14}) == 3.14);
                assert(std::visit(std::move(visitor), V{1729}) == 1729.0);
                assert(std::visit(std::move(std::as_const(visitor)), V{1.414}) == 1.414);
            }
        }

        void run_test() {
            test_immobile_function();
        }
    } // namespace visit

    namespace visit_R {
#if _HAS_CXX20
        //===----------------------------------------------------------------------===//
        //
        // Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
        // See https://llvm.org/LICENSE.txt for license information.
        // SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
        //
        //===----------------------------------------------------------------------===//

        enum CallType : unsigned int { CT_None, CT_NonConst = 1, CT_Const = 2, CT_LValue = 4, CT_RValue = 8 };

        constexpr CallType operator|(CallType LHS, CallType RHS) {
            return static_cast<CallType>(static_cast<unsigned int>(LHS) | static_cast<unsigned int>(RHS));
        }

        struct ForwardingCallObject {
            template <class... Args>
            bool operator()(Args&&...) & {
                set_call<Args&&...>(CT_NonConst | CT_LValue);
                return {};
            }

            template <class... Args>
            char operator()(Args&&...) const& {
                set_call<Args&&...>(CT_Const | CT_LValue);
                return {};
            }

            // Don't allow the call operator to be invoked as an rvalue.
            template <class... Args>
            short operator()(Args&&...) && {
                set_call<Args&&...>(CT_NonConst | CT_RValue);
                return {};
            }

            template <class... Args>
            int operator()(Args&&...) const&& {
                set_call<Args&&...>(CT_Const | CT_RValue);
                return {};
            }

            template <class... Args>
            static void set_call(CallType type) {
                assert(last_call_type == CT_None);
                assert(last_call_args == nullptr);
                last_call_type = type;
                last_call_args = std::addressof(makeArgumentID<Args...>());
            }

            template <class... Args>
            static bool check_call(CallType type) {
                bool result = last_call_type == type && last_call_args && *last_call_args == makeArgumentID<Args...>();
                last_call_type = CT_None;
                last_call_args = nullptr;
                return result;
            }

            static CallType last_call_type;
            static const TypeID* last_call_args;
        };

        CallType ForwardingCallObject::last_call_type      = CT_None;
        const TypeID* ForwardingCallObject::last_call_args = nullptr;

        template <class R>
        void test_call_operator_forwarding() {
            using Fn = ForwardingCallObject;
            Fn obj{};
            const Fn& cobj = obj;
            { // test call operator forwarding - no variant
                std::visit<R>(obj);
                assert(Fn::check_call<>(CT_NonConst | CT_LValue));
                std::visit<R>(cobj);
                assert(Fn::check_call<>(CT_Const | CT_LValue));
                std::visit<R>(std::move(obj));
                assert(Fn::check_call<>(CT_NonConst | CT_RValue));
                std::visit<R>(std::move(cobj));
                assert(Fn::check_call<>(CT_Const | CT_RValue));
            }
            { // test call operator forwarding - single variant, single arg
                using V = std::variant<int>;
                V v(42);
                std::visit<R>(obj, v);
                assert(Fn::check_call<int&>(CT_NonConst | CT_LValue));
                std::visit<R>(cobj, v);
                assert(Fn::check_call<int&>(CT_Const | CT_LValue));
                std::visit<R>(std::move(obj), v);
                assert(Fn::check_call<int&>(CT_NonConst | CT_RValue));
                std::visit<R>(std::move(cobj), v);
                assert(Fn::check_call<int&>(CT_Const | CT_RValue));
            }
            { // test call operator forwarding - single variant, multi arg
                using V = std::variant<int, long, double>;
                V v(42l);
                std::visit<R>(obj, v);
                assert(Fn::check_call<long&>(CT_NonConst | CT_LValue));
                std::visit<R>(cobj, v);
                assert(Fn::check_call<long&>(CT_Const | CT_LValue));
                std::visit<R>(std::move(obj), v);
                assert(Fn::check_call<long&>(CT_NonConst | CT_RValue));
                std::visit<R>(std::move(cobj), v);
                assert(Fn::check_call<long&>(CT_Const | CT_RValue));
            }
            { // test call operator forwarding - multi variant, multi arg
                using V  = std::variant<int, long, double>;
                using V2 = std::variant<int*, std::string>;
                V v(42l);
                V2 v2("hello");
                std::visit<R>(obj, v, v2);
                assert((Fn::check_call<long&, std::string&>(CT_NonConst | CT_LValue)));
                std::visit<R>(cobj, v, v2);
                assert((Fn::check_call<long&, std::string&>(CT_Const | CT_LValue)));
                std::visit<R>(std::move(obj), v, v2);
                assert((Fn::check_call<long&, std::string&>(CT_NonConst | CT_RValue)));
                std::visit<R>(std::move(cobj), v, v2);
                assert((Fn::check_call<long&, std::string&>(CT_Const | CT_RValue)));
            }
        }

        template <class R>
        void test_argument_forwarding() {
            using Fn = ForwardingCallObject;
            Fn obj{};
            const auto Val = CT_LValue | CT_NonConst;
            { // single argument - value type
                using V = std::variant<int>;
                V v(42);
                const V& cv = v;
                std::visit<R>(obj, v);
                assert(Fn::check_call<int&>(Val));
                std::visit<R>(obj, cv);
                assert(Fn::check_call<const int&>(Val));
                std::visit<R>(obj, std::move(v));
                assert(Fn::check_call<int&&>(Val));
                std::visit<R>(obj, std::move(cv));
                assert(Fn::check_call<const int&&>(Val));
            }
        }

        struct ReturnFirst {
            template <class F, class... Args>
            constexpr F operator()(F f, Args&&...) const {
                return f;
            }
        };

        struct ReturnArity {
            template <class... Args>
            constexpr int operator()(Args&&...) const {
                return sizeof...(Args);
            }
        };

        struct simple_base {
            int x;

            constexpr explicit simple_base(int i) noexcept : x{i} {}
        };

        template <int>
        struct simple_derived : simple_base {
            using simple_base::simple_base;
        };

        constexpr bool test_constexpr() {
            constexpr ReturnFirst obj{};
            constexpr ReturnArity aobj{};
            {
                using V = std::variant<int>;
                constexpr V v(42);
                static_assert(std::visit<int>(obj, v) == 42);
            }
            {
                using V = std::variant<short, long, char>;
                constexpr V v(42l);
                static_assert(std::visit<long>(obj, v) == 42);
            }
            {
                using V1 = std::variant<int>;
                using V2 = std::variant<int, char*, long long>;
                using V3 = std::variant<bool, int, int>;
                constexpr V1 v1;
                constexpr V2 v2(nullptr);
                constexpr V3 v3;
                static_assert(std::visit<double>(aobj, v1, v2, v3) == 3.0);
            }
            {
                using V1 = std::variant<int>;
                using V2 = std::variant<int, char*, long long>;
                using V3 = std::variant<void*, int, int>;
                constexpr V1 v1;
                constexpr V2 v2(nullptr);
                constexpr V3 v3;
                static_assert(std::visit<long long>(aobj, v1, v2, v3) == 3LL);
            }
            {
                using V = std::variant<simple_derived<0>, simple_derived<1>, simple_derived<2>>;
                V v{simple_derived<1>{42}};
                auto&& b = std::visit<simple_base&>(std::identity{}, v);
                ASSERT_SAME_TYPE(decltype(b), simple_base&);
                assert(b.x == 42);
                auto&& cb = std::visit<const simple_base&>(std::identity{}, std::as_const(v));
                ASSERT_SAME_TYPE(decltype(cb), const simple_base&);
                assert(cb.x == 42);
                auto&& rb = std::visit<simple_base&&>(std::identity{}, std::move(v));
                ASSERT_SAME_TYPE(decltype(rb), simple_base&&);
                assert(rb.x == 42);
                auto&& crb = std::visit<const simple_base&&>(std::identity{}, std::move(std::as_const(v)));
                ASSERT_SAME_TYPE(decltype(crb), const simple_base&&);
                assert(crb.x == 42);
            }
            return true;
        }

        void test_exceptions() {
#ifndef TEST_HAS_NO_EXCEPTIONS
            ReturnArity obj{};
            auto test = [&](auto&&... args) {
                try {
                    std::visit<void>(obj, args...);
                } catch (const std::bad_variant_access&) {
                    return true;
                } catch (...) {
                }
                return false;
            };
            {
                using V = std::variant<int, MakeEmptyT>;
                V v;
                makeEmpty(v);
                assert(test(v));
            }
            {
                using V  = std::variant<int, MakeEmptyT>;
                using V2 = std::variant<long, std::string, void*>;
                V v;
                makeEmpty(v);
                V2 v2("hello");
                assert(test(v, v2));
            }
            {
                using V  = std::variant<int, MakeEmptyT>;
                using V2 = std::variant<long, std::string, void*>;
                V v;
                makeEmpty(v);
                V2 v2("hello");
                assert(test(v2, v));
            }
            {
                using V  = std::variant<int, MakeEmptyT>;
                using V2 = std::variant<long, std::string, void*, MakeEmptyT>;
                V v;
                makeEmpty(v);
                V2 v2;
                makeEmpty(v2);
                assert(test(v, v2));
            }
#endif
        }

        // See LLVM-31916
        void test_caller_accepts_nonconst() {
            struct A {};
            struct Visitor {
                void operator()(A&) {}
            };
            std::variant<A> v;
            std::visit<void>(Visitor{}, v);
        }

        struct mobile_data {
            int x;

            /* implicit */ mobile_data(int i) : x{i} {}
            mobile_data(const mobile_data&) {
                abort();
            }
            mobile_data(mobile_data&&) {
                abort();
            }
            mobile_data& operator=(const mobile_data&) {
                abort();
                return *this;
            }
            mobile_data& operator=(mobile_data&&) {
                abort();
                return *this;
            }
        };

        struct immobile_data : mobile_data {
            using mobile_data::mobile_data;
            immobile_data(const immobile_data&)            = delete;
            immobile_data& operator=(const immobile_data&) = delete;
        };

        void test_perfect_return() {
            {
                // Verify that a return object need not be copied/moved
                using R = immobile_data;
                assert(std::visit<R>(std::identity{}, std::variant<int, short>{13}).x == 13);
                assert(std::visit<R>(std::identity{}, std::variant<int, short>{short{42}}).x == 42);

                // Verify that conversions to an object that can't be copied/moved are correctly handled
                struct convertible_to_immobile_one {
                    operator immobile_data() const {
                        return immobile_data{1729};
                    }
                };

                struct convertible_to_immobile_other {
                    operator immobile_data() const {
                        return immobile_data{1138};
                    }
                };

                using VarTestConv = std::variant<convertible_to_immobile_one, convertible_to_immobile_other>;
                assert(std::visit<R>(std::identity{}, VarTestConv{convertible_to_immobile_one{}}).x == 1729);
                assert(std::visit<R>(std::identity{}, VarTestConv{convertible_to_immobile_other{}}).x == 1138);
                auto immobile_converter = [](auto src) -> immobile_data { return src; };
                assert(std::visit<R>(immobile_converter, VarTestConv{convertible_to_immobile_one{}}).x == 1729);
                assert(std::visit<R>(immobile_converter, VarTestConv{convertible_to_immobile_other{}}).x == 1138);
            }
            {
                // Verify that a returned object is not copied/moved/modified
                using R = mobile_data;
                assert(std::visit<R>(std::identity{}, std::variant<int, short>{13}).x == 13);
                assert(std::visit<R>(std::identity{}, std::variant<int, short>{short{42}}).x == 42);

                // Verify that a returned reference is not copied/moved/modified
                auto visitor1 = [x = R{1729}](auto) mutable -> R& { return x; };
                assert(std::visit<R&>(visitor1, std::variant<int, short>{13}).x == 1729);
                auto visitor2 = [x = R{1138}](auto) mutable -> R&& { return std::move(x); };
                assert(std::visit<R&&>(visitor2, std::variant<int, short>{13}).x == 1138);
            }
        }

        void test_immobile_function() {
            {
                // Validate that visit need not copy or move the visitor
                using V = std::variant<int, double>;
                immobile_visitor<std::identity> visitor{};
                assert(std::visit<double>(visitor, V{42}) == 42.0);
                assert(std::visit<double>(std::as_const(visitor), V{3.14}) == 3.14);
                assert(std::visit<double>(std::move(visitor), V{1729}) == 1729.0);
                assert(std::visit<double>(std::move(std::as_const(visitor)), V{1.414}) == 1.414);
            }
            {
                // Validate that visit does not copy or move the visitor
                using V = std::variant<int, double>;
                mobile_visitor<std::identity> visitor{};
                assert(std::visit<double>(visitor, V{42}) == 42.0);
                assert(std::visit<double>(std::as_const(visitor), V{3.14}) == 3.14);
                assert(std::visit<double>(std::move(visitor), V{1729}) == 1729.0);
                assert(std::visit<double>(std::move(std::as_const(visitor)), V{1.414}) == 1.414);
            }
        }

        void run_test() {
            test_call_operator_forwarding<void>();
            test_call_operator_forwarding<const void>();
            test_call_operator_forwarding<long>();
            test_argument_forwarding<void>();
            test_argument_forwarding<const void>();
            test_argument_forwarding<long long>();
            test_constexpr();
            static_assert(test_constexpr());
            test_exceptions();
            test_caller_accepts_nonconst();
            test_perfect_return();
            test_immobile_function();
        }
#else // ^^^ _HAS_CXX20 / !_HAS_CXX20 vvv
        void run_test() {}
#endif // _HAS_CXX20
    } // namespace visit_R

    namespace visit_pointer_to_member {
        struct base {
            int x;

            int f() const {
                return x;
            }
        };
        struct derived : base {
            int y;
        };

        void run_test() {
            using V = std::variant<base, derived>;
            assert(std::visit(&base::x, V{base{13}}) == 13);
            assert(std::visit(&base::x, V{derived{{42}, 29}}) == 42);

            assert(std::visit(&base::f, V{base{13}}) == 13);
            assert(std::visit(&base::f, V{derived{{42}, 29}}) == 42);
        }
    } // namespace visit_pointer_to_member

    template <class, class = void>
    constexpr bool has_type = false;
    template <class T>
    constexpr bool has_type<T, std::void_t<typename T::type>> = true;

    // Verify that `_Meta_at_<_Meta_list<>, size_t(-1)>` has no member named `type`, and that instantiating it doesn't
    // consume the entire compiler heap.
    static_assert(!has_type<std::_Meta_at_<std::_Meta_list<>, static_cast<std::size_t>(-1)>>);

    namespace vso468746 {
        // Defend against regression of VSO-468746
        // "std::function's converting constructor/assignment should be unusable for performing copy/move assignments"

        void run_test() {
            struct S {
                std::variant<std::function<S()>> member;
            };

            static_assert(
                sizeof(std::variant<std::function<S()>>) == sizeof(std::function<S()>) + alignof(std::function<S()>));
            static_assert(sizeof(S::member) == sizeof(std::variant<std::function<S()>>));
            static_assert(sizeof(S) >= sizeof(S::member));
        }
    } // namespace vso468746

    namespace vso492097 {
        // Defend against regression of VSO-492097
        // The compiler was not correctly emitting constant data for variants constructed from the address of static
        // duration variables with a constexpr operator& due to mishandling initialization of nested classes with
        // anonymous union members.
        template <class T>
        struct wrap {
            T val_;
            constexpr T* operator&() {
                return &val_;
            }
        };

        void run_test() {
            static wrap<int> intVar       = {42};
            static wrap<double> doubleVar = {3.14};

            using V = std::variant<int*, double*>;
            static constexpr V v1(&intVar);
            static constexpr V v2(&doubleVar);

            static_assert(v1.index() == 0);
            assert(*std::get<0>(v1) == 42);
            static_assert(v2.index() == 1);
            assert(*std::get<1>(v2) == 3.14);
        }
    } // namespace vso492097

    namespace vso508126 {
        void run_test() {
            struct S {};
            static_assert(!std::is_copy_constructible_v<volatile S>);
            static_assert(!std::is_copy_constructible_v<std::variant<volatile S>>);
        }
    } // namespace vso508126

    namespace DevCom1031281 {
        // Compilers may warn when initializing a variant from a "weird" argument, e.g., std::variant<short>{some_int}
        // is potentially narrowing. Compilers should not, however, emit such diagnostics from the metaprogramming that
        // determines which alternative a variant initialization would activate. We don't want to emit warnings when
        // determining implicit conversion sequences early in overload resolution.

        void Overload(int) {}
        void Overload(std::variant<unsigned short>) {}

        void run_test() {
            Overload(42);
        }
    } // namespace DevCom1031281

    namespace gh2770 {
        // Previous metaprogramming to validate the type requirements for std::visit required typelists too long for
        // Clang.
        struct S {
            template <class T0, class T1, class T2, class T3, class T4>
            int operator()(T0, T1, T2, T3, T4) const {
                return 1729;
            }
        };

        void run_test() {
            using V = std::variant<char, int, long, long long>;
            assert(std::visit(S{}, V{'a'}, V{'b'}, V{10}, V{20L}, V{30LL}) == 1729);
#if _HAS_CXX20
            assert(std::visit<int>(S{}, V{'a'}, V{'b'}, V{10}, V{20L}, V{30LL}) == 1729);
#endif // _HAS_CXX20
        }
    } // namespace gh2770

    namespace gh4901 {
#if _HAS_CXX20
#define CONSTEXPR20 constexpr
#else // ^^^ _HAS_CXX20 / !_HAS_CXX20 vvv
#define CONSTEXPR20 inline
#endif // ^^^ !_HAS_CXX20 ^^^
        struct X {
            CONSTEXPR20 ~X() {}
        };

        struct Y {
            X _;
        };

        struct ZA {
            std::variant<Y, int> z;
        };

        struct ZB {
            std::variant<int, Y> z;
        };

#if _HAS_CXX20
        static_assert(ZA{0}.z.index() == 1);
        static_assert(ZA{Y{}}.z.index() == 0);
        static_assert(ZB{0}.z.index() == 0);
        static_assert(ZB{Y{}}.z.index() == 1);
#endif // _HAS_CXX20

        static_assert(std::is_nothrow_destructible_v<X>);
        static_assert(std::is_nothrow_destructible_v<Y>);
        static_assert(std::is_nothrow_destructible_v<std::variant<Y, int>>);
        static_assert(std::is_nothrow_destructible_v<std::variant<int, Y>>);
        static_assert(std::is_nothrow_destructible_v<ZA>);
        static_assert(std::is_nothrow_destructible_v<ZB>);

        // Verify that variant::~variant is noexcept even when an alternative has a potentially-throwing destructor,
        // per N4988 [res.on.exception.handling]/3.
        struct X2 {
            CONSTEXPR20 ~X2() noexcept(false) {}
        };

        struct Y2 {
            X2 _;
        };

        struct ZA2 {
            std::variant<Y2, int> z;
        };

        struct ZB2 {
            std::variant<int, Y2> z;
        };

#if _HAS_CXX20
        static_assert(ZA2{0}.z.index() == 1);
        static_assert(ZA2{Y2{}}.z.index() == 0);
        static_assert(ZB2{0}.z.index() == 0);
        static_assert(ZB2{Y2{}}.z.index() == 1);
#endif // _HAS_CXX20

        static_assert(!std::is_nothrow_destructible_v<X2>);
        static_assert(!std::is_nothrow_destructible_v<Y2>);
        static_assert(std::is_nothrow_destructible_v<std::variant<Y2, int>>);
        static_assert(std::is_nothrow_destructible_v<std::variant<int, Y2>>);
        static_assert(std::is_nothrow_destructible_v<ZA2>);
        static_assert(std::is_nothrow_destructible_v<ZB2>);

        struct ZC {
            std::variant<Y, int, Y2> z;
        };

#if _HAS_CXX20
        static_assert(ZC{Y{}}.z.index() == 0);
        static_assert(ZC{0}.z.index() == 1);
        static_assert(ZC{Y2{}}.z.index() == 2);
#endif // _HAS_CXX20

        static_assert(std::is_nothrow_destructible_v<std::variant<Y, int, Y2>>);
        static_assert(std::is_nothrow_destructible_v<ZC>);
#undef CONSTEXPR20
    } // namespace gh4901

    namespace assign_cv {
        template <class T>
        struct TypeIdentityImpl {
            using type = T;
        };
        template <class T>
        using TypeIdentity = typename TypeIdentityImpl<T>::type;

        struct CvAssignable {
            CvAssignable()                               = default;
            CvAssignable(const CvAssignable&)            = default;
            CvAssignable(CvAssignable&&)                 = default;
            CvAssignable& operator=(const CvAssignable&) = default;
            CvAssignable& operator=(CvAssignable&&)      = default;

            template <class T = CvAssignable>
            CvAssignable(const volatile TypeIdentity<T>&) noexcept {}
            template <class T = CvAssignable>
            CvAssignable(const volatile TypeIdentity<T>&&) noexcept {}

            template <class T = CvAssignable>
            constexpr CvAssignable& operator=(const volatile TypeIdentity<T>&) noexcept {
                return *this;
            }
            template <class T = CvAssignable>
            constexpr CvAssignable& operator=(const volatile TypeIdentity<T>&&) noexcept {
                return *this;
            }

            template <class T = CvAssignable>
            constexpr const volatile CvAssignable& operator=(const volatile TypeIdentity<T>&) const volatile noexcept {
                return *this;
            }
            template <class T = CvAssignable>
            constexpr const volatile CvAssignable& operator=(const volatile TypeIdentity<T>&&) const volatile noexcept {
                return *this;
            }
        };

        void run_test() {
            using std::swap;
            {
                std::variant<const int> oc{};
                oc.emplace<0>(0);
                static_assert(!std::is_copy_assignable_v<decltype(oc)>);
                static_assert(!std::is_move_assignable_v<decltype(oc)>);
                static_assert(!std::is_swappable_v<decltype(oc)>);

                std::variant<volatile int> ov{};
                std::variant<volatile int> ov2{};
                ov.emplace<0>(0);
                swap(ov, ov);
                ov = ov2;
                ov = std::move(ov2);

                std::variant<const volatile int> ocv{};
                ocv.emplace<0>(0);
                static_assert(!std::is_copy_assignable_v<decltype(ocv)>);
                static_assert(!std::is_move_assignable_v<decltype(ocv)>);
                static_assert(!std::is_swappable_v<decltype(ocv)>);
            }
            {
                std::variant<const CvAssignable> oc{};
                std::variant<const CvAssignable> oc2{};
                oc.emplace<0>(CvAssignable{});
                swap(oc, oc);
                oc = oc2;
                oc = std::move(oc2);

                std::variant<volatile CvAssignable> ov{};
                std::variant<volatile CvAssignable> ov2{};
                ov.emplace<0>(CvAssignable{});
                swap(ov, ov);
                ov = ov2;
                ov = std::move(ov2);

                std::variant<const volatile CvAssignable> ocv{};
                std::variant<const volatile CvAssignable> ocv2{};
                ocv.emplace<0>(CvAssignable{});
                swap(ocv, ocv);
                ocv = ocv2;
                ocv = std::move(ocv2);
            }
        }
    } // namespace assign_cv

    namespace gh4959 {
        // Test GH-4959 "P0608R3 breaks flang build with Clang"
        // Constraints on variant's converting constructor and assignment operator templates reject arguments of the
        // variant's type, but did not short-circuit to avoid evaluating the constructibility constraint. For this
        // program, the constructibility constraint is ill-formed outside the immediate context when determining if
        // variant<optional<GenericSpec>> can be initialized from an rvalue of the same type.

        template <typename... RvRef>
        using NoLvalue = std::enable_if_t<(... && !std::is_lvalue_reference_v<RvRef>)>;

        struct Name {};

        struct GenericSpec {
            template <typename A, typename = NoLvalue<A>>
            GenericSpec(A&& x) : u(std::move(x)) {}
            GenericSpec(GenericSpec&&) = default;
            std::variant<Name> u;
        };

        struct InterfaceStmt {
            template <typename A, typename = NoLvalue<A>>
            InterfaceStmt(A&& x) : u(std::move(x)) {}
            InterfaceStmt(InterfaceStmt&&) = default;
            std::variant<std::optional<GenericSpec>> u;
        };
    } // namespace gh4959
} // namespace msvc

int main() {
    msvc::big_variant::run_test();
    msvc::derived_variant::run_test();
    msvc::visit::run_test();
    msvc::visit_R::run_test();
    msvc::visit_pointer_to_member::run_test();

    msvc::vso468746::run_test();
    msvc::vso508126::run_test();
    msvc::vso492097::run_test();
    msvc::DevCom1031281::run_test();
    msvc::gh2770::run_test();
    msvc::assign_cv::run_test();
}
