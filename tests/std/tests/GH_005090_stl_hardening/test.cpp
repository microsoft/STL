// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// env.lst defines _MSVC_STL_HARDENING to 1.

#include <array>
#include <bitset>
#include <deque>
#include <forward_list>
#include <list>
#include <memory>
#include <string>
#include <utility>
#include <valarray>
#include <vector>

#if _HAS_CXX17
#include <optional>
#include <string_view>
#endif // _HAS_CXX17

#if _HAS_CXX20
#include <ranges>
#include <span>
#endif // _HAS_CXX20

#if _HAS_CXX23
#include <expected>
#include <mdspan>
#include <stacktrace>
#endif // _HAS_CXX23

#include <test_death.hpp>

using namespace std;

// <array>
void test_array_subscript() {
    array<int, 3> a{};
#pragma warning(push)
#pragma warning(disable : 28020) // /analyze correctly warns: The expression '_Param_(1)<3' is not true at this call.
    (void) a[3];
#pragma warning(pop)
}

void test_array_subscript_const() {
    const array<int, 3> a{};
#pragma warning(push)
#pragma warning(disable : 28020) // /analyze correctly warns: The expression '_Param_(1)<3' is not true at this call.
    (void) a[3];
#pragma warning(pop)
}

void test_array_zero_subscript() {
    array<int, 0> az{};
    (void) az[0];
}

void test_array_zero_subscript_const() {
    const array<int, 0> az{};
    (void) az[0];
}

void test_array_zero_front() {
    array<int, 0> az{};
    (void) az.front();
}

void test_array_zero_front_const() {
    const array<int, 0> az{};
    (void) az.front();
}

void test_array_zero_back() {
    array<int, 0> az{};
    (void) az.back();
}

void test_array_zero_back_const() {
    const array<int, 0> az{};
    (void) az.back();
}

// <bitset>
void test_bitset_subscript() {
    bitset<100> b{};
    (void) b[100];
}

void test_bitset_subscript_const() {
    const bitset<100> b{};
    (void) b[100];
}

// <deque>
void test_deque_subscript() {
    deque<int> d(3);
    (void) d[3];
}

void test_deque_subscript_const() {
    const deque<int> d(3);
    (void) d[3];
}

void test_deque_front() {
    deque<int> d{};
    (void) d.front();
}

void test_deque_front_const() {
    const deque<int> d{};
    (void) d.front();
}

void test_deque_back() {
    deque<int> d{};
    (void) d.back();
}

void test_deque_back_const() {
    const deque<int> d{};
    (void) d.back();
}

void test_deque_pop_front() {
    deque<int> d{};
    d.pop_front();
}

void test_deque_pop_back() {
    deque<int> d{};
    d.pop_back();
}

// <forward_list>
void test_forward_list_front() {
    forward_list<int> fl{};
    (void) fl.front();
}

void test_forward_list_front_const() {
    const forward_list<int> fl{};
    (void) fl.front();
}

void test_forward_list_pop_front() {
    forward_list<int> fl{};
    fl.pop_front();
}

// <list>
void test_list_front() {
    list<int> l{};
    (void) l.front();
}

void test_list_front_const() {
    const list<int> l{};
    (void) l.front();
}

void test_list_back() {
    list<int> l{};
    (void) l.back();
}

void test_list_back_const() {
    const list<int> l{};
    (void) l.back();
}

void test_list_pop_front() {
    list<int> l{};
    l.pop_front();
}

void test_list_pop_back() {
    list<int> l{};
    l.pop_back();
}

// <memory>
void test_shared_ptr_bounded_array_subscript_underflow() {
    shared_ptr<int[42]> sp{new int[42]{}};
    (void) sp[-1];
}

void test_shared_ptr_bounded_array_subscript_overflow() {
    shared_ptr<int[42]> sp{new int[42]{}};
    (void) sp[84];
}

void test_shared_ptr_unbounded_array_subscript_underflow() {
    shared_ptr<int[]> sp{new int[42]{}};
    (void) sp[-1];
}

// <string>
void test_string_subscript() {
    string s(3, '*');
    (void) s[4]; // beyond null terminator
}

void test_string_subscript_const() {
    const string s(3, '*');
    (void) s[4]; // beyond null terminator
}

void test_string_front() {
    string s{};
    (void) s.front();
}

void test_string_front_const() {
    const string s{};
    (void) s.front();
}

void test_string_back() {
    string s{};
    (void) s.back();
}

