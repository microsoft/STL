// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _HAS_DEPRECATED_RAW_STORAGE_ITERATOR 1
#define _SILENCE_CXX17_RAW_STORAGE_ITERATOR_DEPRECATION_WARNING
#define _SILENCE_CXX23_ALIGNED_UNION_DEPRECATION_WARNING
#define _SILENCE_EXPERIMENTAL_ERASE_DEPRECATION_WARNING

#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <complex>
#include <cstddef>
#include <deque>
#include <experimental/deque>
#include <experimental/forward_list>
#include <experimental/list>
#include <experimental/map>
#include <experimental/set>
#include <experimental/string>
#include <experimental/unordered_map>
#include <experimental/unordered_set>
#include <experimental/vector>
#include <forward_list>
#include <iterator>
#include <list>
#include <map>
#include <memory>
#include <random>
#include <set>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#if _HAS_CXX17
#include <memory_resource>
#endif // _HAS_CXX17

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

// INTENTIONALLY AVOIDED: using namespace std;

template <typename Expected, typename T>
void test_integral(T t) {
    STATIC_ASSERT(std::is_same_v<Expected, T>);
    assert(t.count() == 1729);
}

template <typename Expected, typename T>
void test_floating(T t) {
    STATIC_ASSERT(std::is_same_v<Expected, T>);
    assert(3.13 < t.count() && t.count() < 3.15);
}

template <typename T>
void test_default_ctor() {
    T t0;
    T t1{};
    T t2 = {};
}

void test_N4510();
void test_LWG_3037();
void test_LWG_2106();
void test_LWG_2353();

