// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _HAS_DEPRECATED_IS_LITERAL_TYPE 1
#define _HAS_DEPRECATED_NEGATORS        1
#define _SILENCE_CXX17_IS_LITERAL_TYPE_DEPRECATION_WARNING
#define _SILENCE_CXX17_NEGATORS_DEPRECATION_WARNING
#define _SILENCE_CXX20_IS_POD_DEPRECATION_WARNING

#include <algorithm>
#include <array>
#include <cassert>
#ifndef _M_CEE_PURE
#include <atomic>
#endif // _M_CEE_PURE
#include <bitset>
#include <chrono>
#include <complex>
#include <functional>
#ifndef _M_CEE
#include <future>
#endif // _M_CEE
#include <initializer_list>
#include <ios>
#include <iterator>
#include <limits>
#include <locale>
#include <memory>
#ifndef _M_CEE
#include <mutex>
#endif // _M_CEE
#include <cstdint>
#include <new>
#include <numeric>
#include <random>
#include <ratio>
#include <regex>
#include <string>
#include <system_error>
#include <tuple>
#include <type_traits>
#include <utility>

using namespace std;
using namespace std::chrono;
namespace RC = std::regex_constants;

constexpr auto int32_min = numeric_limits<int32_t>::min();
constexpr auto int32_max = numeric_limits<int32_t>::max();
constexpr auto int64_min = numeric_limits<int64_t>::min();
constexpr auto int64_max = numeric_limits<int64_t>::max();

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

constexpr initializer_list<int> il;
STATIC_ASSERT(il.size() == 0);
STATIC_ASSERT(il.begin() == il.end());
STATIC_ASSERT(begin(il) == end(il));

// TRANSITION,
// constexpr error_category() noexcept;

constexpr int i = 1729;
STATIC_ASSERT(forward<const int&>(i) == 1729);
STATIC_ASSERT(forward<const int>(static_cast<const int&&>(i)) == 1729);
STATIC_ASSERT(move(i) == 1729);
STATIC_ASSERT(move_if_noexcept(i) == 1729);

STATIC_ASSERT(as_const(i) == 1729);

constexpr pair<int, int> p1;
STATIC_ASSERT(p1.first == 0 && p1.second == 0);
constexpr pair<int, int> p2(p1);
STATIC_ASSERT(p2.first == 0 && p2.second == 0);
constexpr pair<int, int> p3(move(const_cast<pair<int, int>&>(p1)));
STATIC_ASSERT(p3.first == 0 && p3.second == 0);
constexpr int k = 19937;
constexpr pair<int, int> p4(i, k);
STATIC_ASSERT(p4.first == 1729 && p4.second == 19937);
constexpr short sh1 = 123;
constexpr short sh2 = 456;
constexpr pair<int, int> p5(sh1, sh2);
STATIC_ASSERT(p5.first == 123 && p5.second == 456);
constexpr pair<long, long> p6(p4);
STATIC_ASSERT(p6.first == 1729 && p6.second == 19937);
constexpr pair<long, long> p7(move(const_cast<pair<int, int>&>(p4)));
STATIC_ASSERT(p7.first == 1729 && p7.second == 19937);

STATIC_ASSERT(!(p1 == p4));
STATIC_ASSERT(p1 < p4);
STATIC_ASSERT(p1 != p4);
STATIC_ASSERT(!(p1 > p4));
STATIC_ASSERT(!(p1 >= p4));
STATIC_ASSERT(p1 <= p4);

STATIC_ASSERT(make_pair(i, k) == p4);

constexpr pair<short, int> p8(sh2, i);
STATIC_ASSERT(get<0>(p8) == 456);
STATIC_ASSERT(get<1>(p8) == 1729);
STATIC_ASSERT(get<short>(p8) == 456);
STATIC_ASSERT(get<int>(p8) == 1729);
STATIC_ASSERT(get<0>(const_cast<pair<short, int>&>(p8)) == 456);
STATIC_ASSERT(get<1>(const_cast<pair<short, int>&>(p8)) == 1729);
STATIC_ASSERT(get<short>(const_cast<pair<short, int>&>(p8)) == 456);
STATIC_ASSERT(get<int>(const_cast<pair<short, int>&>(p8)) == 1729);
STATIC_ASSERT(get<0>(move(const_cast<pair<short, int>&>(p8))) == 456);
STATIC_ASSERT(get<1>(move(const_cast<pair<short, int>&>(p8))) == 1729);
STATIC_ASSERT(get<short>(move(const_cast<pair<short, int>&>(p8))) == 456);
STATIC_ASSERT(get<int>(move(const_cast<pair<short, int>&>(p8))) == 1729);

constexpr piecewise_construct_t piecewise_construct2 = piecewise_construct;

constexpr tuple<int, int, int> t1;
STATIC_ASSERT(get<0>(t1) == 0 && get<1>(t1) == 0 && get<2>(t1) == 0);
constexpr tuple<int, int, int> t2(t1);
STATIC_ASSERT(get<0>(t2) == 0 && get<1>(t2) == 0 && get<2>(t2) == 0);
constexpr tuple<int, int, int> t3(move(const_cast<tuple<int, int, int>&>(t1)));
STATIC_ASSERT(get<0>(t3) == 0 && get<1>(t3) == 0 && get<2>(t3) == 0);
constexpr int x = 15;
constexpr int y = 25;
constexpr int z = 35;
constexpr tuple<int, int, int> t4(x, y, z);
STATIC_ASSERT(get<0>(t4) == 15 && get<1>(t4) == 25 && get<2>(t4) == 35);
constexpr short sh3 = 45;
constexpr short sh4 = 55;
constexpr short sh5 = 65;
constexpr tuple<int, int, int> t5(sh3, sh4, sh5);
STATIC_ASSERT(get<0>(t5) == 45 && get<1>(t5) == 55 && get<2>(t5) == 65);
constexpr tuple<long, long, long> t6(t4);
STATIC_ASSERT(get<0>(t6) == 15 && get<1>(t6) == 25 && get<2>(t6) == 35);
constexpr tuple<long, long, long> t7(move(const_cast<tuple<int, int, int>&>(t4)));
STATIC_ASSERT(get<0>(t7) == 15 && get<1>(t7) == 25 && get<2>(t7) == 35);
constexpr tuple<int, int> t8(p4);
STATIC_ASSERT(get<0>(t8) == 1729 && get<1>(t8) == 19937);
constexpr tuple<int, int> t9(move(const_cast<pair<int, int>&>(p4)));
STATIC_ASSERT(get<0>(t9) == 1729 && get<1>(t9) == 19937);

