// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// ****************************************************************************************************
// THIS FILE IS SPECIAL!
// Don't import or include anything here.
// This is order-dependent. It assumes that the STL has been imported and <assert.h> has been included.
// This is not a true header file. It doesn't need `#pragma once` or `inline`.
// ****************************************************************************************************

void test_algorithm() {
    using namespace std;
    puts("Testing <algorithm>.");
    constexpr int arr[]{11, 0, 22, 0, 33, 0, 44, 0, 55};
    assert(count(begin(arr), end(arr), 0) == 4);
    assert(ranges::count(arr, 0) == 4);
    static_assert(count(begin(arr), end(arr), 0) == 4);
    static_assert(ranges::count(arr, 0) == 4);
}

void test_any() {
    using namespace std;
#if defined(_HAS_STATIC_RTTI) && _HAS_STATIC_RTTI == 0 // intentional: `import std;` can't provide a default definition
    puts("Nothing to test in <any> when static RTTI is disabled.");
#else // ^^^ static RTTI is disabled / static RTTI is enabled vvv
    puts("Testing <any>.");
    any a1{1729};
    any a2{7.5};
    a1.swap(a2);
    assert(any_cast<double>(a1) == 7.5);
    assert(any_cast<int>(a2) == 1729);
#endif // ^^^ static RTTI is enabled ^^^
}

void test_array() {
    using namespace std;
    puts("Testing <array>.");
    constexpr array arr{10, 20, 30, 40, 50};
    assert(arr[2] == 30);
    static_assert(arr[2] == 30);
}

void test_atomic() {
    using namespace std;
    puts("Testing <atomic>.");
    atomic<int> atom{1729};
    assert(atom.load() == 1729);
    ++atom;
    assert(atom.load() == 1730);
    static_assert(atomic<int>::is_always_lock_free); // implementation-defined
}

void test_barrier() {
    using namespace std;
    puts("Testing <barrier>.");
    static_assert(barrier<>::max() >= 5);
    barrier b{2};
    atomic<int> atom{0};
    thread t1{[&] {
        for (int i = 0; i < 5; ++i) {
            auto token = b.arrive();
            b.wait(move(token));
            atom.fetch_add(1, memory_order_relaxed);
        }
    }};
    thread t2{[&] {
        for (int i = 0; i < 3; ++i) {
            b.arrive_and_wait();
            atom.fetch_add(1, memory_order_relaxed);
        }
        b.arrive_and_drop();
    }};
    t1.join();
    t2.join();
    assert(atom.load(memory_order_relaxed) == 8);
}

void test_bit() {
    using namespace std;
    puts("Testing <bit>.");
    assert(popcount(0x1234ABCDu) == 15);
    static_assert(popcount(0x1234ABCDu) == 15);
}

void test_bitset() {
    using namespace std;
    puts("Testing <bitset>.");
    constexpr bitset<32> b{0x1234ABCDu};
    assert(b[3] && b[2] && !b[1] && b[0]);
    static_assert(b[3] && b[2] && !b[1] && b[0]);
}

void test_charconv() {
    using namespace std;
    puts("Testing <charconv>.");
    char buf[4]{};
    const to_chars_result result = to_chars(buf, end(buf), 3.14);
    assert(result.ec == errc{});
    assert(result.ptr == end(buf));
    const string_view sv{buf, size(buf)};
    assert(sv == "3.14");
}

void test_chrono() {
    using namespace std;
    puts("Testing <chrono>.");
    constexpr chrono::seconds dur = 3min;
    assert(dur.count() == 180);
    static_assert(dur.count() == 180);
}

#pragma warning(push)
#pragma warning(disable : 4996) // was declared deprecated
void test_codecvt() {
    using namespace std;
    puts("Testing <codecvt>.");
    const string utf8_koshka_cat{"\xD0\xBA\xD0\xBE\xD1\x88\xD0\xBA\xD0\xB0_\xF0\x9F\x90\x88"};
    const wstring utf16_koshka_cat{L"\x043A\x043E\x0448\x043A\x0430_\xD83D\xDC08"};
    wstring_convert<codecvt_utf8_utf16<wchar_t>> conv;
    assert(conv.from_bytes(utf8_koshka_cat) == utf16_koshka_cat);
    assert(conv.to_bytes(utf16_koshka_cat) == utf8_koshka_cat);

    static_assert(static_cast<int>(codecvt_mode::consume_header) == 4);
}
#pragma warning(pop)

