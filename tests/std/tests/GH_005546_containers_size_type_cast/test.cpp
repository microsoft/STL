// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstddef>
#include <memory>
#include <new>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>

#if _HAS_CXX17
#include <string_view>
#endif // _HAS_CXX17

#if _HAS_CXX23
#include <ranges>
#endif // _HAS_CXX23

#if _HAS_CXX20
#define CONSTEXPR20 constexpr
#else // ^^^ _HAS_CXX20 / !_HAS_CXX20 vvv
#define CONSTEXPR20 inline
#endif // ^^^ !_HAS_CXX20 ^^^

using namespace std;

template <class UInt, enable_if_t<(sizeof(UInt) > sizeof(size_t)), int> = 0>
CONSTEXPR20 void check_alloc_in_range_of_size_t(const UInt n) {
    static_assert(is_integral_v<UInt> && is_unsigned_v<UInt>, "must use unsigned integer type");
    if (n > static_cast<size_t>(-1)) {
        throw bad_alloc{};
    }
}

template <class UInt, enable_if_t<(sizeof(UInt) <= sizeof(size_t)), int> = 0>
CONSTEXPR20 void check_alloc_in_range_of_size_t(UInt) noexcept {
    static_assert(is_integral_v<UInt> && is_unsigned_v<UInt>, "must use unsigned integer type");
}


template <class T, class Diff>
struct redifference_allocator {
    static_assert(is_integral_v<Diff> && is_signed_v<Diff> && !is_same_v<Diff, char>,
        "must use a signed integer type as the difference type");

    using value_type      = T;
    using difference_type = Diff;
    using size_type       = make_unsigned_t<Diff>;

    redifference_allocator() = default;
    template <class U>
    constexpr redifference_allocator(const redifference_allocator<U, Diff>&) noexcept {}

    CONSTEXPR20 T* allocate(const size_type n) {
        check_alloc_in_range_of_size_t(n);
        return allocator<T>{}.allocate(static_cast<size_t>(n));
    }

    CONSTEXPR20 void deallocate(T* p, const size_type n) {
        allocator<T>{}.deallocate(p, static_cast<size_t>(n));
    }

#if _HAS_CXX23
    constexpr allocation_result<T*, size_type> allocate_at_least(const size_type n) {
        check_alloc_in_range_of_size_t(n);
        const auto [ptr, cnt] = allocator<T>{}.allocate_at_least(static_cast<size_t>(n));
        return {ptr, static_cast<size_type>(cnt)};
    }
#endif // _HAS_CXX23

    template <class U>
    friend constexpr bool operator==(const redifference_allocator&, const redifference_allocator<U, Diff>&) noexcept {
        return true;
    }

#if !_HAS_CXX20
    template <class U>
    friend constexpr bool operator!=(const redifference_allocator&, const redifference_allocator<U, Diff>&) noexcept {
        return false;
    }
#endif // !_HAS_CXX20
};

template <class Diff>
void test_basic_string_getline() {
    using tested_allocator = redifference_allocator<char, Diff>;
    using tested_string    = basic_string<char, char_traits<char>, tested_allocator>;

    tested_string s1 = "abc";

    {
        istringstream is{"abc\ndef"};
        getline(is, s1);
        assert(s1 == "abc");
    }
    {
        getline(istringstream{"abc\ndef"}, s1);
        assert(s1 == "abc");
    }
    {
        istringstream is{"abc&def"};
        getline(is, s1, '&');
        assert(s1 == "abc");
    }
    {
        getline(istringstream{"abc&def"}, s1, '&');
        assert(s1 == "abc");
    }
}