constexpr tuple<> tx1;
constexpr tuple<> tx2(tx1);
constexpr tuple<> tx3(move(const_cast<tuple<>&>(tx1)));

STATIC_ASSERT(!(t1 == t4));
STATIC_ASSERT(t1 < t4);
STATIC_ASSERT(t1 != t4);
STATIC_ASSERT(!(t1 > t4));
STATIC_ASSERT(t1 <= t4);
STATIC_ASSERT(!(t1 >= t4));

constexpr tuple<int, int, short> t10(x, y, sh4);
STATIC_ASSERT(get<2>(t10) == 55);
STATIC_ASSERT(get<2>(const_cast<tuple<int, int, short>&>(t10)) == 55);
STATIC_ASSERT(get<2>(move(const_cast<tuple<int, int, short>&>(t10))) == 55);
STATIC_ASSERT(get<short>(t10) == 55);
STATIC_ASSERT(get<short>(const_cast<tuple<int, int, short>&>(t10)) == 55);
STATIC_ASSERT(get<short>(move(const_cast<tuple<int, int, short>&>(t10))) == 55);

STATIC_ASSERT(make_tuple(x, y, z) == t4);
STATIC_ASSERT(forward_as_tuple(x, y, z) == t4);
STATIC_ASSERT(tie(x, y, z) == t4);
STATIC_ASSERT(tuple_cat(t4, t8, t10) == make_tuple(15, 25, 35, 1729, 19937, 15, 25, 55));

#if _HAS_CXX17
STATIC_ASSERT(apply(plus<>{}, p5) == 579);

struct Point {
    int m_a;
    int m_b;
    int m_c;

    constexpr Point(int a, int b, int c) : m_a(a + 1), m_b(b + 2), m_c(c + 3) {}
};

STATIC_ASSERT(make_from_tuple<Point>(t4).m_b == 27);
#endif // _HAS_CXX17

struct Im {};
struct Ex {};

struct Obj {
    constexpr Obj(Im) {}
    constexpr explicit Obj(Ex) {}
};

constexpr Im im{};
constexpr Ex ex{};
constexpr Obj obj(im);
constexpr pair<Im, Im> pim(im, im);
constexpr pair<Ex, Ex> pex(ex, ex);
constexpr tuple<Im, Im> tim(im, im);
constexpr tuple<Ex, Ex> tex(ex, ex);

constexpr pair<Obj, Obj> p9(obj, obj);
constexpr pair<Obj, Obj> p10(im, im);
constexpr pair<Obj, Obj> p11(ex, ex);
constexpr pair<Obj, Obj> p12(pim);
constexpr pair<Obj, Obj> p13(pex);

constexpr tuple<Obj, Obj> t11(obj, obj);
constexpr tuple<Obj, Obj> t12(im, im);
constexpr tuple<Obj, Obj> t13(ex, ex);
constexpr tuple<Obj, Obj> t14(tim);
constexpr tuple<Obj, Obj> t15(tex);
constexpr tuple<Obj, Obj> t16(pim);
constexpr tuple<Obj, Obj> t17(pex);

STATIC_ASSERT(integer_sequence<int, 11, 22, 33, 44>::size() == 4);

constexpr bitset<5> bits{};
STATIC_ASSERT(bits[2] == false);
STATIC_ASSERT(bits.size() == 5);

constexpr bitset<15> bits15(0xFEDCBA9876543210ULL);
STATIC_ASSERT(bits15[4] == true);
STATIC_ASSERT(bits15[5] == false);
constexpr bitset<75> bits75(0xFEDCBA9876543210ULL);
STATIC_ASSERT(bits75[60] == true);
STATIC_ASSERT(bits75[70] == false);

constexpr allocator_arg_t allocator_arg2 = allocator_arg;

STATIC_ASSERT(addressof(i) == &i);

constexpr default_delete<int> dd1{};
constexpr default_delete<int[]> dd2{};

enum class Animal : uint32_t { Ape, Bat, Cat, Dog };

struct TestConstexprCtors {
    unique_ptr<int> up1 = {};
    unique_ptr<int> up2{nullptr};
    unique_ptr<int[]> up3 = {};
    unique_ptr<int[]> up4{nullptr};

    shared_ptr<int> sp1 = {};
    shared_ptr<int> sp2{nullptr};

    weak_ptr<int> wp = {};

#ifndef _M_CEE_PURE
    atomic<Animal> atom1{Animal::Cat};
    atomic<uint64_t> atom2{1729ULL};
    atomic<double*> atom3{nullptr};
#endif // _M_CEE_PURE

    constexpr TestConstexprCtors() {}
};

struct Esft : enable_shared_from_this<Esft> {
    using Base = enable_shared_from_this<Esft>;

    constexpr Esft() : Base() {}
};

STATIC_ASSERT(plus<int>()(1700, 29) == 1729);
STATIC_ASSERT(minus<int>()(1700, 29) == 1671);
STATIC_ASSERT(multiplies<int>()(1700, 29) == 49300);
STATIC_ASSERT(divides<int>()(1700, 29) == 58);
STATIC_ASSERT(modulus<int>()(1700, 29) == 18);
STATIC_ASSERT(negate<int>()(123) == -123);
STATIC_ASSERT(equal_to<int>()(11, 22) == false);
STATIC_ASSERT(not_equal_to<int>()(11, 22) == true);
STATIC_ASSERT(greater<int>()(11, 22) == false);
STATIC_ASSERT(less<int>()(11, 22) == true);
STATIC_ASSERT(greater_equal<int>()(11, 22) == false);
STATIC_ASSERT(less_equal<int>()(11, 22) == true);
STATIC_ASSERT(logical_and<bool>()(true, false) == false);
STATIC_ASSERT(logical_or<bool>()(true, false) == true);
STATIC_ASSERT(logical_not<bool>()(true) == false);
STATIC_ASSERT(bit_and<unsigned int>()(0x53F8133EU, 0xD90D581EU) == 0x5108101EU);
STATIC_ASSERT(bit_or<unsigned int>()(0x53F8133EU, 0xD90D581EU) == 0xDBFD5B3EU);
STATIC_ASSERT(bit_xor<unsigned int>()(0x53F8133EU, 0xD90D581EU) == 0x8AF54B20U);
STATIC_ASSERT(bit_not<unsigned int>()(0x53F8133EU) == 0xAC07ECC1U);