int main() {
    // N3642 "User-defined Literals for Standard Library Types"
    // N3779 "User-defined Literals for std::complex"
    // LWG-2278 "User-defined literals for Standard Library types"


#define TEST_CHRONO                                                        \
    test_integral<std::chrono::hours>(1729h);                              \
    test_integral<std::chrono::minutes>(1729min);                          \
    test_integral<std::chrono::seconds>(1729s);                            \
    test_integral<std::chrono::milliseconds>(1729ms);                      \
    test_integral<std::chrono::microseconds>(1729us);                      \
    test_integral<std::chrono::nanoseconds>(1729ns);                       \
    test_floating<std::chrono::duration<double, std::ratio<3600>>>(3.14h); \
    test_floating<std::chrono::duration<double, std::ratio<60>>>(3.14min); \
    test_floating<std::chrono::duration<double>>(3.14s);                   \
    test_floating<std::chrono::duration<double, std::milli>>(3.14ms);      \
    test_floating<std::chrono::duration<double, std::micro>>(3.14us);      \
    test_floating<std::chrono::duration<double, std::nano>>(3.14ns);

    {
        using namespace std::literals::chrono_literals;
        TEST_CHRONO
    }

    {
        using namespace std::chrono_literals;
        TEST_CHRONO
    }

    {
        using namespace std::literals;
        TEST_CHRONO
    }

    {
        using namespace std;
        TEST_CHRONO
    }

    {
        using namespace std::chrono;
        TEST_CHRONO
    }

    {
        using namespace std;
        using namespace std::chrono;
        TEST_CHRONO
    }

#undef TEST_CHRONO


#define TEST_COMPLEX                                                        \
    auto c1 = 12.34il;                                                      \
    STATIC_ASSERT(std::is_same_v<decltype(c1), std::complex<long double>>); \
    assert(c1.real() == 0.0L);                                              \
    assert(12.33L < c1.imag() && c1.imag() < 12.35L);                       \
    auto c2 = 2048il;                                                       \
    STATIC_ASSERT(std::is_same_v<decltype(c2), std::complex<long double>>); \
    assert(c2.real() == 0.0L);                                              \
    assert(c2.imag() == 2048.0L);                                           \
    auto c3 = 56.78i;                                                       \
    STATIC_ASSERT(std::is_same_v<decltype(c3), std::complex<double>>);      \
    assert(c3.real() == 0.0);                                               \
    assert(56.77 < c3.imag() && c3.imag() < 56.79);                         \
    auto c4 = 1729i;                                                        \
    STATIC_ASSERT(std::is_same_v<decltype(c4), std::complex<double>>);      \
    assert(c4.real() == 0.0);                                               \
    assert(c4.imag() == 1729.0);                                            \
    auto c5 = 3.14if;                                                       \
    STATIC_ASSERT(std::is_same_v<decltype(c5), std::complex<float>>);       \
    assert(c5.real() == 0.0f);                                              \
    assert(3.13f < c5.imag() && c5.imag() < 3.15f);                         \
    auto c6 = 8192if;                                                       \
    STATIC_ASSERT(std::is_same_v<decltype(c6), std::complex<float>>);       \
    assert(c6.real() == 0.0f);                                              \
    assert(c6.imag() == 8192.0f);

    {
        using namespace std::literals::complex_literals;
        TEST_COMPLEX
    }

    {
        using namespace std::complex_literals;
        TEST_COMPLEX
    }

    {
        using namespace std::literals;
        TEST_COMPLEX
    }

    {
        using namespace std;
        TEST_COMPLEX
    }

#undef TEST_COMPLEX


#define TEST_STRING                                              \
    auto s1 = "meow"s;                                           \
    STATIC_ASSERT(std::is_same_v<decltype(s1), std::string>);    \
    assert(s1 == "meow");                                        \
    auto s2 = L"purr"s;                                          \
    STATIC_ASSERT(std::is_same_v<decltype(s2), std::wstring>);   \
    assert(s2 == L"purr");                                       \
    auto s3 = u"bark"s;                                          \
    STATIC_ASSERT(std::is_same_v<decltype(s3), std::u16string>); \
    assert(s3 == u"bark");                                       \
    auto s4 = U"woof"s;                                          \
    STATIC_ASSERT(std::is_same_v<decltype(s4), std::u32string>); \
    assert(s4 == U"woof");

#ifdef __cpp_char8_t
#define TEST_U8STRING                                                        \
    auto s5 = u8"woof"s;                                                     \
    STATIC_ASSERT(std::is_same_v<decltype(s5), std::basic_string<char8_t>>); \
    assert(s5 == u8"woof");
#else // ^^^ __cpp_char8_t / !__cpp_char8_t vvv
#define TEST_U8STRING
#endif // __cpp_char8_t

    {
        using namespace std::literals::string_literals;
        TEST_STRING
        TEST_U8STRING
    }

    {
        using namespace std::string_literals;
        TEST_STRING
        TEST_U8STRING
    }

    {
        using namespace std::literals;
        TEST_STRING
        TEST_U8STRING
    }

    {
        using namespace std;
        TEST_STRING
        TEST_U8STRING
    }

#undef TEST_STRING
#undef TEST_U8STRING


    // N3887 "Consistent Metafunction Aliases"
    STATIC_ASSERT(std::is_same_v<std::tuple_element_t<0, std::tuple<short, int, long>>, short>);
    STATIC_ASSERT(std::is_same_v<std::tuple_element_t<1, std::tuple<short, int, long>>, int>);
    STATIC_ASSERT(std::is_same_v<std::tuple_element_t<2, std::tuple<short, int, long>>, long>);
    STATIC_ASSERT(std::is_same_v<std::tuple_element_t<0, std::pair<float, double>>, float>);
    STATIC_ASSERT(std::is_same_v<std::tuple_element_t<1, std::pair<float, double>>, double>);
    STATIC_ASSERT(std::is_same_v<std::tuple_element_t<4, std::array<char, 7>>, char>);


    // LWG-2112 "User-defined classes that cannot be derived from"
    {
        struct Plain {};
        struct Final final {};
        union PlainUnion {};
        union FinalUnion final {};
        enum UnscopedEnum { Stuff1 };
        enum class ScopedEnum { Stuff2 };

        STATIC_ASSERT(!std::is_final_v<Plain>);
        STATIC_ASSERT(std::is_final_v<Final>);
        STATIC_ASSERT(!std::is_final_v<PlainUnion>);
        STATIC_ASSERT(std::is_final_v<FinalUnion>);

        STATIC_ASSERT(!std::is_final_v<void>);
        STATIC_ASSERT(!std::is_final_v<std::nullptr_t>);
        STATIC_ASSERT(!std::is_final_v<int>);
        STATIC_ASSERT(!std::is_final_v<double>);
        STATIC_ASSERT(!std::is_final_v<int[3]>);
        STATIC_ASSERT(!std::is_final_v<int[]>);
        STATIC_ASSERT(!std::is_final_v<int*>);
        STATIC_ASSERT(!std::is_final_v<int (*)(int)>);
        STATIC_ASSERT(!std::is_final_v<int&>);
        STATIC_ASSERT(!std::is_final_v<int&&>);
        STATIC_ASSERT(!std::is_final_v<int Plain::*>);
        STATIC_ASSERT(!std::is_final_v<int (Plain::*)(int)>);
        STATIC_ASSERT(!std::is_final_v<UnscopedEnum>);
        STATIC_ASSERT(!std::is_final_v<ScopedEnum>);
        STATIC_ASSERT(!std::is_final_v<int(int)>);
    }


    // LWG-2193 "Default constructors for standard library containers are explicit"
    test_default_ctor<std::string>();
    test_default_ctor<std::deque<int>>();
    test_default_ctor<std::forward_list<int>>();
    test_default_ctor<std::list<int>>();
    test_default_ctor<std::vector<int>>();
    test_default_ctor<std::vector<bool>>();
    test_default_ctor<std::map<int, int>>();
    test_default_ctor<std::multimap<int, int>>();
    test_default_ctor<std::set<int>>();
    test_default_ctor<std::multiset<int>>();
    test_default_ctor<std::unordered_map<int, int>>();
    test_default_ctor<std::unordered_multimap<int, int>>();
    test_default_ctor<std::unordered_set<int>>();
    test_default_ctor<std::unordered_multiset<int>>();


    // LWG-2268 "Setting a default argument in the declaration of a member function assign of std::basic_string"
    {
        const std::string kitty("CUTE FLUFFY KITTENS");

        std::string s("meow");

        s.append(kitty, 12);
        assert(s == "meowKITTENS");

        s.assign(kitty, 5);
        assert(s == "FLUFFY KITTENS");

        s = "peppermint";
        s.insert(6, kitty, 15);
        assert(s == "pepperTENSmint");

        s.replace(4, std::string::npos, kitty, 10);
        assert(s == "peppY KITTENS");

        s = "ADORABLE ZOMBIE KITTENS";
        assert(s.compare(0, 8, kitty, 12) < 0);
        assert(s.compare(9, 6, kitty, 12) > 0);
        assert(s.compare(16, 7, kitty, 12) == 0);
    }


    // LWG-2285 "make_reverse_iterator"
    {
        const std::list<int> lst = {11, 22, 33, 44, 55};
        const std::vector<int> v(std::make_reverse_iterator(lst.end()), std::make_reverse_iterator(lst.begin()));
        const std::vector<int> correct = {55, 44, 33, 22, 11};
        assert(v == correct);
    }


    // LWG-2315 "weak_ptr should be movable"
    {
        std::shared_ptr<int> sp = std::make_shared<int>(1729);

        std::weak_ptr<int> wp1(sp);
        assert(wp1.lock() == sp);

        std::weak_ptr<int> wp2(std::move(wp1));
        assert(wp1.expired());
        assert(wp2.lock() == sp);

        std::weak_ptr<int> wp3;
        wp3 = std::move(wp2);
        assert(wp2.expired());
        assert(wp3.lock() == sp);

        std::weak_ptr<const int> wp4(std::move(wp3));
        assert(wp3.expired());
        assert(wp4.lock() == sp);

        wp1 = sp;
        assert(wp1.lock() == sp);
        std::weak_ptr<const int> wp5;
        wp5 = std::move(wp1);
        assert(wp1.expired());
        assert(wp5.lock() == sp);
    }


    // LWG-2339 "Wording issue in nth_element"
    {
        std::mt19937 mt(1729);

        std::vector<unsigned int> v(1024);

        for (auto& e : v) {
            e = mt();
        }

        const auto orig = v;

        nth_element(v.begin(), v.end(), v.end());

        // The Standard doesn't guarantee this, but we've added a no-op check.
        assert(v == orig);
    }


    // N4389 bool_constant
    STATIC_ASSERT(std::is_same_v<std::bool_constant<true>, std::integral_constant<bool, true>>);
    STATIC_ASSERT(std::is_same_v<std::bool_constant<false>, std::integral_constant<bool, false>>);


    // LWG-2454 "Add raw_storage_iterator::base() member"
    {
        int arr[] = {11, 22, 33};

        std::raw_storage_iterator<int*, int> it(arr);

        assert(it.base() == arr);
    }


    // N4273 Uniform Container Erasure
    {
        // Note that the standard actually requires these to be copyable. As an extension, we want
        // to ensure we don't copy them, because copying some functors (e.g. std::function) is comparatively
        // expensive, and even for relatively cheap to copy function objects we care (somewhat) about debug
        // mode perf.
        struct no_copy {
            no_copy()                          = default;
            no_copy(const no_copy&)            = delete;
            no_copy(no_copy&&)                 = default;
            no_copy& operator=(const no_copy&) = delete;
            no_copy& operator=(no_copy&&)      = delete;
        };

        struct is_vowel : no_copy {
            bool operator()(const char c) const {
                return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u';
            }
        };

        std::string str1{"cute fluffy kittens"};
        std::experimental::erase_if(str1, is_vowel{});
        assert(str1 == "ct flffy kttns");

        std::string str2{"asynchronous beat"};
        std::experimental::erase(str2, 'a');
        assert(str2 == "synchronous bet");

        struct is_odd : no_copy {
            bool operator()(const int i) const {
                return i % 2 != 0;
            }
        };

        std::deque<int> d{1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1};
        std::experimental::erase_if(d, is_odd{});
        assert((d == std::deque<int>{2, 4, 6, 6, 4, 2}));
        std::experimental::erase(d, 4);
        assert((d == std::deque<int>{2, 6, 6, 2}));

        std::vector<int> v{1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1};
        std::experimental::erase_if(v, is_odd{});
        assert((v == std::vector<int>{2, 4, 6, 6, 4, 2}));
        std::experimental::erase(v, 4);
        assert((v == std::vector<int>{2, 6, 6, 2}));

        std::forward_list<int> fl{1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1};
        std::experimental::erase_if(fl, is_odd{});
        assert((fl == std::forward_list<int>{2, 4, 6, 6, 4, 2}));
        std::experimental::erase(fl, 4);
        assert((fl == std::forward_list<int>{2, 6, 6, 2}));

        std::list<int> l{1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1};
        std::experimental::erase_if(l, is_odd{});
        assert((l == std::list<int>{2, 4, 6, 6, 4, 2}));
        std::experimental::erase(l, 4);
        assert((l == std::list<int>{2, 6, 6, 2}));

        struct is_first_odd : no_copy {
            bool operator()(const std::pair<const int, int>& p) const {
                return p.first % 2 != 0;
            }
        };

        std::map<int, int> m{{1, 10}, {2, 20}, {3, 30}, {4, 40}, {5, 50}, {6, 60}, {7, 70}};
        std::experimental::erase_if(m, is_first_odd{});
        assert((m == std::map<int, int>{{2, 20}, {4, 40}, {6, 60}}));

        std::multimap<int, int> mm{{1, 10}, {2, 20}, {3, 30}, {4, 40}, {5, 50}, {6, 60}, {7, 70}};
        std::experimental::erase_if(mm, is_first_odd{});
        assert((mm == std::multimap<int, int>{{2, 20}, {4, 40}, {6, 60}}));

        std::set<int> s{1, 2, 3, 4, 5, 6, 7};
        std::experimental::erase_if(s, is_odd{});
        assert((s == std::set<int>{2, 4, 6}));

        std::multiset<int> ms{1, 2, 3, 4, 5, 6, 7};
        std::experimental::erase_if(ms, is_odd{});
        assert((ms == std::multiset<int>{2, 4, 6}));

        // Note that unordered equality considers permutations.

        std::unordered_map<int, int> um{{1, 10}, {2, 20}, {3, 30}, {4, 40}, {5, 50}, {6, 60}, {7, 70}};
        std::experimental::erase_if(um, is_first_odd{});
        assert((um == std::unordered_map<int, int>{{2, 20}, {4, 40}, {6, 60}}));

        std::unordered_multimap<int, int> umm{{1, 10}, {2, 20}, {3, 30}, {4, 40}, {5, 50}, {6, 60}, {7, 70}};
        std::experimental::erase_if(umm, is_first_odd{});
        assert((umm == std::unordered_multimap<int, int>{{2, 20}, {4, 40}, {6, 60}}));

        std::unordered_set<int> us{1, 2, 3, 4, 5, 6, 7};
        std::experimental::erase_if(us, is_odd{});
        assert((us == std::unordered_set<int>{2, 4, 6}));

        std::unordered_multiset<int> ums{1, 2, 3, 4, 5, 6, 7};
        std::experimental::erase_if(ums, is_odd{});
        assert((ums == std::unordered_multiset<int>{2, 4, 6}));

// P0458R2 contains() For Ordered And Unordered Associative Containers
#if _HAS_CXX20
        assert(m.contains(2));
        assert(mm.contains(4));
        assert(s.contains(6));
        assert(ms.contains(2));
        assert(um.contains(4));
        assert(umm.contains(6));
        assert(us.contains(2));
        assert(ums.contains(4));

        std::map<int, int> em;
        std::multimap<int, int> emm;
        std::set<int> es;
        std::multiset<int> ems;
        std::unordered_map<int, int> eum;
        std::unordered_multimap<int, int> eumm;
        std::unordered_set<int> eus;
        std::unordered_multiset<int> eums;

        assert(!em.contains(2));
        assert(!emm.contains(4));
        assert(!es.contains(6));
        assert(!ems.contains(2));
        assert(!eum.contains(4));
        assert(!eumm.contains(6));
        assert(!eus.contains(2));
        assert(!eums.contains(4));

        const std::map<int, int> const_m                  = {{2, 20}, {4, 40}, {6, 60}};
        const std::multimap<int, int> const_mm            = {{2, 20}, {4, 40}, {6, 60}};
        const std::set<int> const_s                       = {2, 4, 6};
        const std::multiset<int> const_ms                 = {2, 4, 6};
        const std::unordered_map<int, int> const_um       = {{2, 20}, {4, 40}, {6, 60}};
        const std::unordered_multimap<int, int> const_umm = {{2, 20}, {4, 40}, {6, 60}};
        const std::unordered_set<int> const_us            = {2, 4, 6};
        const std::unordered_multiset<int> const_ums      = {2, 4, 6};

        assert(const_m.contains(2));
        assert(const_mm.contains(4));
        assert(const_s.contains(6));
        assert(const_ms.contains(2));
        assert(const_um.contains(4));
        assert(const_umm.contains(6));
        assert(const_us.contains(2));
        assert(const_ums.contains(4));

        assert(!const_m.contains(1));
        assert(!const_mm.contains(3));
        assert(!const_s.contains(5));
        assert(!const_ms.contains(1));
        assert(!const_um.contains(3));
        assert(!const_umm.contains(5));
        assert(!const_us.contains(1));
        assert(!const_ums.contains(3));
#endif // _HAS_CXX20
    }

#if _HAS_CXX20
    // P1209R0 erase_if(), erase()
    {
        // Note that the standard actually requires these to be copyable. As an extension, we want
        // to ensure we don't copy them, because copying some functors (e.g. std::function) is comparatively
        // expensive, and even for relatively cheap to copy function objects we care (somewhat) about debug
        // mode perf.
        struct no_copy {
            no_copy()                          = default;
            no_copy(const no_copy&)            = delete;
            no_copy(no_copy&&)                 = default;
            no_copy& operator=(const no_copy&) = delete;
            no_copy& operator=(no_copy&&)      = delete;
        };

        struct is_vowel : no_copy {
            bool operator()(const char c) const {
                return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u';
            }
        };

        std::string str1{"cute fluffy kittens"};
        const auto str1_removed = std::erase_if(str1, is_vowel{});
        assert(str1 == "ct flffy kttns");
        assert(str1_removed == 5);

        std::string str2{"asynchronous beat"};
        const auto str2_removed = std::erase(str2, 'a');
        assert(str2 == "synchronous bet");
        assert(str2_removed == 2);

        struct is_odd : no_copy {
            bool operator()(const int i) const {
                return i % 2 != 0;
            }
        };

        std::deque<int> d{1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1};
        const auto d_removed = std::erase_if(d, is_odd{});
        assert((d == std::deque<int>{2, 4, 6, 6, 4, 2}));
        assert(d_removed == 7);
        const auto d_removed2 = std::erase(d, 4);
        assert((d == std::deque<int>{2, 6, 6, 2}));
        assert(d_removed2 == 2);

        std::vector<int> v{1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1};
        const auto v_removed = std::erase_if(v, is_odd{});
        assert((v == std::vector<int>{2, 4, 6, 6, 4, 2}));
        assert(v_removed == 7);
        const auto v_removed2 = std::erase(v, 4);
        assert((v == std::vector<int>{2, 6, 6, 2}));
        assert(v_removed2 == 2);

        std::forward_list<int> fl{1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1};
        const auto fl_removed = std::erase_if(fl, is_odd{});
        assert((fl == std::forward_list<int>{2, 4, 6, 6, 4, 2}));
        assert(fl_removed == 7);
        const auto fl_removed2 = std::erase(fl, 4);
        assert((fl == std::forward_list<int>{2, 6, 6, 2}));
        assert(fl_removed2 == 2);

        std::list<int> l{1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1};
        const auto l_removed = std::erase_if(l, is_odd{});
        assert((l == std::list<int>{2, 4, 6, 6, 4, 2}));
        assert(l_removed == 7);
        const auto l_removed2 = std::erase(l, 4);
        assert((l == std::list<int>{2, 6, 6, 2}));
        assert(l_removed2 == 2);

        struct is_first_odd : no_copy {
            bool operator()(const std::pair<const int, int>& p) const {
                return p.first % 2 != 0;
            }
        };

        std::map<int, int> m{{1, 10}, {2, 20}, {3, 30}, {4, 40}, {5, 50}, {6, 60}, {7, 70}};
        const auto m_removed = std::erase_if(m, is_first_odd{});
        assert((m == std::map<int, int>{{2, 20}, {4, 40}, {6, 60}}));
        assert(m_removed == 4);

        std::multimap<int, int> mm{{1, 10}, {2, 20}, {3, 30}, {4, 40}, {5, 50}, {6, 60}, {7, 70}};
        const auto mm_removed = std::erase_if(mm, is_first_odd{});
        assert((mm == std::multimap<int, int>{{2, 20}, {4, 40}, {6, 60}}));
        assert(mm_removed == 4);

        std::set<int> s{1, 2, 3, 4, 5, 6, 7};
        const auto s_removed = std::erase_if(s, is_odd{});
        assert((s == std::set<int>{2, 4, 6}));
        assert(s_removed == 4);

        std::multiset<int> ms{1, 2, 3, 4, 5, 6, 7};
        const auto ms_removed = std::erase_if(ms, is_odd{});
        assert((ms == std::multiset<int>{2, 4, 6}));
        assert(ms_removed == 4);

        // Note that unordered equality considers permutations.

        std::unordered_map<int, int> um{{1, 10}, {2, 20}, {3, 30}, {4, 40}, {5, 50}, {6, 60}, {7, 70}};
        const auto um_removed = std::erase_if(um, is_first_odd{});
        assert((um == std::unordered_map<int, int>{{2, 20}, {4, 40}, {6, 60}}));
        assert(um_removed == 4);

        std::unordered_multimap<int, int> umm{{1, 10}, {2, 20}, {3, 30}, {4, 40}, {5, 50}, {6, 60}, {7, 70}};
        const auto umm_removed = std::erase_if(umm, is_first_odd{});
        assert((umm == std::unordered_multimap<int, int>{{2, 20}, {4, 40}, {6, 60}}));
        assert(umm_removed == 4);

        std::unordered_set<int> us{1, 2, 3, 4, 5, 6, 7};
        const auto us_removed = std::erase_if(us, is_odd{});
        assert((us == std::unordered_set<int>{2, 4, 6}));
        assert(us_removed == 4);

        std::unordered_multiset<int> ums{1, 2, 3, 4, 5, 6, 7};
        const auto ums_removed = std::erase_if(ums, is_odd{});
        assert((ums == std::unordered_multiset<int>{2, 4, 6}));
        assert(ums_removed == 4);
    }
#endif // _HAS_CXX20

    // P0007R1 as_const()
    {
        using namespace std;
        int i       = 1729;
        const int k = 2015;

        STATIC_ASSERT(is_same_v<decltype(as_const(i)), const int&>);
        STATIC_ASSERT(is_same_v<decltype(as_const(k)), const int&>);

        STATIC_ASSERT(noexcept(as_const(i)));
        STATIC_ASSERT(noexcept(as_const(k)));

        assert(&as_const(i) == &i);
        assert(&as_const(k) == &k);
    }


    // P0074R0 owner_less<>
    {
        using namespace std;
        using IsTran = owner_less<>::is_transparent;
        STATIC_ASSERT(is_fundamental_v<IsTran> || is_compound_v<IsTran>);

        const owner_less<> ol{};
        assert(!ol(shared_ptr<int>{}, shared_ptr<double>{}));
        assert(!ol(shared_ptr<int>{}, weak_ptr<double>{}));
        assert(!ol(weak_ptr<int>{}, shared_ptr<double>{}));
        assert(!ol(weak_ptr<int>{}, weak_ptr<double>{}));
    }


    // LWG-2127 "Move-construction with raw_storage_iterator"
    {
        using namespace std;

        using UP = unique_ptr<int>;

        using Storage = aligned_union_t<0, UP>;

        Storage storage{};

        UP* p = reinterpret_cast<UP*>(&storage);

        raw_storage_iterator<UP*, UP> rsi(p);

        *rsi = make_unique<int>(1729);

        assert(**p == 1729);

        p->~UP();
    }


    // We fixed VSO-97862 "<utility>: Serious quality of implementation concerns regarding `std::integer_sequence<>` et
    // al" by requesting a compiler hook, __make_integer_seq. Here are basic checks, to rule out problems like
    // VSO-172879 "__make_integer_seq doesn't work with 2 layers of alias templates" (which was fixed). Huge sequences
    // now work, but we're not testing them here because they inherently trigger warning C4503 "decorated name length
    // exceeded, name was truncated".
    {
        using namespace std;

        STATIC_ASSERT(is_same_v<make_integer_sequence<int, 0>, integer_sequence<int>>);
        STATIC_ASSERT(is_same_v<make_integer_sequence<int, 1>, integer_sequence<int, 0>>);
        STATIC_ASSERT(is_same_v<make_integer_sequence<int, 2>, integer_sequence<int, 0, 1>>);
        STATIC_ASSERT(is_same_v<make_integer_sequence<int, 3>, integer_sequence<int, 0, 1, 2>>);

        STATIC_ASSERT(is_same_v<make_integer_sequence<unsigned int, 0U>, integer_sequence<unsigned int>>);
        STATIC_ASSERT(is_same_v<make_integer_sequence<unsigned int, 1U>, integer_sequence<unsigned int, 0U>>);
        STATIC_ASSERT(is_same_v<make_integer_sequence<unsigned int, 2U>, integer_sequence<unsigned int, 0U, 1U>>);
        STATIC_ASSERT(is_same_v<make_integer_sequence<unsigned int, 3U>, integer_sequence<unsigned int, 0U, 1U, 2U>>);

        STATIC_ASSERT(is_same_v<make_integer_sequence<long long, 0LL>, integer_sequence<long long>>);
        STATIC_ASSERT(is_same_v<make_integer_sequence<long long, 1LL>, integer_sequence<long long, 0LL>>);
        STATIC_ASSERT(is_same_v<make_integer_sequence<long long, 2LL>, integer_sequence<long long, 0LL, 1LL>>);
        STATIC_ASSERT(is_same_v<make_integer_sequence<long long, 3LL>, integer_sequence<long long, 0LL, 1LL, 2LL>>);

        STATIC_ASSERT(is_same_v<make_integer_sequence<unsigned long long, 0ULL>, integer_sequence<unsigned long long>>);
        STATIC_ASSERT(
            is_same_v<make_integer_sequence<unsigned long long, 1ULL>, integer_sequence<unsigned long long, 0ULL>>);
        STATIC_ASSERT(is_same_v<make_integer_sequence<unsigned long long, 2ULL>,
            integer_sequence<unsigned long long, 0ULL, 1ULL>>);
        STATIC_ASSERT(is_same_v<make_integer_sequence<unsigned long long, 3ULL>,
            integer_sequence<unsigned long long, 0ULL, 1ULL, 2ULL>>);

        STATIC_ASSERT(is_same_v<index_sequence<1, 7, 2, 9>, integer_sequence<size_t, 1, 7, 2, 9>>);

        STATIC_ASSERT(is_same_v<make_index_sequence<4>, integer_sequence<size_t, 0, 1, 2, 3>>);

        STATIC_ASSERT(is_same_v<index_sequence_for<short, int, long>, integer_sequence<size_t, 0, 1, 2>>);
    }


    test_N4510();
    test_LWG_3037();
    test_LWG_2106();
    test_LWG_2353();
}