void test_string_back_const() {
    const string s{};
    (void) s.back();
}

void test_string_pop_back() {
    string s{};
    s.pop_back();
}

// <valarray>
void test_valarray_subscript() {
    valarray<int> va(3);
    (void) va[3];
}

void test_valarray_subscript_const() {
    const valarray<int> va(3);
    (void) va[3];
}

// <vector>
void test_vector_subscript() {
    vector<int> v(3);
    (void) v[3];
}

void test_vector_subscript_const() {
    const vector<int> v(3);
    (void) v[3];
}

void test_vector_front() {
    vector<int> v{};
    (void) v.front();
}

void test_vector_front_const() {
    const vector<int> v{};
    (void) v.front();
}

void test_vector_back() {
    vector<int> v{};
    (void) v.back();
}

void test_vector_back_const() {
    const vector<int> v{};
    (void) v.back();
}

void test_vector_pop_back() {
    vector<int> v{};
    v.pop_back();
}

void test_vector_bool_subscript() {
    vector<bool> vb(3);
    (void) vb[3];
}

void test_vector_bool_subscript_const() {
    const vector<bool> vb(3);
    (void) vb[3];
}

void test_vector_bool_front() {
    vector<bool> vb{};
    (void) vb.front();
}

void test_vector_bool_front_const() {
    const vector<bool> vb{};
    (void) vb.front();
}

void test_vector_bool_back() {
    vector<bool> vb{};
    (void) vb.back();
}

void test_vector_bool_back_const() {
    const vector<bool> vb{};
    (void) vb.back();
}

void test_vector_bool_pop_back() {
    vector<bool> vb{};
    vb.pop_back();
}

#if _HAS_CXX17
// <optional>
void test_optional_deref_lvalue() {
    optional<int> o{};
    (void) *o;
}

void test_optional_deref_lvalue_const() {
    const optional<int> o{};
    (void) *o;
}

void test_optional_deref_rvalue() {
    optional<int> o{};
    (void) *move(o);
}

void test_optional_deref_rvalue_const() {
    const optional<int> o{};
    (void) *move(o);
}

void test_optional_arrow() {
    optional<int> o{};
    (void) o.operator->();
}

void test_optional_arrow_const() {
    const optional<int> o{};
    (void) o.operator->();
}

// <string_view>
void test_string_view_subscript() {
    const string_view sv{"Toki"};
    (void) sv[4];
}

void test_string_view_front() {
    const string_view sv{};
    (void) sv.front();
}

void test_string_view_back() {
    const string_view sv{};
    (void) sv.back();
}

void test_string_view_remove_prefix() {
    string_view sv{"Cosmos"};
    sv.remove_prefix(7);
}

void test_string_view_remove_suffix() {
    string_view sv{"Meadow"};
    sv.remove_suffix(7);
}
#endif // _HAS_CXX17

#if _HAS_CXX20
// <ranges>
// ranges::subrange derives from ranges::view_interface, which is hardened.
void test_ranges_view_interface_subscript() {
    int arr[10]{};
    ranges::subrange<int*> sr{arr + 5, arr + 8};
    (void) sr[3];
}

void test_ranges_view_interface_subscript_const() {
    int arr[10]{};
    const ranges::subrange<int*> sr{arr + 5, arr + 8};
    (void) sr[3];
}

void test_ranges_view_interface_front() {
    int arr[10]{};
    ranges::subrange<int*> sr{arr + 5, arr + 5};
    (void) sr.front();
}

void test_ranges_view_interface_front_const() {
    int arr[10]{};
    const ranges::subrange<int*> sr{arr + 5, arr + 5};
    (void) sr.front();
}

void test_ranges_view_interface_back() {
    int arr[10]{};
    ranges::subrange<int*> sr{arr + 5, arr + 5};
    (void) sr.back();
}

void test_ranges_view_interface_back_const() {
    int arr[10]{};
    const ranges::subrange<int*> sr{arr + 5, arr + 5};
    (void) sr.back();
}

// <span>
void test_span_ctor_first_count() {
    int arr[10]{};
    span<int, 3> sp_static{arr, 10};
}

void test_span_ctor_first_last() {
    int arr[10]{};
    span<int, 3> sp_static{arr, arr + 10};
}

void test_span_ctor_range() {
    vector<int> v(10);
    span<int, 3> sp_static{v};
}

void test_span_ctor_other() {
    int arr[10]{};
    span<int> other{arr};
    span<int, 3> sp_static{other};
}