STATIC_ASSERT(plus<>()(1700, 29) == 1729);
STATIC_ASSERT(minus<>()(1700, 29) == 1671);
STATIC_ASSERT(multiplies<>()(1700, 29) == 49300);
STATIC_ASSERT(divides<>()(1700, 29) == 58);
STATIC_ASSERT(modulus<>()(1700, 29) == 18);
STATIC_ASSERT(negate<>()(123) == -123);
STATIC_ASSERT(equal_to<>()(11, 22) == false);
STATIC_ASSERT(not_equal_to<>()(11, 22) == true);
STATIC_ASSERT(greater<>()(11, 22) == false);
STATIC_ASSERT(less<>()(11, 22) == true);
STATIC_ASSERT(greater_equal<>()(11, 22) == false);
STATIC_ASSERT(less_equal<>()(11, 22) == true);
STATIC_ASSERT(logical_and<>()(true, false) == false);
STATIC_ASSERT(logical_or<>()(true, false) == true);
STATIC_ASSERT(logical_not<>()(true) == false);
STATIC_ASSERT(bit_and<>()(0x53F8133EU, 0xD90D581EU) == 0x5108101EU);
STATIC_ASSERT(bit_or<>()(0x53F8133EU, 0xD90D581EU) == 0xDBFD5B3EU);
STATIC_ASSERT(bit_xor<>()(0x53F8133EU, 0xD90D581EU) == 0x8AF54B20U);
STATIC_ASSERT(bit_not<>()(0x53F8133EU) == 0xAC07ECC1U);

struct Unary {
    using argument_type = int;

    constexpr Unary() {}

    constexpr Unary(const Unary&) {}

    constexpr bool operator()(const int n) const {
        return n % 2 == 0;
    }
};

constexpr Unary unary{};
STATIC_ASSERT(unary(64) == true);
STATIC_ASSERT(unary(65) == false);
STATIC_ASSERT(not1(unary)(64) == false);
STATIC_ASSERT(not1(unary)(65) == true);

struct Binary {
    using first_argument_type  = int;
    using second_argument_type = int;

    constexpr Binary() {}

    constexpr Binary(const Binary&) {}

    constexpr bool operator()(const int lhs, const int rhs) const {
        return lhs / 10 == rhs / 10;
    }
};

constexpr Binary binary{};
STATIC_ASSERT(binary(64, 65) == true);
STATIC_ASSERT(binary(64, 77) == false);
STATIC_ASSERT(not2(binary)(64, 65) == false);
STATIC_ASSERT(not2(binary)(64, 77) == true);

STATIC_ASSERT(integral_constant<int, 1729>::value == 1729);
constexpr integral_constant<int, 1701> enterprise{};
STATIC_ASSERT(static_cast<int>(enterprise) == 1701);
STATIC_ASSERT(enterprise() == 1701);


// P0006R0 Variable Templates For Type Traits
STATIC_ASSERT(is_void_v<int> == false);
STATIC_ASSERT(is_null_pointer_v<int> == false);
STATIC_ASSERT(is_integral_v<int> == true);
STATIC_ASSERT(is_floating_point_v<int> == false);
STATIC_ASSERT(is_array_v<int> == false);
STATIC_ASSERT(is_pointer_v<int> == false);
STATIC_ASSERT(is_lvalue_reference_v<int> == false);
STATIC_ASSERT(is_rvalue_reference_v<int> == false);
STATIC_ASSERT(is_member_object_pointer_v<int> == false);
STATIC_ASSERT(is_member_function_pointer_v<int> == false);
STATIC_ASSERT(is_enum_v<int> == false);
STATIC_ASSERT(is_union_v<int> == false);
STATIC_ASSERT(is_class_v<int> == false);
STATIC_ASSERT(is_function_v<int> == false);
STATIC_ASSERT(is_reference_v<int> == false);
STATIC_ASSERT(is_arithmetic_v<int> == true);
STATIC_ASSERT(is_fundamental_v<int> == true);
STATIC_ASSERT(is_object_v<int> == true);
STATIC_ASSERT(is_scalar_v<int> == true);
STATIC_ASSERT(is_compound_v<int> == false);
STATIC_ASSERT(is_member_pointer_v<int> == false);
STATIC_ASSERT(is_const_v<int> == false);
STATIC_ASSERT(is_volatile_v<int> == false);
STATIC_ASSERT(is_trivial_v<int> == true);
STATIC_ASSERT(is_trivially_copyable_v<int> == true);
STATIC_ASSERT(is_standard_layout_v<int> == true);
STATIC_ASSERT(is_pod_v<int> == true);
STATIC_ASSERT(is_literal_type_v<int> == true);
STATIC_ASSERT(is_empty_v<int> == false);
STATIC_ASSERT(is_polymorphic_v<int> == false);
STATIC_ASSERT(is_abstract_v<int> == false);
STATIC_ASSERT(is_final_v<int> == false);
STATIC_ASSERT(is_signed_v<int> == true);
STATIC_ASSERT(is_unsigned_v<int> == false);
#if _HAS_CXX20
STATIC_ASSERT(is_bounded_array_v<int> == false);
STATIC_ASSERT(is_unbounded_array_v<int> == false);
#endif // _HAS_CXX20
STATIC_ASSERT(is_constructible_v<int, int> == true);
STATIC_ASSERT(is_default_constructible_v<int> == true);
STATIC_ASSERT(is_copy_constructible_v<int> == true);
STATIC_ASSERT(is_move_constructible_v<int> == true);
STATIC_ASSERT(is_assignable_v<int&, int> == true);
STATIC_ASSERT(is_copy_assignable_v<int> == true);
STATIC_ASSERT(is_move_assignable_v<int> == true);
#if _HAS_CXX17
STATIC_ASSERT(is_swappable_with_v<int&, int&> == true);
STATIC_ASSERT(is_swappable_v<int> == true);
#endif // _HAS_CXX17
STATIC_ASSERT(is_destructible_v<int> == true);
STATIC_ASSERT(is_trivially_constructible_v<int, int> == true);
STATIC_ASSERT(is_trivially_default_constructible_v<int> == true);
STATIC_ASSERT(is_trivially_copy_constructible_v<int> == true);
STATIC_ASSERT(is_trivially_move_constructible_v<int> == true);
STATIC_ASSERT(is_trivially_assignable_v<int&, int> == true);
STATIC_ASSERT(is_trivially_copy_assignable_v<int> == true);
STATIC_ASSERT(is_trivially_move_assignable_v<int> == true);
STATIC_ASSERT(is_trivially_destructible_v<int> == true);
STATIC_ASSERT(is_nothrow_constructible_v<int, int> == true);
STATIC_ASSERT(is_nothrow_default_constructible_v<int> == true);
STATIC_ASSERT(is_nothrow_copy_constructible_v<int> == true);
STATIC_ASSERT(is_nothrow_move_constructible_v<int> == true);
STATIC_ASSERT(is_nothrow_assignable_v<int&, int> == true);
STATIC_ASSERT(is_nothrow_copy_assignable_v<int> == true);
STATIC_ASSERT(is_nothrow_move_assignable_v<int> == true);
#if _HAS_CXX17
STATIC_ASSERT(is_nothrow_swappable_with_v<int&, int&> == true);
STATIC_ASSERT(is_nothrow_swappable_v<int> == true);
#endif // _HAS_CXX17
STATIC_ASSERT(is_nothrow_destructible_v<int> == true);
STATIC_ASSERT(has_virtual_destructor_v<int> == false);
STATIC_ASSERT(alignment_of_v<char> == 1);
STATIC_ASSERT(rank_v<int[11][22][33]> == 3);
STATIC_ASSERT(extent_v<int[11][22][33]> == 11);
STATIC_ASSERT(extent_v<int[11][22][33], 1> == 22);
STATIC_ASSERT(is_same_v<int, int> == true);
STATIC_ASSERT(is_base_of_v<int, double> == false);
STATIC_ASSERT(is_convertible_v<short, int> == true);
#if _HAS_CXX17
STATIC_ASSERT(is_invocable_v<plus<>, int, int> == true);
STATIC_ASSERT(is_invocable_r_v<string, plus<>, int, int> == false);
STATIC_ASSERT(is_nothrow_invocable_v<plus<>, string, string> == false);
STATIC_ASSERT(is_nothrow_invocable_r_v<int, plus<>, string, string> == false);
#endif // _HAS_CXX17
STATIC_ASSERT(conjunction_v<true_type, true_type> == true);
STATIC_ASSERT(disjunction_v<false_type, false_type> == false);
STATIC_ASSERT(negation_v<true_type> == false);

