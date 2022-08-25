// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <array>
#include <bitset>
#include <cassert>
#include <chrono>
#include <complex>
#include <cstddef>
#include <deque>
#include <forward_list>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <queue>
#include <regex>
#include <scoped_allocator>
#include <set>
#include <sstream>
#include <stack>
#include <streambuf>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <valarray>
#include <vector>

#ifndef _M_CEE_PURE
#include <atomic>
#endif // _M_CEE_PURE

#ifndef _M_CEE
#include <future>
#include <mutex>
#include <shared_mutex>
#endif // _M_CEE

#if _HAS_CXX23 && !defined(__clang__) // TRANSITION, DevCom-10107077, Clang has not implemented Deducing this
#define HAS_EXPLICIT_THIS_PARAMETER
#endif // _HAS_CXX23 && !defined(__clang__)

using namespace std;

template <typename T>
struct MyAlloc {
    using value_type = T;

    MyAlloc() = default;

    template <typename U>
    MyAlloc(const MyAlloc<U>&) {}

    T* allocate(size_t n) {
        return allocator<T>{}.allocate(n);
    }

    void deallocate(T* p, size_t n) {
        allocator<T>{}.deallocate(p, n);
    }

    template <typename U>
    bool operator==(const MyAlloc<U>&) const {
        return true;
    }
    template <typename U>
    bool operator!=(const MyAlloc<U>&) const {
        return false;
    }
};

struct MyGreater : greater<> {};

struct MyHash : hash<long> {};

struct MyWideHash : hash<wchar_t> {};

struct MyEqual : equal_to<> {};

struct MyDelete : default_delete<long[]> {};

void nothing() {}

int square(int x) {
    return x * x;
}

long add(short x, int y) {
    return x + y;
}

template <typename Void, template <typename...> class ClassTemplate, typename... CtorArgs>
struct CanDeduceFromHelper : false_type {};

template <template <typename...> class ClassTemplate, typename... CtorArgs>
struct CanDeduceFromHelper<void_t<decltype(ClassTemplate(declval<CtorArgs>()...))>, ClassTemplate, CtorArgs...>
    : true_type {};

template <template <typename...> class ClassTemplate, typename... CtorArgs>
constexpr bool CanDeduceFrom = CanDeduceFromHelper<void, ClassTemplate, CtorArgs...>::value;

void test_initializer_list() {
    initializer_list<long> il{};
    initializer_list il2(il);

    static_assert(is_same_v<decltype(il2), initializer_list<long>>);
}

void test_pair_and_tuple() {
    long x       = 11L;
    const long y = 22L;

    pair p1(x, y);
    pair p2(33L, 'c');
    pair p3(p2);

    static_assert(is_same_v<decltype(p1), pair<long, long>>);
    static_assert(is_same_v<decltype(p2), pair<long, char>>);
    static_assert(is_same_v<decltype(p3), pair<long, char>>);

    const int arr[] = {100, 200, 300};
    const long& z   = y;
    allocator<int> al{};

    tuple t1{};
    tuple t2(x);
    tuple t3(x, y);
    tuple t4('c', 44L, 7.89);
    tuple t5(square, arr, z);
    tuple t6(p2);
    tuple t7(allocator_arg, al);
    tuple t8(allocator_arg, al, 'c');
    tuple t9(allocator_arg, al, 'c', 'c');
    tuple t10(allocator_arg, al, p2);
    tuple t11(allocator_arg, al, t4);
    tuple t12(t11);

    static_assert(is_same_v<decltype(tuple()), tuple<>>);
    static_assert(is_same_v<decltype(tuple{}), tuple<>>);
    static_assert(is_same_v<decltype(t1), tuple<>>);
    static_assert(is_same_v<decltype(t2), tuple<long>>);
    static_assert(is_same_v<decltype(t3), tuple<long, long>>);
    static_assert(is_same_v<decltype(t4), tuple<char, long, double>>);
    static_assert(is_same_v<decltype(t5), tuple<int (*)(int), const int*, long>>);
    static_assert(is_same_v<decltype(t6), tuple<long, char>>);
    static_assert(is_same_v<decltype(t7), tuple<>>);
    static_assert(is_same_v<decltype(t8), tuple<char>>);
    static_assert(is_same_v<decltype(t9), tuple<char, char>>);
    static_assert(is_same_v<decltype(t10), tuple<long, char>>);
    static_assert(is_same_v<decltype(t11), tuple<char, long, double>>);
    static_assert(is_same_v<decltype(t12), tuple<char, long, double>>);
}

void test_optional() {
    optional opt1(1729L);
    optional opt2(opt1);

    static_assert(is_same_v<decltype(opt1), optional<long>>);
    static_assert(is_same_v<decltype(opt2), optional<long>>);
}

void test_bitset() {
    bitset<7> b{};
    bitset b2(b);

    static_assert(is_same_v<decltype(b2), bitset<7>>);
}

void test_allocator() {
    allocator<long> alloc1{};
    allocator alloc2(alloc1);

    static_assert(is_same_v<decltype(alloc2), allocator<long>>);
}

void test_unique_ptr() {
    static_assert(!CanDeduceFrom<unique_ptr>);
    static_assert(!CanDeduceFrom<unique_ptr, nullptr_t>);
    static_assert(!CanDeduceFrom<unique_ptr, long*>);
    static_assert(!CanDeduceFrom<unique_ptr, long*, const default_delete<long>&>);
    static_assert(!CanDeduceFrom<unique_ptr, long*, default_delete<long>>);
    static_assert(CanDeduceFrom<unique_ptr, unique_ptr<double>>);

    unique_ptr<double> up1{};
    unique_ptr up2(move(up1));

    static_assert(is_same_v<decltype(up2), unique_ptr<double>>);
}

void test_shared_ptr_and_weak_ptr() {
    shared_ptr<long[]> sp(new long[3]);
    weak_ptr<long[]> wp(sp);
    unique_ptr<long[], MyDelete> up{};

    shared_ptr sp1(sp);
    shared_ptr sp2(wp);
    shared_ptr sp3(move(up));
    weak_ptr wp1(sp);
    weak_ptr wp2(wp);

    static_assert(is_same_v<decltype(sp1), shared_ptr<long[]>>);
    static_assert(is_same_v<decltype(sp2), shared_ptr<long[]>>);
    static_assert(is_same_v<decltype(sp3), shared_ptr<long[]>>);
    static_assert(is_same_v<decltype(wp1), weak_ptr<long[]>>);
    static_assert(is_same_v<decltype(wp2), weak_ptr<long[]>>);
}

