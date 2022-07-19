// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <bit>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstring>
#include <limits>
#include <type_traits>
#include <utility>

// structure sizing depends on a packing of 8, which is the default

union test_union_1 {
    char a;
    int b;
};
union test_union_2 {
    char a;
    float b;
};

struct middle_class_1 {
    double d;
    virtual void a_member_function_1() {}
};

struct middle_class_2 {
    int a;
    virtual void a_member_function_2() {}
};

struct derived_class : middle_class_1, middle_class_2 {
    virtual void a_member_function_2() override {}
};

struct test_struct_1 {
    char a;
    // char[3]
    int b;
    short c;
    // char[6]
    double d;

    void a_member_function() {}
};

struct test_struct_2 {
    short a;
    // char[2]
    float b;
    float c;
    int d;
    unsigned int e;
    char f;
    // char[1]
    short g;

    void a_member_function() {}
};

struct test_struct_3_member_fn_pointer {
    char a;
    // char[3]
    int b;
    short c;
    char d[2];
    void (test_struct_1::*fn)();
};

struct test_struct_4_large_member_fn_pointer {
    char a;
    // char[3]
    int b;
    double c;
    double d;
    void (derived_class::*fn)(); // "large" member fn pointers are aligned to 8 on both x64 and x86
};

struct test_struct_5_struct {
    char a;
    // char[3]
    int b;
    double c;
    double d;
    void* e;
    size_t f;
};

struct test_struct_6 {
    char a;
    // char[3]
    int b;
    int c;
    // char[4] on x64
    void* v;
};

struct test_struct_7_member_fn_pointer {
    char a;
    // char[3]
    int b;
    int c;
    void (test_struct_1::*fn)();
};

struct test_struct_1_not_trivially_copyable {
    char a;
    // char[3]
    int b;
    short c;
    // char[2]
    double d;

    test_struct_1_not_trivially_copyable& operator=(const test_struct_1_not_trivially_copyable&) {
        return *this;
    }
};

#pragma pack(push, 1)
struct test_struct_1_packed {
    char a;
    int b;
    short c;
    double d;
};
struct test_struct_2_packed {
    short a;
    float b;
    float c;
    int d;
    unsigned int e;
    char f;
    short g;
};
#pragma pack(pop)

static_assert(sizeof(test_struct_1) == sizeof(test_struct_2));

template <typename To, typename From, typename = void>
constexpr bool bit_cast_invocable = false;

template <typename To, typename From>
constexpr bool bit_cast_invocable<To, From, std::void_t<decltype(std::bit_cast<To>(std::declval<From>()))>> = true;

template <int zero = 0, int = ((void) std::bit_cast<test_union_1>(test_union_2{}), zero)>
constexpr bool bit_cast_is_constexpr_union(int) {
    return true;
}

constexpr bool bit_cast_is_constexpr_union(long) {
    return false;
}

template <int zero = 0, int = ((void) std::bit_cast<float*>(nullptr), zero)>
constexpr bool bit_cast_is_constexpr_pointer(int) {
    return true;
}

constexpr bool bit_cast_is_constexpr_pointer(long) {
    return false;
}

template <int zero = 0,
    int            = ((void) std::bit_cast<void (test_struct_1::*)()>(&test_struct_2::a_member_function), zero)>
constexpr bool bit_cast_is_constexpr_member_fn_pointer(int) {
    return true;
}

constexpr bool bit_cast_is_constexpr_member_fn_pointer(long) {
    return false;
}

template <int zero = 0, int = ((void) std::bit_cast<test_struct_6>(test_struct_3_member_fn_pointer{}), zero)>
constexpr bool bit_cast_is_constexpr_pmf_datamember(int) {
    return true;
}

constexpr bool bit_cast_is_constexpr_pmf_datamember(long) {
    return false;
}

template <int zero = 0,
    int            = ((void) std::bit_cast<test_struct_5_struct>(test_struct_4_large_member_fn_pointer{}), zero)>
constexpr bool bit_cast_is_constexpr_large_member_fn_pointer(int) {
    return true;
}