STATIC_ASSERT(tuple_size_v<tuple<int, int, int>> == 3);

STATIC_ASSERT(ratio_equal_v<mega, mega> == true);
STATIC_ASSERT(ratio_not_equal_v<mega, mega> == false);
STATIC_ASSERT(ratio_less_v<mega, mega> == false);
STATIC_ASSERT(ratio_less_equal_v<mega, mega> == true);
STATIC_ASSERT(ratio_greater_v<mega, mega> == false);
STATIC_ASSERT(ratio_greater_equal_v<mega, mega> == true);

STATIC_ASSERT(is_error_code_enum_v<io_errc> == true);
STATIC_ASSERT(is_error_condition_enum_v<errc> == true);

STATIC_ASSERT(uses_allocator_v<tuple<int, int>, allocator<int>> == true);

using Bound = decltype(bind(declval<int (*)(int)>(), 1729));
STATIC_ASSERT(is_bind_expression_v<Bound> == true);
using Place = decltype(placeholders::_5);
STATIC_ASSERT(is_placeholder_v<Place> == 5);

STATIC_ASSERT(treat_as_floating_point_v<double> == true);


STATIC_ASSERT(ratio<252, 105>::num == 12);
STATIC_ASSERT(ratio<252, 105>::den == 5);

STATIC_ASSERT(duration_values<int32_t>::zero() == 0);
STATIC_ASSERT(duration_values<int32_t>::min() == int32_min);
STATIC_ASSERT(duration_values<int32_t>::max() == int32_max);

constexpr seconds d1{};
STATIC_ASSERT(d1.count() == 0);

constexpr seconds d2(1729);
STATIC_ASSERT(d2.count() == 1729);
STATIC_ASSERT((+d2).count() == 1729);
STATIC_ASSERT((-d2).count() == -1729);

constexpr seconds d3(d2);
STATIC_ASSERT(d3.count() == 1729);

constexpr seconds d4(47);
constexpr milliseconds d5(d4);
STATIC_ASSERT(d5.count() == 47000);

STATIC_ASSERT(duration<int64_t>::zero().count() == 0);
STATIC_ASSERT(duration<int64_t>::min().count() == int64_min);
STATIC_ASSERT(duration<int64_t>::max().count() == int64_max);

constexpr seconds d6(1700);
constexpr seconds d7(29);
STATIC_ASSERT((d6 + d7).count() == 1729);
STATIC_ASSERT((d6 - d7).count() == 1671);
STATIC_ASSERT((d6 * 11).count() == 18700);
STATIC_ASSERT((12 * d7).count() == 348);
STATIC_ASSERT((d6 / 13).count() == 130);
STATIC_ASSERT(d6 / d7 == 58);
STATIC_ASSERT((d6 % 14).count() == 6);
STATIC_ASSERT((d6 % d7).count() == 18);
STATIC_ASSERT((d6 == d7) == false);
STATIC_ASSERT((d6 != d7) == true);
STATIC_ASSERT((d6 < d7) == false);
STATIC_ASSERT((d6 <= d7) == false);
STATIC_ASSERT((d6 > d7) == true);
STATIC_ASSERT((d6 >= d7) == true);

constexpr milliseconds d8(-262144);
STATIC_ASSERT(duration_cast<seconds>(d8).count() == -262);
STATIC_ASSERT(floor<seconds>(d8).count() == -263);
STATIC_ASSERT(ceil<seconds>(d8).count() == -262);
STATIC_ASSERT(round<seconds>(d8).count() == -262);
STATIC_ASSERT(abs(d8).count() == 262144);

STATIC_ASSERT((123h).count() == 123);
STATIC_ASSERT((123min).count() == 123);
STATIC_ASSERT((123s).count() == 123);
STATIC_ASSERT((123ms).count() == 123);
STATIC_ASSERT((123us).count() == 123);
STATIC_ASSERT((123ns).count() == 123);
STATIC_ASSERT((3.14h).count() > 3.0);
STATIC_ASSERT((3.14min).count() > 3.0);
STATIC_ASSERT((3.14s).count() > 3.0);
STATIC_ASSERT((3.14ms).count() > 3.0);
STATIC_ASSERT((3.14us).count() > 3.0);
STATIC_ASSERT((3.14ns).count() > 3.0);

#if _HAS_CXX17
constexpr bool test_chrono_constexpr_again() {
    milliseconds durEx{8888};
    assert((++durEx).count() == 8889);
    assert(durEx++.count() == 8889);
    assert(durEx.count() == 8890);

    assert((--durEx).count() == 8889);
    assert(durEx--.count() == 8889);
    assert(durEx.count() == 8888);
    assert((durEx += 10ms).count() == 8898);
    assert((durEx -= 10ms).count() == 8888);
    assert((durEx *= 6).count() == 53328);
    assert((durEx /= 10).count() == 5332);
    assert((durEx %= 1000).count() == 332);
    assert((durEx %= 100ms).count() == 32);

    time_point<system_clock, milliseconds> tpEx{1729ms};
    assert((tpEx += 45ms).time_since_epoch().count() == 1774);
    assert((tpEx -= 1004ms).time_since_epoch().count() == 770);

    return true;
}