// N4510 Supporting Incomplete Types In vector/list/forward_list
struct Elem;

struct Meow {
    Meow();

    int get() const;

    std::vector<Elem> m_v;
    std::list<Elem> m_l;
    std::forward_list<Elem> m_fl;
};

struct Elem {
    int x;
};

Meow::Meow() {
    Elem e1 = {100};
    Elem e2 = {20};
    Elem e3 = {3};

    m_v.push_back(e1);
    m_l.push_back(e2);
    m_fl.push_front(e3);
}

int Meow::get() const {
    return m_v.back().x + m_l.back().x + m_fl.front().x;
}

void test_N4510() {
    Meow m;

    assert(m.get() == 123);
}

// LWG-3037 "polymorphic_allocator and incomplete types"
#if _HAS_CXX17
struct ElemPmr;

struct MeowPmr {
    MeowPmr();

    int get() const;

    std::pmr::vector<ElemPmr> m_v;
    std::pmr::list<ElemPmr> m_l;
    std::pmr::forward_list<ElemPmr> m_fl;
};

struct ElemPmr {
    int x;
};

MeowPmr::MeowPmr() {
    ElemPmr e1 = {100};
    ElemPmr e2 = {20};
    ElemPmr e3 = {3};

    m_v.push_back(e1);
    m_l.push_back(e2);
    m_fl.push_front(e3);
}