void test_owner_less() {
    owner_less<shared_ptr<long>> ol1{};
    owner_less<weak_ptr<long>> ol2{};
    owner_less<> ol3{};

    owner_less ol4(ol1);
    owner_less ol5(ol2);
    owner_less ol6(ol3);
    owner_less ol7{};

    static_assert(is_same_v<decltype(ol4), owner_less<shared_ptr<long>>>);
    static_assert(is_same_v<decltype(ol5), owner_less<weak_ptr<long>>>);
    static_assert(is_same_v<decltype(ol6), owner_less<>>);
    static_assert(is_same_v<decltype(ol7), owner_less<>>);
}

void test_scoped_allocator_adaptor() {
    MyAlloc<short> myal_short{};
    MyAlloc<int> myal_int{};
    MyAlloc<long> myal_long{};

    scoped_allocator_adaptor saa1(myal_short);
    scoped_allocator_adaptor saa2(myal_short, myal_int);
    scoped_allocator_adaptor saa3(myal_short, myal_int, myal_long);
    scoped_allocator_adaptor saa4(saa3);

    static_assert(is_same_v<decltype(saa1), scoped_allocator_adaptor<MyAlloc<short>>>);
    static_assert(is_same_v<decltype(saa2), scoped_allocator_adaptor<MyAlloc<short>, MyAlloc<int>>>);
    static_assert(is_same_v<decltype(saa3), scoped_allocator_adaptor<MyAlloc<short>, MyAlloc<int>, MyAlloc<long>>>);
    static_assert(is_same_v<decltype(saa4), scoped_allocator_adaptor<MyAlloc<short>, MyAlloc<int>, MyAlloc<long>>>);
}

void test_reference_wrapper() {
    long x = 11L;
    reference_wrapper rw1(x);
    reference_wrapper rw2(rw1);

    static_assert(is_same_v<decltype(rw1), reference_wrapper<long>>);
    static_assert(is_same_v<decltype(rw2), reference_wrapper<long>>);
}

void test_transparent_operator_functors() {
    plus op1{};
    minus op2{};
    multiplies op3{};
    divides op4{};
    modulus op5{};
    negate op6{};
    equal_to op7{};
    not_equal_to op8{};
    greater op9{};
    less op10{};
    greater_equal op11{};
    less_equal op12{};
    logical_and op13{};
    logical_or op14{};
    logical_not op15{};
    bit_and op16{};
    bit_or op17{};
    bit_xor op18{};
    bit_not op19{};

    static_assert(is_same_v<decltype(op1), plus<>>);
    static_assert(is_same_v<decltype(op2), minus<>>);
    static_assert(is_same_v<decltype(op3), multiplies<>>);
    static_assert(is_same_v<decltype(op4), divides<>>);
    static_assert(is_same_v<decltype(op5), modulus<>>);
    static_assert(is_same_v<decltype(op6), negate<>>);
    static_assert(is_same_v<decltype(op7), equal_to<>>);
    static_assert(is_same_v<decltype(op8), not_equal_to<>>);
    static_assert(is_same_v<decltype(op9), greater<>>);
    static_assert(is_same_v<decltype(op10), less<>>);
    static_assert(is_same_v<decltype(op11), greater_equal<>>);
    static_assert(is_same_v<decltype(op12), less_equal<>>);
    static_assert(is_same_v<decltype(op13), logical_and<>>);
    static_assert(is_same_v<decltype(op14), logical_or<>>);
    static_assert(is_same_v<decltype(op15), logical_not<>>);
    static_assert(is_same_v<decltype(op16), bit_and<>>);
    static_assert(is_same_v<decltype(op17), bit_or<>>);
    static_assert(is_same_v<decltype(op18), bit_xor<>>);
    static_assert(is_same_v<decltype(op19), bit_not<>>);

    static_assert(is_same_v<decltype(greater()), greater<>>);
    static_assert(is_same_v<decltype(greater{}), greater<>>);
}

template <template <typename> typename F>
void test_function_wrapper() {
    F<short(int, long)> f1{};

    if constexpr (is_copy_constructible_v<F<short(int, long)>>) {
        F f2copy(f1);
        static_assert(is_same_v<decltype(f2copy), F<short(int, long)>>);
    }

    F f2(move(f1));
    static_assert(is_same_v<decltype(f2), F<short(int, long)>>);

    F f3(nothing);
    F f4(&nothing);
    F f5(square);
    F f6(&square);
    F f7(add);
    F f8(&add);

    static_assert(is_same_v<decltype(f3), F<void()>>);
    static_assert(is_same_v<decltype(f4), F<void()>>);
    static_assert(is_same_v<decltype(f5), F<int(int)>>);
    static_assert(is_same_v<decltype(f6), F<int(int)>>);
    static_assert(is_same_v<decltype(f7), F<long(short, int)>>);
    static_assert(is_same_v<decltype(f8), F<long(short, int)>>);

    int n      = 0;
    auto accum = [&n](int x, int y) { return n += x + y; };

    F f9(plus<double>{});
    F f10(accum);

    static_assert(is_same_v<decltype(f9), F<double(const double&, const double&)>>);
    static_assert(is_same_v<decltype(f10), F<int(int, int)>>);

#ifdef HAS_EXPLICIT_THIS_PARAMETER
    struct ExplicitThisByVal {
        void operator()(this ExplicitThisByVal, char) {}
    };

    ExplicitThisByVal explicit_this_by_val_functor{};

    F f11(explicit_this_by_val_functor);
    F f12(as_const(explicit_this_by_val_functor));
    F f13(move(explicit_this_by_val_functor));
    F f14(move(as_const(explicit_this_by_val_functor)));

    static_assert(is_same_v<decltype(f11), F<void(char)>>);
    static_assert(is_same_v<decltype(f12), F<void(char)>>);
    static_assert(is_same_v<decltype(f13), F<void(char)>>);
    static_assert(is_same_v<decltype(f14), F<void(char)>>);

    struct ExplicitThisByRef {
        void operator()(this ExplicitThisByRef&, short) {}
    };

    ExplicitThisByRef explicit_this_by_ref_functor{};

    F f15(explicit_this_by_ref_functor);

    static_assert(is_same_v<decltype(f15), F<void(short)>>);

    struct ExplicitThisByCRef {
        void operator()(this const ExplicitThisByCRef&, int) {}
    };

    ExplicitThisByCRef explicit_this_by_cref_functor{};

    F f16(explicit_this_by_cref_functor);
    F f17(as_const(explicit_this_by_cref_functor));
    F f18(move(explicit_this_by_cref_functor));
    F f19(move(as_const(explicit_this_by_cref_functor)));

    static_assert(is_same_v<decltype(f16), F<void(int)>>);
    static_assert(is_same_v<decltype(f17), F<void(int)>>);
    static_assert(is_same_v<decltype(f18), F<void(int)>>);
    static_assert(is_same_v<decltype(f19), F<void(int)>>);

    struct ExplicitThisByConv {
        struct That {};

        operator That(this ExplicitThisByConv) {
            return {};
        }

        void operator()(this That, long long) {}
    };

    ExplicitThisByConv explicit_this_by_conv_functor{};

    F f20(explicit_this_by_conv_functor);
    F f21(as_const(explicit_this_by_conv_functor));
    F f22(move(explicit_this_by_conv_functor));
    F f23(move(as_const(explicit_this_by_conv_functor)));

    static_assert(is_same_v<decltype(f20), F<void(long long)>>);
    static_assert(is_same_v<decltype(f21), F<void(long long)>>);
    static_assert(is_same_v<decltype(f22), F<void(long long)>>);
    static_assert(is_same_v<decltype(f23), F<void(long long)>>);

    struct ExplicitThisNoexcept {
        float operator()(this ExplicitThisNoexcept, double) noexcept {
            return 3.14f;
        }
    };

    ExplicitThisNoexcept explicit_this_noexcept_functor{};

    F f24(explicit_this_noexcept_functor);
    F f25(as_const(explicit_this_noexcept_functor));
    F f26(move(explicit_this_noexcept_functor));
    F f27(move(as_const(explicit_this_noexcept_functor)));

    static_assert(is_same_v<decltype(f24), F<float(double)>>);
    static_assert(is_same_v<decltype(f25), F<float(double)>>);
    static_assert(is_same_v<decltype(f26), F<float(double)>>);
    static_assert(is_same_v<decltype(f27), F<float(double)>>);
#endif // HAS_EXPLICIT_THIS_PARAMETER
}