STATIC_ASSERT(test_chrono_constexpr_again());
#endif // _HAS_CXX17

constexpr time_point<system_clock, seconds> tp1{};
STATIC_ASSERT(tp1.time_since_epoch().count() == 0);

constexpr time_point<system_clock, seconds> tp2(16s);
STATIC_ASSERT(tp2.time_since_epoch().count() == 16);
constexpr time_point<system_clock, milliseconds> tp3(tp2);
STATIC_ASSERT(tp3.time_since_epoch().count() == 16000);
STATIC_ASSERT(time_point<system_clock, duration<int32_t>>::min().time_since_epoch().count() == int32_min);
STATIC_ASSERT(time_point<system_clock, duration<int32_t>>::max().time_since_epoch().count() == int32_max);

constexpr time_point<system_clock, seconds> tp4(1000s);
constexpr time_point<system_clock, seconds> tp5(729s);
STATIC_ASSERT((tp4 + 24s).time_since_epoch().count() == 1024);
STATIC_ASSERT((1s + tp5).time_since_epoch().count() == 730);
STATIC_ASSERT((tp4 - 8s).time_since_epoch().count() == 992);
STATIC_ASSERT((tp4 - tp5).count() == 271);
STATIC_ASSERT((tp4 == tp5) == false);
STATIC_ASSERT((tp4 != tp5) == true);
STATIC_ASSERT((tp4 < tp5) == false);
STATIC_ASSERT((tp4 <= tp5) == false);
STATIC_ASSERT((tp4 > tp5) == true);
STATIC_ASSERT((tp4 >= tp5) == true);
STATIC_ASSERT(time_point_cast<minutes>(tp5).time_since_epoch().count() == 12);
STATIC_ASSERT(floor<minutes>(tp5).time_since_epoch().count() == 12);
STATIC_ASSERT(ceil<minutes>(tp5).time_since_epoch().count() == 13);
STATIC_ASSERT(round<minutes>(tp5).time_since_epoch().count() == 12);

STATIC_ASSERT(system_clock::is_steady == false); // for VC
STATIC_ASSERT(steady_clock::is_steady == true);
STATIC_ASSERT(high_resolution_clock::is_steady == true); // for VC

STATIC_ASSERT(char_traits<char>::eq('x', 'x') == true);
STATIC_ASSERT(char_traits<char>::lt('a', 'b') == true);
STATIC_ASSERT(char_traits<char>::not_eof('c') == 'c');
STATIC_ASSERT(char_traits<char>::to_char_type('d') == 'd');
STATIC_ASSERT(char_traits<char>::to_int_type('e') == 'e');
STATIC_ASSERT(char_traits<char>::eq_int_type('y', 'y') == true);
STATIC_ASSERT(char_traits<char>::eq_int_type(char_traits<char>::eof(), 'z') == false);

STATIC_ASSERT(char_traits<wchar_t>::eq(L'x', L'x') == true);
STATIC_ASSERT(char_traits<wchar_t>::lt(L'a', L'b') == true);
STATIC_ASSERT(char_traits<wchar_t>::not_eof(L'c') == L'c');
STATIC_ASSERT(char_traits<wchar_t>::to_char_type(L'd') == L'd');
STATIC_ASSERT(char_traits<wchar_t>::to_int_type(L'e') == L'e');
STATIC_ASSERT(char_traits<wchar_t>::eq_int_type(L'y', L'y') == true);
STATIC_ASSERT(char_traits<wchar_t>::eq_int_type(char_traits<wchar_t>::eof(), L'z') == false);

#ifdef __cpp_char8_t
STATIC_ASSERT(char_traits<char8_t>::eq(u8'x', u8'x') == true);
STATIC_ASSERT(char_traits<char8_t>::lt(u8'a', u8'b') == true);
STATIC_ASSERT(char_traits<char8_t>::not_eof(u8'c') == u8'c');
STATIC_ASSERT(char_traits<char8_t>::to_char_type(u8'd') == u8'd');
STATIC_ASSERT(char_traits<char8_t>::to_int_type(u8'e') == u8'e');
STATIC_ASSERT(char_traits<char8_t>::eq_int_type(u8'y', u8'y') == true);
STATIC_ASSERT(char_traits<char8_t>::eq_int_type(char_traits<char8_t>::eof(), u8'z') == false);
#endif // __cpp_char8_t

STATIC_ASSERT(char_traits<char16_t>::eq(u'x', u'x') == true);
STATIC_ASSERT(char_traits<char16_t>::lt(u'a', u'b') == true);
STATIC_ASSERT(char_traits<char16_t>::not_eof(u'c') == u'c');
STATIC_ASSERT(char_traits<char16_t>::to_char_type(u'd') == u'd');
STATIC_ASSERT(char_traits<char16_t>::to_int_type(u'e') == u'e');
STATIC_ASSERT(char_traits<char16_t>::eq_int_type(u'y', u'y') == true);
STATIC_ASSERT(char_traits<char16_t>::eq_int_type(char_traits<char16_t>::eof(), u'z') == false);

STATIC_ASSERT(char_traits<char32_t>::eq(U'x', U'x') == true);
STATIC_ASSERT(char_traits<char32_t>::lt(U'a', U'b') == true);
STATIC_ASSERT(char_traits<char32_t>::not_eof(U'c') == U'c');
STATIC_ASSERT(char_traits<char32_t>::to_char_type(U'd') == U'd');
STATIC_ASSERT(char_traits<char32_t>::to_int_type(U'e') == U'e');
STATIC_ASSERT(char_traits<char32_t>::eq_int_type(U'y', U'y') == true);
STATIC_ASSERT(char_traits<char32_t>::eq_int_type(char_traits<char32_t>::eof(), U'z') == false);

constexpr array<int, 4> arr = {{11, 22, 33, 44}};
STATIC_ASSERT(arr.size() == 4);
STATIC_ASSERT(arr.max_size() > 3);
STATIC_ASSERT(arr.empty() == false);

STATIC_ASSERT(arr[1] == 22);
STATIC_ASSERT(arr.at(2) == 33);
STATIC_ASSERT(arr.front() == 11);
STATIC_ASSERT(arr.back() == 44);
STATIC_ASSERT(get<1>(arr) == 22);
STATIC_ASSERT(get<2>(const_cast<array<int, 4>&>(arr)) == 33);
STATIC_ASSERT(get<3>(move(const_cast<array<int, 4>&>(arr))) == 44);