constexpr bool bit_cast_is_constexpr_large_member_fn_pointer(long) {
    return false;
}

template <typename To, typename From>
void zero_initialized_round_trip() {
    From before{};
    To middle = std::bit_cast<To>(before);
    assert(memcmp(&before, &middle, sizeof(From)) == 0);
    From after = std::bit_cast<From>(middle);
    assert(memcmp(&before, &after, sizeof(From)) == 0);
}

constexpr bool test_float() {
    unsigned int as_int = std::bit_cast<unsigned int>(0x0.000002p-126f);
    assert(as_int == 1);
    assert(std::bit_cast<float>(as_int) == 0x0.000002p-126f);
    as_int = std::bit_cast<unsigned int>(0x1.1p1f);
    assert(as_int == 0x40080000);
    assert(std::bit_cast<float>(as_int) == 0x1.1p1f);
    as_int = std::bit_cast<unsigned int>(0x0.0p0f);
    assert(as_int == 0);
    assert(std::bit_cast<float>(as_int) == 0x0.0p0f);
    if (!std::is_constant_evaluated()) {
        assert(std::signbit(std::bit_cast<float>(as_int)) == false);
    }
    as_int = std::bit_cast<unsigned int>(-0x0.0p0f);
    assert(as_int == 0x80000000);
    assert(std::bit_cast<float>(as_int) == -0x0.0p0f);
    if (!std::is_constant_evaluated()) {
        assert(std::signbit(std::bit_cast<float>(as_int)) == true);
    }
    // signaling nan
    as_int     = 0x7fc00001;
    float snan = std::bit_cast<float>(as_int);
    assert(as_int == std::bit_cast<unsigned int>(snan));
    as_int = std::bit_cast<unsigned int>(std::numeric_limits<float>::infinity());
    assert(as_int == 0x7f800000);
    assert(std::bit_cast<float>(as_int) == std::numeric_limits<float>::infinity());
    return true;
}

int main() {
    static_assert(!bit_cast_invocable<test_struct_2_packed, test_struct_1_packed>);
    static_assert(!bit_cast_invocable<test_struct_1_not_trivially_copyable, test_struct_1>);
    static_assert(!bit_cast_invocable<test_struct_1, test_struct_1_not_trivially_copyable>);
    static_assert(!bit_cast_invocable<test_struct_1_not_trivially_copyable, test_struct_1_not_trivially_copyable>);
    static_assert(bit_cast_invocable<test_union_1, test_union_2>);
    static_assert(bit_cast_invocable<ptrdiff_t, void (test_struct_1::*)()>);
    static_assert(bit_cast_invocable<ptrdiff_t, void (*)()>);
    static_assert(bit_cast_invocable<test_struct_1, test_struct_2>);
    static_assert(bit_cast_invocable<test_struct_4_large_member_fn_pointer, test_struct_5_struct>);

    // tests for conditions on constexprness
    static_assert(!bit_cast_is_constexpr_union(0));
    static_assert(!bit_cast_is_constexpr_pointer(0));
    static_assert(!bit_cast_is_constexpr_member_fn_pointer(0));
    static_assert(!bit_cast_is_constexpr_pmf_datamember(0));
    static_assert(!bit_cast_is_constexpr_large_member_fn_pointer(0));

    zero_initialized_round_trip<test_struct_1, test_struct_2>();
    zero_initialized_round_trip<test_struct_3_member_fn_pointer, test_struct_3_member_fn_pointer>();
    zero_initialized_round_trip<test_struct_3_member_fn_pointer, test_struct_7_member_fn_pointer>();
    zero_initialized_round_trip<test_struct_4_large_member_fn_pointer, test_struct_5_struct>();
    zero_initialized_round_trip<test_struct_6, test_struct_3_member_fn_pointer>();
    zero_initialized_round_trip<float, int>();
    zero_initialized_round_trip<double, long long>();
    zero_initialized_round_trip<unsigned int, float>();

    assert(test_float());
    static_assert(test_float());
}