void test_searchers() {
    const wchar_t first[] = {L'x', L'y', L'z'};
    const auto last       = end(first);
    MyWideHash wh{};
    MyEqual eq{};

    default_searcher ds1(first, last);
    default_searcher ds2(first, last, eq);
    boyer_moore_searcher bms1(first, last);
    boyer_moore_searcher bms2(first, last, wh);
    boyer_moore_searcher bms3(first, last, wh, eq);
    boyer_moore_horspool_searcher bmhs1(first, last);
    boyer_moore_horspool_searcher bmhs2(first, last, wh);
    boyer_moore_horspool_searcher bmhs3(first, last, wh, eq);

    static_assert(is_same_v<decltype(ds1), default_searcher<const wchar_t*>>);
    static_assert(is_same_v<decltype(ds2), default_searcher<const wchar_t*, MyEqual>>);
    static_assert(is_same_v<decltype(bms1), boyer_moore_searcher<const wchar_t*>>);
    static_assert(is_same_v<decltype(bms2), boyer_moore_searcher<const wchar_t*, MyWideHash>>);
    static_assert(is_same_v<decltype(bms3), boyer_moore_searcher<const wchar_t*, MyWideHash, MyEqual>>);
    static_assert(is_same_v<decltype(bmhs1), boyer_moore_horspool_searcher<const wchar_t*>>);
    static_assert(is_same_v<decltype(bmhs2), boyer_moore_horspool_searcher<const wchar_t*, MyWideHash>>);
    static_assert(is_same_v<decltype(bmhs3), boyer_moore_horspool_searcher<const wchar_t*, MyWideHash, MyEqual>>);
}

void test_duration_and_time_point() {
    using namespace std::chrono;

    duration dur1(11ns);
    duration dur2(22h);

    static_assert(is_same_v<decltype(dur1), nanoseconds>);
    static_assert(is_same_v<decltype(dur2), hours>);

    time_point<system_clock, hours> tp{};
    time_point tp2(tp);
    (void) tp2;

    static_assert(is_same_v<decltype(tp2), time_point<system_clock, hours>>);
}

void test_basic_string() {
    const wchar_t first[] = {L'x', L'y', L'z'};
    const auto last       = end(first);
    MyAlloc<wchar_t> myal{};
    initializer_list<wchar_t> il = {L'x', L'y', L'z'};

    basic_string str1(first, last);
    basic_string str2(first, last, myal);
    basic_string str3(str2);
    basic_string str4(str2, 1);
    basic_string str5(str2, 1, myal);
    basic_string str6(str2, 1, 1);
    basic_string str7(str2, 1, 1, myal);
    basic_string str8(L"kitten"sv);
    basic_string str9(L"kitten"sv, myal);
    basic_string str9a(L"kitten"sv, 1, 2);
    basic_string str9b(L"kitten"sv, 1, 2, myal);
    basic_string str10(L"meow");
    basic_string str11(L"meow", myal);
    basic_string str12(L"meow", 1);
    basic_string str13(L"meow", 1, myal);
    basic_string str14(7, L'x');
    basic_string str15(7, L'x', myal);
    basic_string str16(il);
    basic_string str17(il, myal);
    basic_string str18(str2, myal);

    static_assert(is_same_v<decltype(str1), wstring>);
    static_assert(is_same_v<decltype(str2), basic_string<wchar_t, char_traits<wchar_t>, MyAlloc<wchar_t>>>);
    static_assert(is_same_v<decltype(str3), basic_string<wchar_t, char_traits<wchar_t>, MyAlloc<wchar_t>>>);
    static_assert(is_same_v<decltype(str4), basic_string<wchar_t, char_traits<wchar_t>, MyAlloc<wchar_t>>>);
    static_assert(is_same_v<decltype(str5), basic_string<wchar_t, char_traits<wchar_t>, MyAlloc<wchar_t>>>);
    static_assert(is_same_v<decltype(str6), basic_string<wchar_t, char_traits<wchar_t>, MyAlloc<wchar_t>>>);
    static_assert(is_same_v<decltype(str7), basic_string<wchar_t, char_traits<wchar_t>, MyAlloc<wchar_t>>>);
    static_assert(is_same_v<decltype(str8), wstring>);
    static_assert(is_same_v<decltype(str9), basic_string<wchar_t, char_traits<wchar_t>, MyAlloc<wchar_t>>>);
    static_assert(is_same_v<decltype(str9a), wstring>);
    static_assert(is_same_v<decltype(str9b), basic_string<wchar_t, char_traits<wchar_t>, MyAlloc<wchar_t>>>);
    static_assert(is_same_v<decltype(str10), wstring>);
    static_assert(is_same_v<decltype(str11), basic_string<wchar_t, char_traits<wchar_t>, MyAlloc<wchar_t>>>);
    static_assert(is_same_v<decltype(str12), wstring>);
    static_assert(is_same_v<decltype(str13), basic_string<wchar_t, char_traits<wchar_t>, MyAlloc<wchar_t>>>);
    static_assert(is_same_v<decltype(str14), wstring>);
    static_assert(is_same_v<decltype(str15), basic_string<wchar_t, char_traits<wchar_t>, MyAlloc<wchar_t>>>);
    static_assert(is_same_v<decltype(str16), wstring>);
    static_assert(is_same_v<decltype(str17), basic_string<wchar_t, char_traits<wchar_t>, MyAlloc<wchar_t>>>);
    static_assert(is_same_v<decltype(str18), basic_string<wchar_t, char_traits<wchar_t>, MyAlloc<wchar_t>>>);

#if _HAS_CXX23 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
    basic_string str19(from_range, first);
    basic_string str20(from_range, first, myal);

    static_assert(is_same_v<decltype(str19), wstring>);
    static_assert(is_same_v<decltype(str20), basic_string<wchar_t, char_traits<wchar_t>, MyAlloc<wchar_t>>>);
#endif // _HAS_CXX23 && defined(__cpp_lib_concepts)
}