constexpr array<int, 0> arr0 = {{}};
STATIC_ASSERT(arr0.size() == 0);
STATIC_ASSERT(arr0.max_size() != 5);
STATIC_ASSERT(arr0.empty() == true);

constexpr istream_iterator<int> istream_it{};

constexpr istreambuf_iterator<char> istreambuf_it{};

constexpr int raw[] = {10, 20, 30, 40, 50};
STATIC_ASSERT(begin(raw) == raw);
STATIC_ASSERT(end(raw) == raw + 5);
STATIC_ASSERT(cbegin(raw) == raw);
STATIC_ASSERT(cend(raw) == raw + 5);
STATIC_ASSERT(data(raw) == raw);
STATIC_ASSERT(size(raw) == 5);
STATIC_ASSERT(empty(raw) == false);

STATIC_ASSERT(size(arr) == 4);
STATIC_ASSERT(empty(arr) == false);

#if _HAS_CXX20
STATIC_ASSERT(ssize(raw) == 5);
STATIC_ASSERT(ssize(arr) == 4);
#endif // _HAS_CXX20

struct MyContainer {
    constexpr const int* data() const {
        return nullptr;
    }
};
constexpr MyContainer mycontainer{};
STATIC_ASSERT(data(mycontainer) == nullptr);
STATIC_ASSERT(data(const_cast<MyContainer&>(mycontainer)) == nullptr);

STATIC_ASSERT(empty(il) == true);
STATIC_ASSERT(data(il) == il.begin());

STATIC_ASSERT(min(101, 202) == 101);
STATIC_ASSERT(min(303, 404, greater<>()) == 404);
STATIC_ASSERT(max(505, 606) == 606);
STATIC_ASSERT(max(707, 808, greater<>()) == 707);
STATIC_ASSERT(minmax(1729, 64).first == 64);
STATIC_ASSERT(minmax(27, 81, greater<>()).first == 81);
#if _HAS_CXX17
STATIC_ASSERT(clamp(-1, 0, 100) == 0);
STATIC_ASSERT(clamp(200, 0, 100) == 100);
STATIC_ASSERT(clamp(50, 0, 100) == 50);
STATIC_ASSERT(clamp(-1, 100, 0, greater<>()) == 0);
STATIC_ASSERT(clamp(200, 100, 0, greater<>()) == 100);
STATIC_ASSERT(clamp(50, 100, 0, greater<>()) == 50);
#endif // _HAS_CXX17

STATIC_ASSERT(min({36, 25, 64, 49}) == 25);
STATIC_ASSERT(min({36, 25, 64, 49}, greater<>()) == 64);
STATIC_ASSERT(max({36, 25, 64, 49}) == 64);
STATIC_ASSERT(max({36, 25, 64, 49}, greater<>()) == 25);
STATIC_ASSERT(minmax({36, 25, 64, 49}) == make_pair(25, 64));
STATIC_ASSERT(minmax({36, 25, 64, 49}, greater<>()) == make_pair(64, 25));

constexpr int squares[] = {36, 25, 64, 49};

STATIC_ASSERT(min_element(squares, squares + 4) == squares + 1);
STATIC_ASSERT(min_element(squares, squares + 4, greater<>()) == squares + 2);
STATIC_ASSERT(max_element(squares, squares + 4) == squares + 2);
STATIC_ASSERT(max_element(squares, squares + 4, greater<>()) == squares + 1);
STATIC_ASSERT(minmax_element(squares, squares + 4) == make_pair(squares + 1, squares + 2));
STATIC_ASSERT(minmax_element(squares, squares + 4, greater<>()) == make_pair(squares + 2, squares + 1));

constexpr complex<float> cf1(12.0f, 34.0f);
STATIC_ASSERT(cf1.real() == 12.0f);
STATIC_ASSERT(cf1.imag() == 34.0f);
constexpr complex<double> cd1(56.0, 78.0);
STATIC_ASSERT(cd1.real() == 56.0);
STATIC_ASSERT(cd1.imag() == 78.0);
constexpr complex<long double> cl1(91.0L, 92.0L);
STATIC_ASSERT(cl1.real() == 91.0L);
STATIC_ASSERT(cl1.imag() == 92.0L);
constexpr complex<float> cf2(cd1);
STATIC_ASSERT(cf2.real() == 56.0f);
STATIC_ASSERT(cf2.imag() == 78.0f);
constexpr complex<float> cf3(cl1);
STATIC_ASSERT(cf3.real() == 91.0f);
STATIC_ASSERT(cf3.imag() == 92.0f);
constexpr complex<double> cd2(cf1);
STATIC_ASSERT(cd2.real() == 12.0);
STATIC_ASSERT(cd2.imag() == 34.0);
constexpr complex<double> cd3(cl1);
STATIC_ASSERT(cd3.real() == 91.0);
STATIC_ASSERT(cd3.imag() == 92.0);
constexpr complex<long double> cl2(cf1);
STATIC_ASSERT(cl2.real() == 12.0L);
STATIC_ASSERT(cl2.imag() == 34.0L);
constexpr complex<long double> cl3(cd1);
STATIC_ASSERT(cl3.real() == 56.0L);
STATIC_ASSERT(cl3.imag() == 78.0L);
STATIC_ASSERT((cd1 == cd2) == false);
STATIC_ASSERT((cd1 == 9.0) == false);
STATIC_ASSERT((9.0 == cd2) == false);
STATIC_ASSERT((cd1 != cd2) == true);
STATIC_ASSERT((cd1 != 9.0) == true);
STATIC_ASSERT((9.0 != cd2) == true);
STATIC_ASSERT(real(cd1) == 56.0);
STATIC_ASSERT(imag(cd1) == 78.0);

STATIC_ASSERT((10if).imag() == 10.0f);
STATIC_ASSERT((20.0if).imag() == 20.0f);
STATIC_ASSERT((30i).imag() == 30.0);
STATIC_ASSERT((40.0i).imag() == 40.0);
STATIC_ASSERT((50il).imag() == 50.0L);
STATIC_ASSERT((60.0il).imag() == 60.0L);

STATIC_ASSERT(minstd_rand0::multiplier == 16807);
STATIC_ASSERT(minstd_rand0::increment == 0);
STATIC_ASSERT(minstd_rand0::modulus == 2147483647);
STATIC_ASSERT(minstd_rand0::min() == 1);
STATIC_ASSERT(minstd_rand0::max() == 2147483646);
STATIC_ASSERT(minstd_rand0::default_seed == 1);