int MeowPmr::get() const {
    return m_v.back().x + m_l.back().x + m_fl.front().x;
}

void test_LWG_3037() {
    MeowPmr m;

    assert(m.get() == 123);
}
#else // _HAS_CXX17
void test_LWG_3037() {}
#endif // _HAS_CXX17

// LWG-2106 "move_iterator wrapping iterators returning prvalues"
struct InItPrvalue {
    using difference_type   = ptrdiff_t;
    using value_type        = std::string;
    using pointer           = std::string*;
    using reference         = std::string;
    using iterator_category = std::input_iterator_tag;

    std::vector<std::string>::iterator m_it;

    InItPrvalue(const std::vector<std::string>::iterator it, int) : m_it(it) {}

    bool operator==(const InItPrvalue& other) const {
        return m_it == other.m_it;
    }

    bool operator!=(const InItPrvalue& other) const {
        return m_it != other.m_it;
    }

    std::string operator*() const {
        return *m_it;
    }

    std::string* operator->() const {
        return &*m_it;
    }

    InItPrvalue& operator++() {
        ++m_it;
        return *this;
    }

    void operator++(int) = delete; // avoid postincrement
};

void test_LWG_2106() {
    using namespace std;

    using MoveNormal = move_iterator<vector<string>::iterator>;
    STATIC_ASSERT(is_same_v<MoveNormal::reference, string&&>);
    STATIC_ASSERT(is_same_v<decltype(*declval<MoveNormal>()), string&&>);
    vector<string> v1{"zero", "one", "two"};
    vector<string> v2(MoveNormal(v1.begin()), MoveNormal(v1.end()));
    assert(v2[2] == "two");

    using MovePrvalue = move_iterator<InItPrvalue>;
    STATIC_ASSERT(is_same_v<MovePrvalue::reference, string>);
    STATIC_ASSERT(is_same_v<decltype(*declval<MovePrvalue>()), string>);
    vector<string> v3{"ZERO", "ONE", "TWO"};
    vector<string> v4(MovePrvalue(InItPrvalue(v3.begin(), 555)), MovePrvalue(InItPrvalue(v3.end(), 555)));
    assert(v4[2] == "TWO");
}


// LWG-2353 "std::next is over-constrained"
void test_LWG_2353() {
    using namespace std;

    vector<string> v{"cute", "fluffy", "kittens"};

    InItPrvalue i(v.begin(), 555);

    assert(*next(i) == "fluffy");
}