void test_basic_string_view() {
    basic_string_view sv1(L"meow");
    basic_string_view sv2(L"meow", 1);
    basic_string_view sv3(sv2);

    static_assert(is_same_v<decltype(sv1), wstring_view>);
    static_assert(is_same_v<decltype(sv2), wstring_view>);
    static_assert(is_same_v<decltype(sv3), wstring_view>);
}

void test_array() {
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-braces"
#endif // __clang__

    long x       = 11L;
    const long y = 22L;

    array a1{x};
    array a2{x, y};
    array a3{x, y, 33L};
    array b{a3};

    static_assert(is_same_v<decltype(a1), array<long, 1>>);
    static_assert(is_same_v<decltype(a2), array<long, 2>>);
    static_assert(is_same_v<decltype(a3), array<long, 3>>);
    static_assert(is_same_v<decltype(b), array<long, 3>>);

#ifdef __clang__
#pragma clang diagnostic pop
#endif // __clang__
}

template <template <typename T, typename A = allocator<T>> typename Sequence>
void test_sequence_container() {
    const long first[] = {10L, 20L, 30L};
    const auto last    = end(first);
    MyAlloc<long> myal{};
    initializer_list<long> il = {11L, 22L, 33L};

    Sequence c1(first, last);
    Sequence c2(first, last, myal);
    Sequence c3(il);
    Sequence c4(il, myal);
    Sequence c5(7, 44L);
    Sequence c6(7, 44L, myal);
    Sequence c7(c6);

    static_assert(is_same_v<decltype(c1), Sequence<long>>);
    static_assert(is_same_v<decltype(c2), Sequence<long, MyAlloc<long>>>);
    static_assert(is_same_v<decltype(c3), Sequence<long>>);
    static_assert(is_same_v<decltype(c4), Sequence<long, MyAlloc<long>>>);
    static_assert(is_same_v<decltype(c5), Sequence<long>>);
    static_assert(is_same_v<decltype(c6), Sequence<long, MyAlloc<long>>>);
    static_assert(is_same_v<decltype(c7), Sequence<long, MyAlloc<long>>>);

#if _HAS_CXX23 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
    Sequence c8(from_range, first);
    Sequence c9(from_range, first, myal);

    static_assert(is_same_v<decltype(c8), Sequence<long>>);
    static_assert(is_same_v<decltype(c9), Sequence<long, MyAlloc<long>>>);
#endif // _HAS_CXX23 && defined(__cpp_lib_concepts)
}

void test_vector_bool() {
    const bool first[] = {true, false, true};
    const auto last    = end(first);
    MyAlloc<bool> myal{};
    initializer_list<bool> il = {true, false, true};

    vector vb1(first, last);
    vector vb2(first, last, myal);
    vector vb3(il);
    vector vb4(il, myal);
    vector vb5(7, true);
    vector vb6(7, true, myal);
    vector vb7(vb6);

    static_assert(is_same_v<decltype(vb1), vector<bool>>);
    static_assert(is_same_v<decltype(vb2), vector<bool, MyAlloc<bool>>>);
    static_assert(is_same_v<decltype(vb3), vector<bool>>);
    static_assert(is_same_v<decltype(vb4), vector<bool, MyAlloc<bool>>>);
    static_assert(is_same_v<decltype(vb5), vector<bool>>);
    static_assert(is_same_v<decltype(vb6), vector<bool, MyAlloc<bool>>>);
    static_assert(is_same_v<decltype(vb7), vector<bool, MyAlloc<bool>>>);

#if _HAS_CXX23 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
    vector vb8(from_range, first);
    vector vb9(from_range, first, myal);

    static_assert(is_same_v<decltype(vb8), vector<bool>>);
    static_assert(is_same_v<decltype(vb9), vector<bool, MyAlloc<bool>>>);
#endif // _HAS_CXX23 && defined(__cpp_lib_concepts)
}

template <template <typename K, typename V, typename C = less<K>, typename A = allocator<pair<const K, V>>> typename M>
void test_map_or_multimap() {
    using Purr          = pair<long, char>;
    using CPurr         = pair<const long, char>;
    const CPurr first[] = {{10L, 'a'}, {20L, 'b'}, {30L, 'c'}};
    const auto last     = end(first);
    MyGreater gt{};
    MyAlloc<CPurr> myal{};
    initializer_list<CPurr> il = {{11L, 'd'}, {22L, 'e'}, {33L, 'f'}};

    M m1(first, last);
    M m2(first, last, gt);
    M m3(first, last, gt, myal);
    M m4(il);
    M m5(il, gt);
    M m7(first, last, myal);
    M m9(m7);
    M m10{Purr{11L, 'd'}, Purr{22L, 'e'}, Purr{33L, 'f'}};
    M m11({Purr{11L, 'd'}, Purr{22L, 'e'}, Purr{33L, 'f'}}, gt);
    M m12({Purr{11L, 'd'}, Purr{22L, 'e'}, Purr{33L, 'f'}}, gt, myal);
    M m13({Purr{11L, 'd'}, Purr{22L, 'e'}, Purr{33L, 'f'}}, myal);

    static_assert(is_same_v<decltype(m1), M<long, char>>);
    static_assert(is_same_v<decltype(m2), M<long, char, MyGreater>>);
    static_assert(is_same_v<decltype(m3), M<long, char, MyGreater, MyAlloc<CPurr>>>);
    static_assert(is_same_v<decltype(m4), M<long, char>>);
    static_assert(is_same_v<decltype(m5), M<long, char, MyGreater>>);
    static_assert(is_same_v<decltype(m7), M<long, char, less<long>, MyAlloc<CPurr>>>);
    static_assert(is_same_v<decltype(m9), M<long, char, less<long>, MyAlloc<CPurr>>>);
    static_assert(is_same_v<decltype(m10), M<long, char>>);
    static_assert(is_same_v<decltype(m11), M<long, char, MyGreater>>);
    static_assert(is_same_v<decltype(m12), M<long, char, MyGreater, MyAlloc<CPurr>>>);
    static_assert(is_same_v<decltype(m13), M<long, char, less<long>, MyAlloc<CPurr>>>);

#if _HAS_CXX23 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
    M m14(from_range, first);
    M m15(from_range, first, gt);
    M m16(from_range, first, gt, myal);
    M m17(from_range, first, myal);

    static_assert(is_same_v<decltype(m14), M<long, char>>);
    static_assert(is_same_v<decltype(m15), M<long, char, MyGreater>>);
    static_assert(is_same_v<decltype(m16), M<long, char, MyGreater, MyAlloc<CPurr>>>);
    static_assert(is_same_v<decltype(m17), M<long, char, less<long>, MyAlloc<CPurr>>>);
#endif // _HAS_CXX23 && defined(__cpp_lib_concepts)
}