STATIC_ASSERT(mt19937::word_size == 32);
STATIC_ASSERT(mt19937::state_size == 624);
STATIC_ASSERT(mt19937::shift_size == 397);
STATIC_ASSERT(mt19937::mask_bits == 31);
STATIC_ASSERT(mt19937::xor_mask == 0x9908B0DFU);
STATIC_ASSERT(mt19937::tempering_u == 11);
STATIC_ASSERT(mt19937::tempering_d == 0xFFFFFFFFU);
STATIC_ASSERT(mt19937::tempering_s == 7);
STATIC_ASSERT(mt19937::tempering_b == 0x9D2C5680U);
STATIC_ASSERT(mt19937::tempering_t == 15);
STATIC_ASSERT(mt19937::tempering_c == 0xEFC60000U);
STATIC_ASSERT(mt19937::tempering_l == 18);
STATIC_ASSERT(mt19937::initialization_multiplier == 1812433253);
STATIC_ASSERT(mt19937::min() == 0);
STATIC_ASSERT(mt19937::max() == 0xFFFFFFFFU);
STATIC_ASSERT(mt19937::default_seed == 5489);

STATIC_ASSERT(ranlux24_base::word_size == 24);
STATIC_ASSERT(ranlux24_base::short_lag == 10);
STATIC_ASSERT(ranlux24_base::long_lag == 24);
STATIC_ASSERT(ranlux24_base::min() == 0);
STATIC_ASSERT(ranlux24_base::max() == 16777215);
STATIC_ASSERT(ranlux24_base::default_seed == 19780503);

STATIC_ASSERT(ranlux24::block_size == 223);
STATIC_ASSERT(ranlux24::used_block == 23);
STATIC_ASSERT(ranlux24::min() == 0);
STATIC_ASSERT(ranlux24::max() == 16777215);

STATIC_ASSERT(independent_bits_engine<mt19937, 47, uint64_t>::min() == 0);
STATIC_ASSERT(independent_bits_engine<mt19937, 47, uint64_t>::max() == 0x7FFFFFFFFFFFULL);

STATIC_ASSERT(knuth_b::table_size == 256);
STATIC_ASSERT(knuth_b::min() == 1);
STATIC_ASSERT(knuth_b::max() == 2147483646);

STATIC_ASSERT(random_device::min() == numeric_limits<random_device::result_type>::min());
STATIC_ASSERT(random_device::max() == numeric_limits<random_device::result_type>::max());

template <typename T, T... Values>
void test_constants() {}

void test_all_constants() {
    test_constants<ios_base::fmtflags, ios_base::boolalpha, ios_base::dec, ios_base::fixed, ios_base::hex,
        ios_base::internal, ios_base::left, ios_base::oct, ios_base::right, ios_base::scientific, ios_base::showbase,
        ios_base::showpoint, ios_base::showpos, ios_base::skipws, ios_base::unitbuf, ios_base::uppercase,
        ios_base::adjustfield, ios_base::basefield, ios_base::floatfield>();

    test_constants<ios_base::iostate, ios_base::badbit, ios_base::eofbit, ios_base::failbit, ios_base::goodbit>();

    test_constants<ios_base::openmode, ios_base::app, ios_base::ate, ios_base::binary, ios_base::in, ios_base::out,
        ios_base::trunc>();

    test_constants<ios_base::seekdir, ios_base::beg, ios_base::cur, ios_base::end>();

    test_constants<RC::syntax_option_type, RC::icase, RC::nosubs, RC::optimize, RC::collate, RC::ECMAScript, RC::basic,
        RC::extended, RC::awk, RC::grep, RC::egrep, regex::icase, regex::nosubs, regex::optimize, regex::collate,
        regex::ECMAScript, regex::basic, regex::extended, regex::awk, regex::grep, regex::egrep>();

    test_constants<RC::match_flag_type, RC::match_default, RC::match_not_bol, RC::match_not_eol, RC::match_not_bow,
        RC::match_not_eow, RC::match_any, RC::match_not_null, RC::match_continuous, RC::match_prev_avail,
        RC::format_default, RC::format_sed, RC::format_no_copy, RC::format_first_only>();

    test_constants<RC::error_type, RC::error_collate, RC::error_ctype, RC::error_escape, RC::error_backref,
        RC::error_brack, RC::error_paren, RC::error_brace, RC::error_badbrace, RC::error_range, RC::error_space,
        RC::error_badrepeat, RC::error_complexity, RC::error_stack>();
}

constexpr csub_match sm{};
STATIC_ASSERT(sm.first == nullptr);
STATIC_ASSERT(sm.second == nullptr);
STATIC_ASSERT(sm.matched == false);

#ifndef _M_CEE

constexpr defer_lock_t defer_lock2   = defer_lock;
constexpr try_to_lock_t try_to_lock2 = try_to_lock;
constexpr adopt_lock_t adopt_lock2   = adopt_lock;

constexpr once_flag once{};

// TRANSITION,
// constexpr mutex() noexcept;

#endif // _M_CEE

// reverse_iterator and string_view constexpr are tested in P0220R1_string_view

template <typename Bitmask, Bitmask X, Bitmask Y>
void test_bitmask() {
    constexpr Bitmask b1 = X & Y;
    constexpr Bitmask b2 = X | Y;
    constexpr Bitmask b3 = X ^ Y;
    constexpr Bitmask b4 = ~X;

    (void) b1;
    (void) b2;
    (void) b3;
    (void) b4;
}

void test_all_bitmasks() {
    test_bitmask<ctype_base::mask, ctype_base::alpha, ctype_base::digit>();
    test_bitmask<ios_base::fmtflags, ios_base::hex, ios_base::uppercase>();
    test_bitmask<ios_base::iostate, ios_base::eofbit, ios_base::failbit>();
    test_bitmask<ios_base::openmode, ios_base::binary, ios_base::out>();
    test_bitmask<RC::syntax_option_type, RC::icase, RC::nosubs>();
    test_bitmask<RC::match_flag_type, RC::match_not_bol, RC::match_not_eol>();
#ifndef _M_CEE
    test_bitmask<launch, launch::async, launch::deferred>();
#endif // _M_CEE
}