void test_compare() {
    using namespace std;
    puts("Testing <compare>.");
    assert(is_lt(10 <=> 20));
    static_assert(is_lt(10 <=> 20));
}

void test_complex() {
    using namespace std;
    puts("Testing <complex>.");
    constexpr complex<double> c{3.0, 4.0};
    assert(norm(c) == 25.0);
    static_assert(norm(c) == 25.0);
}

void test_concepts() {
    using namespace std;
    puts("Testing <concepts>.");
    static_assert(signed_integral<short>);
    static_assert(!signed_integral<unsigned short>);
    static_assert(!signed_integral<double>);
}

void test_condition_variable() {
    using namespace std;
    puts("Testing <condition_variable>.");
    condition_variable cv;
    mutex mut;
    vector<int> vec = {5};

    thread odd{[&cv, &mut, &vec] {
        unique_lock<mutex> lk{mut};

        while (vec.size() < 6) {
            cv.wait(lk, [&vec] { return vec.size() % 2 == 1; });
            const int n = vec.back();
            vec.push_back(n * 10 + 1);
            cv.notify_one();
        }
    }};

    thread even{[&cv, &mut, &vec] {
        unique_lock<mutex> lk{mut};

        while (vec.size() < 7) {
            cv.wait(lk, [&vec] { return vec.size() % 2 == 0; });
            const int n = vec.back();
            vec.push_back(n * 10 + 2);
            cv.notify_one();
        }
    }};

    odd.join();
    even.join();

    const vector<int> expected_val = {5, 51, 512, 5121, 51212, 512121, 5121212};
    assert(vec == expected_val);

    static_assert(static_cast<int>(cv_status::no_timeout) == 0);
    static_assert(static_cast<int>(cv_status::timeout) == 1);
}

void test_coroutine() {
    using namespace std;
    puts("Testing <coroutine>.");
    constexpr coroutine_handle<> handle{};
    assert(handle.address() == nullptr);
    static_assert(handle.address() == nullptr);
}

void test_deque() {
    using namespace std;
    puts("Testing <deque>.");
    const deque<int> d{10, 20, 30, 40, 50};
    assert(d[2] == 30);
}

void test_exception() {
    using namespace std;
    puts("Testing <exception>.");
    assert(uncaught_exceptions() == 0);
    const exception_ptr ep = current_exception();
    assert(!ep);
}

void test_execution() {
    using namespace std;
    puts("Testing <execution>.");
    constexpr int arr[]{11, 0, 22, 0, 33, 0, 44, 0, 55};
    assert(count(execution::par, begin(arr), end(arr), 0) == 4);
}

void test_expected() {
    using namespace std;
    puts("Testing <expected>.");
    constexpr expected<double, int> test{unexpect, 42};
    assert(test.error() == 42);
}

void test_filesystem() {
    using namespace std;
    puts("Testing <filesystem>.");
    constexpr wstring_view dot{L"."};
    error_code ec{};
    const filesystem::space_info info = filesystem::space(dot, ec);
    assert(!ec);
    assert(info.capacity > 0);
    assert(info.capacity != static_cast<decltype(info.capacity)>(-1));
}

void test_format() {
    using namespace std;
    puts("Testing <format>.");
    assert(format("{} {}", 1729, "kittens") == "1729 kittens");
    assert(format(L"{} {}", 1729, L"kittens") == L"1729 kittens");
}

void test_forward_list() {
    using namespace std;
    puts("Testing <forward_list>.");
    const forward_list<int> fl{10, 20, 30, 40, 50};
    assert(*next(fl.begin(), 2) == 30);
}

void test_fstream() {
    using namespace std;
    puts("Testing <fstream>.");
    const ifstream f{};
    assert(!f.is_open());
}

void test_functional() {
    using namespace std;
    puts("Testing <functional>.");
    function<int(int, int)> f{multiplies{}};
    assert(f(3, 5) == 15);
    f = [](int x, int y) { return x * 100 + y * 10; };
    assert(f(3, 5) == 350);
    constexpr auto b = bind(multiplies{}, placeholders::_1, 11);
    assert(b(3) == 33);
    static_assert(b(3) == 33);
}

void test_future() {
    using namespace std;
    puts("Testing <future>.");
    promise<int> p{};
    future<int> f{p.get_future()};
    assert(f.wait_for(0s) == future_status::timeout);
    p.set_value(1729);
    assert(f.wait_for(0s) == future_status::ready);
    assert(f.get() == 1729);
}

