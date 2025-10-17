// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <type_traits>

using namespace std;

// TRANSITION, MSVC and EDG haven't implemented intrinsics needed for P2255R2.
#ifdef __cpp_lib_reference_from_temporary
enum class conv_explicitly : bool {
    no,
    yes,
};

template <conv_explicitly Explicitly>
struct to_int_lvalue {
    explicit(static_cast<bool>(Explicitly)) operator int&() const;
};

template <conv_explicitly Explicitly>
struct to_int_xvalue {
    explicit(static_cast<bool>(Explicitly)) operator int&&() const;
};

template <conv_explicitly Explicitly>
struct to_int_prvalue {
    explicit(static_cast<bool>(Explicitly)) operator int() const;
};

template <class NonTempBindable>
void test_one_non_temporary_bindable_type() {
    static_assert(!reference_constructs_from_temporary<NonTempBindable, void>::value);

    static_assert(!reference_constructs_from_temporary<NonTempBindable, void()>::value);
    static_assert(!reference_constructs_from_temporary<NonTempBindable, void() const&>::value);

    static_assert(!reference_constructs_from_temporary<NonTempBindable, int>::value);
    static_assert(!reference_constructs_from_temporary<NonTempBindable, const int>::value);
    static_assert(!reference_constructs_from_temporary<NonTempBindable, int[]>::value);
    static_assert(!reference_constructs_from_temporary<NonTempBindable, const int[]>::value);
    static_assert(!reference_constructs_from_temporary<NonTempBindable, int[42]>::value);
    static_assert(!reference_constructs_from_temporary<NonTempBindable, const int[42]>::value);

    static_assert(!reference_constructs_from_temporary<NonTempBindable, int&>::value);
    static_assert(!reference_constructs_from_temporary<NonTempBindable, int&&>::value);
    static_assert(!reference_constructs_from_temporary<NonTempBindable, const int&>::value);
    static_assert(!reference_constructs_from_temporary<NonTempBindable, const int&&>::value);
    static_assert(!reference_constructs_from_temporary<NonTempBindable, void (&)()>::value);
    static_assert(!reference_constructs_from_temporary<NonTempBindable, void (&&)()>::value);

    static_assert(!reference_constructs_from_temporary_v<NonTempBindable, void>);

    static_assert(!reference_constructs_from_temporary_v<NonTempBindable, void()>);
    static_assert(!reference_constructs_from_temporary_v<NonTempBindable, void() const&>);

    static_assert(!reference_constructs_from_temporary_v<NonTempBindable, int>);
    static_assert(!reference_constructs_from_temporary_v<NonTempBindable, const int>);
    static_assert(!reference_constructs_from_temporary_v<NonTempBindable, int[]>);
    static_assert(!reference_constructs_from_temporary_v<NonTempBindable, const int[]>);
    static_assert(!reference_constructs_from_temporary_v<NonTempBindable, int[42]>);
    static_assert(!reference_constructs_from_temporary_v<NonTempBindable, const int[42]>);

    static_assert(!reference_constructs_from_temporary_v<NonTempBindable, int&>);
    static_assert(!reference_constructs_from_temporary_v<NonTempBindable, int&&>);
    static_assert(!reference_constructs_from_temporary_v<NonTempBindable, const int&>);
    static_assert(!reference_constructs_from_temporary_v<NonTempBindable, const int&&>);
    static_assert(!reference_constructs_from_temporary_v<NonTempBindable, void (&)()>);
    static_assert(!reference_constructs_from_temporary_v<NonTempBindable, void (&&)()>);
}

void test_non_temporary_bindable_types() {
    test_one_non_temporary_bindable_type<void>();

    test_one_non_temporary_bindable_type<void()>();
    test_one_non_temporary_bindable_type<void() const&>();

    test_one_non_temporary_bindable_type<int>();
    test_one_non_temporary_bindable_type<to_int_lvalue<conv_explicitly::yes>>();

    test_one_non_temporary_bindable_type<int&>();
    test_one_non_temporary_bindable_type<to_int_lvalue<conv_explicitly::yes>&>();

    test_one_non_temporary_bindable_type<const volatile int&>();
    test_one_non_temporary_bindable_type<const volatile to_int_lvalue<conv_explicitly::yes>&>();

    test_one_non_temporary_bindable_type<void (&)()>();
    test_one_non_temporary_bindable_type<void (&&)()>();
}