void test_span_first_compiletime() {
    int arr[10]{};
    const span<int> sp{arr};
    (void) sp.first<11>();
}

void test_span_first_runtime() {
    int arr[10]{};
    const span<int> sp{arr};
    (void) sp.first(11);
}

void test_span_last_compiletime() {
    int arr[10]{};
    const span<int> sp{arr};
    (void) sp.last<11>();
}

void test_span_last_runtime() {
    int arr[10]{};
    const span<int> sp{arr};
    (void) sp.last(11);
}

void test_span_subspan_compiletime_bad_offset() {
    int arr[10]{};
    const span<int> sp{arr};
    (void) sp.subspan<11>();
}

void test_span_subspan_compiletime_bad_count() {
    int arr[10]{};
    const span<int> sp{arr};
    (void) sp.subspan<3, 8>();
}

void test_span_subspan_runtime_bad_offset() {
    int arr[10]{};
    const span<int> sp{arr};
    (void) sp.subspan(11);
}

void test_span_subspan_runtime_bad_count() {
    int arr[10]{};
    const span<int> sp{arr};
    (void) sp.subspan(3, 8);
}

void test_span_subscript() {
    int arr[10]{};
    const span<int> sp{arr};
    (void) sp[10];
}

void test_span_front() {
    const span<int> sp{};
    (void) sp.front();
}

void test_span_back() {
    const span<int> sp{};
    (void) sp.back();
}
#endif // _HAS_CXX20

#if _HAS_CXX23
// <expected>
void test_expected_arrow() {
    expected<int, string> e{unexpect, "woof"};
    (void) e.operator->();
}

void test_expected_arrow_const() {
    const expected<int, string> e{unexpect, "woof"};
    (void) e.operator->();
}

void test_expected_deref_lvalue() {
    expected<int, string> e{unexpect, "woof"};
    (void) *e;
}

void test_expected_deref_lvalue_const() {
    const expected<int, string> e{unexpect, "woof"};
    (void) *e;
}

void test_expected_deref_rvalue() {
    expected<int, string> e{unexpect, "woof"};
    (void) *move(e);
}

void test_expected_deref_rvalue_const() {
    const expected<int, string> e{unexpect, "woof"};
    (void) *move(e);
}

void test_expected_error_lvalue() {
    expected<int, string> e{1729};
    (void) e.error();
}

void test_expected_error_lvalue_const() {
    const expected<int, string> e{1729};
    (void) e.error();
}

void test_expected_error_rvalue() {
    expected<int, string> e{1729};
    (void) move(e).error();
}

void test_expected_error_rvalue_const() {
    const expected<int, string> e{1729};
    (void) move(e).error();
}

void test_expected_void_deref() {
    const expected<void, string> ev{unexpect, "woof"};
    (void) *ev;
}

void test_expected_void_error_lvalue() {
    expected<void, string> ev{};
    (void) ev.error();
}

void test_expected_void_error_lvalue_const() {
    const expected<void, string> ev{};
    (void) ev.error();
}

void test_expected_void_error_rvalue() {
    expected<void, string> ev{};
    (void) move(ev).error();
}

void test_expected_void_error_rvalue_const() {
    const expected<void, string> ev{};
    (void) move(ev).error();
}

// <mdspan>
void test_mdspan_ctor_other() {
    char arr[12]{};
    mdspan<char, dextents<int, 2>> other{arr, 4, 3};
    mdspan<const char, extents<int, 3, 4>> md{other};
}

#ifdef __cpp_multidimensional_subscript // TRANSITION, P2128R6
void test_mdspan_subscript_multidim() {
    const auto str{"HissMeowPurr"};
    const mdspan<const char, extents<int, 3, 4>> md{str, 3, 4};
    (void) md[1, 4];
}
#endif // ^^^ defined(__cpp_multidimensional_subscript) ^^^

void test_mdspan_subscript_array() {
    const auto str{"HissMeowPurr"};
    const mdspan<const char, extents<int, 3, 4>> md{str, 3, 4};
    const array<int, 2> a_idx{1, 4};
    (void) md[a_idx];
}

void test_mdspan_subscript_span() {
    const auto str{"HissMeowPurr"};
    const mdspan<const char, extents<int, 3, 4>> md{str, 3, 4};
    const array<int, 2> a_idx{1, 4};
    const span<const int, 2> sp_idx{a_idx};
    (void) md[sp_idx];
}

