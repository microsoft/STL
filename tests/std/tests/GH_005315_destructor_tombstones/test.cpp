// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// env.lst defines _MSVC_STL_DESTRUCTOR_TOMBSTONES to 1.

#ifdef _M_CEE // work around a sporadic hang in /clr configurations
int main() {}
#else // ^^^ workaround / no workaround vvv

#include <array>
#include <cstdlib>
#include <deque>
#include <exception>
#include <forward_list>
#include <functional>
#include <iterator>
#include <list>
#include <memory>
#include <new>
#include <regex>
#include <set>
#include <string>
#include <unordered_set>
#include <utility>
#include <valarray>
#include <vector>

#if _HAS_CXX17
#include <any>
#include <memory_resource>
#include <optional>
#include <variant>
#endif

#include <test_death.hpp>

using namespace std;

// A good test is one that consistently gets away with UB when destructor tombstones are disabled,
// but reliably terminates when destructor tombstones are enabled.
// A great test is when not even ASan can detect the UB when destructor tombstones are disabled.
// (Note that it may not always be possible to write a great test. For example, when
// dynamically allocated sentinel nodes are involved, ASan is extremely good at detecting UB.)

template <class T, class Func, class... Args>
void call_on_destroyed_object(Func func, Args&&... args) {
    alignas(T) unsigned char storage[sizeof(T)]{};
    T& t = *::new (static_cast<void*>(storage)) T(forward<Args>(args)...);
    t.~T();
    func(t); // obviously undefined behavior
}

void test_vector() {
    call_on_destroyed_object<vector<double>>([](auto& v) { v.push_back(17.29); });
}
void test_vector_bool() {
    call_on_destroyed_object<vector<bool>>([](auto& vb) { vb.push_back(true); });
}
void test_deque() {
    call_on_destroyed_object<deque<double>>([](auto& d) { d.shrink_to_fit(); });
}
void test_list() {
    call_on_destroyed_object<list<double>>([](auto& l) { (void) (l.begin() == l.end()); });
}
void test_forward_list() {
    call_on_destroyed_object<forward_list<double>>([](auto& fl) { (void) distance(fl.begin(), fl.end()); });
}
void test_set() {
    call_on_destroyed_object<set<double>>([](auto& s) { (void) (s.begin() == s.end()); });
}
void test_unordered_set() {
    call_on_destroyed_object<unordered_set<double>>([](auto& us) { (void) (us.begin() == us.end()); });
}
void test_string() {
    call_on_destroyed_object<string>([](auto& str) { str[0] = '\0'; }); // 1-byte characters
}
void test_wstring() {
    call_on_destroyed_object<wstring>([](auto& wstr) { wstr[0] = L'\0'; }); // 2-byte characters
}
void test_u32string() {
    call_on_destroyed_object<u32string>([](auto& u32str) { u32str[0] = U'\0'; }); // 4-byte characters
}
void test_unique_ptr() {
    call_on_destroyed_object<unique_ptr<double>>([](auto& up) { up.reset(); });
}
void test_unique_ptr_array() {
    call_on_destroyed_object<unique_ptr<double[]>>([](auto& upa) { upa.reset(); });
}
void test_shared_ptr() {
    call_on_destroyed_object<shared_ptr<double>>([](auto& sp) { sp.reset(); });
}
void test_weak_ptr() {
    call_on_destroyed_object<weak_ptr<double>>([](auto& wp) { (void) wp.expired(); });
}
void test_exception_ptr() {
    call_on_destroyed_object<exception_ptr>([](auto& ep) { ep = nullptr; });
}
void test_function() {
    call_on_destroyed_object<function<int(int, int)>>([](auto& f) { f = nullptr; });
}
void test_regex() {
    call_on_destroyed_object<regex>([](auto& r) { (void) r.mark_count(); });
}
void test_valarray() {
    call_on_destroyed_object<valarray<double>>([](auto& va) { va.resize(10); });
}

#if _HAS_CXX17
void test_any() {
    call_on_destroyed_object<any>([](auto& a) { any other{move(a)}; });
}
void test_optional() {
    call_on_destroyed_object<optional<string>>([](auto& o) { o.value() = "woof"; }, "meow");
}
void test_variant() {
    call_on_destroyed_object<variant<double, string>>([](auto& var) { get<string>(var) = "woof"; }, "meow");
}
void test_polymorphic_allocator() {
    call_on_destroyed_object<pmr::polymorphic_allocator<double>>([](auto& pa) { (void) pa.allocate(1); });
}
#endif // _HAS_CXX17

#if _HAS_CXX23
void test_move_only_function() {
    call_on_destroyed_object<move_only_function<int(int, int)>>([](auto& mof) { mof = nullptr; });
}
#endif // _HAS_CXX23

int main(int argc, char* argv[]) {
    std_testing::death_test_executive exec;

    exec.add_death_tests({
        test_vector,
        test_vector_bool,
        test_deque,
        test_list,
        test_forward_list,
        test_set,
        test_unordered_set,
        test_string,
        test_wstring,
        test_u32string,
        test_unique_ptr,
        test_unique_ptr_array,
        test_shared_ptr,
        test_weak_ptr,
        test_exception_ptr,
        test_function,
        test_regex,
        test_valarray,

#if _HAS_CXX17
        test_any,
        test_optional,
        test_variant,
        test_polymorphic_allocator,
#endif // _HAS_CXX17

#if _HAS_CXX23
        test_move_only_function,
#endif // _HAS_CXX23
    });

    return exec.run(argc, argv);
}

#if _HAS_CXX20
// Verify that destructor tombstones don't interfere with constexpr.
constexpr bool test_constexpr() { // COMPILE-ONLY
    vector<double> v(10, 3.14);
    vector<bool> vb(10, true);
    string str{"cats"};
    wstring wstr{L"meow"};
    u32string u32str{U"purr"};
    optional<string> o{"hiss"};

#if _HAS_CXX23
    unique_ptr<double> up{new double{3.14}};
    unique_ptr<double[]> upa{new double[10]{}};
#endif // _HAS_CXX23

    return true;
}

static_assert(test_constexpr());
#endif // _HAS_CXX20

#endif // ^^^ no workaround ^^^