template <class Obj>
void test_one_cvref_to_object() {
    static_assert(!reference_constructs_from_temporary<Obj&, Obj>::value);
    static_assert(!reference_constructs_from_temporary<Obj&, const Obj>::value);
    static_assert(!reference_constructs_from_temporary<Obj&, volatile Obj>::value);
    static_assert(!reference_constructs_from_temporary<Obj&, const volatile Obj>::value);
    static_assert(reference_constructs_from_temporary<const Obj&, Obj>::value);
    static_assert(reference_constructs_from_temporary<const Obj&, const Obj>::value);
    static_assert(reference_constructs_from_temporary<const Obj&, volatile Obj>::value == is_scalar_v<Obj>);
    static_assert(reference_constructs_from_temporary<const Obj&, const volatile Obj>::value == is_scalar_v<Obj>);
    static_assert(!reference_constructs_from_temporary<volatile Obj&, Obj>::value);
    static_assert(!reference_constructs_from_temporary<volatile Obj&, const Obj>::value);
    static_assert(!reference_constructs_from_temporary<volatile Obj&, volatile Obj>::value);
    static_assert(!reference_constructs_from_temporary<volatile Obj&, const volatile Obj>::value);
    static_assert(!reference_constructs_from_temporary<const volatile Obj&, Obj>::value);
    static_assert(!reference_constructs_from_temporary<const volatile Obj&, const Obj>::value);
    static_assert(!reference_constructs_from_temporary<const volatile Obj&, volatile Obj>::value);
    static_assert(!reference_constructs_from_temporary<const volatile Obj&, const volatile Obj>::value);

    static_assert(reference_constructs_from_temporary<Obj&&, Obj>::value);
    static_assert(reference_constructs_from_temporary<Obj&&, const Obj>::value == is_scalar_v<Obj>);
    static_assert(reference_constructs_from_temporary<Obj&&, volatile Obj>::value == is_scalar_v<Obj>);
    static_assert(reference_constructs_from_temporary<Obj&&, const volatile Obj>::value == is_scalar_v<Obj>);
    static_assert(reference_constructs_from_temporary<const Obj&&, Obj>::value);
    static_assert(reference_constructs_from_temporary<const Obj&&, const Obj>::value);
    static_assert(reference_constructs_from_temporary<const Obj&&, volatile Obj>::value == is_scalar_v<Obj>);
    static_assert(reference_constructs_from_temporary<const Obj&&, const volatile Obj>::value == is_scalar_v<Obj>);
    static_assert(reference_constructs_from_temporary<volatile Obj&&, Obj>::value);
    static_assert(reference_constructs_from_temporary<volatile Obj&&, const Obj>::value == is_scalar_v<Obj>);
    static_assert(reference_constructs_from_temporary<volatile Obj&&, volatile Obj>::value);
    static_assert(reference_constructs_from_temporary<volatile Obj&&, const volatile Obj>::value == is_scalar_v<Obj>);
    static_assert(reference_constructs_from_temporary<const volatile Obj&&, Obj>::value);
    static_assert(reference_constructs_from_temporary<const volatile Obj&&, const Obj>::value);
    static_assert(reference_constructs_from_temporary<const volatile Obj&&, volatile Obj>::value);
    static_assert(reference_constructs_from_temporary<const volatile Obj&&, const volatile Obj>::value);

    static_assert(!reference_constructs_from_temporary_v<Obj&, Obj>);
    static_assert(!reference_constructs_from_temporary_v<Obj&, const Obj>);
    static_assert(!reference_constructs_from_temporary_v<Obj&, volatile Obj>);
    static_assert(!reference_constructs_from_temporary_v<Obj&, const volatile Obj>);
    static_assert(reference_constructs_from_temporary_v<const Obj&, Obj>);
    static_assert(reference_constructs_from_temporary_v<const Obj&, const Obj>);
    static_assert(reference_constructs_from_temporary_v<const Obj&, volatile Obj> == is_scalar_v<Obj>);
    static_assert(reference_constructs_from_temporary_v<const Obj&, const volatile Obj> == is_scalar_v<Obj>);
    static_assert(!reference_constructs_from_temporary_v<volatile Obj&, Obj>);
    static_assert(!reference_constructs_from_temporary_v<volatile Obj&, const Obj>);
    static_assert(!reference_constructs_from_temporary_v<volatile Obj&, volatile Obj>);
    static_assert(!reference_constructs_from_temporary_v<volatile Obj&, const volatile Obj>);
    static_assert(!reference_constructs_from_temporary_v<const volatile Obj&, Obj>);
    static_assert(!reference_constructs_from_temporary_v<const volatile Obj&, const Obj>);
    static_assert(!reference_constructs_from_temporary_v<const volatile Obj&, volatile Obj>);
    static_assert(!reference_constructs_from_temporary_v<const volatile Obj&, const volatile Obj>);

    static_assert(reference_constructs_from_temporary_v<Obj&&, Obj>);
    static_assert(reference_constructs_from_temporary_v<Obj&&, const Obj> == is_scalar_v<Obj>);
    static_assert(reference_constructs_from_temporary_v<Obj&&, volatile Obj> == is_scalar_v<Obj>);
    static_assert(reference_constructs_from_temporary_v<Obj&&, const volatile Obj> == is_scalar_v<Obj>);
    static_assert(reference_constructs_from_temporary_v<const Obj&&, Obj>);
    static_assert(reference_constructs_from_temporary_v<const Obj&&, const Obj>);
    static_assert(reference_constructs_from_temporary_v<const Obj&&, volatile Obj> == is_scalar_v<Obj>);
    static_assert(reference_constructs_from_temporary_v<const Obj&&, const volatile Obj> == is_scalar_v<Obj>);
    static_assert(reference_constructs_from_temporary_v<volatile Obj&&, Obj>);
    static_assert(reference_constructs_from_temporary_v<volatile Obj&&, const Obj> == is_scalar_v<Obj>);
    static_assert(reference_constructs_from_temporary_v<volatile Obj&&, volatile Obj>);
    static_assert(reference_constructs_from_temporary_v<volatile Obj&&, const volatile Obj> == is_scalar_v<Obj>);
    static_assert(reference_constructs_from_temporary_v<const volatile Obj&&, Obj>);
    static_assert(reference_constructs_from_temporary_v<const volatile Obj&&, const Obj>);
    static_assert(reference_constructs_from_temporary_v<const volatile Obj&&, volatile Obj>);
    static_assert(reference_constructs_from_temporary_v<const volatile Obj&&, const volatile Obj>);
}