void test_initializer_list() {
    using namespace std;
    puts("Testing <initializer_list>.");
    const initializer_list<int> il{10, 20, 30, 40, 50};
    assert(il.begin()[2] == 30);
}

void test_iomanip() {
    using namespace std;
    puts("Testing <iomanip>.");
    ostringstream oss;
    oss << "I have " << setfill('.') << setw(7) << 9 * 9 * 9 + 10 * 10 * 10 << " cute fluffy kittens.";
    assert(oss.str() == "I have ...1729 cute fluffy kittens.");
    oss.str("");
    oss << quoted(R"(Read "C:\Temp\Cat Names.txt" for more info.)");
    const char* const expected_quoted = R"("Read \"C:\\Temp\\Cat Names.txt\" for more info.")";
    assert(oss.str() == expected_quoted);
}

void test_ios() {
    using namespace std;
    puts("Testing <ios>.");
    ios b{nullptr};
    assert(b.rdbuf() == nullptr);
    assert(b.rdstate() == ios_base::badbit);
    assert(b.precision() == 6);
    static_assert(ios_base::floatfield == (ios_base::fixed | ios_base::scientific));
}

void test_iosfwd() {
    using namespace std;
    puts("Testing <iosfwd>.");
    static_assert(is_same_v<istream, basic_istream<char>>);
    static_assert(is_same_v<wostream, basic_ostream<wchar_t>>);
}

void test_iostream() {
    using namespace std;
    puts("Testing <iostream>.");
    cout << "Testing cout.\n";
    assert(cin.tie() == &cout);
}

void test_istream() {
    using namespace std;
    puts("Testing <istream>.");
    const istream is{nullptr};
    assert(is.gcount() == 0);
}

void test_iterator() {
    using namespace std;
    puts("Testing <iterator>.");
    static constexpr int arr[]{10, 20, 30, 40, 50};
    constexpr reverse_iterator<const int*> ri{end(arr)};
    assert(*ri == 50);
    assert(*next(ri) == 40);
    static_assert(*ri == 50);
    static_assert(*next(ri) == 40);
}

void test_latch() {
    using namespace std;
    puts("Testing <latch>.");
    static_assert(latch::max() >= 5);
    for (const auto& release_wait : {true, false}) {
        latch l{5};
        thread t1{[&] { l.wait(); }};
        thread t2{[&] { l.arrive_and_wait(2); }};
        l.count_down();
        if (release_wait) {
            l.arrive_and_wait(2);
        } else {
            l.count_down(2);
        }
        t1.join();
        t2.join();
    }
}

void test_limits() {
    using namespace std;
    puts("Testing <limits>.");
    static_assert(numeric_limits<short>::min() == -32768);
    static_assert(numeric_limits<short>::max() == 32767);
}

void test_list() {
    using namespace std;
    puts("Testing <list>.");
    const list<int> l{10, 20, 30, 40, 50};
    assert(*next(l.begin(), 2) == 30);
}

void test_locale() {
    using namespace std;
    puts("Testing <locale>.");
    locale loc{};
    assert(isdigit('1', loc));
    assert(!isdigit('a', loc));
    using L = locale;
    static_assert((L::collate | L::ctype | L::monetary | L::numeric | L::time | L::messages | L::all) == L::all);
}

void test_map() {
    using namespace std;
    puts("Testing <map>.");
    map<int, int> m{{10, 11}, {20, 22}, {30, 33}, {40, 44}, {50, 55}};
    assert(m[30] == 33);
}

void test_memory() {
    using namespace std;
    puts("Testing <memory>.");
    unique_ptr<int> up = make_unique<int>(1729);
    assert(*up == 1729);
    shared_ptr<int> sp{move(up)};
    assert(!up);
    assert(*sp == 1729);
    weak_ptr<const int> wp{sp};
    assert(!wp.expired());
    shared_ptr<const int> other = wp.lock();
    assert(*other == 1729);
    ++*sp;
    assert(*other == 1730);
}

void test_memory_resource() {
    using namespace std;
    puts("Testing <memory_resource>.");
    pmr::monotonic_buffer_resource mbr{};
    pmr::polymorphic_allocator<int> al{&mbr};
    pmr::vector<int> v({10, 20, 30, 40, 50}, al);
    assert(v[2] == 30);
    v.push_back(60);
    assert(v[5] == 60);
}