template <template <typename K, typename C = less<K>, typename A = allocator<K>> typename S>
void test_set_or_multiset() {
    const long first[] = {10L, 20L, 30L};
    const auto last    = end(first);
    MyGreater gt{};
    MyAlloc<long> myal{};
    initializer_list<long> il = {11L, 22L, 33L};

    S s1(first, last);
    S s2(first, last, gt);
    S s3(first, last, gt, myal);
    S s4(il);
    S s5(il, gt);
    S s6(il, gt, myal);
    S s7(first, last, myal);
    S s8(il, myal);
    S s9(s8);

    static_assert(is_same_v<decltype(s1), S<long>>);
    static_assert(is_same_v<decltype(s2), S<long, MyGreater>>);
    static_assert(is_same_v<decltype(s3), S<long, MyGreater, MyAlloc<long>>>);
    static_assert(is_same_v<decltype(s4), S<long>>);
    static_assert(is_same_v<decltype(s5), S<long, MyGreater>>);
    static_assert(is_same_v<decltype(s6), S<long, MyGreater, MyAlloc<long>>>);
    static_assert(is_same_v<decltype(s7), S<long, less<long>, MyAlloc<long>>>);
    static_assert(is_same_v<decltype(s8), S<long, less<long>, MyAlloc<long>>>);
    static_assert(is_same_v<decltype(s9), S<long, less<long>, MyAlloc<long>>>);

#if _HAS_CXX23 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
    S s10(from_range, first);
    S s11(from_range, first, gt);
    S s12(from_range, first, gt, myal);
    S s13(from_range, first, myal);

    static_assert(is_same_v<decltype(s10), S<long>>);
    static_assert(is_same_v<decltype(s11), S<long, MyGreater>>);
    static_assert(is_same_v<decltype(s12), S<long, MyGreater, MyAlloc<long>>>);
    static_assert(is_same_v<decltype(s13), S<long, less<long>, MyAlloc<long>>>);
#endif // _HAS_CXX23 && defined(__cpp_lib_concepts)
}

template <template <typename K, typename V, typename H = hash<K>, typename P = equal_to<K>,
    typename A = allocator<pair<const K, V>>>
    typename UM>
void test_unordered_map_or_unordered_multimap() {
    using Purr          = pair<long, char>;
    using CPurr         = pair<const long, char>;
    const CPurr first[] = {{10L, 'a'}, {20L, 'b'}, {30L, 'c'}};
    const auto last     = end(first);
    MyHash hf{};
    MyEqual eq{};
    MyAlloc<CPurr> myal{};
    initializer_list<CPurr> il = {{11L, 'd'}, {22L, 'e'}, {33L, 'f'}};

    UM um1(first, last);
    UM um2(first, last, 7);
    UM um3(first, last, 7, hf);
    UM um4(first, last, 7, hf, eq);
    UM um5(first, last, 7, hf, eq, myal);
    UM um6(il);
    UM um11(first, last, myal);
    UM um12(first, last, 7, myal);
    UM um13(first, last, 7, hf, myal);
    UM um17(um5);
    UM um18{Purr{10L, 'a'}, Purr{20L, 'b'}, Purr{30L, 'c'}};
    UM um19({Purr{10L, 'a'}, Purr{20L, 'b'}, Purr{30L, 'c'}}, 7);
    UM um20({Purr{10L, 'a'}, Purr{20L, 'b'}, Purr{30L, 'c'}}, 7, hf);
    UM um21({Purr{10L, 'a'}, Purr{20L, 'b'}, Purr{30L, 'c'}}, 7, hf, eq);
    UM um22({Purr{10L, 'a'}, Purr{20L, 'b'}, Purr{30L, 'c'}}, 7, hf, eq, myal);
    UM um23({Purr{10L, 'a'}, Purr{20L, 'b'}, Purr{30L, 'c'}}, myal);
    UM um24({Purr{10L, 'a'}, Purr{20L, 'b'}, Purr{30L, 'c'}}, 7, myal);
    UM um25({Purr{10L, 'a'}, Purr{20L, 'b'}, Purr{30L, 'c'}}, 7, hf, myal);

    static_assert(is_same_v<decltype(um1), UM<long, char>>);
    static_assert(is_same_v<decltype(um2), UM<long, char>>);
    static_assert(is_same_v<decltype(um3), UM<long, char, MyHash>>);
    static_assert(is_same_v<decltype(um4), UM<long, char, MyHash, MyEqual>>);
    static_assert(is_same_v<decltype(um5), UM<long, char, MyHash, MyEqual, MyAlloc<CPurr>>>);
    static_assert(is_same_v<decltype(um6), UM<long, char>>);
    static_assert(is_same_v<decltype(um11), UM<long, char, hash<long>, equal_to<long>, MyAlloc<CPurr>>>);
    static_assert(is_same_v<decltype(um12), UM<long, char, hash<long>, equal_to<long>, MyAlloc<CPurr>>>);
    static_assert(is_same_v<decltype(um13), UM<long, char, MyHash, equal_to<long>, MyAlloc<CPurr>>>);
    static_assert(is_same_v<decltype(um17), UM<long, char, MyHash, MyEqual, MyAlloc<CPurr>>>);
    static_assert(is_same_v<decltype(um18), UM<long, char>>);
    static_assert(is_same_v<decltype(um19), UM<long, char>>);
    static_assert(is_same_v<decltype(um20), UM<long, char, MyHash>>);
    static_assert(is_same_v<decltype(um21), UM<long, char, MyHash, MyEqual>>);
    static_assert(is_same_v<decltype(um22), UM<long, char, MyHash, MyEqual, MyAlloc<CPurr>>>);
    static_assert(is_same_v<decltype(um23), UM<long, char, hash<long>, equal_to<long>, MyAlloc<CPurr>>>);
    static_assert(is_same_v<decltype(um24), UM<long, char, hash<long>, equal_to<long>, MyAlloc<CPurr>>>);
    static_assert(is_same_v<decltype(um25), UM<long, char, MyHash, equal_to<long>, MyAlloc<CPurr>>>);

#if _HAS_CXX23 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
    UM um26(from_range, first);
    UM um27(from_range, first, 7);
    UM um28(from_range, first, 7, hf);
    UM um29(from_range, first, 7, hf, eq);
    UM um30(from_range, first, 7, hf, eq, myal);
    UM um31(from_range, first, myal);
    UM um32(from_range, first, 7, myal);
    UM um33(from_range, first, 7, hf, myal);

    static_assert(is_same_v<decltype(um26), UM<long, char>>);
    static_assert(is_same_v<decltype(um27), UM<long, char>>);
    static_assert(is_same_v<decltype(um28), UM<long, char, MyHash>>);
    static_assert(is_same_v<decltype(um29), UM<long, char, MyHash, MyEqual>>);
    static_assert(is_same_v<decltype(um30), UM<long, char, MyHash, MyEqual, MyAlloc<CPurr>>>);
    static_assert(is_same_v<decltype(um31), UM<long, char, hash<long>, equal_to<long>, MyAlloc<CPurr>>>);
    static_assert(is_same_v<decltype(um32), UM<long, char, hash<long>, equal_to<long>, MyAlloc<CPurr>>>);
    static_assert(is_same_v<decltype(um33), UM<long, char, MyHash, equal_to<long>, MyAlloc<CPurr>>>);
#endif // _HAS_CXX23 && defined(__cpp_lib_concepts)
}