// <stacktrace>
void test_stacktrace_current() {
    constexpr auto MaxSize = static_cast<stacktrace::size_type>(-1);
    static_assert(MaxSize > 10);
    (void) stacktrace::current(20, MaxSize - 10);
}

void test_stacktrace_subscript() {
    const auto st = stacktrace::current();
    (void) st[st.size()];
}
#endif // _HAS_CXX23

int main(int argc, char* argv[]) {
    std_testing::death_test_executive exec;

    exec.add_death_tests({
        test_array_subscript,
        test_array_subscript_const,
        test_array_zero_subscript,
        test_array_zero_subscript_const,
        test_array_zero_front,
        test_array_zero_front_const,
        test_array_zero_back,
        test_array_zero_back_const,
        test_bitset_subscript,
        test_bitset_subscript_const,
        test_deque_subscript,
        test_deque_subscript_const,
        test_deque_front,
        test_deque_front_const,
        test_deque_back,
        test_deque_back_const,
        test_deque_pop_front,
        test_deque_pop_back,
        test_forward_list_front,
        test_forward_list_front_const,
        test_forward_list_pop_front,
        test_list_front,
        test_list_front_const,
        test_list_back,
        test_list_back_const,
        test_list_pop_front,
        test_list_pop_back,
        test_shared_ptr_bounded_array_subscript_underflow,
        test_shared_ptr_bounded_array_subscript_overflow,
        test_shared_ptr_unbounded_array_subscript_underflow,
        test_string_subscript,
        test_string_subscript_const,
        test_string_front,
        test_string_front_const,
        test_string_back,
        test_string_back_const,
        test_string_pop_back,
        test_valarray_subscript,
        test_valarray_subscript_const,
        test_vector_subscript,
        test_vector_subscript_const,
        test_vector_front,
        test_vector_front_const,
        test_vector_back,
        test_vector_back_const,
        test_vector_pop_back,
        test_vector_bool_subscript,
        test_vector_bool_subscript_const,
        test_vector_bool_front,
        test_vector_bool_front_const,
        test_vector_bool_back,
        test_vector_bool_back_const,
        test_vector_bool_pop_back,

#if _HAS_CXX17
        test_optional_deref_lvalue,
        test_optional_deref_lvalue_const,
        test_optional_deref_rvalue,
        test_optional_deref_rvalue_const,
        test_optional_arrow,
        test_optional_arrow_const,
        test_string_view_subscript,
        test_string_view_front,
        test_string_view_back,
        test_string_view_remove_prefix,
        test_string_view_remove_suffix,
#endif // _HAS_CXX17

#if _HAS_CXX20
        test_ranges_view_interface_subscript,
        test_ranges_view_interface_subscript_const,
        test_ranges_view_interface_front,
        test_ranges_view_interface_front_const,
        test_ranges_view_interface_back,
        test_ranges_view_interface_back_const,
        test_span_ctor_first_count,
        test_span_ctor_first_last,
        test_span_ctor_range,
        test_span_ctor_other,
        test_span_first_compiletime,
        test_span_first_runtime,
        test_span_last_compiletime,
        test_span_last_runtime,
        test_span_subspan_compiletime_bad_offset,
        test_span_subspan_compiletime_bad_count,
        test_span_subspan_runtime_bad_offset,
        test_span_subspan_runtime_bad_count,
        test_span_subscript,
        test_span_front,
        test_span_back,
#endif // _HAS_CXX20

#if _HAS_CXX23
        test_expected_arrow,
        test_expected_arrow_const,
        test_expected_deref_lvalue,
        test_expected_deref_lvalue_const,
        test_expected_deref_rvalue,
        test_expected_deref_rvalue_const,
        test_expected_error_lvalue,
        test_expected_error_lvalue_const,
        test_expected_error_rvalue,
        test_expected_error_rvalue_const,
        test_expected_void_deref,
        test_expected_void_error_lvalue,
        test_expected_void_error_lvalue_const,
        test_expected_void_error_rvalue,
        test_expected_void_error_rvalue_const,
        test_mdspan_ctor_other,
#ifdef __cpp_multidimensional_subscript // TRANSITION, P2128R6
        test_mdspan_subscript_multidim,
#endif // ^^^ defined(__cpp_multidimensional_subscript) ^^^
        test_mdspan_subscript_array,
        test_mdspan_subscript_span,
        test_stacktrace_current,
        test_stacktrace_subscript,
#endif // _HAS_CXX23
    });

    return exec.run(argc, argv);
}