void test_object_types() {
    test_one_cvref_to_object<int>();
    test_one_cvref_to_object<int[1]>();
    test_one_cvref_to_object<to_int_lvalue<conv_explicitly::yes>>();
    test_one_cvref_to_object<to_int_lvalue<conv_explicitly::no>>();
    test_one_cvref_to_object<to_int_xvalue<conv_explicitly::yes>>();
    test_one_cvref_to_object<to_int_xvalue<conv_explicitly::no>>();
}

template <conv_explicitly Explicitly>
void test_to_int_ref() {
    static_assert(!reference_constructs_from_temporary<int&, to_int_lvalue<Explicitly>>::value);
    static_assert(!reference_constructs_from_temporary<const int&, to_int_lvalue<Explicitly>>::value);
    static_assert(reference_constructs_from_temporary<const long&, to_int_lvalue<Explicitly>>::value
                  == !static_cast<bool>(Explicitly));
    static_assert(!reference_constructs_from_temporary<int&, to_int_xvalue<Explicitly>>::value);
    static_assert(!reference_constructs_from_temporary<const int&, to_int_xvalue<Explicitly>>::value);
    static_assert(reference_constructs_from_temporary<const long&, to_int_xvalue<Explicitly>>::value
                  == !static_cast<bool>(Explicitly));
    static_assert(!reference_constructs_from_temporary<int&, to_int_prvalue<Explicitly>>::value);
    static_assert(reference_constructs_from_temporary<const int&, to_int_prvalue<Explicitly>>::value);
    static_assert(reference_constructs_from_temporary<const long&, to_int_prvalue<Explicitly>>::value
                  == !static_cast<bool>(Explicitly));

    static_assert(!reference_constructs_from_temporary_v<int&, to_int_lvalue<Explicitly>>);
    static_assert(!reference_constructs_from_temporary_v<const int&, to_int_lvalue<Explicitly>>);
    static_assert(reference_constructs_from_temporary_v<const long&, to_int_lvalue<Explicitly>>
                  == !static_cast<bool>(Explicitly));
    static_assert(!reference_constructs_from_temporary_v<int&, to_int_xvalue<Explicitly>>);
    static_assert(!reference_constructs_from_temporary_v<const int&, to_int_xvalue<Explicitly>>);
    static_assert(reference_constructs_from_temporary_v<const long&, to_int_xvalue<Explicitly>>
                  == !static_cast<bool>(Explicitly));

    static_assert(!reference_constructs_from_temporary_v<int&, to_int_prvalue<Explicitly>>);
    static_assert(reference_constructs_from_temporary_v<const int&, to_int_prvalue<Explicitly>>);
    static_assert(reference_constructs_from_temporary_v<const long&, to_int_prvalue<Explicitly>>
                  == !static_cast<bool>(Explicitly));
}

void test_to_int_ref_all() {
    test_to_int_ref<conv_explicitly::no>();
    test_to_int_ref<conv_explicitly::yes>();
}
#endif // ^^^ no workaround ^^^