template <template <typename K, typename H = hash<K>, typename P = equal_to<K>, typename A = allocator<K>> typename US>
void test_unordered_set_or_unordered_multiset() {
    const long first[] = {10L, 20L, 30L};
    const auto last    = end(first);
    MyHash hf{};
    MyEqual eq{};
    MyAlloc<long> myal{};
    initializer_list<long> il = {11L, 22L, 33L};

    US us1(first, last);
    US us2(first, last, 7);
    US us3(first, last, 7, hf);
    US us4(first, last, 7, hf, eq);
    US us5(first, last, 7, hf, eq, myal);
    US us6(il);
    US us7(il, 7);
    US us8(il, 7, hf);
    US us9(il, 7, hf, eq);
    US us10(il, 7, hf, eq, myal);
    US us11(first, last, 7, myal);
    US us12(first, last, 7, hf, myal);
    US us13(il, 7, myal);
    US us14(il, 7, hf, myal);
    US us15(us5);

    static_assert(is_same_v<decltype(us1), US<long>>);
    static_assert(is_same_v<decltype(us2), US<long>>);
    static_assert(is_same_v<decltype(us3), US<long, MyHash>>);
    static_assert(is_same_v<decltype(us4), US<long, MyHash, MyEqual>>);
    static_assert(is_same_v<decltype(us5), US<long, MyHash, MyEqual, MyAlloc<long>>>);
    static_assert(is_same_v<decltype(us6), US<long>>);
    static_assert(is_same_v<decltype(us7), US<long>>);
    static_assert(is_same_v<decltype(us8), US<long, MyHash>>);
    static_assert(is_same_v<decltype(us9), US<long, MyHash, MyEqual>>);
    static_assert(is_same_v<decltype(us10), US<long, MyHash, MyEqual, MyAlloc<long>>>);
    static_assert(is_same_v<decltype(us11), US<long, hash<long>, equal_to<long>, MyAlloc<long>>>);
    static_assert(is_same_v<decltype(us12), US<long, MyHash, equal_to<long>, MyAlloc<long>>>);
    static_assert(is_same_v<decltype(us13), US<long, hash<long>, equal_to<long>, MyAlloc<long>>>);
    static_assert(is_same_v<decltype(us14), US<long, MyHash, equal_to<long>, MyAlloc<long>>>);
    static_assert(is_same_v<decltype(us15), US<long, MyHash, MyEqual, MyAlloc<long>>>);

#if _HAS_CXX23 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
    US us16(from_range, first);
    US us17(from_range, first, 7);
    US us18(from_range, first, 7, hf);
    US us19(from_range, first, 7, hf, eq);
    US us20(from_range, first, 7, hf, eq, myal);
    US us21(from_range, first, 7, myal);
    US us22(from_range, first, 7, hf, myal);

    static_assert(is_same_v<decltype(us16), US<long>>);
    static_assert(is_same_v<decltype(us17), US<long>>);
    static_assert(is_same_v<decltype(us18), US<long, MyHash>>);
    static_assert(is_same_v<decltype(us19), US<long, MyHash, MyEqual>>);
    static_assert(is_same_v<decltype(us20), US<long, MyHash, MyEqual, MyAlloc<long>>>);
    static_assert(is_same_v<decltype(us21), US<long, hash<long>, equal_to<long>, MyAlloc<long>>>);
    static_assert(is_same_v<decltype(us22), US<long, MyHash, equal_to<long>, MyAlloc<long>>>);
#endif // _HAS_CXX23 && defined(__cpp_lib_concepts)
}

void test_queue_and_stack() {
    list<long, MyAlloc<long>> lst{};
    MyAlloc<long> myal{};

    queue q1(lst);
    queue q2(lst, myal);
    queue q3(q2);

    static_assert(is_same_v<decltype(q1), queue<long, list<long, MyAlloc<long>>>>);
    static_assert(is_same_v<decltype(q2), queue<long, list<long, MyAlloc<long>>>>);
    static_assert(is_same_v<decltype(q3), queue<long, list<long, MyAlloc<long>>>>);

#if _HAS_CXX23
    const long first[] = {10L, 20L, 30L};
    const auto last    = end(first);

    queue q4(first, last);
    queue q5(first, last, myal);

    static_assert(is_same_v<decltype(q4), queue<long>>);
    static_assert(is_same_v<decltype(q5), queue<long, deque<long, MyAlloc<long>>>>);

#ifdef __cpp_lib_concepts
    queue q6(from_range, first);
    queue q7(from_range, first, myal);

    static_assert(is_same_v<decltype(q6), queue<long>>);
    static_assert(is_same_v<decltype(q7), queue<long, deque<long, MyAlloc<long>>>>);
#endif // __cpp_lib_concepts
#endif // _HAS_CXX23

    stack s1(lst);
    stack s2(lst, myal);
    stack s3(s2);

    static_assert(is_same_v<decltype(s1), stack<long, list<long, MyAlloc<long>>>>);
    static_assert(is_same_v<decltype(s2), stack<long, list<long, MyAlloc<long>>>>);
    static_assert(is_same_v<decltype(s3), stack<long, list<long, MyAlloc<long>>>>);

#if _HAS_CXX23
    stack s4(first, last);
    stack s5(first, last, myal);

    static_assert(is_same_v<decltype(s4), stack<long>>);
    static_assert(is_same_v<decltype(s5), stack<long, deque<long, MyAlloc<long>>>>);

#ifdef __cpp_lib_concepts
    stack s6(from_range, first);
    stack s7(from_range, first, myal);

    static_assert(is_same_v<decltype(s6), stack<long>>);
    static_assert(is_same_v<decltype(s7), stack<long, deque<long, MyAlloc<long>>>>);
#endif // __cpp_lib_concepts
#endif // _HAS_CXX23
}