template <typename T>
void test_limits() {
    using NL = numeric_limits<T>;

    constexpr auto nl1  = NL::is_specialized;
    constexpr auto nl2  = NL::min();
    constexpr auto nl3  = NL::max();
    constexpr auto nl4  = NL::lowest();
    constexpr auto nl5  = NL::digits;
    constexpr auto nl6  = NL::digits10;
    constexpr auto nl7  = NL::max_digits10;
    constexpr auto nl8  = NL::is_signed;
    constexpr auto nl9  = NL::is_integer;
    constexpr auto nl10 = NL::is_exact;
    constexpr auto nl11 = NL::radix;
    constexpr auto nl12 = NL::epsilon();
    constexpr auto nl13 = NL::round_error();
    constexpr auto nl14 = NL::min_exponent;
    constexpr auto nl15 = NL::min_exponent10;
    constexpr auto nl16 = NL::max_exponent;
    constexpr auto nl17 = NL::max_exponent10;
    constexpr auto nl18 = NL::has_infinity;
    constexpr auto nl19 = NL::has_quiet_NaN;
    constexpr auto nl20 = NL::has_signaling_NaN;
    constexpr auto nl21 = NL::has_denorm;
    constexpr auto nl22 = NL::has_denorm_loss;
    constexpr auto nl23 = NL::infinity();
    constexpr auto nl24 = NL::quiet_NaN();
    constexpr auto nl25 = NL::signaling_NaN();
    constexpr auto nl26 = NL::denorm_min();
    constexpr auto nl27 = NL::is_iec559;
    constexpr auto nl28 = NL::is_bounded;
    constexpr auto nl29 = NL::is_modulo;
    constexpr auto nl30 = NL::traps;
    constexpr auto nl31 = NL::tinyness_before;
    constexpr auto nl32 = NL::round_style;

    (void) nl1;
    (void) nl2;
    (void) nl3;
    (void) nl4;
    (void) nl5;
    (void) nl6;
    (void) nl7;
    (void) nl8;
    (void) nl9;
    (void) nl10;
    (void) nl11;
    (void) nl12;
    (void) nl13;
    (void) nl14;
    (void) nl15;
    (void) nl16;
    (void) nl17;
    (void) nl18;
    (void) nl19;
    (void) nl20;
    (void) nl21;
    (void) nl22;
    (void) nl23;
    (void) nl24;
    (void) nl25;
    (void) nl26;
    (void) nl27;
    (void) nl28;
    (void) nl29;
    (void) nl30;
    (void) nl31;
    (void) nl32;
}

enum class Color : uint32_t { Red, Green, Blue };

void test_all_limits() {
    test_limits<Color>();
    test_limits<bool>();
    test_limits<char>();
    test_limits<signed char>();
    test_limits<unsigned char>();
#ifdef __cpp_char8_t
    test_limits<char8_t>();
#endif // __cpp_char8_t
    test_limits<char16_t>();
    test_limits<char32_t>();
    test_limits<wchar_t>();
    test_limits<short>();
    test_limits<int>();
    test_limits<long>();
    test_limits<long long>();
    test_limits<unsigned short>();
    test_limits<unsigned int>();
    test_limits<unsigned long>();
    test_limits<unsigned long long>();
    test_limits<float>();
    test_limits<double>();
    test_limits<long double>();
}

template <typename Floating, typename Integral>
void assert_bits(const Floating fval, const Integral ival) {
    assert(reinterpret_cast<const Integral&>(fval) == ival);
}

#if _HAS_CXX17
static_assert(hardware_constructive_interference_size == 64);
static_assert(hardware_destructive_interference_size == 64);
#endif // _HAS_CXX17

// P0295R0 gcd(), lcm()
constexpr bool test_gcd_lcm() {
#if _HAS_CXX17
    assert(gcd(0, 0) == 0);
    assert(gcd(3125, 2401) == 1);
    assert(gcd(3840, 2160) == 240);
    assert(gcd(4096, 8192) == 4096);
    assert(gcd(19937, 19937) == 19937);

    assert(lcm(0, 0) == 0);
    assert(lcm(0, 1729) == 0);
    assert(lcm(1729, 0) == 0);
    assert(lcm(1729, 1729) == 1729);
    assert(lcm(4096, 8192) == 8192);
    assert(lcm(1920, 1200) == 9600);
    assert(lcm(25, 49) == 1225);
#endif // _HAS_CXX17

    return true;
}

int main() {
    test_all_constants();
    test_all_bitmasks();
    test_all_limits();

    (void) piecewise_construct2;
    (void) allocator_arg2;

    (void) tx1;
    (void) tx2;
    (void) tx3;

    (void) p9;
    (void) p10;
    (void) p11;
    (void) p12;
    (void) p13;

    (void) t11;
    (void) t12;
    (void) t13;
    (void) t14;
    (void) t15;
    (void) t16;
    (void) t17;

    (void) dd1;
    (void) dd2;

    (void) istream_it;
    (void) istreambuf_it;

#ifndef _M_CEE
    (void) defer_lock2;
    (void) try_to_lock2;
    (void) adopt_lock2;
    (void) once;
#endif // _M_CEE

    // Compare against VC 2013's values.
    assert_bits(numeric_limits<float>::denorm_min(), 0x00000001UL);
    assert_bits(numeric_limits<double>::denorm_min(), 0x0000000000000001ULL);
    assert_bits(numeric_limits<long double>::denorm_min(), 0x0000000000000001ULL);
    assert_bits(numeric_limits<float>::infinity(), 0x7F800000UL);
    assert_bits(numeric_limits<double>::infinity(), 0x7FF0000000000000ULL);
    assert_bits(numeric_limits<long double>::infinity(), 0x7FF0000000000000ULL);
    assert_bits(numeric_limits<float>::quiet_NaN(), 0x7FC00000UL);
    assert_bits(numeric_limits<double>::quiet_NaN(), 0x7FF8000000000000ULL);
    assert_bits(numeric_limits<long double>::quiet_NaN(), 0x7FF8000000000000ULL);
    // signaling_NaN() should be 0x7F800001UL and 0x7FF0000000000001ULL,
    // but compiler limitations are interfering (VSO-128935).

    assert(bitset<0>(0x0ULL).to_string() == "");
    assert(bitset<0>(0xFEDCBA9876543210ULL).to_string() == "");
    assert(bitset<15>(0x6789ULL).to_string() == "110011110001001");
    assert(bitset<15>(0xFEDCBA9876543210ULL).to_string() == "011001000010000");
    assert(bitset<32>(0xABCD1234ULL).to_string() == "10101011110011010001001000110100");
    assert(bitset<32>(0xFEDCBA9876543210ULL).to_string() == "01110110010101000011001000010000");
    assert(bitset<45>(0x1701D1729FFFULL).to_string() == "101110000000111010001011100101001111111111111");
    assert(bitset<45>(0xFEDCBA9876543210ULL).to_string() == "110101001100001110110010101000011001000010000");
    assert(bitset<64>(0xFEDCBA9876543210ULL).to_string()
           == "1111111011011100101110101001100001110110010101000011001000010000");
    assert(bitset<75>(0xFEDCBA9876543210ULL).to_string()
           == "000000000001111111011011100101110101001100001110110010101000011001000010000");

    // P0295R0 gcd(), lcm()
    test_gcd_lcm();
    STATIC_ASSERT(test_gcd_lcm());
}