void test_mutex() {
    using namespace std;
    puts("Testing <mutex>.");
    // see above, tested with <condition_variable>
    int n       = 10;
    auto lambda = [&n] { ++n; };
    once_flag flag;
    assert(n == 10);
    call_once(flag, lambda);
    assert(n == 11);
    call_once(flag, lambda);
    assert(n == 11);
}

void test_new() {
    using namespace std;
    puts("Testing <new>.");
    bool caught_bad_alloc = false;

    try {
        throw bad_array_new_length{};
    } catch (const bad_alloc&) {
        caught_bad_alloc = true;
    } catch (...) {
        assert(false);
    }

    assert(caught_bad_alloc);

    const int* const ptr = new (nothrow) int{1729};
    assert(ptr);
    assert(*ptr == 1729);
    delete ptr;

    static_assert(hardware_constructive_interference_size == 64); // implementation-defined
    static_assert(hardware_destructive_interference_size == 64); // implementation-defined
}

void test_numbers() {
    using namespace std;
    puts("Testing <numbers>.");
    static_assert(3.14 < numbers::pi && numbers::pi < 3.15);
    static_assert(2.71828f < numbers::e_v<float> && numbers::e_v<float> < 2.71829f);
}

void test_numeric() {
    using namespace std;
    puts("Testing <numeric>.");
    constexpr int arr[]{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    assert(accumulate(begin(arr), end(arr), 0) == 55);
    static_assert(accumulate(begin(arr), end(arr), 0) == 55);
}

void test_optional() {
    using namespace std;
    puts("Testing <optional>.");
    constexpr optional<int> opt{in_place, 1729};
    assert(opt.has_value());
    assert(opt.value() == 1729);
    static_assert(opt.has_value());
    static_assert(opt.value() == 1729);

    constexpr optional<int> empty{nullopt};
    assert(!empty.has_value());
    assert(empty.value_or(-1) == -1);
    static_assert(!empty.has_value());
    static_assert(empty.value_or(-1) == -1);
}

void test_ostream() {
    using namespace std;
    puts("Testing <ostream>.");
    const ostream os{nullptr};
    assert(os.rdbuf() == nullptr);
}

void test_print() {
    using namespace std;
    puts("Testing <print>.");
    println("Hello, world!");

#ifdef _CPPRTTI
    println(cout, "The answer to life, the universe, and everything: {}", 42);
#endif // _CPPRTTI
}

void test_queue() {
    using namespace std;
    puts("Testing <queue>.");
    queue<int> q;
    q.push(10);
    q.push(20);
    q.push(30);
    assert(q.size() == 3);
    assert(q.front() == 10);
    q.pop();
    assert(q.front() == 20);
    q.pop();
    assert(q.front() == 30);
    q.pop();
    assert(q.empty());

    priority_queue<int> pq;
    pq.push(50);
    pq.push(10);
    pq.push(1729);
    pq.push(200);
    assert(pq.size() == 4);
    assert(pq.top() == 1729);
    pq.pop();
    assert(pq.top() == 200);
    pq.pop();
    assert(pq.top() == 50);
    pq.pop();
    assert(pq.top() == 10);
    pq.pop();
    assert(pq.empty());
}

void test_random() {
    using namespace std;
    puts("Testing <random>.");
    minstd_rand0 lcg;
    lcg.discard(9999);
    assert(lcg() == 1043618065); // N4868 [rand.predef]/1
}

void test_ranges() {
    using namespace std;
    puts("Testing <ranges>.");
    constexpr int arr[]{11, 0, 22, 0, 33, 0, 44, 0, 55};
    assert(ranges::distance(views::filter(arr, [](int x) { return x == 0; })) == 4);
    static_assert(ranges::distance(views::filter(arr, [](int x) { return x != 0; })) == 5);
}

void test_ratio() {
    using namespace std;
    puts("Testing <ratio>.");
    static_assert(ratio_equal_v<ratio_multiply<milli, hecto>, deci>);
}

void test_regex() {
    using namespace std;
    puts("Testing <regex>.");
    const regex r{R"(\w+)"};
    const string s{"cute! fluffy? kittens."};
    vector<string> v;

    for (sregex_token_iterator it{s.begin(), s.end(), r}, end; it != end; ++it) {
        v.push_back(it->str());
    }

    const vector<string> expected_val{"cute", "fluffy", "kittens"};
    assert(v == expected_val);
}

void test_scoped_allocator() {
    using namespace std;
    puts("Testing <scoped_allocator>.");
    vector<int, scoped_allocator_adaptor<allocator<int>>> v;
    v.push_back(11);
    v.push_back(22);
    v.push_back(33);
    constexpr int expected_val[]{11, 22, 33};
    assert(equal(v.begin(), v.end(), begin(expected_val), end(expected_val)));
}

void test_semaphore() {
    using namespace std;
    puts("Testing <semaphore>.");
    static_assert(binary_semaphore::max() >= 1);
    binary_semaphore s{1};
    int val{0};
    s.acquire();
    thread t1{[&] {
        for (int i = 0; i < 17; ++i) {
            s.acquire();
            val += 100;
            s.release();
        }
    }};
    thread t2{[&] {
        for (int i = 0; i < 29; ++i) {
            s.acquire();
            ++val;
            s.release();
        }
    }};
    s.release();
    t1.join();
    t2.join();
    assert(val == 1729);
}

void test_set() {
    using namespace std;
    puts("Testing <set>.");
    const set<int> s{10, 20, 30, 40, 50};
    assert(*next(s.begin(), 2) == 30);

    const multiset<int> ms{10, 20, 20, 30, 30, 30, 40, 40, 40, 40};
    const auto p = ms.equal_range(30);
    assert(distance(p.first, p.second) == 3);
}

void test_shared_mutex() {
    using namespace std;
    puts("Testing <shared_mutex>.");
    condition_variable_any cv;
    shared_mutex mut;
    vector<int> vec = {5};

    thread odd{[&cv, &mut, &vec] {
        unique_lock<shared_mutex> lk{mut};

        while (vec.size() < 6) {
            cv.wait(lk, [&vec] { return vec.size() % 2 == 1; });
            const int n = vec.back();
            vec.push_back(n * 10 + 1);
            cv.notify_one();
        }
    }};

    thread even{[&cv, &mut, &vec] {
        unique_lock<shared_mutex> lk{mut};

        while (vec.size() < 7) {
            cv.wait(lk, [&vec] { return vec.size() % 2 == 0; });
            const int n = vec.back();
            vec.push_back(n * 10 + 2);
            cv.notify_one();
        }
    }};

    odd.join();
    even.join();

    const vector<int> expected_val = {5, 51, 512, 5121, 51212, 512121, 5121212};
    assert(vec == expected_val);
}

constexpr bool impl_test_source_location() {
    using namespace std;
    const auto sl = source_location::current();
    assert(sl.line() == __LINE__ - 1);
    assert(sl.column() == 38);
#if defined(__clang__) || defined(__EDG__) // TRANSITION, DevCom-10199227 and LLVM-58951
    assert(sl.function_name() == "impl_test_source_location"sv);
#else // ^^^ workaround / no workaround vvv
    assert(sl.function_name() == "bool __cdecl impl_test_source_location(void)"sv);
#endif // TRANSITION, DevCom-10199227 and LLVM-58951
    assert(string_view{sl.file_name()}.ends_with("test_header_units_and_modules.hpp"sv));
    return true;
}

void test_source_location() {
    using namespace std;
    puts("Testing <source_location>.");
    assert(impl_test_source_location());
    static_assert(impl_test_source_location());
}

void test_span() {
    using namespace std;
    puts("Testing <span>.");
    static constexpr int arr[]{11, 22, 33, 44, 55};
    constexpr span<const int, 5> whole{arr};
    constexpr span<const int, 3> mid = whole.subspan<1, 3>();
    assert(mid[0] == 22 && mid[1] == 33 && mid[2] == 44);
    static_assert(mid[0] == 22 && mid[1] == 33 && mid[2] == 44);
}

void test_spanstream() {
    using namespace std;
    puts("Testing <spanstream>.");
    char ibuffer[] = "1 2 3 4 5";
    ispanstream is{span<char>{ibuffer}};
    int read = 0;
    for (int expected_val = 1; expected_val <= 5; ++expected_val) {
        assert(is.good());
        is >> read;
        assert(read == expected_val);
    }

    const char const_buffer[] = "1 2 3 4 5";
    basic_ispanstream<char> is_const_buffer{span<const char>{const_buffer}};
    read = 0;
    for (int expected_val = 1; expected_val <= 5; ++expected_val) {
        assert(is_const_buffer.good());
        is_const_buffer >> read;
        assert(read == expected_val);
    }

    const auto expected_val = "102030"sv;
    char obuffer[10];
    ospanstream os{span<char>{obuffer}};
    os << 10 << 20 << 30;
    assert(equal(begin(os.span()), end(os.span()), begin(expected_val), end(expected_val)));

    char buffer[10];
    spanstream s{span<char>{buffer}};
    s << 10 << 20 << 30;
    assert(equal(begin(s.span()), end(s.span()), begin(expected_val), end(expected_val)));
}

void test_sstream() {
    using namespace std;
    puts("Testing <sstream>.");
    ostringstream oss;
    oss << "I have " << 9 * 9 * 9 + 10 * 10 * 10 << " cute fluffy kittens.";
    assert(oss.str() == "I have 1729 cute fluffy kittens.");
}

void test_stack() {
    using namespace std;
    puts("Testing <stack>.");
    stack<int> s;
    s.push(10);
    s.push(20);
    s.push(30);
    assert(s.size() == 3);
    assert(s.top() == 30);
    s.pop();
    assert(s.top() == 20);
    s.pop();
    assert(s.top() == 10);
    s.pop();
    assert(s.empty());
}

__declspec(dllexport) void test_stacktrace() { // export test_stacktrace to have it named even without debug info
    using namespace std;
    puts("Testing <stacktrace>.");
    auto desc = stacktrace::current().at(0).description();

    if (auto pos = desc.find("!"); pos != string::npos) {
        desc = desc.substr(pos + 1);
    }

    if (auto pos = desc.find("+"); pos != string::npos) {
        desc.resize(pos);
    }

    assert(desc == "test_stacktrace");
}

void test_stdexcept() {
    using namespace std;
    puts("Testing <stdexcept>.");
    bool caught_puppies = false;

    try {
        throw overflow_error{"too many puppies"};
    } catch (const runtime_error& e) {
        caught_puppies = e.what() == "too many puppies"sv;
    } catch (...) {
        assert(false);
    }

    assert(caught_puppies);
}

void test_stdfloat() {
    using namespace std;
    puts("Testing <stdfloat>.");
    // `namespace std` is available, so we're done.
}

void test_stop_token() {
    using namespace std;
    puts("Testing <stop_token>.");
    vector<int> vec;
    {
        latch l{1};
        jthread jt{[&](const stop_token& token) {
            int val{1729};
            // Generate the Collatz sequence for 1729.
            // test_stop_token() shouldn't ask us to stop early; if it does, the sequence will be truncated.
            while (!token.stop_requested()) {
                vec.push_back(val);
                if (val == 1) {
                    break;
                } else if (val % 2 == 0) {
                    val /= 2;
                } else {
                    val = 3 * val + 1;
                }
            }
            l.count_down(); // tell test_stop_token() that we're done
            while (!token.stop_requested()) {
                this_thread::sleep_for(10ms); // not a timing assumption; avoids spinning furiously
            }
            vec.push_back(-1000); // indicate that token.stop_requested() returned true
        }};
        l.wait(); // wait for jt to generate the sequence
    } // destroying jt will ask it to stop
    static constexpr int expected_val[]{1729, 5188, 2594, 1297, 3892, 1946, 973, 2920, 1460, 730, 365, 1096, 548, 274,
        137, 412, 206, 103, 310, 155, 466, 233, 700, 350, 175, 526, 263, 790, 395, 1186, 593, 1780, 890, 445, 1336, 668,
        334, 167, 502, 251, 754, 377, 1132, 566, 283, 850, 425, 1276, 638, 319, 958, 479, 1438, 719, 2158, 1079, 3238,
        1619, 4858, 2429, 7288, 3644, 1822, 911, 2734, 1367, 4102, 2051, 6154, 3077, 9232, 4616, 2308, 1154, 577, 1732,
        866, 433, 1300, 650, 325, 976, 488, 244, 122, 61, 184, 92, 46, 23, 70, 35, 106, 53, 160, 80, 40, 20, 10, 5, 16,
        8, 4, 2, 1, -1000};
    assert(equal(vec.begin(), vec.end(), begin(expected_val), end(expected_val)));
}

void test_streambuf() {
    using namespace std;
    puts("Testing <streambuf>.");
    istringstream iss{"kittens"};
    assert(iss.rdbuf()->in_avail() == 7);
}

void test_string() {
    using namespace std;
    puts("Testing <string>.");
    const string small_string{"homeowner"};
    const string large_string{"Cute fluffy kittens are so adorable when they meow and purr."};
    assert(small_string.find("meow") == 2);
    assert(large_string.find("meow") == 46);
}

void test_string_view() {
    using namespace std;
    puts("Testing <string_view>.");
    constexpr string_view catenary{"catenary"};
    assert(catenary.starts_with("cat"));
    assert(!catenary.starts_with("dog"));
    static_assert(catenary.starts_with("cat"));
    static_assert(!catenary.starts_with("dog"));
}

#pragma warning(push)
#pragma warning(disable : 4996) // was declared deprecated
void test_strstream() {
    using namespace std;
    puts("Testing <strstream>.");
    istrstream istr{"1729"};
    int n = -1;
    istr >> n;
    assert(n == 1729);
}
#pragma warning(pop)

void test_syncstream() {
    using namespace std;
    puts("Testing <syncstream>.");
    syncbuf sync_buf{nullptr};
    assert(sync_buf.get_wrapped() == nullptr);
    assert(sync_buf.get_allocator() == allocator<char>{});
    assert(sync_buf.emit() == false);
    osyncstream sync_str{cout};
    sync_str << "Testing osyncstream.\n";
    assert(sync_str.rdbuf()->get_wrapped() == cout.rdbuf());
}

void test_system_error() {
    using namespace std;
    puts("Testing <system_error>.");
    const error_code code = make_error_code(errc::value_too_large);
    assert(code.value() == static_cast<int>(errc::value_too_large));
    assert(code.category() == generic_category());
    assert(code);
}

void test_thread() {
    using namespace std;
    puts("Testing <thread>.");
    // see above, tested with <condition_variable>
    assert(this_thread::get_id() != thread::id{});
}

void test_tuple() {
    using namespace std;
    puts("Testing <tuple>.");
    constexpr tuple<int, char, double> t{1729, 'c', 1.25};
    assert(get<int>(t) == 1729);
    assert(get<char>(t) == 'c');
    assert(get<double>(t) == 1.25);
    static_assert(get<int>(t) == 1729);
    static_assert(get<char>(t) == 'c');
    static_assert(get<double>(t) == 1.25);
}

void test_type_traits() {
    using namespace std;
    puts("Testing <type_traits>.");
    static_assert(is_void_v<void>);
    static_assert(!is_void_v<double>);
    static_assert(is_same_v<remove_extent_t<int[10][20][30]>, int[20][30]>);
    static_assert(is_same_v<remove_extent_t<double>, double>);

    constexpr auto compiletime_10_or_runtime_20 = [] {
        if (is_constant_evaluated()) {
            return 10;
        } else {
            return 20;
        }
    };

    assert(compiletime_10_or_runtime_20() == 20);
    static_assert(compiletime_10_or_runtime_20() == 10);
}

void test_typeindex() {
    using namespace std;
    puts("Testing <typeindex>.");
    const type_index ti_int{typeid(int)};
    type_index ti{typeid(double)};
    assert(ti != ti_int);
    ti = ti_int;
    assert(ti == ti_int);
}

void test_typeinfo() {
    using namespace std;
    puts("Testing <typeinfo>.");
    const type_info& t1 = typeid(int);
    const type_info& t2 = typeid(const int&);
    const type_info& t3 = typeid(double);
    assert(t1 == t2);
    assert(t1 != t3);

    assert(typeid(double).name() == "double"sv); // also test DevCom-10349749
}

void test_unordered_map() {
    using namespace std;
    puts("Testing <unordered_map>.");
    unordered_map<int, int> um{{1, 1}, {2, 4}, {3, 9}, {4, 16}, {5, 25}};
    for (const auto& p : um) {
        assert(p.first * p.first == p.second);
    }
}

void test_unordered_set() {
    using namespace std;
    puts("Testing <unordered_set>.");
    unordered_set<int> us{10, 20, 30, 40, 50};
    for (const auto& elem : us) {
        assert(elem % 10 == 0);
    }
}

void test_utility() {
    using namespace std;
    puts("Testing <utility>.");
    constexpr pair<int, double> p{44, 5.5};
    assert(p.first == 44);
    assert(p.second == 5.5);
    static_assert(p.first == 44);
    static_assert(p.second == 5.5);

    assert(in_range<unsigned char>(200));
    assert(!in_range<unsigned char>(300));
    static_assert(in_range<unsigned char>(200));
    static_assert(!in_range<unsigned char>(300));

    static_assert(is_same_v<make_integer_sequence<int, 4>, integer_sequence<int, 0, 1, 2, 3>>);
    static_assert(is_same_v<decltype(declval<short>() + declval<unsigned short>()), int>);
}

void test_valarray() {
    using namespace std;
    puts("Testing <valarray>.");
    valarray<int> val{1, 2, 3};
    val *= 10;
    assert(val[0] == 10 && val[1] == 20 && val[2] == 30);
}

void test_variant() {
    using namespace std;
    puts("Testing <variant>.");
    constexpr const char* cats = "CATS";
    constexpr variant<int, const char*, double> var{in_place_type<const char*>, cats};
    static_assert(var.index() == 1);
    static_assert(holds_alternative<const char*>(var));
    static_assert(get<const char*>(var) == cats);
    assert(var.index() == 1);
    assert(holds_alternative<const char*>(var));
    assert(get<const char*>(var) == cats);

    constexpr variant<short, float, double> var2{in_place_type<double>, 2.5};
    assert(var2.index() == 2);
    assert(holds_alternative<double>(var2));
    assert(get<double>(var2) == 2.5);
    static_assert(var2.index() == 2);
    static_assert(holds_alternative<double>(var2));
    static_assert(get<double>(var2) == 2.5);
}

void test_vector() {
    using namespace std;
    puts("Testing <vector>.");
    const vector<int> v{10, 20, 30, 40, 50};
    assert(v[2] == 30);

    const vector<bool> vb{true, true, false, true};
    assert(vb[0] && vb[1] && !vb[2] && vb[3]);
}

void test_version() {
    using namespace std;
#ifdef TEST_HEADER_UNITS
    puts("Testing <version>.");
    static_assert(__cpp_lib_make_unique >= 201304L);
#else // ^^^ header units / named modules vvv
    puts("Nothing to test in <version>, only macros.");
#endif // ^^^ named modules ^^^
}

// VSO-1593165 "Standard Library Modules: time_put<wchar_t> emits bogus error LNK2019: unresolved external symbol"
void test_VSO_1593165() {
    using namespace std;
    puts("Testing VSO-1593165.");

    // Originally from the Dev11_0494593_time_put_wchar_t test:
    using Facet = time_put<wchar_t, wstring::iterator>;

    const tm t = [] {
        tm ret{};

        ret.tm_sec   = 57;
        ret.tm_min   = 42;
        ret.tm_hour  = 20;
        ret.tm_mday  = 28;
        ret.tm_mon   = 3;
        ret.tm_year  = 108;
        ret.tm_wday  = 1;
        ret.tm_yday  = 118;
        ret.tm_isdst = 0;

        return ret;
    }();

    const locale l;
    wstring s(15, L'x');
    wstringstream stream;
    const wchar_t fill          = L' ';
    const wchar_t pattern[]     = L"%Y.%m.%d";
    const wstring::iterator ret = use_facet<Facet>(l).put(s.begin(), stream, fill, &t, begin(pattern), end(pattern));
    assert(ret == s.begin() + 11);
    const wstring correct(L"2008.04.28\0xxxx", 15);
    assert(s == correct);
}

void all_cpp_header_tests() {
    test_algorithm();
    test_any();
    test_array();
    test_atomic();
    test_barrier();
    test_bit();
    test_bitset();
    test_charconv();
    test_chrono();
    test_codecvt();
    test_compare();
    test_complex();
    test_concepts();
    test_condition_variable();
    test_coroutine();
    test_deque();
    test_exception();
    test_execution();
    test_expected();
    test_filesystem();
    test_format();
    test_forward_list();
    test_fstream();
    test_functional();
    test_future();
    test_initializer_list();
    test_iomanip();
    test_ios();
    test_iosfwd();
    test_iostream();
    test_istream();
    test_iterator();
    test_latch();
    test_limits();
    test_list();
    test_locale();
    test_map();
    test_memory();
    test_memory_resource();
    test_mutex();
    test_new();
    test_numbers();
    test_numeric();
    test_optional();
    test_ostream();
    test_print();
    test_queue();
    test_random();
    test_ranges();
    test_ratio();
    test_regex();
    test_scoped_allocator();
    test_semaphore();
    test_set();
    test_shared_mutex();
    test_source_location();
    test_span();
    test_spanstream();
    test_sstream();
    test_stack();
    test_stacktrace();
    test_stdexcept();
    test_stdfloat();
    test_stop_token();
    test_streambuf();
    test_string();
    test_string_view();
    test_strstream();
    test_syncstream();
    test_system_error();
    test_thread();
    test_tuple();
    test_type_traits();
    test_typeindex();
    test_typeinfo();
    test_unordered_map();
    test_unordered_set();
    test_utility();
    test_valarray();
    test_variant();
    test_vector();
    test_version();

    test_VSO_1593165();
}