void test_priority_queue() {
    MyGreater gt{};
    deque<long, MyAlloc<long>> deq{};
    MyAlloc<long> myal{};
    const long first[] = {10L, 20L, 30L};
    const auto last    = end(first);

    priority_queue pq1(gt, deq);
    priority_queue pq2(gt, deq, myal);
    priority_queue pq3(first, last);
    priority_queue pq4(first, last, gt);
    priority_queue pq5(first, last, gt, deq);
    priority_queue pq6(first, last, myal);
    priority_queue pq7(first, last, gt, myal);
    priority_queue pq8(first, last, gt, deq, myal);
    priority_queue pq9(pq5);

    static_assert(is_same_v<decltype(pq1), priority_queue<long, deque<long, MyAlloc<long>>, MyGreater>>);
    static_assert(is_same_v<decltype(pq2), priority_queue<long, deque<long, MyAlloc<long>>, MyGreater>>);
    static_assert(is_same_v<decltype(pq3), priority_queue<long>>);
    static_assert(is_same_v<decltype(pq4), priority_queue<long, vector<long>, MyGreater>>);
    static_assert(is_same_v<decltype(pq5), priority_queue<long, deque<long, MyAlloc<long>>, MyGreater>>);
    static_assert(is_same_v<decltype(pq6), priority_queue<long, vector<long, MyAlloc<long>>>>);
    static_assert(is_same_v<decltype(pq7), priority_queue<long, vector<long, MyAlloc<long>>, MyGreater>>);
    static_assert(is_same_v<decltype(pq8), priority_queue<long, deque<long, MyAlloc<long>>, MyGreater>>);
    static_assert(is_same_v<decltype(pq9), priority_queue<long, deque<long, MyAlloc<long>>, MyGreater>>);

#if _HAS_CXX23 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
    priority_queue pq10(from_range, first);
    priority_queue pq11(from_range, first, gt);
    priority_queue pq12(from_range, first, gt, myal);
    priority_queue pq13(from_range, first, myal);

    static_assert(is_same_v<decltype(pq10), priority_queue<long>>);
    static_assert(is_same_v<decltype(pq11), priority_queue<long, vector<long>, MyGreater>>);
    static_assert(is_same_v<decltype(pq12), priority_queue<long, vector<long, MyAlloc<long>>, MyGreater>>);
    static_assert(is_same_v<decltype(pq13), priority_queue<long, vector<long, MyAlloc<long>>>>);
#endif // _HAS_CXX23 && defined(__cpp_lib_concepts)
}

void test_iterator_adaptors() {
    long* const ptr = nullptr;

    reverse_iterator ri1(ptr);
    reverse_iterator ri2(ri1);

    static_assert(is_same_v<decltype(ri1), reverse_iterator<long*>>);
    static_assert(is_same_v<decltype(ri2), reverse_iterator<long*>>);

    move_iterator mi1(ptr);
    move_iterator mi2(mi1);

    static_assert(is_same_v<decltype(mi1), move_iterator<long*>>);
    static_assert(is_same_v<decltype(mi2), move_iterator<long*>>);
}

void test_insert_iterators() {
    list<long, MyAlloc<long>> lst{};

    back_insert_iterator bii1(lst);
    back_insert_iterator bii2(bii1);

    static_assert(is_same_v<decltype(bii1), back_insert_iterator<list<long, MyAlloc<long>>>>);
    static_assert(is_same_v<decltype(bii2), back_insert_iterator<list<long, MyAlloc<long>>>>);

    front_insert_iterator fii1(lst);
    front_insert_iterator fii2(fii1);

    static_assert(is_same_v<decltype(fii1), front_insert_iterator<list<long, MyAlloc<long>>>>);
    static_assert(is_same_v<decltype(fii2), front_insert_iterator<list<long, MyAlloc<long>>>>);

    insert_iterator ii1(lst, lst.end());
    insert_iterator ii2(ii1);

    static_assert(is_same_v<decltype(ii1), insert_iterator<list<long, MyAlloc<long>>>>);
    static_assert(is_same_v<decltype(ii2), insert_iterator<list<long, MyAlloc<long>>>>);
}

void test_stream_iterators() {
    wostringstream woss{};

    istream_iterator<double, wchar_t> istr_iter{};
    istream_iterator istr_iter2(istr_iter);

    static_assert(is_same_v<decltype(istr_iter2), istream_iterator<double, wchar_t>>);

    ostream_iterator<double, wchar_t> ostr_iter(woss);
    ostream_iterator ostr_iter2(ostr_iter);

    static_assert(is_same_v<decltype(ostr_iter2), ostream_iterator<double, wchar_t>>);

    istreambuf_iterator<wchar_t> istrbuf_iter{};
    istreambuf_iterator istrbuf_iter2(istrbuf_iter);

    static_assert(is_same_v<decltype(istrbuf_iter2), istreambuf_iterator<wchar_t>>);

    ostreambuf_iterator<wchar_t> ostrbuf_iter(woss);
    ostreambuf_iterator ostrbuf_iter2(ostrbuf_iter);

    static_assert(is_same_v<decltype(ostrbuf_iter2), ostreambuf_iterator<wchar_t>>);
}

void test_complex() {
    complex c1(1.0f);
    complex c2(1.0f, 1.0f);
    complex c3(c2);
    complex c4(1.0);
    complex c5(1.0, 1.0);
    complex c6(c5);

    static_assert(is_same_v<decltype(c1), complex<float>>);
    static_assert(is_same_v<decltype(c2), complex<float>>);
    static_assert(is_same_v<decltype(c3), complex<float>>);
    static_assert(is_same_v<decltype(c4), complex<double>>);
    static_assert(is_same_v<decltype(c5), complex<double>>);
    static_assert(is_same_v<decltype(c6), complex<double>>);
}

void test_valarray() {
    int arr1[]                = {10, 20, 30};
    const long arr2[]         = {11L, 22L, 33L};
    initializer_list<long> il = {11L, 22L, 33L};

    valarray va1(arr1, 3);
    valarray va2(arr2, 3);
    valarray va3(44L, 3);
    valarray va4(begin(arr2), 3);
    valarray va5(va4);
    valarray va6(il);

    static_assert(is_same_v<decltype(va1), valarray<int>>);
    static_assert(is_same_v<decltype(va2), valarray<long>>);
    static_assert(is_same_v<decltype(va3), valarray<long>>);
    static_assert(is_same_v<decltype(va4), valarray<long>>);
    static_assert(is_same_v<decltype(va5), valarray<long>>);
    static_assert(is_same_v<decltype(va6), valarray<long>>);
}