template <class Diff>
CONSTEXPR20 void test_basic_string() {
    using UDiff            = make_unsigned_t<Diff>;
    using tested_allocator = redifference_allocator<char, Diff>;
    using tested_string    = basic_string<char, char_traits<char>, tested_allocator>;

    tested_string s1;
    assert(s1.empty());
    tested_string s2(tested_allocator{});
    assert(s2.empty());
    tested_string s3 = s1;
    assert(s3.empty());
    tested_string s4 = move(s1);
    assert(s4.empty());
    tested_string s5 = {s2, 0, tested_allocator{}};
    assert(s5.empty());
    tested_string s6 = {s2, 0, 0, tested_allocator{}};
    assert(s6.empty());
    tested_string s7 = {move(s2), 0, tested_allocator{}};
    assert(s7.empty());
    tested_string s8 = {move(s3), 0, 0, tested_allocator{}};
    assert(s8.empty());
    tested_string s9 = {"ab", 0};
    assert(s9.empty());
    tested_string s10 = "cd";
    assert(s10 == "cd");
    tested_string s11(make_unsigned_t<Diff>{1}, '*');
    assert(s11 == "*");
    tested_string s12 = {s11.begin(), s11.end(), tested_allocator{}};
    assert(s12 == s11);
    tested_string s13 = {'e', 'f'};
    assert(s13 == "ef");
    tested_string s14 = {s4, tested_allocator{}};
    assert(s14.empty());
    tested_string s15 = {move(s4), tested_allocator{}};
    assert(s15.empty());
#if _HAS_CXX17
    tested_string s16 = {string_view{}, 0, 0, tested_allocator{}};
    assert(s16.empty());
    tested_string s17(string_view{}, tested_allocator{});
    assert(s17.empty());
#endif // _HAS_CXX17
#if _HAS_CXX23
    tested_string s18 = {from_range, string_view{}};
    assert(s18.empty());
#endif // _HAS_CXX23

    s1 = s5;
    assert(s1.empty());
    s2 = move(s1);
    assert(s2.empty());
#if _HAS_CXX17
    s16 = string_view{};
    assert(s16.empty());
#endif // _HAS_CXX17

    assert(s2.begin() == s2.end());
    assert(as_const(s2).begin() == as_const(s2).end());
    assert(s2.cbegin() == s2.cend());
    assert(s2.rbegin() == s2.rend());
    assert(as_const(s2).rbegin() == as_const(s2).rend());
    assert(s2.crbegin() == s2.crend());
    assert(s2.size() == 0);
    assert(s2.length() == 0);
    assert(s2.max_size() != 0);
    assert(s2.capacity() >= s2.size());

    s2.resize(UDiff{1}, '*');
    assert(s2 == "*");
    s2.resize(UDiff{1});
    assert(s2 == "*");
    s2.reserve(UDiff{sizeof(s2)});
    assert(s2 == "*");
    assert(s2.capacity() >= sizeof(s2));
    s2.shrink_to_fit();
    s2.clear();
    assert(s2.empty());
#if _HAS_CXX23
    s2.resize_and_overwrite(UDiff{1}, [](char* p, UDiff) {
        p[0] = '*';
        return UDiff{1};
    });
    assert(s2 == "*");
    s2.clear();
#endif // _HAS_CXX23

    assert(s11.front() == '*');
    assert(as_const(s11).front() == '*');
    assert(s11.back() == '*');
    assert(as_const(s11).back() == '*');
    assert(s11[0] == '*');
    assert(s11[1] == '\0');
    assert(as_const(s11)[0] == '*');
    assert(as_const(s11)[1] == '\0');
    assert(s11.at(0) == '*');
    assert(as_const(s11).at(0) == '*');

    s2 += s2;
    assert(s2.empty());
    s2 += "";
    assert(s2.empty());
#if _HAS_CXX17
    s2 += string_view{};
    assert(s2.empty());
#endif // _HAS_CXX17
    s2 += '*';
    assert(s2 == "*");
    s2 += {'g', 'h'};
    assert(s2 == "*gh");

    s2.append(s5);
    assert(s2 == "*gh");
    s2.append(s5, UDiff{0}, UDiff{0});
    assert(s2 == "*gh");
#if _HAS_CXX17
    s2.append(string_view{});
    assert(s2 == "*gh");
    s2.append(string_view{}, UDiff{0}, UDiff{0});
    assert(s2 == "*gh");
#endif // _HAS_CXX17
    s2.append("", UDiff{0});
    assert(s2 == "*gh");
    s2.append("");
    assert(s2 == "*gh");
    s2.append(UDiff{1}, '*');
    assert(s2 == "*gh*");
    s2.append(s5.begin(), s5.end());
    assert(s2 == "*gh*");
#if _HAS_CXX23
    s2.append_range(s5);
    assert(s2 == "*gh*");
#endif // _HAS_CXX23
    s2.append({'i', 'j'});
    assert(s2 == "*gh*ij");
    s2.push_back('k');
    assert(s2 == "*gh*ijk");

    s1.assign(s5);
    assert(s1.empty());
    s2.assign(move(s1));
    assert(s2.empty());
    s2.assign(s5, UDiff{0}, UDiff{0});
    assert(s2.empty());
#if _HAS_CXX17
    s2.assign(string_view{});
    assert(s2.empty());
    s2.assign(string_view{}, UDiff{0}, UDiff{0});
    assert(s2.empty());
#endif // _HAS_CXX17
    s2.assign("kl", UDiff{2});
    assert(s2 == "kl");
    s2.assign("mn");
    assert(s2 == "mn");
    s2.assign({'o', 'p'});
    assert(s2 == "op");
    s2.assign(s5.begin(), s5.end());
    assert(s2.empty());
#if _HAS_CXX23
    s2.assign_range(s5);
    assert(s2.empty());
#endif // _HAS_CXX23

    s2.insert(UDiff{0}, s5);
    assert(s2.empty());
    s2.insert(UDiff{0}, s5, UDiff{0}, UDiff{0});
    assert(s2.empty());
#if _HAS_CXX17
    s2.insert(UDiff{0}, string_view{});
    assert(s2.empty());
    s2.insert(UDiff{0}, string_view{}, UDiff{0}, UDiff{0});
    assert(s2.empty());
#endif // _HAS_CXX17
    s2.insert(UDiff{0}, "qr", UDiff{2});
    assert(s2 == "qr");
    s2.insert(UDiff{0}, "st");
    assert(s2 == "stqr");
    s2.insert(UDiff{2}, UDiff{2}, '*');
    assert(s2 == "st**qr");
    s2.insert(s2.begin(), '*');
    assert(s2 == "*st**qr");
    s2.insert(s2.begin(), UDiff{1}, '*');
    assert(s2 == "**st**qr");
#if _HAS_CXX23
    s2.insert_range(s2.begin(), s5);
    assert(s2 == "**st**qr");
#endif // _HAS_CXX23
    s2.insert(s2.begin(), {'u', 'v'});
    assert(s2 == "uv**st**qr");

    s2.erase(UDiff{3}, tested_string::npos);
    assert(s2 == "uv*");
    s2.erase(next(s2.begin()));
    assert(s2 == "u*");
    s2.erase(next(s2.begin()), s2.end());
    assert(s2 == "u");
    s2.pop_back();
    assert(s2.empty());

    s2.replace(UDiff{0}, UDiff{0}, s5);
    assert(s2.empty());
    s2.replace(UDiff{0}, UDiff{0}, s5, UDiff{0}, UDiff{0});
    assert(s2.empty());
#if _HAS_CXX17
    s2.replace(UDiff{0}, UDiff{0}, string_view{});
    assert(s2.empty());
    s2.replace(UDiff{0}, UDiff{0}, string_view{}, UDiff{0}, UDiff{0});
    assert(s2.empty());
#endif // _HAS_CXX17
    s2.replace(UDiff{0}, UDiff{0}, "wx", 2);
    assert(s2 == "wx");
    s2.replace(UDiff{0}, UDiff{0}, "yz");
    assert(s2 == "yzwx");
    s2.replace(UDiff{0}, UDiff{4}, UDiff{1}, '*');
    assert(s2 == "*");
    s2.replace(s2.begin(), s2.end(), s5);
    assert(s2.empty());
#if _HAS_CXX17
    s2.replace(s2.begin(), s2.end(), string_view{});
    assert(s2.empty());
#endif // _HAS_CXX17
    s2.replace(s2.begin(), s2.end(), "AB", UDiff{2});
    assert(s2 == "AB");
    s2.replace(s2.begin(), s2.end(), "CD");
    assert(s2 == "CD");
    s2.replace(s2.begin(), s2.end(), UDiff{1}, '*');
    assert(s2 == "*");
    s2.replace(s2.begin(), s2.end(), s5.begin(), s5.end());
    assert(s2.empty());
#if _HAS_CXX23
    s2.replace_with_range(s2.begin(), s2.end(), s5);
    assert(s2.empty());
#endif // _HAS_CXX23
    s2.replace(s2.begin(), s2.end(), {'E', 'F'});
    assert(s2 == "EF");

    s5.copy(&s2[0], UDiff{0}, UDiff{0});
    assert(s2 == "EF");

    s2.swap(s5);
    assert(s2.empty());
    assert(s5 == "EF");
    swap(s2, s5);
    assert(s2 == "EF");
    assert(s5.empty());

    assert(s2.data()[0] == 'E');
    assert(as_const(s2).data()[0] == 'E');
    assert(s2.c_str()[0] == 'E');
    assert(s2.data() == as_const(s2).data());
    assert(s2.data() == s2.c_str());
#if _HAS_CXX17
    {
        string_view sv = s2;
        assert(sv == "EF");
    }
#endif // _HAS_CXX17
    assert(s2.get_allocator() == tested_allocator{});

#if _HAS_CXX17
    assert(s2.find(string_view{s2}, UDiff{0}) == 0);
#endif // _HAS_CXX17
    assert(s2.find(s2, UDiff{0}) == 0);
    assert(s2.find("EF", UDiff{0}, UDiff{2}) == 0);
    assert(s2.find("EF", UDiff{0}) == 0);
    assert(s2.find('E', UDiff{0}) == 0);

#if _HAS_CXX17
    assert(s2.rfind(string_view{s2}, UDiff{0}) == 0);
#endif // _HAS_CXX17
    assert(s2.rfind(s2, UDiff{0}) == 0);
    assert(s2.rfind("EF", UDiff{0}, UDiff{2}) == 0);
    assert(s2.rfind("EF", UDiff{0}) == 0);
    assert(s2.rfind('E', UDiff{0}) == 0);

#if _HAS_CXX17
    assert(s2.find_first_of(string_view{s2}, UDiff{0}) == 0);
#endif // _HAS_CXX17
    assert(s2.find_first_of(s2, UDiff{0}) == 0);
    assert(s2.find_first_of("EF", UDiff{0}, UDiff{2}) == 0);
    assert(s2.find_first_of("EF", UDiff{0}) == 0);
    assert(s2.find_first_of('E', UDiff{0}) == 0);

#if _HAS_CXX17
    assert(s2.find_last_of(string_view{s2}, UDiff{0}) == 0);
#endif // _HAS_CXX17
    assert(s2.find_last_of(s2, UDiff{0}) == 0);
    assert(s2.find_last_of("EF", UDiff{0}, UDiff{2}) == 0);
    assert(s2.find_last_of("EF", UDiff{0}) == 0);
    assert(s2.find_last_of('E', UDiff{0}) == 0);

#if _HAS_CXX17
    assert(s2.find_first_not_of(string_view{"AB"}, UDiff{0}) == 0);
#endif // _HAS_CXX17
    assert(s2.find_first_not_of(tested_string{"AB"}, UDiff{0}) == 0);
    assert(s2.find_first_not_of("AB", UDiff{0}, UDiff{2}) == 0);
    assert(s2.find_first_not_of("AB", UDiff{0}) == 0);
    assert(s2.find_first_not_of('A', UDiff{0}) == 0);

#if _HAS_CXX17
    assert(s2.find_last_not_of(string_view{"AB"}, UDiff{0}) == 0);
#endif // _HAS_CXX17
    assert(s2.find_last_not_of(tested_string{"AB"}, UDiff{0}) == 0);
    assert(s2.find_last_not_of("AB", UDiff{0}, UDiff{2}) == 0);
    assert(s2.find_last_not_of("AB", UDiff{0}) == 0);
    assert(s2.find_last_not_of('A', UDiff{0}) == 0);

    assert(s2.substr(UDiff{0}, UDiff{0}).empty());
    assert(tested_string{s2}.substr(UDiff{0}, UDiff{0}).empty());

#if _HAS_CXX17
    assert(s2.compare(string_view{"EF"}) == 0);
    assert(s2.compare(UDiff{0}, UDiff{0}, string_view{}) == 0);
    assert(s2.compare(UDiff{0}, UDiff{0}, string_view{}, UDiff{0}, UDiff{0}) == 0);
#endif // _HAS_CXX17
    assert(s2.compare(s2) == 0);
    assert(s2.compare(UDiff{0}, UDiff{0}, tested_string{}) == 0);
    assert(s2.compare(UDiff{0}, UDiff{0}, tested_string{}, UDiff{0}, UDiff{0}) == 0);
    assert(s2.compare("EF") == 0);
    assert(s2.compare(UDiff{0}, UDiff{0}, "") == 0);
    assert(s2.compare(UDiff{0}, UDiff{2}, "EF", UDiff{2}) == 0);

#if _HAS_CXX20
    assert(s2.starts_with(string_view{"E"}));
    assert(s2.starts_with("E"));
    assert(s2.starts_with('E'));

    assert(s2.ends_with(string_view{"F"}));
    assert(s2.ends_with("F"));
    assert(s2.ends_with('F'));
#endif // _HAS_CXX20
#if _HAS_CXX23
    assert(s2.contains(string_view{"EF"}));
    assert(s2.contains("EF"));
    assert(s2.contains('E'));
#endif // _HAS_CXX23

    assert(s2 + s2 == tested_string{"EFEF"});
    assert(s2 + tested_string{} == tested_string{"EF"});
    assert(tested_string{} + s2 == tested_string{"EF"});
    assert(tested_string{} + tested_string{} == tested_string{});

    assert(s2 + "" == tested_string{"EF"});
    assert("" + s2 == tested_string{"EF"});
    assert(tested_string{} + "" == tested_string{});
    assert("" + tested_string{} == tested_string{});

    assert(s2 + '*' == tested_string{"EF*"});
    assert('*' + s2 == tested_string{"*EF"});
    assert(tested_string{} + '*' == tested_string{"*"});
    assert('*' + tested_string{} == tested_string{"*"});

    assert(s2 == s2);
    assert(!(s2 != s2));
    assert(!(s2 < s2));
    assert(!(s2 > s2));
    assert(s2 <= s2);
    assert(s2 >= s2);

    assert(s2 == "EF");
    assert(!(s2 != "EF"));
    assert(!(s2 < "EF"));
    assert(!(s2 > "EF"));
    assert(s2 <= "EF");
    assert(s2 >= "EF");

    assert("EF" == s2);
    assert(!("EF" != s2));
    assert(!("EF" < s2));
    assert(!("EF" > s2));
    assert("EF" <= s2);
    assert("EF" >= s2);
#if _HAS_CXX20
    assert(s2 <=> s2 == strong_ordering::equal);
    assert(s2 <=> "EF" == strong_ordering::equal);
#endif // _HAS_CXX20

#if _HAS_CXX20
    erase(s2, 'E');
    assert(s2 == "F");
    erase_if(s2, [](char) { return true; });
    assert(s2.empty());
#endif // _HAS_CXX20

#if _HAS_CXX20
    if (!is_constant_evaluated())
#endif // _HAS_CXX20
    {
        test_basic_string_getline<Diff>();

        tested_string s19 = "Hello world!";
        assert(hash<tested_string>{}(s19) == hash<string>{}(string{"Hello world!"}));
#if _HAS_CXX17
        assert(hash<tested_string>{}(s19) == hash<string_view>{}(string_view{"Hello world!"}));
#endif // _HAS_CXX17
    }
}

template <class Diff>
CONSTEXPR20 void test_single() {
    test_basic_string<Diff>();
}

CONSTEXPR20 bool test() {
    test_single<signed char>();
    test_single<short>();
    test_single<int>();
    test_single<long>();
    test_single<long long>();
    return true;
}

#if _HAS_CXX20
static_assert(test());
#endif // _HAS_CXX20

int main() {
    test();
}