void test_regex() {
    const wchar_t first[]        = {L'x', L'y', L'z'};
    const auto last              = end(first);
    initializer_list<wchar_t> il = {L'x', L'y', L'z'};

    basic_regex r1(first, last);
    basic_regex r2(first, last, regex_constants::icase);
    basic_regex r3(L"meow");
    basic_regex r4(L"meow", regex_constants::icase);
    basic_regex r5(L"meow", 1);
    basic_regex r6(L"meow", 1, regex_constants::icase);
    basic_regex r7(r6);
    basic_regex r8(L"kitty"s);
    basic_regex r9(L"kitty"s, regex_constants::icase);
    basic_regex r10(il);
    basic_regex r11(il, regex_constants::icase);

    static_assert(is_same_v<decltype(r1), wregex>);
    static_assert(is_same_v<decltype(r2), wregex>);
    static_assert(is_same_v<decltype(r3), wregex>);
    static_assert(is_same_v<decltype(r4), wregex>);
    static_assert(is_same_v<decltype(r5), wregex>);
    static_assert(is_same_v<decltype(r6), wregex>);
    static_assert(is_same_v<decltype(r7), wregex>);
    static_assert(is_same_v<decltype(r8), wregex>);
    static_assert(is_same_v<decltype(r9), wregex>);
    static_assert(is_same_v<decltype(r10), wregex>);
    static_assert(is_same_v<decltype(r11), wregex>);

    wssub_match sm1{};
    sub_match sm2(sm1);

    static_assert(is_same_v<decltype(sm2), wssub_match>);

    wsmatch mr1{};
    match_results mr2(mr1);

    static_assert(is_same_v<decltype(mr2), wsmatch>);

    regex_iterator ri1(first, last, r1);
    regex_iterator ri2(first, last, r1, regex_constants::match_default);
    regex_iterator ri3(ri2);

    static_assert(is_same_v<decltype(ri1), wcregex_iterator>);
    static_assert(is_same_v<decltype(ri2), wcregex_iterator>);
    static_assert(is_same_v<decltype(ri3), wcregex_iterator>);

    const vector<int> vec_submatches({0});
    initializer_list<int> il_submatches = {0};
    const int arr_submatches[]          = {0};

    regex_token_iterator rti1(first, last, r1);
    regex_token_iterator rti2(first, last, r1, 0);
    regex_token_iterator rti3(first, last, r1, 0, regex_constants::match_default);
    regex_token_iterator rti4(first, last, r1, vec_submatches);
    regex_token_iterator rti5(first, last, r1, vec_submatches, regex_constants::match_default);
    regex_token_iterator rti6(first, last, r1, il_submatches);
    regex_token_iterator rti7(first, last, r1, il_submatches, regex_constants::match_default);
    regex_token_iterator rti8(first, last, r1, arr_submatches);
    regex_token_iterator rti9(first, last, r1, arr_submatches, regex_constants::match_default);
    regex_token_iterator rti10(rti9);

    static_assert(is_same_v<decltype(rti1), wcregex_token_iterator>);
    static_assert(is_same_v<decltype(rti2), wcregex_token_iterator>);
    static_assert(is_same_v<decltype(rti3), wcregex_token_iterator>);
    static_assert(is_same_v<decltype(rti4), wcregex_token_iterator>);
    static_assert(is_same_v<decltype(rti5), wcregex_token_iterator>);
    static_assert(is_same_v<decltype(rti6), wcregex_token_iterator>);
    static_assert(is_same_v<decltype(rti7), wcregex_token_iterator>);
    static_assert(is_same_v<decltype(rti8), wcregex_token_iterator>);
    static_assert(is_same_v<decltype(rti9), wcregex_token_iterator>);
    static_assert(is_same_v<decltype(rti10), wcregex_token_iterator>);
}

void test_atomic() {
#ifndef _M_CEE_PURE
    long x = 11L;

    atomic atom1(x);
    atomic atom2(&x);

    static_assert(is_same_v<decltype(atom1), atomic<long>>);
    static_assert(is_same_v<decltype(atom2), atomic<long*>>);
#endif // _M_CEE_PURE
}

void test_locks() {
#ifndef _M_CEE
    recursive_mutex rm{};
    recursive_timed_mutex rtm{};
    lock_guard lg(rm);
    unique_lock ul(rm);
    unique_lock ul2(move(ul));
    scoped_lock scoped0{};
    scoped_lock scoped1(rm);
    scoped_lock scoped2(rm, rtm);
    scoped_lock scoped3(adopt_lock);
    rm.lock();
    scoped_lock scoped4(adopt_lock, rm);
    rm.lock();
    rtm.lock();
    scoped_lock scoped5(adopt_lock, rm, rtm);
    shared_timed_mutex stm{};
    shared_lock shared(stm);
    shared_lock shared2(move(shared));

    static_assert(is_same_v<decltype(lg), lock_guard<recursive_mutex>>);
    static_assert(is_same_v<decltype(ul), unique_lock<recursive_mutex>>);
    static_assert(is_same_v<decltype(ul2), unique_lock<recursive_mutex>>);
    static_assert(is_same_v<decltype(scoped0), scoped_lock<>>);
    static_assert(is_same_v<decltype(scoped1), scoped_lock<recursive_mutex>>);
    static_assert(is_same_v<decltype(scoped2), scoped_lock<recursive_mutex, recursive_timed_mutex>>);
    static_assert(is_same_v<decltype(scoped3), scoped_lock<>>);
    static_assert(is_same_v<decltype(scoped4), scoped_lock<recursive_mutex>>);
    static_assert(is_same_v<decltype(scoped5), scoped_lock<recursive_mutex, recursive_timed_mutex>>);
    static_assert(is_same_v<decltype(shared), shared_lock<shared_timed_mutex>>);
    static_assert(is_same_v<decltype(shared2), shared_lock<shared_timed_mutex>>);
#endif // _M_CEE
}

int main() {
    test_initializer_list();
    test_pair_and_tuple();
    test_optional();
    test_bitset();
    test_allocator();
    test_unique_ptr();
    test_shared_ptr_and_weak_ptr();
    test_owner_less();
    test_scoped_allocator_adaptor();
    test_reference_wrapper();
    test_transparent_operator_functors();

    test_function_wrapper<function>();
#ifndef _M_CEE
    test_function_wrapper<packaged_task>();
#endif // _M_CEE

    test_searchers();
    test_duration_and_time_point();
    test_basic_string();
    test_basic_string_view();
    test_array();

    test_sequence_container<deque>();
    test_sequence_container<forward_list>();
    test_sequence_container<list>();
    test_sequence_container<vector>();

    test_vector_bool();

    test_map_or_multimap<map>();
    test_map_or_multimap<multimap>();

    test_set_or_multiset<set>();
    test_set_or_multiset<multiset>();

    test_unordered_map_or_unordered_multimap<unordered_map>();
    test_unordered_map_or_unordered_multimap<unordered_multimap>();

    test_unordered_set_or_unordered_multiset<unordered_set>();
    test_unordered_set_or_unordered_multiset<unordered_multiset>();

    test_queue_and_stack();
    test_priority_queue();
    test_iterator_adaptors();
    test_insert_iterators();
    test_stream_iterators();
    test_complex();
    test_valarray();
    test_regex();
    test_atomic();
    test_locks();
}
